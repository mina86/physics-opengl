#ifdef __APPLE__
#  include <OpenGL/OpenGL.h>
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

#include <stdio.h>

#include <cmath>

#include "planet.hpp"
#include "camera.hpp"
#include "text3d.hpp"


namespace mn {

Sphere::~Sphere() {
	for (Sphere *sp = first, *n; sp; sp = n) {
		n = sp->next;
		delete sp;
	}
}


static void drawCircle(float radius) {
	unsigned count = radius <= 500 ? (unsigned)(radius / 2) : 500;
	if (count < 100) count = 100;
	glBegin(GL_LINE_LOOP);
	glVertex3f(0, 0, radius);
	float arg = 0, arg_inc = 2 * M_PI / count;
	do {
		arg += arg_inc;
		glVertex3f(radius * std::sin(arg), 0, radius * std::cos(arg));
	} while (--count);
	glEnd();
}


void Sphere::draw(unsigned long ticks) {
	glPushMatrix();

	glRotatef(ticks * omega, 0, 1, 0);

	glDisable(GL_LIGHTING);
	if (distance > 0.1) {
		if (!circleList) {
			circleList = glGenLists(1);
			if (circleList) glNewList(circleList, GL_COMPILE);
			glColor3f(color.r * 0.2, color.g * 0.2, color.b * 0.2);
			drawCircle(distance);
			if (circleList) glEndList();
		} else {
			glCallList(circleList);
		}
	}
	glEnable(GL_LIGHTING);

	glTranslatef(0, 0, distance);
	glColor3f(color.r, color.g, color.b);
	glutSolidSphere(size, 30, 30);

	glRotatef(ticks * -omega, 0, 1, 0);
	if (first) {
		for (Sphere *sp = first; sp; sp = sp->next) {
			sp->draw(ticks);
		}
	}

	static int t3dInitialised = 0;
	if (!t3dInitialised) {
		try {
			t3d::init();
			t3dInitialised = 1;
		}
		catch (const t3d::load_exception &e) {
			fprintf(stderr, "Error initialising 3D text: %s\n",
			        e.what());
			t3dInitialised = -1;
		}
	}
	if (t3dInitialised == 1) {
		Camera *cam = Camera::getDefaultCamera();
		if (cam) {
			float rot = cam->getRotY();
			glRotatef(rot * (-180.0 / M_PI) - 90, 0, 1, 0);
		}
		glTranslatef(0, size + 0.2, 0);
		glScalef(0.1, 0.1, 0.1);
		t3d::draw3D(name, 0, 0, 0.5);
	}

	glPopMatrix();
}


}
