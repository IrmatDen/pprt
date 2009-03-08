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

				for (Real fragx = (Real)x; fragx < x + 1.0f; fragx += 0.5f)
				{
					for (Real fragy = (Real)y; fragy < y + 1.0f; fragy += 0.5f)
					{
						// Bring x & y in [-1,1] range, and generate primary's ray dir.
						Real fx =		fragx * 1.0f / scene.resX * 2 - 1;
						Real fy = 1 -	fragy * 1.0f / scene.resY * 2;

						scene.cam.project(fx, fy, ray);

						Color traceCol = scene.trace(ray, hitSomething);
						if (hitSomething)
							col += traceCol * 0.25f;
						else
							col += scene.background * 0.25f;
					}
				}

				const float exposure = -0.66f;
				col.r = 1 - expf(col.r * exposure);
				col.g = 1 - expf(col.g * exposure);
				col.b = 1 - expf(col.b * exposure);

				const float invGamma = 0.45f;
				col.r = powf(col.r, invGamma);
				col.g = powf(col.g, invGamma);
				col.b = powf(col.b, invGamma);

				//col.clamp();

				imgData[FI_RGBA_RED]	= BYTE(col.r * 255);
				imgData[FI_RGBA_GREEN]	= BYTE(col.g * 255);
				imgData[FI_RGBA_BLUE]	= BYTE(col.b * 255);
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
	delete [] rt_objects;
	delete [] rt_lights;
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
	rt_objects = new Geometry*[objects.size() + 1];
	int loop = 0;
	for(Geometries::iterator it = objects.begin(); it != objects.end(); ++it, ++loop)
		rt_objects[loop] = (*it).get();
	rt_objects[loop] = 0;

	rt_lights = new Light*[lights.size() + 1];
	loop = 0;
	for(Lights::iterator it = lights.begin(); it != lights.end(); ++it, ++loop)
		rt_lights[loop] = (*it).get();
	rt_lights[loop] = 0;
	
	bvhRoot = new BVH();
	bvhRoot->build(objects);
}

void Scene::render()
{

	fipImage img(FIT_BITMAP, (WORD)resX, (WORD)resY, 32);
	BYTE *imgData = img.accessPixels();

	cam.init(resX, resY);

	tbb::task_scheduler_init tbbInit;
	tbb::parallel_for(tbb::blocked_range2d<int>(0, resY, 0, resX), TraceScanLine(*this, img), tbb::auto_partitioner());
	//tbb::parallel_for(tbb::blocked_range<int>(0, resY), TraceScanLine(*this, img), tbb::auto_partitioner());
	/*Ray r;
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

			col.clamp();

			imgData[FI_RGBA_RED]	= BYTE(col.r * 255);
			imgData[FI_RGBA_GREEN]	= BYTE(col.g * 255);
			imgData[FI_RGBA_BLUE]	= BYTE(col.b * 255);
			imgData[FI_RGBA_ALPHA]	= 255;
		}
	}*/

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
	/*const Geometry *nearestObj(0);
	const Geometry **obj = (const Geometry**)rt_objects;

	Real t = 20000;
	while (*obj)
	{
		if ((*obj)->hit(ray, t))
			nearestObj = *obj;
		++obj;
	}*/

	Real t = 20000;
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

	Vec3 p = ray.origin + ray.direction() * t;
	Vec3 n;
	nearestObj->normalAt(p, n);

	CompiledShader shader(nearestObj->getShader(), true);
	shader.setCurrentDepth(ray.traceDepth);
	shader.setVarValueByIndex(CompiledShader::P, p);
	shader.setVarValueByIndex(CompiledShader::N, n);
	shader.setVarValueByIndex(CompiledShader::Ng, n);
	shader.setVarValueByIndex(CompiledShader::I, ray.direction());
	shader.exec();

	Color Ci, Oi;
	shader.getOutput(Ci, Oi);

	if (isOpaque(Oi))
		return Ci;

	ray.origin += ray.direction() * (t + Epsilon);
	return Ci + (1 - Oi) * traceNoDepthMod(ray, dummy);
}

bool Scene::collide(const Ray &r, Real t, Color &visQty) const
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
	Vec3 n, p;

	visQty.r = visQty.g = visQty.b = 1;

	while (*obj)
	{
		if ((*obj)->hit(r, t))
		{
			p = ray.origin + ray.direction() * t;
			(*obj)->normalAt(p, n);

			CompiledShader shader((*obj)->getShader(), true);
			shader.setCurrentDepth(ray.traceDepth);
			shader.setVarValueByIndex(CompiledShader::P, p);
			shader.setVarValueByIndex(CompiledShader::N, n);
			shader.setVarValueByIndex(CompiledShader::Ng, n);
			shader.setVarValueByIndex(CompiledShader::I, ray.direction());
			shader.exec();

			shader.getOutput(Ci, Oi);

			if (isOpaque(Oi))
			{
				visQty.r = visQty.g = visQty.b = 0;
				return true;
			}
			else
			{
				visQty.r = visQty.r > Oi.r ? Oi.r : visQty.r;
				visQty.g = visQty.g > Oi.g ? Oi.g : visQty.g;
				visQty.b = visQty.b > Oi.b ? Oi.b : visQty.b;

				visQty.r = visQty.r <= 0.01f ? 0 : visQty.r;
				visQty.g = visQty.g <= 0.01f ? 0 : visQty.g;
				visQty.b = visQty.b <= 0.01f ? 0 : visQty.b;

				if (visQty.r <= 0.01f && visQty.g <= 0.01f && visQty.b <= 0.01f)
				{
					visQty.r = visQty.g = visQty.b = 0;
					return true;
				}
			}
		}

		++obj;
	}

	return false;
}

void Scene::diffuse(const Ray &r, Color &out) const
{
	Vec3 normDir = r.direction().normalized();
	const Light **light = (const Light**)rt_lights;

	Color visibility;
	
	// Slightly shift the origin to avoid hitting the same object
	const Vec3 p = r.origin + r.direction() * Epsilon;

	Ray ray(r);
	ray.origin = p;

	while (*light)
	{
		// Check if the current light is occluded
		Vec3 L2P = (*light)->pos - p;
		const Real t = L2P.length();
		L2P.normalize();
		const Real L2PdotN = L2P.dot(normDir);
		
		if (L2PdotN < 0)
		{
			++light;
			continue;
		}

		ray.setDirection(L2P);
		bool lightOccluded = collide(ray, t, visibility);

		if (!lightOccluded)
		{
			out += (*light)->color * (float)L2PdotN * visibility;
		}

		++light;
	}
}

void Scene::specular(const Ray &r, const Vec3 &viewDir, Real roughness, Color &out) const
{
	const Vec3 normDir = r.direction().normalized();
	const Vec3 normVDir = -viewDir.normalized();

	const Light **light = (const Light**)rt_lights;

	Color visibility;
	
	// Slightly shift the origin to avoid hitting the same object
	const Vec3 p = r.origin + r.direction() * Epsilon;

	Ray ray(r);
	ray.origin = p;

	while (*light)
	{
		// Check if the current light is occluded
		Vec3 L2P = (*light)->pos - p;

		Real t = L2P.length();
		ray.setDirection(L2P.normalize());
		bool lightOccluded = collide(ray, t, visibility);

		if (!lightOccluded)
		{
			const Vec3 H = (L2P + normVDir).normalize();
			const Real NdH = normDir.dot(H);
			out += (*light)->color * static_cast<float>(pow(max(0, NdH), 1/roughness)) * visibility;
		}

		++light;
	}
	out.clamp();
}
