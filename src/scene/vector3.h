#ifndef CRT_VECTOR3_H
#define CRT_VECTOR3_H

#include <cmath>

typedef double Real;

#define Epsilon 0.0000001

class Vec3
{
	// Construction
public:
	Vec3() : x(0), y(0), z(0)
	{}

	Vec3(Real _x, Real _y, Real _z) : x(_x), y(_y), z(_z)
	{}

	Vec3(Real v): x(v), y(v), z(v)
	{}

	// Algebraic
public:
	inline Vec3			operator+(const Vec3 &v) const			{ return Vec3(x + v.x, y + v.y, z + v.z); }
	inline Vec3			operator-(const Vec3 &v) const			{ return Vec3(x - v.x, y - v.y, z - v.z); }

	inline Vec3			operator-(Real s)  const				{ return Vec3(x - s, y - s, z - s); }
	inline Vec3			operator+(Real s)  const				{ return Vec3(x + s, y + s, z + s); }
	inline Vec3			operator*(Real s)  const				{ return Vec3(x * s, y * s, z * s); }
	inline Vec3			operator/(Real s)  const				{ Real div = 1/s; return *this * div; }

	inline Vec3&		operator=(const Vec3 &v)				{ x = v.x; y = v.y; z = v.z; return *this; }

	inline Vec3&		operator+=(const Vec3 &v)				{ x += v.x; y += v.y; z += v.z; return *this; }
	inline Vec3&		operator-=(const Vec3 &v)				{ x -= v.x; y -= v.y; z -= v.z; return *this; }
	inline Vec3&		operator*=(Real s)						{ x *= s; y *= s; z *= s; return *this; }
	inline Vec3&		operator/=(Real s)						{ Real div = 1/s; return *this *= div; }

	inline Vec3			operator-(int) const					{ return Vec3(-x, -y, -z); }

	// Geometric
public:
	inline Real			dot(const Vec3 &v) const				{ return x * v.x + y * v.y + z * v.z; }
	inline Vec3			cross(const Vec3 &v) const				{ Vec3 out(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x); return out; }
	inline Real			length() const							{ return sqrt(dot(*this)); }
	inline Real			squaredLength() const					{ return dot(*this); }
	inline Vec3&		normalize()								{ Real l = length(); if (l > Epsilon) *this /= length(); return *this; }
	inline Vec3			normalized() const						{ Vec3 v(*this); return v.normalize(); }
	inline Vec3&		reflect(const Vec3 &n)					{ *this - n * 2 * dot(n); return *this; }
	inline Vec3			reflect(const Vec3 &n) const			{ Vec3 v = *this - n * 2 * dot(n); return v; }

public:
	Real  x, y, z;
};

inline Vec3 operator*(Real s, const Vec3 &v)
{
	return Vec3(v.x * s, v.y * s, v.z * s);
}

inline Vec3 operator-(const Vec3 &v)
{
	return Vec3(-v.x, -v.y, -v.z);
}

#endif
