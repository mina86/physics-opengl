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
namespace graph {

struct Node;

struct Groph {
	typedef bool edge_type;

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

	void set(unsigned theN, Node *theNodes = 0, edge_type *theEdges = 0);
	void set(const Edges &e) { set(e.n, e.edges, e.nodes); }
	Edges &operator=(const Edges &e) { set(e); return *this; }

	void swap(Edges &e) {
		std::swap(n, e.n);
		std::swap(nodes_vec, e.nodes_vec);
		std::swap(edges_vec, e.edges_vec);
	}

	unsigned edges() const { return edges(n); }
	unsigned nodes() const { return n; }
	bool     empty() const { return n == 0; }

	/*
	 * If x is invalid (>= nodes()) NULL is returned, no exception is
	 * thrown.
	 */
	Node *n(unsigned x) {
		return x >= nodes() ? 0 : nodes_vec[x];
	}
	const Node *n(unsigned x) const {
		return x >= nodes() ? 0 : nodes_vec[x];
	}

	typedef edge_type       *nodes_iterator;
	typedef const edge_type *const_nodes_iterator;

	nodes_iterator       nodes_begin()       { return nodes_vec; }
	nodes_iterator       nodes_end()         { return nodes_vec + nodes(); }
	nodes_const_iterator nodes_begin() const { return nodes_vec; }
	nodes_const_iterator nodes_end()   const { return nodes_vec + nodes(); }

	/*
	 * Note that e(x, y) == e(y, x) and e(x, x) == edge_type().
	 * Moreover, those methods do not throw exceptions or anything if
	 * invalid node numbers (>= nodes()) are given.
	 */
	const edge_type &e(unsigned from, unsigned to) const;
	void e(unsigned from, unsigned to, const edge_type &v) const;

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
	Nodes *nodes_vec;
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
