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
	
	#include "parsing_actors.h"
	#include "type_parsers.h"

	//------------------------------------------------------
	// Scn syntax
	struct ScnSyntax : public grammar<ScnSyntax>
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

				// Comment definition
				comment = ('#' >> *(anychar_p - eol_p));

				// Output definition
				outputPath = +((alnum_p | punct_p) - '"');
				output = ( "Output"	>> +blank_p >>
							'"' >> outputPath[scn_output_file(var(self.scene), CONSTRUCT_STR)] >> '"' >> +blank_p >>	// filepath
								   uint_p	 [scn_set_width (var(self.scene), arg1)]	>> +blank_p >>				// width
								   uint_p	 [scn_set_height(var(self.scene), arg1)]	>>							// height
							!(blank_p >> str_p("+z")[scn_store_z(var(self.scene), true)])							// store Z?
						  );

				// Background statement
				background = str_p("Background") >> +blank_p >> color4_p[scn_set_background(var(self.scene), arg1)];

				// CamLookAt statement
				camLookAt = str_p("CamLookAt")	>> +blank_p >> vec3f_p[assign_a(self.scene.camera().pos)] >> +blank_p
															>> vec3f_p[assign_a(self.scene.camera().target)];

				// PointLight statement
				pointLight = (str_p("PointLight") >> +blank_p	>> vec3f_p[assign_a(newPointLight_a::pos)] >> +blank_p
																>> color4_p[assign_a(newPointLight_a::color)]
							  )[newPointLight_a(self.scene)];
				
				// Grammar line definition & root.
				element =	  output
							| camLookAt
							| background
							| pointLight;
				line = *blank_p >> !element >> ending >> *blank_p;
				base_expression = *line;
			}

			const rule<ScannerT>& start() const	{ return base_expression; }

			// Generic types
			rule<ScannerT> ending;
			rule<ScannerT> string, variable;

			// Specific elements
			rule<ScannerT> comment;
			rule<ScannerT> output, outputPath;
			rule<ScannerT> background;
			rule<ScannerT> camLookAt;
			rule<ScannerT> pointLight;

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
