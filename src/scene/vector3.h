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
	Vec3() : x(0), y(0), z(0), pad(0)
	{}

	Vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z), pad(0)
	{}

	Vec3(float v): x(v), y(v), z(v), pad(0)
	{}

	// Private ctor (should it really be private?)
private:
	Vec3(__m128 v) : xyz_ps(v) {}

	// Algebraic
public:
	inline Vec3			operator+(const Vec3 &v) const			{ return Vec3(x + v.x, y + v.y, z + v.z); }
	inline Vec3			operator-(const Vec3 &v) const			{ return Vec3(x - v.x, y - v.y, z - v.z); }

	inline Vec3			operator-(float s)  const				{ return Vec3(x - s, y - s, z - s); }
	inline Vec3			operator+(float s)  const				{ return Vec3(x + s, y + s, z + s); }
	inline Vec3			operator*(float s)  const				{ return Vec3(x * s, y * s, z * s); }
	inline Vec3			operator/(float s)  const				{ float div = 1/s; return *this * div; }

	inline Vec3&		operator=(const Vec3 &v)				{ x = v.x; y = v.y; z = v.z; return *this; }

	inline Vec3&		operator+=(const Vec3 &v)				{ x += v.x; y += v.y; z += v.z; return *this; }
	inline Vec3&		operator-=(const Vec3 &v)				{ x -= v.x; y -= v.y; z -= v.z; return *this; }
	inline Vec3&		operator*=(float s)						{ x *= s; y *= s; z *= s; return *this; }
	inline Vec3&		operator/=(float s)						{ float div = 1/s; return *this *= div; }

	inline Vec3			operator-(int) const					{ return Vec3(-x, -y, -z); }

	//! Set the components of 'this' to the (component-wise) minimum between this and v.
	inline void			asMin(const Vec3 &v)					{ xyz_ps = minps(xyz_ps, v.xyz_ps); }
	//! Set the components of 'this' to the (component-wise) minimum between this and v.
	inline void			asMax(const Vec3 &v)					{ xyz_ps = maxps(xyz_ps, v.xyz_ps); }

	// Geometric
public:
	inline float		dot(const Vec3 &v) const				{ return x * v.x + y*v.y + z*v.z; }
	inline Vec3			cross(const Vec3 &v) const				{ Vec3 out(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x); return out; }
	inline float		length() const							{ return sqrt(dot(*this)); }
	inline float		squaredLength() const					{ return dot(*this); }
	inline Vec3&		normalize()								{ *this /= length(); return *this; }
	inline Vec3			normalized() const						{ Vec3 v(*this); return v.normalize(); }
	inline Vec3&		reflect(const Vec3 &n)					{ *this - n * 2 * dot(n); return *this; }
	inline Vec3			reflect(const Vec3 &n) const			{ Vec3 v = *this - n * 2 * dot(n); return v; }

	// Access
public:
	inline float&		operator[](int idx)						{ return xyz[idx]; }
	inline float		operator[](int idx) const				{ return xyz[idx]; }

public:
	union
	{
		struct
		{
			float  x, y, z, pad;
		};
		float	xyz[3];
		__m128	xyz_ps;
	};
};

inline Vec3 operator*(float s, const Vec3 &v)
{
	return Vec3(v.x * s, v.y * s, v.z * s);
}

inline Vec3 operator/(float s, const Vec3 &v)
{
	return Vec3(s / v.x, s / v.y, s / v.z);
}

inline Vec3 operator-(const Vec3 &v)
{
	return Vec3(-v.x, -v.y, -v.z);
}

#endif
