#include "object.hpp"

#ifdef __APPLE__
#  include <OpenGL/OpenGL.h>
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

#include <stdio.h>

#include <cmath>
#include <vector>
#include <queue>

#include "../common/camera.hpp"
#include "../common/text3d.hpp"
#include "../common/sintable.hpp"
#include "../common/quadric.hpp"
#include "../common/mconst.h"


namespace mn {

namespace physics {


Object::Vector::value_type Object::cutoffDistance2 = 2500.0;
bool Object::lowQuality = false;
bool Object::drawNames = true;
bool Object::useTextures = true;
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


void Object::draw() {
	const gl::Camera *const cam = gl::Camera::camera;
	const bool inFront = cam ? cam->isInFront(point) : true;

	PushMatrix _p;

	glTranslatef(point.x, point.y, point.z);

	if (light >= 0) {
		glEnable(GL_LIGHT0 + light);
		glLightfv(GL_LIGHT0 + light, GL_DIFFUSE, ones);
		glLightfv(GL_LIGHT0 + light, GL_POSITION, zeros);
	}

	if (!inFront) {
		return;
	}

	const bool gotTexture = useTextures && *texture;
	if (gotTexture) {
		glEnable(GL_TEXTURE_2D);
		gluQuadricTexture(gl::Quadric::quadric()->get(), 1);
		glBindTexture(GL_TEXTURE_2D, *texture);
	}

	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE,
	             gotTexture ? ones : materialColor);
	glMaterialfv(GL_FRONT, GL_SPECULAR,
	             light >= 0 ? zeros : materialSpecular);
	glMaterialfv(GL_FRONT, GL_EMISSION,
	             light <  0 ? zeros : (gotTexture ? ones : materialColor));
	glMaterialf(GL_FRONT, GL_SHININESS, light >= 0 ? 0 : 12);

	const Vector::value_type distance2 = cam ? cam->getEye().distance2(point) : 0;
	const Vector::value_type distanceFactor2 = distance2 > cutoffDistance2 ? std::sqrt(distance2 / cutoffDistance2) : 1;
	unsigned slices = 60 / distanceFactor2;
	if (size > 1) slices *= 2;
	if (lowQuality) slices /= 3;
	if (slices < 6) slices = 6;
	gluSphere(gl::Quadric::quadric()->get(), size, slices, slices);

	if (gotTexture) {
		gluQuadricTexture(gl::Quadric::quadric()->get(), 0);
		glDisable(GL_TEXTURE_2D);
	}

	if (!drawNames || distanceFactor2 >= 1.1f) {
		return;
	}

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


}

}
