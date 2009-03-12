#ifndef CRT_AABB_H
#define CRT_AABB_H

#include <xmmintrin.h>

#include "vector3.h"
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
	union
	{
		float	_min[4];
		__m128	_min4;
	};
	union
	{
		float	_max[4];
		__m128	_max4;
	};
};

#endif
