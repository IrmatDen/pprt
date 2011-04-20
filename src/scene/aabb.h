#ifndef PPRT_AABB_H
#define PPRT_AABB_H

#include "ray.h"

class _MM_ALIGN16 AABB
{
public:
	AABB();

	bool	hit(const Ray &ray, const float &t) const;

	float	distanceTo(const AABB &other) const;
	float	surfaceArea() const;

	void	mergeFrom(const AABB &v0, const AABB &v1);
	void	mergeWith(const AABB &other);

public:
	Point3	_min;
	Point3	_max;
};

#endif
