#include "config.hpp"
#include "mconst.h"

namespace mn {

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

}
