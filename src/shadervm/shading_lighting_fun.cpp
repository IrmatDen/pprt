#include "compiled_shader.h"

#include "../scene/scene.h"

using namespace boost;

void CompiledShader::diffuse()
{
	// 1 parameter expected: Normal

	Vec3 *p = any_cast<Vec3>(&(varTable[P].content));
	--esp;
	Vec3 *n = any_cast<Vec3>(&(esp->second));

	Color4 out;
	scene->diffuse(Ray(*p, *n), out);
	
	esp->first = VT_Color;
	esp->second = out;
	++esp;
}

void CompiledShader::specular()
{
	// 2 parameters expected: Normal & roughness

	Vec3 *p = any_cast<Vec3>(&varTable[P].content);
	--esp;
	Vec3 *n = any_cast<Vec3>(&(esp->second));
	Vec3 *i = any_cast<Vec3>(&varTable[I].content);
	--esp;
	Real roughness	= any_cast<Real>(esp->second);

	Color4 out;
	scene->specular(Ray(*p, *n), *i, roughness, out);
	
	esp->first = VT_Color;
	esp->second = out;
	++esp;
}

void CompiledShader::trace()
{
	static Ray r;

	// 2 parameters expected: origin & direction
	--esp;
	r.origin = *any_cast<Vec3>(&(esp->second));
	--esp;
	r.dir = *any_cast<Vec3>(&(esp->second));
	
	r.origin += r.dir * 0.0000001;
	
	r.traceDepth = currentDepth;

	static bool dummy;
	Color4 out = scene->trace(r, dummy);
	
	esp->first = VT_Color;
	esp->second = out;
	++esp;
}
