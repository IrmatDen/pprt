#include <string>

#include "scene/scene.h"

using namespace std;

int main(int argc, char **argv)
{
	//string scnFile = "ScnSamples/constant_color_shaded_sphere.rib";
	//string scnFile = "ScnSamples/normals_shaded_sphere.rib";
	//string scnFile = "ScnSamples/diffuse_shaded_sphere.rib";
	//string scnFile = "ScnSamples/specular_shaded_sphere.rib";
	//string scnFile = "ScnSamples/trace_shaded_sphere.rib";
	//string scnFile = "ScnSamples/opa_shaded_sphere.rib";
	//string scnFile = "ScnSamples/smoothstep.rib";
	//string scnFile = "ScnSamples/mix.rib";
	string scnFile = "ScnSamples/spheres_plane.rib";

	Scene scn;
	bool scnLoaded = scn.loadSceneFile(scnFile);
	if (!scnLoaded)
		cout << "Loading " << scnFile << " failed!" << endl;

	return !scnLoaded;
}