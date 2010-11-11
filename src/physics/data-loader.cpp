/*
 * src/physics/data-loader.cpp
 * Copyright 2010 by Michal Nazarewicz (mina86/AT/mina86/DOT/com)
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "../gl/abstract-scene.hpp"

#include <math.h>

#include <stdexcept>
#include <vector>
#include <string>

#include "../lib/lexer.hpp"

#include "object.hpp"
#include "scene.hpp"

namespace mn {

namespace gl {

const char AbstractScene::extension[] = "psy";

static void autoVelocity(physics::Object &object, const std::string &name) {
	const physics::Object *const o = object.find(name);
	if (!o) {
		return;
	}

	const physics::Object::Vector r = o->getPosition() - object.getPosition();
	const float l2 = r.length2();
	if (l2 < 0.01) {
		return;
	}

	const float V2 = physics::Object::G * o->getMass() / sqrtf(l2);
	object.getVelocity().normalize();
	object.setVelocity(object.getVelocity() * sqrt(V2));
}

AbstractScene::ptr AbstractScene::load(std::istream &in)
	throw(std::ios_base::failure, lib::Lexer::error) {
	static lib::Lexer::Keywords keywords;

	enum {
		T_EOF    = lib::Lexer::T_EOF,
		T_REAL   = lib::Lexer::T_REAL,
		T_STRING = lib::Lexer::T_STRING,
		T_AUTO   = lib::Lexer::T_FIRST_KEYWOARD,
		T_COLOR,
		T_FROZEN,
		T_LIGHT,
		T_MASS,
		T_SIZE,
		T_TEXTURE,
		T_VELOCITY
	};

	if (keywords.empty()) {
		keywords.insert(std::pair<std::string, int>("auto",     T_AUTO));
		keywords.insert(std::pair<std::string, int>("color",    T_COLOR));
		keywords.insert(std::pair<std::string, int>("colour",   T_COLOR));
		keywords.insert(std::pair<std::string, int>("frozen",   T_FROZEN));
		keywords.insert(std::pair<std::string, int>("light",    T_LIGHT));
		keywords.insert(std::pair<std::string, int>("mass",     T_MASS));
		keywords.insert(std::pair<std::string, int>("size",     T_SIZE));
		keywords.insert(std::pair<std::string, int>("texture",  T_TEXTURE));
		keywords.insert(std::pair<std::string, int>("vel",      T_VELOCITY));
		keywords.insert(std::pair<std::string, int>("velocity", T_VELOCITY));
	}

	lib::Lexer lexer(in, &keywords);

	enum State {
		S_START = 0,
		S_CONT,
		S_POSITION,         /* takes 3 floats */
		S_POSITION_READ_1,
		S_POSITION_READ_2,
		S_VELOCITY,         /* takes 3 floats */
		S_VELOCITY_READ_1,
		S_VELOCITY_READ_2,
		S_VELOCITY_DONE,
		S_COLOR,            /* takes 3 floats */
		S_COLOR_READ_1,
		S_COLOR_READ_2,
		S_FACTOR
	};
	unsigned state = S_START;
	physics::Object *object = NULL;

	lib::Lexer::Value value;
	lib::Lexer::Location location;
	unsigned token, lights = 0;
	float x = 0, y = 0;

	float massFactor = 1, sizeFactor = 1, distFactor = 1, velFactor = 1;

#define NEXT()       (token = lexer.nextToken(value, location))
#define ACCEPT(_tok) do { if (NEXT() != (_tok)) goto error; } while (0)

	for (;;) {
		NEXT();

		switch ((enum State)state) {
		case S_START:
			if (token == '*') {
				state = S_FACTOR;
				break;
			}
			if (token != T_STRING) {
				goto error;
			}
			goto s_cont_string;

		case S_VELOCITY_DONE:
			state = S_CONT;
			if (token == T_AUTO) {
				ACCEPT(T_STRING);
				autoVelocity(*object, value.string);
				break;
			}
			/* FALL THROUGH */

		case S_CONT:
			switch (token) {

			s_cont_string:
				state = S_CONT;
				/* FALL THROUGH */
			case T_STRING:
				object = new physics::Object(value.string, object);
				break;

			case '*'       : state = S_FACTOR  ; break;
			case '@'       : state = S_POSITION; break;
			case T_VELOCITY: state = S_VELOCITY; break;
			case T_COLOR   : state = S_COLOR   ; break;

			case 'x':
				ACCEPT(T_REAL);
				object->getPosition().x = value.real * distFactor;
				break;

			case 'y':
				ACCEPT(T_REAL);
				object->getPosition().y =value.real * distFactor;
				break;

			case 'z':
				ACCEPT(T_REAL);
				object->getPosition().z = value.real * distFactor;
				break;

			case T_FROZEN:
				if (object->isFrozen()) goto error;
				object->setFrozen(true);
				break;

			case T_LIGHT:
				if (object->getLight() >= 0) goto error;
				object->setLight(++lights);
				break;

			case T_SIZE:
				ACCEPT(T_REAL);
				object->setSize(value.real * sizeFactor);
				break;

			case T_MASS:
				ACCEPT(T_REAL);
				object->setMass(value.real * massFactor);
				break;

			case T_TEXTURE:
				if (object->texture) goto error;
				ACCEPT(T_STRING);
				object->texture.load(value.string);
				object->colorFromTexture();
				break;

			case T_EOF:
				return AbstractScene::ptr(new physics::Scene(object));

			default:
				goto error;
			}
			break;

		case S_VELOCITY:
		case S_POSITION:
		case S_COLOR:
			if (token != T_REAL) goto error;
			x = value.real;
			++state;
			break;

		case S_POSITION_READ_1:
		case S_VELOCITY_READ_1:
		case S_COLOR_READ_1:
			if (token != T_REAL) goto error;
			y = value.real;
			++state;
			break;

		case S_POSITION_READ_2:
			if (token != T_REAL) goto error;
			object->setPosition(x * distFactor, y * distFactor, value.real * distFactor);
			state = S_CONT;
			break;

		case S_VELOCITY_READ_2:
			if (token != T_REAL) goto error;
			object->setVelocity(x * velFactor, y * velFactor, value.real * velFactor);
			state = S_VELOCITY_DONE;
			break;

		case S_COLOR_READ_2:
			if (token != T_REAL) goto error;
			object->setColor(x, y, value.real);
			state = S_CONT;
			break;

		case S_FACTOR: {
			switch (token) {
			case T_STRING  : goto s_cont_string;
			case '@'       :
			case T_MASS    :
			case T_SIZE    :
			case T_VELOCITY: break;
			default: goto error;
			}
			int prevToken = token;
			ACCEPT(T_REAL);
			switch (prevToken) {
			case '@'       : distFactor = value.real; break;
			case T_MASS    : massFactor = value.real; break;
			case T_SIZE    : sizeFactor = value.real; break;
			case T_VELOCITY: velFactor  = value.real; break;
			}
		}
		}
	}

error:
	throw lib::Lexer::error(location, lib::Lexer::error::msg(lib::Lexer::tokenName(token, value), "unexpected token"));
}

}

}

