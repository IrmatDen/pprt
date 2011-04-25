#ifndef PPRT_ACTORS_GEOMETRIES_H
#define PPRT_ACTORS_GEOMETRIES_H

#include "parser_typedefs.h"

#include <vector>

class Scene;

//-----------------------------------------------
// Data streams

struct DataStream
{
    static Vec3Array Ps;
    static Vec3Array Ns;
    static Vec3Array Css, Oss;
};

struct resetGeomStreams_a
{
	void operator()(const iterator_t&, const iterator_t&) const;
};

//-----------------------------------------------
// Geometric objects

struct newPolygon_a
{
	newPolygon_a(Scene &scn);

	void operator()(const iterator_t&, const iterator_t&) const;

	Scene &scene;
};

struct newPointsPolygons_a
{
	newPointsPolygons_a(Scene &scn);

	void operator()(const iterator_t&, const iterator_t&) const;

	Scene &scene;

    static IntArray vertexPerFaces;
    static IntArray faceDescriptions;
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
