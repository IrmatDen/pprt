#ifndef CRT_VMSTACK_H
#define CRT_VMSTACK_H

#ifdef _DEBUG
#include <stack>
#include <string>
#endif

#include "../common.h"
#include "symtab.h"

// WARNING: this stack only works for types that do NOT require construction nor destruction!
class VMStack
{
public:
	VMStack(size_t stackSize)
		: currentTop(0)
	{
		stackArea = memory::allocate<unsigned char>(stackSize);
	}

	~VMStack()
	{
		memory::deallocate(stackArea);
	}

	__forceinline void reset()
	{
		currentTop = 0;

#ifdef _DEBUG
		while (!stackedTypeNames.empty())
			stackedTypeNames.pop();
#endif
	}

	template <typename T>
	__forceinline void push(const T &v)
	{
		memcpy(stackArea + currentTop, &v, sizeof(T));
		currentTop += roundSizeToNextAlign16<T>();

#ifdef _DEBUG
		stackedTypeNames.push(typeid(T).name());
#endif
	}

	template <typename T>
	__forceinline void pop(T &out)
	{
#ifdef _DEBUG
		const std::string tName(typeid(T).name());
		assert(!stackedTypeNames.empty() && stackedTypeNames.top() == tName);
		stackedTypeNames.pop();

		assert(currentTop > 0);
#endif

		currentTop -= roundSizeToNextAlign16<T>();
		memcpy(&out, stackArea + currentTop, sizeof(T));
	}

	template <typename T>
	__forceinline T& pop()
	{
#ifdef _DEBUG
		const std::string tName(typeid(T).name());
		assert(!stackedTypeNames.empty() && stackedTypeNames.top() == tName);
		stackedTypeNames.pop();

		assert(currentTop > 0);
#endif

		currentTop -= roundSizeToNextAlign16<T>();

		return *reinterpret_cast<T*>(stackArea + currentTop);
	}

private:
	template <typename T>
	__forceinline size_t roundSizeToNextAlign16()
	{
		return (sizeof(T) & ~0xF) + (static_cast<int>((sizeof(T) & 0xF) != 0) * 16);
	}

private:
	unsigned char	*	stackArea;
	size_t				currentTop;

#ifdef _DEBUG
	std::stack<std::string> stackedTypeNames;
#endif
};

#endif
