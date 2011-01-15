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
#include "evolutionarysolver.hpp"
#include "evolutionarysolverplayer.hpp"
#include "../../../lib/rand.hpp"

#include <algorithm>

namespace graph {

namespace solver {

EvolutionarySolver::EvolutionarySolver(QObject *parent, Scene *scene)
	: AbstractSolver(parent, scene)
{
	iterationCount = 0;
	//TODO: adjust population size when populationSize changed
	individual_t seed(*dynamic_cast<Graph*>(scene), Score());
	population = population_ptr(new population_t(config->populationSize, seed));
}

QWidget* EvolutionarySolver::createPlayerWidget(QWidget *theParent)
{
	evo::EvolutionarySolverPlayer *player = new evo::EvolutionarySolverPlayer(theParent);
	connect(player->ui.oneIterationButton, SIGNAL(clicked()), this, SLOT(makeOneIteration()));
	//connect(&(config->populationSize), SIGNAL(changed(long)), player->ui.populationSize, SLOT(setValue(long)));
	player->ui.populationSize->setValue(config->populationSize);
	return player;
}

void EvolutionarySolver::makeOneIteration()
{
//	int j;
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
	scene->set(constgraph(population->front()));
	emit graphChanged();
}


EvolutionarySolver::population_ptr EvolutionarySolver::reproduce(population_ptr & population)
{
	evo::SelectionType selectionType = (evo::SelectionType)((long)config->selectionType);
	switch (selectionType) {
	case evo::Trivial:
		return population_ptr(new population_t(*population));
		break;
	case evo::RandomUniform:
		{
			population_ptr offsprings(new population_t());
			while (offsprings->size() < population->size()) {
				population_t::size_type idx;
				idx = lib::rnd<population_t::size_type>(population->size()-1);
				offsprings->push_back((*population)[idx]);
			}
			return offsprings;
		}
	case evo::Proportional:
		{
			population_ptr offsprings(new population_t());
			population_t::size_type s = population->size();
			double *distribution = new double[s+1]; //cumulative distribution
			double sum = 0;
			population_t::const_iterator i = population->begin();
			population_t::size_type j = 1;
			while (i != population->end()) {
				double eval = evaluate(*i);
				distribution[j] = eval;
				sum += eval;
				++i;
				++j;
			}
			distribution[0] = 0;
			for (j=1; j <= s; ++j) {
				distribution[j] /= sum;
				distribution[j] += distribution[j-1];
			}
			distribution[s-1] = 1;

			for (j=0; j < s; ++j) {
				double r = lib::rndp<double>(1);
				//TODO: optimization with iterative binary search (for huge populations)
				for (unsigned k = 0; k < s; ++k) {
					if (distribution[k] < r && r <= distribution[k+1]) {
						offsprings->push_back((*population)[k]);
						break;
					}
				}
			}
			delete[] distribution;
			return offsprings;
		}
	case evo::Tournament:
		{
			population_ptr offsprings(new population_t());
			//prepare indexes vector
			individual_t **indivptrs = new individual_t*[population->size()];
			for (population_t::size_type i = 0; i < population->size(); ++i) {
				indivptrs[i] = &((*population)[i]);
			}
			//s-times do k-element tournament
			for (population_t::size_type s = 0; s < population->size(); ++s) {
				for (ui::cfg::Integer::value_type k = 0; k < config->selectionInteger1; ++k) {
					population_t::size_type r = lib::rndp<population_t::size_type>(population->size()-k);
					std::swap(indivptrs[k], indivptrs[r+k]);
				}
				std::sort(indivptrs, indivptrs + config->selectionInteger1, strictWeakOrderingOfGraphPointers);
				offsprings->push_back(individual_t(*(indivptrs[0])));
			}
			return offsprings;
		}
	}
}

EvolutionarySolver::population_ptr EvolutionarySolver::genetic(population_ptr reproduced)
{
	for (population_t::iterator i = reproduced->begin(); i != reproduced->end(); ++i)
	{
		for (Graph::nodes_iterator j = graph(*i).nodes_begin(); j != graph(*i).nodes_end(); ++j)
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
	population_ptr newpopulation(new population_t());
	std::sort(population->begin(), population->end(), strictWeakOrderingOfGraphs);
	std::sort(offsprings->begin(), offsprings->end(), strictWeakOrderingOfGraphs);

	population_t::const_iterator i = population->begin(), j = offsprings->begin();

	while (i != population->end() && j != offsprings->end() && newpopulation->size() < population->size())
	{
		if (evaluate(*i) <= evaluate(*j))
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

bool EvolutionarySolver::strictWeakOrderingOfGraphs(const individual_t &first, const individual_t &second)
{
	return evaluate(first) < evaluate(second);
}
bool EvolutionarySolver::strictWeakOrderingOfGraphPointers(const individual_t * const first, const individual_t * const second)
{
	return evaluate(*first) < evaluate(*second);
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
				result += (6.0/dist);

//			// Extra penalty for collisions
			if (dist < 2 * exclusiveRadius)
				result += 5 * (2 * exclusiveRadius - dist);
		}
	}
	return result;
}

}

}
