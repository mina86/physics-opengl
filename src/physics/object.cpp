/*
 * src/physics/object.cpp
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
#include "object.hpp"

#ifdef __APPLE__
#  include <OpenGL/OpenGL.h>
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

#include <cmath>
#include <vector>
#include <queue>

#include "../lib/mconst.h"
#include "../lib/sintable.hpp"

#include "../gl/config.hpp"
#include "../gl/glwidget.hpp"
#include "../gl/vector-print.hpp"

namespace mn {

namespace physics {

const Object::Vector::value_type Object::G = 6.67428-1;

static const GLfloat materialSpecular[] = { 0.75, 0.75, 0.75, 1 };
static const GLfloat zeros           [] = { 0, 0, 0, 1 };
static const GLfloat ones            [] = { 1, 1, 1, 1 };

namespace {

struct PushMatrix {
	PushMatrix() { glPushMatrix(); }
	~PushMatrix() { glPopMatrix(); }
};

}

Object::Object(const std::string &theName, Object *previous)
	: mass(1), size(1), name(theName), light(-1), frozen(false),
	  textList(0), next(this) {
	setColor(1, 1, 1);
	if (previous) {
		next = previous->next;
		previous->next = this;
	}
}

Object::~Object() {
	if (textList) {
		glDeleteLists(textList, 1);
	}
}

void Object::draw(const gl::Widget &gl) {
	glPushMatrix();

	glTranslatef(point.x, point.y, point.z);

	if (light >= 0) {
		glEnable(GL_LIGHT0 + light);
		glLightfv(GL_LIGHT0 + light, GL_DIFFUSE, ones);
		glLightfv(GL_LIGHT0 + light, GL_POSITION, zeros);
	}

	// if (gl.isInFront(point)) {
		bool gotTexture = gl.config->showTextures && *texture;
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE,
		             gotTexture ? ones : materialColor);
		glMaterialfv(GL_FRONT, GL_SPECULAR,
		             light >= 0 ? zeros : materialSpecular);
		glMaterialfv(GL_FRONT, GL_EMISSION,
		             light <  0
		             ? zeros : (gotTexture ? ones : materialColor));
		glMaterialf(GL_FRONT, GL_SHININESS, light >= 0 ? 0 : 12);

		gl.sphere(size, point, &texture, name, &textList);
	// }

	glPopMatrix();
}

namespace {
	struct Acceleration {
		Object::Vector vector;
		Object::Vector::value_type value;

		Acceleration(const Object::Vector &theVector,
		             const Object::Vector::value_type &theValue)
			: vector(theVector), value(theValue) { }
		Acceleration(const Object::Vector &theVector)
			: vector(theVector), value(theVector.length()) { }

		bool operator<(const Acceleration &a) const {
			return value < a.value;
		}
	};
}

void Object::tick_(Vector::value_type dt) {
	static std::priority_queue<Acceleration> accelerations;

	for (Object *o = next; o != this; o = o->next) {
		if (o->mass < 0.01) continue;
		const Vector r = o->point - point;
		const Vector::value_type l2 = r.length2();
		if (l2 < 0.01) continue;
		Vector::value_type value = G * o->mass / l2;
		accelerations.push(Acceleration(r * (value / sqrt(l2)), value));
	}

	if (!accelerations.empty()) {
		Vector a(0, 0, 0);
		do {
			a += accelerations.top().vector;
			accelerations.pop();
		} while (!accelerations.empty());
		velocity += a * dt;
	}
	nextPoint = point + velocity * dt;
}

void Object::save(std::ostream &out) throw(std::ios_base::failure) {
	out << '"' << name << "\"\t@ " << point;
	if (frozen) {
		out << " frozen";
	} else {
		out << " vel " << velocity;
	}
	out << " mass " << mass << " size " << size << " color "
		<< materialColor[0] << ' ' << materialColor[1] << ' '
		<< materialColor[2];
	if (light) {
		out << " light";
	}
	out << '\n';
}

}

}
