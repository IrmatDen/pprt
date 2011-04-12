#include <limits>
#include <algorithm>
#include <set>

#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>

#include <FreeImagePlus.h>

#include <tbb/task_scheduler_init.h>
#include <tbb/blocked_range.h>
#include <tbb/blocked_range2d.h>
#include <tbb/parallel_for.h>

#include "scene.h"
#include "bvh.h"

#include "../crtscn_parser/scnparser.h"

#include "../common.h"

class TraceScanLine
{
public:
	TraceScanLine(Scene &scene, fipImage &out)
		:scn(scene), img(out)
	{
	}

	void operator()(const tbb::blocked_range2d<int> &r) const
	//void operator()(const tbb::blocked_range<int> &r) const
	{
		Scene &scene = scn;

		Ray ray;
		ray.origin = scene.cam.pos;

		for (int y = r.rows().begin(); y != r.rows().end(); ++y)
		//for (int y = r.begin(); y != r.end(); ++y)
		{
			BYTE *imgData = img.getScanLine(y);
			imgData += 4 * r.cols().begin();

			for (int x = r.cols().begin(); x < r.cols().end(); x++, imgData += 4)
			//for (int x = 0; x < scene.resX; x++, imgData += 4)
			{
				Color col(0, 0, 0);
				bool hitSomething;

				for (float fragx = (float)x; fragx < x + 1.0f; fragx += 0.5f)
				{
					for (float fragy = (float)y; fragy < y + 1.0f; fragy += 0.5f)
					{
						// Bring x & y in [-1,1] range, and generate primary's ray dir.
						float fx =		fragx * 1.0f / scene.resX * 2 - 1;
						float fy = 1 -	fragy * 1.0f / scene.resY * 2;

						scene.cam.project(fx, fy, ray);

						Color traceCol = scene.trace(ray, hitSomething);
						if (hitSomething)
							col += traceCol * 0.25f;
						else
							col += scene.background * 0.25f;
					}
				}

				/*const float exposure = -0.66f;
				col.r = 1 - expf(col.r * exposure);
				col.g = 1 - expf(col.g * exposure);
				col.b = 1 - expf(col.b * exposure);

				const float invGamma = 0.45f;
				col.r = powf(col.r, invGamma);
				col.g = powf(col.g, invGamma);
				col.b = powf(col.b, invGamma);*/

				clamp(col);
				
				const float	r = col.getX() * 255,
							g = col.getY() * 255,
							b = col.getZ() * 255;
				imgData[FI_RGBA_RED]	= BYTE(r);
				imgData[FI_RGBA_GREEN]	= BYTE(g);
				imgData[FI_RGBA_BLUE]	= BYTE(b);
				imgData[FI_RGBA_ALPHA]	= 255;
			}
		}
	}

private:
	Scene		&	scn;
	fipImage	&	img;
};

Scene::~Scene()
{
	for(Geometries::iterator it = objects.begin(); it != objects.end(); it++)
		memory::destroy(*it);

	for(Lights::iterator it = lights.begin(); it != lights.end(); it++)
		memory::destroy(*it);

	//delete [] rt_objects;
	
	memory::destroy(rt_lights);
}

bool Scene::loadScnFile(const std::string &filename)
{
	ScnParser::Parser scnParser(*this);
	bool parseRes = scnParser.parseFile(filename);

	return parseRes;
}

Camera& Scene::camera()
{
	return cam;
}

void Scene::prepare()
{
	/*rt_objects = new Geometry*[objects.size() + 1];
	int loop = 0;
	for(Geometries::iterator it = objects.begin(); it != objects.end(); ++it, ++loop)
		rt_objects[loop] = (*it).get();
	rt_objects[loop] = 0;*/

	rt_lights = memory::construct<Light*>(lights.size() + 1);
	int loop = 0;
	for(Lights::iterator it = lights.begin(); it != lights.end(); ++it, ++loop)
		rt_lights[loop] = *it;
	rt_lights[loop] = 0;
	
	bvhRoot = memory::construct<BVH>();
	bvhRoot->build(objects);
}

