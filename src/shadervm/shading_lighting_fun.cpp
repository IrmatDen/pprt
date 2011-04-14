#include "compiled_shader.h"

#include "../scene/scene.h"

using namespace boost;

void CompiledShader::ambient()
{
	Color out;
	scene->ambient(out);
	execStack.push(out);
}

void CompiledShader::diffuse()
{
	// 1 parameter expected: Normal

	Vector3 &p = boost::get<Vector3>(varTable[P].content);
	Vector3 &n = execStack.pop<Vector3>();

	Color out(0.f);
	Ray r(p, n);
	r.traceDepth = currentDepth;
	scene->diffuse(r, out);
	
	execStack.push(out);
}

void CompiledShader::specular()
{
	// 2 parameters expected: Normal & roughness

	Vector3 &p = boost::get<Vector3>(varTable[P].content);
	Vector3 &n = execStack.pop<Vector3>();
	Vector3 &i = boost::get<Vector3>(varTable[I].content);
	float roughness	= execStack.pop<float>();

	Color out(0.f);
	Ray r(p, n);
	r.traceDepth = currentDepth;
	scene->specular(r, i, roughness, out);
	
	execStack.push(out);
}

void CompiledShader::trace()
{
	Ray r;

	// 2 parameters expected: origin & direction
	r.origin = execStack.pop<Vector3>();
	r.setDirection(execStack.pop<Vector3>());
	
	r.origin += r.direction() * 0.0001f;
	
	r.traceDepth = currentDepth;

	static bool dummy;
	Color out = scene->trace(r, dummy);
	
	execStack.push(out);
}
