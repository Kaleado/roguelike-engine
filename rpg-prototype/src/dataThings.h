#ifndef DATATHINGS_H
#define DATATHINGS_H

/*This file is used to store essentially a 'database' about all of the different
 *things which may be spawned. I hope to eventually move this all to external
 *files so that things may be more easily created, but I don't want the headache
 *of making a parser for the files.
*/

//Uses / tosses
void use_Explode(gmThing* owner, gmThing* target, int x, int y, gmMap* map)
{
	SDL_Surface* blast = IMG_Load("res/blast-red.png");
	for (int i = 0; i < map->things.size(); i++)
	{
		if (distance(x, y, map->things[i]->x, map->things[i]->y) <= 1)
		{
			map->things[i]->curHp -= roll(25, 'd', 2);
		}
	}
	if (map == currentMap)
	{
		for (int dx = -1; dx < 2; dx++){
			for (int dy = -1; dy < 2; dy++){
				apply(30 * (x + dx - camX), 30 * (y + dy - camY), blast, screen);
				if (x + dx >= 0 && y + dy >= 0 && x + dx < map->map[0].size() && y + dy < map->map.size())
				{
					map->map[y + dy][x + dx] = &ground;
				}
			}
		}
		SDL_UpdateWindowSurface(gWindow);
		SDL_Delay(100);
	}
	return;
}

void use_Dig(gmThing* owner, gmThing* target, int x, int y, gmMap* map)
{
	messageLog.log.push_back("Which tile will you dig?");
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
	if (owner->x + dx >= 0 && owner->x + dx < map->map[0].size() && owner->y + dy >= 0 && owner->y + dy < map->map[0].size())
	{
		map->map[owner->y + dy][owner->x + dx] = &ground;
	}
	return;
}

void use_MakeTerrain(gmThing* owner, gmThing* target, int x, int y, gmMap* map)
{
	messageLog.log.push_back("Which tile will you convert?");
	messageLog.log.push_back("Converting to : wall.");
	messageLog.show();
	SDL_UpdateWindowSurface(gWindow);
	SDL_Event e;
	gmTile* convertTo = &wall;
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
				case SDLK_TAB:
					convertTo = convertTo == &wall? &door : &wall;
					messageLog.log.push_back(convertTo == &wall ? "Converting to : wall." : "Converting to: door.");
					messageLog.show();
					SDL_UpdateWindowSurface(gWindow);
					break;
				}
			}
		}
	}
	if (owner->x + dx >= 0 && owner->x + dx < map->map[0].size() && owner->y + dy >= 0 && owner->y + dy < map->map[0].size())
	{
		map->map[owner->y + dy][owner->x + dx] = convertTo;
	}
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
	SDL_Surface* blast = IMG_Load("res/blast-red.png");
	for (int i = 0; i < currentMap->things.size(); i++)
	{
		if (distance(owner->x, owner->y, currentMap->things[i]->x, currentMap->things[i]->y) <= 1)
		{
			currentMap->things[i]->curHp -= roll(10,'d',5);
		}
	}
	for (int x = -1; x < 2; x++){for (int y = -1; y < 2; y++){
		apply(30 * (owner->x + x - camX), 30 * (owner->y + y - camY), blast, screen);
		if (owner->x + x > 0 && owner->y + y > 0 && owner->x + x < currentMap->map[0].size() && owner->y + y < currentMap->map.size())
		{
			currentMap->map[owner->y + y][owner->x + x] = &ground;
		}
	} }
	SDL_UpdateWindowSurface(gWindow);
	SDL_Delay(100);
	return;
}

