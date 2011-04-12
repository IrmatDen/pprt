#include <limits>

#include "aabb.h"

using namespace std;

AABB::AABB()
	: _min(sse::plus_inf), _max(sse::minus_inf)
{
}

bool AABB::hit(const Ray &ray, const float &t) const
{
	const Vector3	l1	= mulPerElem(ray.invDir, (_min - ray.origin)),
					l2	= mulPerElem(ray.invDir, (_max - ray.origin)),
					l1a	= minPerElem(l1, Vector3(sse::plus_inf)),
					l2a	= minPerElem(l2, Vector3(sse::plus_inf)),
					l1b	= maxPerElem(l1, Vector3(sse::plus_inf)),
					l2b	= maxPerElem(l2, Vector3(sse::plus_inf)),
					lmax = maxPerElem(l1a, l2a),
					lmin = minPerElem(l1b, l2b);

	const bool hit = !mask_all(orps(cmplt(lmax.get128(), all_zero()), cmpgt(lmin.get128(), lmax.get128())));

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
