#ifndef GMWORLDMAP_H
#define GMWORLDMAP_H

#include <vector>
#include "gmMap.h"

class gmLocale;
class gmMap;

typedef void(*mapMaker)(gmMap* owner);

class gmWorldMap
{
public:
	std::vector <gmLocale*> locales;
	gmMap* map;
	std::vector <std::vector<gmThing*>> spawnLists;
	std::vector <mapMaker> makeRoutines;
	void makeLocales(int numLocales = 10)
	{
		for (int i = 0; i < numLocales; i++)
		{
			locales.push_back(new gmLocale("test", false, 10, "res/dungeon-entrance.png", 0, 0, {}));
			locales.back()->load();
			locales.back()->x = 0;
			locales.back()->y = 0;
			locales.back()->mapList.push_back(new gmMap(50, 50, makeRoutines[rand() % makeRoutines.size()], {}));
			//locales.back()->mapList.back()->make(locales.back()->mapList.back());
		}
		return;
	}
	gmWorldMap(int w, int h, void(*make)(gmMap* owner), std::vector <mapMaker> makeRoutines, std::vector<std::vector<gmThing*>> spawnLists)
	{
		this->spawnLists = spawnLists;
		this->makeRoutines = makeRoutines;
		this->map = new gmMap(w, h, make, {});
		this->makeLocales();
	}
};

#endif