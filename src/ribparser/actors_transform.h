#ifndef PPRT_ACTORS_TRANSFORM_H
#define PPRT_ACTORS_TRANSFORM_H

#include "parser_typedefs.h"
#include "../memory.h"

#include <stack>

struct TransformStack
{
	typedef std::stack<Matrix4, std::deque<Matrix4, memory::AllocAlign16<Matrix4>>> XformStack;

	static XformStack	stack;
	static Matrix4		currentTransform;
};

struct identity_a
{
	void operator()(const iterator_t&, const iterator_t&) const;
};

struct translate_a
{
	void operator()(const NonAlignedVec3 &vec) const;
};

struct rotate_a
{
	void operator()(const iterator_t&, const iterator_t&) const;

	static double	angleDegrees;
	static Vector3	axis;
};

struct transformBegin_a
{
	void operator()(const iterator_t&, const iterator_t&) const;
};

struct transformEnd_a
{
	void operator()(const iterator_t&, const iterator_t&) const;
};


#endif
