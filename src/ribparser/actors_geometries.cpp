#include "actors_geometries.h"
#include "actors_transform.h"
#include "actors_graphicstate.h"

#include "../scene/convex_polygon.h"
#include "../scene/sphere.h"
#include "../scene/disk.h"
#include "../scene/scene.h"

//-----------------------------------------------
// Data streams

Vec3Array PointStream::Ps;

//-----------------------------------------------------
// Convex polygon

newPolygon_a::newPolygon_a(Scene &scn)
	: scene(scn)
{
}

void newPolygon_a::operator()(const iterator_t&, const iterator_t&) const
{
	ConvexPolygon *poly = memory::construct<ConvexPolygon>(TransformStack::currentTransform);

	const size_t npoints = PointStream::Ps.size();

	//! \todo Check that we have at least 3 points
	Point3 *pointArray = memory::allocate<Point3>(npoints);
	std::copy(PointStream::Ps.begin(), PointStream::Ps.end(), pointArray);
	poly->setPoints(npoints, pointArray);

	GraphicStateStack::current.applyToGeometry(&scene, poly);

	scene.addGeometry(poly);
}

//-----------------------------------------------------
// Sphere

double newSphere_a::radius;

newSphere_a::newSphere_a(Scene &scn)
	: scene(scn)
{
}

//! \todo throw material or shader not found
void newSphere_a::operator()(const iterator_t&, const iterator_t&) const
{
	Geometry *g = memory::construct<Sphere>(TransformStack::currentTransform, (float)radius);

	GraphicStateStack::current.applyToGeometry(&scene, g);

	scene.addGeometry(g);

	// Reset fields to allow for defaults
	radius = 0;
}


//-----------------------------------------------------
// Disk

double		newDisk_a::radius;
Vector3		newDisk_a::normal;

newDisk_a::newDisk_a(Scene &scn)
	: scene(scn)
{
}

//! \todo throw material or shader not found
void newDisk_a::operator()(const iterator_t&, const iterator_t&) const
{
	/*Geometry *g = memory::construct<Disk>(TransformStack::currentTransform, (float)radius, normalize(normal));

	GraphicStateStack::current.applyToGeometry(&scene, g);

	scene.addGeometry(g);

	// Reset fields to allow for defaults
	radius = 0;
	normal = Vector3(0);*/
}
