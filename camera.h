#ifndef CRT_CAMERA_H
#define CRT_CAMERA_H

#include "ray.h"

class Camera
{
public:
	Camera()
		:target(0, 0, 1), up(0, 1, 0), fov(60)
	{
	}

	void		init();
	void		project(float x, float y, Ray &r) const;

public:
	Vec3d	pos;
	Vec3d	target;
	Vec3d	up;
	float	fov;

private:
	// Define the camera frame
	Vec3d u, v, w;
};

#endif
