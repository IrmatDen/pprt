#ifndef CRT_DISK_H
#define CRT_DISK_H

#include "geometry.h"

class Disk : public Geometry
{
public:
	Disk(float radius = 1, const Vec3 &position = Vec3(), const Vec3 &normal = Vec3(0, 1, 0));

	virtual bool			hit(const Ray &ray, float &t) const;
	virtual void			fillIntersectionInfo(const Vec3 &p, IntersectionInfo &ii) const	{ ii.normal = this->n; }

private:
	Vec3	n;
	float	d;
	float	r;
	float	rSquared;
};

#endif
