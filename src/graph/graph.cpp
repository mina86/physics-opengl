/*
 * src/graph/graph.hpp
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
#include "graph.hpp"

#include "node.hpp"

namespace graph {

void Graph::set(unsigned theN, edge_type *theEdges, node_type *theNodes) {
	if (!theN) {
		delete nodes_vec;
		delete edges_vec;
		count = 0;
		nodes_vec = 0;
		edges_vec = 0;
		return;
	}

	bool cleared = false;
	if (theN != count) {
		edge_type *e = new edge_type[edges(theN)];
		node_type *n;
		try {
			count = new node_type[theN];
		} catch (...) {
			delete[] e;
			throw;
		}

		delete nodes_vec;
		delete edges_vec;
		count = theN;
		nodes_vec = n;
		edges_vec = e;
		cleared = true;
	}

	if (theNodes) {
		overwriteNodes(theNodes);
	} else if (!cleared) {
		std::fill(nodes_vec, nodes_vec + theN, node_type());
	}

	if (theEdges) {
		overwriteEdges(theEdges);
	} else if (!cleared) {
		std::fill(edges_vec, edges_vec + edges(theN), edge_type());
	}
}

}
