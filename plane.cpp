#include <limits>

#include "plane.h"

bool Plane::hit(const Ray &ray, float &t) const
{
	float vd = n.dot(ray.dir);
	if (vd == 0)
		return false;

	float v0 = -n.dot(ray.origin) + d;
	float dist = v0 / vd;

	if((dist > 0 || !oneSided) && dist < t)
	{
		t = dist;
		return true;
	}

	return false;
}
