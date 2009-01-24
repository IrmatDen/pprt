#include "disk.h"

Disk::Disk(float radius, const Vec3d &position, const Vec3d &normal)
:Plane(normal), r(radius), rSquared(r*r)
{
	pos = position;

	// Infer plane offset from requested position & normal.
	/*Vec3d offset;
	if (n.x != 0)
		offset.x = pos.x / n.x;
	if (n.y != 0)
		offset.y = pos.y / n.y;
	if (n.z != 0)
		offset.z = pos.z / n.z;
	d = offset.length();*/

	d = n.dot(pos);
}

bool Disk::hit(const Ray &ray, double &t) const
{
	// Code is extracted from Plane::hit(...) to avoid too much vfunc calls.

	double dist = (d - ray.origin.dot(n)) / (ray.dir.dot(n));

	if(dist > 0 && dist < t)
	{
		Vec3d p = ray.origin + ray.dir * dist;
		p -= pos;
		double pSquaredL = p.squaredLength();
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
