/*
 * src/gl/vector.hpp
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
#ifndef H_VECTOR_HPP
#define H_VECTOR_HPP

#include <cmath>

namespace gl {

template<class T = float>
struct Vector {
	typedef T value_type;

	T  x() const { return vector[0]; }
	T &x()       { return vector[0]; }
	T  y() const { return vector[1]; }
	T &y()       { return vector[1]; }
	T  z() const { return vector[2]; }
	T &z()       { return vector[2]; }
	const T *v() const { return vector; }
	T *v() { return vector; }

	Vector() {
		/* Default constructor does not initialise */
	}
	Vector(T theX, T theY, T theZ) {
		set(theX, theY, theZ);
	}
	template<class T2>
	Vector(const Vector<T2> &vec) {
		vector[0] = vec.x();
		vector[1] = vec.y();
		vector[2] = vec.z();
	}

	void set(T theX, T theY, T theZ) {
		vector[0] = theX;
		vector[1] = theY;
		vector[2] = theZ;
	}

	void zero() {
		set(0.0, 0.0, 0.0);
	}

	T length   () const { return std::sqrt(length2()); }
	T length2  () const { return x()*x() + y()*y() + z()*z(); }
	T distance (const Vector &vec) const { return (*this - vec).length (); }
	T distance2(const Vector &vec) const { return (*this - vec).length2(); }
	Vector &normalize() {
		*this /= length();
		return *this;
	}
	Vector &limit(T v) {
		T l = length();
		if (l > v) {
			*this *= v / l;
		}
		return *this;
	}

	Vector &operator+=(const Vector &vec) {
		x() += vec.x();
		y() += vec.y();
		z() += vec.z();
		return *this;
	}

	Vector &operator-=(const Vector &vec) {
		x() -= vec.x();
		y() -= vec.y();
		z() -= vec.z();
		return *this;
	}

	Vector &operator*=(const Vector &vec) {
		return *this = *this * vec;
	}

	Vector &operator*=(T r) {
		x() *= r;
		y() *= r;
		z() *= r;
		return *this;
	}

	Vector &operator/=(T r) {
		return *this *= 1 / r;
	}

	Vector  operator-() const {
		return Vector(-x(), -y(), -z());
	}

	Vector  operator+() const {
		return *this;
	}

private:
	T vector[3];
};

template<class T>
inline Vector<T> operator+(Vector<T> a, const Vector<T> &b) { return a += b; }
template<class T>
inline Vector<T> operator-(Vector<T> a, const Vector<T> &b) { return a -= b; }
template<class T>
inline Vector<T> operator*(Vector<T> a, T r) { return a *= r; }
template<class T>
inline Vector<T> operator*(T r, Vector<T> a) { return a *= r; }
template<class T>
inline Vector<T> operator/(Vector<T> a, T r) { return a /= r; }

template<class T>
inline Vector<T> operator*(const Vector<T> &a, const Vector<T> &b) {
	return Vector<T>(
		a.y() * b.z() - a.z() * b.y(),
		a.z() * b.x() - a.x() * b.z(),
		a.x() * b.y() - a.y() * b.x()
	);
}

}

#endif
