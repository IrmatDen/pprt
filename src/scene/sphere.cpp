#include <cmath>

#include "sphere.h"

bool Sphere::hit(const Ray &ray, float &t) const
{
	const Vec3 dist	= pos - ray.origin;

	const float b		= ray.direction().dot(dist);
	const float d		= b*b - dist.dot(dist) + r*r;

	if (d < 0 || b < 0)
		return false;
	
	const float t0		= b - sqrt(d);
	const float t1		= b + sqrt(d);

	if (t0 > 0 && t0 < t)
	{
		t = t0;
		return true;
	}

	if (t1 > 0 && t1 < t)
	{
		t = t1;
		return true;
	}

	return false;
}

void Sphere::normalAt(const Vec3 &p, Vec3 &n) const
{
	n = p - pos;
	n.normalize();
}
