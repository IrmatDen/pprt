#include <iostream>
#include <functional>

#include "shader_manager.h"

#include "../scene/ray.h"
#include "../scene/vector3.h"
#include "../scene/scene.h"

using namespace std;

ShaderManager::ShaderManager()
:scene(0)
{
}

ShaderManager::~ShaderManager()
{
}

void ShaderManager::loadFile(const std::string &fileName)
{
	cout << "loading " << fileName.c_str() << ": " << endl;
}

void ShaderManager::registerP(const Vec3 &v)
{
	env.P = v;
}

void ShaderManager::registerN(const Vec3 &v)
{
	env.N = v;
}

void ShaderManager::registerI(const Vec3 &v)
{
	env.I = v;
}

void ShaderManager::execute(const string &shaderName, const ShaderParams &params, Color4 &out)
{
}

void ShaderManager::diffuse(const Vec3 &N, Color4 &out) const
{
	scene->diffuse(Ray(env.P, N), out);
}

void ShaderManager::specular(const Vec3 &N, double roughness, Color4 &out) const
{
	scene->specular(Ray(env.P, N), env.I, roughness, out);
}
