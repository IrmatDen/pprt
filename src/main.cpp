#include <windows.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <CGTFile.h>

#include "scene/scene.h"
#include "shadervm/shader_manager.h"

using namespace std;

int main(int argc, char **argv)
{
	/*ShaderManager sm;
	sm.loadFile("./Shaders/specular_sample.crtsl");

	return 0;*/

	//string scnFile = "ScnSamples/constant_color_shaded_sphere.crtscn";
	//string scnFile = "ScnSamples/diffuse_shaded_sphere.crtscn";
	//string scnFile = "ScnSamples/normals_shaded_sphere.crtscn";
	//string scnFile = "ScnSamples/specular_shaded_sphere.crtscn";
	string scnFile = "ScnSamples/trace_shaded_sphere.crtscn";

	Scene scn;
	bool scnLoaded = scn.loadScnFile(scnFile);
	if (scnLoaded)
	{
		cout << endl << "Parsed \"" << scnFile << "\" successfully!" << endl;
		cout << "Start rendering..." << endl;
		scn.prepare();
		__int64 begin = timeGetTime();
		scn.render();
		__int64 end = timeGetTime();
		cout << "Finished rendering, time: " << end - begin << " ms" << endl;
	}
	else
		cout << "Loading " << scnFile << " failed!" << endl;
	
	/*cout << endl << "Press return to quit..." << endl;
	cin.get();*/

	return !scnLoaded;
}