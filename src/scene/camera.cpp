#include "camera.h"

#define PIOVER180 0.017453292519943f

void Camera::init(int width, int height)
{
	fov = 18.3;

	float fovx = fov * PIOVER180;
	float hFov = tan(fovx);
	float vFov = tan(((float)height / width) * 47 * PIOVER180);
	
	up.normalize();
	
	w = target - pos;
	w.normalize();

	u = up.cross(w);
	u *= hFov;

	v = w.cross(-u);
	v *= vFov;
}

void Camera::project(float x, float y, Ray &r) const
{
	r.origin = pos;

	Vec3 dir = x * u + y * v + w;
	dir.normalize();
	r.setDirection(dir);
}
