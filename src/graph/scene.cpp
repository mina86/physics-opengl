/*
 * src/graph/scene.cpp
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
#include "scene.hpp"

#include <stdio.h>

#include <algorithm>

#include "../gl/glwidget.hpp"
#include "../gl/vector-print.hpp"

namespace graph {

void Scene::commonInit() {
	unsigned theN = nodes();
	char buffer[32];

	nodes_ext_vec = new Node[theN];

	Graph::nodes_iterator it = Graph::nodes_begin();
	Node *node = nodes_ext_vec;
	for (unsigned i = 0; i < theN; ++i, ++node) {
		sprintf(buffer, "#%u", i);
		node->name.assign(buffer);
		node->loadedPosition = *it;
		++it;
		std::fill_n(node->color, 4, (float)1);
	}
}

Scene::Scene(unsigned theN) throw(std::invalid_argument, std::bad_alloc)
	: Graph(theN) {
	commonInit();
}

Scene::Scene(const Graph &g) : Graph(g) {
	commonInit();
}

Scene::~Scene() {
	delete[] nodes_ext_vec;
}

Scene::Scene(const Scene &s) : AbstractScene(), Graph(s) {
	unsigned theN = nodes();
	nodes_ext_vec = new Node[theN];

	const Node *in = s.nodes_ext_vec;
	Node *out = nodes_ext_vec;
	do {
		*out++ = *in++;
	} while (--theN);
}

Scene::ptr Scene::clone() {
	return ptr(new Scene(*this));
}


void Scene::initializeGL(const gl::Widget &) {
	static const GLfloat direction[] = { 0, 0, -1, 0 };

	static const GLfloat colors[][4] = {
		{ 0.0, 0.0, 0.7, 1.0 },
		{ 0.5, 0.7, 0.7, 1.0 },
		{ 0.7, 0.5, 0.5, 1.0 },
		{ 0.5, 0.7, 0.5, 1.0 },
	};

	for (unsigned i = 0; i < sizeof colors / sizeof *colors; ++i) {
		glEnable(GL_LIGHT0 + i);
		glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, colors[i]);
	}

	glLightfv(GL_LIGHT0, GL_POSITION, gl::Widget::zeros);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, direction);
	glLightf (GL_LIGHT0, GL_SPOT_CUTOFF, 5.0f);
}

void Scene::drawScene(const gl::Widget &gl) {
	static const GLfloat positions[][4] = {
		{ 0.00, 0.00, 0.00, 0.00 }, /* this is skipped */
		{ 0.00, +MAX_DISTANCE, 0.00, 1.00 },
		{ 0.00, -MAX_DISTANCE, 0.00, 1.00 },
		{ 0.00, 0.00, 0.00, 1.00 },
	};

	for (unsigned i = 1; i < sizeof positions / sizeof *positions; ++i) {
		glLightfv(GL_LIGHT0 + i, GL_POSITION, positions[i]);
	}

	edges_iterator eit = edges_begin();
	nodes_iterator nit = nodes_begin();
	const unsigned nn = nodes();

	for (unsigned from = 0; from < nn; ++from, ++nit) {
		const node_type node1 = *nit;
		const bool visible = gl.isInFront(node1.first);

		glPushMatrix();

		glTranslatef(node1.first.x(), node1.first.y(), node1.first.z());

		if (visible) {
			gl.sphere(1.0, node1.first, node1.second.color, node1.second.name);
		}

		for (unsigned to = 0; to < from; ++to, ++eit) {
			if (!*eit) {
				continue;
			}

			const node_type node2 = n(to);
			if (visible || gl.isInFront(node2.first)) {
				gl.connection(0.2, node2.first - node1.first,
				              node1.second.color, node2.second.color);
			}
		}

		glPopMatrix();
	}
}

void Scene::updateState(unsigned ticks, float dt) {
	(void)ticks; (void)dt;
}

void Scene::save(std::ostream &out) throw(std::ios_base::failure) {
	saveNodes(out);
	saveEdges(out);
}

void Scene::saveNodes(std::ostream &out) throw(std::ios_base::failure) {
	nodes_iterator it = nodes_begin(), e = nodes_end();
	for (; it != e; ++it) {
		out << "node ";

		if ((*it).second.name.find('"', 0) == std::string::npos) {
			out << '"' << (*it).second.name << '"';
		} else if ((*it).second.name.find('\'', 0) == std::string::npos) {
			out << '\'' << (*it).second.name << '\'';
		}

		out << " @ " << (*it).first << " color "
			<< (*it).second.color[0] << ' '
			<< (*it).second.color[1] << ' '
			<< (*it).second.color[2] << '\n';
	}
}

void Scene::saveEdges(std::ostream &out) throw(std::ios_base::failure) {
	edges_iterator it = edges_begin();
	const unsigned n = nodes();

	for (unsigned u = 1; u < n; ++u) {
		for (unsigned v = 0; v < u; ++v, ++it) {
			if (*it) {
				out << "edge " << u << ' ' << v << '\n';
			}
		}
	}
}

}
