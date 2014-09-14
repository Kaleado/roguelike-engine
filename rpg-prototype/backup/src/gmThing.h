#ifndef GMTHING_H
#define GMTHING_H

#include "gmRender.h"
#include "gmBodyPart.h"
#include "gmEquipment.h"

/*Things have alignments, be they objects or characters. Capped at 100.*/
#define MAJOR_EVIL		-80
#define MINOR_EVIL		-30
//#define NEUTRAL		0
#define MINOR_GOOD		30
#define MAJOR_GOOD		80

#define MAJOR_CHAOS		-80
#define MINOR_CHAOS		-30
#define NEUTRAL			0
#define MINOR_LAW		30
#define MAJOR_LAW		80

class gmAi;
class gmEquipment;
class gmBodyPart;
class gmMap;

class gmThing
{
public:
	signed int goodRating = 0;	//The good/evil rating of the thing.
	signed int lawRating = 0;	//The chaotic/lawful rating of the thing.
	gmEquipment* equippedStats = new gmEquipment(gmDice(1,2,0),0,0,0,0,0,0,0);
	gmThing* target = NULL;
	SDL_Surface* img = NULL;
	std::string imgPath;
	std::string name;
	int volume;//Volume and weight could be used for items, or even pets which can be placed in the inventory and dropped when desired.
	//int weight;
	std::vector <gmThing*> inventory;
	std::vector <gmBodyPart*> bodyParts;
	int x, y;
	int str, dex, con, itl, cha, per;
	int viewRadius;
	int rarity;//Rare things spawn less often! From 0 to 100. 0 = never spawns, 100 = always spawns.
	int getStrMod(){ int total = 0; for (int i = 0; i < bodyParts.size(); i++){ if (bodyParts[i]->equippedThing){ total += bodyParts[i]->equippedThing->equippedStats->strMod; } } return total; };
	int getDexMod(){ int total = 0; for (int i = 0; i < bodyParts.size(); i++){ if (bodyParts[i]->equippedThing){ total += bodyParts[i]->equippedThing->equippedStats->dexMod; } } return total; };
	int getConMod(){ int total = 0; for (int i = 0; i < bodyParts.size(); i++){ if (bodyParts[i]->equippedThing){ total += bodyParts[i]->equippedThing->equippedStats->conMod; } } return total; };
	int getItlMod(){ int total = 0; for (int i = 0; i < bodyParts.size(); i++){ if (bodyParts[i]->equippedThing){ total += bodyParts[i]->equippedThing->equippedStats->itlMod; } } return total; };
	int getChaMod(){ int total = 0; for (int i = 0; i < bodyParts.size(); i++){ if (bodyParts[i]->equippedThing){ total += bodyParts[i]->equippedThing->equippedStats->chaMod; } } return total; };
	int getPerMod(){ int total = 0; for (int i = 0; i < bodyParts.size(); i++){ if (bodyParts[i]->equippedThing){ total += bodyParts[i]->equippedThing->equippedStats->perMod; } } return total; };
	int ac, curHp, maxHp;
	int acMod, hpMod;
	bool isPassable;
	bool inanimate;
	void(*onStep)(gmThing* owner, gmThing* target, int x, int y, gmMap* map);
	void(*death)(gmThing* owner);
	void(*attack)(gmThing* owner, gmThing* target);
	void(*chat)(gmThing* owner, gmThing* target, std::vector <std::string> dialogue);
	void(*use)(gmThing* owner, gmThing* target, int x, int y, gmMap* map);
	void(*toss)(gmThing* owner, gmThing* target, int x, int y, gmMap* map);
	gmAi* ai;

	void step(int dx, int dy)
	{
		this->x += dx;
		this->y += dy;
	}

	void show()
	{
		if (img){ apply((x - camX) * 30, (y - camY) * 30, img, screen); }
	}

	bool load()
	{
		this->img = IMG_Load(imgPath.c_str());
		if (img != NULL)
		{
			return true;
		}
		return false;
	}
	gmThing(
		std::string name,
		bool inanimate,
		bool isPassable,
		std::string imgPath,
		gmEquipment* equippedStats,
		void(*onStep)(gmThing* owner, gmThing* target, int x, int y, gmMap* map),
		void(*death)(gmThing* owner),
		void(*attack)(gmThing* owner, gmThing* target),
		void(*chat)(gmThing* owner, gmThing* target, std::vector <std::string> dialogue),
		void(*use)(gmThing* owner, gmThing* target, int x, int y, gmMap* map),
		void(*toss)(gmThing* owner, gmThing* target, int x, int y, gmMap* map),
		int str,
		int dex,
		int con,
		int itl,
		int cha,
		int per,
		int viewRadius, int rarity,
		std::vector <gmBodyPart*> bodyParts,
		std::vector <gmThing*> inventory,
		gmAi* ai
		)
	{
		this->name = name;
		this->inanimate = inanimate;
		this->isPassable = isPassable;
		this->imgPath = imgPath;
		this->equippedStats = equippedStats;
		this->onStep = onStep;
		this->death = death;
		this->attack = attack;
		this->chat = chat;
		this->use = use;
		this->toss = toss;
		this->str = str;
		this->dex = dex;
		this->con = con;
		this->itl = itl;
		this->cha = cha;
		this->per = per;
		this->viewRadius = viewRadius;
		this->rarity = rarity;
		this->ac = 2*con + dex;
		this->maxHp = 2*con + str;
		this->curHp = this->maxHp;
		this->bodyParts = bodyParts;
		this->inventory = inventory;
		this->ai = ai;
	}
};

gmThing* player;

#endif