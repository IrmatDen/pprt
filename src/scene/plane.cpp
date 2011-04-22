#include <limits>

#include "plane.h"

bool Plane::hit(const Ray &ray, float &t, IntersectionInfo &ii) const
{
	float dist = (d - dot(Vector3(ray.origin), n)) / (dot(ray.direction(), n));

	if(dist < 0 || dist > t || !aabb.hit(ray, dist))
		return false;

	t = dist;

	ii.normal	= n;
	ii.point	= Point3(ray.origin + t * ray.direction());
	return true;
}
