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
	Object(const gl::Vector &thePoint, const gl::Vector &theVelocity,
	       float theMass, float theSize,
	       const gl::Color &theColor, const std::string &theName,
	       Object *previous = 0)
		: point(thePoint), nextPoint(thePoint), velocity(theVelocity),
		  mass(theMass), size(theSize), name(theName), light(-1),
		  textList(0), next(this) {
		materialColor[0] = theColor.r;
		materialColor[1] = theColor.g;
		materialColor[2] = theColor.b;
		materialColor[3] = 1;

		if (previous) {
			next = previous->next;
			previous->next = this;
		}
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
		dt /= count;
		do {
			updatePointAll();
			tickAll(dt);
		} while (--count);
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


private:
	gl::Vector point, nextPoint, velocity;
	const float mass, size;
	const std::string name;
	int light;

	float materialColor[4];
	unsigned int textList;

	Object *next;
};


}

}

#endif
