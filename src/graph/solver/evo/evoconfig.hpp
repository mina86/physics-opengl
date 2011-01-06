#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "../../../ui/abstract-config.hpp"

namespace graph {

namespace solver {

namespace evo {

struct Data;

typedef ui::Config<Data> Config;

struct Data : public ui::cfg::Data {
	ui::cfg::Integer populationSize;

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
