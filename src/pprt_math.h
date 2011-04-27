#ifndef PPRT_PPRT_MATH_H
#define PPRT_PPRT_MATH_H

#include <vectormath/cpp/vectormath_aos.h>

using namespace Vectormath::Aos;

typedef Vector3 Normal;

template <typename T>
inline float deg2rad(T deg)
{
	return static_cast<float>(deg * 0.01745329252);
}

__forceinline float cotangeant(const Point3 &a, const Point3 &b, const Point3 &c)
{
	const Vector3 ba(a - b);
	const Vector3 cb(c - b);
	return dot(cb, ba) / length(cross(cb, ba));
}

#endif
