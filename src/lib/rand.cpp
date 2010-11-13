/*
 * src/lib/rand.cpp
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
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <stdexcept>

#include "rand.hpp"

namespace lib {

#ifdef USE_GSL

gsl_rng *gsl_rng_object;

static void cleanupGSL() {
	gsl_rng_free(gsl_rng_object);
}

static inline void initGSL() {
	gsl_rng_env_setup();
	gsl_rng_object = gsl_rng_alloc(gsl_rng_default);
	if (!gsl_rng_object) {
		throw std::bad_alloc();
	}

	unsigned long seed = gsl_rng_default_seed;
	gsl_rng_set(gsl_rng_object, seed ? seed : time(NULL));

	atexit(cleanupGSL);

}

#else

static inline void initGSL() {
}

#endif

void initRand() {
	srand(time(NULL));
	initGSL();
}

}
