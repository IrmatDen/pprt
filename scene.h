#ifndef CRT_SCENE_H
#define CRT_SCENE_H

#include <vector>
#include <string>
#include <cmath>

#include "ray.h"
#include "color4.h"
#include "geometry.h"
#include "light.h"

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
	
	struct Camera
	{
		Camera()
			:target(0, 0, 1), up(0, 1, 0), fov(60)
		{
		}

		Vec3f pos;
		Vec3f target;
		Vec3f up;
		float fov;
	};

public:
	Scene(int width, int height)
		: resX(width), resY(height)
	{
	}

	Camera&		camera()									{ return cam; }

	void		setBackground(const Color4 &bg)				{ background = bg; }

	void		addGeometry(GeometryPtr obj)				{ objects.push_back(obj); }
	void		addLight(LightPtr l)						{ lights.push_back(l); }

	void		render(const std::string &filename);
	Color4		trace(Ray r, int maxRecurse = 10, bool returnBackground = false);

	bool		collide(const Ray &r, float &t);

private:
	void		setupCamData();

	// Set ray direction from screen coordinates.
	void		createRay(float x, float y, Ray &r);

private:
	typedef std::vector<GeometryPtr> Geometries;
	typedef std::vector<LightPtr> Lights;

private:
	struct CamData
	{
		// u & v will be pre-modified by defined fov.
		Vec3f u, v, w;
	};

private:
	int						resX, resY;

	Color4					background;

	Camera					cam;
	CamData					camData;

	Geometries				objects;
	Lights					lights;
};

#endif
