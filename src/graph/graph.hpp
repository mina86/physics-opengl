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
#ifndef H_GRAPH_HPP
#define H_GRAPH_HPP

#include <cstring>

#include "../gl/vector.hpp"

namespace graph {

struct Groph {
	typedef bool              edge_type;
	typedef gl::Vector<float> node_type;

	Graph() : count(), nodes_vec(), edges_vec() { }
	explicit Edges(unsigned theN) : count(), nodes_vec(), edges_vec() {
		set(theN);
	}
	Edges(const Edges &e) : count(), nodes_vec(), edges_vec() {
		set(e.n, e.nodes_vec, e.edges_vec);
	}
	~Edges() {
		delete nodes_vec;
		delete edges_vec;
	}

	void set(unsigned theN, node_type *theNodes = 0, edge_type *theEdges = 0);
	void set(const Edges &e) { set(e.n, e.edges, e.nodes); }
	Edges &operator=(const Edges &e) { set(e); return *this; }

	void overwriteNodes(const node_type *theNodes) {
		std::memcpy(nodes_vec, theNodes, nodes() * sizeof *theNodes);
	}
	void overwriteEdges(const edge_type *theEdges) {
		std::memcpy(edges_vec, theEdges, edges() * sizeof *theEdges);
	}

	void swapNodes(node_type *&theNodes) { std::swap(nodes_vec, theNodes); }
	void swapEdges(edge_type *&theEdges) { std::swap(edges_vec, theEdges); }

	void swap(Edges &e) {
		std::swap(n, e.n);
		std::swap(nodes_vec, e.nodes_vec);
		std::swap(edges_vec, e.edges_vec);
	}

	unsigned edges() const { return edges(n); }
	unsigned nodes() const { return n; }
	bool     empty() const { return n == 0; }

	/* No checking of v is done.  Beware! */
	node_type &n(unsigned v) {
		return nodes_vec[v];
	}
	const node_type &n(unsigned v) const {
		return nodes_vec[v];
	}
	void n(unsigned v, const node_type &val) {
		nodes_vec[v] = val;
	}

	typedef edge_type       *nodes_iterator;
	typedef const edge_type *const_nodes_iterator;

	nodes_iterator       nodes_begin()       { return nodes_vec; }
	nodes_iterator       nodes_end()         { return nodes_vec + nodes(); }
	nodes_const_iterator nodes_begin() const { return nodes_vec; }
	nodes_const_iterator nodes_end()   const { return nodes_vec + nodes(); }

	/*
	 * Note that e(u, v) == e(v, u) and e(v, v) == edge_type().
	 * Invalid indexes are likely to cause troubles, se make
	 * sure u < edges() and v < edges().
	 */
	edge_type e(unsigned u, unsigned v) const {
		if (u == v) {
			return edge_type();
		} else if (from < to) {
			std::swap(from, to);
		}
		return edges_vec[size(u) + v];
	}
	void e(unsigned u, unsigned v, const edge_type &val) {
		if (u != v) {
			if (u < v) {
				std::swap(u, v);
			}
			edges_vec[size(u) + v] = val;
		}
	}

	/*
	 * To traverse all the edges knowing which nodes they lead from
	 * and to use loop similar to the below:
	 *
	 * const unsigned n = g.nodes();
	 * Edges::edges_iterator it = g.edges_begin();
	 * for (unsigned from = 1; from < n; ++from) {
	 *     for (unsigned to = 0; to < from; ++to, ++it) {
	 *         ... do stuff ...
	 *     }
	 * }
	 *
	 * Such order is cosistant with e() method above.
	 */
	typedef edge_type       *edges_iterator;
	typedef const edge_type *const_edges_iterator;

	edges_iterator       edges_begin()       { return edges_vec; }
	edges_iterator       edges_end()         { return edges_vec + edges(); }
	edges_const_iterator edges_begin() const { return edges_vec; }
	edges_const_iterator edges_end()   const { return edges_vec + edges(); }

private:
	unsigned count;
	node_type *nodes_vec;
	edge_type *edges_vec;

	static unsigned edges(unsigned n) {
		return n * (n - 1) / 2;
	}
};

}

namespace std {

void swap(graph::Graph &e1, graph::Graph &e2) {
	e1.swap(e2);
}

}

#endif
