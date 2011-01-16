/*
 * src/graph/generator/euclideannetworkgenerator.hpp
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
#include "euclideannetworkgenerator.hpp"

#include "make-connected.hpp"

#include "../vector-rand.hpp"

namespace graph {

namespace generator {

namespace euclinet {

Data::iterator Data::items() const {
	static const unsigned array[] = {
		CFG_DATA_OFFSET(nodesCount),
		CFG_DATA_OFFSET(radius),
		CFG_DATA_OFFSET(scale),
		~0u,
	};
	return iterator(array);
}

Data::Data()
  : nodesCount("Number of nodes", 1, 10000, 20),
	radius("Proximity radius", 0, 1, 0.3),
	scale("Scale", 1, 1000, 30) {
	init();
}

}

ui::cfg::Data *EuclideanNetworkGenerator::getConfigData() {
	return &*config;
}

EuclideanNetworkGenerator::graph_ptr EuclideanNetworkGenerator::generate() {
	unsigned nodesCount = (unsigned)config->nodesCount;
	float scale = config->scale;
	float radius = config->radius * scale;

	graph_ptr g(new Graph(nodesCount));

	/* Randomize nodes */
	for (Graph::nodes_iterator i = g->nodes_begin(); i != g->nodes_end(); ++i) {
		randSpheric(*i, scale);
	}

	/* Find edges */
	Graph::const_nodes_iterator j, k, offset;
	offset = g->nodes_begin();
	for (j = g->nodes_begin(); j != g->nodes_end(); ++j) {
		k = j;
		for (++k; k != g->nodes_end(); ++k) {
			if (j->distance(*k) < radius) {
				g->e(j-offset, k-offset, true);
			}
		}
	}

	/* Make connected */
	makeConnected(*g.get());

	return g;
}

}

}
