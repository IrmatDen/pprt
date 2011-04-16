#ifndef CRT_PLANE_H
#define CRT_PLANE_H

#include "geometry.h"

class Plane : public Geometry
{
public:
	Plane(const Vector3 &normal = Vector3::yAxis(), float offset = 0, bool _oneSided = true)
		: n(normal), d(offset), oneSided(_oneSided)
	{buildAABB();}

	virtual bool			hit(const Ray &ray, float &t) const;
	virtual void			fillIntersectionInfo(const Vector3 &p, IntersectionInfo &ii) const	{ ii.normal = n; }

private:
	void	buildAABB()
	{
		aabb._min = pos - Vector3(5, d, 5);
		aabb._max = pos + Vector3(5, d, 5);
	}

protected:
	Vector3		n;
	float		d;
	bool		oneSided;
};

#endif
