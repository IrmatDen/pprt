#include <iostream>
#include <functional>

#include "shader_manager.h"

#include "../scene/ray.h"
#include "../scene/vector3.h"
#include "../scene/scene.h"

using namespace std;

extern void bindLuaToCRT(lua_State *s);

ShaderManager::ShaderManager()
:scene(0)
{
	luaState = lua_open();
	luaL_openlibs(luaState);
	luabind::open(luaState);

	bindLuaToCRT(luaState);
}

ShaderManager::~ShaderManager()
{
	lua_close(luaState);
}

void ShaderManager::loadLuaFile(const std::string &fileName)
{
	cout << "loading " << fileName.c_str() << ": " << luaL_dofile(luaState, fileName.c_str()) << endl;
}

void ShaderManager::registerP(const Vec3 &v)
{
	luabind::globals(luaState)["P"] = &v;
	env.P = v;
}

void ShaderManager::registerN(const Vec3 &v)
{
	luabind::globals(luaState)["N"] = &v;
	env.N = v;
}

void ShaderManager::execute(const string &shaderName, Color4 &out)
{
	try
	{
		out = luabind::call_function<Color4>(luaState, shaderName.c_str());
	}
	catch(luabind::error &e)
	{
		const char* message = lua_tostring(e.state(), -1);
		cout << message << endl;
		out = Color4(1, 0, 1, 1);
	}
}

void ShaderManager::diffuse(const Vec3 &N, Color4 &out) const
{
	scene->diffuse(Ray(env.P, N), out);
}
