#ifndef CRT_GEOMETRY_H
#define CRT_GEOMETRY_H

#include <boost/shared_ptr.hpp>

#include "ray.h"
#include "material.h"

class Geometry
{
public:
	virtual bool			hit(const Ray &ray, float &t) const = 0;
	virtual void			normalAt(const Vec3f &p, Vec3f &n) const = 0;

	const Vec3f&			position() const	{ return pos; }

	const MaterialPtr		material() const	{ return mat; }
	MaterialPtr&			material()			{ return mat; }

protected:
	Geometry()								{}
	Geometry(const Vec3f &p) : pos(p)		{}

protected:
	Vec3f			pos;
	MaterialPtr		mat;
};

typedef boost::shared_ptr<Geometry> GeometryPtr;

#endif
