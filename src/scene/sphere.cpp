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

void Sphere::fillIntersectionInfo(const Vec3 &p, IntersectionInfo &ii) const
{
	ii.normal = (p - pos)* invr;
	//ii.normal.normalize();

	float invPi = 1.f / 3.141592654f;
	ii.s = ::asinf(ii.normal.x) * invPi + 0.5f;
	ii.t = ::asinf(ii.normal.y) * invPi + 0.5f;
}
