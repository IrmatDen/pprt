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

class Vec3;
class Scene;

class ShaderManager
{
public:
	~ShaderManager();

	static inline ShaderManager& getInstance()
	{
		static ShaderManager sm;
		return sm;
	}

	void setScene(Scene &scn)									{ scene = &scn; }

	void loadLuaFile(const std::string &fileName);

	void registerP(const Vec3 &v);
	void registerN(const Vec3 &v);

	void execute(const std::string &shaderName, Color4 &out);

	// Functions acting as lua bindings to get scene related info
	void diffuse(const Vec3 &N, Color4 &out) const;

private:
	ShaderManager();

private:
	struct ExecEnv
	{
		Vec3 P, N;
	} env;

private:
	lua_State		*	luaState;
	Scene			*	scene;
};

#endif
