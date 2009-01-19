#ifndef CRT_SCENE_H
#define CRT_SCENE_H

#include <vector>
#include <string>
#include <cmath>

#include "ray.h"
#include "color4.h"
#include "geometry.h"
#include "light.h"
#include "camera.h"

class Scene
{
	friend class TraceScanLine;

public:
	enum OutputFormat
	{
		Jpeg,
		Png,
		Tiff
	};

public:
	Scene(int width, int height)
		: resX(width), resY(height)
	{
	}

	void		loadFromRib(std::string filename);

	Camera&		camera();

	void		setBackground(const Color4 &bg)				{ background = bg; }

	void		addGeometry(GeometryPtr obj)				{ objects.push_back(obj); }
	void		addLight(LightPtr l)						{ lights.push_back(l); }

	void		render(const std::string &filename);
	Color4		trace(const Ray &eye, int maxRecurse = 10, bool returnBackground = false);

	bool		collide(const Ray &r, float &t);

private:
	typedef std::vector<GeometryPtr> Geometries;
	typedef std::vector<LightPtr> Lights;

private:
	int						resX, resY;

	Color4					background;

	Camera					cam;

	Geometries				objects;
	Lights					lights;
};

#endif
