#ifndef PPRT_LIGHT_H
#define PPRT_LIGHT_H

#include <boost/shared_ptr.hpp>

#include "../sse.h"
#include "color.h"

class Light
{
public:
	Vector3		pos;
	Color		color;
};

#endif
