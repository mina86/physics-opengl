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
#include "../../../gl/glconfig.hpp"
#include "../../../ui/playercontrolwidget.hpp"

namespace graph {

namespace solver {

ForceSolver::ForceSolver(Scene &scene)
	: AbstractSolver(scene), first(true),
	  nodes(new NodeState[scene.nodes()]) {

	const unsigned count = scene.nodes();

	Graph::edges_iterator it = scene.edges_begin();
	for (unsigned from = 1; from < count; ++from) {
		for (unsigned to = 0; to < from; ++to, ++it) {
			if (*it) {
				++nodes[from].deg;
				++nodes[to].deg;
			}
		}
	}
}

ForceSolver::~ForceSolver() {
	delete[] nodes;
}

QWidget *ForceSolver::createPlayerWidget(QWidget *parent) {
	ui::PlayerControlWidget *player = new ui::PlayerControlWidget(parent);
	connect(player, SIGNAL(newFrameNeeded(uint)),
	        this, SLOT(playNextFrame(uint)));
	connect(this, SIGNAL(graphStable()), player, SLOT(pause()));
	return player;
}

ui::cfg::Data *ForceSolver::getConfigData() {
	return &*config;
}

void ForceSolver::playNextFrame(unsigned iterations) {
	const unsigned count = scene.nodes();

	if (first) {
		first = false;
		if (config->positionDisord >= 0.0001) {
			Graph::nodes_iterator it = scene.Graph::nodes_begin(),
				end = scene.Graph::nodes_end();
			for (; it != end; ++it) {
				addRandNormal(*it, config->positionDisord);
			}
		}
	}

	while (iterations--) {
		const float dt = config->dt;

		Graph::edges_iterator it = scene.edges_begin();
		for (unsigned from = 1; from < count; ++from) {
			const gl::Vector<float> p(scene.Graph::n(from));
			const unsigned deg = nodes[from].deg;
			for (unsigned to = 0; to < from; ++to, ++it) {
				gl::Vector<float> f = p - scene.Graph::n(to);
				calculateForce(f, (float)deg * nodes[to].deg, *it);
				nodes[from].force += f;
				nodes[to].force   -= f;
			}
		}

		float energy = 0.0;
		Graph::nodes_iterator n = scene.Graph::nodes_begin();
		for (NodeState *it = nodes, *end = nodes + count;
		     it != end; ++it, ++n) {
			addMiddleForce(it->force, *n);
			it->velocity += it->force * dt;
			it->velocity.limit(config->velocityLimit);

			energy += it->velocity.length2();
			*n += ((it->force * (dt * 0.5f) + it->velocity) * dt).limit(config->moveLimit);

			it->velocity *= config->damping;
			it->force.zero();
		}

		if (config->massCenterFix) {
			scene.moveBy(-scene.massCenter());
		}

		if (energy < 0.0001) {
			emit graphStable();
			break;
		}
	}

	emit graphChanged();
}

void ForceSolver::calculateForce(gl::Vector<float> &r, float q, bool connected) {
	float d = r.length();

	if (d < 0.01) {
		/* Nodes are (nearly) at the same position. */
		randVersor(r) *= config->hitForce;
	} else {
		float f = config->repulsionForce / (d * d);
		if (config->attractionMultiply) {
			f *= q;
		}

		if (connected) {
			f -= config->attractionForce * (d - config->desiredDistance);
		}

		r *= f / d;
	}
}

void ForceSolver::addMiddleForce(gl::Vector<float> &f, const gl::Vector<float> &x) {
	float l = x.length(), d;
	if (l > MAX_DISTANCE - 0.5) {
		d = MAX_DISTANCE - 0.5;
	} else {
		d = MAX_DISTANCE - l;
	}

	if (l > 1.0) {
		d = 1 / d;
		f += x * (config->middleForce * d * d / l);
	}
}

ForceSolver::NodeState::NodeState()
	: velocity(0.0, 0.0, 0.0), force(0.0, 0.0, 0.0), deg(0) {
}


}

}
