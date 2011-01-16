/*
 * src/lib/auto-array.hpp
 * Copyright 2010 by Michal Nazarewicz <mina86@mina86.com>
 *               and Maciej Swietochowski
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
#ifndef H_AUTO_ARRAY_HPP
#define H_AUTO_ARRAY_HPP

namespace lib {

/* Just like auto_ptr, but for arrays. */
template<class T>
struct auto_array {
	explicit auto_array(T *theArray = NULL) : array(theArray) { }

	explicit auto_array(auto_array<T> &theArray)
		: array(theArray.release()) { }

	~auto_array() {
		delete[] array;
	}

	void operator=(auto_array<T> &theArray) {
		reset(theArray.array);
	}

	T &operator*() const {
		return *array;
	}

	T *operator->() const {
		return array;
	}

	T *operator+(int i) const {
		return array + i;
	}

	T &operator[](int i) const {
		return array[i];
	}

	T* get() const {
		return array;
	}

	T *release() {
		T *a = array;
		array = NULL;
		return a;
	}

	void reset(T *theArray = NULL) {
		if (theArray != array) {
			delete[] array;
			array = theArray;
		}
	}

private:
	auto_array(const auto_array<T> &theArray);
	void operator=(const auto_array<T> &theArray);

	T *array;
};

}

#endif
