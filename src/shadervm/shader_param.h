#ifndef CRT_SHADERVM_SHADER_PARAM_H
#define CRT_SHADERVM_SHADER_PARAM_H

#include <vector>
#include <string>

#include "symtab.h"

enum ParamType
{
	PT_Color,
	PT_Float
};

struct ShaderParam
{
	ShaderParam()	{}
	ShaderParam(const std::string &n, ParamType t, const VarValue &v) : name(n), type(t), value(v)	{}

	std::string		name;
	ParamType		type;
	VarValue		value;
};

typedef std::vector<ShaderParam>	ShaderParams;

#endif
