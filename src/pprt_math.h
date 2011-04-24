#ifndef PPRT_PPRT_MATH_H
#define PPRT_PPRT_MATH_H

#include <vectormath/cpp/vectormath_aos.h>

using namespace Vectormath::Aos;

__forceinline float cotangeant(const Point3 &a, const Point3 &b, const Point3 &c)
{
	const Vector3 ba(a - b);
	const Vector3 cb(c - b);
	return dot(cb, ba) / length(cross(cb, ba));
}

#endif
