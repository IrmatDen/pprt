#ifndef CRT_SPHERE_H
#define CRT_SPHERE_H

#include "geometry.h"

class Sphere : public Geometry
{
public:
	Sphere(float radius = 1) : r(radius)	{}
	Sphere(float radius, Vec3 p) : r(radius), Geometry(p) {}

	virtual bool			hit(const Ray &ray, float &t) const;
	virtual void			normalAt(const Vec3 &p, Vec3 &n) const;

private:
	float	r;
};

#endif