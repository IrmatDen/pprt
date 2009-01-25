#ifndef CRT_GEOMETRY_H
#define CRT_GEOMETRY_H

#include <boost/shared_ptr.hpp>

#include "ray.h"
#include "material.h"

class Geometry
{
public:
	virtual bool			hit(const Ray &ray, Real &t) const = 0;
	virtual void			normalAt(const Vec3 &p, Vec3 &n) const = 0;

	const Vec3&				position() const	{ return pos; }

	const MaterialPtr		material() const	{ return mat; }
	MaterialPtr&			material()			{ return mat; }

protected:
	Geometry()								{}
	Geometry(const Vec3 &p) : pos(p)		{}

protected:
	Vec3			pos;
	MaterialPtr		mat;
};

typedef boost::shared_ptr<Geometry> GeometryPtr;

#endif
