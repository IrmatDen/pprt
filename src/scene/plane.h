#ifndef CRT_PLANE_H
#define CRT_PLANE_H

#include "geometry.h"

class Plane : public Geometry
{
public:
	Plane(const Vec3 &normal = Vec3(0, 1, 0), float offset = 0, bool _oneSided = true)
		: n(normal), d(offset), oneSided(_oneSided)
	{}

	virtual bool			hit(const Ray &ray, float &t) const;
	virtual void			fillIntersectionInfo(const Vec3 &p, IntersectionInfo &ii) const	{ ii.normal = this->n; }

protected:
	Vec3		n;
	float		d;
	bool		oneSided;
};

#endif
