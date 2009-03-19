#include "compiled_shader.h"

#include "../scene/vector3.h"

void CompiledShader::smoothstep()
{
	--esp;
	float min = esp->second;
	--esp;
	float max = esp->second;
	--esp;
	float val = esp->second;


	if (val < min)
		esp->second = 0.f;
	else if (val > max)
		esp->second = 1.f;
	else
		esp->second = (val - min) / (max - min);
		
	esp->first = VT_Float;
	++esp;
}
