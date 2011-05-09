#include "scene.h"
#include "bvh.h"
#include "framebuffer.h"
#include "traceblock.h"

#include "../ribparser/ribparser.h"
#include "../memory.h"

#include <limits>
#include <algorithm>
#include <set>
#include <iostream>

#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>

#include <FreeImagePlus.h>

#include <SFML/Graphics.hpp>

#include <tbb/task_scheduler_init.h>
#include <tbb/blocked_range2d.h>
#include <tbb/parallel_for.h>

#include <windows.h>

using namespace std;

#pragma warning(disable:4355)
Scene::Scene()
	:	background(all_zero()),
		rt_objects(nullptr), rt_lights(nullptr),
		bvhRoot(nullptr),
		imgStore(nullptr), fb(nullptr), renderThread(nullptr), tracer(nullptr),
		threadingEnabled(true), xInvSamples(0.5f), yInvSamples(0.5f), maxTraceDetph(8)
{
	shaderManager.setScene(*this);
}
#pragma warning(default:4355)

Scene::~Scene()
{
	clear();
}

void Scene::clear()
{
	if (bvhRoot)
		memory::destroy(bvhRoot);

	for(Geometries::iterator it = objects.begin(); it != objects.end(); it++)
		memory::destroy(*it);

	for(Lights::iterator it = lights.begin(); it != lights.end(); it++)
		memory::destroy(*it);
	
	memory::destroy(rt_lights);
	
	objects.clear();
	lights.clear();
	shaderManager.reset();
}

bool Scene::loadSceneFile(const string &filename)
{
	sceneFileName = filename;
	return reloadSceneFile();
}

bool Scene::reloadSceneFile()
{
	clear();

	RibParser::Parser scnParser(*this);
	const __int64 begin = timeGetTime();
	bool parseRes = scnParser.parseFile(sceneFileName);
	const __int64 parseStep = timeGetTime();
	
	if (parseRes)
	{
		cout << endl << "Parsed \"" << sceneFileName << "\" successfully! (" << parseStep - begin << " ms)" << endl;

		cout << "Preparing scene..." << endl;
		prepare();
		const __int64 prepareStep = timeGetTime();
		cout << "Finished preparing, duration: " << prepareStep - parseStep << " ms" << endl;

		render();
	}

	return parseRes;
}

void Scene::enableThreading(bool enable)
{
	threadingEnabled = enable;
}

void Scene::setXPixelSamples(double xs)
{
	xInvSamples = static_cast<float>(1.0 / max(1.0, xs));
}

void Scene::setYPixelSamples(double ys)
{
	xInvSamples = static_cast<float>(1.0 / max(1.0, ys));
}

Camera& Scene::camera()
{
	return cam;
}

void Scene::prepare()
{
	for(Geometries::iterator it = objects.begin(); it != objects.end(); ++it)
		(*it)->prepare();

	rt_lights = memory::construct<Light*>(lights.size() + 1);
	int loop = 0;
	for(Lights::iterator it = lights.begin(); it != lights.end(); ++it, ++loop)
		rt_lights[loop] = *it;
	rt_lights[loop] = 0;
	
	bvhRoot = memory::construct<SceneBVH>();
	bvhRoot->build(objects);
}

