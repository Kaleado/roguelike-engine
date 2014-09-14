#ifndef GMLOCALE_H
#define GMLOCALE_H

#include <vector>
#include <string>
#include <SDL.h>

class gmMap;

class gmLocale
{
public:
	bool isTown;
	int x, y, dangerLevel;
	std::string name;
	SDL_Surface* entranceImg;
	std::string entranceImgPath;
	signed int goodRating;
	signed int lawRating;
	std::vector <gmMap*> mapList;
	void load()
	{
		entranceImg = IMG_Load(entranceImgPath.c_str());
		return;
	}
	void show()
	{
		apply(30 * (x - camX), 30 * (y - camY), entranceImg, screen);
		return;
	}
	gmLocale(
		std::string name,
		bool isTown,
		int dangerLevel,
		std::string entranceImgPath,
		signed int goodRating,
		signed int lawRating,
		std::vector <gmMap*> mapList
		)
	{
		this->name = name;
		this->isTown = isTown;
		this->dangerLevel = dangerLevel;
		this->entranceImgPath = entranceImgPath;
		this->goodRating = goodRating;
		this->lawRating = lawRating;
		this->mapList = mapList;
		for (int i = 0; i < mapList.size(); i++)
		{
			this->mapList[i]->make(this->mapList[i]);
		}
	}
};

#endif