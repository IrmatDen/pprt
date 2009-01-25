#ifndef CRT_SCENE_H
#define CRT_SCENE_H

#include <vector>
#include <string>
#include <cmath>

#include "camera.h"
#include "color4.h"
#include "geometry.h"
#include "light.h"
#include "material.h"
#include "ray.h"

class Scene
{
	friend class TraceScanLine;

public:
	Scene(int width=0, int height=0)
		: resX(width), resY(height), storeZ(false),
		background(0, 0, 0, 0)
	{
	}

	bool		loadScnFile(const std::string &filename);

	void		setOutputFile(const std::string &outFile)	{ outName = outFile; }
	void		setWidth(int width)							{ resX = width; }
	void		setHeight(int height)						{ resY = height; }
	void		storeZValues(bool store)					{ storeZ = store; }

	Camera&		camera();

	void		setBackground(const Color4 &bg)				{ background = bg; }

	void		addGeometry(GeometryPtr obj)				{ objects.push_back(obj); }
	void		addMaterial(MaterialPtr mat)				{ materials.push_back(mat); }
	void		addLight(LightPtr l)						{ lights.push_back(l); }

	MaterialPtr	getMaterialByName(const std::string &name);

	void		render();
	Color4		trace(const Ray &eye, int maxRecurse = 10, bool returnBackground = false);

	bool		collide(const Ray &r, Real &t);

private:
	typedef std::vector<GeometryPtr> Geometries;
	typedef std::vector<MaterialPtr> Materials;
	typedef std::vector<LightPtr> Lights;

private:
	std::string				outName;
	int						resX, resY;
	bool					storeZ;

	Color4					background;

	Camera					cam;

	Geometries				objects;
	Materials				materials;
	Lights					lights;
};

#endif
