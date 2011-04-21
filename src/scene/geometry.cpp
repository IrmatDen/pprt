#include "geometry.h"
#include "../shadervm/compiled_shader.h"

#include "../memory.h"

Geometry::~Geometry()
{
	memory::destroy(shader);
}

void Geometry::setShader(const CompiledShader &cs)
{
	memory::destroy(shader);

	shader = memory::construct<CompiledShader>(cs);
}

void Geometry::setShaderParams(const ShaderParams &p)
{
	assert(shader);

	shaderParams = p;
	for(ShaderParams::iterator it = shaderParams.begin(); it != shaderParams.end(); ++it)
	{
		shader->setVarValue(it->name, it->value);
	}

	VarValue _MM_ALIGN16 tmp(color);
	shader->setVarValueByIndex(CompiledShader::Cs, tmp);
	tmp = opacity;
	shader->setVarValueByIndex(CompiledShader::Os, tmp);
}

bool Geometry::hasShader() const
{
	return shader != 0 && shader->shaderType() != CompiledShader::ST_Invalid;
}

void Geometry::prepareShader()
{
	assert(shader);
	shader->finalize();
}

