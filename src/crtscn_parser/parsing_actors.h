#ifndef PPRT_PARSING_ACTORS_H
#define PPRT_PARSING_ACTORS_H

// IMPORTANT: this file can't be used alone, it's only to be included in scnparser.h which
// contains the good includes and typedefs!

#include <boost/filesystem.hpp>
#include <iostream>

#include "../common.h"
#include "../shadervm/shader_param.h"

template <typename T>
inline float deg2rad(T deg) { return static_cast<float>(deg * 0.01745329252); }

//-----------------------------------------------------------------------------------------------------------
// Transform actors

struct TransformStack
{
	static std::stack<Matrix4>	stack;
	static Matrix4				currentTransform;
};

std::stack<Matrix4>	TransformStack::stack;
Matrix4				TransformStack::currentTransform(Matrix4::identity());

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
		TransformStack::currentTransform = Matrix4::translation(vec) * TransformStack::currentTransform;
	}
};

struct rotate_a
{
	void operator()(const iterator_t&, const iterator_t&) const
	{
		const Matrix4 rot = Matrix4::rotation(deg2rad(angleDegrees), normalize(axis));
		TransformStack::currentTransform = rot * TransformStack::currentTransform;
	}

	static double	angleDegrees;
	static Vector3	axis;
};

double	rotate_a::angleDegrees = 0.f;
Vector3	rotate_a::axis;

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
				if (p.extension() == ".crtsl")
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
	static ShaderParams params;
};
std::string		shaderParams_a::paramName;
ShaderParams	shaderParams_a::params;

struct currentColorOpa_a
{
	static Color		color;
	static Color		opacity;
};
Color currentColorOpa_a::color(1,1,1);
Color currentColorOpa_a::opacity(1,1,1);

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

#include "Sphere.h"
struct newSphere_a
{
	newSphere_a(Scene &scn) : scene(scn) {}
	
	//! \todo throw material or shader not found
	void operator()(const iterator_t&, const iterator_t&) const
	{
		Point3 p = pos;
		Geometry *g(memory::construct<Sphere>((float)radius, p));
		g->setColor(currentColorOpa_a::color);
		g->setOpacity(currentColorOpa_a::opacity);
		g->setShader(scene.shaderManager.instanciate(matName));
		g->setShaderParams(shaderParams_a::params);
		scene.addGeometry(g);

		// Reset fields to allow for defaults
		pos = Point3(0);
		radius = 0;
		matName = "";

		shaderParams_a::params = ShaderParams();
	}

	Scene			&	scene;
	static double		radius;
	static Point3		pos;
	static std::string	matName;
};
double		newSphere_a::radius;
Point3		newSphere_a::pos;
std::string	newSphere_a::matName;



#include "Plane.h"
struct newPlane_a
{
	newPlane_a(Scene &scn) : scene(scn) {}
	
	//! \todo throw material or shader not found
	void operator()(const iterator_t&, const iterator_t&) const
	{
		Geometry *g(memory::construct<Plane>(normalize(normal), (float)offset, !twoSided));
		g->setColor(currentColorOpa_a::color);
		g->setOpacity(currentColorOpa_a::opacity);
		g->setShader(scene.shaderManager.instanciate(matName));
		g->setShaderParams(shaderParams_a::params);
		scene.addGeometry(g);

		// Reset fields to allow for defaults
		normal = Vector3(0);
		offset = 0;
		twoSided = false;
		matName = "";

		shaderParams_a::params = ShaderParams();
	}

	Scene			&	scene;
	static Vector3		normal;
	static double		offset;
	static bool			twoSided;
	static std::string	matName;
};
Vector3		newPlane_a::normal;
double		newPlane_a::offset;
bool		newPlane_a::twoSided;
std::string	newPlane_a::matName;



#include "Disk.h"
struct newDisk_a
{
	newDisk_a(Scene &scn) : scene(scn) {}
	
	//! \todo throw material or shader not found
	void operator()(const iterator_t&, const iterator_t&) const
	{
		Geometry *g(memory::construct<Disk>((float)radius, pos, normalize(normal)));
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

		shaderParams_a::params = ShaderParams();
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
