/*
 * src/physics/physics.cpp
 * Copyright 2009 by Michal Nazarewicz (mina86/AT/mina86/DOT/com)
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
#ifndef _GNU_SOURCE
#  define _GNU_SOURCE 1
#endif

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <getopt.h>

#include <vector>
#include <limits>

#include "../common/application.hpp"
#include "../common/sintable.hpp"
#include "../common/text3d.hpp"
#include "../common/texture.hpp"
#include "../common/mconst.h"

#include "object.hpp"
#include "data-loader.hpp"
#include "widget.hpp"

namespace mn {

/** \namespace mn::physics
 *
 * This namespace contains source code of an application that
 * simulates the way bodies interact with each other as ar as
 * gravitation is concerned.
 *
 * When invoked, application reads specification of objects either
 * from standard input (if no input file was given on command line) or
 * a given file.  For description of accepted file format see
 * documentation of loadData() function.
 *
 * Application accepts several switches which affect the way things
 * are displayed.  Most of those can also be toggled in run time.  For
 * list of switches invoke the program with <tt>-h</tt> argument and
 * to know possible key bindings see the output program written just
 * priory to starting simulation.
 *
 * For a quick start, after building programs (<tt>make</tt> in root
 * directory) invoke one of <tt>./physics data/physics.psy</tt>,
 * <tt>./physics data/physics-small.psy</tt> or <tt>./physics
 * data/harmonic.psy</tt> in <tt>dist</tt> directory.  In case of
 * animation being slow (less then 24 fps) try adding the following
 * switches: <tt>-m -c -x0</tt> as well as pressing <tt>v</tt> while
 * simulation is running.
 */
namespace physics {

void PhysicsWidget::initializeGL() {
	GLWidget::initializeGL();

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	static const GLfloat light0Color[] = { 0.01f, 0.4f, 0.2f, 1.0f };

	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0Color);
	glEnable(GL_LIGHT0);

	// static const GLfloat light1Color[] = { 0.6f, 0.4f, 0.0f, 1.0f };
	// static const GLfloat light1Pos[] = { -0.5, -5.0, 7.0, 1.0 };

	// glLightfv(GL_LIGHT1, GL_DIFFUSE, light1Color);
	// glLightfv(GL_LIGHT1, GL_POSITION, light1Pos);
	// glEnable(GL_LIGHT1);

// Create light components
//GLfloat ambientLight[] = { 0.2, 0.2f, 0.2f, 1.0f };
GLfloat diffuseLight[] = { 0.1f, 0.1f, 0.0, 1.0f };
GLfloat specularLight[] = { 0.0f, 0.0f, 0.1f, 1.0f };
GLfloat position[] = { -1.5f, 1.0f, -4.0f, 1.0f };

// Assign created components to GL_LIGHT0
glLightfv(GL_LIGHT0, GL_AMBIENT, zeros);
glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
glLightfv(GL_LIGHT0, GL_POSITION, position);

	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, ones);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT, GL_EMISSION, zeros);
	glMaterialf(GL_FRONT, GL_SHININESS, 12);
}

void PhysicsWidget::doPaint() {
	// static const GLfloat lightDirection[] = { 0.5, -1, -1, 0 };
	// glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, lightDirection);

	// sphere(5, Vector(0, 0, 0), ones);

	objects->drawAll(*this);
}

PhysicsWidget::PhysicsWidget(Object *theObjects, gl::Configuration &cfg,
                             QWidget *parent)
	: ui::GLWidget(parent, cfg), objects(theObjects) {
}

void PhysicsWidget::deleteObjects(Object *objects) {
	if (objects) {
		Object *o = objects, *n;
		do {
			n = o->getNext();
			delete o;
		} while (n != objects);
	}
}

PhysicsWidget::~PhysicsWidget() {
	deleteObjects(objects);
}

ui::GLWidget *PhysicsWidget::Factory::create(gl::Configuration &theConfig,
                                             QWidget *parent) {
	if (!objects) {
		throw std::bad_alloc();
	}
	Object *o = objects;
	objects = NULL;
	try {
		return new PhysicsWidget(o, theConfig, parent);
	}
	catch (...) {
		deleteObjects(o);
		throw;
	}
}

PhysicsWidget::Factory::~Factory() {
	deleteObjects(objects);
}

}

}
