#include <limits>

#include "plane.h"

bool Plane::hit(const Ray &ray, float &t) const
{
	float dist = (d - dot(ray.origin, n)) / (dot(ray.direction(), n));

	if(dist > 0 && dist < t)
	{
		t = dist;
		return true;
	}

	return false;
}
