#ifndef GMMAP_H
#define GMMAP_H
#include <stdio.h>
#include <string>
#include "gmTile.h"
#include "gmThing.h"

class gmMap
{
public:
	std::vector <std::vector<gmTile*>> map;//The tiles on the map.
	std::vector <std::vector<bool>> fovMap;//Whether or not the tiles are within the player's FoV.
	std::vector <std::vector<bool>> rememberMap;//Whether or not the player has seen the tiles before.
	std::string wallPath, floorPath, doorPath;
	std::vector <gmThing*> spawnList;//Things which may spawn on this level.
	std::vector <gmThing*> things;//The things present on this map, also known as the 'thinglist'.
	void(*make)(gmMap* owner);//The function to make the map.
	void spawnThings()
	{
		struct coords
		{
			int x, y;
			coords(int x, int y){ this->x = x; this->y = y; }
		};
		std::vector <coords> openSpaces;
		for (int dx = 0; dx < map[0].size(); dx++){for (int dy = 0; dy < map.size(); dy++)
		{
			if (map[dy][dx]->passable){ openSpaces.push_back(coords(dx, dy)); }
		}}
		if (spawnList.size() > 0)
		{
			for (int i = 0; i < 3;)
			{
				for (int j = 0; j < spawnList.size(); j++)
				{
					if (roll(1, 'd', 100) <= spawnList[j]->rarity)
					{ 
						things.push_back(new gmThing(*spawnList[j])); 
						int a = roll(1, 'd', openSpaces.size()) - 1;
						things.back()->x = openSpaces[a].x;
						things.back()->y = openSpaces[a].y;
						things.back()->load();
						i++;
					}
				}
			}
		}
		return;
	}
	gmMap(int w, int h, void(*make)(gmMap* owner), std::vector <gmThing*> spawnList)
	{
		this->make = make;
		map.resize(h, std::vector<gmTile*>(w));
		fovMap.resize(h, std::vector<bool>(w));
		rememberMap.resize(h, std::vector<bool>(w));
		this->spawnList = spawnList;
	}
};

#endif