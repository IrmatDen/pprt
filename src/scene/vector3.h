#ifndef CRT_VECTOR3_H
#define CRT_VECTOR3_H

#include <cmath>

class Vec3
{
	// Construction
public:
	Vec3() : x(0), y(0), z(0)
	{}

	Vec3(double _x, double _y, double _z) : x(_x), y(_y), z(_z)
	{}

	Vec3(double v): x(v), y(v), z(v)
	{}

	// Algebraic
public:
	inline Vec3			operator+(const Vec3 &v) const			{ return Vec3(x + v.x, y + v.y, z + v.z); }
	inline Vec3			operator-(const Vec3 &v) const			{ return Vec3(x - v.x, y - v.y, z - v.z); }
	inline Vec3			operator*(double s)  const				{ return Vec3(x * s, y * s, z * s); }
	inline Vec3			operator/(double s)  const				{ double div = 1/s; return *this * div; }

	inline Vec3&		operator=(const Vec3 &v)				{ x = v.x; y = v.y; z = v.z; return *this; }

	inline Vec3&		operator+=(const Vec3 &v)				{ x += v.x; y += v.y; z += v.z; return *this; }
	inline Vec3&		operator-=(const Vec3 &v)				{ x -= v.x; y -= v.y; z -= v.z; return *this; }
	inline Vec3&		operator*=(double s)					{ x *= s; y *= s; z *= s; return *this; }
	inline Vec3&		operator/=(double s)					{ double div = 1/s; return *this *= div; }

	inline Vec3			operator-(int) const						{ return Vec3(-x, -y, -z); }

	// Geometric
public:
	inline double		dot(const Vec3 &v) const				{ return x * v.x + y * v.y + z * v.z; }
	inline Vec3			cross(const Vec3 &v) const				{ Vec3 out(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x); return out; }
	inline double		length() const							{ return sqrt(dot(*this)); }
	inline double		squaredLength() const					{ return dot(*this); }
	inline Vec3&		normalize()								{ double l = length(); if (l > 0.00001) *this /= length(); return *this; }
	inline Vec3			normalized() const						{ Vec3 v(*this); return v.normalize(); }
	inline Vec3&		reflect(const Vec3 &n)					{ *this - n * 2 * dot(n); return *this; }
	inline Vec3			reflect(const Vec3 &n) const			{ Vec3 v = *this - n * 2 * dot(n); return v; }

public:
	double  x, y, z;
};

inline Vec3 operator*(double s, const Vec3 &v)
{
	return Vec3(v.x * s, v.y * s, v.z * s);
}

inline Vec3 operator-(const Vec3 &v)
{
	return Vec3(-v.x, -v.y, -v.z);
}

#endif
