/*
 * src/lib/sintable.cpp
 * Copyright 2009 by Michal Nazarewicz (mina86/AT/mina86/DOT/com)
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "sintable.hpp"

#include <cmath>

#include "mconst.h"

namespace mn {

namespace lib {

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

}
