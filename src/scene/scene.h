#ifndef CRT_SCENE_H
#define CRT_SCENE_H

#include <vector>
#include <string>
#include <cmath>

#include "camera.h"
#include "color.h"
#include "geometry.h"
#include "light.h"
#include "ray.h"

#include "../shadervm/shader_manager.h"

class BVH;

class Scene
{
	friend class TraceScanLine;

public:
	typedef std::vector<Geometry*>	Geometries;
	typedef std::vector<Light*>		Lights;

public:
	ShaderManager shaderManager;

public:
	#pragma warning(disable:4355)
	Scene(int width=0, int height=0)
		: resX(width), resY(height), storeZ(false),
		background(all_zero()),
		rt_objects(0), rt_lights(0),
		bvhRoot(0)
	{
		shaderManager.setScene(*this);
	}
	#pragma warning(default:4355)

	~Scene();

	bool		loadScnFile(const std::string &filename);

	void		setOutputFile(const std::string &outFile)	{ outName = outFile; }
	void		setWidth(int width)							{ resX = width; }
	void		setHeight(int height)						{ resY = height; }
	void		storeZValues(bool store)					{ storeZ = store; }

	Camera&		camera();

	void		setBackground(const Color &bg)				{ background = bg; }

	void		addGeometry(Geometry* obj)					{ objects.push_back(obj); }
	void		addLight(Light* l)							{ lights.push_back(l); }

	void		prepare();

	void		render();
	Color		trace(const Ray &ray, bool &hitSomething);

	bool		collide(const Ray &r, float t, Color &visQty, Color &influencedColor) const;

public:
	// Shading & lighting functions
	Color		traceNoDepthMod(Ray &eye, bool &hitSomething);
	void		ambient(Color &out) const;
	void		diffuse(const Ray &r, Color &out) const;
	void		specular(const Ray &r, const Vector3 &viewDir, float roughness, Color &out) const;

private:
	std::string				outName;
	int						resX, resY;
	bool					storeZ;

	Color					background;

	Camera					cam;

	Geometries				objects;
	Lights					lights;

	Geometry			**	rt_objects;
	Light				**	rt_lights;

	BVH					*	bvhRoot;
};

#endif
