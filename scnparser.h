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

typedef char						char_t;
typedef file_iterator<char_t>		iterator_t;
typedef scanner<iterator_t>			scanner_t;
typedef rule<scanner_t>				rule_t;
	
#include "parsing_actors.h"
#include "type_parsers.h"
				
//! Deferred string construction based on 2 iterators.
#define CONSTRUCT_STR			construct_<std::string>(arg1, arg2)

namespace ScnParser
{
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
				string		= +(alnum_p | punct_p | '_');

				// Comment definition
				comment = ('#' >> *(anychar_p - eol_p));

				// Scene definition
					scene =		output
							|	background
							|	camLookAt;

					output = ( "Output"	>> +blank_p >>
								string	[bind(&Scene::setOutputFile)(var(self.scene), CONSTRUCT_STR)]  >> +blank_p >>	// filepath
								uint_p	[bind(&Scene::setWidth) (var(self.scene), arg1)]	>> +blank_p >>				// width
								uint_p	[bind(&Scene::setHeight)(var(self.scene), arg1)]	>>							// height
								!(blank_p >> str_p("+z")[bind(&Scene::storeZValues)(var(self.scene), true)])				// store Z?
							  );

					background = "Background" >> +blank_p >> color4_p[bind(&Scene::setBackground)(var(self.scene), arg1)];

					camLookAt = "CamLookAt"	>> +blank_p >> vec3f_p[assign_a(self.scene.camera().pos)] >> +blank_p
														>> vec3f_p[assign_a(self.scene.camera().target)];

				// Lights definitions
					lights = pointLight;
					pointLight = ("PointLight" >> +blank_p	>> vec3f_p[assign_a(newPointLight_a::pos)] >> +blank_p
															>> color4_p[assign_a(newPointLight_a::color)]
								  )[newPointLight_a(self.scene)];

				// Material definition
					material = ("Material" >> +blank_p >> (+alnum_p)[assign_a(newMaterial_a::name)] >> ending >>
								'{' >> ending >>
									+(	(*blank_p >> "Color"		>> +blank_p >> color4_p[assign_a(newMaterial_a::color)] >> ending) |
										(*blank_p >> "Reflexivity"	>> +blank_p >> real_p[assign_a(newMaterial_a::reflexivity)] >> ending)
									) >>
								'}'
							   )[newMaterial_a(self.scene)];

				// Geometry definitions
					geometries =	sphere
								|	plane
								|	disk;
					sphere	=	(	"Sphere" >> +blank_p >> real_p[assign_a(newSphere_a::radius)] >> +blank_p >>
															vec3f_p[assign_a(newSphere_a::pos)] >> +blank_p >>
															(+alnum_p)[assign_a(newSphere_a::matName)]
								)[newSphere_a(self.scene)];

					plane	= 	(	"Plane" >> +blank_p >> vec3f_p[assign_a(newPlane_a::normal)] >> +blank_p >>
															real_p[assign_a(newPlane_a::offset)] >> +blank_p >>
															!("TwoSided" >> +blank_p)[assign_a(newPlane_a::twoSided, true)] >>
															(+alnum_p)[assign_a(newPlane_a::matName)]
								)[newPlane_a(self.scene)];

					disk	= 	(	"Disk" >> +blank_p >> real_p[assign_a(newDisk_a::radius)] >> +blank_p >>
														  vec3f_p[assign_a(newDisk_a::pos)] >> +blank_p >>
														  vec3f_p[assign_a(newDisk_a::normal)] >> +blank_p >>
														  (+alnum_p)[assign_a(newDisk_a::matName)]
								)[newDisk_a(self.scene)];
				
				// Grammar line definition & root.
					element =	  scene
								| lights
								| material
								| geometries;
					statement = *blank_p >> !element >> ending >> *blank_p;
					base_expression = *statement;
			}

			const rule<ScannerT>& start() const	{ return base_expression; }

			// Generic types
			rule<ScannerT> ending;
			rule<ScannerT> string;

			// Specific elements
			rule<ScannerT> comment;
			rule<ScannerT> scene, output, background, camLookAt;
			rule<ScannerT> lights, pointLight;
			rule<ScannerT> material;
			rule<ScannerT> geometries, sphere, plane, disk;

			// General description
			rule<ScannerT> element, statement, base_expression;
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
