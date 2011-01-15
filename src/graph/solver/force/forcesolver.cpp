/*
 * src/graph/solver/force/forcesolver.cpp
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
#include "forcesolver.hpp"

#include "../../vector-rand.hpp"

#include <algorithm>
#include <queue>

namespace graph {

namespace solver {

ForceSolver::ForceSolver(QObject *parent, Scene *scene)
	: AbstractSolver(parent, scene),
	  nodes(dynamic_cast<Graph &>(*scene).nodes()) {
}

QWidget *ForceSolver::createPlayerWidget(QWidget *parent) {
	/* XXX TODO */
	return NULL;
}

ui::cfg::Data *ForceSolver::getConfigData() {
	/* XXX TODO */
	return NULL;
}

static const float repulsion_force = 1.0;
static const float attraction_force = 1.0;
static const float hit_force = 2.5;
static const float dt = 0.001;
static const float desired = 5.0;
static const float damping = 0.9;
static const float energy_limit = 1.0;

float ForceSolver::makeOneIteration() {
	Graph &g = dynamic_cast<Graph &>(*scene);
	const unsigned count = g.nodes();

	float energy = 0.0;

	Graph::edges_iterator it = g.edges_begin();
	for (unsigned from = 1; from < count; ++from) {
		gl::Vector<float> p(g.n(from));
		for (unsigned to = 0; to < from; ++to, ++it) {
			gl::Vector<float> f = calculateForce(p - g.n(to), *it);
			nodes[from].force += f;
			nodes[to].force   -= f;
		}
	}

	Graph::nodes_iterator n = g.nodes_begin();
	for (Nodes::iterator it = nodes.begin(), end = nodes.end();
	     it != end; ++it, ++n) {
		it->velocity += it->force * dt;
		it->velocity *= damping;
		energy += it->velocity.length2();
		*n += (it->force * (dt * 0.5f) + it->velocity) * dt;
		it->force.set(0.0, 0.0, 0.0);
	}

	return energy > energy_limit;
}

gl::Vector<float> ForceSolver::calculateForce(gl::Vector<float> r,
                                              bool connected) {
	float d = r.length(), f;

	if (d < 0.01) {
		/* Nodes are (nearly) at the same position. */
		randVersor(r) *= hit_force;
	} else {
		f = repulsion_force / (d * d);
		if (connected) {
			f -= attraction_force * fabs(d - desired);
		}

		r *= f / d;
	}

	return r;
}

}

}

