#include <limits>
#include <algorithm>

#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>

#include <FreeImagePlus.h>

#include <tbb/task_scheduler_init.h>
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>

#include "scene.h"
#include "../crtscn_parser/scnparser.h"

class TraceScanLine
{
public:
	TraceScanLine(Scene &scene, fipImage &out)
		:scn(scene), img(out)
	{
	}

	void operator()(const tbb::blocked_range<int> &r) const
	{
		Scene &scene = scn;

		Ray ray;
		ray.origin = scene.cam.pos;

		for (int y = r.begin(); y != r.end(); ++y)
		{
			BYTE *imgData = img.getScanLine(y);

			for (int x = 0; x < scene.resX; x++, imgData += 4)
			{
				Color4 col(0, 0, 0, 0);
				bool hitSomething;

				for (Real fragx = (Real)x; fragx < x + 1.0f; fragx += 0.5f)
				{
					for (Real fragy = (Real)y; fragy < y + 1.0f; fragy += 0.5f)
					{
						// Bring x & y in [-1,1] range, and generate primary's ray dir.
						Real fx =		fragx * 1.0f / scene.resX * 2 - 1;
						Real fy = 1 -	fragy * 1.0f / scene.resY * 2;

						scene.cam.project(fx, fy, ray);

						Color4 traceCol = scene.trace(ray, hitSomething);
						if (hitSomething)
							col += traceCol * 0.25f;
						else
							col += scene.background * 0.25f;
					}
				}

				imgData[FI_RGBA_RED]	= BYTE(col.r * 255);
				imgData[FI_RGBA_GREEN]	= BYTE(col.g * 255);
				imgData[FI_RGBA_BLUE]	= BYTE(col.b * 255);
				imgData[FI_RGBA_ALPHA]	= BYTE(col.a * 255);
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

MaterialPtr	Scene::getMaterialByName(const std::string &name)
{
	using namespace boost::lambda;

	Materials::iterator matIt = std::find_if(materials.begin(),
											 materials.end(),
											 boost::lambda::_1 == name
											);
	if (matIt != materials.end())
		return *matIt;

	return MaterialPtr((Material*)0);
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
}

void Scene::render()
{

	fipImage img(FIT_BITMAP, (WORD)resX, (WORD)resY, 32);
	BYTE *imgData = img.accessPixels();

	cam.init(resX, resY);

	tbb::task_scheduler_init tbbInit;
	tbb::parallel_for(tbb::blocked_range<int>(0, resY), TraceScanLine(*this, img), tbb::auto_partitioner());
	/*Ray r;
	r.origin = cam.pos;
	for (int y = 0; y < resY; y++)
	{
		for (int x = 0; x < resX; x++, imgData += 4)
		{
			Color4 col(0, 0, 0, 0);
			bool hitSomething;

			for (float fragx = (float)x; fragx < x + 1.0f; fragx += 0.5f)
			{
				for (float fragy = (float)y; fragy < y + 1.0f; fragy += 0.5f)
				{
					// Bring x & y in [-1,1] range, and generate primary's ray dir.
					float fx =		fragx * 1.0f / resX * 2 - 1;
					float fy = 1 -	fragy * 1.0f / resY * 2;

					cam.project(fx, fy, r);

					Color4 traceCol = trace(r, hitSomething);
					if (hitSomething)
						col += traceCol * 0.25f;
					else
						col += background * 0.25f;
				}
			}

			imgData[FI_RGBA_RED]	= BYTE(col.r * 255);
			imgData[FI_RGBA_GREEN]	= BYTE(col.g * 255);
			imgData[FI_RGBA_BLUE]	= BYTE(col.b * 255);
			imgData[FI_RGBA_ALPHA]	= BYTE(col.a * 255);
		}
	}*/

	img.flipVertical();
	img.save(outName.c_str());
}

Color4 Scene::trace(const Ray &eye, bool &hitSomething)
{
	Color4 out(0, 0, 0, 0);
	if (eye.traceDepth == 4)
		return out;

	Ray ray(eye);
	ray.traceDepth++;
	Real t = 20000;
	const Geometry *nearestObj(0);
	const Geometry **obj = (const Geometry**)rt_objects;

	while (*obj)
	{
		if ((*obj)->hit(ray, t))
			nearestObj = *obj;
		++obj;
	}

	if (!nearestObj)
	{
		hitSomething = false;
		return out;
	}

	hitSomething = true;

	// Object doesn't have shader, make it appear even for blind people!
	if (!nearestObj->hasShader())
		return Color4(1, 0, 1, 1);

	Vec3 p = ray.origin + ray.dir * t;
	Vec3 n;
	nearestObj->normalAt(p, n);

	CompiledShader shader(nearestObj->getShader(), true);
	shader.setCurrentDepth(ray.traceDepth);
	shader.setVarValueByIndex(CompiledShader::P, p);
	shader.setVarValueByIndex(CompiledShader::N, n);
	shader.setVarValueByIndex(CompiledShader::Ng, n);
	shader.setVarValueByIndex(CompiledShader::I, ray.dir);
	shader.exec(out);

	/*const float exposure = -0.66f;
	out.r = 1 - expf(out.r * exposure);
	out.g = 1 - expf(out.g * exposure);
	out.b = 1 - expf(out.b * exposure);

	const float invGamma = 0.45f;
	out.r = powf(out.r, invGamma);
	out.g = powf(out.g, invGamma);
	out.b = powf(out.b, invGamma);*/

	return out.clamp();
}

bool Scene::collide(const Ray &r, Real &t) const
{
	const Geometry **obj = (const Geometry**)rt_objects;

	while (*obj)
	{
		if ((*obj)->hit(r, t))
			return true;
		++obj;
	}

	return false;
}

void Scene::diffuse(const Ray &r, Color4 &out) const
{
	Vec3 normDir = r.dir.normalized();

	const Light **light = (const Light**)rt_lights;
	while (*light)
	{
		// Slightly shift the origin to avoid hitting the same object
		Vec3 p = r.origin + r.dir * Epsilon;

		// Check if the current light is occluded
		Vec3 L2P = (*light)->pos - p;
		Real t = L2P.length();
		Ray ray(p, L2P.normalize());
		bool lightOccluded = collide(ray, t);

		if (!lightOccluded)
		{
			out += (*light)->color * (float)L2P.dot(normDir);
		}

		++light;
	}
}

void Scene::specular(const Ray &r, const Vec3 &viewDir, Real roughness, Color4 &out) const
{
	Vec3 normDir = r.dir.normalized();
	Vec3 normVDir = -viewDir.normalized();

	const Light **light = (const Light**)rt_lights;
	while (*light)
	{
		// Slightly shift the origin to avoid hitting the same object
		Vec3 p = r.origin + r.dir * Epsilon;

		// Check if the current light is occluded
		Vec3 L2P = (*light)->pos - p;
		Real t = L2P.length();
		Ray ray(p, L2P.normalize());
		bool lightOccluded = collide(ray, t);

		if (!lightOccluded)
		{
			Vec3 H = (L2P + normVDir).normalize();
			out += (*light)->color * static_cast<float>(pow(max(0, normDir.dot(H)), 1/roughness));
		}

		++light;
	}
}
