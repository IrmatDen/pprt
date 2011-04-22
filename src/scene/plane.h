#ifndef PPRT_PLANE_H
#define PPRT_PLANE_H

#include "geometry.h"

class Plane : public Geometry
{
public:
	Plane(const Vector3 &normal = Vector3::yAxis(), float offset = 0, bool _oneSided = true)
		: n(normal), d(offset), oneSided(_oneSided)
	{
		objectToWorld.setTranslation(d * n);
		worldToObject = inverse(objectToWorld);

		buildAABB();
	}

	virtual bool hit(const Ray &ray, float &t, IntersectionInfo &ii) const;

private:
	void	buildAABB()
	{
		aabb._min = Point3(-6, -2.51f, -8);//(objectToWorld * Vector3(6.f, 0.0001f, 8.f)).get128());
		aabb._max = Point3(6, -2.49f, 8);//(objectToWorld * Vector3(6.f, 0.0001f, 16.f)).get128());
	}

protected:
	Vector3		n;
	float		d;
	bool		oneSided;
};

#endif
