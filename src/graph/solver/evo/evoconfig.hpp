#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "../../../ui/abstract-config.hpp"

namespace graph {

namespace solver {

namespace evo {

enum SelectionType { Trivial, RandomUniform, Proportional, Tournament };
const char * const selectionTypes[] = {
	"Trivial", "Random uniform", "Proportional", "Tournament(k)", NULL
};

enum CrossoverType { Mean, Arithmetic, Interchange };
const char * const crossoverTypes[] = {
	"Arithmeatic (mean)", "Arithmetic (random)", "Interchange", NULL
};

enum SuccessionType { Straight, EliteOfUnion, EliteSumOf };
const char * const successionTypes[] = {
	"Straight", "Elite of union", "Union of elite", NULL
};

enum AdjacentFunctionType { Adj_None, Adj_XPower };
const char * const adjacentFunctionTypes[] = {
	"none", "abs(a-x)^b", NULL
};

enum NonAdjacentFunctionType { Nadj_None, Nadj_OverX };
const char * const nonAdjacentFunctionTypes[] = {
	"none", "a/(x-b)", NULL
};

enum AllFunctionType { All_None, All_XPower, All_OverX };
const char * const allFunctionTypes[] = {
	"none", "abs(a-x)^b", "a/(x-b)", NULL
};

struct Data;

typedef ui::Config<Data> Config;

struct Data : public ui::cfg::Data {
	ui::cfg::Integer populationSize;

	ui::cfg::List selectionType;
	ui::cfg::Integer selectionInteger1;

	ui::cfg::Real crossoverProbability;
	ui::cfg::List crossoverType;

	// Mutation X ~ N(0, sigma^2)
	ui::cfg::Real mutationProbability;
	ui::cfg::Real mutationSigma;

	ui::cfg::List successionType;

	ui::cfg::List adjacentFunctionType;
	ui::cfg::Real adjacentParamA;
	ui::cfg::Real adjacentParamB;

	ui::cfg::List nonAdjacentFunctionType;
	ui::cfg::Real nonAdjacentParamA;
	ui::cfg::Real nonAdjacentParamB;

	ui::cfg::List allFunctionType;
	ui::cfg::Real allParamA;
	ui::cfg::Real allParamB;

	virtual struct iterator items() const;

private:
	Data();

	friend struct ui::Config<Data>;

	Q_OBJECT
};

}

}

}

#endif // CONFIG_HPP
