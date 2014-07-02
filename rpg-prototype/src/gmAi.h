#ifndef GMAI_H
#define GMAI_H

class gmThing;

class gmAi
{
public:
	gmThing* owner;
	void(*getTarget)(gmThing* owner);
	void(*takeTurn)(gmThing* owner);
	void(*attack)(gmThing* owner);
	gmAi::gmAi(void(*takeTurn)(gmThing* owner), void(*getTarget)(gmThing* owner) = NULL)
	{
		this->getTarget = getTarget;
		this->takeTurn = takeTurn;
	}
};

#endif