#ifndef GMCONTAINER_H
#define GMCONTAINER_H

#include <vector>

class gmThing;

class gmContainer
{
public:
	int volumeCapacity;
	int unitCapacity;
	//int weightCapacity;
	std::vector <gmThing*> contents;
};

#endif