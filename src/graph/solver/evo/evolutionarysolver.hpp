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
#include "../../../lib/isnan.hpp"

#include "evoconfig.hpp"

#include <vector>
#include <limits>

namespace graph {

namespace solver {

struct Individual;

struct EvolutionarySolver : public AbstractSolver {
	EvolutionarySolver(Scene &scene);
	virtual QWidget* createPlayerWidget(QWidget *parent);
	ui::cfg::Data* getConfigData() { return &(*config); }

	typedef std::vector<Individual> Population;
	typedef std::auto_ptr<Population> population_ptr;

protected:
	int iterationCount;
	bool statsShown;
	void updateScene();
	population_ptr population;
	population_ptr reproduce(const Population *population);
	population_ptr genetic(population_ptr reproduced);
	population_ptr succession(population_ptr population, population_ptr offsprings);

protected slots:
	void playNextFrame(unsigned iterations);

private:
	evo::Config config;

	Q_OBJECT
};

}

}

#endif /* EVOLUTIONARYSOLVER_HPP */
