#ifndef CRT_GEOMETRY_H
#define CRT_GEOMETRY_H

#include <string>
#include <boost/shared_ptr.hpp>

#include "ray.h"

#include "../shadervm/shader_param.h"

class CompiledShader;

class Geometry
{
public:
	virtual ~Geometry();

	virtual bool			hit(const Ray &ray, Real &t) const = 0;
	virtual void			normalAt(const Vec3 &p, Vec3 &n) const = 0;

	const Vec3&				position() const						{ return pos; }

	void					setShader(const CompiledShader &cs);
	bool					hasShader() const						{ return shader != 0; }
	CompiledShader&			getShader() const						{ return *shader; }

	void					setShaderParams(ShaderParams p);
	const ShaderParams&		getShaderParams() const					{ return shaderParams; }

protected:
	Geometry() : shader(0)											{}
	Geometry(const Vec3 &p) : pos(p), shader(0)						{}

protected:
	Vec3					pos;

	CompiledShader		*	shader;
	ShaderParams			shaderParams;
};

typedef boost::shared_ptr<Geometry> GeometryPtr;

#endif
