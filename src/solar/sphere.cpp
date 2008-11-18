#ifdef __APPLE__
#  include <OpenGL/OpenGL.h>
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

#include "sphere.hpp"

#include <stdio.h>

#include <cmath>

#include "../common/camera.hpp"
#include "../common/text3d.hpp"
#include "../common/sintable.hpp"
#include "../common/quadric.hpp"
#include "../common/mconst.h"


namespace mn {

namespace solar {


float Sphere::cutoffDistance2 = 2500.0;
bool Sphere::lowQuality = false;
bool Sphere::drawOrbits = true;
bool Sphere::drawNames = true;
bool Sphere::useTextures = true;

static const GLfloat materialSpecular[] = { 0.75, 0.75, 0.75, 1 };
static const GLfloat zeros           [] = { 0, 0, 0, 1 };
static const GLfloat ones            [] = { 1, 1, 1, 1 };


Sphere::~Sphere() {
	for (Sphere *sp = first, *n; sp; sp = n) {
		n = sp->next;
		delete sp;
	}
}


static void drawCircle(float radius) {
	static GLuint displayList = 0;

	glPushMatrix();
	glScalef(radius, radius, radius);
	if (displayList) {
		glCallList(displayList);
	} else {
		displayList = glGenLists(1);
		if (displayList) {
			glNewList(displayList, GL_COMPILE);
		}
		glBegin(GL_LINE_LOOP);
		for (int arg = 0; arg < 360; ++arg) {
			glVertex3f(mn::sin(arg), 0, mn::cos(arg));
		}
		glEnd();
		if (displayList) {
			glEndList();
			glCallList(displayList);
		}
	}
	glPopMatrix();
}


void Sphere::draw(unsigned long ticks, const gl::Vector &centerPos) {
	const float phi = ticks * omega;
	const gl::Vector pos = centerPos + gl::Vector(distance * mn::sin(phi + 0.5f), 0, distance * mn::cos(phi + 0.5f));
	gl::Camera *const cam = gl::Camera::camera;
	const float distance2 = cam ? cam->getEye().distance2(pos) : 0;
	const float distanceFactor2 = distance2 > cutoffDistance2 ? std::sqrt(distance2 / cutoffDistance2) : 1;
	const bool inFront = cam ? cam->isInFront(pos) : true;

	glPushMatrix();

	glRotatef(phi, 0, 1, 0);

	if (drawOrbits && distance > 0.1f) {
		glDisable(GL_LIGHTING);
		glColor3f(materialColor[0] * 0.4f, materialColor[1] * 0.4f,
		          materialColor[2] * 0.4f);
		drawCircle(distance);
		glEnable(GL_LIGHTING);
	}

	glTranslatef(0, 0, distance);

	if (light >= 0) {
		glEnable(GL_LIGHT0 + light);
		glLightfv(GL_LIGHT0 + light, GL_DIFFUSE, ones);
		glLightfv(GL_LIGHT0 + light, GL_POSITION, zeros);
	}

	if (inFront) {
		const bool gotTexture = useTextures && *texture;
		if (gotTexture) {
			glEnable(GL_TEXTURE_2D);
			gluQuadricTexture(gl::Quadric::quadric()->get(), 1);
			glBindTexture(GL_TEXTURE_2D, *texture);
			glPushMatrix();
			glRotatef(90, 1, 0, 0);
			glRotatef(ticks * omega2, 0, 0, 1);
		}

		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE,
		             gotTexture ? ones : materialColor);
		glMaterialfv(GL_FRONT, GL_SPECULAR,
		             light >= 0 ? zeros : materialSpecular);
		glMaterialfv(GL_FRONT, GL_EMISSION,
		             light <  0 ? zeros : (gotTexture ? ones : materialColor));
		glMaterialf(GL_FRONT, GL_SHININESS, light >= 0 ? 0 : 12);

		unsigned slices = 60 / distanceFactor2;
		if (size > 1) slices *= 2;
		if (lowQuality) slices /= 3;
		if (slices < 6) slices = 6;
		gluSphere(gl::Quadric::quadric()->get(), size, slices, slices);

		if (gotTexture) {
			glPopMatrix();
			gluQuadricTexture(gl::Quadric::quadric()->get(), 0);
			glDisable(GL_TEXTURE_2D);
		}
	}

	glRotatef(-phi, 0, 1, 0);
	if (first) {
		for (Sphere *sp = first; sp; sp = sp->next) {
			sp->draw(ticks, pos);
		}
	}

	if (inFront && drawNames && distanceFactor2 < 1.1f) {
		if (cam) {
			glRotatef(cam->getRotY() * -MN_180_PI, 0, 1, 0);
		}
		if (!textList) {
			textList = glGenLists(1);
			if (textList) glNewList(textList, GL_COMPILE);
			glTranslatef(0, size + 0.3f, 0);
			glScalef(0.1, 0.1, 0.1);
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, materialColor);
			glMaterialfv(GL_FRONT, GL_EMISSION, zeros);
			t3d::draw3D(name, 0, 0, 0.5);
			if (textList) {
				glEndList();
				glCallList(textList);
			}
		} else {
			glCallList(textList);
		}
	}

	glPopMatrix();
}


}

}