void Scene::render()
{
	if (fb == nullptr)
		fb = new Framebuffer<RGBAStore>(*this);
	
	delete imgStore;
	delete renderThread;
	delete tracer;
	
	imgStore = new RGBAStore(cam.getWidth(), cam.getHeight());

	if (displayType == DT_File)
	{
		tracer = new TraceBlock<RGBAStore>(*this, *imgStore);
	}
	else // DT_Framebuffer
	{
		fb->create(*imgStore, displayName);
		fb->allowSceneReload(false);
		
		tracer = new TraceBlock<RGBAStore>(*this, *imgStore, fb);
	}
	
	cout << "Start rendering..." << endl;
	renderBeginTime = timeGetTime();
	if (threadingEnabled)
		renderThread = new sf::Thread(&Scene::multithreadRender, this);
	else
		renderThread = new sf::Thread(&Scene::monothreadRender, this);

	renderThread->Launch();
	
	if (displayType == DT_File)
	{
		renderThread->Wait();

		fipImage img(FIT_BITMAP, (WORD)cam.getWidth(), (WORD)cam.getHeight(), 32);

		for (int y = 0; y != cam.getHeight(); y++)
		{
			RGBAStore::pixel_t *pixelsData = imgStore->getScanline(y);
			BYTE *imgData = img.getScanLine(y);

			for (int x = 0; x != cam.getWidth(); x++, imgData += 4, pixelsData += imgStore->getPixelSize())
			{
				imgData[FI_RGBA_RED]	= BYTE(pixelsData[0]);
				imgData[FI_RGBA_GREEN]	= BYTE(pixelsData[1]);
				imgData[FI_RGBA_BLUE]	= BYTE(pixelsData[2]);
				imgData[FI_RGBA_ALPHA]	= BYTE(pixelsData[3]);
			}
		}
		
		img.flipVertical();
		img.save(displayName.c_str());
	}
	else
	{
		fb->run();
	}
}

void Scene::multithreadRender()
{
	tbb::task_scheduler_init tbbInit;
	tbb::parallel_for(tbb::blocked_range2d<int>(0, cam.getHeight(), 0, cam.getWidth()), *tracer, tbb::auto_partitioner());

	onRenderFinished();
}

void Scene::monothreadRender()
{
	const int blockSide = 32;

	int y = 0;
	const int	maxY = (cam.getHeight() / blockSide) * blockSide,
				remY = cam.getHeight() - maxY;
	const int	maxX = (cam.getWidth() / blockSide) * blockSide,
				remX = cam.getWidth() - maxX;
	for (; y != maxY; y += blockSide)
	{
		int x = 0;
		for (; x < maxX; x += blockSide)
			(*tracer)(tbb::blocked_range2d<int>(y, y + blockSide, x, x + blockSide));

		if (remX > 0)
			(*tracer)(tbb::blocked_range2d<int>(y, y + blockSide, maxX, cam.getWidth()));
	}

	if (remY > 0)
	{
		int x = 0;
		for (; x < maxX; x += blockSide)
			(*tracer)(tbb::blocked_range2d<int>(maxY, cam.getHeight(), x, x + blockSide));

		if (remX > 0)
			(*tracer)(tbb::blocked_range2d<int>(maxY, cam.getHeight(), maxX, cam.getWidth()));
	}

	onRenderFinished();
}

void Scene::onRenderFinished()
{
	__int64 end = timeGetTime();
	cout << "Finished rendering, duration: " << end - renderBeginTime << " ms" << endl;
	fb->allowSceneReload(true);
}

Color Scene::trace(const Ray &eye, bool &hitSomething, Color &Oi) const
{
	if (eye.traceDepth == 16)
	{
		return Color(all_zero());
	}

	Ray ray(eye);
	Oi = Color(all_zero());
	Color traced = traceNoDepthMod(ray, hitSomething, Oi);
	return traced + mulPerElem((Vector3(1) - Oi), background);
}

Color Scene::traceNoDepthMod(Ray &ray, bool &hitSomething, Color &Oi) const
{
	if (ray.traceDepth == maxTraceDetph)
	{
		hitSomething = false;
		return Color(all_zero());
	}
	ray.traceDepth++;

	const float prevMaxT = ray.maxT;

	IntersectionInfo info;
	const Geometry *nearestObj = bvhRoot->findClosest(ray, info);

	if (!nearestObj)
	{
		hitSomething = false;
		return Color(all_zero());
	}

	hitSomething = true;

	// Object doesn't have shader, make it appear even for blind people!
	if (!nearestObj->hasShader())
	{
		Oi = Color(sse::all_one);
		return Color(1, 0, 1);
	}

	CompiledShader shader(nearestObj->getShader(), true);
	shader.setCurrentDepth(ray.traceDepth);
    shader.setRTVarValueByIndex(CompiledShader::Cs, info.Cs);
    shader.setRTVarValueByIndex(CompiledShader::Os, info.Os);
	shader.setRTVarValueByIndex(CompiledShader::P, Vector3(info.P));
	shader.setRTVarValueByIndex(CompiledShader::N, info.N);
	shader.setRTVarValueByIndex(CompiledShader::Ng, info.Ng);
	shader.setRTVarValueByIndex(CompiledShader::s, info.s);
	shader.setRTVarValueByIndex(CompiledShader::t, info.t);
	shader.setRTVarValueByIndex(CompiledShader::I, Vector3((cam.WorldToCamN * ray.direction()).get128()));
	shader.exec();

	Color Ci, thisOi;
	shader.getOutput(Ci, thisOi);
	Oi += thisOi;

	if (isOpaque(Oi))
		return Ci;

	ray.origin = info.P + (ray.direction() * 0.001f);
	ray.maxT = prevMaxT - ray.maxT;

	bool nextHit;
	const Color nextColor = traceNoDepthMod(ray, nextHit, Oi);

	if (!nextHit)
		return Ci;

	return Ci + mulPerElem((Vector3(1) - thisOi), nextColor);
}

