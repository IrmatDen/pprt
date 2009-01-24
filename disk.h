#ifndef CRT_DISK_H
#define CRT_DISK_H

#include "plane.h"

class Disk : public Plane
{
public:
	Disk(float radius = 1, const Vec3d &position = Vec3d(), const Vec3d &normal = Vec3d(0, 1, 0));

	virtual bool			hit(const Ray &ray, double &t) const;
	virtual void			normalAt(const Vec3d &p, Vec3d &n) const	{ n = this->n; }

private:
	float r;
	float rSquared;
};

#endif
