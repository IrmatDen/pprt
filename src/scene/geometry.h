#ifndef CRT_GEOMETRY_H
#define CRT_GEOMETRY_H

#include <string>
#include <boost/shared_ptr.hpp>

#include "aabb.h"
#include "ray.h"
#include "color.h"

#include "../shadervm/shader_param.h"

class CompiledShader;

struct IntersectionInfo
{
	Vector3	normal;
	float	s, t;
};

class _MM_ALIGN16 Geometry
{
public:
	virtual ~Geometry();

	virtual bool			hit(const Ray &ray, float &t) const = 0;
	virtual void			fillIntersectionInfo(const Vector3 &p, IntersectionInfo &ii) const = 0;

	const AABB&				getAABB() const							{ return aabb; }

	const Vector3&			position() const						{ return pos; }

	void					setColor(const Color &c)				{ color = c; }
	void					setOpacity(const Color &o)				{ opacity = o; }
	const Color&			getOpacity() const						{ return opacity; }

	void					setShader(const CompiledShader &cs);
	bool					hasShader() const;
	CompiledShader&			getShader() const						{ return *shader; }

	void					setShaderParams(const ShaderParams &p);

	void					prepareShader();

protected:
	Geometry() : pos(0.f), color(0.f), opacity(0.f), shader(0) 			{}
	Geometry(const Vector3 &p) : pos(p), color(1), shader(0)			{}

protected:
	AABB					aabb;

	Vector3					pos;

	Color					color;
	Color					opacity;

	CompiledShader		*	shader;
	ShaderParams			shaderParams;
};

#endif
