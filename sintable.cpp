#include <cmath>

#include "sintable.hpp"

namespace mn {

float sinTable[360];

void initSinTable(void) {
	for (int i = 0; i < 360; ++i) {
		sinTable[i] = std::sin((float)i * (M_PI / 180));
	}
}

}
