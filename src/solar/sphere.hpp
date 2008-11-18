#ifndef H_PLANET_HPP
#define H_PLANET_HPP

#include <string>

#include "color.hpp"
#include "vector.hpp"
#include "texture.hpp"


namespace mn {

namespace solar {

struct Sphere {
	Sphere(float theDistance, float theSize, float theOmega, float theOmega2,
	       const gl::Color &color, const std::string &theName)
		: distance(theDistance), size(theSize), omega(theOmega),
		  omega2(theOmega2), name(theName), light(-1), first(0),
		  next(0), textList(0) {
		materialColor[0] = color.r;
		materialColor[1] = color.g;
		materialColor[2] = color.b;
		materialColor[3] = 1;
	}

	~Sphere();


	int getLight() const { return light; }
	void setLight(int theLight) { light = theLight; }

	Sphere &pushSatelite(Sphere &sphere) {
		Sphere *sp = &sphere;
		while (sp->next) sp = sp->next;
		sp->next = first;
		first = &sphere;
		return sphere;
	}

	void draw(unsigned long ticks, const gl::Vector &pos);

	static float cutoffDistance2;
	static bool lowQuality, drawOrbits, drawNames, useTextures;

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
	const float distance, size, omega, omega2;
	const std::string name;
	int light;
	Sphere *first, *next;

	float materialColor[4];
	unsigned int textList;

};


}

}

#endif
