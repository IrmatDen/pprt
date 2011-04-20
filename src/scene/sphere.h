#ifndef PPRT_SPHERE_H
#define PPRT_SPHERE_H

#include "geometry.h"

class _MM_ALIGN16 Sphere : public Geometry
{
public:
	Sphere(float radius = 1) : r(radius),invr(1/r)							{}
	Sphere(float radius, const Point3 &p) : Geometry(p), r(radius),invr(1/r)  { buildAABB(); }

	virtual bool			hit(const Ray &ray, float &t) const;
	virtual void			fillIntersectionInfo(const Point3 &p, IntersectionInfo &ii) const;

private:
	void	buildAABB()
	{
		const Vector3 vecR(r);
		aabb._min = pos - vecR;
		aabb._max = pos + vecR;
	}

private:
	float	r;
    float   invr;
};

#endif