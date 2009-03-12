#ifndef CRT_SYMTAB_H
#define CRT_SYMTAB_H

#include <string>

#include <boost/variant.hpp>

#include "../scene/vector3.h"
#include "../scene/color.h"

enum VariableStorageType
{
	VST_Uniform,
	VST_Varying
};

enum VariableType
{
	VT_float,
	VT_Color,
	VT_Vector
};

typedef boost::variant<float, Color, Vec3> VarValue;

/*union _MM_ALIGN16 VarValue
{
	float	f;
	__m128	c;
	__m128	v;
};*/

struct Variable
{
	Variable() : storageType(VST_Uniform)	{}

	VariableStorageType		storageType;
	VariableType			type;
	std::string				name;
	VarValue				content;
};

#endif
