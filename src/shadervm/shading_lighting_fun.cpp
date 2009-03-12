#include "compiled_shader.h"

#include "../scene/scene.h"

using namespace boost;

void CompiledShader::diffuse()
{
	// 1 parameter expected: Normal

	Vec3 &p = boost::get<Vec3>(varTable[P].content);
	--esp;
	Vec3 &n = boost::get<Vec3>(esp->second);

	Color out;
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

	Vec3 &p = boost::get<Vec3>(varTable[P].content);
	--esp;
	Vec3 &n = boost::get<Vec3>(esp->second);
	Vec3 &i = boost::get<Vec3>(varTable[I].content);
	--esp;
	float roughness	= boost::get<float>(esp->second);

	Color out;
	Ray r(p, n);
	r.traceDepth = currentDepth;
	scene->specular(r, i, roughness, out);
	
	esp->first = VT_Color;
	esp->second = out;
	++esp;
}

void CompiledShader::trace()
{
	Ray r;

	// 2 parameters expected: origin & direction
	--esp;
	r.origin = boost::get<Vec3>(esp->second);
	--esp;
	r.setDirection(boost::get<Vec3>(esp->second));
	
	r.origin += r.direction() * Epsilon;
	
	r.traceDepth = currentDepth;

	static bool dummy;
	Color out = scene->trace(r, dummy);
	
	esp->first = VT_Color;
	esp->second = out;
	++esp;
}
