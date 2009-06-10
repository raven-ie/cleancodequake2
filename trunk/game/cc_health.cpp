/*
Copyright (C) 1997-2001 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
/*
This source file is contained as part of CleanCode Quake2, a project maintained
by Paril, to 'clean up' and make Quake2 an easier source base to read and work with.

You may use any part of this code to help create your own bases and own mods off
this code if you wish. It is under the same license as Quake 2 source (as above),
therefore you are free to have to fun with it. All I ask is you email me so I can
list the mod on my page for CleanCode Quake2 to help get the word around. Thanks.
*/

//
// cc_health.cpp
// New, improved, better, stable item system!
//

#include "cc_local.h"

CHealth::CHealth (char *Classname, char *WorldModel, int EffectFlags,
			   char *PickupSound, char *Icon, char *Name, EItemFlags Flags,
			   char *Precache, int Amount, EHealthFlags HealthFlags) :
CBaseItem (Classname, WorldModel, EffectFlags, PickupSound, Icon, Name, Flags,
		   Precache),
Amount(Amount),
HealthFlags(HealthFlags)
{
};

bool CHealth::Pickup (edict_t *ent, CPlayerEntity *other)
{
	if (!(this->HealthFlags & HEALTHFLAG_IGNOREMAX) && (other->gameEntity->health >= other->gameEntity->max_health))
		return false;

#ifdef CLEANCTF_ENABLED
//ZOID
	if (other->gameEntity->health >= 250 && ent->count > 25)
		return false;
//ZOID
#endif

	other->gameEntity->health += this->Amount;

#ifdef CLEANCTF_ENABLED
//ZOID
	if (other->gameEntity->health > 250 && ent->count > 25)
		other->gameEntity->health = 250;
//ZOID
#endif

	if (!(this->HealthFlags & HEALTHFLAG_IGNOREMAX))
	{
		if (other->gameEntity->health > other->gameEntity->max_health)
			other->gameEntity->health = other->gameEntity->max_health;
	}

	if (!(ent->spawnflags & DROPPED_ITEM) && (game.mode & GAME_DEATHMATCH))
		SetRespawn (ent, 30);

	return true;
}

void AddHealthToList ()
{
	CHealth *StimPack = new CHealth("item_health_small", "models/items/healing/stimpack/tris.md2", 0, "items/s_health.wav", "i_health", "Stimpack", ITEMFLAG_HEALTH|ITEMFLAG_GRABBABLE, "", 2, HEALTHFLAG_IGNOREMAX);
	CHealth *SmallHealth = new CHealth("item_health", "models/items/healing/medium/tris.md2", 0, "items/n_health.wav", "i_health", "Medium Health", ITEMFLAG_HEALTH|ITEMFLAG_GRABBABLE, "", 10, HEALTHFLAG_NONE);
	CHealth *LargeHealth = new CHealth("item_health_large", "models/items/healing/large/tris.md2", 0, "items/l_health.wav", "i_health", "Large Health", ITEMFLAG_HEALTH|ITEMFLAG_GRABBABLE, "", 25, HEALTHFLAG_NONE);

	ItemList->AddItemToList (StimPack);
	ItemList->AddItemToList (SmallHealth);
	ItemList->AddItemToList (LargeHealth);
}