/*
 * src/graph/randomise-graph.cpp
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
#include "randomise-graph.hpp"

#include "scene.hpp"
#include "vector-rand.hpp"
#include "../lib/rand.hpp"
#include "../gl/glconfig.hpp"

#include <math.h>

namespace graph {

namespace rnd {

Data::Data()
	: relative("Move relative to current position", false),
	  normal("Normal distribution (not uniform)", false),
	  overwrite("Overwrite values loaded from file", true),
	  reset("Reset values loaded from file", false),
	  r("Radius (for uniform), sigma (for normal)", 0.0, MAX_DISTANCE, 10.0) {
	init();
}

Data::iterator Data::items() const {
	static const unsigned array[] = {
		CFG_DATA_OFFSET(relative),
		CFG_DATA_OFFSET(normal),
		CFG_DATA_OFFSET(r),
		CFG_DATA_OFFSET(overwrite),
		CFG_DATA_OFFSET(reset),
		~0u,
	};
	return iterator(array);
}

}

void GraphRandomiser::randomise(Scene &scene) {
	gl::Vector<float> v;

	Scene::nodes_iterator it = scene.nodes_begin(), end = scene.nodes_end();
	for (; it != end; ++it) {
		//isnan() fix:
		float secondLoadedPositionX = (*it).second.loadedPosition.x();
		if (secondLoadedPositionX == secondLoadedPositionX && !config->overwrite) {
			if (config->reset) {
				(*it).first = (*it).second.loadedPosition;
			}
		} else if (config->r < 0.00001) {
			if (!config->relative) {
				(*it).first.zero();
			}
		} else if (config->relative) {
			if (config->normal) {
				addRandNormal((*it).first, config->r);
			} else {
				(*it).first += randSpheric(v, config->r);
			}
		} else {
			if (config->normal) {
				randNormal((*it).first, config->r);
			} else {
				randSpheric((*it).first, config->r);
			}
		}
		(*it).first.limit(MAX_DISTANCE);
	}
}

}
