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
	//string scnFile = "ScnSamples/trace_shaded_sphere.crtscn";
	string scnFile = "ScnSamples/opa_shaded_sphere.crtscn";
	//string scnFile = "ScnSamples/reflexive_opa_spheres.crtscn";
	//string scnFile = "ScnSamples/inner_reflexion.crtscn";

	Scene scn;
	bool scnLoaded = scn.loadScnFile(scnFile);
	if (scnLoaded)
	{
		cout << endl << "Parsed \"" << scnFile << "\" successfully!" << endl;

		cout << "Preparing scene..." << endl;
		__int64 begin = timeGetTime();
		scn.prepare();
		__int64 end = timeGetTime();
		cout << "Finished preparing, duration: " << end - begin << " ms" << endl;

		cout << "Start rendering..." << endl;
		begin = timeGetTime();
		scn.render();
		end = timeGetTime();
		cout << "Finished rendering, duration: " << end - begin << " ms" << endl;
	}
	else
		cout << "Loading " << scnFile << " failed!" << endl;
	
	/*cout << endl << "Press return to quit..." << endl;
	cin.get();*/

	return !scnLoaded;
}