#ifndef CRT_PLANE_H
#define CRT_PLANE_H

#include "geometry.h"

class Plane : public Geometry
{
public:
	Plane(const Vector3 &normal = Vector3::yAxis(), float offset = 0, bool _oneSided = true)
		: n(normal), d(offset), oneSided(_oneSided)
	{}

	virtual bool			hit(const Ray &ray, float &t) const;
	virtual void			fillIntersectionInfo(const Vector3 &p, IntersectionInfo &ii) const	{ ii.normal = n; }

protected:
	Vector3		n;
	float		d;
	bool		oneSided;
};

#endif
