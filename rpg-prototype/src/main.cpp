bool seeded = false;
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <algorithm>
#include "fov.h"
#include "gmRender.h"
#include "gmThing.h"
#include "gmDice.h"
#include "gmTile.h"
#include "gmAi.h"
#include "gmMap.h"
#include "gmFov.h"
#include "gmInterface.h"
#include "gmToss.h"
#include "gmLocale.h"
#include "gmWorldMap.h"
#include "gmGrab.h"

#define DEBUG true
#define FOV_RADIUS 3

#define PANEL_NONE			0
#define PANEL_PLAYERSTATUS	1
#define PANEL_TARGETSTATUS	2
#define PANEL_INVENTORY		3

int panelMode = PANEL_NONE;

SDL_Surface* background = NULL;
gmTile ground("res/floor.png", true, false);
gmTile wall("res/wall.png", false, true);
gmTile door("res/door.png", true, true);
gmTile downStairs("res/stairs-down.png", true, false);
gmTile upStairs("res/stairs-up.png", true, false);
gmMap* currentMap;

int distance(int dX0, int dY0, int dX1, int dY1)
{
	return int(sqrt(abs((dX1 - dX0)*(dX1 - dX0) + (dY1 - dY0)*(dY1 - dY0))));
}

gmThing* getThingByLocation(int x, int y, gmMap* map = currentMap, gmThing* ignore = NULL)
{
	for (int i = 0; i < map->things.size(); i++)
	{
		if (map->things[i]->x == x && map->things[i]->y == y)
		{
			if (ignore != NULL)
			{
				if (map->things[i] != ignore) { return map->things[i]; }
			}
			else
			{
				return map->things[i];
			}
		}
	}
	return NULL;
}

std::vector <gmThing*> getThingListByLocation(int x, int y, gmMap* map = currentMap, gmThing* ignore = NULL)
{
	std::vector <gmThing*> l;
	for (int i = 0; i < map->things.size(); i++)
	{
		if (map->things[i]->x == x && map->things[i]->y == y)
		{
			if (ignore != NULL)
			{
				if (map->things[i] != ignore) { l.push_back(map->things[i]); }
			}
			else
			{
				l.push_back(map->things[i]);
			}
		}
	}
	return l;
}

std::vector <std::string> getNames(std::vector <gmThing*> a)
{
	std::vector <std::string> b;
	for (int i = 0; i < a.size(); i++)
	{
		b.insert(b.end(), a[i]->name);
	}
	return b;
}

bool getCanMove(int x, int y, gmMap* map = currentMap)
{
	//If there is nothing there, true. If there is, and it is passable, true. Otherwise, false.
	bool thingPresent = (getThingByLocation(x, y, map) == NULL) ? false : getThingByLocation(x, y, map)->isPassable == false;
	return (!thingPresent && x >= 0 && y >= 0 && x < map->map[0].size() && y < map->map.size() && map->map[y][x]->passable);
}

void moveMaps(gmThing* target, gmMap* source, gmMap* destination, int x, int y)
{
	//Remember: the player is always the first thing in the thinglist.
	target->x = x;
	target->y = y;
	target == player ? destination->things.insert(destination->things.begin(), target) : destination->things.insert(destination->things.end(), target);
	int index = 0;
	for (int i = 0; i < source->things.size(); i++){ if (source->things[i] == target){ index = i; break; } }//Find the target thing's index so that we can...
	source->things.erase(source->things.begin() + index); //...erase it from the source thinglist.
	if (target == player)
	{
		currentMap = destination;//If the target is the player, set the currentMap to the destination.
		player = currentMap->things[0];//And reset the player pointer, or things get messy.
		camX = target->x - SCREEN_WIDTH / 60;
		camY = target->y - SCREEN_HEIGHT / 60;
	}
	return;
}

std::vector <std::string> getBodyPartNames(gmThing* target)
{
	std::vector <std::string> names;
	for (int i = 0; i < target->bodyParts.size(); i++)
	{
		if (target->bodyParts[i]->equippedThing)
		{
			names.push_back("On " + target->bodyParts[i]->name + ": " + target->bodyParts[i]->equippedThing->name);
		}
		else
		{
			names.push_back("Empty " + target->bodyParts[i]->name);
		}
	}
	return names;
}

