#ifndef CRT_COLOR4_H
#define CRT_COLOR4_H

class Color4
{
public:
	Color4() : r(0), g(0), b(0), a(1)												{}
	Color4(float _r, float _g, float _b, float _a) : r(_r), g(_g), b(_b), a(_a)		{}
	Color4(float v, float _a = 1)	: r(v), g(v), b(v), a(_a)						{}

	Color4&		operator=(const Color4 &v)											{ r=v.r; g=v.g; b=v.b; a=v.a; return *this; }

public:
	Color4&		clamp()
	{
		r = r > 1 ? 1 : r;
		g = g > 1 ? 1 : g;
		b = b > 1 ? 1 : b;
		a = a > 1 ? 1 : a;
		return *this;
	}

public:
	Color4		operator+(const Color4 &v) const									{ return Color4(r+v.r, g+v.g, b+v.b, a+v.a); }
	Color4		operator*(const Color4 &v) const									{ return Color4(r*v.r, g*v.g, b*v.b, a*v.a); }
	Color4		operator*(const float &s) const										{ return Color4(r*s, g*s, b*s, a); }

public:
	Color4&		operator+=(const Color4 &v)											{ r+=v.r; g+=v.g; b+=v.b; a+=v.a; return *this; }
	
public:
	float r, g, b, a;
};

#endif
