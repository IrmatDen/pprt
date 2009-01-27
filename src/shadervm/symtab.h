#ifndef CRT_SYMTAB_H
#define CRT_SYMTAB_H

#include <string>
#include <map>
#include <iostream>

#include <boost/any.hpp>

enum VariableStorageType
{
	VST_Uniform,
	VST_Varying
};

enum VariableType
{
	VT_Color
};

struct Variable
{
	Variable() : storageType(VST_Uniform)	{}

	VariableStorageType		storageType;
	VariableType			type;
	std::string				name;
	boost::any				content;
};

class SymbolTable
{
public:
	//! \todo throw exception if variable already defined
	void			addVar(const Variable &var)
	{
		variables[var.name] = var;
	}

	Variable*		find(const std::string &varName)
	{
		VariablesTable::iterator it = variables.find(varName);
		if (it != variables.end())
			return &it->second;

		return 0;
	}

	void			clear()
	{
		variables.clear();
	}

	friend std::ostream& operator<<(std::ostream &os, const SymbolTable &st);

private:
	typedef std::map<std::string, Variable> VariablesTable;

	VariablesTable	variables;
};

std::ostream& operator<<(std::ostream &os, const Variable &v);
std::ostream& operator<<(std::ostream &os, const SymbolTable &st);

#endif
