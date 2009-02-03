#include <cmath>
#include <limits>

#include "sphere.h"

bool Sphere::hit(const Ray &ray, double &t) const
{
	const Vec3 dist	= pos - ray.origin;

	const double b		= ray.dir.dot(dist);
	const double d		= b*b - dist.dot(dist) + r*r;

	if (d < 0)
		return false;

	const double t0		= b - sqrt(d);
	const double t1		= b + sqrt(d);

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
