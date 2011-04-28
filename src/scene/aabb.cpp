#include <limits>

#include "aabb.h"

using namespace std;

AABB::AABB()
	: _min(sse::plus_inf), _max(sse::minus_inf)
{
}

bool AABB::hit(const Ray &ray, float &tmin, float &tmax) const
{
    
	// First, unpack everything
	_MM_ALIGN16 float rayOrigin[4];
	_MM_ALIGN16 float rayInvDir[4];
	_MM_ALIGN16 float aabbMin[4];
	_MM_ALIGN16 float aabbMax[4];

	storeps(ray.origin.get128(), rayOrigin);
	storeps(ray.invDir.get128(), rayInvDir);
	storeps(_min.get128(), aabbMin);
	storeps(_max.get128(), aabbMax);

	// From tbp's Radius code: http://cvs.gna.org/cvsweb/radius/src/rt_render_packet.cc?rev=1.3;cvsroot=radius#l382

	const __m128
		xl1 = mulps(set1ps(rayInvDir[0]), subps(set1ps(aabbMin[0]), set1ps(rayOrigin[0]))),
		xl2 = mulps(set1ps(rayInvDir[0]), subps(set1ps(aabbMax[0]), set1ps(rayOrigin[0]))),
		xl1a = minps(xl1, sse::plus_inf),  xl2a = minps(xl2, sse::plus_inf),
		xl1b = maxps(xl1, sse::minus_inf), xl2b = maxps(xl2, sse::minus_inf);
	
	__m128
		lmax = maxps(xl1a,xl2a),
		lmin = minps(xl1b,xl2b);
	
	const __m128
		yl1 = mulps(set1ps(rayInvDir[1]), subps(set1ps(aabbMin[1]), set1ps(rayOrigin[1]))),
		yl2 = mulps(set1ps(rayInvDir[1]), subps(set1ps(aabbMax[1]), set1ps(rayOrigin[1]))),
		yl1a = minps(yl1, sse::plus_inf),  yl2a = minps(yl2, sse::plus_inf),
		yl1b = maxps(yl1, sse::minus_inf), yl2b = maxps(yl2, sse::minus_inf);
	
	lmax = minps(maxps(yl1a,yl2a), lmax);
	lmin = maxps(minps(yl1b,yl2b), lmin);
	
	const __m128
		zl1 = mulps(set1ps(rayInvDir[2]), subps(set1ps(aabbMin[2]), set1ps(rayOrigin[2]))),
		zl2 = mulps(set1ps(rayInvDir[2]), subps(set1ps(aabbMax[2]), set1ps(rayOrigin[2]))),
		zl1a = minps(zl1, sse::plus_inf),  zl2a = minps(zl2, sse::plus_inf),
		zl1b = maxps(zl1, sse::minus_inf), zl2b = maxps(zl2, sse::minus_inf);
	
	lmax = minps(maxps(zl1a,zl2a), lmax);
	lmin = maxps(minps(zl1b,zl2b), lmin);
	
	const bool hit = !mask_all(orps(cmpltps(lmax, all_zero()),cmpgtps(lmin, lmax)));
	storess(lmin, &tmin);
	storess(lmax, &tmax);
    
	return hit;
}

float AABB::distanceTo(const AABB &other) const
{
	Vector3 thisCenter((_max - _min) * 0.5f);
	Vector3 otherCenter((other._max - other._min) * 0.5f);
	Vector3 direction = thisCenter - otherCenter;
	return length(direction);
}

float AABB::surfaceArea() const
{
	const Vector3 center(_max - _min);
	return 2 * sum(cross(center, center));
	//(c.x * c.y + c.x * c.z + c.y * c.z);
}

void AABB::mergeFrom(const AABB &v0, const AABB &v1)
{
	*this = v0;
	mergeWith(v1);
}

void AABB::mergeWith(const AABB &other)
{
	_min = minPerElem(_min, other._min);
	_max = maxPerElem(_max, other._max);
}
