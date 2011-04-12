#include <cmath>

#include "sphere.h"

bool Sphere::hit(const Ray &ray, float &t) const
{
	const Vector3 dist	= pos - ray.origin;

	const float b		= dot(ray.direction(), dist);
	const float d		= b*b - dot(dist, dist) + r*r;

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

void Sphere::fillIntersectionInfo(const Vector3 &p, IntersectionInfo &ii) const
{
	ii.normal = (p - pos)* invr;
	//ii.normal.normalize();

	float invPi = 1.f / 3.141592654f;
	ii.s = ::asinf(ii.normal.getX()) * invPi + 0.5f;
	ii.t = ::asinf(ii.normal.getY()) * invPi + 0.5f;
}
