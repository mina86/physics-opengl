/*
 * src/graph/solver/evo/evolutionarysolver.hpp
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

#include "../abstractsolver.hpp"
#include "../../../gl/vector.hpp"

#include "evoconfig.hpp"

#include <vector>

namespace graph {

namespace solver {

	struct EvolutionarySolver : public AbstractSolver
	{

		EvolutionarySolver(QObject *parent, Scene *scene);
		virtual QWidget* createPlayerWidget(QWidget *parent);
		ui::cfg::Data* getConfigData() { return &(*config); }

	protected:
		struct Score {
			Score() : valid(false) { }
			mutable bool valid;
			mutable double score;
		};

		typedef std::pair<Graph, Score> individual_t;
		typedef std::vector<individual_t> population_t;
		typedef std::auto_ptr<population_t> population_ptr;

		static Graph&       graph(individual_t &i)            { i.second.valid = false; return i.first; }
		static const Graph& graph(const individual_t &i)      { return i.first; }
		static const Graph& constgraph(const individual_t &i) { return i.first; }

		static double evaluate(const individual_t &i) {
			if (!i.second.valid) {
				i.second.score = evaluate(i.first);
				i.second.valid = true;
			}
			return i.second.score;
		}

		int iterationCount;
		population_ptr population;
		population_ptr reproduce(population_ptr & population);
		population_ptr genetic(population_ptr reproduced);
		population_ptr succession(population_ptr population, population_ptr offsprings);

		static bool strictWeakOrderingOfGraphs(const individual_t &first, const individual_t &second);
		static double evaluate(const Graph &g);

	protected slots:
		void makeOneIteration();

	private:
		evo::Config config;

		Q_OBJECT
	};

}

}

#endif // EVOLUTIONARYSOLVER_HPP
