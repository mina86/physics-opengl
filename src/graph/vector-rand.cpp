/*
 * src/graph/vector-rand.cpp
 * Copyright 2010 by Michal Nazarewicz    <mina86@mina86.com>
 *               and Maciej Swietochowski <m@swietochowski.eu>
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
#include "vector-rand.hpp"

#include <math.h>

#include "../lib/rand.hpp"
#include "../lib/mconst.h"

namespace graph {

gl::Vector<float> &randCartesian(gl::Vector<float> &v, float max) {
	v.x() = lib::rndp(2.0 * max) - max;
	v.y() = lib::rndp(2.0 * max) - max;
	v.z() = lib::rndp(2.0 * max) - max;
	return v;
}

gl::Vector<float> &randSpheric(gl::Vector<float> &v, float max) {
	float r = lib::rnd(max);
	float a = lib::rnd(M_PI);
	float b = lib::rndp(2.0 * M_PI);
	float s = std::sin(a);
	v.x() = r * s * std::cos(b);
	v.y() = r * s * std::sin(b);
	v.z() = r * std::cos(a);
	return v;
}

/*
 * GSL uses this algorithm but instead of returning two numbers
 * returns only one.  This is unfortunate, so we have created our own
 * code that uses both calculated values therefore hoping to get twice
 * as fast as gsl_ran_gaussian().
 *
 * This is not really thread-safe but we don't use threads as of now.
 * This is something to keep in mind though.
 */
gl::Vector<float> &addRandNormal(gl::Vector<float> &v, float sigma) {
	static bool remembered = false;
	static float memory;

	float a, b, r2;
	bool first = true;

	for (;;) {
		do {
			a = lib::rndp(2.0) - 1.0;
			b = lib::rndp(2.0) - 1.0;
			r2 = a*a + b*b;
		} while (r2 > 1.0 || r2 == 0);

		r2 = sigma * std::sqrt(-2.0 * std::log(r2) / r2);
		a = a * r2;
		b = b * r2;

		if (first) {
			v.x() += a;
			v.y() += b;
			if (remembered) {
				v.z() = memory;
				remembered = false;
				break;
			}
			first = false;
		} else {
			v.z() += a;
			remembered = true;
			memory = b;
			break;
		}
	}

	return v;
}

}
