#ifndef CRT_SYMTAB_H
#define CRT_SYMTAB_H

#include <string>

#include <boost/variant.hpp>

#include "../scene/vector3.h"
#include "../scene/color4.h"

enum VariableStorageType
{
	VST_Uniform,
	VST_Varying
};

enum VariableType
{
	VT_Real,
	VT_Color,
	VT_Vector
};

typedef boost::variant<Real, Color4, Vec3> VarValue;

struct Variable
{
	Variable() : storageType(VST_Uniform)	{}

	VariableStorageType		storageType;
	VariableType			type;
	std::string				name;
	VarValue				content;
};

#endif
