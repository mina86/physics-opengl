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
