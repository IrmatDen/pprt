#ifndef CRT_SHADERVM_SHADER_PARAM_H
#define CRT_SHADERVM_SHADER_PARAM_H

#include <vector>
#include <string>
#include <boost/any.hpp>

enum ParamType
{
	PT_Color4,
	PT_Real
};

struct ShaderParam
{
	ShaderParam()	{}
	ShaderParam(const std::string &n, ParamType t, boost::any v) : name(n), type(t), value(v)	{}

	std::string		name;
	ParamType		type;
	boost::any		value;
};

typedef std::vector<ShaderParam>	ShaderParams;

#endif
