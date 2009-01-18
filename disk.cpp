#include "disk.h"

Disk::Disk(float radius, const Vec3f &position, const Vec3f &normal)
:Plane(normal), r(radius), rSquared(r*r)
{
	pos = position;

	// Infer plane offset from requested position & normal.
	/*Vec3f offset;
	if (n.x != 0)
		offset.x = pos.x / n.x;
	if (n.y != 0)
		offset.y = pos.y / n.y;
	if (n.z != 0)
		offset.z = pos.z / n.z;
	d = offset.length();*/

	d = n.dot(pos);
}

bool Disk::hit(const Ray &ray, float &t) const
{
	// Code is extracted from Plane::hit(...) to avoid too much vfunc calls.

	float dist = (d - ray.origin.dot(n)) / (ray.dir.dot(n));

	if(dist > 0 && dist < t)
	{
		Vec3f p = ray.origin + ray.dir * dist;
		p -= pos;
		float pSquaredL = p.squaredLength();
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
