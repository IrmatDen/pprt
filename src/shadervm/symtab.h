#ifndef CRT_SYMTAB_H
#define CRT_SYMTAB_H

#include <string>

#include <boost/variant.hpp>

#include "../scene/vector3.h"
#include "../scene/color.h"

#include "varvalue.h"

enum VariableStorageType
{
	VST_Uniform,
	VST_Varying
};

enum VariableType
{
	VT_Float,
	VT_Color,
	VT_Vector
};

struct Variable
{
	Variable() : storageType(VST_Uniform)	{}

	VariableStorageType		storageType;
	VariableType			type;
	std::string				name;
	VarValue				content;
};

#endif
