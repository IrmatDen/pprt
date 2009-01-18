#ifndef CRT_CAMERA_H
#define CRT_CAMERA_H

#include "vector3.h"

class Camera
{
public:
	Vec3f	pos;
	Vec3f	dir;
	Vec3f	up, right;
	float	halfFrustumAngle;

	void		project(int x, int y, Vec3f &dir)
	{
	}
};

#endif
