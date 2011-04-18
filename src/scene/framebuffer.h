#ifndef PPRT_FRAMEBUFFER_H
#define PPRT_FRAMEBUFFER_H

#include "pixel_store.h"

#include <tbb/atomic.h>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Sprite.hpp>

template <typename PixelStoreT = RGBAStore>
class Framebuffer
{
public:
	typedef PixelStoreT	pixel_store_t;

public:
	Framebuffer();
	~Framebuffer();

	void create(pixel_store_t &pixStore, const std::string &displayName);
	void run();
	void tagUpdate();

private:
	sf::RenderWindow	*win;
	sf::Image			displayImg;
	sf::Sprite			displaySprite;
	pixel_store_t		*pixelStore;

	tbb::atomic<bool>	updateRequired;
};

template <typename PixelStoreT>
Framebuffer<PixelStoreT>::Framebuffer()
	: win(nullptr), pixelStore(nullptr)
{
}

template <typename PixelStoreT>
Framebuffer<PixelStoreT>::~Framebuffer()
{
	delete win;
}

template <typename PixelStoreT>
void Framebuffer<PixelStoreT>::create(pixel_store_t &pixStore, const std::string &displayName)
{
	pixelStore = &pixStore;
	win = new sf::RenderWindow(sf::VideoMode(pixelStore->getWidth(), pixelStore->getHeight()), displayName);
	// Try not to kill the render times...
	win->SetFramerateLimit(5);

	displayImg.Create(pixelStore->getWidth(), pixelStore->getHeight(), sf::Color(0, 0, 0, 0));
	displayImg.SetSmooth(false);
	displaySprite.SetImage(displayImg);
	displaySprite.FlipY(true);

	updateRequired = false;
}

template <typename PixelStoreT>
void Framebuffer<PixelStoreT>::run()
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

template <typename PixelStoreT>
void Framebuffer<PixelStoreT>::tagUpdate()
{
	updateRequired = true;
}

#endif
