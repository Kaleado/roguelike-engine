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
	bool add(gmThing* thing)
	{
		if (contents.size()+1 >= unitCapacity){return false;}
		contents.insert(contents.end(), thing);
		return true;
	}
};

#endif