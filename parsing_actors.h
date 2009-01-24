#ifndef CRT_PARSING_ACTORS_H
#define CRT_PARSING_ACTORS_H

// IMPORTANT: this file can't be used alone, it's only to be included in scnparser.h which
// contains the good includes and typedefs!

//-----------------------------------------------------------------------------------------------------------
// Material actor

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
		pos = Vec3f(0);
		color = Color4(0, 0);
	}

	Scene			&	scene;
	static Vec3f		pos;
	static Color4		color;
};
Vec3f	newPointLight_a::pos;
Color4	newPointLight_a::color;
//-----------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------
// Geometries actors

#include "Sphere.h"
struct newSphere_a
{
	newSphere_a(Scene &scn) : scene(scn) {}
	
	//! \todo throw material not found
	void operator()(const iterator_t&, const iterator_t&) const
	{
		MaterialPtr m = scene.getMaterialByName(matName);
		if (m)
		{
			GeometryPtr g(new Sphere((float)radius, pos));
			g->material() = m;
			scene.addGeometry(g);
		}

		// Reset fields to allow for defaults
		pos = Vec3f(0);
		radius = 0;
		matName = "";
	}

	Scene			&	scene;
	static double		radius;
	static Vec3f		pos;
	static std::string	matName;
};
double		newSphere_a::radius;
Vec3f		newSphere_a::pos;
std::string	newSphere_a::matName;



#include "Plane.h"
struct newPlane_a
{
	newPlane_a(Scene &scn) : scene(scn) {}
	
	//! \todo throw material not found
	void operator()(const iterator_t&, const iterator_t&) const
	{
		MaterialPtr m = scene.getMaterialByName(matName);
		if (m)
		{
			GeometryPtr g(new Plane(normal.normalize(), (float)offset, !twoSided));
			g->material() = m;
			scene.addGeometry(g);
		}

		// Reset fields to allow for defaults
		normal = Vec3f(0);
		offset = 0;
		twoSided = false;
		matName = "";
	}

	Scene			&	scene;
	static Vec3f		normal;
	static double		offset;
	static bool			twoSided;
	static std::string	matName;
};
Vec3f		newPlane_a::normal;
double		newPlane_a::offset;
bool		newPlane_a::twoSided;
std::string	newPlane_a::matName;



#include "Disk.h"
struct newDisk_a
{
	newDisk_a(Scene &scn) : scene(scn) {}
	
	//! \todo throw material not found
	void operator()(const iterator_t&, const iterator_t&) const
	{
		MaterialPtr m = scene.getMaterialByName(matName);
		if (m)
		{
			GeometryPtr g(new Disk((float)radius, pos, normal.normalize()));
			g->material() = m;
			scene.addGeometry(g);
		}

		// Reset fields to allow for defaults
		radius = 0;
		pos = Vec3f(0);
		normal = Vec3f(0);
		matName = "";
	}

	Scene			&	scene;
	static double		radius;
	static Vec3f		pos;
	static Vec3f		normal;
	static std::string	matName;
};
double		newDisk_a::radius;
Vec3f		newDisk_a::pos;
Vec3f		newDisk_a::normal;
std::string	newDisk_a::matName;
//-----------------------------------------------------------------------------------------------------------

#endif
