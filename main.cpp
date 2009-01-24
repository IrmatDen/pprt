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
	Scene scn;

	/*
	Vec3f dn(-0.2f, -0.3f, -1);
	dn.normalize();
	s = GeometryPtr(new Disk(1, Vec3f(-1, 1.5f, 7), dn));
	s->material().ambient = Color4(0.1f, 0, 0, 1);
	s->material().diffuse = Color4(0.8f, 0.8f, 0.8f, 1);
	s->material().reflexivity = 0.9f;
	scn.addGeometry(s);*/

	string scnFile = "ScnSamples\\spheres_plane.scn";

	bool scnLoaded = scn.loadScnFile(scnFile);
	if (!scnLoaded)
		cout << "Loading " << scnFile << " failed!" << endl;
	else
	{
		cout << endl << "Parsed \"" << scnFile << "\" successfully!" << endl;
		cout << "Start rendering..." << endl;
		__int64 begin = timeGetTime();
		scn.render();
		__int64 end = timeGetTime();
		cout << "Finished rendering, time: " << end - begin << " ms" << endl;
	}
	
	cout << endl << "Press return to quit..." << endl;
	cin.get();

	return !scnLoaded;
}