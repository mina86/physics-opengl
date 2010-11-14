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

#include <cstring>

#include <algorithm>
#include <stdexcept>

namespace graph {

void Graph::init(unsigned theN) throw(std::invalid_argument, std::bad_alloc) {
	if (!theN) {
		throw std::invalid_argument("empty graph");
	}

	count = theN;

	nodes_vec = new node_type[nodes()];
	try {
		edges_vec = new edge_type[edges()];
	}
	catch (...) {
		delete nodes_vec;
		throw;
	}
}

void Graph::check_size(const Graph &g) throw(std::invalid_argument) {
	if (g.count != count) {
		throw std::invalid_argument("graphs' size mismatch");
	}
}

void Graph::set(const Graph &g) throw(std::invalid_argument) {
	check_size(g);
	setNodes(g.nodes_vec);
	setEdges(g.edges_vec);
}
void Graph::setNodes(const Graph &g) throw(std::invalid_argument) {
	check_size(g);
	setNodes(g.nodes_vec);
}
void Graph::setEdges(const Graph &g) throw(std::invalid_argument) {
	check_size(g);
	setEdges(g.edges_vec);
}

void Graph::swap(Graph &g) throw(std::invalid_argument) {
	check_size(g);
	swapNodes(g.nodes_vec);
	swapEdges(g.edges_vec);
}
void Graph::swapNodes(Graph &g) throw(std::invalid_argument) {
	check_size(g);
	swapNodes(g.nodes_vec);
}
void Graph::swapEdges(Graph &g) throw(std::invalid_argument) {
	check_size(g);
	swapEdges(g.edges_vec);
}

void Graph::setNodes(const node_type *theNodes) {
	std::memcpy(nodes_vec, theNodes, nodes() * sizeof *theNodes);
}
void Graph::setEdges(const edge_type *theEdges) {
	std::memcpy(edges_vec, theEdges, edges() * sizeof *theEdges);
}

void Graph::swapNodes(node_type *&theNodes) {
	std::swap(nodes_vec, theNodes);
}
void Graph::swapEdges(edge_type *&theEdges) {
	std::swap(edges_vec, theEdges);
}

}
