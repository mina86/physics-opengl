#include "evoconfig.hpp"

namespace graph {

namespace solver {

namespace evo {

Data::Data()
	: populationSize("Population size", 1, 1000, 20),
	selectionType("Selection type", selectionTypes, Trivial),
	selectionInteger1("Selection: k-param", 1, 1000, 2),
	crossoverProbability("Crossover probability", 0, 1, 0.1),
	crossoverType("Crossover type", crossoverTypes, Mean),
	mutationProbability("Mutation probability", 0, 1, 0.7),
	mutationSigma("Mutation standard derivation", -10, 10, 0.1),
	successionType("Succession type", successionTypes, EliteOfUnion)
{
	init();
}

ui::cfg::Data::iterator Data::items() const {
	static const unsigned array[] = {
		CFG_DATA_OFFSET(populationSize),

		CFG_DATA_OFFSET(selectionType),
		CFG_DATA_OFFSET(selectionInteger1),

		CFG_DATA_OFFSET(crossoverProbability),
		CFG_DATA_OFFSET(crossoverType),

		CFG_DATA_OFFSET(mutationProbability),
		CFG_DATA_OFFSET(mutationSigma),

		CFG_DATA_OFFSET(successionType),

		~0u,
	};
	return iterator(array);
}


}

}

}
