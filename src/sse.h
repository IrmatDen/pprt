#ifndef CRT_SSE_H
#define CRT_SSE_H

#include <limits>
#include <xmmintrin.h>

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

namespace sse
{
	static const float flt_plus_inf = std::numeric_limits<float>::infinity();
	static const float _MM_ALIGN16
		ps_cst_plus_inf[4]	= {  flt_plus_inf,  flt_plus_inf,  flt_plus_inf,  flt_plus_inf },
		ps_cst_minus_inf[4]	= { -flt_plus_inf, -flt_plus_inf, -flt_plus_inf, -flt_plus_inf };

	static const __m128
		plus_inf	= loadps(ps_cst_plus_inf),
		minus_inf	= loadps(ps_cst_minus_inf);
}

#endif
