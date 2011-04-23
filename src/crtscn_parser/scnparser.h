#ifndef PPRT_SCNPARSER_H
#define PPRT_SCNPARSER_H

#include "scene.h"

#include "../memory.h"

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

struct NonAlignedVec3
{
	NonAlignedVec3() {}
	NonAlignedVec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
	float x, y, z;

	operator Vector3() const	{ return Vector3(x, y, z); }
	operator Point3() const		{ return Point3(x, y, z); }
};
typedef std::vector<NonAlignedVec3> Vec3Array;

#include "parsing_actors.h"
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
			bool						_bool;
			int							_int;
			std::string					_str;
			std::vector<std::string>	_strVector;
			NonAlignedVec3				_vec3;
			Vec3Array					_pointVector;
			ShaderPath					_shaderPaths;

		public:
			definition(ScnSyntax const &self)
				: _shaderPaths(self.scene)
			{
				// Generic rules
					ending			= *blank_p >> !comment >> eol_p; 
					quotedString	= confix_p( '"', (*(anychar_p & ~ch_p('"'))) [assign_a(_str)], '"');
					foldername		= '&' | +(alnum_p | '.' | ":/" | '/' | '_' | '-');
					foldersArray	= "[\"" >> list_p(foldername[push_back_a(_strVector)], ':') >> "\"]";
					vec3			= vec3_p [assign_a(_vec3)];
					vec3Array		= "[" >> list_p(vec3 [push_back_a(_pointVector, _vec3)], ' ') >> "]";
					singleBoolArray	= "[" >> bool_p [assign_a(_bool)] >> "]";

				// Comment definition
					comment = ('#' >> *(anychar_p - eol_p));

				// Attibutes (RiSpec 3.2, §4)
					worldBegin	= str_p("WorldBegin") [worldBegin_a(self.scene)];
					worldEnd	= str_p("WorldEnd");

				// Camera definitions (RiSpec 3.2, §4.1.1)
					camera =	format | projection | clipping;

					format		=	"Format" >> +blank_p >>
									int_p	[assign_a(CameraSettings::resX)]	>> +blank_p >>		// width
									int_p	[assign_a(CameraSettings::resY)]	>> +blank_p >>		// height
									real_p	[assign_a(CameraSettings::pix_aspectRatio)];			// pixel aspect ratio

					projection	=	"Projection" >> +blank_p >> (perspProj | orthoProj);
					perspProj	=	str_p("\"perspective\"") [assign_a(CameraSettings::projType)] >>
									!(+blank_p >> "\"fov\"" >> +blank_p >> real_p [assign_a(CameraSettings::fov)]);
					orthoProj	=	str_p("\"orthographic\"") [assign_a(CameraSettings::projType)];

					clipping	=	"Clipping" >> +blank_p >>
									real_p	[assign_a(CameraSettings::hither)] >> +blank_p >>
									real_p	[assign_a(CameraSettings::yon)];

				// Displays definitions (RiSpec 3.2, §4.1.2)
					displays =	display;

					display =	"Display" >> +blank_p >>
								quotedString	[bind(&Scene::setDisplayName, &self.scene, boost::cref(_str))]	>> +blank_p >>	// name
								quotedString	[displayType_a(self.scene)] >> +blank_p >>										// type
								quotedString;																					// mode

				// Options (RiSpec 3.2, §4.1.4)
					option = "Option" >> +blank_p >> (searchpath | usethreads);

					searchpath = "\"searchpath\"" >> +blank_p >> (spShader)[clear_a(_strVector)];
					spShader = "\"shader\"" >> +blank_p >> foldersArray[assign_a(_shaderPaths, _strVector)];

					usethreads = "\"usethreads\"" >> +blank_p >> singleBoolArray[bind(&Scene::enableThreading, &self.scene, boost::cref(_bool))];
							  

				// Scene definition
					scene	= background;

					background = "Background" >> +blank_p >> color_p[bind(&Scene::setBackground)(var(self.scene), arg1)];

				// Graphics state
					graphicsState = attributeBegin | attributeEnd | color | opacity | surface;

					attributeBegin	= str_p("AttributeBegin")	[attributeBegin_a()];
					attributeEnd	= str_p("AttributeEnd")		[attributeEnd_a()];
					
					color	= ("Color"		>> +blank_p >> color_p[assign_a(GraphicStateStack::current._color)]);
					opacity	= ("Opacity"	>> +blank_p >> color_p[assign_a(GraphicStateStack::current._opacity)]);
					
					shaderParams =	(	quotedString [assign_a(shaderParams_a::paramName, _str)] >> +blank_p >>
										(color_p[shaderParams_a()] | real_p[shaderParams_a()])
									) % blank_p;
					surface	=	"Surface" >> +blank_p >>
								quotedString [assign_a(GraphicStateStack::current.surfaceShader, _str)] >>
								!(+blank_p >> shaderParams [activateCurrentShaderParams_a()]);

				// Lights definitions
					lights = pointLight;
					pointLight = ("PointLight" >> +blank_p	>> vec3_p[assign_a(newPointLight_a::pos)] >> +blank_p
															>> color_p[assign_a(newPointLight_a::color)]
								 )[newPointLight_a(self.scene)];
				
				// Transformations (RiSpec 3.2, §4.3)
					transform	= identity | translate | rotate
								| transformBegin | transformEnd;

					identity	=	str_p("Identity") [identity_a()];
					translate	=	"Translate" >> +blank_p >> vec3_p[translate_a()];
					rotate		=	("Rotate"	>> +blank_p >> real_p [assign_a(rotate_a::angleDegrees)]
												>> +blank_p >> vec3_p [assign_a(rotate_a::axis)]
									) [rotate_a()];

					transformBegin	= str_p("TransformBegin")	[transformBegin_a()];
					transformEnd	= str_p("TransformEnd")		[transformEnd_a()];

				// Geometry definitions
					geometries =	polygon
								|	sphere
								|	disk;

					polygon =	(	"Polygon" >> +blank_p >> "\"P\"" >> +blank_p >> vec3Array [assign_a(newPolygon_a::points, _pointVector)]
								) [newPolygon_a(self.scene)];

					sphere	=	(	"Sphere" >> +blank_p >> real_p[assign_a(newSphere_a::radius)]
								) [newSphere_a(self.scene)];

					disk	= 	(	"Disk" >> +blank_p >> real_p[assign_a(newDisk_a::radius)]	>> +blank_p >>
														  vec3_p[assign_a(newDisk_a::pos)]		>> +blank_p >>
														  vec3_p[assign_a(newDisk_a::normal)]	>> +blank_p
								) [newDisk_a(self.scene)];
				
				// Grammar line definition & root.
					element =	  option
								| camera
								| displays
								| scene
								| lights
								| graphicsState
								| transform
								| worldBegin | worldEnd
								| geometries;
					statement = *blank_p >> !element >> ending >> *blank_p;
					base_expression = *statement;
			}

			const rule<ScannerT>& start() const	{ return base_expression; }

			// General types
			rule<ScannerT> ending;
			rule<ScannerT> quotedString, foldername, foldersArray, singleBoolArray, vec3, vec3Array;
			rule<ScannerT> comment;

			// Specific elements
			rule<ScannerT> worldBegin, worldEnd;
			rule<ScannerT> option, searchpath, spShader, usethreads;
			rule<ScannerT> camera, format, projection, perspProj, orthoProj, clipping;
			rule<ScannerT> displays, display;
			rule<ScannerT> scene, background;
			rule<ScannerT> graphicsState, attributeBegin, attributeEnd, color, opacity, shaderParams, surface;
			rule<ScannerT> lights, pointLight;
			rule<ScannerT> transform, identity, translate, rotate, transformBegin, transformEnd;
			rule<ScannerT> geometries, polygon, sphere, plane, disk;

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
