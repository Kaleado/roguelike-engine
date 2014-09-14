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
	std::vector <gmThing> things;//The things present on this map, also known as the 'thinglist'.
	void(*make)(gmMap* owner);//The function to make the map.
	gmMap(int w, int h, void(*make)(gmMap* owner))
	{
		this->make = make;
		map.resize(h, std::vector<gmTile*>(w));
		fovMap.resize(h, std::vector<bool>(w));
		rememberMap.resize(h, std::vector<bool>(w));
	}
};

#endif