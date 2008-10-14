#ifndef H_SINTABLE_HPP
#define H_SINTABLE_HPP

namespace mn {

extern float sinTable[360];

void initSinTable(void);

inline float sin(int arg) {
	return sinTable[((arg % 360) + 360) % 360];
}

inline float cos(int arg) {
	return sinTable[(((arg + 90) % 360) + 360) % 360];
}

}

#endif
