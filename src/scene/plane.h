#ifndef CRT_PLANE_H
#define CRT_PLANE_H

#include "geometry.h"

class Plane : public Geometry
{
public:
	Plane(const Vec3 &normal = Vec3(0, 1, 0), Real offset = 0, bool _oneSided = true)
		: n(normal), d(offset), oneSided(_oneSided)
	{}

	virtual bool			hit(const Ray &ray, Real &t) const;
	virtual void			normalAt(const Vec3 &p, Vec3 &n) const	{ n = this->n; }

protected:
	Vec3		n;
	Real		d;
	bool		oneSided;
};

#endif
