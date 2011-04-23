#ifndef PPRT_RAY_H
#define PPRT_RAY_H

#include "../sse.h"
#include "../pprt_math.h"

class _MM_ALIGN16 Ray
{
public:
	Ray()
		: traceDepth(0), minT(0.f), maxT(0.f)
	{
	}
	Ray(const Point3 &o, const Vector3 &d)
		:	origin(o), dir(d), invDir(recipPerElem(dir)),
			traceDepth(0), minT(0.f), maxT(0.f)
	{
	}
	Ray(const Ray &r)
		:	origin(r.origin), dir(r.dir), invDir(r.invDir),
			traceDepth(r.traceDepth), minT(r.minT), maxT(r.maxT)
	{
	}

	const Vector3&	direction() const				{ return dir; }
	void			setDirection(const Vector3 &d)	{ dir = d; invDir = recipPerElem(dir); }

public:
	Point3	origin;
	Vector3	invDir;
	
	mutable int		traceDepth;
	mutable float	minT, maxT;

private:
	Vector3	dir;
};

inline Ray operator*(const Matrix4 &xform, const Ray &r)
{
	Ray ret(r);
	ret.origin = Point3((xform * r.origin).get128());
	ret.setDirection(Vector3((xform * r.direction()).get128()));

	return ret;
}

#endif
