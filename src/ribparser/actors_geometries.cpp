#include "actors_geometries.h"
#include "actors_transform.h"
#include "actors_graphicstate.h"

#include "../scene/convex_polygon.h"
#include "../scene/sphere.h"
#include "../scene/disk.h"
#include "../scene/scene.h"

#include <iostream>

using namespace std;

//-----------------------------------------------
// Data streams

Vec3Array DataStream::Ps;
Vec3Array DataStream::Ns;
Vec3Array DataStream::Css;
Vec3Array DataStream::Oss;

void resetGeomStreams_a::operator()(const iterator_t&, const iterator_t&) const
{
    DataStream::Ps.swap(Vec3Array());
    DataStream::Ns.swap(Vec3Array());
    DataStream::Css.swap(Vec3Array());
    DataStream::Oss.swap(Vec3Array());
}

//-----------------------------------------------------
// Convex polygon

newPolygon_a::newPolygon_a(Scene &scn)
	: scene(scn)
{
}

void newPolygon_a::operator()(const iterator_t&, const iterator_t&) const
{
	const size_t npoints = DataStream::Ps.size();

    // Setup mesh flags
    ConvexPolygon::MeshCreationData::ComponentSet format;
    if (DataStream::Ns.size() > 0)
    {
        if (DataStream::Ns.size() != npoints)
            cout << "Normal & Points count mismatch in Polygon declaration. Using default normals" << endl;
        else
            format.set(ConvexPolygon::MeshCreationData::HasNormals);
    }
    if (DataStream::Css.size() > 0)
    {
        if (DataStream::Css.size() != npoints)
            cout << "Cs & Points count mismatch in Polygon declaration. Using default color." << endl;
        else
            format.set(ConvexPolygon::MeshCreationData::HasColors);
    }
    if (DataStream::Oss.size() > 0)
    {
        if (DataStream::Oss.size() != npoints)
            cout << "Os & Points count mismatch in Polygon declaration. Using default opacity." << endl;
        else
            format.set(ConvexPolygon::MeshCreationData::HasOpacities);
    }

    // Set mesh data
    ConvexPolygon::MeshCreationData data(npoints, 1, format);
    copy(DataStream::Ps.begin(), DataStream::Ps.end(), data.points);
    
    if (format.test(ConvexPolygon::MeshCreationData::HasNormals))
        copy(DataStream::Ns.begin(), DataStream::Ns.end(), data.normals);
    
    if (format.test(ConvexPolygon::MeshCreationData::HasColors))
        copy(DataStream::Css.begin(), DataStream::Css.end(), data.cs);

    if (format.test(ConvexPolygon::MeshCreationData::HasOpacities))
        copy(DataStream::Oss.begin(), DataStream::Oss.end(), data.os);

    size_t *pointsIdx = new size_t[npoints];
    size_t idx = 0;
    generate(pointsIdx, pointsIdx + npoints, [&] () -> size_t { return idx++; } );
    data.addFace(npoints, pointsIdx);

    // Build mesh
	Geometry *g = ConvexPolygon::create(TransformStack::currentTransform, data);
	GraphicStateStack::current.applyToGeometry(&scene, g);
	scene.addGeometry(g);
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
