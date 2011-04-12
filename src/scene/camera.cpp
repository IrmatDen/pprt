#include "camera.h"

#define PIOVER180 0.017453292519943f

void Camera::init(int width, int height)
{
	fov = 18.3f;

	float fovx = fov * PIOVER180;
	float hFov = tan(fovx);
	float vFov = tan(((float)height / width) * 47 * PIOVER180);
	
	up = normalize(up);
	
	w = target - pos;
	w = normalize(w);

	u = cross(up, w);
	u *= hFov;

	v = cross(w, -u);
	v *= vFov;
}

void Camera::project(float x, float y, Ray &r) const
{
	r.origin = pos;

	Vector3 dir = x * u + y * v + w;
	dir = normalize(dir);
	r.setDirection(dir);
}
