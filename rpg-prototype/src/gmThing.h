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
class gmMaterial;

class gmThing
{
public:
	signed int goodRating = 0;	//The good/evil rating of the thing.
	signed int lawRating = 0;	//The chaotic/lawful rating of the thing.
	gmMaterial* material;		//The substance the thing is made from.
	gmEquipment* equippedStats = new gmEquipment(gmDice(1,2,0),0,0,0,0,0,0,0);//The statistic the thing grants when worn in the hand as a weapon.
	gmThing* target = NULL;//The thing the thing is targeting.
	SDL_Surface* img = NULL;//The thing's sprite.
	std::string imgPath;//The path to the thing's sprite.
	std::string name;//The thing's name, as displayed by the game.
	int volume;//Volume and weight could be used for items, or even pets which can be placed in the inventory and dropped when desired.
	//int weight;
	std::vector <gmThing*> inventory;//The items the thing carries or contains.
	std::vector <gmBodyPart*> bodyParts;//The body parts the thing has: objects generally have none.
	int x, y;//The thing's position.
	int str, dex, con, itl, cha, per;//The thing's stats.
	int viewRadius;//The number of tiles the thing can see away.
	int rarity;//Rare things spawn less often! From 0 to 100. 0 = never spawns, 100 = always spawns.
	//Methods to get the bonuses granted by equipment and such.
	int getStrMod(){ int total = 0; for (int i = 0; i < bodyParts.size(); i++){ if (bodyParts[i]->equippedThing && bodyParts[i]->equippedThing->equippedStats){ total += bodyParts[i]->equippedThing->equippedStats->strMod; } } return total; };
	int getDexMod(){ int total = 0; for (int i = 0; i < bodyParts.size(); i++){ if (bodyParts[i]->equippedThing && bodyParts[i]->equippedThing->equippedStats){ total += bodyParts[i]->equippedThing->equippedStats->dexMod; } } return total; };
	int getConMod(){ int total = 0; for (int i = 0; i < bodyParts.size(); i++){ if (bodyParts[i]->equippedThing && bodyParts[i]->equippedThing->equippedStats){ total += bodyParts[i]->equippedThing->equippedStats->conMod; } } return total; };
	int getItlMod(){ int total = 0; for (int i = 0; i < bodyParts.size(); i++){ if (bodyParts[i]->equippedThing && bodyParts[i]->equippedThing->equippedStats){ total += bodyParts[i]->equippedThing->equippedStats->itlMod; } } return total; };
	int getChaMod(){ int total = 0; for (int i = 0; i < bodyParts.size(); i++){ if (bodyParts[i]->equippedThing && bodyParts[i]->equippedThing->equippedStats){ total += bodyParts[i]->equippedThing->equippedStats->chaMod; } } return total; };
	int getPerMod(){ int total = 0; for (int i = 0; i < bodyParts.size(); i++){ if (bodyParts[i]->equippedThing && bodyParts[i]->equippedThing->equippedStats){ total += bodyParts[i]->equippedThing->equippedStats->perMod; } } return total; };
	int ac, curHp, maxHp;//The thing's AC (how hard to hit), hp, etc.
	int acMod, hpMod;//Probably useless, should be replaced with the methods above.
	bool isPassable;//Whether the thing can be walked through.
	bool inanimate;//Whether the thing is animate. Inanimate is for objects.
	void(*onStep)(gmThing* owner, gmThing* target, int x, int y, gmMap* map);//Function performed when somethign steps on (or tries to) this thing.
	void(*death)(gmThing* owner);//Function performed when the thing's HP is reduced to zero.
	void(*attack)(gmThing* owner, gmThing* target);//Function performed when the thing attacks. If the thing has a weapon, it will use that function instead.
	void(*chat)(gmThing* owner, gmThing* target, std::vector <std::string> dialogue);//What the thing says when spoken to. Unimplemented.
	void(*use)(gmThing* owner, gmThing* target, int x, int y, gmMap* map);//The function run when the thing is used.
	void(*toss)(gmThing* owner, gmThing* target, int x, int y, gmMap* map);//The function run when the thing is thrown.
	gmAi* ai;//The thing's AI.
	int getStatTotal(){ return(str+dex+con+itl+cha+per); }//Returns the sum of all of the thing's stats, for when calculating danger, etc.
	void step(int dx, int dy)//Used for when the thing walks.
	{
		this->x += dx;
		this->y += dy;
	}

	void show()//Shows the thing on the screen.
	{
		if (img){ apply((x - camX) * 30, (y - camY) * 30, img, screen); }
	}

	bool load()//Loads the thing's image: run when the thing is introduced to the game for the first time.
	{
		this->img = IMG_Load(imgPath.c_str());
		if (img != NULL)
		{
			return true;
		}
		return false;
	}
	//Constructor.
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

gmThing* player;//Pointer to the player: can be changed to ANYTHING.

#endif