#ifndef H_PLANET_HPP
#define H_PLANET_HPP

#include <string>

namespace mn {

struct Color {
	float r, g, b;
	Color(float _r = 1, float _g = 1, float _b = 1) : r(_r), g(_g), b(_b) { }
};

struct Sphere {
	Sphere(float theDistance, float theSize, float theOmega, Color theColor,
	       const std::string &theName)
		: distance(theDistance), size(theSize), omega(theOmega),
		  color(theColor), name(theName),
		  first(0), next(0), circleList(0) {}

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
	const Color color;
	const std::string name;
	Sphere *first, *next;

	GLuint circleList;
};

}

#endif
