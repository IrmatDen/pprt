#ifndef CRT_RAY_H
#define CRT_RAY_H

#include "vector3.h"

class _MM_ALIGN16 Ray
{
public:
	Ray():traceDepth(0) {}
	Ray(const Vec3 &o, const Vec3 &d) : origin(o), dir(d), invDir(1/dir), traceDepth(0)				{}
	Ray(const Ray &r) : origin(r.origin), dir(r.dir), invDir(r.invDir), traceDepth(r.traceDepth)	{}

	const Vec3&		direction() const				{ return dir; }
	void			setDirection(const Vec3 &d)		{ dir = d; invDir = 1 / dir; }

public:
	Vec3	origin;
	Vec3	invDir;
	int		traceDepth;

private:
	Vec3	dir;
};

#endif
