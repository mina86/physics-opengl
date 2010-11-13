/*
 * src/lib/rand.hpp
 * Copyright 2010 by Michal Nazarewicz    <mina86@mina86.com>
 *               and Maciej Swietochowski <m@swietochowski.eu>
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
#ifndef H_RAND_HPP
#define H_RAND_HPP

namespace lib {

void initRand();

/*
 * Even if USE_GSL is defined, libc's rand() is also initialised.
 */
#ifdef USE_GSL

#include <gsl/gsl_rng.h>

extern gsl_rng *gsl_rng_object;

template<typename T>
T rnd(T n) {
	return gsl_rng_uniform(gsl_rng_object) * n;
}

template<typename T>
T rndp(T n) {
	return gsl_rng_uniform_pos(gsl_rng_object) * n;
}

#else

#include <stdlib.h>

template<typename T>
T rnd(T n) {
	return rand() / (RAND_MAX + (T)1.0) * n;
}

template<typename T>
T rndp(T n) {
	int r;
	do {
		r = rand();
	} while (!r);
	return r / (RAND_MAX + (T)1.0) * n;
}

#endif

}

#endif
