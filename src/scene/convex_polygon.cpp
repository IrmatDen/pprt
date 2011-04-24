#include "convex_polygon.h"

#include <algorithm>

using namespace std;

struct ConvexPolygon::Vertex
{
	Point3	pos;
    Vector3 n;
    Color   cs;

	Vector3	edgeNormal;	//!< Edge normal is defined to be the normal of the edge between this point & the next
};

ConvexPolygon::ConvexPolygon()
	: barCoordProvider(&memory::PoolCreator<float, 1>::create)
{
}

ConvexPolygon::ConvexPolygon(const Matrix4 &obj2world)
	: Geometry(obj2world), barCoordProvider(&memory::PoolCreator<float, 1>::create)
{
}

ConvexPolygon::~ConvexPolygon()
{
	memory::deallocate(vertices);
}

void ConvexPolygon::setPoints(size_t pointsCount, Point3 *pointArray)
{
	nVertices	= pointsCount;
	vertices	= memory::allocate<Vertex>(nVertices);

	// Get supporting plane
	//! \todo Search valid points instead of assuming the first 3 are...
	polyPlane = Plane::fromPoints(pointArray[0], pointArray[1], pointArray[2]);

	// Copy vertices' positions
	size_t vIdx = 0;
	for_each(pointArray, pointArray + nVertices,
        [&] (const Point3 &p)
        {
            vertices[vIdx].pos  = p;
            vertices[vIdx].n    = polyPlane.n;
            vertices[vIdx].cs   = color;
            vIdx++;
        } );

	buildAABB();
	
	// Compute edges' normal
	for (vIdx = 0; vIdx != nVertices; vIdx++)
	{
		const size_t next			= (vIdx + 1) % nVertices;
		const Vector3 nextToCurrent	= vertices[next].pos - vertices[vIdx].pos;
		vertices[vIdx].edgeNormal	= cross(normalize(nextToCurrent), polyPlane.n);
	}
}

void ConvexPolygon::setNormals(Vector3 *ns)
{
    for (size_t vIdx = 0; vIdx != nVertices; vIdx++)
        vertices[vIdx].n = normalize(ns[vIdx]);
}

void ConvexPolygon::setPointsColors(Color *cs)
{
    for (size_t vIdx = 0; vIdx != nVertices; vIdx++)
        vertices[vIdx].cs = cs[vIdx];
}

void ConvexPolygon::buildAABB()
{
	for_each(vertices, vertices + nVertices,
		[&] (const Vertex &v)
		{
			aabb._min = minPerElem(aabb._min, v.pos);
			aabb._max = maxPerElem(aabb._max, v.pos);
		} );
}

bool ConvexPolygon::hit(const Ray &ray, IntersectionInfo &ii) const
{
	Ray localRay(worldToObject * ray);

	float dist;
	Point3 pointInPlane;
	if (!polyPlane.intersection(localRay, dist, pointInPlane) || dist > ray.maxT)
		return false;

	// Test point-in-polygon (lazy mode)
	size_t insideCount = 0;
	for (size_t pIdx = 0; pIdx != nVertices; pIdx++)
	{
		const Vector3 PToCurrent = pointInPlane - vertices[pIdx].pos;
		if (dot(vertices[pIdx].edgeNormal, PToCurrent) < 0.f)
			insideCount++;
	}
	if (insideCount < nVertices)
		return false;

	// Compute barycentric coordinates based on
	// "Generalized Barycentric Coordinates on Irregular Polygons"
	// by Meyer et al. (2002)
	// Published in JGT Vol. 7, Nr 1, 2002
	float *weights	= reinterpret_cast<float*>(barCoordProvider.local()->ordered_malloc(nVertices));
	float weightSum	= 0.f;

	bool shouldNormalizeWeights = true;
	for (size_t vIdx = 0; vIdx != nVertices; vIdx++)
	{
		const size_t prev	= (vIdx + nVertices - 1) % nVertices;
		const size_t next	= (vIdx + 1) % nVertices;

		// Determine if point is almost on an edge
		const Vector3 PToCurrent = pointInPlane - vertices[vIdx].pos;
		const Vector3 nextToCurrent	= vertices[next].pos - vertices[vIdx].pos;
		const float area		= lengthSqr(cross(nextToCurrent, PToCurrent));
		const float ntcSqrdLen	= lengthSqr(nextToCurrent);
		if (area <= 0.000001f * ntcSqrdLen)
		{
			// Reset all weights and interpolate between this vertex and the next
			for (size_t wIdx = 0; wIdx != nVertices; wIdx++)
				weights[wIdx] = 0.f;

			const float w = lengthSqr(PToCurrent) / ntcSqrdLen;
			weights[vIdx] = 1.f - w;
			weights[next] = w;

			shouldNormalizeWeights = false;
			break;
		}

		const float cot1		= cotangeant(pointInPlane, vertices[vIdx].pos, vertices[prev].pos);
		const float cot2		= cotangeant(pointInPlane, vertices[vIdx].pos, vertices[next].pos);
		const float distSqrd	= lengthSqr(pointInPlane - vertices[vIdx].pos);
		weights[vIdx]			= (cot1 + cot2) / distSqrd;
		weightSum				+= weights[vIdx];
	}
	
	// Normalize weights
	if (shouldNormalizeWeights)
	{
		const float invWeightSum = 1.f / weightSum;
		for (size_t wIdx = 0; wIdx != nVertices; wIdx++)
			weights[wIdx] *= invWeightSum;
	}

    ii.normal   = Vector3(0.f);
    ii.cs       = Color(0.f);
	for (size_t vIdx = 0; vIdx != nVertices; vIdx++)
    {
		 ii.normal  += vertices[vIdx].n * weights[vIdx];
         ii.cs      += vertices[vIdx].cs * weights[vIdx];
    }

	barCoordProvider.local()->ordered_free(weights, nVertices);

	// Fill intersection info
	ray.maxT	= dist;
	ii.point	= Point3((objectToWorld * pointInPlane).get128());

	return true;
}
