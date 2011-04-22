#ifndef PPRT_SPHERE_H
#define PPRT_SPHERE_H

#include "geometry.h"

class _MM_ALIGN16 Sphere : public Geometry
{
public:
	Sphere(float radius = 1) : r(radius),invr(1/r)												{}
	Sphere(const Matrix4 &obj2world, float radius) : Geometry(obj2world), r(radius),invr(1/r)	{ buildAABB(); }

	virtual bool hit(const Ray &ray, float &t, IntersectionInfo &ii) const;

private:
	void	buildAABB()
	{
		const Point3 ptRad(r);
		const Point3 ptMinusRad(-r);
		aabb._min = Point3((objectToWorld * ptMinusRad).get128());
		aabb._max = Point3((objectToWorld * ptRad).get128());
	}

private:
	float	r;
    float   invr;
};

#endif