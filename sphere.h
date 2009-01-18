#ifndef CRT_SPHERE_H
#define CRT_SPHERE_H

#include "geometry.h"

class Sphere : public Geometry
{
public:
	Sphere(float radius = 1) : r(radius)	{}
	Sphere(float radius, Vec3f p) : r(radius), Geometry(p) {}

	virtual bool			hit(const Ray &ray, float &t) const;
	virtual void			normalAt(const Vec3f &p, Vec3f &n) const;

private:
	float	r;
};

#endif