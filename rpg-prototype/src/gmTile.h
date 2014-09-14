#ifndef GMTILE_H
#define GMTILE_H

#include "gmRender.h"

class gmThing;
class gmMap;

class gmTile
{
public:
	//Make this static somehow.
	SDL_Surface* mask;//The semi-transparent overlay placed over tiles not within view range. Couldn't find a better way to do this.
	SDL_Surface* img = NULL;//The tile's sprite.
	std::string imgPath;//Path to the tile's sprite.
	gmMap* enterTo;//The map to go to when pressing '>'.
	int entryX, entryY;//The coordinates for above.
	gmMap* exitTo;//The map to go to when pressing '<'.
	int exitX, exitY;//The coordinates for above.
	bool passable;//Whether the tile can be walked over.
	bool opaque;//Whether the tile can be seen through.
	void(*onStep)(gmThing* target);//Function to run when a creature attempts to step on to the tile.
	void show(int x, int y)//Shows the tile on screen.
	{
		apply((x - camX) * 30, (y - camY) * 30, img, screen);
	}

	void showTransparent(int x, int y)//Shows the tile on the screen, with the mask overlaying, for when it is not in view.
	{
		apply((x - camX) * 30, (y - camY) * 30, img, screen);
		apply((x - camX) * 30, (y - camY) * 30, mask, screen);
	}

	bool load()//Load the tile's sprite.
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
		bool opaque,
		gmMap* enterTo=NULL,
		int entryX=0, int entryY=0,
		gmMap* exitTo=NULL,
		int exitX=0, int exitY=0)
	{
		if (!mask)
		{ 
			mask = IMG_Load("res/mask.png"); 
			SDL_SetSurfaceAlphaMod(mask, 200);
		}
		this->imgPath = imgPath;
		this->passable = passable;
		this->opaque = opaque;
		this->enterTo = enterTo;
		this->exitTo = exitTo;
		this->entryX = entryX; this->entryY = entryY;
		this->exitX = exitX; this->exitY = exitY;
	}
};

#endif