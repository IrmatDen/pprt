#ifndef PPRT_TRACEBLOCK_H
#define PPRT_TRACEBLOCK_H

#include "framebuffer.h"
#include "scene.h"

#include <tbb/blocked_range2d.h>

template <typename PixelStoreT = RGBAStore>
class TraceBlock
{
public:
	typedef PixelStoreT	pixel_store_t;

public:
	TraceBlock(Scene &scene, pixel_store_t &out, Framebuffer<pixel_store_t> *renderWnd = nullptr);

	void operator()(const tbb::blocked_range2d<int> &r) const;

private:
	Scene						*	scn;
	pixel_store_t				*	img;
	Framebuffer<pixel_store_t>	*	fb;
};

template <typename PixelStoreT>
TraceBlock<PixelStoreT>::TraceBlock(Scene &scene, pixel_store_t &out, Framebuffer<pixel_store_t> *renderWnd)
	:scn(&scene), img(&out), fb(renderWnd)
{
}

template <typename PixelStoreT>
void TraceBlock<PixelStoreT>::operator()(const tbb::blocked_range2d<int> &r) const
{
	Ray ray;

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
					scn->camera().project(fragx, fragy, ray);

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


#endif
