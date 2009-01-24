#ifndef CRT_PLANE_H
#define CRT_PLANE_H

#include "geometry.h"

class Plane : public Geometry
{
public:
	Plane(const Vec3d &normal = Vec3d(0, 1, 0), float offset = 0, bool _oneSided = true)
		: n(normal), d(offset), oneSided(_oneSided)
	{}

	virtual bool			hit(const Ray &ray, double &t) const;
	virtual void			normalAt(const Vec3d &p, Vec3d &n) const	{ n = this->n; }

protected:
	Vec3d		n;
	double		d;
	bool		oneSided;
};

#endif
