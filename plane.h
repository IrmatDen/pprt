#ifndef CRT_PLANE_H
#define CRT_PLANE_H

#include "geometry.h"

class Plane : public Geometry
{
public:
	Plane(const Vec3f &normal = Vec3f(0, 1, 0), float offset = 0, bool _oneSided = true)
		: n(normal), d(offset), oneSided(_oneSided)
	{}

	virtual bool			hit(const Ray &ray, float &t) const;
	virtual void			normalAt(const Vec3f &p, Vec3f &n) const	{ n = this->n; }

protected:
	Vec3f		n;
	float		d;
	bool		oneSided;
};

#endif
