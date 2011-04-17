#ifndef PPRT_SYMTAB_H
#define PPRT_SYMTAB_H

#include "../sse.h"
#include "../common.h"
#include "../scene/color.h"

#include <string>

#include <boost/variant.hpp>

enum VariableStorageType
{
	VST_Uniform,
	VST_Varying
};

enum VariableType
{
	VT_Vector,
	VT_Color,
	VT_Float,
};
typedef boost::variant<Vector3, Color, float> VarValue;

struct _MM_ALIGN16 Variable
{
	Variable() : storageType(VST_Uniform)	{}
	Variable(VariableStorageType _storageType, VariableType _type, const std::string &_name, const VarValue &_value)
		: storageType(_storageType), type(_type), name(_name), content(_value)
	{
	}

	VarValue  				content;
	VariableStorageType		storageType;
	VariableType			type;
	std::string				name;
};

#endif
