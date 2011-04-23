#ifndef PPRT_PARSING_ACTORS_H
#define PPRT_PARSING_ACTORS_H

// IMPORTANT: this file can't be used alone, it's only to be included in scnparser.h which
// contains the good includes and typedefs!

#include <boost/filesystem.hpp>
#include <iostream>

#include "../memory.h"
#include "../shadervm/shader_param.h"

template <typename T>
inline float deg2rad(T deg) { return static_cast<float>(deg * 0.01745329252); }

//-----------------------------------------------------------------------------------------------------------
// Transform actors

struct TransformStack
{
	typedef std::stack<Matrix4, std::deque<Matrix4, memory::AllocAlign16<Matrix4>>> XformStack;

	static XformStack	stack;
	static Matrix4		currentTransform;
};

TransformStack::XformStack	TransformStack::stack;
Matrix4						TransformStack::currentTransform(Matrix4::identity());

struct identity_a
{
	void operator()(const iterator_t&, const iterator_t&) const
	{
		TransformStack::currentTransform = Matrix4::identity();
	}
};

struct translate_a
{
	void operator()(const NonAlignedVec3 &vec) const
	{
		TransformStack::currentTransform = TransformStack::currentTransform * Matrix4::translation(vec);
	}
};

struct rotate_a
{
	void operator()(const iterator_t&, const iterator_t&) const
	{
		const Matrix4 rot = Matrix4::rotation(deg2rad(angleDegrees), normalize(axis));
		TransformStack::currentTransform = TransformStack::currentTransform * rot;
	}

	static double	angleDegrees;
	static Vector3	axis;
};

double	rotate_a::angleDegrees = 0.f;
Vector3	rotate_a::axis;

struct transformBegin_a
{
	void operator()(const iterator_t&, const iterator_t&) const
	{
		TransformStack::stack.push(TransformStack::currentTransform);
	}
};

struct transformEnd_a
{
	void operator()(const iterator_t&, const iterator_t&) const
	{
		TransformStack::currentTransform = TransformStack::stack.top();
		TransformStack::stack.pop();
	}
};

//-----------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------
// Graphics state

struct shaderParams_a
{
	void operator()(const Color &col) const
	{
		params.push_back(ShaderParam(paramName, PT_Color, col));
		paramName="";
	}
	
	void operator()(double d) const
	{
		params.push_back(ShaderParam(paramName, PT_Float, (float)d));
		paramName="";
	}

	static std::string	paramName;
	static ShaderParams	params;
};
std::string		shaderParams_a::paramName;
ShaderParams	shaderParams_a::params;

struct GraphicState
{
	GraphicState()
		:	_color(1,1,1), _opacity(1,1,1)
	{}

	Color	_color;
	Color	_opacity;
	
	ShaderParams	shaderParams;
	std::string		surfaceShader;
};

struct GraphicStateStack
{
	typedef std::stack<GraphicState, std::deque<GraphicState, memory::AllocAlign16<GraphicState>>> Stack;

	static Stack		stateStack;
	static GraphicState	current;
};

GraphicStateStack::Stack	GraphicStateStack::stateStack;
GraphicState				GraphicStateStack::current;

struct activateCurrentShaderParams_a
{
	void operator()(const iterator_t&, const iterator_t&) const
	{
		GraphicStateStack::current.shaderParams = shaderParams_a::params;
		shaderParams_a::params.swap(ShaderParams());
	}
};

struct attributeBegin_a
{
	void operator()(const iterator_t &s, const iterator_t &e) const
	{
		transformBegin_a()(s, e);

		GraphicStateStack::stateStack.push(GraphicStateStack::current);
	}
};

struct attributeEnd_a
{
	void operator()(const iterator_t &s, const iterator_t &e) const
	{
		transformBegin_a()(s, e);

		GraphicStateStack::current = GraphicStateStack::stateStack.top();
		GraphicStateStack::stateStack.pop();
	}
};

//-----------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------
// CameraSettings holder

struct CameraSettings
{
	// Format
	static int		resX, resY;
	static double	pix_aspectRatio;

	// Projection
	static std::string	projType;
	static double		fov;

	// Clipping
	static double	hither, yon;
};

int		CameraSettings::resX			= 640;
int		CameraSettings::resY			= 480;
double	CameraSettings::pix_aspectRatio	= 1.0;

std::string	CameraSettings::projType("orthographic");
double		CameraSettings::fov = 90.0;

// Getting eps/max from float & cast to double is required because:
// 1. boost.spirit real parser expects double
// 2. but the renderer works with floats
double	CameraSettings::hither	= static_cast<double>(std::numeric_limits<float>::epsilon());
double	CameraSettings::yon		= static_cast<double>(std::numeric_limits<float>::max());
//-----------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------
// Attributes actions

struct worldBegin_a
{
	worldBegin_a(Scene &scn) : scene(scn) {}
	
	Scene	&scene;

	void operator()(const iterator_t&, const iterator_t&) const
	{
		// Finalize camera
		const float aspect = static_cast<float>(CameraSettings::resX * CameraSettings::pix_aspectRatio / CameraSettings::resY);
		
		float screenExtents[4];
		if (aspect > 1.f)
		{
			screenExtents[0] = -aspect;
			screenExtents[1] =  aspect;
			screenExtents[2] = -1.f;
			screenExtents[3] =  1.f;
		}
		else
		{
			screenExtents[0] = -1.f;
			screenExtents[1] =  1.f;
			screenExtents[2] = -1.f / aspect;
			screenExtents[3] =  1.f / aspect;
		}

		Camera::CameraModel cm = Camera::CM_Orthographic;
		if (CameraSettings::projType == "\"perspective\"")
			cm = Camera::CM_Perspective;

		scene.camera().finalize(cm, TransformStack::currentTransform, aspect, deg2rad(CameraSettings::fov),
								CameraSettings::resX, CameraSettings::resY, (float)CameraSettings::hither, (float)CameraSettings::yon,
								screenExtents);

		// Reset current transform
		TransformStack::currentTransform = Matrix4::identity();
	}
};

