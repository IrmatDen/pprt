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
		if (!m)
			return;

		GeometryPtr g(new Sphere((float)radius, pos));
		g->material() = m;
		scene.addGeometry(g);
	}

	Scene			&	scene;
	static double		radius;
	static Vec3f		pos;
	static std::string	matName;
};
double		newSphere_a::radius;
Vec3f		newSphere_a::pos;
std::string	newSphere_a::matName;
//-----------------------------------------------------------------------------------------------------------

#endif
