#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "../../../ui/abstract-config.hpp"

namespace graph {

namespace solver {

namespace evo {

enum SelectionType { Trivial, RandomUniform, Proportional, Tournament };
const char * const selectionTypes[] = {
	"Trivial", "Random uniform", "Proportional", "Tournament(k)"
};

enum CrossoverType { NoCrossover, Arithmetic, Interchange };
const char * const crossoverTypes[] = {
	"None", "Arithmetic", "Interchange"
};

enum SuccessionType { Straight, EliteOfUnion /*, EliteSumOf*/ };
const char * const successionTypes[] = {
	"Straight", "Elite of union" /*, "Union of elite" */
};

struct Data;

typedef ui::Config<Data> Config;

struct Data : public ui::cfg::Data {
	ui::cfg::Integer populationSize;
	ui::cfg::List selectionType;
	ui::cfg::List crossoverType;
	ui::cfg::List successionType;

	ui::cfg::Integer selectionInteger1;

	// Mutation X ~ N(mu, sigma^2)
	ui::cfg::Bool mutationEnabled;
	ui::cfg::Real mutationMu;
	ui::cfg::Real mutationSigmasquared;

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
