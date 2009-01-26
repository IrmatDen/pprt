#include "symtab.h"

using namespace std;

ostream& operator<<(ostream &os, const Variable &v)
{
	if (v.storageType == VST_Uniform)		os << "uniform ";
	else									os << "varying ";

	if (v.type == VT_Color)		os << "color ";
	else						os << "<unknown type> ";

	os << v.name;

	if (v.content)		os << " [initialized, node " << hex << v.content << "]";
	else				os << " [unitialized]";

	return os;
}

ostream& operator<<(ostream &os, const SymbolTable &st)
{
	os << "Symbol table details:" << endl;
	os << "Variables:" << endl;
	for(SymbolTable::VariablesTable::const_iterator it = st.variables.begin();
		it != st.variables.end();
		++it)
	{
		os << "\t" << *(it->second) << endl;
	}

	return os;
}
