#ifndef PPRT_PIXEL_STORE_H
#define PPRT_PIXEL_STORE_H

template <typename PixelType = unsigned char, int PixelSize = 4>
class PixelStore
{
public:
	typedef PixelType pixel_t;

	static const int pixel_size = PixelSize;

public:
	PixelStore(int w, int h);
	~PixelStore();

	int getWidth() const		{ return width; }
	int getHeight() const		{ return height; }
	static int getPixelSize()	{ return PixelSize; }

	pixel_t* getScanline(int y);
	pixel_t* getPixels();

private:
	const int	width;
	const int	height;
	const int	rowSize;
	pixel_t		*imgData;
};
typedef PixelStore<> RGBAStore;

template <typename PixelType, int PixelSize>
PixelStore<PixelType, PixelSize>::PixelStore(int w, int h)
	: width(w), height(h), rowSize(width * PixelSize), imgData(nullptr)
{
	imgData = new pixel_t[width * height * PixelSize];
}

template <typename PixelType, int PixelSize>
PixelStore<PixelType, PixelSize>::~PixelStore()
{
	delete [] imgData;
}

template <typename PixelType, int PixelSize>
typename PixelStore<PixelType, PixelSize>::pixel_t*
	PixelStore<PixelType, PixelSize>::getScanline(int y)
{
	assert(y < height);
	return imgData + y * rowSize;
}

template <typename PixelType, int PixelSize>
typename PixelStore<PixelType, PixelSize>::pixel_t*
	PixelStore<PixelType, PixelSize>::getPixels()
{
	return imgData;
}


#endif
