#ifndef H_OBJECT_HPP
#define H_OBJECT_HPP

#include <math.h>

#include <string>

#include "../common/color.hpp"
#include "../common/vector.hpp"
#include "../common/texture.hpp"


namespace mn {

namespace physics {


struct Object {
	typedef gl::Vector<double> Vector;


	Object(const std::string &theName, Object *previous)
		: mass(1), size(1), name(theName), light(-1), frozen(false),
		  textList(0), next(this) {
		setColor(1, 1, 1);
		if (previous) {
			next = previous->next;
			previous->next = this;
		}
	}


	const Vector &getPosition() const { return point; }
	Vector &getPosition() { return point; }
	void setPosition(const Vector &thePoint) {
		point = nextPoint = thePoint;
	}
	void setPosition(Vector::value_type x, Vector::value_type y, Vector::value_type z) {
		point.x = nextPoint.x = x;
		point.y = nextPoint.y = y;
		point.z = nextPoint.z = z;
	}

	Vector &getVelocity() { return velocity; }
	const Vector &getVelocity() const { return velocity; }
	void setVelocity(const Vector &theVelocity) { velocity = theVelocity; }
	void setVelocity(Vector::value_type x, Vector::value_type y, Vector::value_type z) {
		velocity.x = x;
		velocity.y = y;
		velocity.z = z;
	}

	bool isFrozen() const { return frozen; }
	void setFrozen(bool theFrozen) { frozen = theFrozen; }

	Vector::value_type getMass() const { return mass; }
	void setMass(Vector::value_type theMass) { mass = theMass; }

	Vector::value_type getSize() const { return size; }
	void setSize(Vector::value_type theSize) { size = theSize; }

	void setColor(const gl::Color &theColor) {
		materialColor[0] = theColor.r;
		materialColor[1] = theColor.g;
		materialColor[2] = theColor.b;
		materialColor[3] = 1;
	}

	void setColor(float r, float g, float b) {
		materialColor[0] = r;
		materialColor[1] = g;
		materialColor[2] = b;
		materialColor[3] = 1;
	}


	int getLight() const { return light; }
	void setLight(int theLight) { light = theLight; }

	static Vector::value_type cutoffDistance2;
	static bool lowQuality, drawNames, useTextures;

	void draw();
	void drawAll() {
		Object *o = this;
		do o->draw(); while ((o = o->next) != this);
	}

	void tick(Vector::value_type dt) {
		if (!frozen) {
			tick_(dt);
		}
	}
	void tickAll(Vector::value_type dt) {
		Object *o = this;
		do o->tick(dt); while ((o = o->next) != this);
	}

	void updatePoint() {
		if (!frozen) {
			point = nextPoint;
		}
	}
	void updatePointAll() {
		Object *o = this;
		do o->updatePoint(); while ((o = o->next) != this);
	}

	void ticksAll(unsigned count, Vector::value_type dt) {
		tickAll(dt);
		while (--count) {
			updatePointAll();
			tickAll(dt);
		}
	}


	gl::Texture texture;
	void colorFromTexture() {
		if (texture) {
			const gl::Color &avg = texture.getAverageColor();
			materialColor[0] = avg.r;
			materialColor[1] = avg.g;
			materialColor[2] = avg.b;
		}
	}


	Object *getNext() { return next; }
	const Object *getNext() const { return next; }
	Object *find(const std::string &theName) {
		return const_cast<Object*>(const_cast<const Object*>(this)->find(theName));
	}
	const Object *find(const std::string &theName) const {
		const Object *o = this;
		do if (o->name == theName) return o; while ((o = o->next) != this);
		return 0;
	}


	static const Vector::value_type G;


private:
	Vector point, nextPoint, velocity;
	Vector::value_type mass, size;
	const std::string name;
	int light;
	bool frozen;

	float materialColor[4];
	unsigned int textList;

	Object *next;

	void tick_(Vector::value_type dt);
};


}

}

#endif
