#ifndef PPRT_SCENE_H
#define PPRT_SCENE_H

#include "bvh.h"
#include "camera.h"
#include "color.h"
#include "geometry.h"
#include "light.h"
#include "ray.h"
#include "pixel_store.h"

#include "../shadervm/shader_manager.h"

#include <vector>
#include <string>
#include <cmath>

#include <SFML/System/Thread.hpp>

template <typename PixelStoreT> class Framebuffer;
template <typename PixelStoreT> class TraceBlock;

enum DisplayType
{
	DT_File,
	DT_Framebuffer,
};

class Scene
{
public:
	typedef std::vector<Geometry*>	Geometries;
	typedef std::vector<Light*>		Lights;

public:
	ShaderManager shaderManager;

public:
	Scene();
	~Scene();

	void		clear();

	bool		loadSceneFile(const std::string &filename);
	bool		reloadSceneFile();

	void		setDisplayName(const std::string &name)		{ displayName = name; }
	void		setDisplayType(DisplayType type)			{ displayType = type; }

	void		enableThreading(bool enable);

	// double to keep boost.spirit happy
	void		setXPixelSamples(double xs);
	float		getXPixelSteps() const						{ return xInvSamples; }
	// double to keep boost.spirit happy
	void		setYPixelSamples(double ys);
	float		getYPixelSteps() const						{ return yInvSamples; }

    void        setMaxTraceDetph(int n)                     { maxTraceDetph = n; }

	void		setBackground(const Color &bg)				{ background = bg; }

	void		addGeometry(Geometry* obj)					{ objects.push_back(obj); }
	void		addLight(Light* l)							{ lights.push_back(l); }

	Camera&		camera();

	void		prepare();

	void		render();
	void		multithreadRender();
	void		monothreadRender();
	void		onRenderFinished();

	Color		trace(const Ray &ray, bool &hitSomething, Color &Oi = Color()) const;

public:
	// Shading & lighting functions
	Color		traceNoDepthMod(Ray &eye, bool &hitSomething, Color &Oi) const;
	void		ambient(Color &out) const;
	void		diffuse(const Ray &r, Color &out) const;
	void		specular(const Ray &r, const Vector3 &viewDir, float roughness, Color &out) const;

private:
    typedef BVH<Geometry, Geometries> SceneBVH;

private:
	std::string				sceneFileName;

	std::string				displayName;
	DisplayType				displayType;

	RGBAStore				*imgStore;
	Framebuffer<RGBAStore>	*fb;
	sf::Thread				*renderThread;
	TraceBlock<RGBAStore>	*tracer;

	bool		threadingEnabled;
	__int64		renderBeginTime;

	Color			background;

	float	xInvSamples, yInvSamples;
    int     maxTraceDetph;

	Camera			cam;

	Geometries		objects;
	Lights			lights;

	Geometry	**	rt_objects;
	Light		**	rt_lights;

	SceneBVH *	bvhRoot;
};

#endif
