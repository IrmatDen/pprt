#include <limits>

#include "aabb.h"
#include "../sse.h"

using namespace std;

AABB::AABB()
{
	_min[0] = numeric_limits<float>::infinity();
	_min[1] = numeric_limits<float>::infinity();
	_min[2] = numeric_limits<float>::infinity();
	
	_max[0] = -numeric_limits<float>::infinity();
	_max[1] = -numeric_limits<float>::infinity();
	_max[2] = -numeric_limits<float>::infinity();
}

bool AABB::hit(const Ray &ray, const float &t) const
{
	const __m128
		idx = set1ps(ray.invDir.x),
		ox	= set1ps(ray.origin.x),
		xl1 = mulps(idx, subps(set1ps(_min[0]), ox)),
		xl2 = mulps(idx, subps(set1ps(_max[0]), ox)),
		xl1a = minps(xl1, sse::plus_inf),  xl2a = minps(xl2, sse::plus_inf),
		xl1b = maxps(xl1, sse::minus_inf), xl2b = maxps(xl2, sse::minus_inf);

	__m128
		lmax = maxps(xl1a,xl2a),
		lmin = minps(xl1b,xl2b);

	const __m128
		idy = set1ps(ray.invDir.y),
		oy	= set1ps(ray.origin.y),
		yl1 = mulps(idy, subps(set1ps(_min[1]), oy)),
		yl2 = mulps(idy, subps(set1ps(_max[1]), oy)),
		yl1a = minps(yl1, sse::plus_inf),  yl2a = minps(yl2, sse::plus_inf),
		yl1b = maxps(yl1, sse::minus_inf), yl2b = maxps(yl2, sse::minus_inf);

	lmax = minps(maxps(yl1a,yl2a), lmax);
	lmin = maxps(minps(yl1b,yl2b), lmin);

	const __m128
		idz = set1ps(ray.invDir.z),
		oz	= set1ps(ray.origin.z),
		zl1 = mulps(idz, subps(set1ps(_min[2]), oz)),
		zl2 = mulps(idz, subps(set1ps(_max[2]), oz)),
		zl1a = minps(zl1, sse::plus_inf),  zl2a = minps(zl2, sse::plus_inf),
		zl1b = maxps(zl1, sse::minus_inf), zl2b = maxps(zl2, sse::minus_inf);

	lmax = minps(maxps(zl1a,zl2a), lmax);
	lmin = maxps(minps(zl1b,zl2b), lmin);

	const bool hit = !mask_all(orps(cmplt(lmax, all_zero()), cmpgt(lmin, lmax)));

	return hit;
}

float AABB::distanceTo(const AABB &other) const
{
	Vec3 thisCenter( (_max[0] - _min[0]) / 2, (_max[1] - _min[1]) / 2, (_max[2] - _min[2]) / 2 );
	Vec3 otherCenter( (other._max[0] - other._min[0]) / 2, (other._max[1] - other._min[1]) / 2, (other._max[2] - other._min[2]) / 2 );
	Vec3 direction = thisCenter - otherCenter;
	return direction.length();
}

float AABB::surfaceArea() const
{
	const Vec3 c(_max[0] - _min[0], _max[1] - _min[1], _max[2] - _min[2]);
	return 2 * (c.x * c.y + c.x * c.z + c.y * c.z);
}

void AABB::mergeFrom(const AABB &v0, const AABB &v1)
{
	*this = v0;
	mergeWith(v1);
}

void AABB::mergeWith(const AABB &other)
{
	_min.asMin(other._min);
	_max.asMax(other._max);
}
