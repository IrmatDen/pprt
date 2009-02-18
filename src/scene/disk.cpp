#include "disk.h"

Disk::Disk(Real radius, const Vec3 &position, const Vec3 &normal)
:n(normal), r(radius), rSquared(r*r)
{
	pos = position;
	d = n.dot(pos);
}

bool Disk::hit(const Ray &ray, Real &t) const
{
	// Code is extracted from Plane::hit(...) to avoid too much vfunc calls.

	Real dist = (d - ray.origin.dot(n)) / (ray.dir.dot(n));

	if(dist > 0 && dist < t)
	{
		Vec3 p = ray.origin + ray.dir * dist;
		p -= pos;
		Real pSquaredL = p.squaredLength();
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
