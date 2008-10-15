#ifndef H_VECTOR_HPP
#define H_VECTOR_HPP

namespace mn {

namespace gl {


struct Vector {
	float x, y, z;

	Vector(float theX = 0, float theY = 0, float theZ = 0)
		: x(theX), y(theY), z(theZ) { }

	float length   () const { return std::sqrt(length2()); }
	float length2  () const { return x*x + y*y + z*z; }
	inline float distance (const Vector &v) const;
	inline float distance2(const Vector &v) const;
	void normalize () { *this *= 1 / length(); }


	Vector &operator+=(const Vector &v) {
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	Vector &operator-=(const Vector &v) {
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	Vector &operator*=(float r) {
		x *= r;
		y *= r;
		z *= r;
		return *this;
	}

	Vector &operator/=(float r) {
		return *this *= 1 / r;
	}

	Vector  operator- () const {
		return Vector(-x, -y, -z);
	}

	Vector  operator+ () const {
		return *this;
	}
};


inline Vector operator+(Vector a, const Vector &b) { return a += b; }
inline Vector operator-(Vector a, const Vector &b) { return a -= b; }
inline Vector operator*(Vector a, float r) { return a *= r; }
inline Vector operator*(float r, Vector a) { return a *= r; }
inline Vector operator/(Vector a, float r) { return a /= r; }


inline float Vector::distance (const Vector &v) const {
	return (*this - v).length (); 
}
inline float Vector::distance2(const Vector &v) const {
	return (*this - v).length2(); 
}


}

}

#endif