void Scene::render()
{

	fipImage img(FIT_BITMAP, (WORD)resX, (WORD)resY, 32);
	BYTE *imgData = img.accessPixels();

	cam.init(resX, resY);

#ifndef NOTHREADING
	tbb::task_scheduler_init tbbInit;
	tbb::parallel_for(tbb::blocked_range2d<int>(0, resY, 0, resX), TraceScanLine(*this, img), tbb::auto_partitioner());
	//tbb::parallel_for(tbb::blocked_range<int>(0, resY), TraceScanLine(*this, img), tbb::auto_partitioner());
#else
	Ray r;
	r.origin = cam.pos;
	for (int y = 0; y < resY; y++)
	{
		for (int x = 0; x < resX; x++, imgData += 4)
		{
			Color col(0, 0, 0);
			bool hitSomething;

			for (float fragx = (float)x; fragx < x + 1.0f; fragx += 0.5f)
			{
				for (float fragy = (float)y; fragy < y + 1.0f; fragy += 0.5f)
				{
					// Bring x & y in [-1,1] range, and generate primary's ray dir.
					float fx =		fragx * 1.0f / resX * 2 - 1;
					float fy = 1 -	fragy * 1.0f / resY * 2;

					cam.project(fx, fy, r);

					Color traceCol = trace(r, hitSomething);
					if (hitSomething)
						col += traceCol * 0.25f;
					else
						col += background * 0.25f;
				}
			}

			clamp(col);
			
			const float	r = col.getX() * 255,
						g = col.getY() * 255,
						b = col.getZ() * 255;
			imgData[FI_RGBA_RED]	= BYTE(r);
			imgData[FI_RGBA_GREEN]	= BYTE(g);
			imgData[FI_RGBA_BLUE]	= BYTE(b);
			imgData[FI_RGBA_ALPHA]	= 255;
		}
	}
#endif

	img.save(outName.c_str());
}

bool dummy;
Color Scene::trace(const Ray &eye, bool &hitSomething)
{
	if (eye.traceDepth == 16)
		return Color(0, 0, 0);

	Ray ray(eye);
	ray.traceDepth++;

	return traceNoDepthMod(ray, hitSomething);
}

Color Scene::traceNoDepthMod(Ray &ray, bool &hitSomething)
{
	if (ray.traceDepth == 16)
		return Color(sse::all_one);
	ray.traceDepth++;

	float t = 20000;
	const Geometry *nearestObj = bvhRoot->findClosest(ray, t);

	if (!nearestObj)
	{
		hitSomething = false;
		return Color(0, 0, 0);
	}

	hitSomething = true;

	// Object doesn't have shader, make it appear even for blind people!
	if (!nearestObj->hasShader())
		return Color(1, 0, 1);

	Vector3 p = ray.origin + ray.direction() * t;
	IntersectionInfo info;
	nearestObj->fillIntersectionInfo(p, info);

	CompiledShader shader(nearestObj->getShader(), true);
	shader.setCurrentDepth(ray.traceDepth);
	shader.setVarValueByIndex(CompiledShader::P, p);
	shader.setVarValueByIndex(CompiledShader::N, info.normal);
	shader.setVarValueByIndex(CompiledShader::Ng, info.normal);
	shader.setVarValueByIndex(CompiledShader::s, info.s);
	shader.setVarValueByIndex(CompiledShader::t, info.t);
	shader.setVarValueByIndex(CompiledShader::I, ray.direction());
	shader.exec();

	Color Ci, Oi;
	shader.getOutput(Ci, Oi);

	if (isOpaque(Oi))
		return Ci;

	ray.origin += ray.direction() * (t + 0.00000001f);
	return Ci + mulPerElem((Vector3(1) - Oi), traceNoDepthMod(ray, dummy));
}

