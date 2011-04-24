#ifndef PPRT_POLYGON_H
#define PPRT_POLYGON_H

#include "geometry.h"
#include "../memory.h"
#include "../math/plane.h"

class _MM_ALIGN16 ConvexPolygon : public Geometry
{
public:
	ConvexPolygon();
	ConvexPolygon(const Matrix4 &obj2world);
	~ConvexPolygon();

	// Point array ownership is transferred to the polygon.
	void setPoints(size_t pointsCount, Point3 *pointArray);

	virtual bool hit(const Ray &ray, IntersectionInfo &ii) const;

private:
	struct Vertex;

private:
	void buildAABB();

private:
	Plane		polyPlane;

	size_t		nVertices;
	Vertex	*	vertices;

	// TLS for barycentric coordinates
	mutable memory::TLPool	barCoordProvider;
};

#endif
