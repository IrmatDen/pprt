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

//#define NOTHREADING

class TraceScanLine
{
public:
	TraceScanLine(Scene &scene, fipImage &out)
		:scn(scene), img(out)
	{
	}

	void operator()(const tbb::blocked_range2d<int> &r) const
	{
		Scene &scene = scn;

		Ray ray;
		ray.origin = scene.cam.pos;

		for (int y = r.rows().begin(); y != r.rows().end(); ++y)
		{
			BYTE *imgData = img.getScanLine(y);
			imgData += 4 * r.cols().begin();

			for (int x = r.cols().begin(); x < r.cols().end(); x++, imgData += 4)
			{
				Color outPix(all_zero());
				Color alphaPix(all_zero());
				bool hitSomething;

				for (float fragx = (float)x; fragx < x + 1.0f; fragx += 0.5f)
				{
					for (float fragy = (float)y; fragy < y + 1.0f; fragy += 0.5f)
					{
						// Bring x & y in [-1,1] range, and generate primary's ray dir.
						float fx =		fragx * 1.0f / scene.resX * 2 - 1;
						float fy = 1 -	fragy * 1.0f / scene.resY * 2;

						scene.cam.project(fx, fy, ray);

						Color alphaFrag, outFrag;
						outFrag = scene.trace(ray, hitSomething, alphaFrag);
						if (hitSomething)
							outPix += outFrag * 0.25f;
						else
							outPix += scene.background * 0.25f;
						alphaPix += alphaFrag * 0.25f;
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

				clamp(outPix);
				clamp(alphaPix);
			
				const float	r = outPix.getX() * 255,
							g = outPix.getY() * 255,
							b = outPix.getZ() * 255,
							a = maxElem(alphaPix) * 255;
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
#else
	Ray r;
	r.origin = cam.pos;
	for (int y = 0; y < resY; y++)
	{
		for (int x = 0; x < resX; x++, imgData += 4)
		{
			Color outPix(all_zero());
			Color alphaPix(all_zero());
			bool hitSomething;

			for (float fragx = (float)x; fragx < x + 1.0f; fragx += 0.5f)
			{
				for (float fragy = (float)y; fragy < y + 1.0f; fragy += 0.5f)
				{
					// Bring x & y in [-1,1] range, and generate primary's ray dir.
					float fx =		fragx * 1.0f / resX * 2 - 1;
					float fy = 1 -	fragy * 1.0f / resY * 2;

					cam.project(fx, fy, r);

					Color alphaFrag, outFrag;
					outFrag = trace(r, hitSomething, alphaFrag);
					if (hitSomething)
						outPix += outFrag * 0.25f;
					else
						outPix += background * 0.25f;
					alphaPix += alphaFrag * 0.25f;
				}
			}

			clamp(outPix);
			clamp(alphaPix);
			
			const float	r = outPix.getX() * 255,
						g = outPix.getY() * 255,
						b = outPix.getZ() * 255,
						a = maxElem(alphaPix) * 255;
			imgData[FI_RGBA_RED]	= BYTE(r);
			imgData[FI_RGBA_GREEN]	= BYTE(g);
			imgData[FI_RGBA_BLUE]	= BYTE(b);
			imgData[FI_RGBA_ALPHA]	= BYTE(a);
		}
	}
#endif

	img.save(outName.c_str());
}

Color Scene::trace(const Ray &eye, bool &hitSomething, Color &Oi) const
{
	if (eye.traceDepth == 16)
	{
		return Color(all_zero());
	}

	Ray ray(eye);
	Oi = Color(all_zero());
	return traceNoDepthMod(ray, hitSomething, Oi);
}

Color Scene::traceNoDepthMod(Ray &ray, bool &hitSomething, Color &Oi) const
{
	if (ray.traceDepth == 8)
	{
		hitSomething = false;
		return Color(all_zero());
	}
	ray.traceDepth++;

	float t = 20000;
	const Geometry *nearestObj = bvhRoot->findClosest(ray, t);

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

	Color Ci, thisOi;
	shader.getOutput(Ci, thisOi);
	Oi += thisOi;

	if (isOpaque(Oi))
		return Ci;

	ray.origin += ray.direction() * (t + 0.001f);

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

	
	// Slightly shift the origin to avoid hitting the same object
	const Vector3 p = r.origin + r.direction() * 0.0001f;

	Ray ray(r);
	ray.origin = p;

	while (*light)
	{
		// Check if the current light is occluded
		Vector3 L2P = (*light)->pos - p;
		const float t = length(L2P);
		L2P /= t;
		const float L2PdotN = dot(L2P, dir);
		
		if (L2PdotN < 0)
		{
			++light;
			continue;
		}

		ray.setDirection(L2P);
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

	Color visibility, influencedColor;
	
	// Slightly shift the origin to avoid hitting the same object
	const Vector3 p = r.origin + dir * 0.0001f;

	Ray ray(r);
	ray.origin = p;

	while (*light)
	{
		// Check if the current light is occluded
		Vector3 L2P = (*light)->pos - p;

		Vectormath::floatInVec t(_mm_sqrt_ps(_vmathVfDot3( L2P.get128(), L2P.get128() )));

		//Vectormath::floatInVec t = length(L2P);
		L2P /= t;
		const float L2PdotN = dot(L2P, dir);
		
		if (L2PdotN < 0)
		{
			++light;
			continue;
		}

		ray.setDirection(L2P);

		bool hit;
		Color opacity(all_zero());
		Color influencedColor = trace(ray, hit, opacity);
		Color visibility = (Color(sse::all_one) - opacity);

		if (!hit || minElem(visibility) > 0.001f)
		{
			const Vector3 H = normalize(L2P + viewDir);
			const float NdH = dot(dir, H);
			const float specMult = static_cast<float>(pow(max(0, NdH), 1/roughness));

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