bool Scene::collide(const Ray &r, float t, Color &visQty, Color &influencedColor) const
{
	const Geometry **obj = (const Geometry**)rt_objects;

	if (r.traceDepth >= 4)
	{
		visQty = Color();
		return true;
	}

	Ray ray = r;
	ray.traceDepth++;

	Color Ci, Oi;
	Vector3 p;

	visQty = Vector3(1);

	static const size_t	maxAccumulatedObjects(10);
	Geometry *			accum[maxAccumulatedObjects];
	float				dist[maxAccumulatedObjects];

	const size_t objGathered = bvhRoot->gatherAlong(ray, t, accum, dist, maxAccumulatedObjects);
	for (size_t i = 0; i < objGathered; i++)
	{
		p = ray.origin + ray.direction() * dist[i];
		IntersectionInfo info;
		accum[i]->fillIntersectionInfo(p, info);

		CompiledShader shader(accum[i]->getShader(), true);
		shader.setCurrentDepth(ray.traceDepth);
		shader.setVarValueByIndex(CompiledShader::P, p);
		shader.setVarValueByIndex(CompiledShader::N, info.normal);
		shader.setVarValueByIndex(CompiledShader::Ng, info.normal);
		shader.setVarValueByIndex(CompiledShader::s, info.s);
		shader.setVarValueByIndex(CompiledShader::t, info.t);
		shader.setVarValueByIndex(CompiledShader::I, ray.direction());
		shader.exec();

		shader.getOutput(Ci, Oi);

		if (isOpaque(Oi))
		{
			visQty = Vector3(all_zero());
			return true;
		}
		else
		{
			visQty = minPerElem(visQty, Oi);

			/*visQty.r = visQty.r <= 0.01f ? 0 : visQty.r;
			visQty.g = visQty.g <= 0.01f ? 0 : visQty.g;
			visQty.b = visQty.b <= 0.01f ? 0 : visQty.b;*/

			influencedColor += Ci;

			if (maxElem(visQty) <= 0.01f)
			{
				influencedColor = Vector3(all_zero());
				visQty = Vector3(all_zero());
				return true;
			}
		}
	}

	clamp(influencedColor);

	return false;
}

void Scene::diffuse(const Ray &r, Color &out) const
{
	Vector3 normDir = normalize(r.direction());
	const Light **light = (const Light**)rt_lights;

	Color visibility, influencedColor;
	
	// Slightly shift the origin to avoid hitting the same object
	const Vector3 p = r.origin + r.direction() * 0.000000001f;

	Ray ray(r);
	ray.origin = p;

	while (*light)
	{
		// Check if the current light is occluded
		Vector3 L2P = (*light)->pos - p;
		const float t = length(L2P);
		L2P /= t;
		const float L2PdotN = dot(L2P, normDir);
		
		if (L2PdotN < 0)
		{
			++light;
			continue;
		}

		ray.setDirection(L2P);
		bool lightOccluded = collide(ray, t, visibility, influencedColor);

		if (!lightOccluded)
		{
			if (minElem(visibility) > 0.999f)
				out += (*light)->color * L2PdotN;
			else
				out += mulPerElem((*light)->color, mulPerElem(visibility, influencedColor)) * L2PdotN;
		}

		++light;
	}

	clamp(out);
}

void Scene::specular(const Ray &r, const Vector3 &viewDir, float roughness, Color &out) const
{
	const Vector3 normDir = normalize(r.direction());
	const Vector3 normVDir = normalize(-viewDir);

	const Light **light = (const Light**)rt_lights;

	Color visibility, influencedColor;
	
	// Slightly shift the origin to avoid hitting the same object
	const Vector3 p = r.origin + r.direction() * 0.00000001f;

	Ray ray(r);
	ray.origin = p;

	while (*light)
	{
		// Check if the current light is occluded
		Vector3 L2P = (*light)->pos - p;

		float t = length(L2P);
		L2P /= t;
		ray.setDirection(L2P);
		bool lightOccluded = collide(ray, t, visibility, influencedColor);

		if (!lightOccluded)
		{
			const Vector3 H = normalize(L2P + normVDir);
			const float NdH = dot(normDir, H);
			if (minElem(visibility) > 0.999f)
				out += (*light)->color * static_cast<float>(pow(max(0, NdH), 1/roughness));
			else
				out += mulPerElem((*light)->color, mulPerElem(visibility, influencedColor)) * static_cast<float>(pow(max(0, NdH), 1/roughness));
		}

		++light;
	}

	clamp(out);
}
