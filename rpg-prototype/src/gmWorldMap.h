#ifndef GMWORLDMAP_H
#define GMWORLDMAP_H

#include <vector>
#include "gmMap.h"

class gmLocale;
class gmMap;

typedef void(*mapMaker)(gmMap* owner);//An awful hack to make the proper vector.

class gmWorldMap
{
public:
	std::vector <gmLocale*> locales;//A list of locales such as dungeons and towns, which are accessible from the world map.
	gmMap* map;//The world map.
	std::vector <std::vector<gmThing*>> spawnLists;//The lists of things to spawn in each locale.
	std::vector <mapMaker> makeRoutines;//The functions used to generate locales.
	void makeLocales(int numLocales = 10)//Randomly generate locales, such as dungeons and towns. This is very clunky at the moment.
	{
		int sx, sy, ux, uy;
		struct coords
		{
			int x, y;
			coords(int x, int y){ this->x = x; this->y = y; }
		};
		std::vector <coords> openSpaces;
		for (int dx = 0; dx < map->map[0].size(); dx++){
			for (int dy = 0; dy < map->map.size(); dy++)
			{
				if (map->map[dy][dx]->passable){ openSpaces.push_back(coords(dx, dy)); }
			}
		}
		for (int i = 0; i < numLocales; i++)
		{
			int a = rand() % openSpaces.size();
			locales.push_back(new gmLocale("test", false, false, 10, "res/dungeon-entrance.png", 0, 0, {}));
			locales.back()->load();
			locales.back()->x = openSpaces[a].x;
			locales.back()->y = openSpaces[a].y;
			locales.back()->mapList.push_back(new gmMap(10, 10, false, makeRoutines[rand() % makeRoutines.size()], spawnLists[rand() % spawnLists.size()]));
			locales.back()->mapList.back()->make(locales.back()->mapList.back());
			//Generate stairs.
			for (int j = 0; j < locales.back()->mapList.size(); j++)
			{
				//X and Y coords of the stairs.
				sx = rand() % locales.back()->mapList[j]->map[0].size();
				sy = rand() % locales.back()->mapList[j]->map.size();
				ux = rand() % locales.back()->mapList[j]->map[0].size();
				uy = rand() % locales.back()->mapList[j]->map.size();
				while (locales.back()->mapList[j]->map[sy][sx]->passable == false)
				{
					sx = rand() % locales.back()->mapList[j]->map[0].size();
					sy = rand() % locales.back()->mapList[j]->map.size();
				}
				while (locales.back()->mapList[j]->map[uy][ux]->passable == false)
				{
					ux = rand() % locales.back()->mapList[j]->map[0].size();
					uy = rand() % locales.back()->mapList[j]->map.size();
				}
				if (j != locales.back()->mapList.size()-1)//For every floor other than the last, generate stairs leading to the next floor.
				{
					locales.back()->mapList[j]->map[uy][ux] = new gmTile("res/stairs-down.png", true, false, locales.back()->mapList[j + 1], ux, uy);
					locales.back()->mapList[j]->map[uy][ux]->load();
				}
				if (j == 0)//For the first floor, generate stairs back to the world map.
				{
					locales.back()->mapList[j]->map[sy][sx] = new gmTile("res/stairs-up.png", true, false,NULL, 0,0, this->map, locales.back()->x, locales.back()->y);
					locales.back()->mapList[j]->map[sy][sx]->load();
				}
				else//For every other floor, generate stairs leading to the previous / next floors.
				{
					locales.back()->mapList[j]->map[sy][sx] = new gmTile("res/stairs-up.png", true, false, NULL, 0, 0, locales.back()->mapList[j-1], sx, sy);
					locales.back()->mapList[j]->map[sy][sx]->load();
				}
			}
		}
		return;
	}
	gmWorldMap(int w, int h, void(*make)(gmMap* owner), std::vector <mapMaker> makeRoutines, std::vector<std::vector<gmThing*>> spawnLists)
	{
		if (!seeded){ srand(time(NULL)); }
		this->spawnLists = spawnLists;
		this->makeRoutines = makeRoutines;
		this->map = new gmMap(w, h, true, make, {});
		this->map->make(this->map);
		this->makeLocales();
		for (int i = 0; i < locales.size(); i++)
		{
			locales[i]->setDangerLevel();
		}
	}
};

#endif