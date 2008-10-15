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
#include "sintable.hpp"


namespace mn {

namespace solar {


const GLfloat Sphere::materialSpecular[4] = { 0.75, 0.75, 0.75, 1 };
const GLfloat Sphere::materialNoEmission[4] = { 0, 0, 0, 1 };


Sphere::~Sphere() {
	for (Sphere *sp = first, *n; sp; sp = n) {
		n = sp->next;
		delete sp;
	}
}


static void drawCircle(float radius) {
	glBegin(GL_LINE_LOOP);
	for (int arg = 0; arg < 360; ++arg) {
		glVertex3f(radius * mn::sin(arg), 0, radius * mn::cos(arg));
	}
	glEnd();
}


void Sphere::draw(unsigned long ticks, bool center) {
	glPushMatrix();

	glRotatef(ticks * omega, 0, 1, 0);

	glDisable(GL_LIGHTING);
	if (distance > 0.1) {
		if (!circleList) {
			circleList = glGenLists(1);
			if (circleList) glNewList(circleList, GL_COMPILE);
			glColor3f(materialColor[0] * 0.2, materialColor[1] * 0.2, materialColor[2] * 0.2);
			drawCircle(distance);
			if (circleList) {
				glEndList();
				glCallList(circleList);
			}
		} else {
			glCallList(circleList);
		}
	}
	glEnable(GL_LIGHTING);

	glTranslatef(0, 0, distance);

	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, materialColor);
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	glMaterialfv(GL_FRONT, GL_EMISSION, center ? materialEmission : materialNoEmission);
	glMaterialf(GL_FRONT, GL_SHININESS, 12);
	glutSolidSphere(size, 30, 30);

	glRotatef(ticks * -omega, 0, 1, 0);
	if (first) {
		for (Sphere *sp = first; sp; sp = sp->next) {
			sp->draw(ticks);
		}
	}

	gl::Camera *cam = gl::Camera::getDefaultCamera();
	if (cam) {
		float rot = cam->getRotY();
		glRotatef(rot * (-180.0 / M_PI) - 90, 0, 1, 0);
	}
	if (!textList) {
		textList = glGenLists(1);
		if (textList) glNewList(textList, GL_COMPILE);
		glTranslatef(0, size + 0.3, 0);
		glScalef(0.1, 0.1, 0.1);
		t3d::draw3D(name, 0, 0, 0.5);
		if (textList) {
			glEndList();
			glCallList(textList);
		}
	} else {
		glCallList(textList);
	}

	glPopMatrix();
}


}

}
