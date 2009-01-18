#ifndef CRT_DISK_H
#define CRT_DISK_H

#include "plane.h"

class Disk : public Plane
{
public:
	Disk(float radius = 1, const Vec3f &position = Vec3f(), const Vec3f &normal = Vec3f(0, 1, 0));

	virtual bool			hit(const Ray &ray, float &t) const;
	virtual void			normalAt(const Vec3f &p, Vec3f &n) const	{ n = this->n; }

private:
	float r;
	float rSquared;
};

#endif
