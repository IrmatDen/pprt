#ifndef CRT_GEOMETRY_H
#define CRT_GEOMETRY_H

#include <string>
#include <boost/shared_ptr.hpp>

#include "ray.h"
#include "material.h"

class Geometry
{
public:
	virtual ~Geometry();

	virtual bool			hit(const Ray &ray, float &t) const = 0;
	virtual void			normalAt(const Vec3 &p, Vec3 &n) const = 0;

	const Vec3&				position() const						{ return pos; }

	const MaterialPtr		material() const						{ return mat; }
	MaterialPtr&			material()								{ return mat; }

	void					setShader(const std::string &name)		{ shaderName = name; }
	bool					hasShader() const						{ return shaderName.length() != 0; }
	const std::string&		getShaderName() const					{ return shaderName; }

protected:
	Geometry()														{}
	Geometry(const Vec3 &p) : pos(p)								{}

protected:
	Vec3					pos;
	MaterialPtr				mat;
	std::string				shaderName;
};

typedef boost::shared_ptr<Geometry> GeometryPtr;

#endif
