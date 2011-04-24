#include <cmath>

#include "sphere.h"

bool Sphere::hit(const Ray &ray, IntersectionInfo &ii) const
{
	// Make a vector to avoid Point -> Vector casting below
	const Point3 localRayOrigin((worldToObject * ray.origin).get128());
	const Vector3 localRayOriginAsVec(localRayOrigin);
	const Vector3 localRayDir	((worldToObject * ray.direction()).get128());

	const __m128 a = dotps(localRayDir.get128(), localRayDir.get128());
	const __m128 b = mulps(set1ps(2), dotps(localRayDir.get128(), localRayOriginAsVec.get128()));
	const __m128 rv = set1ps(r);
	const __m128 c = subps(dotps(localRayOriginAsVec.get128(), localRayOriginAsVec.get128()), mulps(rv, rv));
	
	// Solve quadratic
	const float ar = a.m128_f32[0];
	const float br = b.m128_f32[0];
	const float cr = c.m128_f32[0];
	const float d = (subps(mulps(b,b), mulps(set1ps(4), mulps(a, c)))).m128_f32[0];
	if (d < 0)
		return false;
	
	const float sqrtD = sqrt(d);
	float q;
	if (br < 0)
		q = -0.5f * (br - sqrtD);
	else
		q = -0.5f * (br + sqrtD);

	float t0	= q / ar;
	float t1	= cr / q;
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

    ii.cs = color;
	
	const float invPi = 1.f / 3.141592654f;
	ii.s = ::asinf(ii.normal.getX()) * invPi + 0.5f;
	ii.t = ::asinf(ii.normal.getY()) * invPi + 0.5f;

	return true;
}
