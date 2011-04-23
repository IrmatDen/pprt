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

	ray.maxT	= dist;
	ii.point	= Point3((objectToWorld * pointInPlane).get128());
	ii.normal	= polyPlane.n;

	return true;
}
