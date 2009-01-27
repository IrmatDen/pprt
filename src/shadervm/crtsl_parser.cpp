#include <iostream>

#include <boost/spirit.hpp>

#include "crtsl_parser.h"
#include "crtsl_grammar.h"
#include "symtab.h"

using namespace std;
using namespace boost;
using namespace boost::spirit;

using namespace SLParser;

Parser::Parser()
{
	syntax = new ShaderGrammar(*this);
	symTable = new SymbolTable();
}

Parser::~Parser()
{
	delete syntax;
	delete symTable;
}

bool Parser::parseFile(const string &filename)
{
	iterator_t fileBegin(filename);
	if (!fileBegin)
	{
		cerr << "Can't open " << filename << endl;
		return false;
	}
	iterator_t fileEnd = fileBegin.make_end();

	parse_info<iterator_t> info = parse(fileBegin, fileEnd, *syntax,  blank_p);

	// Display sym table
	cout << ".vars" << endl;
	cout << *symTable << endl << endl;

	// Display instructions
	cout << ".code" << endl;
	while (instructions.size() > 0)
	{
		cout << instructions.front() << endl;
		instructions.pop();
	}

	return info.full;
}

void Parser::endVariable()
{
	Variable var;

	while (varsTokens.size() > 0)
	{
		const string &s = varsTokens.front();

		if (s == "varying")
			var.storageType = VST_Varying;
		else if (s == "uniform")
			var.storageType = VST_Uniform;
		else if (s == "color")
			var.type = VT_Color;
		else
			var.name = s;
		
		varsTokens.pop();
	}

	symTable->addVar(var);
}

void Parser::endInstruction()
{
	while (stmtsTokens.size() > 0)
	{
		instructions.push(stmtsTokens.top());
		stmtsTokens.pop();
	}
}
