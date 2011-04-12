#ifndef CRT_CAMERA_H
#define CRT_CAMERA_H

#include "ray.h"

class Camera
{
public:
	Camera()
		:target(Vector3::zAxis()), up(Vector3::yAxis()), fov(60)
	{
	}

	void		init(int width, int height);
	void		project(float x, float y, Ray &r) const;

public:
	Vector3	pos;
	Vector3	target;
	Vector3	up;
	float	fov;

private:
	// Define the camera frame
	Vector3 u, v, w;
};

#endif