void equipThing(gmThing* owner, gmThing* equipping, gmBodyPart* targetPart)
{
	int index = -1;
	//Find the item in the owner's bag, and save its index.
	if (equipping != NULL){for (int i = 0; i < owner->inventory.size(); i++){if (owner->inventory[i] == equipping){index = i;}}}
	if (targetPart->equippedThing)
	{
		owner->inventory.push_back(targetPart->equippedThing);
		targetPart->equippedThing = NULL;
	}
	targetPart->equippedThing = equipping;
	if (equipping != NULL){owner->inventory.erase(owner->inventory.begin() + index);}
	return;
}

bool grab(gmMap* map)
{
	messageLog.log.push_back("Which direction would you like to grab?");
	messageLog.show();
	SDL_UpdateWindowSurface(gWindow);
	SDL_Event e;
	bool selected = false;
	int dx, dy;
	while (!selected)
	{
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_KEYDOWN)
			{
				switch (e.key.keysym.sym)
				{
				case SDLK_UP:
					dy = -1;
					dx = 0;
					selected = true;
					break;
				case SDLK_DOWN:
					dy = 1;
					dx = 0;
					selected = true;
					break;
				case SDLK_LEFT:
					dx = -1;
					dy = 0;
					selected = true;
					break;
				case SDLK_RIGHT:
					dx = 1;
					dy = 0;
					selected = true;
					break;
				}
			}
		}
	}
	std::vector <gmThing*> t = getThingListByLocation(player->x + dx, player->y + dy);
	if (t.size() > 1)
	{
		sidePanel.heading = "SELECT A TARGET TO GRAB:";
		sidePanel.options = getNames(t);
		sidePanel.selectFromList();
		if (sidePanel.finalSelectionList.back() >= 0)
		{
			grabThing(player, t[sidePanel.finalSelectionList.back()], currentMap);
		}
		sidePanel.reset();
	}
	else if (t.size() > 0)
	{
		grabThing(player, t[0], currentMap);
	}
	return false;
}


void dropThing (gmThing* owner, gmThing** target)
{
	if (owner->inventory.size() > 0)
	{
		int index = 0;
		//Find the thing in the owner's inventory.
		for (int i = 0; i < owner->inventory.size(); i++){ if (*target == owner->inventory[i]){ index = i; } };
		//Add it to the map and remove it from the inventory.
		owner->inventory[index]->x = owner->x;
		owner->inventory[index]->y = owner->y;
		currentMap->things.insert(currentMap->things.end(), owner->inventory[index]);
		owner->inventory.erase(owner->inventory.begin() + index);
		currentMap->things.back()->load();
	}
	return;
}

//Mapmakers
void mapMake_Blank(gmMap* owner)//100% walkable ground.
{
	for (int y = 0; y < owner->map.size(); y++)
	{
		for (int x = 0; x < owner->map[y].size(); x++)
		{
			owner->map[y][x] = &ground;
		}
	}
	owner->spawnThings();
	return;
}

void mapMake_Random(gmMap* owner)//Random, with no refinement.
{
	for (int y = 0; y < owner->map.size(); y++)
	{
		for (int x = 0; x < owner->map[y].size(); x++)
		{
			roll(1, 'd', 6) == 1 ? owner->map[y][x] = &wall : owner->map[y][x] = &ground;
		}
	}
	owner->spawnThings();
}

