#include "compiled_shader.h"

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
		esp->second = (val - min) / (max - min);
		
	esp->first = VT_Float;
	++esp;
}
