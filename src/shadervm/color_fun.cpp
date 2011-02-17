#include <cassert>

#include "compiled_shader.h"

using namespace std;
using namespace boost;

void CompiledShader::mix()
{
	--esp;
	const Color &v0 = get<Color>(esp->second);
	--esp;
	const Color &v1 = get<Color>(esp->second);
	--esp;
	float interp = get<float>(esp->second);

	esp->first = VT_Color;
	esp->second = interp * v0 + (1 - interp) * v1;
	++esp;
}
