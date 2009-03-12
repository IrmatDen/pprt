#ifndef CRT_GEOMETRY_H
#define CRT_GEOMETRY_H

#include <string>
#include <boost/shared_ptr.hpp>

#include "aabb.h"
#include "ray.h"
#include "color.h"

#include "../shadervm/shader_param.h"

class CompiledShader;

class _MM_ALIGN16 Geometry
{
public:
	virtual ~Geometry();

	virtual bool			hit(const Ray &ray, float &t) const = 0;
	virtual void			normalAt(const Vec3 &p, Vec3 &n) const = 0;

	const AABB&				getAABB() const							{ return aabb; }

	const Vec3&				position() const						{ return pos; }

	void					setColor(const Color &c)				{ color = c; }
	void					setOpacity(const Color &o)				{ opacity = o; }
	const Color&			getOpacity() const						{ return opacity; }

	void					setShader(const CompiledShader &cs);
	bool					hasShader() const						{ return shader != 0; }
	CompiledShader&			getShader() const						{ return *shader; }

	void					setShaderParams(ShaderParams p);

protected:
	Geometry() : color(1), shader(0) 								{}
	Geometry(const Vec3 &p) : pos(p), color(1), shader(0)			{}

protected:
	AABB					aabb;

	Vec3					pos;

	Color					color;
	Color					opacity;

	CompiledShader		*	shader;
	ShaderParams			shaderParams;
};

#endif
