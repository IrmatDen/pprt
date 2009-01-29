#include "geometry.h"
#include "../shadervm/compiled_shader.h"

Geometry::~Geometry()
{
	delete shader;
}

void Geometry::setShader(const CompiledShader &cs)
{
	delete shader;

	shader = new CompiledShader(cs);
}

void Geometry::shade(Color4 &out) const
{
	shader->exec(out);
}
