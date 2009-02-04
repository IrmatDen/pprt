#include "shading_lighting_fun.h"

#include "shader_manager.h"

Color4 diffuse(const Vec3 &N)
{
	Color4 out;
	ShaderManager::getInstance().diffuse(N, out);
	return out;
}

Color4 specular(const Vec3 &N, double roughness)
{
	Color4 out;
	ShaderManager::getInstance().specular(N, roughness, out);
	return out;
}
