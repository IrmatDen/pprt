#ifndef CRT_AABB_H
#define CRT_AABB_H

#include "vector3.h"
#include "ray.h"

class AABB
{
public:
	AABB();

	bool	hit(const Ray &ray, const Real &t) const;

	Real	distanceTo(const AABB &other) const;
	Real	surfaceArea() const;

	void	mergeFrom(const AABB &v0, const AABB &v1);
	void	mergeWith(const AABB &other);

public:
	Vec3	_min;
	Vec3	_max;
};

#endif
