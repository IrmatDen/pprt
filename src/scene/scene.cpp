#include <limits>
#include <algorithm>
#include <set>

#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>

#include <FreeImagePlus.h>

#include <tbb/task_scheduler_init.h>
#include <tbb/blocked_range2d.h>
#include <tbb/parallel_for.h>
#include <tbb/mutex.h>
#include <tbb/atomic.h>

#include "scene.h"
#include "bvh.h"

#include "../crtscn_parser/scnparser.h"

#include "../common.h"

#include <SFML/Graphics.hpp>

#define THREADING 1

struct Rect
{
	int left,	top;
	int width,	height;
};

template <typename PixelType = unsigned char, int PixelSize = 4>
class PixelStore
{
public:
	typedef PixelType pixel_t;

	static const int pixel_size = PixelSize;

public:
	PixelStore(int w, int h)
		: width(w), height(h), rowSize(width * PixelSize), imgData(nullptr)
	{
		imgData = new pixel_t[width * height * PixelSize];
	}

	~PixelStore()
	{
		delete [] imgData;
	}

	int getWidth() const		{ return width; }
	int getHeight() const		{ return height; }
	static int getPixelSize()	{ return PixelSize; }

	pixel_t* getScanline(int y)
	{
		assert(y < height);
		return imgData + y * rowSize;
	}

	pixel_t* getPixels()
	{
		return imgData;
	}

private:
	const int	width;
	const int	height;
	const int	rowSize;
	pixel_t		*imgData;
};
typedef PixelStore<> RGBAStore;

template <typename PixelStoreT = RGBAStore>
class Framebuffer
{
public:
	typedef PixelStoreT	pixel_store_t;

public:
	Framebuffer()
		: win(nullptr), pixelStore(nullptr)
	{
	}

	~Framebuffer()
	{
		delete win;
	}

	void create(pixel_store_t &pixStore, const std::string &displayName)
	{
		pixelStore = &pixStore;
		win = new sf::RenderWindow(sf::VideoMode(pixelStore->getWidth(), pixelStore->getHeight()), displayName);
		displayImg.Create(pixelStore->getWidth(), pixelStore->getHeight(), sf::Color(0, 0, 0, 0));
		displayImg.SetSmooth(false);
		displaySprite.SetImage(displayImg);
		displaySprite.FlipY(true);

		updateRequired = false;
	}

	void run()
	{
		if (win == nullptr)
			return;

		while (win->IsOpened())
		{
			if (updateRequired)
			{
				displayImg.UpdatePixels(pixelStore->getPixels());
				updateRequired = false;
			}

			sf::Event evt;
			while (win->GetEvent(evt))
			{
				switch (evt.Type)
				{
				case sf::Event::Closed:
					win->Close();
					break;
				}
			}

			win->Clear();
			win->Draw(displaySprite);
			win->Display();
		}
	}

	void tagUpdate()
	{
		updateRequired = true;
	}

private:
	sf::RenderWindow	*win;
	sf::Image			displayImg;
	sf::Sprite			displaySprite;
	pixel_store_t		*pixelStore;

	tbb::atomic<bool>	updateRequired;
};

template <typename PixelStoreT = RGBAStore>
class TraceBlock
{
public:
	typedef PixelStoreT	pixel_store_t;

public:
	TraceBlock(Scene &scene, pixel_store_t &out, Framebuffer<pixel_store_t> *renderWnd = nullptr)
		:scn(&scene), img(&out), fb(renderWnd)
	{
	}

