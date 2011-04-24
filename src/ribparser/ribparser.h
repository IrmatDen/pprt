#ifndef PPRT_RIBPARSER_H
#define PPRT_RIBPARSER_H

#include "../scene/scene.h"

#include "parser_typedefs.h"
#include "parsing_actors.h"
#include "type_parsers.h"

#include <boost/spirit/include/classic.hpp>
#include <boost/spirit/include/phoenix1.hpp>
#include <boost/spirit/include/classic_clear_actor.hpp>

#include <string>
#include <vector>

using namespace boost::spirit::classic;
using namespace phoenix;

typedef scanner<iterator_t>	scanner_t;
typedef rule<scanner_t>		rule_t;

namespace RibParser
{
	struct RibSyntax : public grammar<RibSyntax>
	{
	private:
		// Scene to load the file into.
		Scene &scene;

	public:
		RibSyntax(Scene &scn) : scene(scn)		{}
		virtual ~RibSyntax()					{}

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
			definition(RibSyntax const &self)
				: _shaderPaths(self.scene)
			{
                // Generic rules
                    ending = *blank_p >> !comment >> eol_p;
                    separation = ( *blank_p >> !( ending >> *blank_p ) );
                    quotedString = confix_p( '"', (*(anychar_p & ~ch_p('"'))) [assign_a(_str)], '"');

                    foldername
                        = '&'
                        | +( alnum_p | '.' | ":/" | '/' | '_' | '-' )
                        ;

                    foldersArray
                        =   "[\""
                            >> list_p( foldername [push_back_a(_strVector)], ':' )
                            >> "\"]"
                        ;

                    vec3 = vec3_p [assign_a(_vec3)];
                    vec3Array
                        =   str_p("[") [clear_a(_pointVector)]
                            >> (vec3 [push_back_a(_pointVector, _vec3)]) % separation
                            >> "]"
                        ;

                    singleBoolArray	= "[" >> bool_p [assign_a(_bool)] >> "]";

                // Comment definition
                    comment = ('#' >> *(anychar_p - eol_p));

                // Data streams (P, N, Cs...)
                    pointStream
                        =   str_p("\"P\"") [clear_a(DataStream::Ps)]
                            >> +blank_p
                            >> vec3Array [assign_a(DataStream::Ps, _pointVector)]
                        ;
                    normalStream
                        =   str_p("\"N\"") [clear_a(DataStream::Ns)]
                            >> +blank_p
                            >> vec3Array [assign_a(DataStream::Ns, _pointVector)]
                        ;
                    colorStream
                        =   str_p("\"Cs\"") [clear_a(DataStream::Css)]
                            >> +blank_p
                            >> vec3Array [assign_a(DataStream::Css, _pointVector)]
                        ;
                    opacityStream
                        =   str_p("\"Os\"") [clear_a(DataStream::Oss)]
                            >> +blank_p
                            >> vec3Array [assign_a(DataStream::Oss, _pointVector)]
                        ;

                // Attibutes (RiSpec 3.2, §4)
                    worldBegin	= str_p("WorldBegin") [worldBegin_a(self.scene)];
                    worldEnd	= str_p("WorldEnd");

                // Camera definitions (RiSpec 3.2, §4.1.1)
                    camera =	format | projection | clipping;

                    format
                        =   "Format"
                            >> +blank_p
                            >> int_p [assign_a(CameraSettings::resX)]
                            >> +blank_p
                            >> int_p [assign_a(CameraSettings::resY)]
                            >> +blank_p
                            >> real_p [assign_a(CameraSettings::pix_aspectRatio)]
                        ;

                    projection = "Projection" >> +blank_p >> (perspProj | orthoProj);
                    perspProj
                        =   str_p("\"perspective\"") [assign_a(CameraSettings::projType)]
                            >> !(   +blank_p
                                    >> "\"fov\""
                                    >> +blank_p
                                    >> real_p [assign_a(CameraSettings::fov)]
                                )
                        ;
                    orthoProj =	str_p("\"orthographic\"") [assign_a(CameraSettings::projType)];

                    clipping
                        =   "Clipping" >> +blank_p
                            >> real_p [assign_a(CameraSettings::hither)] >> +blank_p
                            >> real_p [assign_a(CameraSettings::yon)];

                // Displays definitions (RiSpec 3.2, §4.1.2)
                    displays =	pixelSamples | display;

                    pixelSamples
                        =	"PixelSamples"
                            >> +blank_p
                            >> real_p [bind(&Scene::setXPixelSamples)(var(self.scene), arg1)]
                            >> +blank_p
                            >> real_p [bind(&Scene::setYPixelSamples)(var(self.scene), arg1)]
                        ;

                    display
                        =	"Display"
                            >> +blank_p
                            >> quotedString	[bind(&Scene::setDisplayName, &self.scene, boost::cref(_str))]
                            >> +blank_p
                            >> quotedString	[displayType_a(self.scene)]
                            >> +blank_p
                            >> quotedString
                        ;

                // Options (RiSpec 3.2, §4.1.4)
                    option = "Option" >> +blank_p >> (searchpath | usethreads);

                    searchpath  = "\"searchpath\""  >> +blank_p >> spShader [clear_a(_strVector)];
                    spShader    = "\"shader\""      >> +blank_p >> foldersArray [assign_a(_shaderPaths, _strVector)];

                    usethreads
                        =	"\"usethreads\""
                            >> +blank_p
                            >> singleBoolArray [bind(&Scene::enableThreading, &self.scene, boost::cref(_bool))]
                        ;
							  

                // Scene definition
                    scene = background;

                    background
                        =	"Background"
                            >> +blank_p
                            >> color_p [bind(&Scene::setBackground)(var(self.scene), arg1)]
                        ;

                // Graphics state
                    graphicsState = attributeBegin | attributeEnd | color | opacity | surface;

                    attributeBegin	= str_p("AttributeBegin")	[attributeBegin_a()];
                    attributeEnd	= str_p("AttributeEnd")		[attributeEnd_a()];
					
                    color	= "Color"		>> +blank_p >> color_p [assign_a(GraphicStateStack::current._color)];
                    opacity	= "Opacity"	>> +blank_p >> color_p [assign_a(GraphicStateStack::current._opacity)];
					
                    shaderParams
                        =	(	quotedString [assign_a(shaderParams_a::paramName, _str)]
                                >> +blank_p
                                >> (color_p [shaderParams_a()] | real_p [shaderParams_a()])
                            ) % blank_p
                        ;
                    surface
                        =	"Surface"
                            >> +blank_p
                            >> quotedString [assign_a(GraphicStateStack::current.surfaceShader, _str)]
                            >> !( +blank_p >> shaderParams [activateCurrentShaderParams_a()] )
                        ;

                // Lights definitions
                    lights = pointLight;

                    pointLight
                        =	(	"PointLight"
                                >> +blank_p
                                >> vec3_p [assign_a(newPointLight_a::pos)]
                                >> +blank_p
                                >> color_p[assign_a(newPointLight_a::color)]
                            ) [newPointLight_a(self.scene)]
                        ;
				
                // Transformations (RiSpec 3.2, §4.3)
                    transform	= identity | translate | rotate
                                | transformBegin | transformEnd;

                    identity	=	str_p("Identity") [identity_a()];
                    translate	=	"Translate" >> +blank_p >> vec3_p[translate_a()];
                    rotate
                        =	(	"Rotate"
                                >> +blank_p
                                >> real_p [assign_a(rotate_a::angleDegrees)]
                                >> +blank_p
                                >> vec3_p [assign_a(rotate_a::axis)]
                            ) [rotate_a()]
                        ;

                    transformBegin	= str_p("TransformBegin")	[transformBegin_a()];
                    transformEnd	= str_p("TransformEnd")		[transformEnd_a()];

                // Geometry definitions
                    geometries  =   polygon
                                |	sphere
                                |	disk;

                    polygon
                        =	(	str_p("Polygon") [resetGeomStreams_a()]
                                >> *( separation >> ( pointStream | normalStream | colorStream | opacityStream ) )
                            ) [newPolygon_a(self.scene)]
                        ;

                    sphere
                        =	(	"Sphere"
                                >> +blank_p
                                >> real_p[assign_a(newSphere_a::radius)]
                            ) [newSphere_a(self.scene)]
                        ;

                    disk
                        = 	(	"Disk"
                                >> +blank_p
                                >> real_p [assign_a(newDisk_a::radius)]
                                >> +blank_p
                                >> vec3_p [assign_a(newDisk_a::normal)]
                                >> +blank_p
                            ) [newDisk_a(self.scene)]
                        ;
				
                // Grammar line definition & root.
                element =   option
                        |   camera
                        |   displays
                        |   scene
                        |   lights
                        |   graphicsState
                        |   transform
                        |   worldBegin | worldEnd
                        |   geometries;

                statement = *blank_p >> !element >> ending >> *blank_p;
                base_expression = *statement;
			}

			const rule<ScannerT>& start() const	{ return base_expression; }

			// General types
			rule<ScannerT> separation, ending;
			rule<ScannerT> quotedString, foldername, foldersArray, singleBoolArray, vec3, vec3Array;
			rule<ScannerT> comment;

			// Data streams (P, N, Cs...)
			rule<ScannerT> pointStream, normalStream, colorStream, opacityStream;

			// Specific elements
			rule<ScannerT> worldBegin, worldEnd;
			rule<ScannerT> option, searchpath, spShader, usethreads;
			rule<ScannerT> camera, format, projection, perspProj, orthoProj, clipping;
			rule<ScannerT> displays, pixelSamples, display;
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
		Parser(Scene &scn);
		bool parseFile(const std::string &filename);

	private:
		Scene		&scene;
		RibSyntax	syntax;
	};
}

#endif
