#ifndef PPRT_ACTORS_GEOMETRIES_H
#define PPRT_ACTORS_GEOMETRIES_H

#include "parser_typedefs.h"

#include <vector>

class Scene;

//-----------------------------------------------
// Data streams

struct PointStream
{
	static Vec3Array Ps;
};

//-----------------------------------------------
// Geometric objects

struct newPolygon_a
{
	newPolygon_a(Scene &scn);

	void operator()(const iterator_t&, const iterator_t&) const;

	Scene &scene;
};

struct newSphere_a
{
	newSphere_a(Scene &scn);
	
	//! \todo throw material or shader not found
	void operator()(const iterator_t&, const iterator_t&) const;

	Scene &scene;
	static double		radius;
};

struct newDisk_a
{
	newDisk_a(Scene &scn);
	
	//! \todo throw material or shader not found
	void operator()(const iterator_t&, const iterator_t&) const;

	Scene &scene;

	static double		radius;
	static Vector3		normal;
};

#endif
