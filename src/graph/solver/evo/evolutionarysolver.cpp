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

#include "../../randomise-graph.hpp"
#include "../../vector-rand.hpp"
#include "../../../lib/auto-array.hpp"
#include "../../../lib/rand.hpp"
#include "../../../ui/playercontrolwidget.hpp"

#include <algorithm>

namespace graph {

namespace solver {


static double evaluate(const Graph &g, const evo::Data &data);


struct Individual {
	Individual(const Graph &theGraph)
		: graph_(theGraph), score_(std::numeric_limits<double>::quiet_NaN()) { }

	Graph       &graph() {
		score_ = std::numeric_limits<double>::quiet_NaN();
		return graph_;
	}
	const Graph &graph() const { return graph_; }
	const Graph &constgraph() { return graph_; }

	double score(const evo::Data &data) const {
		if (isnan(score_)) {
			score_ = evaluate(graph_, data);
		}
		return score_;
	}

private:
	Individual();

	Graph graph_;
	mutable double score_;
};


namespace {

struct Compare {
	Compare(const evo::Data &theData) : data(theData) { }

	double operator()(const Individual &ind) { return ind.score(data); }

	bool operator()(const Individual &a, const Individual &b) {
		return a.score(data) < b.score(data);
	}

	bool operator()(const Individual *a, const Individual *b) {
		return (*this)(*a, *b);
	}

private:
	const evo::Data &data;
};

}


EvolutionarySolver::EvolutionarySolver(Scene &scene)
	: AbstractSolver(scene), iterationCount(), statsShown() {
	population.reset(new Population(config->populationSize, Individual(scene)));

	GraphRandomiser rnd;

	rnd::Data &data = rnd.getConfigData();
	data.relative = false;
	data.normal = false;
	data.overwrite = true;
	data.r = 100.0;

	for (Population::iterator it = population->begin(), end = population->end();
	     ++it != end; ) {
		rnd.randomise(it->graph());
	}

	std::sort(population->begin(), population->end(), Compare(*config));
	updateScene();
}

QWidget* EvolutionarySolver::createPlayerWidget(QWidget *theParent)
{
	ui::PlayerControlWidget *player = new ui::PlayerControlWidget(theParent);
	connect(player, SIGNAL(newFrameNeeded(uint)), this, SLOT(playNextFrame(uint)));
	return player;
}


EvolutionarySolver::population_ptr EvolutionarySolver::reproduce(const Population *population)
{
	Population::size_type newPopulationSize = config->populationSize;
	switch (config->selectionType) {
	case evo::Trivial:
	{
		population_ptr offsprings(new Population());
		while (offsprings->size() < newPopulationSize) {
			for (Population::const_iterator i = population->begin();
				 i != population->end() && offsprings->size() < newPopulationSize;
				 ++i) {
				offsprings->push_back(*i);
			}
		}
		return offsprings;
	}
	break;
	case evo::RandomUniform:
	{
		population_ptr offsprings(new Population());
		while (offsprings->size() < newPopulationSize) {
			Population::size_type idx;
			idx = lib::rnd<Population::size_type>(population->size()-1);
			offsprings->push_back((*population)[idx]);
		}
		return offsprings;
	}
	case evo::Proportional:
	{
		population_ptr offsprings(new Population());
		Population::size_type s = population->size();
		lib::auto_array<double> a_distribution(new double[s+1]);
		double *distribution = a_distribution.get(); //cumulative distribution
		Population::const_iterator i = population->begin();
		Population::size_type j = 1;
		Compare evaluator(*config);
		double worst = 0;
		//At first, distribution[j] is score
		while (i != population->end()) {
			double eval = evaluator(*i);
			distribution[j] = eval;
			worst = std::max(worst, eval);
			++i;
			++j;
		}
		worst += 1;
		double sum = 0;
		//Now distribution[j] is distance from the 1.0-worse-than-worst score
		//sum is sum of such distances
		for (j=1; j <= s; ++j) {
			distribution[j] = worst - distribution[j];
			sum += distribution[j];
		}
		//It's easy to calculate distribution, once distance is subject to maximization
		distribution[0] = 0;
		for (j=1; j <= s; ++j) {
			distribution[j] /= sum;
			distribution[j] += distribution[j-1];
		}
		distribution[s-1] = 1;

		for (j=0; j < newPopulationSize; ++j) {
			double r = lib::rndp<double>(1);
			//TODO: optimization with iterative binary search (for huge populations)
			for (unsigned k = 0; k < s; ++k) {
				if (distribution[k] < r && r <= distribution[k+1]) {
					offsprings->push_back((*population)[k]);
					break;
				}
			}
		}
		return offsprings;
	}
	case evo::Tournament:
	{
		population_ptr offsprings(new Population());

		//prepare pointers vector
		lib::auto_array<const Individual *> a_indivptrs(new const Individual*[population->size()]);
		const Individual **indivptrs = a_indivptrs.get();
		for (Population::size_type i = 0; i < population->size(); ++i) {
			indivptrs[i] = &((*population)[i]);
		}

		//population->size()-times do k-element tournament
		//pick tournament members by swapping them to beginning of pointers vector
		//resolve tournament by sorting front of that vector on evaluation score
		for (Population::size_type s = 0; s < newPopulationSize; ++s) {
			for (ui::cfg::Integer::value_type k = 0; k < config->selectionInteger1; ++k) {
				Population::size_type r = lib::rndp<Population::size_type>(population->size()-k);
				std::swap(indivptrs[k], indivptrs[r+k]);
			}
			std::sort(indivptrs, indivptrs + config->selectionInteger1, Compare(*config));
			offsprings->push_back(Individual(*(indivptrs[0])));
		}
		return offsprings;
	}
	default:
		return population_ptr(new Population(*population));
	}
}

EvolutionarySolver::population_ptr EvolutionarySolver::genetic(population_ptr reproduced)
{
	if (config->crossoverProbability > 0) {
		std::random_shuffle(reproduced->begin(), reproduced->end(), lib::rnd<long>);
		Population::size_type numberOfPairs = reproduced->size() / 2;
		switch (config->crossoverType) {
		case evo::Mean:
		{
			float middlepoint = 0.5;
			float middlepoint2 = 1 - middlepoint;
			for (Population::size_type i = 0; i < numberOfPairs; ++i) {
				if (lib::rndp<float>(1.0) <= config->crossoverProbability) {
					Graph &g1 = (*reproduced)[2*i  ].graph();
					const Graph &g2 = (*reproduced)[2*i+1].constgraph();
					Graph::nodes_iterator ni1 = g1.nodes_begin();
					Graph::const_nodes_iterator ni2 = g2.nodes_begin();
					while (ni1 != g1.nodes_end()) {
						ni1->set(ni1->x()*middlepoint + ni2->x()*middlepoint2,
								 ni1->y()*middlepoint + ni2->y()*middlepoint2,
								 ni1->z()*middlepoint + ni2->z()*middlepoint2);
						++ni1;
						++ni2;
					}
				}
			}
		}
		break;
		case evo::Arithmetic:
		{
			float middlepoint = lib::rnd<float>(1.0);
			float middlepoint2 = 1 - middlepoint;
			for (Population::size_type i = 0; i < numberOfPairs; ++i) {
				if (lib::rndp<float>(1.0) <= config->crossoverProbability) {
					Graph &g1 = (*reproduced)[2*i  ].graph();
					const Graph &g2 = (*reproduced)[2*i+1].constgraph();
					Graph::nodes_iterator ni1 = g1.nodes_begin();
					Graph::const_nodes_iterator ni2 = g2.nodes_begin();
					while (ni1 != g1.nodes_end()) {
						ni1->set(ni1->x()*middlepoint + ni2->x()*middlepoint2,
								 ni1->y()*middlepoint + ni2->y()*middlepoint2,
								 ni1->z()*middlepoint + ni2->z()*middlepoint2);
						++ni1;
						++ni2;
					}
				}
			}
		}
		break;
		case evo::Interchange:
		{
			for (Population::size_type i = 0; i < numberOfPairs; ++i) {
				if (lib::rndp<float>(1.0) <= config->crossoverProbability) {
					Graph &g1 = (*reproduced)[2*i  ].graph();
					Graph &g2 = (*reproduced)[2*i+1].graph();
					Graph::nodes_iterator ni1 = g1.nodes_begin();
					Graph::nodes_iterator ni2 = g2.nodes_begin();
					unsigned j=0;
					unsigned breakingpoint = g1.nodes() * lib::rndp(1.0);
					while (j < breakingpoint) {
						Graph::node_type tmp(*ni1);
						(*ni1) = (*ni2);
						(*ni2) = tmp;
						++ni1;
						++ni2;
						++j;
					}
				}
			}
		}
		break;
		default:
			/* no crossover */
			break;
		}
	}

	//Mutation
	if (config->mutationProbability > 0) {
		for (Population::iterator i = reproduced->begin(); i != reproduced->end(); ++i)
		{
			float r = lib::rndp<float>(1.0);
			if (r <= config->mutationProbability) {
				for (Graph::nodes_iterator j = i->graph().nodes_begin(),
						 end = i->graph().nodes_end(); j != end; ++j) {
					graph::addRandNormal(*j, config->mutationSigma);
				}
			}
		}
	}
	return reproduced;
}

EvolutionarySolver::population_ptr EvolutionarySolver::succession(population_ptr population, population_ptr offsprings)
{
	Population::size_type newPopulationSize = config->populationSize;
	switch (config->successionType) {
	case evo::Straight:
		std::sort(offsprings->begin(), offsprings->end(), Compare(*config));
		return offsprings;
	case evo::EliteOfUnion:
	{
		population_ptr newpopulation(new Population());
		std::sort(population->begin(), population->end(), Compare(*config));
		std::sort(offsprings->begin(), offsprings->end(), Compare(*config));

		Population::const_iterator i = population->begin(), j = offsprings->begin();

		Compare evaluator(*config);
		while (i != population->end() && j != offsprings->end() && newpopulation->size() < newPopulationSize)
		{
			if (evaluator(*i) <= evaluator(*j))
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

		while (i != population->end() && newpopulation->size() < newPopulationSize)
			newpopulation->push_back(*(i++));
		while (j != offsprings->end() && newpopulation->size() < newPopulationSize)
			newpopulation->push_back(*(j++));

		return newpopulation;
	}
	case evo::EliteSumOf:
	{
		population_ptr newpopulation(new Population());
		std::sort(population->begin(), population->end(), Compare(*config));
		std::sort(offsprings->begin(), offsprings->end(), Compare(*config));

		Population::const_iterator i[2];
		i[0] = population->begin();
		i[1] = offsprings->begin();
		unsigned j = 0;

		while (i[0] != population->end() && i[1] != offsprings->end() && newpopulation->size() < newPopulationSize)
		{
			newpopulation->push_back(*(i[j]));
			++(i[j]);
			j = 1 - j;
		}

		std::sort(newpopulation->begin(), newpopulation->end(), Compare(*config));
		return newpopulation;
	}
	default:
		/* no succession -- should never happen */
		return population;
	}
}

void EvolutionarySolver::playNextFrame(unsigned iterations)
{
	for (;;) {
		if (!statsShown) {
			fprintf(stderr, "%u\t%.20g\n", iterationCount,
			        population->front().score(*config));
			statsShown = true;
		}

		if (!iterations) {
			break;
		}

		--iterations;
		++iterationCount;
		statsShown = false;

		population = succession(population, genetic(reproduce(population.get())));
	}
	updateScene();
}

void EvolutionarySolver::updateScene()
{
	scene.set(population->front().constgraph());
	emit graphChanged();
}


static double evaluate(const Graph &g, const evo::Data &data) {
	double result = 0.0;
	double exclusiveRadius = 1.0;

	Graph::const_nodes_iterator i, j;
	unsigned ki, kj;

	for (i = g.nodes_begin(); i != g.nodes_end(); ++i)
	{
		ki = i - g.nodes_begin();
		j = i;
		++j;

		for (; j != g.nodes_end(); ++j)
		{
			kj = (j-g.nodes_begin());

			double dist = i->distance(*j) + 0.00001; //avoid division by zero

			// If vertices connected - they should be at specified distance
			if (g.e(ki, kj)) {
				switch(data.adjacentFunctionType) {
				case evo::Adj_XPower:
					result += pow(fabs(data.adjacentParamA - dist), data.adjacentParamB);
					break;
				default:
					break;
				}
			} else {
				switch(data.nonAdjacentFunctionType) {
				case evo::Nadj_OverX:
					result += data.nonAdjacentParamA / (dist - data.nonAdjacentParamB);
					break;
				default:
					break;
				}
			}

			switch(data.allFunctionType) {
			case evo::All_XPower:
				result += pow(fabs(data.allParamA - dist), data.allParamB);
				break;
			case evo::All_OverX:
				result += data.allParamA / (dist - data.allParamB);
				break;
			default:
				break;
			}

			// Extra penalty for collisions
			if (dist < 2 * exclusiveRadius)
				result += 5 * (2 * exclusiveRadius - dist);
		}
	}
	return result;
}

}

}
