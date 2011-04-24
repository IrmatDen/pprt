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

// From Geometric Tools for Computer Graphics (Schneider/Eberly)
bool Plane::intersection(const Ray &ray, float &dist, Point3 &intersection) const
{
	const float denominator = dot(ray.direction(), n);
	const float dotNOd		= dot(Vector3(ray.origin), n) + d;

	// Check if ray is parallel
	if (abs(denominator) < 0.001f)
	{
		// Check if ray's origin lies on the plane
		if (dotNOd < 0.001f)
		{
			dist = 0.f;
			intersection = ray.origin;
			return true;
		}
		else
			return false;
	}

	dist = -(dotNOd) / denominator;
	if (dist < 0)
		return false;

	intersection = Point3(ray.origin + dist * ray.direction());
	return true;
}
