#ifndef CRT_SYMTAB_H
#define CRT_SYMTAB_H

#include <string>

#include <boost/any.hpp>

enum VariableStorageType
{
	VST_Uniform,
	VST_Varying
};

enum VariableType
{
	VT_Double,
	VT_Color,
	VT_Vector
};

struct Variable
{
	Variable() : storageType(VST_Uniform)	{}

	VariableStorageType		storageType;
	VariableType			type;
	std::string				name;
	boost::any				content;
};

#endif
