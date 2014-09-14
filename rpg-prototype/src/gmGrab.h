#ifndef GMGRAB_H
#define GMGRAB_H

bool grabThing(gmThing* owner, gmThing* target, gmMap* map)
{
	int index = -1;
	if ( target->str + target->getStrMod() == 0 || rand() % owner->str + owner->getStrMod() >= rand() % target->str + target->getStrMod())
	{
		if (owner == player)
		{
			messageLog.log.push_back("You successfully pick up and carry the " + target->name + ".");
		}
		owner->inventory.push_back(target);
		for (int i = 0; i < map->things.size(); i++){ if (map->things[i] == target){ index = i; break; } }
		messageLog.log.push_back("Picked up: " + map->things[index]->name);
		map->things.erase(map->things.begin() + index);
	}
	else
	{
		if (owner == player)
		{
			messageLog.log.push_back("You fail to pick up and carry the " + target->name + ".");
		}
	}
	return false;
}

#endif