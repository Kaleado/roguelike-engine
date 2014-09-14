#ifndef GMEQUIPMENT_H
#define GMEQUIPMENT_H

#include "gmDice.h"

class gmThing;

class gmEquipment
{
public:
	//gmThing* owner;
	void(*attack)(gmThing* owner, gmThing* target);
	int strMod, dexMod, conMod, itlMod, chaMod, perMod, acMod, maxHpMod;
	gmDice damageDice = gmDice(1,2,0);
	gmEquipment(gmDice damageDice, int strMod = 0, int dexMod = 0, int itlMod = 0, int chaMod = 0, int perMod = 0, int acMod = 0, int maxHpMod = 0,
		void(*attack)(gmThing* owner, gmThing* target) = NULL)
	{	
		this->strMod = strMod;
		this->dexMod = dexMod;
		this->itlMod = itlMod;
		this->chaMod = chaMod;
		this->perMod = perMod;
		this->acMod = acMod;
		this->maxHpMod = maxHpMod;
		this->damageDice = damageDice;
		this->attack = attack;
	}
};

#endif