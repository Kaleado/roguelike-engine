#ifndef GMTOSS_H
#define GMTOSS_H

#include "gmFov.h"
#include "gmMap.h"
#include <vector>

class gmMap;
class gmTile;

bool isTrajectoryBlocked(int x0, int y0, int x1, int y1, int range, gmMap* map)
{
	struct coords
	{
		int x, y;
		coords(int x, int y){ this->x = x, this->y = y; }
	};
	int x = x0;
	int y = y0;
	int x2 = x1;
	int y2 = y1;
	std::vector <coords> lineCoords;
	int w = x2 - x;
	int h = y2 - y;
	int dx1 = 0, dy1 = 0, dx2 = 0, dy2 = 0;
	if (w<0) dx1 = -1; else if (w>0) dx1 = 1;
	if (h<0) dy1 = -1; else if (h>0) dy1 = 1;
	if (w<0) dx2 = -1; else if (w>0) dx2 = 1;
	int longest = abs(w);
	int shortest = abs(h);
	if (!(longest>shortest))
	{
		longest = abs(h);
		shortest = abs(w);
		if (h<0) dy2 = -1; else if (h>0) dy2 = 1;
		dx2 = 0;
	}
	int numerator = longest >> 1;
	for (int i = 0; i <= longest; i++)
	{
		lineCoords.push_back(coords(x, y));
		numerator += shortest;
		if (!(numerator<longest))
		{
			numerator -= longest;
			x += dx1;
			y += dy1;
		}
		else
		{
			x += dx2;
			y += dy2;
		}
	}
	for (int i = 0; i < lineCoords.size(); i++)
	{
		if (map->map[lineCoords[i].y][lineCoords[i].x]->passable == false){ return true; }
	}
	return false;
}

void tossThing(gmThing* ammo, gmThing* owner, int x2, int y2, gmMap* map)
{
	struct coords
	{
		int x, y;
		coords(int x, int y){ this->x = x, this->y = y; }
	};
	std::vector <coords> lineCoords;
	bool selected = false;
	bool cancelled = false;
	SDL_Surface* targetCursor = IMG_Load("res/tile-cursor.png");
	SDL_Event e;
	while (!selected)
	{
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_KEYDOWN)
			{
				int x = owner->x;
				int y = owner->y;
				lineCoords = std::vector <coords>();
				switch (e.key.keysym.sym)
				{
				case SDLK_UP:
					--y2 < 0 ? y2 = 0: y2 = y2;
					y2 > map->map.size()-1 ? y2 = map->map.size()-1 : y2 = y2;
					y2 > SCREEN_HEIGHT / 30 + camY - 1 ? y2 = SCREEN_HEIGHT / 30 + camY - 1 : y2 = y2;
					!map->rememberMap[y2][x2] ? y2++ : y2 = y2;
					break;
				case SDLK_DOWN:
					++y2 < 0 ? y2 = 0 : y2 = y2;
					y2 > map->map.size()-1 ? y2 = map->map.size()-1 : y2 = y2;
					y2 > SCREEN_HEIGHT / 30 + camY - 1 ? y2 = SCREEN_HEIGHT / 30 + camY - 1 : y2 = y2;
					!map->rememberMap[y2][x2] ? y2-- : y2 = y2;
					break;
				case SDLK_LEFT:
					--x2 < 0 ? x2 = 0 : x2 = x2;
					x2 > map->map[0].size() - 1 ? x2 = map->map[0].size() - 1 : x2 = x2;
					x2 > SCREEN_WIDTH / 30 + camX - 1 ? x2 = SCREEN_WIDTH / 30 + camX - 1 : x2 = x2;
					!map->rememberMap[y2][x2] ? x2++ : x2 = x2;
					break;
				case SDLK_RIGHT:
					++x2 < 0 ? x2 = 0 : x2 = x2;
					x2 > map->map[0].size()-1 ? x2 = map->map[0].size()-1 : x2 = x2;
					x2 > SCREEN_WIDTH / 30 + camX - 1 ? x2 = SCREEN_WIDTH / 30 + camX - 1 : x2 = x2;
					!map->rememberMap[y2][x2] ? x2-- : x2 = x2;
					break;
				case SDLK_RETURN:
					selected = true;
					break;
				case SDLK_ESCAPE:
					cancelled = true;
					selected = true;
					break;
				}
				int w = x2 - x;
				int h = y2 - y;
				int dx1 = 0, dy1 = 0, dx2 = 0, dy2 = 0;
				if (w<0) dx1 = -1; else if (w>0) dx1 = 1;
				if (h<0) dy1 = -1; else if (h>0) dy1 = 1;
				if (w<0) dx2 = -1; else if (w>0) dx2 = 1;
				int longest = abs(w);
				int shortest = abs(h);
				if (!(longest>shortest))
				{
					longest = abs(h);
					shortest = abs(w);
					if (h<0) dy2 = -1; else if (h>0) dy2 = 1;
					dx2 = 0;
				}
				int numerator = longest >> 1;
				for (int i = 0; i <= longest; i++) 
				{
					lineCoords.push_back(coords(x, y));
					numerator += shortest;
					if (!(numerator<longest)) 
					{
						numerator -= longest;
						x += dx1;
						y += dy1;
					}
					else 
					{
						x += dx2;
						y += dy2;
					}
				}
			}
			for (int ry = camY < 0 ? 0 : camY; ry < (camY + (SCREEN_HEIGHT / 30) > map->map.size() ? map->map.size() : camY + (SCREEN_HEIGHT / 30)); ry++)
			{
				for (int rx = camX < 0 ? 0 : camX; rx < (camX + (SCREEN_WIDTH / 30) > map->map[ry].size() ? map->map[ry].size() : camX + (SCREEN_WIDTH / 30)); rx++)
				{
					if (map->rememberMap[ry][rx])
					{
						map->map[ry][rx]->showTransparent(rx, ry);
					}
			}}
			for (int i = 0; i < lineCoords.size(); i++)
			{
				if (map->rememberMap[lineCoords[i].y][lineCoords[i].x])
				{
					map->map[lineCoords[i].y][lineCoords[i].x]->show(lineCoords[i].x, lineCoords[i].y);
				}
			}
			if (lineCoords.size() > 0)
			{
				apply(30*(lineCoords.back().x - camX), 30*(lineCoords.back().y - camY), targetCursor, screen);
			}
			for (int j = 0; j < map->things.size(); j++)
			{
				map->things[j]->show();
			}
			SDL_UpdateWindowSurface(gWindow);
		}
	}
	//ammo->toss(owner, getThingByLocation(x2, y2, map), x2, y2, map);
	if ( !cancelled && ammo && ammo->toss)
	{
		if (!isTrajectoryBlocked(owner->x, owner->y, x2, y2, 10, map)){ ammo->toss(owner, owner, x2, y2, map); }
	}
	return;
}

#endif