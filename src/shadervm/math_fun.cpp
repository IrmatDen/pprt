#include "compiled_shader.h"

void CompiledShader::smoothstep()
{
	float min = execStack.pop<float>();
	float max = execStack.pop<float>();
	float val = execStack.pop<float>();


	if (val < min)
		execStack.push(0.f);
	else if (val > max)
		execStack.push(1.f);
	else
		execStack.push((val - min) / (max - min));
}
