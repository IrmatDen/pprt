#ifndef CRT_COLOR_H
#define CRT_COLOR_H

#include "../sse.h"

typedef Vectormath::Aos::Vector3 Color;

inline void clamp(Color &c)
{
	__m128 old		= c.get128();
	__m128 newCol	= minps(old, sse::all_one);
	newCol			= maxps(newCol, all_zero());
	c = Color(newCol);
}

inline bool isOpaque(const Color &c)
{
	__m128 r = minps(c.get128(), sse::all_one);
	r = shuffle1ps(r, 0, 0, 0, 0);

	float a;
	storess(r, &a);

	return a >= 1 - 0.00001f;
}

#endif
