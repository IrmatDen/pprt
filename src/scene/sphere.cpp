#include <cmath>

#include "sphere.h"

bool Sphere::hit(const Ray &ray, IntersectionInfo &ii) const
{
	// Make a vector to avoid Point -> Vector casting below
	const Point3 localRayOrigin((worldToObject * ray.origin).get128());
	const Vector3 localRayOriginAsVec(localRayOrigin);
	const Vector3 localRayDir	((worldToObject * ray.direction()).get128());

	const float a = dot(localRayDir, localRayDir);
	const float b = 2 * dot(localRayDir, localRayOriginAsVec);
	const float c = dot(localRayOriginAsVec, localRayOriginAsVec) - r*r;
	
	// Solve quadratic
	const float d = b*b - 4 * a * c;
	if (d < 0)
		return false;
	
	const float sqrtD = sqrt(d);
	float q;
	if (b < 0)
		q = -0.5f * (b - sqrtD);
	else
		q = -0.5f * (b + sqrtD);

	float t0	= q / a;
	float t1	= c / q;
	if (t0 > t1)
		std::swap(t0, t1);

	if (t0 > ray.maxT || t1 < 0)
		return false;

	float hit = t0;
	if (t0 < 0)
	{
		hit = t1;
		if (hit > ray.maxT)
			return false;
	}
	ray.maxT = hit;

	// Now that we have a hit fill the intersection info structure
	const Point3 localHitP(localRayOrigin + ray.maxT * localRayDir);

	ii.point	= Point3((objectToWorld * localHitP).get128());
	ii.normal	= Vector3(localHitP) * invr;
	
	const float invPi = 1.f / 3.141592654f;
	ii.s = ::asinf(ii.normal.getX()) * invPi + 0.5f;
	ii.t = ::asinf(ii.normal.getY()) * invPi + 0.5f;

	return true;
}
