#include <cassert>

#include "compiled_shader.h"

using namespace std;
using namespace boost;

void CompiledShader::colorCtor()
{
	Color col;
	--esp;
	col.r = (float)boost::get<float>(esp->second);
	--esp;
	col.g = (float)boost::get<float>(esp->second);
	--esp;
	col.b = (float)boost::get<float>(esp->second);

	esp->first = VT_Color;
	esp->second = col;
	++esp;
}

void CompiledShader::vec3Ctor()
{
	Vec3 out;
	--esp;
	out.x = boost::get<float>(esp->second);
	--esp;
	out.y = boost::get<float>(esp->second);
	--esp;
	out.z = boost::get<float>(esp->second);

	esp->first = VT_Vector;
	esp->second = out;
	++esp;
}
