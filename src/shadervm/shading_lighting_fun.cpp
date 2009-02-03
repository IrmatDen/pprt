#include "shading_lighting_fun.h"

#include "shader_manager.h"

Color4 diffuse(const Vec3 &N)
{
	Color4 out;
	ShaderManager::getInstance().diffuse(N, out);
	return out;
}
