#ifndef H_TYPES_HPP
#define H_TYPES_HPP

#include <cmath>
#include <stdio.h>

namespace mn {


struct Vector {
	float x, y, z;

	Vector(float theX = 0, float theY = 0, float theZ = 0)
		: x(theX), y(theY), z(theZ) { }

	float length() const {
		return std::sqrt(x*x + y*y + z*z);
	}

	void normalize() { *this *= 1 / length(); }

	Vector &operator+=(const Vector &v) {
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	Vector &operator-=(const Vector &v) {
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	Vector &operator*=(float r) {
		x *= r;
		y *= r;
		z *= r;
		return *this;
	}

	Vector &operator/=(float r) {
		return *this *= 1 / r;
	}

	Vector  operator- () const {
		return Vector(-x, -y, -z);
	}

	Vector  operator+ () const {
		return *this;
	}
};


inline Vector operator+(Vector a, const Vector &b) { return a += b; }
inline Vector operator-(Vector a, const Vector &b) { return a -= b; }
inline Vector operator*(Vector a, float r) { return a *= r; }
inline Vector operator*(float r, Vector a) { return a *= r; }
inline Vector operator/(Vector a, float r) { return a /= r; }


static struct Camera {
	static const float r = 3;

	Camera() : eye(0, 0, 5), rotX(M_PI/2), rotY(-M_PI/2), valid(false) { }

	const Vector &getEye() const { return eye; }
	void setEye(const Vector &v) { eye = v; }
	float getEyeX() const { return eye.x; }
	float getEyeY() const { return eye.y; }
	float getEyeZ() const { return eye.z; }

	void moveLeft(float x) { eye += x * left; }
	void moveTop(float y) { eye += y * top; }
	void moveForward(float z) { eye += z * forward; }
	void move(const Vector &v) {
		((eye += v.x * left) += v.y * top) += v.z * forward;
	}

	float getRotX() const { return rotX; }
	float getRotY() const { return rotY; }
	void setRotX(float v) {
		rotX = v; valid = false;
		if (rotX > 3) rotX = 3; else if (rotX < 0) rotX = 0;
	}
	void setRotY(float v) { rotY = v; valid = false; }

	void rotateTop(float x) { setRotX(rotX - x); }
	void rotateLeft(float y) { rotY -= y; valid = false; }
	void rotate(const Vector &v) { setRotX(rotX - v.x); rotY -= v.y; }

	const Vector &getForward() const {
		if (!valid) update();
		return forward;
	}
	Vector getCenter() const { return r * getForward() + eye; }
	float getCenterX() const { return r * getForward().x + eye.x; }
	float getCenterY() const { return r * getForward().y + eye.y; }
	float getCenterZ() const { return r * getForward().z + eye.z; }

	const Vector &getTop() { if (!valid) update(); return top; }
	float getTopX() { return getTop().x; }
	float getTopY() { return getTop().y; }
	float getTopZ() { return getTop().z; }

private:
	void update() const {
		const float cx = std::cos(rotX), sx = std::sin(rotX);
		const float cy = std::cos(rotY), sy = std::sin(rotY);

		forward.y = cx;
		forward.x = sx * cy;
		forward.z = sx * sy;

		top.y = sx;
		top.x = -cx * cy;
		top.z = -cx * sy;

		left.y = 0;
		left.x = sy;
		left.z = -cy;

		valid = true;

		fprintf(stderr, "f: <%f, %f, %f>; t: <%f, %f, %f>\n",
				forward.x, forward.y, forward.z,
				top.x, top.y, top.z);
	}

	Vector eye;
	float rotX, rotY;
	mutable Vector forward, top, left;
	mutable bool valid;
} camera;

}

#endif
