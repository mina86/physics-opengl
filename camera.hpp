#ifndef H_CAMERA_HPP
#define H_CAMERA_HPP

#include <cmath>

#ifdef __APPLE__
#  include <OpenGL/OpenGL.h>
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

#include "vector.hpp"


namespace mn {

namespace gl {


struct Camera {
	Camera() : eye(0, 0, 5), rotX(M_PI/2), rotY(-M_PI/2), valid(false) { }

	void reset() {
		eye.x = eye.y = 0;
		eye.z = 5;
		rotX = M_PI/2;
		rotY = -M_PI/2;
		valid = false;
	}

	const Vector &getEye() const { return eye; }
	void setEye(const Vector &v) { eye = v; checkDistance(); }
	float getEyeX() const { return eye.x; }
	float getEyeY() const { return eye.y; }
	float getEyeZ() const { return eye.z; }

	void moveLeft(float x) {
		if (!valid) update();
		eye += x * left;
		checkDistance();
	}
	void moveTop(float y) {
		if (!valid) update();
		eye += y * top;
		checkDistance();
	}
	void moveForward(float z) {
		if (!valid) update();
		eye += z * forward;
		checkDistance();
	}
	void move(const Vector &v) {
		if (!valid) update();
		((eye += v.x * left) += v.y * top) += v.z * forward;
		checkDistance();
	}

	float getRotX() const { return rotX; }
	float getRotY() const { return rotY; }
	void setRotX(float v) {
		rotX = v; valid = false;
		if (rotX > M_PI - 0.1) rotX = M_PI - 0.1;
		else if (rotX < 0.1) rotX = 0.1;
	}
	void setRotY(float v) { rotY = v; valid = false; }

	void rotateTop(float x) { setRotX(rotX - x); }
	void rotateLeft(float y) { rotY -= y; valid = false; }
	void rotate(const Vector &v) { setRotX(rotX - v.x); rotY -= v.y; }

	const Vector &getForward() const {
		if (!valid) update();
		return forward;
	}

	const Vector &getTop() { if (!valid) update(); return top; }
	float getTopX() { return getTop().x; }
	float getTopY() { return getTop().y; }
	float getTopZ() { return getTop().z; }

	void doRotate() const;
	void doLookAt() const {
		doRotate();
		glTranslatef(-eye.x, -eye.y, -eye.z);
	}

private:
	void update() const;
	void checkDistance() {
		const float length = eye.length();
		if (length > maxDistance) {
			eye *= maxDistance / length;
		}
	}

	Vector eye;
	float rotX, rotY;
	mutable Vector forward, top, left;
	mutable bool valid;


public:
	static void defaultLookAt() {
		if (camera) {
			camera->doLookAt();
		}
	}
	static void registerHandlers();
	static void printHelp();

	typedef void(*KeyboardFunc)(unsigned key, bool down, int x, int y);

	static float keyMovementFactor, keyRotationTopFactor;
	static float keyRotationLeftFactor, mouseMovementFactor;
	static float mouseRotationTopFactor, mouseRotationLeftFactor;
	static float runFactor, creepFactor;
	static float maxDistance;

	static bool tickRedisplays, countTicks;
	static Camera *camera;
	static KeyboardFunc keyboardFunc;
	static unsigned long ticks;
};


}

}

#endif
