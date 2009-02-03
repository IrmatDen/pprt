#ifndef CRT_SPHERE_H
#define CRT_SPHERE_H

#include "geometry.h"

class Sphere : public Geometry
{
public:
	Sphere(double radius = 1) : r(radius)	{}
	Sphere(double radius, Vec3 p) : r(radius), Geometry(p) {}

	virtual bool			hit(const Ray &ray, double &t) const;
	virtual void			normalAt(const Vec3 &p, Vec3 &n) const;

private:
	double	r;
};

#endif