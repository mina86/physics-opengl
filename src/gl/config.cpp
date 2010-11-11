/*
 * src/gl/config.cpp
 * Copyright 2010 by Michal Nazarewicz    <mina86@mina86.com>
 *               and Maciej Swietochowski <m@swietochowski.eu>
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
#include "config.hpp"

#include "../lib/mconst.h"

namespace gl {

namespace cfg {

Data::Data()
	: mouseMovementFactor(0.01),
	  mouseRotationFactor(10),
	  runFactor(10.0),
	  creepFactor(0.1),
	  cutOffDistance2(2500.0),
	  showText(true),
	  showTextures(true),
	  lowQuality(false),
	  showHelperAxis(true),
	  showStars(true),
	  references(1) {

	connect(&cutOffDistance2, SIGNAL(changed(value_type)),
	        this, SLOT(somethingChanged()));

	connect(&showText, SIGNAL(changed(bool)),
	        this, SLOT(somethingChanged()));
	connect(&showTextures, SIGNAL(changed(bool)),
	        this, SLOT(somethingChanged()));
	connect(&lowQuality, SIGNAL(changed(bool)),
	        this, SLOT(somethingChanged()));
	connect(&showHelperAxis, SIGNAL(changed(bool)),
	        this, SLOT(somethingChanged()));
	connect(&showStars, SIGNAL(changed(bool)),
	        this, SLOT(somethingChanged()));

	connect(&quad, SIGNAL(drawStyleChanged(GLenum)),
	        this, SLOT(somethingChanged()));
}

void Data::somethingChanged() {
	emit changed();
}

void Bool::set(bool v) {
	if (v != value) {
		value = v;
		emit changed(v);
	}
}

void Real::set(value_type v) {
	if (v != value) {
		value = v;
		emit changed(v);
	}
}

Quadric::Quadric()
	: quad(gluNewQuadric()), drawStyle(GLU_FILL) {
	if (!quad) {
		throw std::bad_alloc();
	}
}

Quadric::~Quadric() {
	gluDeleteQuadric(quad);
}

void Quadric::setDrawStyle(GLenum theDrawStyle) {
	if (theDrawStyle != drawStyle) {
			gluQuadricDrawStyle(quad, drawStyle = theDrawStyle);
			emit drawStyleChanged(drawStyle);
	}
}

void Quadric::setNextDrawStyle() {
	GLenum ds;
	switch (drawStyle) {
	case GLU_FILL: ds = GLU_LINE; break;
	case GLU_LINE: ds = GLU_POINT; break;
	default: ds = GLU_FILL; break;
	}
	setDrawStyle(ds);
}

}

}
