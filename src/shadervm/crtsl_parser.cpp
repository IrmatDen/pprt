#include <iostream>
#include <sstream>

#include <boost/spirit.hpp>

#include "crtsl_parser.h"
#include "crtsl_grammar.h"
#include "symtab.h"
#include "compiled_shader.h"

#include "../scene/scene.h"

using namespace std;
using namespace boost;
using namespace boost::spirit;

using namespace SLParser;

Parser::Parser(Scene &scn)
:scene(scn)
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

	if (!info.full)
	{
		// Discard all instructions this shader generated.
		shaderName = "";
		symTable->clear();
		varsTokens = VariablesStack();
		stmtsTokens = StatementsStack();
		instructions = Instructions();
		return false;
	}

	string mnemonics;
	programAsMnemonics(mnemonics);
	scene.shaderManager.addShaderFromMnemonics(mnemonics);

	return true;
}

void Parser::programAsMnemonics(string &str) const
{
	Instructions otherInstr(instructions);
	ostringstream oss;
	
	oss << ".shaderid " << shaderName << endl << endl;

	oss << ".vars" << endl;
	oss << *symTable << endl;

	// Display instructions
	oss << ".code" << endl;
	while (otherInstr.size() > 0)
	{
		oss << otherInstr.front() << endl;
		otherInstr.pop();
	}

	str = oss.str();
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
