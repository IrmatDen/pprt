#ifndef CRT_SCNPARSER_H
#define CRT_SCNPARSER_H

#include <string>
#include <iostream>
#include <vector>

#include <boost/spirit.hpp>
#include <boost/spirit/phoenix.hpp>

#include "scene.h"

using namespace boost::spirit;
using namespace phoenix;
				
//! Deferred string construction based on 2 iterators.
#define CONSTRUCT_STR			construct_<std::string>(arg1, arg2)

namespace ScnParser
{
	typedef char						char_t;
	typedef file_iterator<char_t>		iterator_t;
	typedef scanner<iterator_t>			scanner_t;
	typedef rule<scanner_t>				rule_t;

	//------------------------------------------------------
	// Background scene color from 4 reals
	struct scn_background_a
	{
		scn_background_a(Scene *scn = NULL) : scene(scn) {}

		void operator()(const iterator_t&, const iterator_t&) const
		{
			if (scene)
			{
				Color4 bgCol((float)channel_values[0], (float)channel_values[1], (float)channel_values[2], (float)channel_values[3]);
				scene->setBackground(bgCol);
			}
		}

		Scene *scene;
		static std::vector<double> channel_values;
	};
	std::vector<double> scn_background_a::channel_values;

	//------------------------------------------------------
	// Scn syntax
	struct ScnSyntax : public boost::spirit::grammar<ScnSyntax>
	{
	private:
		// Scene to load the file into.
		Scene &scene;

	public:
		ScnSyntax(Scene &scn) : scene(scn)		{}
		virtual ~ScnSyntax()					{}

		template <typename ScannerT>
		struct definition
		{
		public:
			definition(ScnSyntax const &self)
			{
				// Generic rules
				ending		= *blank_p >> !comment >> eol_p; 
				string		= '"' >> (+((alnum_p | punct_p) - '"')) >> '"';
				variable	= string | real_p;
				r01			= limit_d(0, 1)[real_p];

				// Comment definition
				comment = ('#' >> *(anychar_p - eol_p));

				// Output definition
				member_function_ptr<void, Scene, const std::string&>	scn_output_file	= bind(&Scene::setOutputFile);
				member_function_ptr<void, Scene, int>					scn_set_width	= bind(&Scene::setWidth);
				member_function_ptr<void, Scene, int>					scn_set_height	= bind(&Scene::setHeight);
				member_function_ptr<void, Scene, bool>					scn_store_z		= bind(&Scene::storeZValues);

				outputPath = +((alnum_p | punct_p) - '"');
				output = ( "Output"	>> blank_p >>
							'"' >> outputPath[scn_output_file(var(self.scene), CONSTRUCT_STR)] >> '"' >> blank_p >>	// filepath
							uint_p		[scn_set_width(var(self.scene), arg1)]	>> blank_p >>						// width
							uint_p		[scn_set_height(var(self.scene), arg1)]	>>									// height
							!(blank_p >> str_p("+z")[scn_store_z(var(self.scene), true)])					// store Z?
						  );

				// Background statement
				//! FUCKING limit_d breaks the parsing done by real_p, and ask the functor to accept 2 iterators (not working).
				background = str_p("Background ") >>
								'(' >>(real_p[push_back_a(scn_background_a::channel_values)] >> !blank_p >> ',' >> !blank_p >>
									   real_p[push_back_a(scn_background_a::channel_values)] >> !blank_p >> ',' >> !blank_p >>
									   real_p[push_back_a(scn_background_a::channel_values)] >> !blank_p >> ',' >> !blank_p >>
									   real_p[push_back_a(scn_background_a::channel_values)]
								      )[scn_background_a(&self.scene)]
								 >> ')';

				// Grammar line & root.
				element = output | background;
				line = *blank_p >> !element >> ending >> *blank_p;
				base_expression = *line;
			}

			const rule<ScannerT>& start() const	{ return base_expression; }

			// Generic types
			rule<ScannerT> ending, string, variable;
			rule<ScannerT> r01;	// real value in [0, 1]

			// Specific elements
			rule<ScannerT> comment;
			rule<ScannerT> output, outputPath;
			rule<ScannerT> background;

			// General description
			rule<ScannerT> element, line, base_expression;
		};
	};

	//------------------------------------------------------
	// .scn parser
	class Parser
	{
	public:
		Parser(Scene &scn)
			:scene(scn), syntax(scene)
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

			parse_info<iterator_t> info = parse(fileBegin, fileEnd, syntax);

			return info.full;
		}

	private:
		Scene		&scene;
		ScnSyntax	syntax;
	};
}

#endif
