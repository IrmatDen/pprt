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

	void		init(int width, int height);
	void		project(float x, float y, Ray &r) const;

public:
	Vec3	pos;
	Vec3	target;
	Vec3	up;
	float	fov;

private:
	// Define the camera frame
	Vec3 u, v, w;
};

#endif
