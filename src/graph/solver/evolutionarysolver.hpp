/*
 * src/graph/solver/evolutionarysolver.hpp
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
#ifndef EVOLUTIONARYSOLVER_HPP
#define EVOLUTIONARYSOLVER_HPP

#include "abstractsolver.hpp"
#include <vector>
#include "../../gl/vector.hpp"

#include "evo/evoconfig.hpp"

namespace graph {

	struct EvolutionarySolver : public AbstractSolver
	{

		EvolutionarySolver(QObject *parent, Scene *scene);
		virtual QWidget* createPlayerWidget(QWidget *parent);

	protected:
		typedef std::auto_ptr<std::vector<Graph> > population_ptr;

		int iterationCount;
		population_ptr population;
		population_ptr reproduce(population_ptr & population);
		population_ptr genetic(population_ptr reproduced);
		population_ptr succession(population_ptr population, population_ptr offsprings);
		static bool strictWeakOrderingOfGraphs(const Graph first, const Graph second);
		static double evaluate(const Graph &g);

		solver::evo::Config config;

	protected slots:
		void makeOneIteration();

	private:
		Q_OBJECT
	};

}

#endif // EVOLUTIONARYSOLVER_HPP
