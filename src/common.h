#ifndef PPRT_COMMON_H
#define PPRT_COMMON_H

#include "sse.h"

#include <boost/pool/singleton_pool.hpp>

#include <tbb/enumerable_thread_specific.h>

#ifndef _MM_ALIGN16
#define _MM_ALIGN16		_declspec(align(16))
#endif

namespace memory
{
	const size_t	alignment = 16;

	template <typename T>
	static	T*		allocate(size_t n=1)
	{
		return static_cast<T*>(_mm_malloc(sizeof(T) * n, alignment));
	}

	template <typename T>
	static	void	deallocate(T *p)
	{
		_mm_free(p);
	}

	template <typename T>
	static	T*		construct(size_t n=1)
	{
		return n > 1 ? new (allocate<T>(n)) T[n] : new (allocate<T>(n)) T;
	}

	template <typename T, typename Arg1T>
	static	T*		construct(const Arg1T &a1)
	{
		return new (allocate<T>()) T(a1);
	}

	template <typename T, typename Arg1T, typename Arg2T>
	static	T*		construct(const Arg1T &a1, const Arg2T &a2)
	{
		return new (allocate<T>()) T(a1, a2);
	}

	template <typename T, typename Arg1T, typename Arg2T, typename Arg3T>
	static	T*		construct(const Arg1T &a1, const Arg2T &a2, const Arg3T &a3)
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

	template <typename T>
	class AllocAlign16
	{
	public:
		// type definitions
		typedef T				value_type;
		typedef T*				pointer;
		typedef const T*		const_pointer;
		typedef T&				reference;
		typedef const T&		const_reference;
		typedef std::size_t		size_type;
		typedef std::ptrdiff_t	difference_type;

		// rebind allocator to type U
		template <typename U>
		struct rebind
		{
			typedef AllocAlign16<U> other;
		};

		// return address of values
		pointer address (reference value) const
		{
			return &value;
		}
		const_pointer address (const_reference value) const
		{
			return &value;
		}

		/* constructors and destructor
		* - nothing to do because the allocator has no state
		*/
		AllocAlign16() throw()
		{
		}
		AllocAlign16(const AllocAlign16& src) throw()
		{
		}
		template <typename U>
		AllocAlign16 (const AllocAlign16<U> &src) throw()
		{
		}
		~AllocAlign16() throw()
		{
		}

		// return maximum number of elements that can be allocated
		size_type max_size () const throw()
		{
			// estimate maximum array size
			size_type maxSize = (size_type)(-1) / sizeof (T);
			return (0 < maxSize ? maxSize : 1);
		}

		// allocate but don't initialize num elements of type T
		pointer allocate (size_type num, const void* = 0)
		{
			return memory::allocate<T>(num);
		}

		// initialize elements of allocated storage p with value value
		void construct (pointer p, const T& value)
		{
			// initialize memory with placement new
			new((void*)p)T(value);
		}

		// destroy elements of initialized storage p
		void destroy (pointer p)
		{
			// destroy objects by calling their destructor
			p->~T();
		}

		// deallocate storage p of deleted elements
		void deallocate (pointer p, size_type /*num*/)
		{
			memory::deallocate(p);
		}

		// deallocate storage p of deleted elements
		void deallocate (pointer p)
		{
			memory::deallocate(p);
		}
	};

	// return that all specializations of this allocator are interchangeable
	template <typename T1, typename T2>
	bool operator== (const AllocAlign16<T1>&, const AllocAlign16<T2>&) throw()
	{
		return true;
	}
	template <typename T1, typename T2>
	bool operator!= (const AllocAlign16<T1>&, const AllocAlign16<T2>&) throw()
	{
		return false;
	}


	//---------------------------------------------------------------
	// Pool related stuff
	//---------------------------------------------------------------

	struct PoolAllocAlign16
	{
		typedef size_t		size_type;
		typedef ptrdiff_t	difference_type;

		static char* malloc(const size_type bytes)
		{
			return allocate<char>(bytes);
		}

		static void free(char *block)
		{
			deallocate(block);
		}
	};

	typedef boost::pool<memory::PoolAllocAlign16>			AlignedPool;
	typedef tbb::enumerable_thread_specific<AlignedPool*>	UCharPool;

	template <size_t RequestedSize, size_t NextSize = 32>
	struct UCharPoolCreator
	{
		static AlignedPool* create()
		{
			return new AlignedPool(sizeof(unsigned char) * RequestedSize, NextSize);
		}
	};
}

#endif
