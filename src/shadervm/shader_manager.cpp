#include <iostream>

#include "shader_manager.h"

using namespace std;

extern void bindLuaToCRT(lua_State *s);

ShaderManager::ShaderManager()
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
	cout << "loading " << fileName.c_str() << endl << "result: " << luaL_dofile(luaState, fileName.c_str()) << endl;
}

void ShaderManager::registerGlobalVec3(const string &varName, const Vec3 &v)
{
	luabind::globals(luaState)[varName] = &v;
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
