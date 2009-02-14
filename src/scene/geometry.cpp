#include "geometry.h"
#include "../shadervm/compiled_shader.h"

Geometry::~Geometry()
{
}

void Geometry::setShader(const CompiledShader &cs)
{
	delete shader;

	shader = new CompiledShader(cs);
}

void Geometry::setShaderParams(ShaderParams p)
{
	assert(shader);

	shaderParams = p;
	for(ShaderParams::iterator it = shaderParams.begin(); it != shaderParams.end(); ++it)
	{
		shader->setVarValue(it->name, it->value);
	}
}