void mapMake_CellularAutomata(gmMap* owner)//Random, with cellular automata-style refinement of edges, etc.
{
	int adjacent = 0;
	int iterations = 7;
	for (int y = 0; y < owner->map.size(); y++)
	{
		for (int x = 0; x < owner->map[y].size(); x++)
		{
			roll(1, 'd', 100) > 42 ? owner->map[y][x] = &wall : owner->map[y][x] = &ground;
		}
	}
	for (int i = 0; i < iterations; i++)
	{
		for (int y = 1; y < owner->map.size() - 1; y++)
		{
			for (int x = 1; x < owner->map[y].size() - 1; x++)
			{
				adjacent = 0;
				for (int x2 = -1; x2 <= 1; x2++)
				{
					for (int y2 = -1; y2 <= 1; y2++)
					{
						adjacent += (&wall == owner->map[y+y2][x+x2] ? 1 : 0);
					}
				}
				owner->map[y][x] = adjacent >= 6 ? &wall : owner->map[y][x];
				owner->map[y][x] = adjacent <= 4 ? &ground : owner->map[y][x];
			}
		}
	}
	owner->spawnThings();
	return;
}

gmTile water("res/water.png",false, false);
gmTile grass("res/grass.png", true, false);

void mapMake_World(gmMap* owner)
{
	water.load();
	grass.load();
	int adjacent = 0;
	int iterations = 7;
	for (int y = 0; y < owner->map.size(); y++)
	{
		for (int x = 0; x < owner->map[y].size(); x++)
		{
			roll(1, 'd', 100) > 42 ? owner->map[y][x] = &water : owner->map[y][x] = &grass;
		}
	}
	for (int i = 0; i < iterations; i++)
	{
		for (int y = 1; y < owner->map.size() - 1; y++)
		{
			for (int x = 1; x < owner->map[y].size() - 1; x++)
			{
				adjacent = 0;
				for (int x2 = -1; x2 <= 1; x2++)
				{
					for (int y2 = -1; y2 <= 1; y2++)
					{
						adjacent += (&water == owner->map[y + y2][x + x2] ? 1 : 0);
					}
				}
				owner->map[y][x] = adjacent >= 6 ? &water : owner->map[y][x];
				owner->map[y][x] = adjacent <= 4 ? &grass : owner->map[y][x];
			}
		}
	}
	return;
}
void moveThing(gmThing* target, int dx, int dy, gmMap* map = currentMap)
{
	std::vector <gmThing*> t;
	int index = -1;
	if (getThingByLocation(target->x + dx, target->y + dy) != NULL && getThingByLocation(target->x + dx, target->y + dy)->isPassable == false)
	{
		if (target->attack && target != player){ target->attack(target, getThingByLocation(target->x + dx, target->y + dy)); }
		else if (target->attack && target == player)
		{
			t = getThingListByLocation(target->x + dx, target->y + dy);
			if (t.size() > 1)
			{
				sidePanel.heading = "SELECT A TARGET FOR YOUR ATTACK:";
				sidePanel.options = getNames(t);
				sidePanel.selectFromList();
				if (sidePanel.finalSelectionList.back() >= 0)
				{
					target->attack(target, t[sidePanel.finalSelectionList.back()]);
				}
				sidePanel.reset();
			}
			else
			{
				target->attack(target, getThingByLocation(target->x + dx, target->y + dy));
			}
		}
	}
	else if (getCanMove(target->x + dx, target->y + dy))//Uses data from the map to determine whether the tile can be walked onto.
	{
		target->step(dx, dy);//Step in the direction.
		if (target == player)
		{
			camX = target->x - SCREEN_WIDTH / 60;
			camY = target->y - SCREEN_HEIGHT / 60;
		}
	}
	return;
}

#include "dataThings.h"

gmMap testMap(100, 100, false, mapMake_CellularAutomata, {&thing_Myconid});
gmMap testMap2(100, 100, false, mapMake_Random, { &thing_Bomber });
gmWorldMap worldMap(
	30, 30, 
	mapMake_World, 
	{ mapMake_Blank, mapMake_CellularAutomata, mapMake_Random }, 
	{ {new gmThing(thing_Pickaxe), new gmThing(thing_Bomber), new gmThing(thing_Myconid)} });

