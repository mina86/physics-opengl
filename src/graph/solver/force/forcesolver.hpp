/*
 * src/graph/solver/force/forcesolver.hpp
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
#ifndef FORCESOLVER_HPP
#define FORCESOLVER_HPP

#include "../../../gl/vector.hpp"
#include "../abstractsolver.hpp"
#include "forceconfig.hpp"

#include <vector>

namespace graph {

namespace solver {

struct ForceSolver : public AbstractSolver {
	ForceSolver(QObject *parent, Scene *scene);
	QWidget* createPlayerWidget(QWidget *parent);
	ui::cfg::Data *getConfigData();

protected slots:
	void playNextFrame(unsigned iterations);

signals:
	void graphStable();

private:
	void calculateForce(gl::Vector<float> &r, float q, bool connected);
	void addMiddleForce(gl::Vector<float> &f, const gl::Vector<float> &x);

	struct NodeState {
		NodeState();

		gl::Vector<float> velocity, force;
		unsigned deg;
	};
	typedef std::vector< NodeState > Nodes;

	Nodes nodes;
	force::Config config;

	Q_OBJECT
};

}

}

#endif
