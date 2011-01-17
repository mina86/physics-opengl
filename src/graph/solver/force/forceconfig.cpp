/*
 * src/graph/solver/force/forceconfig.cpp
 * Copyright 2010 by Michal Nazarewicz <mina86@mina86.com>
 *               and Maciej Swietochowski
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
#include "forceconfig.hpp"

#include "../../../gl/glconfig.hpp"

namespace graph {

namespace solver {

namespace force {

Data::Data()
	: repulsionForce("Repulsion force", 1.0, 10000.0, 100.0),
	  attractionForce("Attraction force", 1.0, 10000.0, 10.0),
	  hitForce("Repulsion when nodes are very near", 1.0, 10000.0, 25.0),
	  middleForce("Force towards the center", 0.0, 10000.0, 100),
	  desiredDistance("Desired edge length", 2.0, 100.0, 5.0),
	  damping("Damping", 0.01, 0.99, 0.9),
	  moveLimit("Move limit", 1.0, MAX_DISTANCE, 10.0),
	  velocityLimit("Velocity limit", 1.0, MAX_DISTANCE, 500.0),
	  dt("dt per iteration", 0.0001, 0.5, 0.05),
	  attractionMultiply("Attraction proportional to degree", true),
	  massCenterFix("Use center of the mass fix", true) {
	init();
}

ui::cfg::Data::iterator Data::items() const {
	static const unsigned array[] = {
		CFG_DATA_OFFSET(repulsionForce),
		CFG_DATA_OFFSET(attractionForce),
		CFG_DATA_OFFSET(attractionMultiply),
		CFG_DATA_OFFSET(hitForce),
		CFG_DATA_OFFSET(middleForce),
		CFG_DATA_OFFSET(desiredDistance),
		CFG_DATA_OFFSET(damping),
		CFG_DATA_OFFSET(moveLimit),
		CFG_DATA_OFFSET(velocityLimit),
		CFG_DATA_OFFSET(dt),
		CFG_DATA_OFFSET(massCenterFix),

		~0u,
	};
	return iterator(array);
}

}

}

}
