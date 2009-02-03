#include <iostream>

#include "shader_manager.h"

using namespace std;

int add_file_and_line(lua_State* L)
{
	lua_Debug d;
	lua_getstack(L, 1, &d);
	lua_getinfo(L, "Sln", &d);
	std::string err = lua_tostring(L, -1);
	lua_pop(L, 1);
	std::stringstream msg;
	msg << d.short_src << ":" << d.currentline;

	if (d.name != 0)
	{
		msg << "(" << d.namewhat << " " << d.name << ")";
	}
	msg << " " << err;
	cout << msg << endl;
	lua_pushstring(L, msg.str().c_str());
	return 1;
}

ShaderManager::ShaderManager()
{
	luaState = lua_open();
	luaL_openlibs(luaState);
	luabind::open(luaState);

	using namespace luabind;

	module(luaState)
		[
			class_<Color4>("Color4")
			.def(constructor<>())
			.def(constructor<float, float, float, float>())
			.def(constructor<float, float>())
			.def_readwrite("r", &Color4::r)
			.def_readwrite("g", &Color4::r)
			.def_readwrite("b", &Color4::r)
			.def_readwrite("a", &Color4::r)
		];
}

ShaderManager::ShaderManager(const ShaderManager &other)
:luaState(other.luaState)
{
	cout << "copied" << endl;
}

ShaderManager::~ShaderManager()
{
	lua_close(luaState);
}

void ShaderManager::loadLuaFile(const std::string &fileName)
{
	cout << "loading " << fileName.c_str() << endl << "result: " << luaL_dofile(luaState, fileName.c_str()) << endl;
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
		out = Color4(1, 0, 1, 1);
	}
}
