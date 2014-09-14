#ifndef GMMATERIAL_H
#define GMMATERIAL_H

/*Class used for all the materials from which things and tiles are constructed from.
 *Ultimately, it would be expected that every thing and tile is given a material,
 *which would define how they behave when certain events occur, such as setting
 *them on fire or attacking them. It would also determine the resources produced when
 *the tile or thing is broken.
*/

class gmMaterial
{
public:
	bool isFiresafe;//Whether or not the material burns.
};

#endif