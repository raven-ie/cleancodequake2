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
// cc_armor.cpp
// New, improved, better, stable item system!
//

#include "cc_local.h"

CArmor::CArmor (char *Classname, char *WorldModel, int EffectFlags,
			   char *PickupSound, char *Icon, char *Name, EItemFlags Flags,
			   char *Precache, int baseCount, int maxCount, float normalProtection,
			   float energyProtection) :
CBaseItem(Classname, WorldModel, EffectFlags, PickupSound, Icon, Name, Flags,
		Precache),
baseCount(baseCount),
maxCount(maxCount),
normalProtection(normalProtection),
energyProtection(energyProtection)
{
};

bool CArmor::Pickup (edict_t *ent, edict_t *other)
{
	if (this->normalProtection == -1)
	{
		if (other->client->pers.Armor == NULL)
		{
			other->client->pers.Inventory.Set (FindItem("Jacket Armor"), 2);
			other->client->pers.Armor = dynamic_cast<CArmor*>(FindItem("Jacket Armor"));
		}
		else
		{
			if (this->maxCount != -1 && (other->client->pers.Inventory.Has(ent->client->pers.Armor) >= this->maxCount))
				return false;

			other->client->pers.Inventory.Add (other->client->pers.Armor, 2);
			if (this->maxCount != -1 && (other->client->pers.Inventory.Has(other->client->pers.Armor) > this->maxCount))
				other->client->pers.Inventory.Set(ent->client->pers.Armor, this->maxCount);
		}
		if (!(ent->spawnflags & DROPPED_ITEM) && (game.mode == GAME_DEATHMATCH))
			SetRespawn (ent, 20);
		return true;
	}

	if (other->client->pers.Armor != NULL)
	{
		if (this->normalProtection > other->client->pers.Armor->normalProtection)
		{
			// calc new armor values
			int newCount = this->baseCount + ((other->client->pers.Armor->normalProtection / this->normalProtection) * other->client->pers.Inventory.Has(other->client->pers.Armor));
			if (newCount > this->maxCount)
				newCount = this->maxCount;

			// zero count of old armor so it goes away
			other->client->pers.Inventory.Set(other->client->pers.Armor, 0);

			// change armor to new item with computed value
			other->client->pers.Inventory.Set(this, newCount);
			other->client->pers.Armor = this;
		}
		else
		{
			// calc new armor values
			int newCount = other->client->pers.Inventory.Has(other->client->pers.Armor) + ((this->normalProtection / other->client->pers.Armor->normalProtection) * this->baseCount);
			if (newCount > other->client->pers.Armor->maxCount)
				newCount = other->client->pers.Armor->maxCount;

			// if we're already maxed out then we don't need the new armor
			if (other->client->pers.Inventory.Has(other->client->pers.Armor) >= newCount)
				return false;

			// update current armor value
			other->client->pers.Inventory.Set(other->client->pers.Armor, newCount);
		}
	}
	// Player has no other armor, just use it
	else
	{
		other->client->pers.Armor = this;
		other->client->pers.Inventory.Set(this, this->baseCount);
	}

	if (!(ent->spawnflags & DROPPED_ITEM) && (game.mode == GAME_DEATHMATCH))
		SetRespawn (ent, 20);

	return true;
}

// No dropping or using armor.
void CArmor::Use(edict_t *ent)
{
}

void CArmor::Drop (edict_t *ent)
{
}

void AddArmorToList ()
{
	CArmor *JacketArmor = new CArmor ("item_armor_jacket", "models/items/armor/jacket/tris.md2", EF_ROTATE, "misc/ar1_pkup.wav", "i_jacketarmor", "Jacket Armor", ITEMFLAG_GRABBABLE|ITEMFLAG_ARMOR, "", 25, 50, .30f, .00f);
	CArmor *CombatArmor = new CArmor ("item_armor_combat", "models/items/armor/combat/tris.md2", EF_ROTATE, "misc/ar1_pkup.wav", "i_combatarmor", "Combat Armor", ITEMFLAG_GRABBABLE|ITEMFLAG_ARMOR, "", 50, 100, .60f, .30f);
	CArmor *BodyArmor = new CArmor ("item_armor_body", "models/items/armor/body/tris.md2", EF_ROTATE, "misc/ar1_pkup.wav", "i_bodyarmor", "Body Armor", ITEMFLAG_GRABBABLE|ITEMFLAG_ARMOR, "", 100, 200, .80f, .60f);
	CArmor *ArmorShard = new CArmor ("item_armor_shard", "models/items/armor/shard/tris.md2", EF_ROTATE, "misc/ar2_pkup.wav", "i_jacketarmor", "Armor Shard", ITEMFLAG_GRABBABLE|ITEMFLAG_ARMOR, "", 2, -1, -1, -1);

	ItemList->AddItemToList (JacketArmor);
	ItemList->AddItemToList (CombatArmor);
	ItemList->AddItemToList (BodyArmor);
	ItemList->AddItemToList (ArmorShard);
}