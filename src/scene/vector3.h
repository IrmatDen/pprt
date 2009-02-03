#ifndef CRT_VECTOR3_H
#define CRT_VECTOR3_H

#include <cmath>

class Vec3
{
	// Construction
public:
	Vec3() : x(0), y(0), z(0)
	{}

	Vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z)
	{}

	Vec3(float v): x(v), y(v), z(v)
	{}

	// Algebraic
public:
	inline Vec3		operator+(const Vec3 &v) const				{ return Vec3(x + v.x, y + v.y, z + v.z); }
	inline Vec3		operator-(const Vec3 &v) const				{ return Vec3(x - v.x, y - v.y, z - v.z); }
	inline Vec3		operator*(const float &s)  const			{ return Vec3(x * s, y * s, z * s); }
	inline Vec3		operator/(const float &s)  const			{ float div = 1/s; return *this * div; }

	inline Vec3&		operator=(const Vec3 &v)				{ x = v.x; y = v.y; z = v.z; return *this; }

	inline Vec3&		operator+=(const Vec3 &v)				{ x += v.x; y += v.y; z += v.z; return *this; }
	inline Vec3&		operator-=(const Vec3 &v)				{ x -= v.x; y -= v.y; z -= v.z; return *this; }
	inline Vec3&		operator*=(const float &s)				{ x *= s; y *= s; z *= s; return *this; }
	inline Vec3&		operator/=(const float &s)				{ float div = 1/s; return *this *= div; }

	// Geometric
public:
	inline float		dot(const Vec3 &v) const				{ return x * v.x + y * v.y + z * v.z; }
	inline Vec3			cross(const Vec3 &v) const				{ Vec3 out(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x); return out; }
	inline float		length() const							{ return sqrtf(dot(*this)); }
	inline float		squaredLength() const					{ return dot(*this); }
	inline Vec3&		normalize()								{ float l = length(); if (l > 0.00001f) *this /= length(); return *this; }
	inline Vec3			normalized() const						{ Vec3 v(*this); return v.normalize(); }
	inline Vec3&		reflect(const Vec3 &n)					{ *this - n * 2 * dot(n); return *this; }
	inline Vec3			reflect(const Vec3 &n) const			{ Vec3 v = *this - n * 2 * dot(n); return v; }

public:
	float  x, y, z;
};

inline Vec3 operator*(float s, const Vec3 &v)
{
	return Vec3(v.x * s, v.y * s, v.z * s);
}

#endif
