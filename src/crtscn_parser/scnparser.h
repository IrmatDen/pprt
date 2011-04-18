#ifndef PPRT_SCNPARSER_H
#define PPRT_SCNPARSER_H

#include "scene.h"

#include "../common.h"

#include <boost/spirit.hpp>
#include <boost/spirit/phoenix.hpp>
#include <boost/spirit/actor/clear_actor.hpp>
#include <boost/filesystem.hpp>

#include <algorithm>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

using namespace boost::spirit;
using namespace phoenix;

typedef char						char_t;
typedef file_iterator<char_t>		iterator_t;
typedef scanner<iterator_t>			scanner_t;
typedef rule<scanner_t>				rule_t;

#include "parsing_actors.h"

struct NonAlignedVec3
{
	NonAlignedVec3() {}
	NonAlignedVec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
	float x, y, z;

	operator Vector3() const { return Vector3(x, y, z); }
};
#include "../parser/type_parsers.h"

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
		private:
			// Temporary parsing data
			std::string					_str;
			std::vector<std::string>	_strVector;
			ShaderPath					_shaderPaths;

		public:
			definition(ScnSyntax const &self)
				: _shaderPaths(self.scene)
			{
				// Generic rules
					ending			= *blank_p >> !comment >> eol_p; 
					string			= confix_p( '"', (*(anychar_p & ~ch_p('"'))) [assign_a(_str)], '"');
					foldername		= '&' | +(alnum_p | '.' | ":/" | '/' | '_' | '-');
					foldersarray	= "[\"" >> list_p(foldername[push_back_a(_strVector)], ':') >> "\"]";

				// Comment definition
					comment = ('#' >> *(anychar_p - eol_p));

				// Camera definitions (RiSpec 3.2, §4.1.1)
					camera =	format;

					format =	"Format" >> +blank_p >>
								int_p	[bind(&Scene::setWidth)(var(self.scene), arg1)]		>> +blank_p >>		// width
								int_p	[bind(&Scene::setHeight)(var(self.scene), arg1)]	>> +blank_p >>		// height
								real_p;																			// pixel aspect ratio

				// Displays definitions (RiSpec 3.2, §4.1.2)
					displays =	display;

					display =	"Display" >> +blank_p >>
								string	[bind(&Scene::setDisplayName, &self.scene, boost::cref(_str))]	>> +blank_p >>	// name
								string	[displayType_a(self.scene)] >> +blank_p >>										// type
								string;																					// mode

				// Options (RiSpec 3.2, §4.1.4)
					option = "Option" >> +blank_p >> searchpath;

					searchpath = confix_p( '"', "searchpath", '"') >> +blank_p >> (spShader)[clear_a(_strVector)];
					spShader = confix_p( '"', "shader", '"') >> +blank_p >> foldersarray[assign_a(_shaderPaths, _strVector)];
							  

				// Scene definition
					scene =		background
							|	camLookAt;

					background = "Background" >> +blank_p >> color_p[bind(&Scene::setBackground)(var(self.scene), arg1)];

					camLookAt = "CamLookAt"	>> +blank_p >> vec3_p[assign_a(self.scene.camera().pos)] >> +blank_p
														>> vec3_p[assign_a(self.scene.camera().target)];

				// Graphics state
					graphicsState = color | opacity;
					
					color	= ("Color" >> +blank_p >> color_p[assign_a(currentColorOpa_a::color)]);
					opacity	= ("Opacity" >> +blank_p >> color_p[assign_a(currentColorOpa_a::opacity)]);

				// Lights definitions
					lights = pointLight;
					pointLight = ("PointLight" >> +blank_p	>> vec3_p[assign_a(newPointLight_a::pos)] >> +blank_p
															>> color_p[assign_a(newPointLight_a::color)]
								  )[newPointLight_a(self.scene)];

				// Geometry definitions
					geometries =	sphere
								|	plane
								|	disk;
					sphere	=	(	"Sphere" >> +blank_p >> real_p[assign_a(newSphere_a::radius)] >> +blank_p >>
															vec3_p[assign_a(newSphere_a::pos)] >> +blank_p >>
															string[assign_a(newSphere_a::matName, _str)] >>
															!(+blank_p >> shaderParams)
								)[newSphere_a(self.scene)];

					plane	= 	(	"Plane" >> +blank_p >>	vec3_p[assign_a(newPlane_a::normal)] >> +blank_p >>
															real_p[assign_a(newPlane_a::offset)] >> +blank_p >>
															!("TwoSided" >> +blank_p)[assign_a(newPlane_a::twoSided, true)] >>
															string[assign_a(newPlane_a::matName, _str)] >>
															!(+blank_p >> shaderParams)
								)[newPlane_a(self.scene)];

					disk	= 	(	"Disk" >> +blank_p >> real_p[assign_a(newDisk_a::radius)] >> +blank_p >>
														  vec3_p[assign_a(newDisk_a::pos)] >> +blank_p >>
														  vec3_p[assign_a(newDisk_a::normal)] >> +blank_p >>
														  string[assign_a(newDisk_a::matName, _str)] >>
														  !(+blank_p >> shaderParams)
								)[newDisk_a(self.scene)];

				// Material-related rules
					materialName = +(alnum_p | '_');
					shaderParams =	(
										'"' >> (+(alnum_p | '_'))[assign_a(shaderParams_a::paramName)] >> '"' >> +blank_p >>
										(color_p[shaderParams_a()] | real_p[shaderParams_a()])
									) % blank_p;
				
				// Grammar line definition & root.
					element =	  option
								| camera
								| displays
								| scene
								| lights
								| graphicsState
								| geometries;
					statement = *blank_p >> !element >> ending >> *blank_p;
					base_expression = *statement;
			}

			const rule<ScannerT>& start() const	{ return base_expression; }

			// Generic types
			rule<ScannerT> ending;
			rule<ScannerT> string, foldername, foldersarray;
			rule<ScannerT> comment;

			// Specific elements
			rule<ScannerT> option, searchpath, spShader;
			rule<ScannerT> camera, format;
			rule<ScannerT> displays, display;
			rule<ScannerT> scene, background, camLookAt;
			rule<ScannerT> graphicsState, color, opacity;
			rule<ScannerT> lights, pointLight;
			rule<ScannerT> geometries, sphere, plane, disk;
			rule<ScannerT> materialName;
			rule<ScannerT> shaderParams;

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

			if (info.full)
				return true;

			std::cout << "Parse error on line:" << std::endl;
			iterator_t end_error_line = std::find(info.stop, fileEnd, '\n');
			std::ostream_iterator<char_t> out_it(std::cout, "");
			std::copy(info.stop, end_error_line, out_it);
			std::cout << std::endl;
			return false;
		}

	private:
		Scene		&scene;
		ScnSyntax	syntax;
	};
}

#endif
