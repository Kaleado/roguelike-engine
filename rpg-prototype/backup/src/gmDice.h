#ifndef GMDICE_H
#define GMDICE_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int roll(int num, char d, int die)
{
	return (rand() % (num*die)-num) + num;
}

class gmDice
{
public:
	signed int numOfDice, numOfSides, modifier;
	int roll()
	{
		return (rand() % (numOfDice*numOfSides) - numOfDice) + numOfDice;
	}
	gmDice(){}
	gmDice(
		int numOfDice,
		int numOfSides,
		int modifier
		)
	{
		this->numOfDice = numOfDice;
		this->numOfSides = numOfSides;
		this->modifier = modifier;
	}
};


#endif