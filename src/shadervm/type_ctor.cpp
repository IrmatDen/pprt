#include <cassert>

#include "compiled_shader.h"

using namespace std;
using namespace boost;

void CompiledShader::colorCtor()
{
	Color col;
	--esp;
	col.r = esp->second;
	--esp;
	col.g = esp->second;
	--esp;
	col.b = esp->second;

	esp->first = VT_Color;
	esp->second = col;
	++esp;
}

void CompiledShader::vec3Ctor()
{
	Vec3 out;
	--esp;
	out.x = esp->second;
	--esp;
	out.y = esp->second;
	--esp;
	out.z = esp->second;

	esp->first = VT_Vector;
	esp->second = out;
	++esp;
}
