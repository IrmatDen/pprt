#ifndef CRT_COLOR_H
#define CRT_COLOR_H

#include "vector3.h"

class Color
{
public:
	Color()								: r(0), g(0), b(0)								{}
	Color(float _r, float _g, float _b)	: r(_r), g(_g), b(_b)							{}
	Color(const Vec3 &v)				: r((float)v.x), g((float)v.y), b((float)v.z)	{}
	Color(float v)						: r(v), g(v), b(v)								{}

	inline Color&		operator=(const Color &v)										{ r=v.r; g=v.g; b=v.b; return *this; }

public:
	inline Color&		clamp()
	{
		r = r > 1 ? 1 : r;
		g = g > 1 ? 1 : g;
		b = b > 1 ? 1 : b;
		return *this;
	}

public:
	inline Color		operator+(const Color &v) const							{ return Color(r+v.r, g+v.g, b+v.b); }
	inline Color		operator+(float s) const								{ return Color(r+s, g+s, b+s); }
	inline Color		operator*(const Color &v) const							{ return Color(r*v.r, g*v.g, b*v.b); }
	inline Color		operator*(float s) const								{ return Color(r*s, g*s, b*s); }
	inline Color		operator/(float s) const								{ float div = 1 / s; return *this * div; }
	
public:
	inline Color&		operator+=(const Color &v)								{ r+=v.r; g+=v.g; b+=v.b; return *this; }
	inline Color&		operator/=(float s)										{ r/=s; g/=s; b/=s; return *this; }
	
public:
	float r, g, b;
};

inline Color operator *(float s, const Color &v)
{
	return v * s;
}

inline Color operator -(float s, const Color &v)
{
	return Color(s - v.r, s - v.g, s - v.b);
}

inline bool isOpaque(const Color &c)
{
	return c.r >= OneMinusEpsilon && c.g >= OneMinusEpsilon && c.b >= OneMinusEpsilon;
}

#endif
