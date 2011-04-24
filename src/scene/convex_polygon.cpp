#include "convex_polygon.h"

#include <algorithm>

using namespace std;

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
	nPoints	= pointsCount;
	points	= pointArray;

	buildAABB();

	//! \todo Search valid points instead of assuming the first 3 are...
	polyPlane = Plane::fromPoints(points[0], points[1], points[2]);
}

ConvexPolygon::~ConvexPolygon()
{
	memory::deallocate(points);
}

void ConvexPolygon::buildAABB()
{
	for_each(points, points + nPoints,
		[&] (const Point3 &p)
		{
			aabb._min = minPerElem(aabb._min, p);
			aabb._max = maxPerElem(aabb._max, p);
		} );
}

// Implementation of this follows the paper:
// "Generalized Barycentric Coordinates on Irregular Polygons"
// by Meyer et al. (2002)
// Published in JGT Vol. 7, Nr 1, 2002
bool ConvexPolygon::hit(const Ray &ray, IntersectionInfo &ii) const
{
	Ray localRay(worldToObject * ray);

	float dist, bbMin;
	Point3 pointInPlane;
	if (!polyPlane.intersection(localRay, dist, pointInPlane) || dist > ray.maxT || !aabb.hit(ray, bbMin, dist))
		return false;

	// Compute barycentric coordinates
	float *weights	= reinterpret_cast<float*>(barCoordProvider.local()->ordered_malloc(nPoints));
	float weightSum	= 0.f;
	size_t insideCount = 0;

	for (size_t pIdx = 0; pIdx != nPoints; pIdx++)
	{
		const size_t prev		= (pIdx + nPoints - 1) % nPoints;
		const size_t next		= (pIdx + 1) % nPoints;

		// Determine if point is almost on an edge
		const Vector3 nextToCurrent	= points[next] - points[pIdx];
		const Vector3 PToCurrent	= pointInPlane - points[pIdx];
		const float area		= lengthSqr(cross(nextToCurrent, PToCurrent));

		Vector3 edgeN(normalize(Vector3(shuffle1ps(nextToCurrent.get128(), 2, 1, 0, 3))));
		edgeN.setX(-edgeN.getX());
		if (dot(edgeN, pointInPlane - points[pIdx]) < 0.f)
			insideCount++;

		const float cot1		= cotangeant(pointInPlane, points[pIdx], points[prev]);
		const float cot2		= cotangeant(pointInPlane, points[pIdx], points[next]);
		const float distSqrd	= lengthSqr(pointInPlane - points[pIdx]);
		weights[pIdx]			= (cot1 + cot2) / distSqrd;
		weightSum				+= weights[pIdx];
	}
	
	// Normalize weights
	for (size_t wIdx = 0; wIdx != nPoints; wIdx++)
		weights[wIdx] /= weightSum;

	barCoordProvider.local()->ordered_free(weights, nPoints);

	if (insideCount < nPoints)
		return false;

	// Fill intersection info
	ray.maxT	= dist;
	ii.point	= Point3((objectToWorld * pointInPlane).get128());
	ii.normal	= polyPlane.n;

	return true;
}
