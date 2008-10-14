#ifndef H_PLANET_HPP
#define H_PLANET_HPP

namespace mn {

struct Color {
	float r, g, b;
	Color(float _r = 1, float _g = 1, float _b = 1) : r(_r), g(_g), b(_b) { }
};

struct Sphere {
	Sphere(float theDistance, float theSize, float theOmega, Color theColor)
		: distance(theDistance), size(theSize), omega(theOmega),
		  color(theColor), first(0), next(0) { }

	~Sphere();

	Sphere &pushSatelite(Sphere &sphere) {
		Sphere *sp = &sphere;
		while (sp->next) sp = sp->next;
		sp->next = first;
		first = &sphere;
		return sphere;
	}

	void draw(unsigned long ticks);

	static float omegaFactor;
	static float distanceFactor;
	static float sizeFactor;

private:
	const float distance, size, omega;
	const Color color;
	Sphere *first, *next;
};

}

#endif
