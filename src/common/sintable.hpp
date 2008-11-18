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
