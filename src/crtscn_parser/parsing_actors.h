#ifndef CRT_PARSING_ACTORS_H
#define CRT_PARSING_ACTORS_H

// IMPORTANT: this file can't be used alone, it's only to be included in scnparser.h which
// contains the good includes and typedefs!

//-----------------------------------------------------------------------------------------------------------
// Shader path actor

#include <boost/filesystem.hpp>

struct shaderPath_a
{
	shaderPath_a(Scene &scn) : scene(scn) {}
	
	void operator()(const iterator_t &first, const iterator_t &end) const
	{
		std::string folder(first, end);

		namespace fs = boost::filesystem;
		//! \todo throw path not a folder/not found exception

		if (fs::is_directory(folder))
		{
			for (fs::directory_iterator it(folder); it != fs::directory_iterator(); ++it)
			{
				fs::path p = it->path();
				if (p.extension() == ".crtsl")
				{
					scene.shaderManager.loadFile(p.file_string());
				}
			}
		}
	}

	Scene			&	scene;
};
//-----------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------
// Material actors

struct newMaterial_a
{
	newMaterial_a(Scene &scn) : scene(scn) {}
	
	//! \todo throw material not found
	void operator()(const iterator_t&, const iterator_t&) const
	{
		MaterialPtr m(new Material);
		m->name			= name;
		m->color		= color;
		m->reflexivity	= (float)reflexivity;
		scene.addMaterial(m);

		// Reset fields to allow for defaults
		name = "";
		color = Color4(0, 0);
		reflexivity = 0;
	}

	Scene			&	scene;
	static std::string	name;
	static Color4		color;
	static double		reflexivity;
};
std::string	newMaterial_a::name;
Color4		newMaterial_a::color;
double		newMaterial_a::reflexivity;

#include "../shadervm/shader_param.h"
struct shaderParams_a
{
	void operator()(const Color4 &col) const
	{
		params.push_back(ShaderParam(PT_Color4, col));
	}

	static ShaderParams params;
};
ShaderParams shaderParams_a::params;
//-----------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------
// Light actors
struct newPointLight_a
{
	newPointLight_a(Scene &scn) : scene(scn) {}
	
	void operator()(const iterator_t&, const iterator_t&) const
	{
		LightPtr l(new Light);
		l->pos		= pos;
		l->color	= color;
		scene.addLight(l);

		// Reset fields to allow for defaults
		pos = Vec3(0);
		color = Color4(0, 0);
	}

	Scene			&	scene;
	static Vec3			pos;
	static Color4		color;
};
Vec3	newPointLight_a::pos;
Color4	newPointLight_a::color;
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
		GeometryPtr g(new Sphere((float)radius, pos));
		g->setShader(scene.shaderManager.instanciate(matName));
		g->setShaderParams(shaderParams_a::params);
		scene.addGeometry(g);

		// Reset fields to allow for defaults
		pos = Vec3(0);
		radius = 0;
		matName = "";

		shaderParams_a::params = ShaderParams();
	}

	Scene			&	scene;
	static double		radius;
	static Vec3			pos;
	static std::string	matName;
};
double		newSphere_a::radius;
Vec3		newSphere_a::pos;
std::string	newSphere_a::matName;



#include "Plane.h"
struct newPlane_a
{
	newPlane_a(Scene &scn) : scene(scn) {}
	
	//! \todo throw material or shader not found
	void operator()(const iterator_t&, const iterator_t&) const
	{
		GeometryPtr g(new Plane(normal.normalize(), (float)offset, !twoSided));
		g->setShader(scene.shaderManager.instanciate(matName));
		g->setShaderParams(shaderParams_a::params);
		scene.addGeometry(g);

		// Reset fields to allow for defaults
		normal = Vec3(0);
		offset = 0;
		twoSided = false;
		matName = "";

		shaderParams_a::params = ShaderParams();
	}

	Scene			&	scene;
	static Vec3			normal;
	static double		offset;
	static bool			twoSided;
	static std::string	matName;
};
Vec3		newPlane_a::normal;
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
		GeometryPtr g(new Disk((float)radius, pos, normal.normalize()));
		g->setShader(scene.shaderManager.instanciate(matName));
		g->setShaderParams(shaderParams_a::params);
		scene.addGeometry(g);

		// Reset fields to allow for defaults
		radius = 0;
		pos = Vec3(0);
		normal = Vec3(0);
		matName = "";

		shaderParams_a::params = ShaderParams();
	}

	Scene			&	scene;
	static double		radius;
	static Vec3			pos;
	static Vec3			normal;
	static std::string	matName;
};
double		newDisk_a::radius;
Vec3		newDisk_a::pos;
Vec3		newDisk_a::normal;
std::string	newDisk_a::matName;
//-----------------------------------------------------------------------------------------------------------

#endif
