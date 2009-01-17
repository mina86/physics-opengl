/*
 * src/solar/sphere.hpp
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
#ifndef H_SPHERE_HPP
#define H_SPHERE_HPP

#include <string>

#include "../common/color.hpp"
#include "../common/vector.hpp"
#include "../common/texture.hpp"


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

	void draw(unsigned long ticks, const gl::Vector<float> &pos);

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
