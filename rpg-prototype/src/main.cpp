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

#define DEBUG true
#define FOV_RADIUS 3

SDL_Surface* background = NULL;
gmTile ground("res/test2.png", true, false);
gmTile wall("res/test3.png", false, true);
gmMap* currentMap;

int distance(int dX0, int dY0, int dX1, int dY1)
{
	return int(sqrt((dX1 - dX0)*(dX1 - dX0) + (dY1 - dY0)*(dY1 - dY0)));
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
	}
	return;
}

void dropThing(gmThing* owner, gmThing** target)
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
	}
	return;
}

//TakeTurns
void takeTurn_Test(gmThing* owner)//Flashes red and yellow. Test.
{
	if (owner->imgPath == "res/test4.png")
	{
		owner->imgPath = "res/test3.png";
		owner->load();
	}
	else
	{
		owner->imgPath = "res/test4.png";
		owner->load();
	}
	return;
}

void takeTurn_Follow(gmThing* owner)//Follows the target.
{
	int dx, dy;
	dx = owner->x - owner->target->x < 0 ? 1 : -1;
	dx = owner->x - owner->target->x == 0 ? 0 : dx;

	dy = owner->y - owner->target->y < 0 ? 1 : -1;
	dy = owner->y - owner->target->y == 0 ? 0 : dy;
	if (getThingByLocation(owner->x + dx, owner->y + dy) != NULL && getThingByLocation(owner->x + dx, owner->y + dy)->isPassable == false)
	{
		owner->attack(owner, getThingByLocation(owner->x + dx, owner->y + dy));
	}
	else if (owner->x + dx >= 0 && owner->y + dy >= 0 && currentMap->map[owner->y + dy][owner->x + dx]->passable && getThingByLocation(owner->x + dx, owner->y + dy) == NULL)
	{
		owner->step(dx, dy);
	}
	return;
}

void takeTurn_Random(gmThing* owner)//Walk around randomly.
{
	int dx = rand() % 3 - 1;
	int dy = rand() % 3 - 1;
	if (owner->x + dx >= 0 && owner->y + dy >= 0 //Owner is within the map dimensions.
		&& owner->x + dx < currentMap->map[0].size() && owner->y + dy < currentMap->map.size()
		&& currentMap->map[owner->y + dy][owner->x + dx]->passable //The tile is passable.
		&& getThingByLocation(owner->x + dx, owner->y + dy) == NULL) //And there is nothing there.
	{
		owner->step(dx, dy);
	}
	return;
}

//GetTargets
void getTarget_Player(gmThing* owner)//Target the player.
{
	owner->target = player;
	return;
}

//Attacks
void attack_Normal(gmThing* owner, gmThing* target)//Default attack.
{
	target->curHp -= roll(1, 'd', 6);
	return;
}

//Deaths
void death_Normal(gmThing* owner)//Default death.
{
	for (int i = 0; i < owner->inventory.size(); i++)
	{
		owner->inventory[i]->x = owner->x;
		owner->inventory[i]->y = owner->y;
		currentMap->things.insert(currentMap->things.end(), owner->inventory[i]);
	}
	return;
}

