#ifndef CRT_CRTSL_GRAMMAR_H
#define CRT_CRTSL_GRAMMAR_H

#include <string>
#include <iostream>

#include <boost/spirit.hpp>
#include <boost/spirit/phoenix.hpp>

#include "crtsl_parser.h"

using namespace boost::spirit;
using namespace phoenix;

typedef char						char_t;
typedef file_iterator<char_t>		iterator_t;
typedef scanner<iterator_t>			scanner_t;
typedef rule<scanner_t>				rule_t;

#include "crtsl_shader_actors.h"

namespace SLParser
{

	// Syntax GREATLY inspired from RenderMan SL!
	struct ShaderGrammar : public grammar<ShaderGrammar>
	{
	private:
		Parser &parser;

	public:
		ShaderGrammar(Parser &p) : parser(p)		{}
		virtual ~ShaderGrammar()					{}

		template <typename ScannerT>
		struct definition
		{
		public:
			definition(ShaderGrammar const &self)
			{
				// Declarations
				definitions			=	shader_definition >> ending;
				shader_definition	=	shader_type >> ending >>
											identifier >> '(' >> ')' >> ending >>
											'{' >> +ending >>
												!variables >> *ending >>
												+statements >>
											'}';
				shader_type			=	str_p("surface");

				variables			=	variable_definitions % +ending;
				variable_definitions=	+(typespec >> def_expressions >> ';')[endInstruction_a(self.parser)];
				typespec			=	!detail[varToken_a(self.parser)] >> type[varToken_a(self.parser)];
				def_expressions		=	def_expression[endVariable_a(self.parser)] % ',';
				def_expression		=	(varname ^ outKeywords)[varAssignOp_a(self.parser)] >> (!def_init)[varCheckDiscard_a(self.parser)];
				def_init			=	'=' >> expression;
				detail				=	str_p("varying") | "uniform";
				type				=	str_p("color");

				// Statements
				statements			=	+(+statement >> +ending);
				statement			=	(assignexpression >> ';')[endInstruction_a(self.parser)];

				// Expressions
				expression			=	primary;
				primary				=		real_p[push_a(self.parser)]
										|	varname[push_a(self.parser)]
										|	procedurecall
										|	assignexpression;
				assignexpression	=	(varname[assignOp_a(self.parser)] >> asgnop >> expression);
				procedurecall		=	(identifier[procCall_a(self.parser)] >> '(' >> !proc_arguments >> ')');
				proc_arguments		=	expression % ',';

				// Operators
				asgnop				=	ch_p('=');


				// General
				comment				=	'#' >> *(anychar_p - eol_p);
				ending				=	!comment >> +eol_p;
				identifier			=	(alpha_p | '_') >> *(alnum_p | '_');
				outKeywords			=	str_p("out");
				//inKeywords			=	str_p("");
				varname				=	identifier ^ type;// ^ inKeywords;
			}

			const rule<ScannerT>& start() const	{ return definitions; }

			// Declarations
			rule<ScannerT>	definitions;
			rule<ScannerT>	shader_definition;
			rule<ScannerT>	shader_type;
			rule<ScannerT>	variables;
			rule<ScannerT>	variable_definitions;
			rule<ScannerT>	typespec;
			rule<ScannerT>	def_expressions, def_expression, def_init;
			rule<ScannerT>	detail;
			rule<ScannerT>	type;

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
			rule<ScannerT>	varname, outKeywords, /*inKeywords,*/ identifier;
		};
	};
}

#endif
