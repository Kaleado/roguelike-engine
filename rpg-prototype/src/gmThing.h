#ifndef GMTHING_H
#define GMTHING_H

#include "gmRender.h"

class gmAi;

class gmThing
{
public:

	gmThing* target = NULL;
	SDL_Surface* img = NULL;
	std::string imgPath;
	std::string name;
	int volume;//Volume and weight could be used for items, or even pets which can be placed in the inventory and dropped when desired.
	//int weight;
	std::vector <gmThing*> inventory;
	int x, y;
	int str, dex, con, itl, cha, per;
	int viewRadius;
	int strMod, dexMod, conMod, itlMod, chaMod, perMod;
	int ac, curHp, maxHp;
	int acMod, hpMod;
	bool isPassable;
	bool inanimate;
	void(*death)(gmThing* owner);
	void(*attack)(gmThing* owner, gmThing* target);
	gmAi* ai;

	void step(int dx, int dy)
	{
		this->x += dx;
		this->y += dy;
	}

	void show()
	{
		if (img){ apply((x - camX) * 30, (y - camY) * 30, img, screen); }
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

	gmThing::gmThing(
		std::string name,
		std::string imgPath,
		bool isPassable = false,
		gmAi* ai = NULL)
	{
		this->name = name;
		this->imgPath = imgPath;
		this->ai = ai;
		this->isPassable = isPassable;
	}
};

gmThing* player;

#endif