#include "evoconfig.hpp"

namespace graph {

namespace solver {

namespace evo {

Data::Data()
	: populationSize("Population size", 1, 1000, 20),
	selectionType("Selection type", selectionTypes, Trivial),
	crossoverType("Crossover type", crossoverTypes, NoCrossover),
	successionType("Succession type", successionTypes, EliteOfUnion),
	selectionInteger1("Selection: k-param", 1, 1000, 2),
	mutationProbability("Mutation probability", 0, 1, 0.7),
	mutationSigma("Mutation standard derivation", -10, 10, 0.1)
{
	init();
}

ui::cfg::Data::iterator Data::items() const {
	static const unsigned array[] = {
		CFG_DATA_OFFSET(populationSize),

		CFG_DATA_OFFSET(selectionType),
		CFG_DATA_OFFSET(selectionInteger1),
		CFG_DATA_OFFSET(crossoverType),
		CFG_DATA_OFFSET(successionType),

		CFG_DATA_OFFSET(mutationProbability),
		CFG_DATA_OFFSET(mutationSigma),

		~0u,
	};
	return iterator(array);
}


}

}

}
