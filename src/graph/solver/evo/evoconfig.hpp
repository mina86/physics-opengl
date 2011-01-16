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

enum SuccessionType { Straight, EliteOfUnion /*, EliteSumOf*/ };
const char * const successionTypes[] = {
	"Straight", "Elite of union" /*, "Union of elite" */, NULL
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
