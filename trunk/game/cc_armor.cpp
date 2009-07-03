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

bool CArmor::Pickup (edict_t *ent, CPlayerEntity *other)
{
	if (this->normalProtection == -1)
	{
		if (other->Client.pers.Armor == NULL)
		{
			other->Client.pers.Inventory.Set (FindItem("Jacket Armor"), 2);
			other->Client.pers.Armor = dynamic_cast<CArmor*>(FindItem("Jacket Armor"));
		}
		else
		{
			if (this->maxCount != -1 && (other->Client.pers.Inventory.Has(other->Client.pers.Armor) >= this->maxCount))
				return false;

			other->Client.pers.Inventory.Add (other->Client.pers.Armor, 2);
			if (this->maxCount != -1 && (other->Client.pers.Inventory.Has(other->Client.pers.Armor) > this->maxCount))
				other->Client.pers.Inventory.Set(other->Client.pers.Armor, this->maxCount);
		}
		if (!(ent->spawnflags & DROPPED_ITEM) && (game.mode & GAME_DEATHMATCH))
			SetRespawn (ent, 20);
		return true;
	}

	if (other->Client.pers.Armor != NULL)
	{
		if (this->normalProtection > other->Client.pers.Armor->normalProtection)
		{
			// calc new armor values
			int newCount = this->baseCount + ((other->Client.pers.Armor->normalProtection / this->normalProtection) * other->Client.pers.Inventory.Has(other->Client.pers.Armor));
			if (newCount > this->maxCount)
				newCount = this->maxCount;

			// zero count of old armor so it goes away
			other->Client.pers.Inventory.Set(other->Client.pers.Armor, 0);

			// change armor to new item with computed value
			other->Client.pers.Inventory.Set(this, newCount);
			other->Client.pers.Armor = this;
		}
		else
		{
			// calc new armor values
			int newCount = other->Client.pers.Inventory.Has(other->Client.pers.Armor) + ((this->normalProtection / other->Client.pers.Armor->normalProtection) * this->baseCount);
			if (newCount > other->Client.pers.Armor->maxCount)
				newCount = other->Client.pers.Armor->maxCount;

			// if we're already maxed out then we don't need the new armor
			if (other->Client.pers.Inventory.Has(other->Client.pers.Armor) >= newCount)
				return false;

			// update current armor value
			other->Client.pers.Inventory.Set(other->Client.pers.Armor, newCount);
		}
	}
	// Player has no other armor, just use it
	else
	{
		other->Client.pers.Armor = this;
		other->Client.pers.Inventory.Set(this, this->baseCount);
	}

	if (!(ent->spawnflags & DROPPED_ITEM) && (game.mode & GAME_DEATHMATCH))
		SetRespawn (ent, 20);

	return true;
}

// No dropping or using armor.
void CArmor::Use(CPlayerEntity *ent)
{
}

void CArmor::Drop (CPlayerEntity *ent)
{
}

int CArmor::CheckArmor (CPlayerEntity *Player, vec3_t point, vec3_t normal, int damage, int dflags)
{
	if (!damage)
		return 0;
	if (dflags & DAMAGE_NO_ARMOR)
		return 0;

	int save = ceil (((dflags & DAMAGE_ENERGY) ? ((float)energyProtection / 100) : ((float)normalProtection / 100)) * damage);
	if (save >= Player->Client.pers.Inventory.Has(this))
		save = Player->Client.pers.Inventory.Has(this);

	if (!save)
		return 0;

	Player->Client.pers.Inventory.Remove(GetIndex(), save);
	CTempEnt_Splashes::Sparks (point, normal, (dflags & DAMAGE_BULLET) ? CTempEnt_Splashes::STBulletSparks : CTempEnt_Splashes::STSparks, CTempEnt_Splashes::SPTSparks);

	// Ran out of armor?
	if (!Player->Client.pers.Inventory.Has(this))
		Player->Client.pers.Armor = NULL;

	return save;
}

void AddArmorToList ()
{
	CArmor *JacketArmor = QNew (com_gamePool, 0) CArmor ("item_armor_jacket", "models/items/armor/jacket/tris.md2", EF_ROTATE, "misc/ar1_pkup.wav", "i_jacketarmor", "Jacket Armor", ITEMFLAG_GRABBABLE|ITEMFLAG_ARMOR, "", 25, 50, 30, 00);
	CArmor *CombatArmor = QNew (com_gamePool, 0) CArmor ("item_armor_combat", "models/items/armor/combat/tris.md2", EF_ROTATE, "misc/ar1_pkup.wav", "i_combatarmor", "Combat Armor", ITEMFLAG_GRABBABLE|ITEMFLAG_ARMOR, "", 50, 100, 60, 30);
	CArmor *BodyArmor = QNew (com_gamePool, 0) CArmor ("item_armor_body", "models/items/armor/body/tris.md2", EF_ROTATE, "misc/ar1_pkup.wav", "i_bodyarmor", "Body Armor", ITEMFLAG_GRABBABLE|ITEMFLAG_ARMOR, "", 100, 200, 80, 60);
	CArmor *ArmorShard = QNew (com_gamePool, 0) CArmor ("item_armor_shard", "models/items/armor/shard/tris.md2", EF_ROTATE, "misc/ar2_pkup.wav", "i_jacketarmor", "Armor Shard", ITEMFLAG_GRABBABLE|ITEMFLAG_ARMOR, "", 2, -1, -1, -1);

	ItemList->AddItemToList (JacketArmor);
	ItemList->AddItemToList (CombatArmor);
	ItemList->AddItemToList (BodyArmor);
	ItemList->AddItemToList (ArmorShard);
}