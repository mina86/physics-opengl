/*
 * src/common/quadric.hpp
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
#ifndef H_QUADRIC_HPP
#define H_QUADRIC_HPP

#include <stdexcept>

#ifdef __APPLE__
#  include <OpenGL/OpenGL.h>
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

namespace mn {

namespace gl {


struct Quadric {
	explicit Quadric(GLUquadric *theQuad = 0)
		: quad(theQuad ? theQuad : gluNewQuadric()), drawStyle(GLU_FILL) {
		if (!quad) throw std::bad_alloc();
	}

	~Quadric() {
		gluDeleteQuadric(quad);
	}

	GLenum getDrawStyle() const { return drawStyle; }
	void setDrawStyle(GLenum theDrawStyle) {
		gluQuadricDrawStyle(quad, drawStyle = theDrawStyle);
	}
	void setNextDrawStyle();

	GLUquadric *get() { return quad; }
	operator GLUquadric *() const { return quad; }



	inline static Quadric *quadric() {
		if (!singleton) {
			singleton = new Quadric();
		}
		return singleton;
	}

	static void destroy() {
		delete singleton;
		singleton = 0;
	}


private:
	GLUquadric *quad;
	GLenum drawStyle;

	Quadric(const Quadric &q) { (void)q; }
	void operator=(const Quadric &q) { (void)q; }


	static Quadric *singleton;
};

}

}


#endif
