/*
 * src/graph/generator/treegenerator.hpp
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
#include "treegenerator.hpp"

#include "../vector-rand.hpp"
#include "../../lib/rand.hpp"

#include <math.h>

namespace graph {

namespace generator {

namespace tree {

Data::iterator Data::items() const {
	static const unsigned array[] = {
		CFG_DATA_OFFSET(nodesCount),
		~0u,
	};
	return iterator(array);
}

Data::Data() : nodesCount("Number of nodes", 1, 10000, 20) {
	init();
}

}

ui::cfg::Data *TreeGenerator::getConfigData() {
	return &*config;
}

TreeGenerator::graph_ptr TreeGenerator::generate() {
	unsigned nodesCount = (unsigned)config->nodesCount;
	float r = sqrt(10.0f * nodesCount);
	graph_ptr g(new Graph(nodesCount));

	/* Randomize nodes */
	for (Graph::nodes_iterator i = g->nodes_begin(); i != g->nodes_end(); ++i) {
		randSpheric(*i, r);
	}

	for (unsigned i = 1; i < nodesCount; ++i) {
		unsigned n = lib::rnd(i);
		g->e(i, n, true);
	}

	return g;
}

}

}
