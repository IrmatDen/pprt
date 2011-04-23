#ifndef PPRT_PLANE_H
#define PPRT_PLANE_H

#include "../pprt_math.h"
#include "../scene/ray.h"

class Plane
{
public:
	//! Points are assumed to be non coincident and not all collinear
	static Plane fromPoints(const Point3 &p0, const Point3 &p1, const Point3 &p2);

public:
	Plane();

	bool intersection(const Ray &ray, float &dist, Point3 &intersection) const;

public:
	Vector3	n;
	float	d;
};

#endif
