#include <stdio.h>

#include <stdexcept>
#include <vector>
#include <string>

#include "data-loader.hpp"
#include "sphere.hpp"
#include "lexer.hpp"


namespace mn {

namespace solar {


static inline float toOmega(float period, float factor) {
	return std::fabs(period) > 0.00000001 ? factor / period : 0;
}


Sphere *loadData(const std::string &filename) {
	Lexer lexer(filename);
	if (!lexer) {
		fprintf(stderr, "%s: could not open\n", filename.c_str());
		return 0;
	}

	float factors[4] = { 1, 1, 1, 1 };

	enum State {
		S_FACTORS_READ_0 = 0,
		S_FACTORS_READ_1,
		S_FACTORS_READ_2,
		S_FACTORS_READ_3,
		S_START,
		S_SPHERE_READ_DIST,
		S_SPHERE_READ_SIZE,
		S_SPHERE_READ_YEAR,
		S_SPHERE_READ_DAY,
		S_SPHERE_READ_OPAREN,
		S_SPHERE_READ_R,
		S_SPHERE_READ_G,
		S_SPHERE_READ_B,
		S_SPHERE_READ_CPAREN,
		S_WAIT_OPEN,
		S_READ_TEXTURE,
		S_READ_TEXTURE_AND_WAIT_OPEN
	};
	unsigned state = S_START;

	std::vector<Sphere*> stack;

	int lights = 0;
	struct {
		std::string name;
		float data[8]; /* dist, size, year, day, pad, r, g, b */
	} sphere_desc;
	Sphere *sphere = 0;

	Lexer::Value value;
	Lexer::Location location;
	int token;


	for(;;) {
		token = lexer.nextToken(value, location);

		switch (state) {
		case S_START:
			switch (token) {
			case Lexer::T_FACTORS:
				sphere = 0;
				state = S_FACTORS_READ_0;
				break;

			case Lexer::T_STRING:
				if (stack.empty() && sphere) goto error;
				sphere_desc.name.assign(value.string);
				delete[] value.string;
				state = S_SPHERE_READ_DIST;
				break;

			case '}':
				switch (stack.size()) {
				case 0: goto error;
				case 1: return stack.back();
				}
				stack.pop_back();
				sphere = 0;
				break;

			case Lexer::T_LIGHT:
				if (!sphere || sphere->getLight() >= 0) goto error;
				sphere->setLight(++lights);
				break;

			default:
				goto s_wait_open;
			}
			break;

		s_wait_open:
		case S_WAIT_OPEN:
			switch (token) {
			case Lexer::T_EOF:
				return stack.empty() ? sphere : stack.back();

			case Lexer::T_LIGHT:
				if (!sphere || sphere->getLight() >= 0) goto error;
				sphere->setLight(++lights);
				break;

			case Lexer::T_TEXTURE:
				if (!sphere || sphere->texture) goto error;
				state = state == S_START ? S_READ_TEXTURE : S_READ_TEXTURE_AND_WAIT_OPEN;
				break;

			case '{':
				if (!sphere) goto error;
				stack.push_back(sphere);
				sphere = 0;
				state = S_START;
				break;

			default:
				goto error;
			}
			break;

		case S_READ_TEXTURE:
		case S_READ_TEXTURE_AND_WAIT_OPEN:
			if (token != Lexer::T_STRING) goto error;
			sphere->texture.load(value.string);
			delete[] value.string;
			state = state == S_READ_TEXTURE ? S_START : S_WAIT_OPEN;
			break;

		case S_FACTORS_READ_0:
		case S_FACTORS_READ_1:
		case S_FACTORS_READ_2:
		case S_FACTORS_READ_3:
			if (token != Lexer::T_REAL) goto error;
			factors[state - S_FACTORS_READ_0] = value.real;
			++state;
			break;

		case S_SPHERE_READ_DIST:
		case S_SPHERE_READ_SIZE:
		case S_SPHERE_READ_YEAR:
		case S_SPHERE_READ_DAY:
		case S_SPHERE_READ_R:
		case S_SPHERE_READ_G:
		case S_SPHERE_READ_B:
			if (token != Lexer::T_REAL) goto error;
			sphere_desc.data[state - S_SPHERE_READ_DIST] = value.real;
			++state;
			break;

		case S_SPHERE_READ_OPAREN:
			if (token != '(') goto error;
			++state;
			break;

		case S_SPHERE_READ_CPAREN:
			if (token != ')') goto error;
			sphere = new Sphere(sphere_desc.data[0] * factors[0],
			                    sphere_desc.data[1] * factors[1],
			                    toOmega(sphere_desc.data[2], factors[2]),
			                    toOmega(sphere_desc.data[3], factors[3]),
			                    gl::color(sphere_desc.data[5],
			                              sphere_desc.data[6],
			                              sphere_desc.data[7]),
			                    sphere_desc.name);
			if (stack.empty()) {
				state = S_WAIT_OPEN;
			} else {
				stack.back()->pushSatelite(*sphere);
				state = S_START;
			}
			break;
		}
	}




error:
	fprintf(stderr, "%s:%u:%u: unexpected token %s\n",
	        lexer.getFileName().c_str(),
	        location.begin.line, location.begin.column,
	        Lexer::tokenName(token));
	if (token == Lexer::T_STRING) {
		delete[] value.string;
	}
	if (stack.empty()) {
		delete sphere;
	} else {
		delete stack.front();
	}
	return 0;
}


}

}
