#include "plane.h"

Plane Plane::fromPoints(const Point3 &p0, const Point3 &p1, const Point3 &p2)
{
	Plane ret;
	ret.n = normalize(cross(p1 - p0, p2 - p0));
	ret.d = -dot(Vector3(p0), ret.n);

	return ret;
}

Plane::Plane()
	: d(0.f), n(Vector3::yAxis())
{
}

bool Plane::intersection(const Ray &ray, float &dist, Point3 &intersection) const
{
	dist = (d - dot(Vector3(ray.origin), n)) / (dot(ray.direction(), n));
	if (dist < 0)
		return false;

	intersection = Point3(ray.origin + dist * ray.direction());
	return true;
}
