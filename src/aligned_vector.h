#ifndef PPRT_ALIGNED_VECTOR_H
#define PPRT_ALIGNED_VECTOR_H

#include "memory.h"

#include <algorithm>

template <typename T, typename Allocator = memory::AllocAlign16<T> >
class AlignedVector
{
public:
	typedef AlignedVector<T, Allocator>		SelfType;

	typedef typename Allocator::template rebind<T>::other Alloc;

	typedef typename Alloc::reference		reference;
	typedef typename Alloc::const_reference const_reference;
	typedef typename Alloc::size_type		size_type;

	typedef typename Alloc::pointer			pointer;
	typedef typename Alloc::const_pointer	const_pointer;

	typedef pointer			iterator;
	typedef const_pointer	const_iterator;

public:
	AlignedVector()
		: _data(nullptr), _size(0), _capacity(16)
	{
		_data = _allocator.allocate(_capacity);
	}

	AlignedVector(const SelfType &other)
		: _data(nullptr), _size(other._size), _capacity(other._capacity)
	{
		assert(_capacity != 0);
		assert(_size <= _capacity);

		_data = _allocator.allocate(_capacity);

		for (size_type idx = 0; idx != _size; idx++)
			_allocator.construct(&_data[idx], other._data[idx]);
	}

	AlignedVector(SelfType &&other)
		: _data(other._data), _size(other._size), _capacity(other._capacity)
	{
		assert(_capacity != 0);
		assert(_size <= _capacity);

		other._data = nullptr;
		other._size = 0;
		other._capacity = 0;
	}

	~AlignedVector()
	{
		if (_data)
		{
			for (size_type idx = 0; idx != _size; idx++)
				_allocator.destroy(&_data[idx]);

			_allocator.deallocate(_data);
		}
	}

	SelfType& operator=(const SelfType &other)
	{
		if (this != &other)
		{
			assert(other._capacity != 0);

			if (_capacity < other._capacity)
				resize(other._capacity);

			assert(_size <= _capacity);
			assert(other._size <= other._capacity);

			for (size_type idx = other._size; idx < _size; idx++)
				_allocator.destroy(&_data[idx]);

			_size = other._size;
			for (size_type idx = 0; idx != _size; idx++)
				_allocator.construct(&_data[idx], other._data[idx]);
		}

		return *this;
	}

	SelfType& operator=(SelfType &&other)
	{
		if (this != &other)
		{
			_data = other._data;
			_size = other._size;
			_capacity = other._capacity;
		
			assert(other._capacity != 0);
			assert(_size <= _capacity);

			other._data = nullptr;
			other._size = 0;
			other._capacity = 0;
		}

		return *this;
	}

	const_reference	at(size_type idx) const
	{
		assert(idx < _size);
		return _data[idx];
	}

	reference at(size_type idx)
	{
		assert(idx < _size);
		return _data[idx];
	}

	iterator begin()
	{
		return _data;
	}

	iterator end()
	{
		return _data + _size;
	}
	
	const_iterator begin() const
	{
		return _data;
	}

	const_iterator end() const
	{
		return _data + _size;
	}

	void push_back(const_reference elem)
	{
		assert (_capacity != 0);
		assert (_size <= _capacity);

		if (_size == _capacity)
			resize(_capacity * 2);

		_allocator.construct(&_data[_size], elem);
		_size++;
	}

	void resize(size_type newCapacity)
	{
		T *newData = _allocator.allocate(newCapacity);
		for (size_type idx = 0; idx != _size; idx++)
		{
			_allocator.construct(&newData[idx], _data[idx]);
			_allocator.destroy(&_data[idx]);
		}
		_allocator.deallocate(_data);
		_data = newData;
		_capacity = newCapacity;
	}

	size_type size() const
	{
		return _size;
	}

	void swap(SelfType &other)
	{
		if (this != &other)
		{
			T*			tmpData = _data;
			size_type	tmpSize = _size;
			size_type	tmpCapacity	= _capacity;

			_data = other._data;
			_size = other._size;
			_capacity = other._capacity;

			other._data = tmpData;
			other._size = tmpSize;
			other._capacity = tmpCapacity;
		}
	}

	void swap(SelfType &&other)
	{
		if (this != &other)
		{
			for (size_type idx = 0; idx != _size; idx++)
				_allocator.destroy(&_data[idx]);
			_allocator.deallocate(_data);

			_data = other._data;
			_size = other._size;
			_capacity = other._capacity;

			other._data = nullptr;
			other._size = 0;
			other._capacity = 0;
		}
	}

private:
	Alloc			_allocator;
	T			*	_data;
	size_type		_size;
	size_type		_capacity;
};

#endif
