#ifndef CRT_PLANE_H
#define CRT_PLANE_H

#include "geometry.h"

class Plane : public Geometry
{
public:
	Plane(const Vector3 &normal = Vector3::yAxis(), float offset = 0, bool _oneSided = true)
		: n(normal), d(offset), oneSided(_oneSided)
	{
		pos = Vector3(0, 0, 0) + d * n;
		buildAABB();
	}

	virtual bool			hit(const Ray &ray, float &t) const;
	virtual void			fillIntersectionInfo(const Vector3 &p, IntersectionInfo &ii) const	{ ii.normal = n; }

private:
	void	buildAABB()
	{
		aabb._min = pos - Vector3(6.f, 0.0001f, 8.f);
		aabb._max = pos + Vector3(6.f, 0.0001f, 16.f);
	}

protected:
	Vector3		n;
	float		d;
	bool		oneSided;
};

#endif
