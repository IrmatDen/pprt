#include "aabb.h"


static inline Real min(Real v0, Real v1)
{
	return v0 < v1 ? v0 : v1;
}

static inline Real max(Real v0, Real v1)
{
	return v0 > v1 ? v0 : v1;
}

bool AABB::hit(const Ray &ray, Real &t) const
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

	return t1 >= t0 && t1 >= 0;// && t > (min(t1, min(t0, 0)));
}
