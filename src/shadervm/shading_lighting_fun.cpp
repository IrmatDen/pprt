#include <cassert>

#include "compiled_shader.h"

#include "../scene/scene.h"

using namespace boost;

void CompiledShader::diffuse()
{
	// 1 parameter expected: Normal
	assert(execStack.size() >= 1);

	Vec3 p = any_cast<Vec3>(varTable[P].content);
	Vec3 n = any_cast<Vec3>(execStack.top().second); execStack.pop();

	Color4 out;
	scene->diffuse(Ray(p, n), out);
	
	execStack.push(make_pair(VT_Color, out));
}

void CompiledShader::specular()
{
	// 2 parameters expected: Normal & roughness
	assert(execStack.size() >= 2);

	Vec3 p = any_cast<Vec3>(varTable[P].content);
	Vec3 n = any_cast<Vec3>(execStack.top().second); execStack.pop();
	Vec3 i = any_cast<Vec3>(varTable[I].content);
	double roughness	= any_cast<double>(execStack.top().second);	execStack.pop();

	Color4 out;
	scene->specular(Ray(p, n), i, roughness, out);
	
	execStack.push(make_pair(VT_Color, out));
}
