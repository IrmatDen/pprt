#include "disk.h"

Disk::Disk(float radius, const Point3 &position, const Vector3 &normal)
:n(normal), r(radius), rSquared(r*r)
{
	pos = position;
	d = dot(n, Vector3(pos));
}

bool Disk::hit(const Ray &ray, float &t) const
{
	// Code is extracted from Plane::hit(...) to avoid too much vfunc calls.

	float dist = (d - dot(Vector3(ray.origin), n)) / (dot(ray.direction(), n));

	if(dist > 0 && dist < t)
	{
		Vector3 p = Vector3(ray.origin) + ray.direction() * dist;
		p -= Vector3(pos);
		float pSquaredL = lengthSqr(p);
		if (pSquaredL < rSquared)
		{
			t = dist;
			return true;
		}
		else
			return false;
	}

	return false;
}
