#ifndef CRT_VARVALUE_H
#define CRT_VARVALUE_H

#include "../common.h"

class VarValue
{
public:
	VarValue()
	{
	}

	template<typename T>
	VarValue(const T &v)
	{
		new (storage) T(v);
	}

	template<typename T>
	VarValue& operator=(const T &v)
	{
		new (storage) T(v);
		return *this;
	}

	template<typename T>
	T& cast()
	{
		return *reinterpret_cast<T*>(storage);
	}

	template<typename T>
	operator T&()
	{
		return cast<T>();
	}

private:
	float _MM_ALIGN16 storage[16]; //!< Enough storage for a 4x4 matrix.
};

#endif
