#include <windows.h>

#include <iostream>

#include "sphere.h"
#include "plane.h"
#include "light.h"
#include "scene.h"

using namespace std;

int main(int argc, char **argv)
{
	Scene scn(1024, 768);
	scn.setBackground(Color4(0, 0, 0, 0));

	scn.camera().fov = 45;
	scn.camera().pos = Vec3f(0, 0, 0);
	scn.camera().target = Vec3f(0, -2, 5);

	GeometryPtr s(new Sphere(1, Vec3f(0, -0.8f, 5)));
	s->material().ambient = Color4(0.1f, 0, 0, 1);
	s->material().diffuse = Color4(1, 0, 0, 1);
	s->material().reflexivity = 0.5f;
	scn.addGeometry(s);

	s = GeometryPtr(new Sphere(1, Vec3f(-0.8f, 0.8f, 5)));
	s->material().ambient = Color4(0, 0.1f, 0, 1);
	s->material().diffuse = Color4(0, 1, 0, 1);
	s->material().reflexivity = 0.5f;
	scn.addGeometry(s);

	s = GeometryPtr(new Sphere(1, Vec3f(0.8f, 0.8f, 5)));
	s->material().ambient = Color4(0, 0, 0.1f, 1);
	s->material().diffuse = Color4(0, 0, 1, 1);
	s->material().reflexivity = 0.5f;
	scn.addGeometry(s);

	s = GeometryPtr(new Plane(Vec3f(0, 1, 0), -2));
	s->material().ambient = Color4(0.1f, 0.1f, 0.1f, 1);
	s->material().diffuse = Color4(1, 0, 1, 1);
	s->material().reflexivity = 0.8f;
	scn.addGeometry(s);

	LightPtr l(new Light());
	l->pos = Vec3f(-4, -1.5f, 3);
	l->color = Color4(1, 1, 1, 1);
	scn.addLight(l);
	
	l = LightPtr(new Light());
	l->pos = Vec3f(10, 0, -50);
	l->color = Color4(0.15f, 0.15f, 0.1f, 1);
	scn.addLight(l);

    __int64 begin = timeGetTime();
	scn.render("test.png");
    __int64 end = timeGetTime();

	cout << end - begin << " ms";
	cin.get();

	return 0;
}