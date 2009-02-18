#ifndef CRT_SPHERE_H
#define CRT_SPHERE_H

#include "geometry.h"

class Sphere : public Geometry
{
public:
	Sphere(Real radius = 1) : r(radius)	{}
	Sphere(Real radius, const Vec3 &p) : r(radius), Geometry(p) {}

	virtual bool			hit(const Ray &ray, Real &t) const;
	virtual void			normalAt(const Vec3 &p, Vec3 &n) const;

private:
	Real	r;
};

#endif