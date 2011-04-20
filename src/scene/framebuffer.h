#ifndef PPRT_FRAMEBUFFER_H
#define PPRT_FRAMEBUFFER_H

#include "pixel_store.h"
#include "scene.h"

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
	Framebuffer(Scene &scn);
	~Framebuffer();

	void create(pixel_store_t &pixStore, const std::string &displayName);
	void run();
	void tagUpdate();

	void allowSceneReload(bool allow);

private:
	sf::RenderWindow	*win;
	sf::Image			displayImg;
	sf::Sprite			displaySprite;
	pixel_store_t		*pixelStore;

	Scene	&scene;
	bool	allowReload;
	
	bool	isCreated;
	bool	isRunning;

	tbb::atomic<bool>	updateRequired;
};

template <typename PixelStoreT>
Framebuffer<PixelStoreT>::Framebuffer(Scene &scn)
	: win(nullptr), pixelStore(nullptr), scene(scn), allowReload(false),
	isCreated(false), isRunning(false)
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

	if (isCreated)
		return;

	win = new sf::RenderWindow(sf::VideoMode(pixelStore->getWidth(), pixelStore->getHeight()), displayName);
	// Try not to kill the render times...
	win->SetFramerateLimit(5);

	displayImg.Create(pixelStore->getWidth(), pixelStore->getHeight(), sf::Color(0, 0, 0, 0));
	displayImg.SetSmooth(false);
	displaySprite.SetImage(displayImg);

	updateRequired = false;

	isCreated = true;
}

template <typename PixelStoreT>
void Framebuffer<PixelStoreT>::run()
{
	if (isRunning || win == nullptr)
		return;

	isRunning = true;

	while (win->IsOpened())
	{
		if (updateRequired)
		{
			displayImg.UpdatePixels(pixelStore->getPixels());
			updateRequired = false;
		}

		sf::Event evt;
		while (win->PollEvent(evt))
		{
			switch (evt.Type)
			{
			case sf::Event::Closed:
				win->Close();
				break;

			case sf::Event::KeyReleased:
				switch (evt.Key.Code)
				{
				case sf::Key::F5:
					if (allowReload)
						scene.reloadSceneFile();
					break;
				}
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

template <typename PixelStoreT>
void Framebuffer<PixelStoreT>::allowSceneReload(bool allow)
{
	allowReload = allow;
}

#endif
