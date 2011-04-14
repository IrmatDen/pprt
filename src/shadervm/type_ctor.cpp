#include <cassert>

#include "compiled_shader.h"

using namespace std;
using namespace boost;

void CompiledShader::colorCtor()
{
	float &r = execStack.pop<float>();
	float &g = execStack.pop<float>();
	float &b = execStack.pop<float>();

	execStack.push(Color(r, g, b));
}

void CompiledShader::vec3Ctor()
{
	float &x = execStack.pop<float>();
	float &y = execStack.pop<float>();
	float &z = execStack.pop<float>();

	execStack.push(Vector3(x, y, z));
}
