#include "camera.h"

#define PIOVER180 0.017453292519943295769236907684886

void Camera::init(int width, int height)
{
	// Compute real fov
	fov = 18.3;

	double fovx = fov * PIOVER180;
	double hFov = tan(fovx);
	double vFov = tan(((double)height / width) * 47 * PIOVER180);
	
	up.normalize();
	
	w = target - pos;
	w.normalize();

	u = up.cross(w);
	u *= hFov;

	v = w.cross(u);
	v *= vFov;
}

void Camera::project(Real x, Real y, Ray &r) const
{
	r.origin = pos;

	r.dir = x * u + y * v + w;
	r.dir.normalize();
}
