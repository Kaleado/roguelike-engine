#ifndef GMMAP_H
#define GMMAP_H
#include <stdio.h>
#include <string>
#include "gmTile.h"
#include "gmThing.h"

/*Class for all the maps the player may explore.
 *This needs a lot of work:
 *		- The addition of more tile types would assist in the creation of more diverse maps.
 *		- The removal of the simple 'wall, floor or door' manner in which the tiles are determined.
 *		- The spawnlist needs to be segmented into the spawnlist for items and the spawnlist for creatures.
*/

class gmMap
{
public:
	bool isOutdoors; //Whether the map is outside or underground / undercover. Determines viewRadius.
	std::vector <std::vector<gmTile*>> map;//The tiles on the map.
	std::vector <std::vector<bool>> fovMap;//Whether or not the tiles are within the player's FoV.
	std::vector <std::vector<bool>> rememberMap;//Whether or not the player has seen the tiles before.
	std::string wallPath, floorPath, doorPath;//Paths to the images for the walls, floors, and doors. This HAS to change.
	std::vector <gmThing*> spawnList;//Things which may spawn on this level.
	std::vector <gmThing*> things;//The things present on this map, also known as the 'thinglist'.
	void(*make)(gmMap* owner);//The function to make the map.
	void spawnThings()//Randomly spawns things from the spawnList to populate the level. Currently this does not distinguish between enemies and items.
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
			for (int i = 0; i < map.size() * map[0].size() / 100;)
			{
				for (int j = 0; j < spawnList.size(); j++)
				{
					if (roll(1, 'd', 100) <= spawnList[j]->rarity)
					{ 
						things.push_back(new gmThing(*spawnList[j])); 
						int a = rand() % openSpaces.size();
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
	gmMap(int w, int h, bool isOutdoors, void(*make)(gmMap* owner), std::vector <gmThing*> spawnList)
	{
		this->isOutdoors = isOutdoors;
		this->make = make;
		map.resize(h, std::vector<gmTile*>(w));
		fovMap.resize(h, std::vector<bool>(w));
		rememberMap.resize(h, std::vector<bool>(w));
		this->spawnList = spawnList;
	}
};

#endif