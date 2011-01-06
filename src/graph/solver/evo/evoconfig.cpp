#include "evoconfig.hpp"

namespace graph {

namespace solver {

namespace evo {

Data::Data()
	: populationSize("Population size", 1, 1000, 20)
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
