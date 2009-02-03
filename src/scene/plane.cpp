#include <limits>

#include "plane.h"

bool Plane::hit(const Ray &ray, double &t) const
{
	double dist = (d - ray.origin.dot(n)) / (ray.dir.dot(n));

	if(dist > 0 && dist < t)
	{
		t = dist;
		return true;
	}

	return false;
}