//-----------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------
// Options actor

struct ShaderPath
{
	ShaderPath(Scene &scn) : scene(scn) {}

	void operator=(const std::vector<std::string> &folders)
	{
		using namespace std;

		vector<string> newFolders(folders);
		string defaultFolder("./shaders");
		replace(newFolders.begin(), newFolders.end(), string("&"), defaultFolder);
		for_each(newFolders.begin(), newFolders.end(), [&] (const string &f) { processFolder(f); } );
	}

	void processFolder(const std::string &folderName) const
	{
		namespace fs = boost::filesystem;
		//! \todo throw path not a folder/not found exception

		if (fs::is_directory(folderName))
		{
			for (fs::directory_iterator it(folderName); it != fs::directory_iterator(); ++it)
			{
				fs::path p = it->path();
				if (p.extension() == ".sl")
				{
					scene.shaderManager.loadFile(p.file_string());
				}
			}
		}
	}

	Scene &scene;
};
//-----------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------
// Displays actor

struct displayType_a
{
	displayType_a(Scene &scn) : scene(scn) {}
	
	void operator()(const iterator_t &first, const iterator_t &end) const
	{
		std::string type_str(first, end);
		type_str.erase(0, 1);
		type_str.erase(type_str.length() - 1, 1);

		if (type_str == "file")
		{
			scene.setDisplayType(DT_File);
		}
		else if (type_str == "framebuffer")
		{
			scene.setDisplayType(DT_Framebuffer);
		}
		else
		{
			std::cout << "Unknown display type: \"" << type_str << "\", defaulting to framebuffer." << std::endl;
			scene.setDisplayType(DT_Framebuffer);
		}
	}

	Scene			&	scene;
};
//-----------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------
// Material actors

//-----------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------
// Light actors
struct newPointLight_a
{
	newPointLight_a(Scene &scn) : scene(scn) {}
	
	void operator()(const iterator_t&, const iterator_t&) const
	{
		Light *l(memory::construct<Light>());
		l->pos		= pos;
		l->color	= color;
		scene.addLight(l);

		// Reset fields to allow for defaults
		pos = Point3(0);
		color = Color(0);
	}

	Scene			&	scene;
	static Point3		pos;
	static Color		color;
};
Point3	newPointLight_a::pos;
Color	newPointLight_a::color;
//-----------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------
// Geometries actors

#include "../scene/convex_polygon.h"
struct newPolygon_a
{
	newPolygon_a(Scene &scn) : scene(scn) {}

	void operator()(const iterator_t&, const iterator_t&) const
	{
		ConvexPolygon *poly = memory::construct<ConvexPolygon>(TransformStack::currentTransform);

		const size_t npoints = points.size();

		//! \todo Check that we have at least 3 points
		Point3 *pointArray = memory::allocate<Point3>(npoints);
		std::copy(points.begin(), points.end(), pointArray);
		poly->setPoints(npoints, pointArray);

		poly->setColor(GraphicStateStack::current._color);
		poly->setOpacity(GraphicStateStack::current._opacity);
		poly->setShader(scene.shaderManager.instanciate(GraphicStateStack::current.surfaceShader));
		poly->setShaderParams(GraphicStateStack::current.shaderParams);

		scene.addGeometry(poly);
	}

	Scene		&scene;
	
	static Vec3Array	points;
};
Vec3Array	newPolygon_a::points;

#include "../scene/sphere.h"
struct newSphere_a
{
	newSphere_a(Scene &scn) : scene(scn) {}
	
	//! \todo throw material or shader not found
	void operator()(const iterator_t&, const iterator_t&) const
	{
		Geometry *g = memory::construct<Sphere>(TransformStack::currentTransform, (float)radius);
		g->setColor(GraphicStateStack::current._color);
		g->setOpacity(GraphicStateStack::current._opacity);
		g->setShader(scene.shaderManager.instanciate(GraphicStateStack::current.surfaceShader));
		g->setShaderParams(GraphicStateStack::current.shaderParams);
		scene.addGeometry(g);

		// Reset fields to allow for defaults
		radius = 0;
	}

	Scene			&	scene;
	static double		radius;
};
double		newSphere_a::radius;

#include "../scene/disk.h"
struct newDisk_a
{
	newDisk_a(Scene &scn) : scene(scn) {}
	
	//! \todo throw material or shader not found
	void operator()(const iterator_t&, const iterator_t&) const
	{
		/*Geometry *g = memory::construct<Disk>(TransformStack::currentTransform, (float)radius, normalize(normal));
		g->setColor(currentColorOpa_a::color);
		g->setOpacity(currentColorOpa_a::opacity);
		g->setShader(scene.shaderManager.instanciate(matName));
		g->setShaderParams(shaderParams_a::params);
		scene.addGeometry(g);

		// Reset fields to allow for defaults
		radius = 0;
		pos = Point3(0);
		normal = Vector3(0);
		matName = "";

		shaderParams_a::params = ShaderParams();*/
	}

	Scene			&	scene;
	static double		radius;
	static Point3		pos;
	static Vector3		normal;
	static std::string	matName;
};
double		newDisk_a::radius;
Point3		newDisk_a::pos;
Vector3		newDisk_a::normal;
std::string	newDisk_a::matName;
//-----------------------------------------------------------------------------------------------------------

#endif
