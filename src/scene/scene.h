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

class Scene
{
	friend class TraceScanLine;

public:
	ShaderManager shaderManager;

public:
	#pragma warning(disable:4355)
	Scene(int width=0, int height=0)
		: resX(width), resY(height), storeZ(false),
		background(0, 0, 0),
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

	void		addGeometry(GeometryPtr obj)				{ objects.push_back(obj); }
	void		addLight(LightPtr l)						{ lights.push_back(l); }

	void		prepare();

	void		render();
	Color		trace(const Ray &ray, bool &hitSomething);

	bool		collide(const Ray &r, Real t, Color &visQty) const;

public:
	// Shading & lighting functions
	Color		traceNoDepthMod(Ray &eye, bool &hitSomething);
	void		diffuse(const Ray &r, Color &out) const;
	void		specular(const Ray &r, const Vec3 &viewDir, Real roughness, Color &out) const;

private:
	typedef std::vector<GeometryPtr> Geometries;
	typedef std::vector<LightPtr> Lights;

	struct BVHNode
	{
		BVHNode() : left(0), right(0), geoLeft(0), geoRight(0)	{}

		inline bool		isLeaf() { return !left && !right; }

		bool			visit(const Ray &ray, Real &t) const;

		AABB		aabb;

		BVHNode	*	left;
		BVHNode	*	right;

		Geometry *	geoLeft;
		Geometry *	geoRight;
	};

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

	BVHNode				*	bvhRoot;
};

#endif
