#include <cassert>

#include "compiled_shader.h"

using namespace std;
using namespace boost;

void CompiledShader::colorCtor()
{
	--esp;
	float r = (float)boost::get<float>(esp->second);
	--esp;
	float g = (float)boost::get<float>(esp->second);
	--esp;
	float b = (float)boost::get<float>(esp->second);

	esp->first = VT_Color;
	esp->second = Color(r, g, b);
	++esp;
}

void CompiledShader::vec3Ctor()
{
	--esp;
	float x = boost::get<float>(esp->second);
	--esp;
	float y = boost::get<float>(esp->second);
	--esp;
	float z = boost::get<float>(esp->second);

	esp->first = VT_Vector;
	esp->second = Vector3(x, y, z);
	++esp;
}
