#ifndef CRT_CRTSL_GRAMMAR_H
#define CRT_CRTSL_GRAMMAR_H

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

#include "../scene/scene.h"
	
//#include "../parser/type_parsers.h"

struct debug_a
{
	void operator()(const iterator_t &begin, const iterator_t &end) const
	{
		std::cout << "Token found: " << std::string(begin, end) << std::endl;
	}
};

namespace SLParser
{
	// Syntax GREATLY inspired from RenderMan!
	struct ShaderGrammar : public grammar<ShaderGrammar>
	{
	private:
		// Scene to load the file into.
		//Scene &scene;

	public:
		ShaderGrammar(){}//Scene &scn) : scene(scn)		{}
		virtual ~ShaderGrammar()					{}

		template <typename ScannerT>
		struct definition
		{
		public:
			definition(ShaderGrammar const &self)
			{
				// Declarations
				definitions			=	shader_definition >> ending;
				shader_definition	=	shader_type >> ending >> identifier >> *blank_p >> '(' >> *blank_p >> ')' >> ending >>
											'{' >> +ending >> +statements >>
											'}';
				shader_type			=	str_p("surface");

				// Statements
				statements			=	+(*blank_p >> statement >> *blank_p >> ';' >> +ending);
				statement			=	assignexpression;

				// Expressions
				expression			=	primary;
				primary				=	real_p | procedurecall | identifier | assignexpression;
				assignexpression	=	identifier >> *blank_p >> asgnop  >> *blank_p >> expression;
				procedurecall		=	identifier >> *blank_p >> '(' >> *blank_p >> *proc_arguments >> *blank_p >> ')';
				proc_arguments		=	*(expression >> ',' >> *blank_p) >> expression;

				// Operators
				asgnop				=	ch_p('=');

				// General
				comment				=	'#' >> *(anychar_p - eol_p);
				ending				=	*blank_p >> !comment >> eol_p; 
				identifier			=	+(alnum_p | '_');
			}

			const rule<ScannerT>& start() const	{ return definitions; }

			// Declarations
			rule<ScannerT>	definitions;
			rule<ScannerT>	shader_definition;
			rule<ScannerT>	shader_type;

			// Statements
			rule<ScannerT>	statements;
			rule<ScannerT>	statement;

			// Expressions
			rule<ScannerT>	expression;
			rule<ScannerT>	primary;
			rule<ScannerT>	procedurecall;
			rule<ScannerT>	proc_arguments;
			rule<ScannerT>	assignexpression;

			// Operators;
			rule<ScannerT>	asgnop;

			// General
			rule<ScannerT>	ending;
			rule<ScannerT>	comment;
			rule<ScannerT>	identifier;
		};
	};
}

#endif
