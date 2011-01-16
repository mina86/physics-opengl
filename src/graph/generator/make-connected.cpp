/*
 * src/graph/generator/make-connected.cpp
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
#include "make-connected.hpp"

#include "../graph.hpp"

#include <utility>
#include <algorithm>

namespace graph {

namespace generator {

namespace {

struct Node {
	int parent;
	unsigned deg;
};

static unsigned find(Node *vec, unsigned x) {
	if (vec[x].parent < 0) {
		return x;
	} else {
		return vec[x].parent = find(vec, vec[x].parent);
	}
}

static void join(Node *vec, unsigned x, unsigned y) {
	x = find(vec, x);
	y = find(vec, y);

	if (x == y) {
		/* nothing, same set */
	} else if (vec[x].parent > vec[y].parent) {
		/* x's rank is smaller, ie. it's shorter, make it y's child */
		vec[x].parent = y;
	} else {
		if (vec[x].parent == vec[y].parent) {
			--vec[x].parent;
		}
		vec[y].parent = x;
	}
}

}

void makeConnected(Graph &g) {
	unsigned count = g.nodes();
	if (count < 2) {
		return;
	}

	Node *vec = new Node[count];

	/* Initialise */
	{
		Node n = { -1, 0 };
		std::fill(vec, vec + count, n);
	}

	/* Count degree and make sets */
	Graph::edges_iterator it = g.edges_begin();
	unsigned x, y;
	for (x = 1; x < count; ++x) {
		for (y = 0; y < x; ++y, ++it) {
			if (*it) {
				++vec[x].deg;
				++vec[y].deg;
				join(vec, x, y);
			}
		}
	}

	/* Find each set's representative */
	for (x = 0; x < count; ++x) {
		y = find(vec, x);
		if (vec[y].deg < vec[x].deg) {
			vec[x].parent = -1;
			vec[y].parent = x;
		}
	}

	/* Connect connected subgraphs of the graph */
	unsigned prev, i;
	x = prev = find(vec, 0);
	for (i = 1; i < count; ++i) {
		y = find(vec, i);
		if (y != x) {
			g.e(prev, y, true);
			prev = y;
			vec[y].parent = x;
		}
	}
	if (x != prev) {
		g.e(x, prev);
	}

	delete[] vec;
}

}

}
