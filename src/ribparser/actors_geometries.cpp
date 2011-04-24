#include "actors_geometries.h"
#include "actors_transform.h"
#include "actors_graphicstate.h"

#include "../scene/convex_polygon.h"
#include "../scene/sphere.h"
#include "../scene/disk.h"
#include "../scene/scene.h"

//-----------------------------------------------------
// Convex polygon

Vec3Array newPolygon_a::points;

newPolygon_a::newPolygon_a(Scene &scn)
	: scene(scn)
{
}

void newPolygon_a::operator()(const iterator_t&, const iterator_t&) const
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

	points.swap(Vec3Array());
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
	g->setColor(GraphicStateStack::current._color);
	g->setOpacity(GraphicStateStack::current._opacity);
	g->setShader(scene.shaderManager.instanciate(GraphicStateStack::current.surfaceShader));
	g->setShaderParams(GraphicStateStack::current.shaderParams);
	scene.addGeometry(g);

	// Reset fields to allow for defaults
	radius = 0;
}


//-----------------------------------------------------
// Disk

double		newDisk_a::radius;
Point3		newDisk_a::pos;
Vector3		newDisk_a::normal;
std::string	newDisk_a::matName;

newDisk_a::newDisk_a(Scene &scn)
	: scene(scn)
{
}

//! \todo throw material or shader not found
void newDisk_a::operator()(const iterator_t&, const iterator_t&) const
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
