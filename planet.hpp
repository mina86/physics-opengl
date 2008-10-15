#ifndef H_PLANET_HPP
#define H_PLANET_HPP

#include <string>

#include "color.hpp"


namespace mn {

namespace solar {

struct Sphere {
	Sphere(float theDistance, float theSize, float theOmega,
	       gl::Color theColor, const std::string &theName)
		: distance(theDistance), size(theSize), omega(theOmega),
		  color(theColor), name(theName),
		  first(0), next(0), circleList(0), textList(0) {}

	~Sphere();

	Sphere &pushSatelite(Sphere &sphere) {
		Sphere *sp = &sphere;
		while (sp->next) sp = sp->next;
		sp->next = first;
		first = &sphere;
		return sphere;
	}

	void draw(unsigned long ticks);

private:
	const float distance, size, omega;
	const gl::Color color;
	const std::string name;
	Sphere *first, *next;

	unsigned int circleList, textList;
};


}

}

#endif
