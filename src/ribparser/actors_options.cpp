#include "actors_options.h"

#include "../scene/scene.h"

#include <boost/filesystem.hpp>

#include <iostream>
#include <limits>

using namespace std;
namespace fs = boost::filesystem;

//----------------------------------------------------
// Camera option

int		CameraSettings::resX			= 640;
int		CameraSettings::resY			= 480;
double	CameraSettings::pix_aspectRatio	= 1.0;

string	CameraSettings::projType("orthographic");
double	CameraSettings::fov = 90.0;

// Getting eps/max from float & cast to double is required because:
// 1. boost.spirit real parser expects double
// 2. but the renderer works with floats
double	CameraSettings::hither	= static_cast<double>(numeric_limits<float>::epsilon());
double	CameraSettings::yon		= static_cast<double>(numeric_limits<float>::max());

//----------------------------------------------------
// Display option

displayType_a::displayType_a(Scene &scn)
	: scene(scn)
{
}
	
void displayType_a::operator()(const iterator_t &first, const iterator_t &end) const
{
	string type_str(first, end);
	type_str.erase(0, 1);
	type_str.erase(type_str.length() - 1, 1);

	if (type_str == "file")
	{
		scene.setDisplayType(DT_File);
	}
	else if (type_str == "framebuffer")
	{
		scene.setDisplayType(DT_Framebuffer);
	}
	else
	{
		cout << "Unknown display type: \"" << type_str << "\", defaulting to framebuffer." << endl;
		scene.setDisplayType(DT_Framebuffer);
	}
}

//----------------------------------------------------
// Additional options

ShaderPath::ShaderPath(Scene &scn)
	: scene(scn)
{
}

void ShaderPath::operator=(const std::vector<std::string> &folders)
{
	using namespace std;

	vector<string> newFolders(folders);
	string defaultFolder("./shaders");
	replace(newFolders.begin(), newFolders.end(), string("&"), defaultFolder);
	for_each(newFolders.begin(), newFolders.end(), [&] (const string &f) { processFolder(f); } );
}

void ShaderPath::processFolder(const std::string &folderName) const
{
	//! \todo throw path not a folder/not found exception

	if (fs::is_directory(folderName))
	{
		for (fs::directory_iterator it(folderName); it != fs::directory_iterator(); ++it)
		{
			fs::path p = it->path();
			if (p.extension() == ".sl")
			{
				scene.shaderManager.loadFile(p.file_string());
			}
		}
	}
}
