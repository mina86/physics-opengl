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
	Object(const std::string &theName, Object *previous)
		: mass(1), size(1), name(theName), light(-1),
		  textList(0), next(this) {
		setColor(1, 1, 1);
		if (previous) {
			next = previous->next;
			previous->next = this;
		}
	}


	const gl::Vector &getPosition() const { return point; }
	gl::Vector &getPosition() { return point; }
	void setPosition(const gl::Vector &thePoint) {
		point = nextPoint = thePoint;
	}
	void setPosition(float x, float y, float z) {
		point.x = nextPoint.x = x;
		point.y = nextPoint.y = y;
		point.z = nextPoint.z = z;
	}

	gl::Vector &getVelocity() { return velocity; }
	const gl::Vector &getVelocity() const { return velocity; }
	void setVelocity(const gl::Vector &theVelocity) { velocity = theVelocity; }
	void setVelocity(float x, float y, float z) {
		velocity.x = x;
		velocity.y = y;
		velocity.z = z;
	}

	float getMass() const { return mass; }
	void setMass(float theMass) { mass = theMass; }

	float getSize() const { return size; }
	void setSize(float theSize) { size = theSize; }

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

	static float cutoffDistance2;
	static bool lowQuality, drawNames, useTextures;

	void draw();
	void drawAll() {
		Object *o = this;
		do o->draw(); while ((o = o->next) != this);
	}

	void tick(float dt);
	void tickAll(float dt) {
		Object *o = this;
		do o->tick(dt); while ((o = o->next) != this);
	}

	void updatePoint() { point = nextPoint; }
	void updatePointAll() {
		Object *o = this;
		do o->updatePoint(); while ((o = o->next) != this);
	}

	void ticksAll(unsigned count, float dt) {
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


	static float G;


private:
	gl::Vector point, nextPoint, velocity;
	float mass, size;
	const std::string name;
	int light;

	float materialColor[4];
	unsigned int textList;

	Object *next;
};


}

}

#endif
