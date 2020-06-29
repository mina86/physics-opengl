/*
 * src/common/camera.hpp
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
#include "mconst.h"


namespace mn {

namespace gl {


struct Camera {
	Camera() : eye(0, 0, 5), rotX(0), rotY(0), valid(true) {
		for (unsigned i = 0; i < 16; ++i) matrix[i] = ID[i];
	}

	void reset() {
		eye.x = eye.y = 0;
		eye.z = 5;
		rotX = rotY = 0;
		for (unsigned i = 0; i < 11; ++i) matrix[i] = ID[i];
		valid = true;
	}

	const Vector<float> &getEye() const { return eye; }
	void setEye(const Vector<float> &v) { eye = v; checkDistance(); }
	float getEyeX() const { return eye.x; }
	float getEyeY() const { return eye.y; }
	float getEyeZ() const { return eye.z; }

	void moveX(float x) {
		moved = true;
		if (!valid) update();
		eye.x += x * matrix[ 0];
		/*eye.y += x * matrix[ 4];*/
		eye.z += x * matrix[ 8];
		checkDistance();
	}
	void moveY(float y) {
		moved = true;
		if (!valid) update();
		eye.x += y * matrix[ 1];
		eye.y += y * matrix[ 5];
		eye.z += y * matrix[ 9];
		checkDistance();
	}
	void moveZ(float z) {
		moved = true;
		if (!valid) update();
		eye.x += z * matrix[ 2];
		eye.y += z * matrix[ 6];
		eye.z += z * matrix[10];
		checkDistance();
	}

	float getRotX() const { return rotX; }
	float getRotY() const { return rotY; }
	void setRotX(float v) {
		rotX = std::fabs(v)>MN_PI_2-0.02f ? copysignf(MN_PI_2-0.02f, v) : v;
		valid = false;
	}
	void setRotY(float v) {
		rotY = std::fmod(v + MN_2PI, MN_2PI);
		if (rotY > MN_PI) {
			rotY -= MN_2PI;
		}
		valid = false;
	}

	void rotateTop(float x) { setRotX(rotX + x); }
	void rotateLeft(float y) { setRotY(rotY - y); }
	void rotate(const Vector<float> &v) {
		setRotX(rotX + v.x);
		setRotY(rotY - v.y);
	}

	void doRotate() const {
		if (!valid) update();
		glMultMatrixf(matrix);
	}
	void doMove() const {
		glTranslatef(-eye.x, -eye.y, -eye.z);
	}
	void doLookAt() const {
		doRotate();
		doMove();
	}


	bool isInFront(const Vector<float> &point) const {
		const float md1 =
			eye.x * matrix[ 2] +
			eye.y * matrix[ 6] +
			eye.z * matrix[10];
		const float md2 =
			point.x * matrix[ 2] +
			point.y * matrix[ 6] +
			point.z * matrix[10];
		return md2 < md1;
	}


	bool moved;

private:
	void update() const;
	void checkDistance() {
		const float length = eye.length();
		if (length > maxDistance) {
			eye *= maxDistance / length;
		}
	}

	Vector<float> eye;
	float rotX, rotY;
	mutable float matrix[16];
	mutable bool valid;

	static const float ID[16];


public:
	static void defaultLookAt() {
		if (camera) {
			camera->doLookAt();
		}
	}
	static void registerHandlers();
	static void printHelp();

	static float aspect() { return _aspect; }

	typedef void(*KeyboardFunc)(unsigned key, bool down, int x, int y);

	static float keyMovementFactor, keyRotationTopFactor;
	static float keyRotationLeftFactor, mouseMovementFactor;
	static float mouseRotationTopFactor, mouseRotationLeftFactor;
	static float runFactor, creepFactor;
	static float maxDistance;

	enum TickRedisplayPolicy { NEVER, ALWAYS, WHEN_COUNTING };
	static TickRedisplayPolicy tickRedisplayPolicy;
	static bool nextTickRedisplays, countTicks;
	static Camera *camera;
	static KeyboardFunc keyboardFunc;
	static unsigned long ticks, tickIncrement;

private:
	static float _aspect;

	friend struct CameraImpl;
};


}

}

#endif
