#ifndef CRT_SPHERE_H
#define CRT_SPHERE_H

#include "geometry.h"

class _MM_ALIGN16 Sphere : public Geometry
{
public:
	Sphere(float radius = 1) : r(radius),invr(1/r)							{}
	Sphere(float radius, const Vec3 &p) : Geometry(p), r(radius),invr(1/r)  { buildAABB(); }

	virtual bool			hit(const Ray &ray, float &t) const;
	virtual void			fillIntersectionInfo(const Vec3 &p, IntersectionInfo &ii) const;

private:
	void	buildAABB()
	{
		aabb._min[0] = pos.x - r;
		aabb._min[1] = pos.y - r;
		aabb._min[2] = pos.z - r;
		
		aabb._max[0] = pos.x + r;
		aabb._max[1] = pos.y + r;
		aabb._max[2] = pos.z + r;
	}

private:
	float	r;
    float   invr;
};

#endif