void Scene::ambient(Color &out) const
{
	out = Color(0.15f);
}

void Scene::diffuse(const Ray &r, Color &out) const
{
	Vector3 dir = r.direction();
	const Light **light = (const Light**)rt_lights;

	Ray ray(r);

	while (*light)
	{
		// Check if the current light is occluded
        Vector3 worldL2P = (*light)->pos - r.origin;
		const float maxDist = length(worldL2P);
        const float invMaxDist = 1.f / maxDist;

		Vector3 camSpaceL2P = Vector3((cam.WorldToCamN * worldL2P).get128());
		camSpaceL2P *= invMaxDist;
		const float L2PdotN = dot(camSpaceL2P, dir);
		if (L2PdotN < 0)
		{
			++light;
			continue;
		}

		ray.maxT = maxDist;
		ray.setDirection(worldL2P * invMaxDist);
	    ray.origin = r.origin + ray.direction() * 0.0001f;

		bool hit;
		Color opacity(all_zero());
		Color influencedColor = trace(ray, hit, opacity);
		Color visibility = (Color(sse::all_one) - opacity);

		if (!hit)
		{
			out += (*light)->color * L2PdotN;
		}
		else if (minElem(visibility) > 0.001f)
		{
			Color receivedLight = mulPerElem(visibility, (*light)->color);
			receivedLight += influencedColor;
			out += receivedLight * L2PdotN;
		}

		++light;
	}
}

void Scene::specular(const Ray &r, const Vector3 &viewDir, float roughness, Color &out) const
{
	const Vector3 &dir = r.direction();
	const Light **light = (const Light**)rt_lights;

	Ray ray(r);

	while (*light)
	{
		// Check if the current light is occluded
        Vector3 worldL2P = (*light)->pos - r.origin;
		const float maxDist = length(worldL2P);
        const float invMaxDist = 1.f / maxDist;

		Vector3 camSpaceL2P = Vector3((cam.WorldToCamN * worldL2P).get128());
		camSpaceL2P *= invMaxDist;
		const float L2PdotN = dot(camSpaceL2P, dir);
		if (L2PdotN < 0)
		{
			++light;
			continue;
		}

		ray.maxT = maxDist;
		ray.setDirection(worldL2P * invMaxDist);
	    ray.origin = r.origin + ray.direction() * 0.0001f;

		bool hit;
		Color opacity(all_zero());
		Color influencedColor = trace(ray, hit, opacity);
		Color visibility = (Color(sse::all_one) - opacity);

		if (!hit || minElem(visibility) > 0.001f)
		{
			const Vector3 H = normalize(camSpaceL2P + viewDir);
			const float NdH = dot(dir, H);

            //! \note The current roughness modifier (8.f) emulates Aqsis, which emulates BMRT
            const float roughnessModifier = 8.f;
			const float specMult = static_cast<float>(pow(max(0.f, NdH), roughnessModifier / roughness));

			if (!hit)
				out += (*light)->color * specMult;
			else
			{
				Color receivedLight = mulPerElem(visibility, (*light)->color);
				receivedLight += influencedColor;
				out += receivedLight * specMult;
			}
		}

		++light;
	}
}
