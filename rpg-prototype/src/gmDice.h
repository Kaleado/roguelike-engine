#ifndef GMDICE_H
#define GMDICE_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int roll(int num, char d, int die)
{
	return (rand() % (num*die)-num) + num;
}

#endif