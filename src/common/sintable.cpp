#include "sintable.hpp"

#include <cmath>

#include "mconst.h"

namespace mn {

#if SINTABLE_OPT_SPACE
float sinTable[91];
#else
float sinTable[360];
#endif

void initSinTable(void) {
	for (unsigned i = 0; i < sizeof sinTable / sizeof *sinTable; ++i) {
		sinTable[i] = std::sin(i * MN_PI_180);
	}
}

}
