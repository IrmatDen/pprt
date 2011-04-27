#include "geometry.h"
#include "scene.h"
#include "../shadervm/compiled_shader.h"

#include "../memory.h"

Geometry::Geometry(Scene *scn)
    : objectToWorld(Matrix4::identity()), color(0.f), opacity(0.f), shader(0)
{
    setScene(scn);
}

Geometry::Geometry(Scene *scn, const Matrix4 &obj2world)
    : objectToWorld(obj2world), color(1), shader(0)
{
    setScene(scn);
    worldToObject = inverse(objectToWorld);
}

Geometry::~Geometry()
{
	memory::destroy(shader);
}

void Geometry::setScene(Scene *scn)
{
    scene = scn;

    if (scene)
    {
        worldToObjectN = objectToWorld * scene->camera().WorldToCam;
        worldToObjectN.setElem(3, 0, 0.f);
        worldToObjectN.setElem(3, 1, 0.f);
        worldToObjectN.setElem(3, 2, 0.f);
        worldToObjectN.setElem(0, 3, 0.f);
        worldToObjectN.setElem(1, 3, 0.f);
        worldToObjectN.setElem(2, 3, 0.f);
        worldToObjectN.setElem(3, 3, 1.f);
        worldToObjectN = transpose(inverse(worldToObjectN));
    }
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
		shader->setVarValue(it->name, it->value);
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

