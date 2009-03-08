#ifndef CRT_AABB_H
#define CRT_AABB_H

#include "vector3.h"
#include "ray.h"

class AABB
{
public:
	bool hit(const Ray &ray, Real &t) const;

	void mergeFrom(const AABB &v0, const AABB &v1);

public:
	Vec3	_min;
	Vec3	_max;
};

#endif
