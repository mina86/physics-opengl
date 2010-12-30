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
#include "evolutionarysolver.hpp"
#include "../../lib/rand.hpp"
#include "evolutionarysolverplayer.hpp"

#include <QHBoxLayout>
#include <QPushButton>

#include <cmath>
#include <ctime>
#include <memory>
#include <algorithm>

namespace graph {

EvolutionarySolver::EvolutionarySolver(QObject *parent)
	: AbstractSolver(parent),
	population(0)
{
}

QWidget* EvolutionarySolver::createPlayerWidget(QWidget *theParent)
{
//	QWidget *player = new QWidget(theParent);
//	QHBoxLayout *layout = new QHBoxLayout();
//	QPushButton *button = new QPushButton(tr("One iteration"), theParent);

//	layout->addWidget(button);
//	player->setLayout(layout);

//	connect(button, SIGNAL(clicked()), this, SLOT(makeOneIteration()));

	EvolutionarySolverPlayer *player = new EvolutionarySolverPlayer(theParent);
	connect(player->ui.oneIterationButton, SIGNAL(clicked()), this, SLOT(makeOneIteration()));

	return player;
}

bool EvolutionarySolver::setScene(Scene *theScene)
{
	scene = theScene;
	iterationCount = 0;
	populationSize = 10;
	//TODO: delete graphs if re-setting scene
	population = population_ptr(new std::vector<Graph>(populationSize, *(dynamic_cast<Graph*>(scene))));
	return true;
}

void EvolutionarySolver::makeOneIteration()
{
	int j;
//	while (fabs(bestvold - bestv)/bestvold > 0.00000001)
//	{
		++iterationCount;
		population_ptr offsprings = genetic(reproduce(population));
//		j = 1;
//		for(std::vector<Graph>::const_iterator i = offsprings->begin(); i!= offsprings->end(); ++i)
//		{
//			std::cout << "Offspring " << j << " scored " << evaluate(*i) << std::endl;
//			++j;
//		}
		population = succession(population, offsprings);
//		bestvold = bestv;
//		bestv = best(population);
//	}
//	j = 1;
//	for(std::vector<Graph>::const_iterator i = population->begin(); i!= population->end(); ++i)
//	{
//		std::cout << "Graph " << j << " scored " << evaluate(*i) << std::endl;
//		++j;
//	}
	scene->set(population->front());
	emit graphChanged();
}


EvolutionarySolver::population_ptr EvolutionarySolver::reproduce(population_ptr & population)
{
	return population_ptr(new std::vector<Graph>(*population));
}

EvolutionarySolver::population_ptr EvolutionarySolver::genetic(population_ptr reproduced)
{
	for (std::vector<Graph>::iterator i = reproduced->begin(); i != reproduced->end(); ++i)
	{
		for (Graph::nodes_iterator j = i->nodes_begin(); j != i->nodes_end(); ++j)
		{
			j->x() += gsl_ran_gaussian(lib::gsl_rng_object, 1.0);
			j->y() += gsl_ran_gaussian(lib::gsl_rng_object, 1.0);
			j->z() += gsl_ran_gaussian(lib::gsl_rng_object, 1.0);
		}
	}
	return reproduced;
}

EvolutionarySolver::population_ptr EvolutionarySolver::succession(population_ptr population, population_ptr offsprings)
{
	population_ptr newpopulation(new std::vector<Graph>());
	std::sort(population->begin(), population->end(), strictWeakOrderingOfGraphs);
	std::sort(offsprings->begin(), offsprings->end(), strictWeakOrderingOfGraphs);

	std::vector<Graph>::const_iterator i = population->begin(), j = offsprings->begin();

	while (i != population->end() && j != offsprings->end() && newpopulation->size() < population->size())
	{
		double v1 = evaluate(*i);
		double v2 = evaluate(*j);
		if (v1 <= v2)
		{
			newpopulation->push_back(*i);
			++i;
		}
		else
		{
			newpopulation->push_back(*j);
			++j;
		}
	}

	while (i != population->end() && newpopulation->size() < population->size())
		newpopulation->push_back(*(i++));
	while (j != offsprings->end() && newpopulation->size() < population->size())
		newpopulation->push_back(*(j++));

	return newpopulation;
}

bool EvolutionarySolver::strictWeakOrderingOfGraphs(const Graph first, const Graph second)
{
	double v1 = evaluate(first);
	double v2 = evaluate(second);
	return v1 < v2;
}

double EvolutionarySolver::evaluate(const Graph &g)
{
	double result = 0.0;
	double exclusiveRadius = 1.0;

	Graph::const_nodes_iterator i, j;
	unsigned ki, kj;

	for (i = g.nodes_begin(); i != g.nodes_end(); ++i)
	{
		ki = (i-g.nodes_begin());
		j = i;
		++j;

		for (; j != g.nodes_end(); ++j)
		{
			kj = (j-g.nodes_begin());

			double dist = i->distance(*j);
//			std::cout << "Dist_(" << ki << "," << kj << "): " << dist  << std::endl;
//			std::cout << "Dist2(" << ki << "," << kj << "): " << dist2 << std::endl;

//			// If vertices connected - they should be at specified distance
			if (g.e(ki, kj))
				result += fabs(3.0 - dist);
//			// Otherwise, they should be at another distance
//			else
				result += fabs(6.0 - dist);

//			// Extra penalty for collisions
			if (dist < 2 * exclusiveRadius)
				result += 5 * (2 * exclusiveRadius - dist);
		}
	}
	return result;
}

double EvolutionarySolver::euclidean_distance(const gl::Vector<float> &v1, const gl::Vector<float> &v2)
{
	return sqrt(pow(v1.x() - v2.x(), 2) + pow(v1.y() - v2.y(), 2) + pow(v1.z() - v2.z(), 2));
}


}
