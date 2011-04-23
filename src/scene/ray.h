#ifndef PPRT_RAY_H
#define PPRT_RAY_H

#include "../sse.h"
#include "../pprt_math.h"

class _MM_ALIGN16 Ray
{
public:
	Ray():traceDepth(0) {}
	Ray(const Point3 &o, const Vector3 &d)	: origin(o),		dir(d),		invDir(recipPerElem(dir)),	traceDepth(0)				{}
	Ray(const Ray &r)						: origin(r.origin), dir(r.dir), invDir(r.invDir),			traceDepth(r.traceDepth)	{}

	const Vector3&	direction() const				{ return dir; }
	void			setDirection(const Vector3 &d)	{ dir = d; invDir = recipPerElem(dir); }

public:
	Point3	origin;
	Vector3	invDir;
	int		traceDepth;

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
