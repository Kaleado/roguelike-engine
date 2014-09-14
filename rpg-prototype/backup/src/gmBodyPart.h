#ifndef GMBODYPART_H
#define GMBODYPART_H

class gmThing;

class gmBodyPart
{
public:
	std::string name;
	bool vital; //The creature dies if it loses this part.
	gmThing* equippedThing;//The thing that is attached to this part, such as a sword, an amulet, etc.
	std::vector <gmBodyPart*> subParts;//Bodyparts which are attached to this body part, for things like fingers, etc.
	gmBodyPart(std::string name, std::vector <gmBodyPart*> subParts = std::vector <gmBodyPart*>())
	{
		this->name = name;
		this->subParts = subParts;
	}
};

#endif