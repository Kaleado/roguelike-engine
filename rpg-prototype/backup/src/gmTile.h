#ifndef GMTILE_H
#define GMTILE_H

#include "gmRender.h"

class gmTile
{
public:
	SDL_Surface* mask;
	SDL_Surface* img = NULL;
	std::string imgPath;
	bool passable;
	bool opaque;

	void show(int x, int y)
	{
		apply((x - camX) * 30, (y - camY) * 30, img, screen);
	}

	void showTransparent(int x, int y)
	{
		apply((x - camX) * 30, (y - camY) * 30, img, screen);
		apply((x - camX) * 30, (y - camY) * 30, mask, screen);
	}

	bool load()
	{
		this->img = IMG_Load(imgPath.c_str());
		if (img != NULL)
		{
			return true;
		}
		return false;
	}

	gmTile::gmTile(
		std::string imgPath,
		bool passable,
		bool opaque)
	{
		mask = IMG_Load("res/mask.png");
		SDL_SetSurfaceAlphaMod(mask, 125);
		this->imgPath = imgPath;
		this->passable = passable;
		this->opaque = opaque;
	}
};

#endif