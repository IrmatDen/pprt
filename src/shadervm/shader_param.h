#ifndef CRT_SHADERVM_SHADER_PARAM_H
#define CRT_SHADERVM_SHADER_PARAM_H

#include <vector>
#include <boost/any.hpp>

enum ParamType
{
	PT_Color4
};

struct ShaderParam
{
	ShaderParam()	{}
	ShaderParam(ParamType t, boost::any v) : type(t), value(v)	{}

	ParamType	type;
	boost::any	value;
};

typedef std::vector<ShaderParam>	ShaderParams;

#endif
