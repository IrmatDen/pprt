#include <limits>

#include <FreeImagePlus.h>

#include <tbb/task_scheduler_init.h>
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>

#include "scene.h"

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

				Color4 col = scene.trace(ray, 3, true);

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

Camera&	  Scene::camera()
{
	return cam;
}

void Scene::render(const std::string &filename)
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
	img.save(filename.c_str());
}

Color4 Scene::trace(Ray r, int maxRecurse, bool returnBackground)
{
	float reflCoef = 1;
	int currRecurse = 0;
	Color4 out;
	bool hitSomething = false;

	while (currRecurse < maxRecurse && reflCoef > 0)
	{
		float t = 20000;
		GeometryPtr nearestObj((Geometry*)0);

		for(Geometries::iterator it = objects.begin(); it != objects.end(); ++it)
		{
			if ((*it)->hit(r, t))
				nearestObj = *it;
		}

		if (!nearestObj)
			break;

		hitSomething = true;

		Vec3f p = r.origin + r.dir * t;
		Vec3f n;

		nearestObj->normalAt(p, n);
		for(Lights::const_iterator it = lights.begin(); it != lights.end(); ++it)
		{
			const Light &light = **it;
			// Slightly shift the origin to avoid hitting the same object
			p += n * 0.001f;
			Vec3f L2P = light.pos - p;
			t = L2P.length();
			Ray r(p, L2P.normalize());
			if (!collide(r, t))
				out += nearestObj->material().shade(p, n, light) * reflCoef;
			else
				out += nearestObj->material().ambient * reflCoef;
		}

		currRecurse++;
		reflCoef  *= nearestObj->material().reflexivity;
		r.origin = p;
		r.dir = r.dir - 2 * r.dir.dot(n) * n;
	}

	if (!hitSomething && returnBackground)
		out = background;

	return out.clamp();
}

bool Scene::collide(const Ray &r, float &t)
{
	for(Geometries::iterator it = objects.begin(); it != objects.end(); ++it)
	{
		if ((*it)->hit(r, t))
			return true;
	}

	return false;
}
