#include <cassert>

#include "compiled_shader.h"

using namespace std;
using namespace boost;

void CompiledShader::mix()
{
	const Color &v0 = execStack.pop<Color>();
	const Color &v1 = execStack.pop<Color>();
	float &interp = execStack.pop<float>();

	execStack.push(Color(interp * v0 + (1 - interp) * v1));
}
