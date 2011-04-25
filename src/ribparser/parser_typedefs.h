#ifndef PPRT_PARSER_TYPEDEFS_H
#define PPRT_PARSER_TYPEDEFS_H

#include <boost/spirit/include/classic_file_iterator.hpp>

#include <vector>

#include "../pprt_math.h"

typedef char char_t;
typedef boost::spirit::classic::file_iterator<char_t> iterator_t;

struct NonAlignedVec3
{
	NonAlignedVec3() {}
	NonAlignedVec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
	float x, y, z;

	operator Vector3() const	{ return Vector3(x, y, z); }
	operator Point3() const		{ return Point3(x, y, z); }
};
typedef std::vector<NonAlignedVec3> Vec3Array;

typedef std::vector<int> IntArray;

#endif