	void operator()(const tbb::blocked_range2d<int> &r) const
	{
		Ray ray;
		ray.origin = scn->camera().pos;

		Rect blockDef;
		blockDef.top	= r.rows().begin();
		blockDef.left	= r.cols().begin();
		blockDef.height	= r.rows().size();
		blockDef.width	= r.cols().size();

		for (int y = r.rows().begin(); y != r.rows().end(); ++y)
		{
			typename pixel_store_t::pixel_t *imgData = img->getScanline(y);
			imgData += pixel_store_t::getPixelSize() * r.cols().begin();

			for (int x = r.cols().begin(); x < r.cols().end(); x++, imgData += pixel_store_t::getPixelSize())
			{
				Color outPix(all_zero());
				Color alphaPix(all_zero());
				bool hitSomething;

				for (float fragx = (float)x; fragx < x + 1.0f; fragx += 0.5f)
				{
					for (float fragy = (float)y; fragy < y + 1.0f; fragy += 0.5f)
					{
						// Bring x & y in [-1,1] range, and generate primary's ray dir.
						float fx =		fragx * 1.0f / img->getWidth() * 2 - 1;
						float fy = 1 -	fragy * 1.0f / img->getHeight() * 2;

						scn->camera().project(fx, fy, ray);

						Color alphaFrag, outFrag;
						outFrag = scn->trace(ray, hitSomething, alphaFrag);

						outPix += outFrag * 0.25f;
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
			
				imgData[0] = static_cast<pixel_store_t::pixel_t>(outPix.getX() * 255);
				imgData[1] = static_cast<pixel_store_t::pixel_t>(outPix.getY() * 255);
				imgData[2] = static_cast<pixel_store_t::pixel_t>(outPix.getZ() * 255);
				imgData[3] = static_cast<pixel_store_t::pixel_t>(maxElem(alphaPix) * 255);
			}
		}

		if (fb)
			fb->tagUpdate();
	}

private:
	Scene						*	scn;
	pixel_store_t				*	img;
	Framebuffer<pixel_store_t>	*	fb;
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
	for(Geometries::iterator it = objects.begin(); it != objects.end(); ++it)
		(*it)->prepareShader();

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
	RGBAStore imgStore(resX, resY);
	Framebuffer<RGBAStore> fb;
	sf::Thread *renderThread;
	TraceBlock<RGBAStore> *tracer;

	cam.init(resX, resY);

	if (displayType == DT_File)
	{
		tracer = new TraceBlock<RGBAStore>(*this, imgStore);
	}
	else // DT_Framebuffer
	{
		fb.create(imgStore, displayName);
		tracer = new TraceBlock<RGBAStore>(*this, imgStore, &fb);
	}

#if THREADING == 1
	renderThread = new sf::Thread(
		[&] ()
		{
			tbb::task_scheduler_init tbbInit;
			tbb::parallel_for(tbb::blocked_range2d<int>(0, resY, 0, resX), *tracer, tbb::auto_partitioner());
		} );
	renderThread->Launch();
#else
	renderThread = new sf::Thread(
		[&] ()
		{
			const int blockSide = 16;

			int y = 0;
			const int	maxY = (resY / blockSide) * blockSide,
						remY = resY - maxY;
			const int	maxX = (resX / blockSide) * blockSide,
						remX = resX - maxX;
			for (; y != maxY; y += blockSide)
			{
				int x = 0;
				for (; x < maxX; x += blockSide)
					(*tracer)(tbb::blocked_range2d<int>(y, y + blockSide, x, x + blockSide));

				if (remX > 0)
					(*tracer)(tbb::blocked_range2d<int>(y, y + blockSide, maxX, resX));
			}

			if (remY > 0)
			{
				int x = 0;
				for (; x < maxX; x += blockSide)
					(*tracer)(tbb::blocked_range2d<int>(maxY, resY, x, x + blockSide));

				if (remX > 0)
					(*tracer)(tbb::blocked_range2d<int>(maxY, resY, maxX, resX));
			}
		} );
	renderThread->Launch();
#endif
	
	if (displayType == DT_File)
	{
		renderThread->Wait();

		fipImage img(FIT_BITMAP, (WORD)resX, (WORD)resY, 32);

		for (int y = 0; y != resY; y++)
		{
			RGBAStore::pixel_t *pixelsData = imgStore.getScanline(y);
			BYTE *imgData = img.getScanLine(y);

			for (int x = 0; x != resX; x++, imgData += 4, pixelsData += imgStore.getPixelSize())
			{
				imgData[FI_RGBA_RED]	= BYTE(pixelsData[0]);
				imgData[FI_RGBA_GREEN]	= BYTE(pixelsData[1]);
				imgData[FI_RGBA_BLUE]	= BYTE(pixelsData[2]);
				imgData[FI_RGBA_ALPHA]	= BYTE(pixelsData[3]);
			}
		}

		img.save(displayName.c_str());
	}
	else
	{
		fb.run();
	}
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
	shader.setRTVarValueByIndex(CompiledShader::P, p);
	shader.setRTVarValueByIndex(CompiledShader::N, info.normal);
	shader.setRTVarValueByIndex(CompiledShader::Ng, info.normal);
	shader.setRTVarValueByIndex(CompiledShader::s, info.s);
	shader.setRTVarValueByIndex(CompiledShader::t, info.t);
	shader.setRTVarValueByIndex(CompiledShader::I, ray.direction());
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
