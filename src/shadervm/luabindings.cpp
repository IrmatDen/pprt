extern "C"
{
    #include "lua.h"
    #include "lauxlib.h"
	#include "lualib.h"
}

#include <luabind/luabind.hpp>
#include <luabind/operator.hpp>

#include "../scene/color4.h"
#include "../scene/vector3.h"

#include "shading_lighting_fun.h"

using namespace luabind;

void bindColor4(lua_State *s)
{
	module(s)
		[
			class_<Color4>("Color4")
			.def(constructor<>())
			.def(constructor<float, float, float, float>())
			.def(constructor<float, float>())
			.def(constructor<const Vec3&>())
			.def(const_self + float())
			.def(const_self + Color4())
			.def(const_self / float())
			.def(const_self * Color4())
			.def(const_self * float())
			.def_readwrite("r", &Color4::r)
			.def_readwrite("g", &Color4::g)
			.def_readwrite("b", &Color4::b)
			.def_readwrite("a", &Color4::a)
		];
}

void bindVec3(lua_State *s)
{
	module(s)
		[
			class_<Vec3>("Vec3")
			.def(constructor<>())
			.def(constructor<double, double, double>())
			.def(constructor<double>())
			.def_readwrite("x", &Vec3::x)
			.def_readwrite("y", &Vec3::y)
			.def_readwrite("z", &Vec3::z)
		];
}

void bindLuaToCRT(lua_State *s)
{
	bindColor4(s);
	bindVec3(s);
	
	module(s)
		[
			def("diffuse", &diffuse),
			def("specular", &specular)
		];
}
