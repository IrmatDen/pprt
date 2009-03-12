#ifndef CRT_SPHERE_H
#define CRT_SPHERE_H

#include "geometry.h"

class _MM_ALIGN16 Sphere : public Geometry
{
public:
	Sphere(Real radius = 1) : r(radius)							{}
	Sphere(Real radius, const Vec3 &p) : r(radius), Geometry(p) { buildAABB(); }

	virtual bool			hit(const Ray &ray, Real &t) const;
	virtual void			normalAt(const Vec3 &p, Vec3 &n) const;

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
	Real	r;
};

#endif