#include "compiled_shader.h"

#include "../scene/scene.h"

using namespace boost;

void CompiledShader::diffuse()
{
	// 1 parameter expected: Normal

	Vector3 &p = boost::get<Vector3>(varTable[P].content);
	--esp;
	Vector3 &n = boost::get<Vector3>(esp->second);

	Color out(0.f);
	Ray r(p, n);
	r.traceDepth = currentDepth;
	scene->diffuse(r, out);
	
	esp->first = VT_Color;
	esp->second = out;
	++esp;
}

void CompiledShader::specular()
{
	// 2 parameters expected: Normal & roughness

	Vector3 &p = boost::get<Vector3>(varTable[P].content);
	--esp;
	Vector3 &n = boost::get<Vector3>(esp->second);
	Vector3 &i = boost::get<Vector3>(varTable[I].content);
	--esp;
	float roughness	= boost::get<float>(esp->second);

	Color out(0.f);
	Ray r(p, n);
	r.traceDepth = currentDepth;
	scene->specular(r, i, roughness, out);
	
	esp->first = VT_Color;
	esp->second = VarValue(out);
	++esp;
}

void CompiledShader::trace()
{
	Ray r;

	// 2 parameters expected: origin & direction
	--esp;
	r.origin = boost::get<Vector3>(esp->second);
	--esp;
	r.setDirection(boost::get<Vector3>(esp->second));
	
	r.origin += r.direction() * 0.0000001f;
	
	r.traceDepth = currentDepth;

	static bool dummy;
	Color out = scene->trace(r, dummy);
	
	esp->first = VT_Color;
	esp->second = out;
	++esp;
}
