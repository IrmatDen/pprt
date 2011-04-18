#include <string>

#include "scene/scene.h"

using namespace std;

int main(int argc, char **argv)
{
	//string scnFile = "ScnSamples/constant_color_shaded_sphere.crtscn";
	//string scnFile = "ScnSamples/normals_shaded_sphere.crtscn";
	//string scnFile = "ScnSamples/diffuse_shaded_sphere.crtscn";
	//string scnFile = "ScnSamples/specular_shaded_sphere.crtscn";
	//string scnFile = "ScnSamples/trace_shaded_sphere.crtscn";
	//string scnFile = "ScnSamples/opa_shaded_sphere.crtscn";
	//string scnFile = "ScnSamples/reflexive_opa_spheres.crtscn";
	//string scnFile = "ScnSamples/inner_reflexion.crtscn";
	//string scnFile = "ScnSamples/smoothstep.crtscn";
	//string scnFile = "ScnSamples/mix.crtscn";
	string scnFile = "ScnSamples/spheres_plane.crtscn";

	Scene scn;
	bool scnLoaded = scn.loadSceneFile(scnFile);
	if (!scnLoaded)
		cout << "Loading " << scnFile << " failed!" << endl;

	return !scnLoaded;
}