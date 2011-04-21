#ifndef PPRT_COLOR_H
#define PPRT_COLOR_H

#include "../sse.h"
#include "../pprt_math.h"

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
	return minElem(c) >= 1 - 0.00001f;
}

#endif
