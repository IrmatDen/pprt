#ifndef PPRT_GEOMETRY_H
#define PPRT_GEOMETRY_H

#include <string>
#include <boost/shared_ptr.hpp>

#include "aabb.h"
#include "ray.h"
#include "color.h"

#include "../shadervm/shader_param.h"

class CompiledShader;

struct IntersectionInfo
{
	Point3	point;
	Vector3	normal;
	float	s, t;
};

class _MM_ALIGN16 Geometry
{
public:
	virtual ~Geometry();

	virtual bool			hit(const Ray &ray, IntersectionInfo &ii) const = 0;

	const AABB&				getAABB() const							{ return aabb; }

	Point3					position() const						{ return Point3(objectToWorld.getTranslation()); }

	void					setColor(const Color &c)				{ color = c; }
	void					setOpacity(const Color &o)				{ opacity = o; }
	const Color&			getOpacity() const						{ return opacity; }

	void					setShader(const CompiledShader &cs);
	bool					hasShader() const;
	CompiledShader&			getShader() const						{ return *shader; }

	void					setShaderParams(const ShaderParams &p);

	void					prepareShader();

protected:
	Geometry() : objectToWorld(Matrix4::identity()), color(0.f), opacity(0.f), shader(0) 			{}
	Geometry(const Matrix4 &obj2world) : objectToWorld(obj2world), color(1), shader(0)				{ worldToObject = inverse(objectToWorld); }

protected:
	AABB					aabb;

	Matrix4					objectToWorld;
	Matrix4					worldToObject;

	Color					color;
	Color					opacity;

	CompiledShader		*	shader;
	ShaderParams			shaderParams;
};

#endif
