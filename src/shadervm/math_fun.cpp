#include "compiled_shader.h"

#include "../scene/vector3.h"

void CompiledShader::smoothstep()
{
	--esp;
	float min = boost::get<float>(esp->second);
	--esp;
	float max = boost::get<float>(esp->second);
	--esp;
	float val = boost::get<float>(esp->second);

	val = val * val * (3 - 2 * val);
	
	esp->first = VT_float;
	esp->second = val;
	++esp;
}
