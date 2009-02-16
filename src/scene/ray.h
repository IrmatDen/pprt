#ifndef CRT_RAY_H
#define CRT_RAY_H

#include "vector3.h"

class Ray
{
public:
	Ray():traceDepth(0) {}
	Ray(const Vec3 &o, const Vec3 &d) : origin(o), dir(d), traceDepth(0)			{}
	Ray(const Ray &r) : origin(r.origin), dir(r.dir), traceDepth(r.traceDepth)		{}

public:
	Vec3	origin, dir;
	int		traceDepth;
};

#endif
