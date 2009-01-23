#include <windows.h>

#include <iostream>
#include <string>

#include "sphere.h"
#include "plane.h"
#include "light.h"
#include "scene.h"
#include "camera.h"
#include "disk.h"

using namespace std;

int main(int argc, char **argv)
{
	Scene scn(1280, 1024);
	scn.setBackground(Color4(0, 0, 0, 0));

	scn.camera().fov = 45;
	scn.camera().pos = Vec3f(-3, 2, 0);
	scn.camera().target = Vec3f(0, 0, 5);

	//Disk test scene.
	/*Vec3f dn(1, 1, 0);
	dn.normalize();
	GeometryPtr s(new Disk(1, Vec3f(0, -2, 5), dn));
	s->material().ambient = Color4(0.1f, 0, 0, 1);
	s->material().diffuse = Color4(1, 0, 0, 1);
	s->material().reflexivity = 0;
	scn.addGeometry(s);

	LightPtr l(new Light());
	l->pos = Vec3f(0, 0.3f, 5);
	l->color = Color4(1, 1, 1, 1);
	scn.addLight(l);*/

	/*GeometryPtr s(new Sphere(1, Vec3f(0, -0.8f, 5)));
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
	
	Vec3f dn(-0.2f, -0.3f, -1);
	dn.normalize();
	s = GeometryPtr(new Disk(1, Vec3f(-1, 1.5f, 7), dn));
	s->material().ambient = Color4(0.1f, 0, 0, 1);
	s->material().diffuse = Color4(0.8f, 0.8f, 0.8f, 1);
	s->material().reflexivity = 0.9f;
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
	cin.get();*/

	string scnFile = "ScnSamples\\sphere.scn";

	bool scnLoaded = scn.loadScnFile(scnFile);
	if (!scnLoaded)
	{
		cout << "Loading " << scnFile << " failed!" << endl;
		return 1;
	}

	cout << endl << "Parsed \"" << scnFile << "\" successfully!" << endl;
	cin.get();

	return 0;
}