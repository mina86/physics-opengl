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

#include "../gl/vector.hpp"

#include <stdexcept>

namespace graph {

struct Graph {
	typedef bool              edge_type;
	typedef gl::Vector<float> node_type;

	explicit Graph(unsigned theN)
		throw(std::invalid_argument, std::bad_alloc) {
		init(theN);
	}
	Graph(const Graph &g)
		throw(std::invalid_argument, std::bad_alloc) {
		init(g.count);
		set(g);
	}
	~Graph() {
		delete[] nodes_vec;
		delete[] edges_vec;
	}

	Graph &operator=(const Graph &g)  throw(std::invalid_argument) {
		set(g);
		return *this;
	}
	void set(const Graph &g) throw(std::invalid_argument);
	void setNodes(const Graph &g) throw(std::invalid_argument);
	void setEdges(const Graph &g) throw(std::invalid_argument);

	void swap(Graph &g) throw(std::invalid_argument);
	void swapNodes(Graph &g) throw(std::invalid_argument);
	void swapEdges(Graph &g) throw(std::invalid_argument);

	void setNodes(const node_type *theNodes);
	void setEdges(const edge_type *theEdges);

	void swapNodes(node_type *&theNodes);
	void swapEdges(edge_type *&theEdges);

	unsigned edges() const { return edges(count); }
	unsigned nodes() const { return count; }
	bool     empty() const { return count == 0; }

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

	typedef node_type       *nodes_iterator;
	typedef const node_type *const_nodes_iterator;

	nodes_iterator       nodes_begin()       { return nodes_vec; }
	nodes_iterator       nodes_end()         { return nodes_vec + nodes(); }
	const_nodes_iterator nodes_begin() const { return nodes_vec; }
	const_nodes_iterator nodes_end()   const { return nodes_vec + nodes(); }

	/*
	 * Note that e(u, v) == e(v, u) and e(v, v) == edge_type().
	 * Invalid indexes are likely to cause troubles, se make
	 * sure u < nodes() and v < nodes().
	 */
	edge_type e(unsigned u, unsigned v) const {
		if (u == v) {
			return edge_type();
		} else if (u < v) {
			std::swap(u, v);
		}
		return edges_vec[edges(u) + v];
	}
	void e(unsigned u, unsigned v, const edge_type &val) {
		if (u != v) {
			if (u < v) {
				std::swap(u, v);
			}
			edges_vec[edges(u) + v] = val;
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
	const_edges_iterator edges_begin() const { return edges_vec; }
	const_edges_iterator edges_end()   const { return edges_vec + edges(); }

private:
	Graph();
	void init(unsigned theN) throw(std::invalid_argument, std::bad_alloc);
	void check_size(const Graph &g) throw(std::invalid_argument);

	unsigned count;
	node_type *nodes_vec;
	edge_type *edges_vec;

	static inline unsigned edges(unsigned n) {
		return n * (n - 1) / 2;
	}
};

}

namespace std {

static inline void swap(graph::Graph &g1, graph::Graph &g2) {
	g1.swap(g2);
}

}

#endif
