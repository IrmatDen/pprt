#ifndef CRT_LIGHT_H
#define CRT_LIGHT_H

#include <boost/shared_ptr.hpp>

#include "vector3.h"
#include "color.h"

class Light
{
public:
	Vec3		pos;
	Color		color;
};

#endif
