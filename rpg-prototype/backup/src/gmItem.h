#ifndef GMITEM_H
#define GMITEM_H

class gmThing;

class gmItem
{
public:	
	void(*apply)(gmThing* owner, gmThing* target);
};

#endif