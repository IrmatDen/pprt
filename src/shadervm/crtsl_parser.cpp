#include <iostream>

#include <boost/spirit.hpp>

#include "crtsl_parser.h"
#include "crtsl_grammar.h"

using namespace std;
using namespace boost;
using namespace boost::spirit;

using namespace SLParser;

Parser::Parser()
{
	syntax = new ShaderGrammar(*this);
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

	// Display instructions
	while (instructions.size() > 0)
	{
		cout << instructions.front() << endl;
		instructions.pop();
	}

	return info.full;
}
