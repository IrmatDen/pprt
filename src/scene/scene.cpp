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
				// Bring x & y in [-1,1] range, and generate primary's ray dir.
				float fx = x * 1.0f / scene.resX * 2 - 1;
				float fy = 1 - y * 1.0f / scene.resY * 2;

				scene.cam.project(fx, fy, ray);

				Color4 col = scene.trace(ray, true);

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
											 _1 == name
											);
	if (matIt != materials.end())
		return *matIt;

	return MaterialPtr((Material*)0);
}

Camera& Scene::camera()
{
	return cam;
}

void Scene::render()
{

	fipImage img(FIT_BITMAP, (WORD)resX, (WORD)resY, 32);
	BYTE *imgData = img.accessPixels();

	cam.init();

	tbb::task_scheduler_init tbbInit;
	tbb::parallel_for(tbb::blocked_range<int>(0, resY), TraceScanLine(*this, img), tbb::auto_partitioner());
	/*Ray r;
	r.origin = cam.pos;
	for (int y = 0; y < resY; y++)
	{
		for (int x = 0; x < resX; x++, imgData += 4)
		{
			// Bring x & y in [-1,1] range, and generate primary's ray dir.
			float fx = x * 1.0f / resX * 2 - 1;
			float fy = 1 - y * 1.0f / resY * 2;

			cam.project(fx, fy, r);

			Color4 col = trace(r, 10, true);

			imgData[FI_RGBA_RED]	= BYTE(col.r * 255);
			imgData[FI_RGBA_GREEN]	= BYTE(col.g * 255);
			imgData[FI_RGBA_BLUE]	= BYTE(col.b * 255);
			imgData[FI_RGBA_ALPHA]	= BYTE(col.a * 255);
		}
	}*/

	img.flipVertical();
	img.save(outName.c_str());
}

Color4 Scene::trace(const Ray &eye, bool returnBackground)
{
	Color4 out(0, 0, 0, 0);
	Ray ray = eye;
	Real t = 20000;
	GeometryPtr nearestObj((Geometry*)0);

	for(Geometries::iterator it = objects.begin(); it != objects.end(); ++it)
	{
		if ((*it)->hit(ray, t))
			nearestObj = *it;
	}

	if (!nearestObj)
	{
		if (returnBackground)
			return background;
		return out;
	}

	// Object doesn't have shader, make it appear even for blind people!
	if (!nearestObj->hasShader())
		return Color4(1, 0, 1, 1);

	Vec3 p = ray.origin + ray.dir * t;
	Vec3 n;
	nearestObj->normalAt(p, n);

	nearestObj->shade(out);

	return out.clamp();

		/*
		for(Lights::const_iterator it = lights.begin(); it != lights.end(); ++it)
		{
			const Light &light = **it;
			// Slightly shift the origin to avoid hitting the same object
			p += n * (Real)0.000012;

			// Check if the current light is occluded
			Vec3 L2P = light.pos - p;
			t = L2P.length();
			Ray r(p, L2P.normalize());
			bool lightOccluded = collide(r, t);

			if (!lightOccluded)
				out += (*nearestObj->material()).shade(p, n, light) * reflCoef;
		}*/
		//reflCoef  *= (*nearestObj->material()).reflexivity;
		/*ray.origin = p;
		ray.dir = ray.dir - 2 * ray.dir.dot(n) * n;*/
}

bool Scene::collide(const Ray &r, Real &t)
{
	for(Geometries::iterator it = objects.begin(); it != objects.end(); ++it)
	{
		if ((*it)->hit(r, t))
			return true;
	}

	return false;
}
