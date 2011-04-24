#include "ribparser.h"

#include <algorithm>
#include <iostream>
#include <iterator>

using namespace RibParser;
using namespace std;

boost::spirit::classic::functor_parser<bool_parser>		bool_p;
boost::spirit::classic::functor_parser<color_parser>	color_p;
boost::spirit::classic::functor_parser<vec3_parser>		vec3_p;

Parser::Parser(Scene &scn)
	:scene(scn), syntax(scene)
{
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

	parse_info<iterator_t> info = parse(fileBegin, fileEnd, syntax);

	if (info.full)
		return true;

	cout << "Parse error on line:" << endl;
	iterator_t end_error_line = find(info.stop, fileEnd, '\n');
	copy(info.stop, end_error_line, ostream_iterator<char_t>(cout, ""));
	cout << endl;
	return false;
}
