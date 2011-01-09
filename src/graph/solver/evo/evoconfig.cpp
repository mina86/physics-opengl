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
	mutationEnabled("Enable mutation (normal distribtuion)?", true),
	mutationMu("Mutation mean value", -10, 10, 0),
	mutationSigmasquared("Mutation variance", -10, 10, 0)
{
	init();
}

ui::cfg::Data::iterator Data::items() const {
	static const unsigned array[] = {
		CFG_DATA_OFFSET(populationSize),
		~0u,
	};
	return iterator(array);
}


}

}

}
