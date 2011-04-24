#include "convex_polygon.h"

#include <algorithm>

using namespace std;

struct ConvexPolygon::Vertex
{
	Point3	pos;
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

void ConvexPolygon::setPoints(size_t pointsCount, Point3 *pointArray)
{
	nVertices	= pointsCount;
	vertices	= memory::allocate<Vertex>(nVertices);

	// Copy vertices' positions
	size_t vIdx = 0;
	for_each(pointArray, pointArray + nVertices,
		[&] (const Point3 &p) { vertices[vIdx++].pos = p; } );

	buildAABB();

	// Get supporting plane
	//! \todo Search valid points instead of assuming the first 3 are...
	polyPlane = Plane::fromPoints(pointArray[0], pointArray[1], pointArray[2]);
	
	// Compute edges' normal
	for (vIdx = 0; vIdx != nVertices; vIdx++)
	{
		const size_t next			= (vIdx + 1) % nVertices;
		const Vector3 nextToCurrent	= vertices[next].pos - vertices[vIdx].pos;
		vertices[vIdx].edgeNormal	= cross(normalize(nextToCurrent), polyPlane.n);
	}

	memory::deallocate(pointArray);
}

ConvexPolygon::~ConvexPolygon()
{
	memory::deallocate(vertices);
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
	for (size_t pIdx = 0; pIdx != nVertices; pIdx++)
	{
		const size_t prev	= (pIdx + nVertices - 1) % nVertices;
		const size_t next	= (pIdx + 1) % nVertices;

		// Determine if point is almost on an edge
		const Vector3 PToCurrent = pointInPlane - vertices[pIdx].pos;
		const Vector3 nextToCurrent	= vertices[next].pos - vertices[pIdx].pos;
		const float area		= lengthSqr(cross(nextToCurrent, PToCurrent));
		const float ntcSqrdLen	= lengthSqr(nextToCurrent);
		if (area <= 0.001f * ntcSqrdLen)
		{
			// Reset all weights and interpolate between this vertex and the next
			for (size_t wIdx = 0; wIdx != nVertices; wIdx++)
				weights[wIdx] = 0.f;

			const float w = lengthSqr(PToCurrent) / ntcSqrdLen;
			weights[pIdx] = 1.f - w;
			weights[next] = w;

			shouldNormalizeWeights = false;
			break;
		}

		const float cot1		= cotangeant(pointInPlane, vertices[pIdx].pos, vertices[prev].pos);
		const float cot2		= cotangeant(pointInPlane, vertices[pIdx].pos, vertices[next].pos);
		const float distSqrd	= lengthSqr(pointInPlane - vertices[pIdx].pos);
		weights[pIdx]			= (cot1 + cot2) / distSqrd;
		weightSum				+= weights[pIdx];
	}
	
	// Normalize weights
	if (shouldNormalizeWeights)
	{
		for (size_t wIdx = 0; wIdx != nVertices; wIdx++)
			weights[wIdx] /= weightSum;
	}

	barCoordProvider.local()->ordered_free(weights, nVertices);

	// Fill intersection info
	ray.maxT	= dist;
	ii.point	= Point3((objectToWorld * pointInPlane).get128());
	ii.normal	= polyPlane.n;

	return true;
}
