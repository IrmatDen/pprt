#ifndef CRT_GEOMETRY_H
#define CRT_GEOMETRY_H

#include <boost/shared_ptr.hpp>

#include "ray.h"
#include "material.h"

class Geometry
{
public:
	virtual bool			hit(const Ray &ray, double &t) const = 0;
	virtual void			normalAt(const Vec3d &p, Vec3d &n) const = 0;

	const Vec3d&			position() const	{ return pos; }

	const MaterialPtr		material() const	{ return mat; }
	MaterialPtr&			material()			{ return mat; }

protected:
	Geometry()								{}
	Geometry(const Vec3d &p) : pos(p)		{}

protected:
	Vec3d			pos;
	MaterialPtr		mat;
};

typedef boost::shared_ptr<Geometry> GeometryPtr;

#endif
