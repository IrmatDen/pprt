#ifndef CRT_VECTOR3_H
#define CRT_VECTOR3_H

#include <cmath>

#include "../common.h"

#define Epsilon 0.001f
#define OneMinusEpsilon (1.f - Epsilon)

class _MM_ALIGN16 Vec3
{
	// Construction
public:
	Vec3() : x(0), y(0), z(0), w(0)
	{}

	Vec3(float _x, float _y, float _z, float _w = 0) : x(_x), y(_y), z(_z), w(_w)
	{}

	Vec3(float v): x(v), y(v), z(v), w(0)
	{}

	// Algebraic
public:
	inline Vec3			operator+(const Vec3 &v) const			{ return Vec3(x + v.x, y + v.y, z + v.z, w); }
	inline Vec3			operator-(const Vec3 &v) const			{ return Vec3(x - v.x, y - v.y, z - v.z, w); }

	inline Vec3			operator-(float s)  const				{ return Vec3(x - s, y - s, z - s, w); }
	inline Vec3			operator+(float s)  const				{ return Vec3(x + s, y + s, z + s, w); }
	inline Vec3			operator*(float s)  const				{ return Vec3(x * s, y * s, z * s, w); }
	inline Vec3			operator/(float s)  const				{ float div = 1/s; return *this * div; }

	inline Vec3&		operator=(const Vec3 &v)				{ x = v.x; y = v.y; z = v.z; w = v.w; return *this; }

	inline Vec3&		operator+=(const Vec3 &v)				{ x += v.x; y += v.y; z += v.z; return *this; }
	inline Vec3&		operator-=(const Vec3 &v)				{ x -= v.x; y -= v.y; z -= v.z; return *this; }
	inline Vec3&		operator*=(float s)						{ x *= s; y *= s; z *= s; return *this; }
	inline Vec3&		operator/=(float s)						{ float div = 1/s; return *this *= div; }

	inline Vec3			operator-(int) const					{ return Vec3(-x, -y, -z, w); }

	// Geometric
public:
	inline float		dot(const Vec3 &v) const				{ return x * v.x + y * v.y + z * v.z; }
	inline Vec3			cross(const Vec3 &v) const				{ Vec3 out(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x, w); return out; }
	inline float		length() const							{ return sqrt(dot(*this)); }
	inline float		squaredLength() const					{ return dot(*this); }
	inline Vec3&		normalize()								{ float l = length(); if (l > Epsilon) *this /= length(); return *this; }
	inline Vec3			normalized() const						{ Vec3 v(*this); return v.normalize(); }
	inline Vec3&		reflect(const Vec3 &n)					{ *this - n * 2 * dot(n); return *this; }
	inline Vec3			reflect(const Vec3 &n) const			{ Vec3 v = *this - n * 2 * dot(n); return v; }

public:
	union
	{
		struct
		{
			float  x, y, z, w;
		};
		float	xyzw[4];
		__m128	xyzw_ps;
	};
};

inline Vec3 operator*(float s, const Vec3 &v)
{
	return Vec3(v.x * s, v.y * s, v.z * s, v.w);
}

inline Vec3 operator/(float s, const Vec3 &v)
{
	return Vec3(s / v.x, s / v.y, s / v.z, v.w);
}

inline Vec3 operator-(const Vec3 &v)
{
	return Vec3(-v.x, -v.y, -v.z, v.w);
}

#endif
