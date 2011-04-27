#ifndef PPRT_DISK_H
#define PPRT_DISK_H

#include "geometry.h"

class Disk : public Geometry
{
public:
	Disk(Scene *scn, const Matrix4 &obj2world, float radius = 1, const Vector3 &normal = Vector3::yAxis());

	virtual bool hit(const Ray &ray, float &t, IntersectionInfo &ii) const;

private:
	Vector3	n;
	float	d;
	float	r;
	float	rSquared;
};

#endif
