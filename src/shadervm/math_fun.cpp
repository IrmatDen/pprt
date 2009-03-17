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

	if (val < min)
		esp->second = 0.f;
	else if (val > max)
		esp->second = 1.f;
	else
	{
		val = val * val * (3 - 2 * val);
		esp->second = val * min + (max * (1 - val));
	}
		
	esp->first = VT_float;
	++esp;
}
