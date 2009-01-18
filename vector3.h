#ifndef CRT_VECTOR3_H
#define CRT_VECTOR3_H

#include <cmath>

template <typename T>
class Vector3
{
public:
	typedef T				type;
	typedef Vector3<T>		value;
	typedef value&			ref;
	typedef const value&	const_ref;
	typedef value*			ptr;

	// Construction
public:
	Vector3() : x(0), y(0), z(0)
	{}

	Vector3(T _x, T _y, T _z) : x(_x), y(_y), z(_z)
	{}

	Vector3(T v): x(v), y(v), z(v)
	{}

	// Algebraic
public:
	value		operator+(const_ref v) const			{ return value(x + v.x, y + v.y, z + v.z); }
	value		operator-(const_ref v) const			{ return value(x - v.x, y - v.y, z - v.z); }
	value		operator*(const type &s)  const			{ return value(x * s, y * s, z * s); }
	value		operator/(const type &s)  const			{ type div = 1/s; return *this * div; }

	ref			operator=(const_ref v)					{ x = v.x; y = v.y; z = v.z; return *this; }

	ref			operator+=(const_ref v)					{ x += v.x; y += v.y; z += v.z; return *this; }
	ref			operator-=(const_ref v)					{ x -= v.x; y -= v.y; z -= v.z; return *this; }
	ref			operator*=(const type &s)				{ x *= s; y *= s; z *= s; return *this; }
	ref			operator/=(const type &s)				{ type div = 1/s; return *this *= div; }

	// Geometric
public:
	type		dot(const_ref v) const					{ return x * v.x + y * v.y + z * v.z; }
	value		cross(const_ref v) const				{ value out(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x); return out; }
	type		length() const;
	ref			normalize()								{ type l = length(); if (l > 0.00001f) *this /= length(); return *this; }
	ref			reflect(const_ref n)					{ *this - 2 * dot(n) * n; return *this; }
	value		reflect(const_ref n) const				{ value v = *this - 2 * dot(n) * n; return v; }

public:
	T x, y, z;
};

template<typename T>
Vector3<T> operator*(const T &s, const Vector3<T> &v)
{
	return Vector3<T>(v.x * s, v.y * s, v.z * s);
}

typedef Vector3<float>	Vec3f;
typedef Vector3<double>	Vec3d;

#endif
