#ifndef CRT_AABB_H
#define CRT_AABB_H

#include "vector3.h"
#include "ray.h"

class AABB
{
public:
	bool hit(const Ray &ray, Real &t) const;

public:
	Vec3	_min;
	Vec3	_max;
};

#endif
