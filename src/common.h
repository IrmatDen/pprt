#ifndef CRT_COMMON_H
#define CRT_COMMON_H

#include <malloc.h>
#include <xmmintrin.h>

#ifndef _MM_ALIGN16
#define _MM_ALIGN16		_declspec(align(16))
#endif

namespace memory
{
	const size_t	alignment = 16;

	template <typename T>
	static	T*		allocate(size_t n=1)
	{
		return static_cast<T*>(_aligned_malloc(sizeof(T) * n, 16));
	}

	template <typename T>
	static	void	deallocate(T *p)
	{
		_aligned_free(p);
	}

	template <typename T>
	static	T*		construct(size_t n=1)
	{
		return n > 1 ? new (allocate<T>(n)) T[n] : new (allocate<T>(n)) T;
	}

	template <typename T, typename Arg1T>
	static	T*		construct(Arg1T a1)
	{
		return new (allocate<T>()) T(a1);
	}

	template <typename T, typename Arg1T, typename Arg2T>
	static	T*		construct(Arg1T a1, Arg2T a2)
	{
		return new (allocate<T>()) T(a1, a2);
	}

	template <typename T, typename Arg1T, typename Arg2T, typename Arg3T>
	static	T*		construct(Arg1T a1, Arg2T a2, Arg3T a3)
	{
		return new (allocate<T>()) T(a1, a2, a3);
	}

	template <typename T>
	static	void	destroy(T *p)
	{
		if (p)
		{
			p->~T();
			deallocate(p);
		}
	}
}

#endif
