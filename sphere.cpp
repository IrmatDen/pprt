#include <cmath>
#include <limits>

#include "sphere.h"

bool Sphere::hit(const Ray &ray, float &t) const
{
	const Vec3f dist	= pos - ray.origin;

	const float b		= ray.dir.dot(dist);
	const float d		= b*b - dist.dot(dist) + r*r;

	if (d < 0)
		return false;

	const float t0		= b - sqrtf(d);
	const float t1		= b + sqrtf(d);

	if (t0 > std::numeric_limits<float>::epsilon() && t0 < t)
	{
		t = t0;
		return true;
	}

	if (t1 > std::numeric_limits<float>::epsilon() && t1 < t)
	{
		t = t1;
		return true;
	}

	return false;
}

void Sphere::normalAt(const Vec3f &p, Vec3f &n) const
{
	n = p - pos;
	n.normalize();
}
