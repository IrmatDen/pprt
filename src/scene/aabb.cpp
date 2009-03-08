#include "aabb.h"


static inline Real min(Real v0, Real v1)
{
	return v0 < v1 ? v0 : v1;
}

static inline Real max(Real v0, Real v1)
{
	return v0 > v1 ? v0 : v1;
}

bool AABB::hit(const Ray &ray, const Real &t) const
{
	Real l1 = (_min.x - ray.origin.x) * ray.invDir.x;
	Real l2 = (_max.x - ray.origin.x) * ray.invDir.x;
	Real t0 = min(l1, l2);
	Real t1 = max(l1, l2);

	l1 = (_min.y - ray.origin.y) * ray.invDir.y;
	l2 = (_max.y - ray.origin.y) * ray.invDir.y;
	t0 = max(min(l1, l2), t0);
	t1 = min(max(l1, l2), t1);

	l1 = (_min.z - ray.origin.z) * ray.invDir.z;
	l2 = (_max.z - ray.origin.z) * ray.invDir.z;
	t0 = max(min(l1, l2), t0);
	t1 = min(max(l1, l2), t1);

	return t1 >= t0 && t1 >= 0 && t > (min(t1, min(t0, 0)));
}

Real AABB::distanceTo(const AABB &other) const
{
	Vec3 thisCenter		= (_max - _min) / 2;
	Vec3 otherCenter	= (other._max - other._min) / 2;
	Vec3 direction = thisCenter - otherCenter;
	return direction.length();
}

Real AABB::surfaceArea() const
{
	const Vec3 c = _max - _min;
	return 2 * (c.x * c.y + c.x * c.z + c.y * c.z);
}

void AABB::mergeFrom(const AABB &v0, const AABB &v1)
{
	*this = v0;
	mergeWith(v1);
}

void AABB::mergeWith(const AABB &other)
{
	_min.x = min(_min.x, other._min.x);
	_min.y = min(_min.y, other._min.y);
	_min.z = min(_min.z, other._min.z);
	
	_max.x = max(_max.x, other._max.x);
	_max.y = max(_max.y, other._max.y);
	_max.z = max(_max.z, other._max.z);
}
