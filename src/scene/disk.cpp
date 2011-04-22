#include "disk.h"

Disk::Disk(const Matrix4 &obj2world, float radius, const Vector3 &normal)
: Geometry(obj2world), n(normal), r(radius), rSquared(r*r)
{
	d = dot(n, Vector3(objectToWorld.getTranslation()));
}

bool Disk::hit(const Ray &ray, float &t, IntersectionInfo &ii) const
{
	// Code is extracted from Plane::hit(...) to avoid too much vfunc calls.

	/*float dist = (d - dot(Vector3(ray.origin), n)) / (dot(ray.direction(), n));

	if(dist > 0 && dist < t)
	{
		Vector3 p = Vector3(ray.origin) + ray.direction() * dist;
		p -= Vector3(pos);
		float pSquaredL = lengthSqr(p);
		if (pSquaredL < rSquared)
		{
			t = dist;
			ii.normal = n;
			return true;
		}
		else
			return false;
	}*/

	return false;
}