bool init()
{
	srand(time(NULL));
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Create window
		gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Initialize PNG loading
			int imgFlags = IMG_INIT_PNG;
			if (!(IMG_Init(imgFlags) & imgFlags))
			{
				printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
				success = false;
			}
			else
			{
				//Get window surface
				screen = SDL_GetWindowSurface(gWindow);
				SDL_SetSurfaceBlendMode(screen, SDL_BLENDMODE_BLEND);
			}
		}
		TTF_Init();
		messageLog.log.push_back("Welcome.");
		background = load_surface("res/background-alternate.png");
		ground.load();
		wall.load();
		door.load();
	}

	fov_settings_init(&fovSettings);
	fov_settings_set_opacity_test_function(&fovSettings, opaque);
	fov_settings_set_apply_lighting_function(&fovSettings, applyFov);

	currentMap = worldMap.map;

	currentMap->things.insert(currentMap->things.begin(), new gmThing(thing_Player));
	currentMap->things[0]->load();

	player = currentMap->things[0];
	camX = player->x - SCREEN_WIDTH / 60;
	camY = player->y - SCREEN_HEIGHT / 60;

	return success;
}

void close()
{
	//Destroy window
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

void run()
{
	//Whether the player has taken an action or not.
	bool hasMoved = false;
	//Main loop flag
	gameState = PLAYING;

	//Event handler
	SDL_Event e;

	//While application is running
	while (gameState != QUITTING)
	{
		//Handle events on queue
		while (SDL_PollEvent(&e) != 0)
		{
			//User requests quit
			if (e.type == SDL_QUIT)
			{
				gameState = QUITTING;
			}
			int index = 0;
			if (gameState == PLAYING)
			{
				if (e.type == SDL_KEYDOWN)
				{
					switch (e.key.keysym.sym)
					{
					case SDLK_KP_7:
						moveThing(player, -1, -1);
						hasMoved = true;
						break;
					case SDLK_KP_8:
					case SDLK_UP:
						moveThing(player, 0, -1);
						hasMoved = true;//The player has moved.
						break;
					case SDLK_KP_9:
						moveThing(player, 1, -1);
						hasMoved = true;
						break;
					case SDLK_KP_1:
						moveThing(player, -1, 1);
						hasMoved = true;
						break;
					case SDLK_KP_3:
						moveThing(player, 1, 1);
						hasMoved = true;
						break;
					case SDLK_KP_2:
					case SDLK_DOWN:
						moveThing(player, 0, 1);						
						hasMoved = true;
						break;
					case SDLK_KP_6:
					case SDLK_RIGHT:
						moveThing(player, 1, 0);
						hasMoved = true;
						break;
					case SDLK_KP_4:
					case SDLK_LEFT:
						moveThing(player, -1, 0);
						hasMoved = true;
						break;
					case SDLK_KP_5:
					case SDLK_SPACE:
						hasMoved = true;
						break;
					case SDLK_g:
						if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_LSHIFT] || SDL_GetKeyboardState(NULL)[SDL_SCANCODE_RSHIFT])
						{
							grab(currentMap);
						}
						else if (getThingByLocation(player->x, player->y, currentMap, player))
						{
							player->inventory.insert(player->inventory.end(), getThingByLocation(player->x, player->y, currentMap, player));
							for (int i = 0; i < currentMap->things.size(); i++){ if (currentMap->things[i] == getThingByLocation(player->x, player->y, currentMap, player)){ index = i; break; } }
							messageLog.log.push_back("Picked up: "+currentMap->things[index]->name);
							currentMap->things.erase(currentMap->things.begin() + index);
						}
						break;
					case SDLK_d:
						sidePanel.heading = "SELECT SOMETHING TO DROP:";
						sidePanel.options = getNames(player->inventory);
						sidePanel.selectFromList();
						if (sidePanel.finalSelectionList.back() >= 0)
						{
							dropThing(player, &player->inventory[sidePanel.finalSelectionList.back()]);
						}
						sidePanel.reset();
						break;
					case SDLK_r:
						sidePanel.heading = "SELECT AN ITEM TO REMOVE:";
						sidePanel.options = getBodyPartNames(player);
						sidePanel.selectFromList();
						if (sidePanel.finalSelectionList.back() >= 0)
						{
							equipThing(player, NULL, player->bodyParts[sidePanel.finalSelectionList.back()]);
						}
						sidePanel.reset();
						break;
					case SDLK_a:
						if (player->bodyParts.size() > 0 && player->bodyParts[0]->equippedThing && player->bodyParts[0]->equippedThing->use)
						{
							player->bodyParts[0]->equippedThing->use(player, player, player->x, player->y, currentMap);
						}
						break;
					case SDLK_u:
						sidePanel.heading = "SELECT SOMETHING TO USE:";
						sidePanel.options = getNames(player->inventory);
						sidePanel.selectFromList();
						if (sidePanel.finalSelectionList.back() >= 0)
						{
							player->inventory[sidePanel.finalSelectionList.back()]->use(player, player, player->x, player->y, currentMap);
						}
						sidePanel.reset();
						break;
					case SDLK_t:
						sidePanel.heading = "SELECT SOMETHING TO TOSS:";
						sidePanel.options = getNames(player->inventory);
						sidePanel.selectFromList();
						if (sidePanel.finalSelectionList.back() >= 0)
						{
							tossThing(player->inventory[sidePanel.finalSelectionList.back()], player, player->x, player->y, currentMap);
						}
						sidePanel.reset();
						break;
					case SDLK_i:
						sidePanel.heading = "INVENTORY:";
						sidePanel.options = getNames(player->inventory);
						sidePanel.selectFromList();
						sidePanel.reset();
						break;
					case SDLK_e:
						sidePanel.heading = "SELECT SOMETHING TO EQUIP:";
						sidePanel.options = getNames(player->inventory);
						sidePanel.selectFromList();
						if (sidePanel.finalSelectionList.back() >= 0)
						{
							sidePanel.heading = "SELECT WHICH BODY PART TO EQUIP TO:";
							sidePanel.options = getBodyPartNames(player);
							sidePanel.selectFromList();
							if (sidePanel.finalSelectionList.back() >= 0)
							{
								equipThing(
									player, 
									player->inventory[sidePanel.finalSelectionList[sidePanel.finalSelectionList.size()-2]],
									player->bodyParts[sidePanel.finalSelectionList.back()]
									);
							}
						}
						sidePanel.reset();
						break;
					case SDLK_TAB:
						panelMode = panelMode > 3 ? 0 : panelMode + 1;
					case SDLK_PERIOD:
						if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_LSHIFT] || SDL_GetKeyboardState(NULL)[SDL_SCANCODE_RSHIFT])
						{
							if (currentMap->map[player->y][player->x]->enterTo)
							{
								int x = player->x;
								int y = player->y;
								gmMap* map = currentMap->map[y][x]->enterTo;
								int px = currentMap->map[y][x]->entryX;
								int py = currentMap->map[y][x]->entryY;
								moveMaps(player, currentMap, map, px, py);
							}
							if (currentMap == worldMap.map)
							{
								for (int i = 0; i < worldMap.locales.size(); i++)
								{
									if (worldMap.locales[i]->x == player->x && worldMap.locales[i]->y == player->y)
									{
										worldMap.locales[i]->mapList[0]->make(worldMap.locales[i]->mapList[0]);
										moveMaps(player, currentMap, worldMap.locales[i]->mapList[0], 0, 0);
										break;
									}
								}
							}
						}
						break;
					case SDLK_1:
						if (DEBUG)
						{
							//Switch control.
							if (currentMap->things.size() > 1){ player = (player == currentMap->things[0]) ? currentMap->things[1] : currentMap->things[0]; }
							//Set the camera over the player, with the player centred.
							camX = player->x - SCREEN_WIDTH / 60;
							camY = player->y - SCREEN_HEIGHT / 60;
						}
						break;
					case SDLK_2:
						if (DEBUG)
						{
							moveMaps(player, currentMap, &testMap2, 0, 0);
							camX = player->x - SCREEN_WIDTH / 60;
							camY = player->y - SCREEN_HEIGHT / 60;
						}
						break;
					case SDLK_3:
						if (DEBUG)
						{
							if (currentMap->things.size() >= 1){moveMaps(currentMap->things[currentMap->things.size()-1], currentMap, &testMap2,0, 0);}
						}
						break;
					case SDLK_4:
						if (DEBUG)
						{
							messageLog.log.push_back("The quick brown fox jumped over the lazy dog.");
						}
						break;
					case SDLK_5:
						if (DEBUG)
						{
							currentMap->things.push_back(new gmThing(thing_Imp));
							currentMap->things.back()->load();
							currentMap->things.back()->x = player->x;
							currentMap->things.back()->y = player->y;
						}
						break;
					case SDLK_6:
						if (DEBUG)
						{
							player->curHp += 10000000;
						}
						break;
					case SDLK_7:
						if (DEBUG)
						{
							sidePanel.heading = "THINGLIST:";
							sidePanel.options = getNames(currentMap->things);
							sidePanel.selectFromList();
							sidePanel.reset();
						}
						break;
					case SDLK_ESCAPE:
						gameState = QUITTING;
						break;
					}
				}
			}
		}
		if (hasMoved)//If the player has moved.
		{
			if (currentMap == worldMap.map)
			{
				for (int i = 0; i < worldMap.locales.size(); i++)
				{
					if (player->x == worldMap.locales[i]->x && player->y == worldMap.locales[i]->y)
					{
						if (worldMap.locales[i]->dangerLevel < player->getStatTotal())
						{
							messageLog.log.push_back("You whistle as you pass by the "+ worldMap.locales[i]->name + ".");
						}
						else if (worldMap.locales[i]->dangerLevel == player->getStatTotal())
						{
							messageLog.log.push_back("You look curiously as you pass the " + worldMap.locales[i]->name + ".");
						}
						else if (worldMap.locales[i]->dangerLevel > player->getStatTotal())
						{
							messageLog.log.push_back("Your heart pounds as you pass the " + worldMap.locales[i]->name + ".");
						}
					}
				}
			}
			for (int i = 0; i < currentMap->things.size(); i++)
			{
				//If the thing has died.
				if (currentMap->things[i]->curHp <= 0 && !currentMap->things[i]->inanimate)
				{
					currentMap->things[i]->death(currentMap->things[i]);
					currentMap->things.erase(currentMap->things.begin() + i);
					i -= 1;
					i = i < 0 ? 0 : i;
				}
				//For all things other than the player.
				if (currentMap->things[i] != player)
				{
					//If the thing doesn't have a target...
					if (currentMap->things[i]->target == NULL && currentMap->things[i]->ai && currentMap->things[i]->ai->getTarget)
					{
						currentMap->things[i]->ai->getTarget(currentMap->things[i]);//...get a target.
					}
					//The if statement here determines whether or not the thing has AI at all. If it does...
					if (currentMap->things[i]->ai && currentMap->things[i]->ai->takeTurn){ currentMap->things[i]->ai->takeTurn(currentMap->things[i]); }//...each 'thing' takes a turn.
				}
			}
		}
		apply(0, 0, background, screen);
		//Reset the fovMap so that all tiles are 'hidden'.
		for (int x = 0, y = 0; y < currentMap->fovMap.size(); x++)
		{
			currentMap->fovMap[y][x] = false;
			if (x == currentMap->fovMap[y].size()-1){x = -1; y++;}
		}
		//Recalculate the new fovMap.
		if (currentMap->isOutdoors){ doFov(player->x, player->y, player->viewRadius+3, currentMap); }
		else{doFov(player->x, player->y, player->viewRadius, currentMap);}
		//Render the tiles.
		for (int y = camY < 0 ? 0 : camY; y < (camY + (SCREEN_HEIGHT / 30) > currentMap->map.size() ? currentMap->map.size() : camY + (SCREEN_HEIGHT / 30)); y++)
		{
			for (int x = camX < 0 ? 0 : camX; x < (camX + (SCREEN_WIDTH / 30) > currentMap->map[y].size() ? currentMap->map[y].size() : camX + (SCREEN_WIDTH / 30) ); x++)
			{
				//If the tile is within the player's field of view...
				if (currentMap->map[y][x]->img && currentMap->fovMap[y][x] == true){ currentMap->map[y][x]->show(x, y); } //...show the tile.
				//Otherwise, if the tile has been seen before by the player, but is not within their field of view...
				else if (currentMap->map[y][x]->img && currentMap->rememberMap[y][x] == true)
				{
					currentMap->map[y][x]->showTransparent(x, y);
				}
			}
		}
		if (currentMap == worldMap.map)
		{
			for (int i = 0; i < worldMap.locales.size(); i++)
			{
				worldMap.locales[i]->show();
			}
		}
		for (int i = 0; i < currentMap->things.size(); i++)
		{
			if (currentMap->things[i] != player && 
				currentMap->fovMap[currentMap->things[i]->y][currentMap->things[i]->x] && 
				currentMap->things[i]->img){ currentMap->things[i]->show(); }
		}
		player->show();
		if (player->curHp <= 0)//If the player has died.
		{
			gameState = QUITTING;
		}
		//std::vector <std::string> itemNames;
		//itemNames = std::vector <std::string>();
		//for (int i = 0; i < player->inventory.size(); i++){ itemNames.insert(itemNames.end(), player->inventory[i]->name); }
		switch (panelMode)
		{
		case PANEL_NONE:
			sidePanel.reset();
			break;
		case PANEL_PLAYERSTATUS:
			sidePanel.reset();
			sidePanel.heading = "YOUR STATISTICS: ";
			sidePanel.options.push_back("Name: " + player->name);
			sidePanel.options.push_back("HP: " + std::to_string(player->curHp) + "/" + std::to_string(player->maxHp + player->hpMod));
			sidePanel.options.push_back("STR: " + std::to_string(player->str) + "+" + std::to_string(player->getStrMod()));
			sidePanel.options.push_back("DEX: " + std::to_string(player->dex) + "+" + std::to_string(player->getDexMod()));
			sidePanel.options.push_back("CON: " + std::to_string(player->con) + "+" + std::to_string(player->getConMod()));
			sidePanel.options.push_back("ITL: " + std::to_string(player->itl) + "+" + std::to_string(player->getItlMod()));
			sidePanel.options.push_back("CHA: " + std::to_string(player->cha) + "+" + std::to_string(player->getChaMod()));
			sidePanel.options.push_back("PER: " + std::to_string(player->per) + "+" + std::to_string(player->getPerMod()));
			sidePanel.options.push_back("AC: " + std::to_string(player->ac) + "+" + std::to_string(player->acMod));
			break;
		case PANEL_TARGETSTATUS:
			sidePanel.reset();
			sidePanel.heading = "TARGET STATISTICS: ";
			if (player->target)
			{
				sidePanel.options.push_back("Name: " + player->target->name);
				sidePanel.options.push_back("HP: " + std::to_string(player->target->curHp) + "/" + std::to_string(player->target->maxHp + player->target->hpMod));
				sidePanel.options.push_back("STR: " + std::to_string(player->target->str) + "+" + std::to_string(player->target->getStrMod()));
				sidePanel.options.push_back("DEX: " + std::to_string(player->target->dex) + "+" + std::to_string(player->target->getDexMod()));
				sidePanel.options.push_back("CON: " + std::to_string(player->target->con) + "+" + std::to_string(player->target->getConMod()));
				sidePanel.options.push_back("ITL: " + std::to_string(player->target->itl) + "+" + std::to_string(player->target->getItlMod()));
				sidePanel.options.push_back("CHA: " + std::to_string(player->target->cha) + "+" + std::to_string(player->target->getChaMod()));
				sidePanel.options.push_back("PER: " + std::to_string(player->target->per) + "+" + std::to_string(player->target->getPerMod()));
				sidePanel.options.push_back("AC: " + std::to_string(player->target->ac) + "+" + std::to_string(player->target->acMod));
			}
			else
			{
				sidePanel.options.push_back("No target.");
			}
			break;
		}
		messageLog.show();
		sidePanel.show();
		SDL_UpdateWindowSurface(gWindow);
		hasMoved = false;
	}
}

int main(int argc, char* args[])
{
	//Start up SDL and create window
	if (!init())
	{
		printf("Failed to initialize!\n");
	}
	else
	{
		run();
	}
	//Free resources and close SDL
	close();
	return 0;
}