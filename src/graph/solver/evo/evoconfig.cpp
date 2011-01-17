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
	successionType("Succession type", successionTypes, EliteOfUnion),
	adjacentFunctionType("Evaluation Func for adjacent", adjacentFunctionTypes, Adj_XPower),
	adjacentParamA("EvF-adjacent, param a", -1000, 1000, 5),
	adjacentParamB("EvF-adjacent, param a", -1000, 1000, 1.5),
	nonAdjacentFunctionType("Evaluation Func for non-adjacent", nonAdjacentFunctionTypes, Nadj_None),
	nonAdjacentParamA("EvF-non-adjacent, param a", -1000, 1000, 0),
	nonAdjacentParamB("EvF-non-adjacent, param a", -1000, 1000, 0),
	allFunctionType("Evaluation Func for all pairs", allFunctionTypes, All_OverX),
	allParamA("EvF-all, param a", -1000, 1000, 10),
	allParamB("EvF-all, param a", -1000, 1000, 0)
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

		CFG_DATA_OFFSET(adjacentFunctionType),
		CFG_DATA_OFFSET(adjacentParamA),
		CFG_DATA_OFFSET(adjacentParamB),
		CFG_DATA_OFFSET(nonAdjacentFunctionType),
		CFG_DATA_OFFSET(nonAdjacentParamA),
		CFG_DATA_OFFSET(nonAdjacentParamB),
		CFG_DATA_OFFSET(allFunctionType),
		CFG_DATA_OFFSET(allParamA),
		CFG_DATA_OFFSET(allParamB),
		~0u,
	};
	return iterator(array);
}


}

}

}
