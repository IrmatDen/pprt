#ifndef CRT_RAY_H
#define CRT_RAY_H

#include "vector3.h"

class Ray
{
public:
	Ray() {}
	Ray(const Vec3f &o, const Vec3f &d) : origin(o), dir(d)	{}
	Ray(const Ray &r) : origin(r.origin), dir(r.dir) {}

public:
	Vec3f origin, dir;
};

#endif
