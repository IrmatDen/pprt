#include <limits>

#include "aabb.h"

using namespace std;

// turn those verbose intrinsics into something readable.
#define set1ps(v)			_mm_set1_ps((v))
#define loadps(mem)			_mm_load_ps((const float * const)(mem))
#define storess(ss,mem)		_mm_store_ss((float * const)(mem),(ss))
#define minss				_mm_min_ss
#define maxss				_mm_max_ss
#define minps				_mm_min_ps
#define maxps				_mm_max_ps
#define mulps				_mm_mul_ps
#define subps				_mm_sub_ps
#define orps(v1,v2)			_mm_or_ps((v1),(v2))
#define cmplt(v1,v2)		_mm_cmplt_ps((v1),(v2))
#define cmpgt(v1,v2)		_mm_cmpgt_ps((v1),(v2))
#define all_zero()			_mm_setzero_ps()
#define movemask(ps)		_mm_movemask_ps((ps))
#define mask_all(ps)		(movemask(ps) == 15)


static const float flt_plus_inf = numeric_limits<float>::infinity();
static const float _MM_ALIGN16
	ps_cst_plus_inf[4]	= {  flt_plus_inf,  flt_plus_inf,  flt_plus_inf,  flt_plus_inf },
	ps_cst_minus_inf[4]	= { -flt_plus_inf, -flt_plus_inf, -flt_plus_inf, -flt_plus_inf };

static const __m128
	plus_inf	= loadps(ps_cst_plus_inf),
	minus_inf	= loadps(ps_cst_minus_inf);

AABB::AABB()
{
	_min[0] = numeric_limits<float>::infinity();
	_min[1] = numeric_limits<float>::infinity();
	_min[2] = numeric_limits<float>::infinity();
	_min[3] = 0;
	
	_max[0] = -numeric_limits<float>::infinity();
	_max[1] = -numeric_limits<float>::infinity();
	_max[2] = -numeric_limits<float>::infinity();
	_max[3] = 0;
}

bool AABB::hit(const Ray &ray, const Real &t) const
{
	float _MM_ALIGN16 fPos[4]	= {(float)ray.origin.x, (float)ray.origin.y, (float)ray.origin.z, 0};
	float _MM_ALIGN16 fInvDir[4]= {(float)ray.invDir.x, (float)ray.invDir.y, (float)ray.invDir.z, 0};

	const __m128
		idx = set1ps(fInvDir[0]),
		ox	= set1ps(fPos[0]),
		xl1 = mulps(idx, subps(set1ps(_min[0]), ox)),
		xl2 = mulps(idx, subps(set1ps(_max[0]), ox)),
		xl1a = minps(xl1, plus_inf),  xl2a = minps(xl2, plus_inf),
		xl1b = maxps(xl1, minus_inf), xl2b = maxps(xl2, minus_inf);

	__m128
		lmax = maxps(xl1a,xl2a),
		lmin = minps(xl1b,xl2b);

	const __m128
		idy = set1ps(fInvDir[1]),
		oy	= set1ps(fPos[1]),
		yl1 = mulps(idy, subps(set1ps(_min[1]), oy)),
		yl2 = mulps(idy, subps(set1ps(_max[1]), oy)),
		yl1a = minps(yl1, plus_inf),  yl2a = minps(yl2, plus_inf),
		yl1b = maxps(yl1, minus_inf), yl2b = maxps(yl2, minus_inf);

	lmax = minps(maxps(yl1a,yl2a), lmax);
	lmin = maxps(minps(yl1b,yl2b), lmin);

	const __m128
		idz = set1ps(fInvDir[2]),
		oz	= set1ps(fPos[2]),
		zl1 = mulps(idz, subps(set1ps(_min[2]), oz)),
		zl2 = mulps(idz, subps(set1ps(_max[2]), oz)),
		zl1a = minps(zl1, plus_inf),  zl2a = minps(zl2, plus_inf),
		zl1b = maxps(zl1, minus_inf), zl2b = maxps(zl2, minus_inf);

	lmax = minps(maxps(zl1a,zl2a), lmax);
	lmin = maxps(minps(zl1b,zl2b), lmin);

	const bool hit = !mask_all(orps(cmplt(lmax, all_zero()), cmpgt(lmin, lmax)));

	return hit;
}

Real AABB::distanceTo(const AABB &other) const
{
	Vec3 thisCenter		= (_max - _min) / 2;
	Vec3 otherCenter	= (other._max - other._min) / 2;
	Vec3 direction = thisCenter - otherCenter;
	return direction.length();
}

Real AABB::surfaceArea() const
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
	_min4 = _mm_min_ps(_min4, other._min4);
	_max4 = _mm_max_ps(_max4, other._max4);
}
