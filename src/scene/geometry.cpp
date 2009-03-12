#include "geometry.h"
#include "../shadervm/compiled_shader.h"

#include "../common.h"

Geometry::~Geometry()
{
	memory::destroy(shader);
}

void Geometry::setShader(const CompiledShader &cs)
{
	memory::destroy(shader);

	shader = memory::construct<CompiledShader>(cs);
}

void Geometry::setShaderParams(ShaderParams p)
{
	assert(shader);

	shaderParams = p;
	for(ShaderParams::iterator it = shaderParams.begin(); it != shaderParams.end(); ++it)
	{
		shader->setVarValue(it->name, it->value);
	}

	shader->setVarValueByIndex(CompiledShader::Cs, color);
	shader->setVarValueByIndex(CompiledShader::Os, opacity);
}
