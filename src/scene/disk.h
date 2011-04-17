#ifndef PPRT_DISK_H
#define PPRT_DISK_H

#include "geometry.h"

class Disk : public Geometry
{
public:
	Disk(float radius = 1, const Vector3 &position = Vector3(), const Vector3 &normal = Vector3::yAxis());

	virtual bool			hit(const Ray &ray, float &t) const;
	virtual void			fillIntersectionInfo(const Vector3 &p, IntersectionInfo &ii) const	{ ii.normal = this->n; }

private:
	Vector3	n;
	float	d;
	float	r;
	float	rSquared;
};

#endif
