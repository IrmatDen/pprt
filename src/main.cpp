#include <windows.h>

#include <iostream>
#include <string>

#include "scene/scene.h"
#include "shadervm/crtsl_parser.h"

using namespace std;

int main(int argc, char **argv)
{
	string scnFile = "ScnSamples/diffuse_shaded_sphere.crtscn";

	Scene scn;
	bool scnLoaded = scn.loadScnFile(scnFile);
	if (scnLoaded)
	{
		cout << endl << "Parsed \"" << scnFile << "\" successfully!" << endl;
		//cout << endl << "Press return to start rendering..." << endl;
		//cin.get();
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

	/*Scene scn;
	string slFile = "Shaders/constantcolor.crtsl";
	SLParser::Parser p(scn);
	p.parseFile(slFile);
	
	cout << endl << "Press return to quit..." << endl;
	cin.get();

	return 0;*/
}