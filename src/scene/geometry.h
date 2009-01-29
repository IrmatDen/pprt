#ifndef CRT_GEOMETRY_H
#define CRT_GEOMETRY_H

#include <boost/shared_ptr.hpp>

#include "ray.h"
#include "material.h"

class CompiledShader;

class Geometry
{
public:
	virtual ~Geometry();

	virtual bool			hit(const Ray &ray, Real &t) const = 0;
	virtual void			normalAt(const Vec3 &p, Vec3 &n) const = 0;

	const Vec3&				position() const			{ return pos; }

	const MaterialPtr		material() const			{ return mat; }
	MaterialPtr&			material()					{ return mat; }

	void					setShader(const CompiledShader &cs);
	bool					hasShader() const			{ return shader != 0; }
	CompiledShader&			getShader()					{ return *shader; }

protected:
	Geometry() : shader(0)							{}
	Geometry(const Vec3 &p) : pos(p), shader(0)		{}

protected:
	Vec3					pos;
	MaterialPtr				mat;
	CompiledShader		*	shader;
};

typedef boost::shared_ptr<Geometry> GeometryPtr;

#endif
