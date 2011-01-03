/*
 * src/graph/solver/dummysolver.cpp
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
#include "dummysolver.hpp"

#include <QPushButton>

#include "../vector-rand.hpp"

namespace graph
{

DummySolver::DummySolver(QObject *parent, Scene *scene) :
	AbstractSolver(parent, scene)
{
}

QWidget* DummySolver::createPlayerWidget(QWidget *theParent)
{
	QPushButton* button = new QPushButton(tr("Play once"), theParent);
	connect(button, SIGNAL(clicked()), this, SLOT(playOnce()));
	return button;
}

void DummySolver::playOnce()
{
	Scene::nodes_iterator i;
	for (i = scene->nodes_begin(); i != scene->nodes_end(); ++i)
	{
		Scene::node_type n = *i;
		addRandNormal(n.first, 0.03);
	}

	emit oneStepMade();
}

}
