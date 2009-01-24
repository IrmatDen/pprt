#ifndef CRT_SPHERE_H
#define CRT_SPHERE_H

#include "geometry.h"

class Sphere : public Geometry
{
public:
	Sphere(float radius = 1) : r(radius)	{}
	Sphere(float radius, Vec3d p) : r(radius), Geometry(p) {}

	virtual bool			hit(const Ray &ray, double &t) const;
	virtual void			normalAt(const Vec3d &p, Vec3d &n) const;

private:
	float	r;
};

#endif