//Chats
void sayMessage(gmThing* owner, gmThing* target, std::vector <std::string> dialogue)
{
	for (int i = 0; i < dialogue.size(); i++)
	{
		messageLog.log.insert(messageLog.log.end(), dialogue[i]);
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

void takeTurn_AstarToTarget(gmThing* owner)//(pseudo) A* pathfind to the target. Attacks the target.
{
	if (owner->target)
	{
		int sx, sy;
		struct coords { int x, y, f, g, h; coords* parent; coords(int x, int y, coords* parent){ this->x = x; this->y = y; this->parent = parent; } };
		coords dest(owner->target->x, owner->target->y, NULL);
		coords a(owner->x, owner->y, NULL);
		coords* curTile = &a;
		coords* minTile;
		std::vector <coords> openList;
		for (int dx = -1; dx < 2; dx++){for (int dy = -1; dy < 2; dy++)
		{
				//if (getCanMove(curTile->x + dx, curTile->y + dy) && (dx != 0 || dy != 0))
			if (((getThingByLocation(curTile->x + dx, curTile->y + dy) == NULL ||
				getThingByLocation(curTile->x + dx, curTile->y + dy) == owner->target ||
				getThingByLocation(curTile->x + dx, curTile->y + dy)->isPassable)) && 
				curTile->y + dy > 0 && curTile->x + dx > 0 &&
				curTile->y + dy <  currentMap->map.size() && curTile->x + dx < currentMap->map[0].size() &&
				currentMap->map[curTile->y + dy][curTile->x + dx]->passable && (dx != 0 || dy != 0))
			{ 
				openList.push_back(coords(curTile->x + dx, curTile->y + dy, curTile));
				if (dx != 0 && dy != 0){ openList.back().g = 14; } else{ openList.back().g = 10; }
				openList.back().h = 10 * (abs(curTile->y + dy - dest.y) + abs(curTile->x + dx - dest.x));
				openList.back().f = openList.back().g + openList.back().h;
			}
		}}
		if (openList.size() > 0)
		{	
			minTile = &openList[0];
			for (int i = 0; i < openList.size(); i++)
			{
				if (openList[i].f < minTile->f){ minTile = &openList[i]; }
			}
			sx = minTile->x - owner->x;
			sy = minTile->y - owner->y;
			if (getThingByLocation(owner->x + sx, owner->y + sy) != NULL && getThingByLocation(owner->x + sx, owner->y + sy)->isPassable == false)
			{
				if (owner->attack){ owner->attack(owner, getThingByLocation(owner->x + sx, owner->y + sy)); }
			}
			else if (getCanMove(owner->x+sx, owner->y+sy))
			{
				owner->step(sx, sy);
			}	
		}
	}
	else
	{
		takeTurn_Random(owner);
	}
	return;
}

void takeTurn_keepDistance(gmThing* owner)
{
	if (owner->target)
	{
		if (distance(owner->x, owner->y, owner->target->x, owner->target->y) < owner->viewRadius - 2)
		{
			int dx = owner->x - owner->target->x;
			int dy = owner->y - owner->target->y;
			dx = dx > 0 ? 1 : dx;
			dx = dx < 0 ? -1 : dx;
			dy = dy > 0 ? 1 : dy;
			dy = dy < 0 ? -1 : dy;
			if (getCanMove(owner->x+dx, owner->y+dy))
			{
				owner->step(dx, dy);
			}
		}
		else
		{
			if (roll(1, 'd', 3) >= 2)
			{
				takeTurn_AstarToTarget(owner);
			}
			else
			{
				if (distance(owner->x, owner->y, owner->target->x, owner->target->y) < owner->viewRadius - 1)
				{
					owner->attack(owner, owner->target);
				}
			}
		}
	}
	return;
}

//GetTargets
void getTarget_None(gmThing* owner)//Target nothing.
{
	owner->target = NULL;
	return;
}

void getTarget_Player(gmThing* owner)//Target the player.
{
	if (distance(owner->x, owner->y, player->x, player->y) <= owner->viewRadius)
	{
		owner->target = player;
	}
	else
	{
		owner->target = NULL;
	}
	return;
}

//Attacks
void attack_Normal(gmThing* owner, gmThing* target)//Default attack.
{
	owner->target = target;//Temporary, to check monster status sidePanel option.
	if (owner->bodyParts[0]->equippedThing &&
		owner->bodyParts[0]->equippedThing->equippedStats &&
		owner->bodyParts[0]->equippedThing->equippedStats->attack)
	{
		owner->bodyParts[0]->equippedThing->equippedStats->attack(owner, target);
	}
	else
	{
		target->curHp -= roll(1 * (1 + owner->str / 10), 'd', 6);
	}
	return;
}

void attack_spellFireDart(gmThing* owner, gmThing* target)
{
	SDL_Surface* blast = IMG_Load("res/blast-red.png");
	if (isTrajectoryBlocked(owner->x, owner->y, target->x, target->y, 4, currentMap) == false)
	{
		apply(30 * (target->x - camX), 30*(target->y - camY), blast, screen);
		SDL_UpdateWindowSurface(gWindow);
		SDL_Delay(100);
		target->curHp -= roll(1*(1+owner->itl/10),'d', owner->itl/2);
	}
	return;
}

gmAi ai_Default(takeTurn_Random, getTarget_None);
gmAi ai_Aggressive(takeTurn_AstarToTarget, getTarget_Player);
gmAi ai_Caster(takeTurn_keepDistance, getTarget_Player);

gmBodyPart finger("finger");
gmBodyPart hand("hand");
gmBodyPart head("head");
gmBodyPart legs("legs");
gmBodyPart neck("neck");
gmBodyPart body("body");
gmBodyPart waist("waist");
gmBodyPart wing("wing");
gmBodyPart tail("tail");

gmThing thing_Pickaxe(
	"Pickaxe",
	true,
	true,
	"res/pickaxe.png",
	new gmEquipment(gmDice(1, 4, 0), 0, 0, 0, 0, 0, 0, 0),
	NULL,
	NULL,
	NULL,
	NULL, use_Dig, NULL,
	0, 0, 0, 0, 0, 0, 0, 10,
	{},
	{},
	NULL
	);

gmThing thing_ConstructionKit(
	"Construction kit",
	true,
	true,
	"res/construction-kit.png",
	NULL,
	NULL,
	NULL,
	NULL,
	NULL, use_MakeTerrain, NULL,
	0, 0, 0, 0, 0, 0, 0, 10,
	{},
	{},
	NULL
	);

gmThing thing_Imp(
	"Imp",
	false,
	false,
	"res/imp.png",
	NULL, NULL,
	death_Normal,
	attack_spellFireDart,
	NULL, NULL, NULL,
	10, 10, 10, 10, 10, 10, 10, 25,
	{
		new gmBodyPart(head),
		new gmBodyPart(neck),
		new gmBodyPart(wing),
		new gmBodyPart(wing),
		new gmBodyPart(hand),
		new gmBodyPart(hand),
		new gmBodyPart(waist),
		new gmBodyPart(legs)
	},
	{},
	&ai_Caster
	);

gmThing thing_Myconid(
	"Myconid",
	false,
	false,
	"res/myconid.png",
	NULL, NULL,
	death_Normal,
	attack_Normal,
	NULL, NULL, NULL,
	10, 10, 10, 10, 10, 10, 3, 35,
	{ new gmBodyPart(hand),
	new gmBodyPart(hand),
	new gmBodyPart(head),
	new gmBodyPart(neck),
	new gmBodyPart(body),
	new gmBodyPart(legs)
	},
	{},
	&ai_Aggressive
);

gmThing thing_Bomber(
	"Bomber",
	false,
	false,
	"res/bomb.png",
	NULL, NULL,
	death_Explode,
	attack_Normal,
	NULL, use_Explode, use_Explode,
	1, 1, 1, 1, 1, 1, 3, 15,
	{new gmBodyPart(body)},
	{},
	&ai_Aggressive
);

gmThing thing_Player(
	"Player",
	false,
	false,
	"res/player.png",
	NULL, NULL,
	death_Normal,
	attack_Normal,
	NULL, NULL, NULL,
	10, 10, 10, 10, 10, 10, 7, 0,
	{ new gmBodyPart(hand), 
	new gmBodyPart(hand),
	new gmBodyPart(head),
	new gmBodyPart(neck),
	new gmBodyPart(body),
	new gmBodyPart(legs)
	},
	{ new gmThing(thing_Pickaxe), new gmThing(thing_ConstructionKit) },
	&ai_Default
);

#endif