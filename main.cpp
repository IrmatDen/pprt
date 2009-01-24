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
	string scnFile = "ScnSamples\\spheres_plane_disk.scn";

	Scene scn;
	bool scnLoaded = scn.loadScnFile(scnFile);
	if (scnLoaded)
	{
		cout << endl << "Parsed \"" << scnFile << "\" successfully!" << endl;
		cout << "Start rendering..." << endl;
		__int64 begin = timeGetTime();
		scn.render();
		__int64 end = timeGetTime();
		cout << "Finished rendering, time: " << end - begin << " ms" << endl;
	}
	else
		cout << "Loading " << scnFile << " failed!" << endl;
	
	cout << endl << "Press return to quit..." << endl;
	cin.get();

	return !scnLoaded;
}