#include "camera.h"

#define PIOVER180 0.017453292519943295769236907684886

void Camera::init()
{
	float angleOfView = tan(float(PIOVER180) * 0.5f * fov);
	
	up.normalize();
	
	w = target - pos;
	w.normalize();

	u = up.cross(w);
	u.normalize();
	u *= angleOfView;

	v = w.cross(u);
	v.normalize();
	v *= angleOfView;
}

void Camera::project(float x, float y, Ray &r) const
{
	r.origin = pos;

	r.dir = (double)x * u + (double)y * v + w;
	r.dir.normalize();
}
