#include <limits>

#include "plane.h"

bool Plane::hit(const Ray &ray, float &t) const
{
	float dist = (d - ray.origin.dot(n)) / (ray.direction().dot(n));

	if(dist > 0 && dist < t)
	{
		t = dist;
		return true;
	}

	return false;
}
