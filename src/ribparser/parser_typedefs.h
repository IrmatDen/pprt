#ifndef PPRT_PARSER_TYPEDEFS_H
#define PPRT_PARSER_TYPEDEFS_H

#include <boost/spirit/include/classic_file_iterator.hpp>

#include <vector>

#include "../sse.h"
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

    static NonAlignedVec3 fromVector3(const Vector3 &v)
    {
        _MM_ALIGN16 float val[4];
        storeps(v.get128(), val);
        return NonAlignedVec3(val[0], val[1], val[2]);
    }
};
typedef std::vector<NonAlignedVec3> Vec3Array;

inline NonAlignedVec3 operator- (const NonAlignedVec3 &v1, const NonAlignedVec3 &v2)
{
    NonAlignedVec3 ret;
    ret.x = v1.x - v2.x;
    ret.y = v1.y - v2.y;
    ret.z = v1.z - v2.z;
    return ret;
}

inline NonAlignedVec3 operator* (const NonAlignedVec3 &v1, float n)
{
    NonAlignedVec3 ret;
    ret.x = v1.x * n;
    ret.y = v1.y * n;
    ret.z = v1.z * n;
    return ret;
}

inline NonAlignedVec3& operator+= (NonAlignedVec3 &lhs, const NonAlignedVec3 &rhs)
{
    lhs.x += rhs.x;
    lhs.y += rhs.y;
    lhs.z += rhs.z;
    return lhs;
}

inline NonAlignedVec3& operator*= (NonAlignedVec3 &lhs, float rhs)
{
    lhs.x *= rhs;
    lhs.y *= rhs;
    lhs.z *= rhs;
    return lhs;
}

typedef std::vector<int> IntArray;

#endif
