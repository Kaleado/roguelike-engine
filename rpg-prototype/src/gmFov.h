#ifndef GMFOV_H
#define GMFOV_H

#include "fov.h"
#include "gmMap.h"

fov_settings_type fovSettings;

void applyFov(void* map, int x, int y, int dx, int dy, void *src)
{
	if (x >= 0 && y >= 0 && y < ((gmMap*)map)->fovMap.size() && x < ((gmMap*)map)->fovMap[y].size())
	{
		((gmMap*)map)->rememberMap[y][x] = true;
		((gmMap*)map)->fovMap[y][x] = true;
	}
}

bool opaque(void* map, int x, int y)
{
	if (x >= 0 && y >= 0 && y < ((gmMap*)map)->fovMap.size() && x < ((gmMap*)map)->fovMap[y].size())
	{
		return ((gmMap*)map)->map[y][x]->opaque;
	}
	return true;
}

void doFov(int x, int y, int radius, void* map)
{
	fov_circle(&fovSettings, map, NULL, x, y, radius);
	((gmMap*)map)->fovMap[y][x] = true;
	((gmMap*)map)->rememberMap[y][x] = true;
	return;
}


#endif