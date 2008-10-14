#ifdef __APPLE__
#  include <OpenGL/OpenGL.h>
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

#include <stdio.h>

#include "planet.hpp"


namespace mn {

float Sphere::omegaFactor = 1.0;
float Sphere::distanceFactor = 1.0;
float Sphere::sizeFactor = 1.0;


Sphere::~Sphere() {
	for (Sphere *sp = first, *n; sp; sp = n) {
		n = sp->next;
		delete sp;
	}
}


void Sphere::draw(unsigned long ticks) {
	glPushMatrix();
	glRotatef(ticks * omega * omegaFactor, 0, 1, 0);
	glTranslatef(0, 0, distance * distanceFactor);
	glColor3f(color.r, color.g, color.b);
	glutSolidSphere(size * sizeFactor, 30, 30);
	if (first) {
		glRotatef(-ticks * omega * omegaFactor, 0, 1, 0);
		for (Sphere *sp = first; sp; sp = sp->next) {
			sp->draw(ticks);
		}
	}
	glPopMatrix();
}


}
