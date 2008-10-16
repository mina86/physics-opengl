#include <stdio.h>

#include <stdexcept>
#include <vector>
#include <string>

#include "data-loader.hpp"
#include "sphere.hpp"
#include "lexer.hpp"


namespace mn {

namespace solar {


Sphere *loadData(const std::string &filename) {
	Lexer lexer(filename);
	if (!lexer) {
		fprintf(stderr, "%s: could not open\n", filename.c_str());
		return 0;
	}

	float factors[3] = { 1, 1, 1 };

	enum State {
		S_FACTORS_READ_0 = 0,
		S_FACTORS_READ_1,
		S_FACTORS_READ_2,
		S_START,
		S_SPHERE_READ_DIST,
		S_SPHERE_READ_SIZE,
		S_SPHERE_READ_TIME,
		S_SPHERE_READ_OPAREN,
		S_SPHERE_READ_R,
		S_SPHERE_READ_G,
		S_SPHERE_READ_B,
		S_SPHERE_READ_CPAREN,
		S_WAIT_OPEN
	};
	unsigned state = S_START;

	std::vector<Sphere*> stack;

	int lights = 0;
	struct {
		std::string name;
		float data[7]; /* dist, size, time, pad, r, g, b */
		int light;
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
			case Lexer::T_EOF:
				return stack.empty() ? sphere : stack.back();

			case Lexer::T_FACTORS:
				sphere = 0;
				state = S_FACTORS_READ_0;
				break;

			case Lexer::T_STRING:
				if (stack.empty() && sphere) goto error;
				sphere_desc.name.assign(value.string);
				delete[] value.string;
				sphere_desc.light = -1;
				state = S_SPHERE_READ_DIST;
				break;

			case '{':
				if (!sphere) goto error;
				stack.push_back(sphere);
				sphere = 0;
				break;

			case '}':
				switch (stack.size()) {
				case 0: goto error;
				case 1: return stack.back();
				}
				stack.pop_back();
				sphere = 0;
				break;

			default:
				goto error;
			}
			break;

		case S_FACTORS_READ_0:
		case S_FACTORS_READ_1:
		case S_FACTORS_READ_2:
			if (token != Lexer::T_REAL) goto error;
			factors[state - S_FACTORS_READ_0] = value.real;
			++state;
			break;

		case S_SPHERE_READ_DIST:
		case S_SPHERE_READ_SIZE:
		case S_SPHERE_READ_TIME:
		case S_SPHERE_READ_R:
		case S_SPHERE_READ_G:
		case S_SPHERE_READ_B:
			if (token != Lexer::T_REAL) goto error;
			sphere_desc.data[state - S_SPHERE_READ_DIST] = value.real;
			++state;
			break;

		case S_SPHERE_READ_OPAREN:
			switch (token) {
			case Lexer::T_LIGHT:
				if (sphere_desc.light != -1) goto error;
				sphere_desc.light = lights;
				++lights;
				break;
			case '(':
				++state;
				break;
			default:
				goto error;
			}
			break;

		case S_SPHERE_READ_CPAREN:
			if (token != ')') goto error;
			sphere_desc.data[2] = std::fabs(sphere_desc.data[2]) > 0.01
				? factors[2] / sphere_desc.data[2] : 0;
			sphere = new Sphere(sphere_desc.data[0] * factors[0],
			                    sphere_desc.data[1] * factors[1],
			                    sphere_desc.data[2],
			                    gl::color(sphere_desc.data[4],
			                              sphere_desc.data[5],
			                              sphere_desc.data[6]),
			                    sphere_desc.name, sphere_desc.light);
			if (stack.empty()) {
				state = S_WAIT_OPEN;
			} else {
				stack.back()->pushSatelite(*sphere);
				state = S_START;
			}
			break;

		case S_WAIT_OPEN:
			if (token == Lexer::T_EOF) return sphere;
			if (token != '{') goto error;
			stack.push_back(sphere);
			state = S_START;
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
