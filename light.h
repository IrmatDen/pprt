#ifndef CRT_LIGHT_H
#define CRT_LIGHT_H

#include <boost/shared_ptr.hpp>

#include "vector3.h"
#include "color4.h"

class Light
{
public:
	Vec3f		pos;
	Color4		color;
};

typedef boost::shared_ptr<Light> LightPtr;

#endif
