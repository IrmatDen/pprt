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

	void setPoints(size_t pointsCount, Point3 *pointArray);
    //! Normals count is assumed to be equal to vertices' count specified in setPoints
	void setNormals(Vector3 *ns);
    //! Colors count is assumed to be equal to vertices' count specified in setPoints
	void setPointsColors(Color *cs);

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
