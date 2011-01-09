/*
 * src/graph/solver/abstractsolver.hpp
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
#ifndef ABSTRACTSOLVER_HPP
#define ABSTRACTSOLVER_HPP

#include <QWidget>
#include "../scene.hpp"

namespace graph {

namespace solver {

struct AbstractSolver : public QObject
{
	AbstractSolver(QObject *parent, Scene *theScene) : QObject(parent), scene(theScene) { }
	virtual QWidget* createPlayerWidget(QWidget *parent) = 0;

signals:
	void graphChanged();

protected:
	Scene* scene;

private:
	Q_OBJECT
};

}

}

#endif // ABSTRACTSOLVER_HPP
