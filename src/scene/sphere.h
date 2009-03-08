#ifndef CRT_SPHERE_H
#define CRT_SPHERE_H

#include "geometry.h"

class Sphere : public Geometry
{
public:
	Sphere(Real radius = 1) : r(radius)							{}
	Sphere(Real radius, const Vec3 &p) : r(radius), Geometry(p) { buildAABB(); }

	virtual bool			hit(const Ray &ray, Real &t) const;
	virtual void			normalAt(const Vec3 &p, Vec3 &n) const;

private:
	void	buildAABB()
	{
		aabb._min.x = pos.x - r;
		aabb._min.y = pos.y - r;
		aabb._min.z = pos.z - r;
		
		aabb._max.x = pos.x + r;
		aabb._max.y = pos.y + r;
		aabb._max.z = pos.z + r;
	}

private:
	Real	r;
};

#endif