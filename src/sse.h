#ifndef PPRT_SSE_H
#define PPRT_SSE_H

#include <limits>
#include <xmmintrin.h>
#include <smmintrin.h>

#include <vectormath/cpp/vectormath_aos.h>

using namespace Vectormath::Aos;

#define set1ps(v)			_mm_set1_ps((v))
#define loadps(mem)			_mm_load_ps((const float * const)(mem))
#define storess(ss,mem)		_mm_store_ss((float * const)(mem),(ss))

#define addps				_mm_add_ps
#define subps				_mm_sub_ps
#define mulps				_mm_mul_ps
#define dotps(a,b)			_mm_dp_ps((a), (b), 0x71).m128_f32[0]

#define minss				_mm_min_ss
#define maxss				_mm_max_ss
#define minps				_mm_min_ps
#define maxps				_mm_max_ps

#define orps(v1,v2)			_mm_or_ps((v1),(v2))
#define cmplt(v1,v2)		_mm_cmplt_ps((v1),(v2))
#define cmpgt(v1,v2)		_mm_cmpgt_ps((v1),(v2))
#define cmpge(v1,v2)		_mm_cmpge_ps((v1),(v2))

#define movemask(ps)		_mm_movemask_ps((ps))
#define mask_all(ps)		(movemask(ps) == 15)

#define shuffleps_mask(x,y,z,w)		((x)|((y)<<2)|((z)<<4)|((w)<<6))
#define shuffle1ps(ps1,x,y,z,w)		_mm_shuffle_ps((ps1),(ps1),shuffleps_mask(x,y,z,w))

#define all_zero()			_mm_setzero_ps()

namespace sse
{
	static const float flt_plus_inf = std::numeric_limits<float>::infinity();

	static const __m128
		plus_inf	= set1ps(flt_plus_inf),
		minus_inf	= set1ps(-flt_plus_inf),
		all_one		= set1ps(1.f);

	inline __m128 loadXYZW(float x, float y, float z, float w)
	{
		const float _MM_ALIGN16 a[4] = { x, y, z, w };
		return loadps(a);
	}
}

#endif
