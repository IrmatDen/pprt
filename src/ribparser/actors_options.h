#ifndef PPRT_ACTORS_OPTIONS_H
#define PPRT_ACTORS_OPTIONS_H

#include "parser_typedefs.h"

#include <string>
#include <vector>

class Scene;

struct CameraSettings
{
	// Format
	static int		resX, resY;
	static double	pix_aspectRatio;

	// Projection
	static std::string	projType;
	static double		fov;

	// Clipping
	static double	hither, yon;
};

struct displayType_a
{
	displayType_a(Scene &scn);
	
	void operator()(const iterator_t &first, const iterator_t &end) const;

	Scene &scene;
};

struct ShaderPath
{
	ShaderPath(Scene &scn);

	void operator=(const std::vector<std::string> &folders);
	void processFolder(const std::string &folderName) const;

	Scene &scene;
};

#endif
