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
	
#include "crtsl_shader_actors.h"

struct debug_a
{
	void operator()(const iterator_t &begin, const iterator_t &end) const
	{
		std::cout << "Token found: " << std::string(begin, end) << std::endl;
	}

	template<typename ValueT>
	void operator()(const ValueT &v) const
	{
		std::cout << v << std::endl;
	}
};

namespace SLParser
{
	// Syntax GREATLY inspired from RenderMan SL!
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
				shader_definition	=	shader_type >> ending >>
											identifier >> '(' >> ')' >> ending >>
											'{' >> +ending >>
												!variables >> *ending >>
												+statements >>
											'}';
				shader_type			=	str_p("surface");

				variables			=	variable_definitions % +ending;
				variable_definitions=	+(typespec >> def_expressions >> ';');
				typespec			=	!detail >> type;
				def_expressions		=	def_expression % ',';
				def_expression		=	varname >> !def_init;
				def_init			=	'=' >> expression;
				detail				=	str_p("varying") | "uniform";
				type				=	str_p("color");

				// Statements
				statements			=	+(+statement >> +ending);
				statement			=	assignexpression >> ';';

				// Expressions
				expression			=	primary;
				primary				=	real_p | procedurecall | identifier[debug_a()] | assignexpression;
				assignexpression	=	(varname >> asgnop >> expression)[assignOp_a()];
				procedurecall		=	(identifier >> '(' >> !proc_arguments >> ')')[procCall_a()];
				proc_arguments		=	expression[pushArg_a()] % ',';

				// Operators
				asgnop				=	ch_p('=');


				// General
				comment				=	'#' >> *(anychar_p - eol_p);
				ending				=	!comment >> +eol_p;
				identifier			=	+(alnum_p | '_');
				varname				=	identifier ^ type;
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
			rule<ScannerT>	varname, identifier;
		};
	};
}

#endif
