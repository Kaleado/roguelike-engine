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
	void make()
	{
		std::string evilNames[6];
		std::string unalignedNames[6];
		std::string goodNames[6];

		std::string chaoticNames[6];
		std::string neutralNames[6];
		std::string lawfulNames[6];

		if (isTown == false)//For dungeons.
		{
			evilNames[0] = "Diabolical";
			evilNames[1] = "Devil's";
			evilNames[2] = "Thieves'";
			evilNames[3] = "Burning";
			evilNames[4] = "Monstrous";
			evilNames[5] = "Terrifying";
			unalignedNames[0] = "Eerie";
			unalignedNames[1] = "Foreboding";
			unalignedNames[2] = "Ominous";
			unalignedNames[3] = "Ancient";
			unalignedNames[4] = "Mysterious";
			unalignedNames[5] = "Enormous";
			goodNames[0] = "Holy";
			goodNames[1] = "Sacred";
			goodNames[2] = "Bright";
			goodNames[3] = "Glowing";
			goodNames[4] = "God's";
			goodNames[5] = "Prophet's";

			chaoticNames[0] = "Cave";
			chaoticNames[1] = "Rampart";
			chaoticNames[2] = "Hive";
			chaoticNames[3] = "Den";
			chaoticNames[4] = "Complex";
			chaoticNames[5] = "Maze";
			neutralNames[0] = "Mountain";
			neutralNames[1] = "Labyrinth";
			neutralNames[2] = "Tower";
			neutralNames[3] = "Chamber";
			neutralNames[4] = "Dungeon";
			neutralNames[5] = "Mine";
			lawfulNames[0] = "Forest";
			lawfulNames[1] = "Fortress";
			lawfulNames[2] = "Prison";
			lawfulNames[3] = "Monastery";
			lawfulNames[4] = "Church";
			lawfulNames[5] = "Cathedral";
		}
		else//For towns.
		{
			evilNames[0] = "Gloomy";
			evilNames[1] = "Sinister";
			evilNames[2] = "Dark";
			evilNames[3] = "Corrupt";
			evilNames[4] = "Unfriendly";
			evilNames[5] = "Bandit's";
			unalignedNames[0] = "Modest";
			unalignedNames[1] = "Homely";
			unalignedNames[2] = "Famous";
			unalignedNames[3] = "Merchant's";
			unalignedNames[4] = "Large";
			unalignedNames[5] = "Small";
			goodNames[0] = "Bustling";
			goodNames[1] = "Vibrant";
			goodNames[2] = "Joyous";
			goodNames[3] = "Friendly";
			goodNames[4] = "Beautiful";
			goodNames[5] = "Ornate";

			chaoticNames[0] = "Camp";
			chaoticNames[1] = "Hideout";
			chaoticNames[2] = "Outpost";
			chaoticNames[3] = "Township";
			chaoticNames[4] = "Settlement";
			chaoticNames[5] = "Colony";
			neutralNames[0] = "Town";
			neutralNames[1] = "Hamlet";
			neutralNames[2] = "Village";
			neutralNames[3] = "Metropolis";
			neutralNames[4] = "Municipality";
			neutralNames[5] = "Underground City";
			lawfulNames[0] = "City";
			lawfulNames[1] = "Fort";
			lawfulNames[2] = "Castle";
			lawfulNames[3] = "Base";
			lawfulNames[4] = "Capitol";
			lawfulNames[5] = "Encampment";

		}
		if (goodRating < -20)//If the locale is evil.
		{
			this->name = evilNames[rand() % 5];
		}
		else if (goodRating >= -20 && goodRating <= 20)
		{
			this->name = unalignedNames[rand() % 5];
		}
		else if (goodRating > 20)
		{
			this->name = goodNames[rand() % 5];
		}
		if (lawRating < -20)//If the locale is evil.
		{
			this->name += " "+chaoticNames[rand() % 3];
		}
		else if (lawRating >= -20 && lawRating <= 20)
		{
			this->name += " " + neutralNames[rand() % 5];
		}
		else if (lawRating > 20)
		{
			this->name += " " + lawfulNames[rand() % 5];
		}

		return;
	}
	void setDangerLevel()
	{
		//Danger level = the average base stat total of all the spawnable things in the dungeon.
		//This may not work well, seeing as the spawnList also includes items, which have no stats.
		//May put them into their own spawnList.
		int a = 0;
		int b = 0;
		for (int i = 0; i < mapList[0]->spawnList.size(); i++)
		{
			if (mapList[0]->spawnList[i]->ai){ a += mapList[0]->spawnList[i]->getStatTotal(); b++; }
		}
		if (mapList[0]->spawnList.size() > 0)
		{
			dangerLevel = (a / b);
		}
		else
		{
			dangerLevel = 0;
		}
		return;
	}
	gmLocale(
		std::string name,
		bool fixed,//Whether the locale is pregenerated.
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
		//this->dangerLevel = dangerLevel;
		this->entranceImgPath = entranceImgPath;
		this->goodRating = goodRating;
		this->lawRating = lawRating;
		this->mapList = mapList;
		for (int i = 0; i < mapList.size(); i++)
		{
			this->mapList[i]->make(this->mapList[i]);
		}
		if (!fixed)
		{
			this->goodRating = rand() % 100 - 50;
			this->lawRating = rand() % 100 - 50;
			this->make();
		}
		//this->setDangerLevel();
	}
};

#endif