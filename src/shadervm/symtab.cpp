#include "symtab.h"

using namespace std;

ostream& operator<<(ostream &os, const Variable &v)
{
	if (v.storageType == VST_Uniform)		os << "uniform ";
	else									os << "varying ";

	if (v.type == VT_Color)		os << "color ";
	else						os << "<unknown type> ";

	os << v.name;

	return os;
}

ostream& operator<<(ostream &os, const SymbolTable &st)
{
	for(SymbolTable::VariablesTable::const_iterator it = st.variables.begin();
		it != st.variables.end();
		++it)
	{
		os << it->second << endl;
	}

	return os;
}