void death_Explode(gmThing* owner)//Deals damage to those in the vicinity, both friend and foe.
{
	for (int i = 0; i < currentMap->things.size(); i++)
	{
		distance(owner->x, owner->y, currentMap->things[i]->x, currentMap->things[i]->y) < 3 ? currentMap->things[i]->curHp -= 12 : NULL;
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
}

gmMap testMap(100, 100, mapMake_CellularAutomata);
gmMap testMap2(50, 100, mapMake_Random);
gmAi kekAi(takeTurn_Random, getTarget_Player);

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
		messageLog.insert(messageLog.end(), "The quick brown fox jumped over the lazy dog.");
		background = load_surface("res/background-alternate.png");
		ground.load();
		wall.load();
	}

	fov_settings_init(&fovSettings);
	fov_settings_set_opacity_test_function(&fovSettings, opaque);
	fov_settings_set_apply_lighting_function(&fovSettings, applyFov);

	//Set up test environment
	testMap.make(&testMap);
	testMap2.make(&testMap2);
	currentMap = &testMap;

	currentMap->things.insert(currentMap->things.end(), new gmThing("testPlayer", "res/test.png"));
	currentMap->things[0]->load();
	currentMap->things[0]->x = currentMap->map[0].size() / 2;
	currentMap->things[0]->y = currentMap->map.size() / 2;	
	currentMap->things[0]->ai = &kekAi;
	currentMap->things[0]->death = death_Normal;
	currentMap->things[0]->attack = attack_Normal;
	currentMap->things[0]->curHp = 12;
	currentMap->things[0]->viewRadius = 4;

	currentMap->things.insert(currentMap->things.end(), new gmThing("testNPC", "res/test4.png", false, &kekAi));
	currentMap->things[1]->load();
	currentMap->things[1]->x = currentMap->map[0].size() / 2;
	currentMap->things[1]->y = currentMap->map.size() / 2;
	currentMap->things[1]->death = death_Normal;
	currentMap->things[1]->attack = attack_Normal;
	currentMap->things[1]->curHp = 12;
	currentMap->things[1]->viewRadius = 8;

	currentMap->things.insert(currentMap->things.end(), new gmThing("testNPC", "res/test4.png", false, &kekAi));
	currentMap->things[2]->load();
	currentMap->things[2]->x = currentMap->map[0].size() / 2;
	currentMap->things[2]->y = currentMap->map.size() / 2;
	currentMap->things[2]->death = death_Normal;
	currentMap->things[2]->attack = attack_Normal;
	currentMap->things[2]->curHp = 12;
	currentMap->things[2]->viewRadius = 8;

	currentMap->things.insert(currentMap->things.end(), new gmThing("testNPC", "res/test4.png", false, &kekAi));
	currentMap->things[3]->load();
	currentMap->things[3]->x = currentMap->map[0].size() / 2;
	currentMap->things[3]->y = currentMap->map.size() / 2;
	currentMap->things[3]->death = death_Normal;
	currentMap->things[3]->attack = attack_Normal;
	currentMap->things[3]->curHp = 12;
	currentMap->things[3]->viewRadius = 8;

	currentMap->things.insert(currentMap->things.end(), new gmThing("testItem", "res/test.png", true, NULL));
	currentMap->things[4]->load();
	currentMap->things[4]->x = currentMap->map[0].size() / 2 + 1;
	currentMap->things[4]->y = currentMap->map.size() / 2;
	currentMap->things[4]->death = NULL;
	currentMap->things[4]->attack = NULL;
	currentMap->things[4]->curHp = 1;
	currentMap->things[4]->viewRadius = 0;

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
					case SDLK_UP:
						if (getThingByLocation(player->x, player->y - 1) != NULL && getThingByLocation(player->x, player->y - 1)->isPassable == false)
						{
							if (player->attack){ player->attack(player, getThingByLocation(player->x, player->y - 1)); }
						}
						else if (getCanMove(player->x, player->y-1))//Uses data from the map to determine whether the tile can be walked onto.
						{
							player->step(0, -1);//Step in the direction.
							camX = player->x - SCREEN_WIDTH / 60;
							camY = player->y - SCREEN_HEIGHT / 60;
						}
						hasMoved = true;//The player has moved.
						break;
					case SDLK_DOWN:
						if (getThingByLocation(player->x, player->y + 1) != NULL && getThingByLocation(player->x, player->y + 1)->isPassable == false)
						{
							if (player->attack){ player->attack(player, getThingByLocation(player->x, player->y + 1)); }
						}
						else if (getCanMove(player->x, player->y+1))
						{
							player->step(0, 1);
							camX = player->x - SCREEN_WIDTH / 60;
							camY = player->y - SCREEN_HEIGHT / 60;
						}
						hasMoved = true;
						break;
					case SDLK_RIGHT:
						if (getThingByLocation(player->x+1, player->y) != NULL && getThingByLocation(player->x+1, player->y)->isPassable == false)
						{
							if (player->attack){ player->attack(player, getThingByLocation(player->x + 1, player->y)); }
						}
						if (getCanMove(player->x+1, player->y))
						{
							player->step(1, 0);
							camX = player->x - SCREEN_WIDTH / 60;
							camY = player->y - SCREEN_HEIGHT / 60;
						}
						hasMoved = true;
						break;
					case SDLK_LEFT:
						if (getThingByLocation(player->x-1, player->y) != NULL && getThingByLocation(player->x-1, player->y)->isPassable == false)
						{
							if (player->attack){ player->attack(player, getThingByLocation(player->x - 1, player->y)); }
						}
						if (getCanMove(player->x-1, player->y))
						{
							player->step(-1, 0);
							camX = player->x - SCREEN_WIDTH / 60;
							camY = player->y - SCREEN_HEIGHT / 60;
						}
						hasMoved = true;
						break;
					case SDLK_g:
						if (getThingByLocation(player->x, player->y, currentMap, player))
						{
							player->inventory.insert(player->inventory.end(), getThingByLocation(player->x, player->y, currentMap, player));
							for (int i = 0; i < currentMap->things.size(); i++){ if (currentMap->things[i] == getThingByLocation(player->x, player->y, currentMap, player)){ index = i; break; } }
							messageLog.insert(messageLog.end(), "Picked up: "+currentMap->things[index]->name);
							currentMap->things.erase(currentMap->things.begin() + index);
						}
						break;
					case SDLK_d:
						sidePanel.options = getNames(player->inventory);
						dropThing(player, &player->inventory[sidePanel.selectFromList()]);
						break;
					case SDLK_i:
						sidePanel.options = getNames(player->inventory);
						sidePanel.selectFromList();
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
							messageLog.insert(messageLog.end(), "The quick brown fox jumped over the lazy dog.");
						}
						break;
					case SDLK_5:
						if (DEBUG)
						{
							sidePanel.currentSelection = NOT_SELECTED;
							sidePanel.finalSelection = NOT_SELECTED;
							sidePanel.options = std::vector <std::string>(7, "test");
							gameState = SELECTING_FROM_LIST;
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
			for (int i = 0; i < currentMap->things.size(); i++)
			{
				//If the thing has died.
				if (currentMap->things[i]->curHp <= 0)
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
			if (x == currentMap->fovMap[y].size()-1){x = 0; y++;}
		}
		//Recalculate the new fovMap.
		doFov(player->x, player->y, player->viewRadius, currentMap);
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
		std::vector <std::string> itemNames;
		itemNames = std::vector <std::string>();
		for (int i = 0; i < player->inventory.size(); i++){ itemNames.insert(itemNames.end(), player->inventory[i]->name); }
		renderMessageLog();
		sidePanel.show();
		//renderSidePanel(itemNames);
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