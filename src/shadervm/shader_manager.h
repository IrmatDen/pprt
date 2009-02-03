#ifndef CRT_SHADERVM_SHADER_MANAGER_H
#define CRT_SHADERVM_SHADER_MANAGER_H

#include <map>
#include <string>

extern "C"
{
    #include "lua.h"
    #include "lauxlib.h"
	#include "lualib.h"
}

#include <luabind/luabind.hpp>

#include "../scene/color4.h"

class ShaderManager
{
public:
	ShaderManager();
	ShaderManager(const ShaderManager &other);
	~ShaderManager();

	void loadLuaFile(const std::string &fileName);

	void execute(const std::string &shaderName, Color4 &out);

private:
	lua_State		*	luaState;
};

#endif
