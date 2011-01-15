/*
 * src/graph/vector-rand.hpp
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
#ifndef H_VECTOR_RAND_HPP
#define H_VECTOR_RAND_HPP

#include "../gl/vector.hpp"

namespace graph {

gl::Vector<float> &randCartesian(gl::Vector<float> &v, float max);
gl::Vector<float> &randVersor(gl::Vector<float> &v);
gl::Vector<float> &randSpheric(gl::Vector<float> &v, float max);
gl::Vector<float> &addRandNormal(gl::Vector<float> &v, float sigma);

static inline gl::Vector<float>
&randNormal(gl::Vector<float> &v, float sigma) {
	v.x() = 0.0;
	v.y() = 0.0;
	v.z() = 0.0;
	return randNormal(v, sigma);
}

}

#endif
