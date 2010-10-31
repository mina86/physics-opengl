/*
 * src/common/sintable.hpp
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
#ifndef H_SINTABLE_HPP
#define H_SINTABLE_HPP

namespace mn {

#if SINTABLE_OPT_SPACE
extern float sinTable[91];
#else
extern float sinTable[360];
#endif

void initSinTable(void);

inline float sin(int arg) {
#if SINTABLE_OPT_SPACE
	unsigned foo = arg / 90;
	arg %= 90;
	switch (foo % 4) {
	case 0: /*   0 -  89 */ return  sinTable[   arg];
	case 1: /*  90 - 179 */ return  sinTable[90-arg];
	case 2: /* 180 - 269 */ return -sinTable[   arg];
	case 3: /* 270 - 359 */ return -sinTable[90-arg];
	}
#else
	return sinTable[arg % 360];
#endif
}

inline float cos(int arg) {
	return sin(arg + 90);
}

}

#endif
