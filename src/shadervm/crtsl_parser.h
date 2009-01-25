#ifndef CRT_CRTCL_PARSER_H
#define CRT_CRTCL_PARSER_H

#include <string>
#include <iostream>

#include <boost/spirit.hpp>
#include <boost/spirit/phoenix.hpp>

using namespace boost::spirit;
using namespace phoenix;

typedef char						char_t;
typedef file_iterator<char_t>		iterator_t;
typedef scanner<iterator_t>			scanner_t;
typedef rule<scanner_t>				rule_t;
	
#include "crtsl_grammar.h"

namespace SLParser
{
	class Parser
	{
	public:
		Parser()//Scene &scn)
			//:scene(scn), syntax(scene)
		{
		}

		bool parseFile(const std::string &filename)
		{
			iterator_t fileBegin(filename);
			if (!fileBegin)
			{
				std::cerr << "Can't open " << filename << std::endl;
				return false;
			}
			iterator_t fileEnd = fileBegin.make_end();

			parse_info<iterator_t> info = parse(fileBegin, fileEnd, syntax,  blank_p);

			return info.full;
		}

	private:
		//Scene			&	scene;
		ShaderGrammar		syntax;
	};
}

#endif
