#include "actors_geometries.h"
#include "actors_transform.h"
#include "actors_graphicstate.h"

#include "../scene/mesh.h"
#include "../scene/sphere.h"
#include "../scene/disk.h"
#include "../scene/scene.h"

#include <algorithm>
#include <iostream>
#include <numeric>

using namespace std;

// Helpers (mainly setting up vertex formats & copying streams
// Not included in DataStream class to try not to grow the
// already awfuls compile time (spirit is nice, but still, meh...)
namespace
{
    bool prePolygonCheck()
    {
        if (DataStream::Ps.size() < 3)
        {
            cout << "Can't create a polygon without at least 3 positions." << endl;
            return false;
        }

        return true;
    }

    Mesh::ComponentSet getVertexFormatForCurrentDataStream()
    {
	    const size_t npoints = DataStream::Ps.size();

        Mesh::ComponentSet format;
        if (DataStream::Ns.size() > 0)
        {
            if (DataStream::Ns.size() != npoints)
                cout << "Normal & Points count mismatch in Polygon declaration. Using geometry normals" << endl;
            else
                format.set(Mesh::HasNormals);
        }
        if (DataStream::Css.size() > 0)
        {
            if (DataStream::Css.size() != npoints)
                cout << "Cs & Points count mismatch in Polygon declaration. Using global color." << endl;
            else
                format.set(Mesh::HasColors);
        }
        if (DataStream::Oss.size() > 0)
        {
            if (DataStream::Oss.size() != npoints)
                cout << "Os & Points count mismatch in Polygon declaration. Using global opacity." << endl;
            else
                format.set(Mesh::HasOpacities);
        }

        return format;
    }

    void applyDataStream(Mesh::MeshCreationData &data)
    {
        copy(DataStream::Ps.begin(), DataStream::Ps.end(), data.points);
    
        if (data.vertexFormat.test(Mesh::HasNormals))
            copy(DataStream::Ns.begin(), DataStream::Ns.end(), data.normals);
    
        if (data.vertexFormat.test(Mesh::HasColors))
            copy(DataStream::Css.begin(), DataStream::Css.end(), data.cs);

        if (data.vertexFormat.test(Mesh::HasOpacities))
            copy(DataStream::Oss.begin(), DataStream::Oss.end(), data.os);
    }
}

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
    // Pre-check
	const size_t npoints = DataStream::Ps.size();
    if (!prePolygonCheck())
        return;

    // Set mesh data
    const Mesh::ComponentSet format = getVertexFormatForCurrentDataStream();
    Mesh::MeshCreationData data(npoints, 1, format);
    applyDataStream(data);

    // Build our single face
    size_t *pointsIdx = new size_t[npoints];
    size_t idx = 0;
    generate(pointsIdx, pointsIdx + npoints, [&] () -> size_t { return idx++; } );
    data.addFace(npoints, pointsIdx);
    delete [] pointsIdx;

    // Build mesh
	Geometry *g = Mesh::create(TransformStack::currentTransform, data);
	GraphicStateStack::current.applyToGeometry(&scene, g);
	scene.addGeometry(g);
}

//-----------------------------------------------------
// Convex polygon

IntArray newPointsPolygons_a::vertexPerFaces;
IntArray newPointsPolygons_a::faceDescriptions;

newPointsPolygons_a::newPointsPolygons_a(Scene &scn)
	: scene(scn)
{
}

void newPointsPolygons_a::operator()(const iterator_t&, const iterator_t&) const
{
    // Pre-check
    if (!prePolygonCheck())
        return;

	const size_t nfaces = vertexPerFaces.size();
	const size_t npoints = DataStream::Ps.size();

    if (nfaces < 1)
    {
        cout << "Can't create a PointsPolygons without at least 1 face." << endl;
        return;
    }

    const size_t expectedNumVertexIds = accumulate(vertexPerFaces.begin(), vertexPerFaces.end(), 0);
    if (expectedNumVertexIds != faceDescriptions.size())
    {
        cout << "Not enought vertex ids. Expected " << expectedNumVertexIds << ", got: " << faceDescriptions.size() << endl;
        return;
    }

    
    // Set mesh data
    const Mesh::ComponentSet format = getVertexFormatForCurrentDataStream();
    Mesh::MeshCreationData data(npoints, nfaces, format);
    applyDataStream(data);

    // Generate faces
    const size_t maxFaceDescSize = *max_element(vertexPerFaces.begin(), vertexPerFaces.end());
    size_t *pointsIdx = new size_t[maxFaceDescSize];
    IntArray::const_iterator currFaceDescIt = faceDescriptions.begin();
    for (IntArray::const_iterator vertexInFaceit = vertexPerFaces.begin();
        vertexInFaceit != vertexPerFaces.end();
        ++vertexInFaceit)
    {
        generate(pointsIdx, pointsIdx + *vertexInFaceit,
            [&] () -> size_t { int n = *currFaceDescIt; ++currFaceDescIt; return n; } );
        data.addFace(*vertexInFaceit, pointsIdx);
    }
    delete [] pointsIdx;

    // Build mesh
	Geometry *g = Mesh::create(TransformStack::currentTransform, data);
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
