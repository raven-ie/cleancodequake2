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
// cc_items.cpp
// New, improved, better, stable item system!
// Crammed for now, split off into filter later.
//

#include "cc_local.h"

CItemList *ItemList;

CBaseItem::CBaseItem () :
Index(-1),
Classname(NULL),
WorldModel(NULL),
PickupSound(NULL),
Icon(NULL),
Name(NULL),
EffectFlags(0),
Precache(NULL),
Flags(ITEMFLAG_NONE)
{
};

CBaseItem::CBaseItem (char *Classname, char *WorldModel, int EffectFlags,
			   char *PickupSound, char *Icon, char *Name, EItemFlags Flags,
			   char *Precache) :
Index(-1),
Classname(Classname),
WorldModel(WorldModel),
EffectFlags(EffectFlags),
PickupSound(PickupSound),
Icon(Icon),
Name(Name),
Flags(Flags),
Precache(Precache)
{
	if (Classname)
		hashedClassnameValue = Com_HashGeneric(Classname, MAX_ITEMS_HASH);
	if (Name)
		hashedNameValue = Com_HashGeneric(Name, MAX_ITEMS_HASH);
}

void CBaseItem::DoRespawn (edict_t *ent)
{
	if (ent->team)
	{
		edict_t	*master;
		int	count;
		int choice;

		master = ent->teammaster;

		for (count = 0, ent = master; ent; ent = ent->chain, count++)
			;

		choice = rand() % count;

		for (count = 0, ent = master; count < choice; ent = ent->chain, count++)
			;
	}

	ent->svFlags &= ~SVF_NOCLIENT;
	ent->solid = SOLID_TRIGGER;
	gi.linkentity (ent);

	// send an effect
	ent->s.event = EV_ITEM_RESPAWN;
}

void CBaseItem::SetRespawn (edict_t *ent, float delay)
{
	ent->flags |= FL_RESPAWN;
	ent->svFlags |= SVF_NOCLIENT;
	ent->solid = SOLID_NOT;
	ent->nextthink = level.time + delay;
	ent->think = DoRespawn;
	gi.linkentity (ent);
}

CWeaponItem::CWeaponItem() : 
CBaseItem ()
{
};

CWeaponItem::CWeaponItem (char *Classname, char *WorldModel, int EffectFlags,
			   char *PickupSound, char *Icon, char *Name, EItemFlags Flags,
			   char *Precache, class CWeapon *Weapon, class CAmmo *Ammo, int Quantity) :
CBaseItem(Classname, WorldModel, EffectFlags, PickupSound, Icon, Name, Flags, Precache),
Weapon(Weapon),
Ammo(Ammo),
Quantity(Quantity)
{
	if (!Weapon)
		gi.dprintf ("Warning: Weapon with no weapon!\n");
	else
	{
		Weapon->Item = this;
		Weapon->WeaponItem = this;
	}
	if (!Ammo)
		gi.dprintf ("Warning: Weapon with no ammo!\n");
}

CAmmo::CAmmo() :
CBaseItem ()
{
};

CAmmo::CAmmo (char *Classname, char *WorldModel, int EffectFlags,
			   char *PickupSound, char *Icon, char *Name, EItemFlags Flags,
			   char *Precache, int Quantity, EAmmoTag Tag, CWeapon *Weapon, int Amount) :
CBaseItem (Classname, WorldModel, EffectFlags, PickupSound, Icon, Name, Flags,
		   Precache),
Quantity(Quantity),
Tag(Tag),
Weapon(Weapon),
Amount(Amount)
{
	if (Weapon)
		Weapon->Item = this;
}

CBasePowerUp::CBasePowerUp() :
CBaseItem ()
{
};

CHealth::CHealth() :
CBaseItem ()
{
};

CHealth::CHealth (char *Classname, char *WorldModel, int EffectFlags,
			   char *PickupSound, char *Icon, char *Name, EItemFlags Flags,
			   char *Precache, int Amount, EHealthFlags HealthFlags) :
CBaseItem (Classname, WorldModel, EffectFlags, PickupSound, Icon, Name, Flags,
		   Precache),
Amount(Amount),
HealthFlags(HealthFlags)
{
};

CKey::CKey() :
CBaseItem ()
{
};
CItemList::CItemList() :
numItems(0)
{
	memset (HashedClassnameItemList, 0, sizeof(CBaseItem*) * MAX_ITEMS_HASH);
	memset (HashedNameItemList, 0, sizeof(CBaseItem*) * MAX_ITEMS_HASH);
};

void CItemList::AddItemToList (CBaseItem *Item)
{
	Items[numItems] = Item;
	Item->Index = numItems++;

	// Hash!
	if (Item->Classname)
	{
		Item->hashClassnameNext = HashedClassnameItemList[Item->hashedClassnameValue];
		HashedClassnameItemList[Item->hashedClassnameValue] = Item;
	}
	if (Item->Name)
	{
		Item->hashNameNext = HashedNameItemList[Item->hashedNameValue];
		HashedNameItemList[Item->hashedNameValue] = Item;
	}
}

bool CWeaponItem::Pickup (edict_t *ent, edict_t *other)
{
	int			index;

	index = this->GetIndex();

	if ( (dmFlags.dfWeaponsStay || coop->Integer()) 
		&& other->client->pers.Inventory.Has(index))
	{
		if (!(ent->spawnflags & (DROPPED_ITEM | DROPPED_PLAYER_ITEM) ) )
			return false;	// leave the weapon for others to pickup
	}

	other->client->pers.Inventory += this;

	if (!(ent->spawnflags & DROPPED_ITEM) )
	{
		// give them some ammo with it
		if (this->Ammo)
		{
			if (dmFlags.dfInfiniteAmmo)
				this->Ammo->AddAmmo (other, 1000);
			else
				this->Ammo->AddAmmo (other, this->Ammo->Quantity);
		}

		if (! (ent->spawnflags & DROPPED_PLAYER_ITEM) )
		{
			if (deathmatch->Integer())
			{
				if (dmFlags.dfWeaponsStay)
					ent->flags |= FL_RESPAWN;
				else
					SetRespawn (ent, 30);
			}
			if (coop->Integer())
				ent->flags |= FL_RESPAWN;
		}
	}

	/*if (other->client->pers.weapon != ent->item && 
		(other->client->pers.inventory[index] == 1) &&
		( !deathmatch->Integer() || other->client->pers.weapon == FindItem("blaster") ) )
		other->client->newweapon = ent->item;*/

	return true;
}

void CWeaponItem::Use (edict_t *ent)
{
	// see if we're already using it
	if (this->Weapon == ent->client->pers.Weapon)
		return;

	if (this->Ammo && !g_select_empty->Integer() && !(this->Flags & ITEMFLAG_AMMO))
	{
		if (!ent->client->pers.Inventory.Has(this->Ammo->GetIndex()))
		{
			gi.cprintf (ent, PRINT_HIGH, "No %s for %s.\n", this->Ammo->Name, this->Name);
			return;
		}

		if (ent->client->pers.Inventory.Has(this->Ammo->GetIndex()) < this->Quantity)
		{
			gi.cprintf (ent, PRINT_HIGH, "Not enough %s for %s.\n", this->Ammo->Name, this->Name);
			return;
		}
	}

	// change to this weapon when down
	ent->client->NewWeapon = this->Weapon;
}

void CWeaponItem::Drop (edict_t *ent)
{
}

void InitItemMaxValues (edict_t *ent)
{
	ent->client->pers.maxAmmoValues[AMMOTAG_SHELLS] = 100;
	ent->client->pers.maxAmmoValues[AMMOTAG_BULLETS] = 200;
	ent->client->pers.maxAmmoValues[AMMOTAG_GRENADES] = 50;
	ent->client->pers.maxAmmoValues[AMMOTAG_ROCKETS] = 50;
	ent->client->pers.maxAmmoValues[AMMOTAG_CELLS] = 200;
	ent->client->pers.maxAmmoValues[AMMOTAG_SLUGS] = 50;
}

int CAmmo::GetMax (edict_t *ent)
{
	return ent->client->pers.maxAmmoValues[this->Tag];
}

void CAmmo::Use (edict_t *ent)
{
	if (!(this->Flags & ITEMFLAG_WEAPON))
		return;

	// see if we're already using it
	if (this->Weapon == ent->client->pers.Weapon)
		return;

	if (!g_select_empty->Integer())
	{
		if (!ent->client->pers.Inventory.Has(this->GetIndex()))
		{
			gi.cprintf (ent, PRINT_HIGH, "No %s for %s.\n", this->Name, this->Name);
			return;
		}

		if (ent->client->pers.Inventory.Has(this->GetIndex()) < this->Amount)
		{
			gi.cprintf (ent, PRINT_HIGH, "Not enough %s.\n", this->Name);
			return;
		}
	}

	// change to this weapon when down
	ent->client->NewWeapon = this->Weapon;
}

void CAmmo::Drop (edict_t *ent)
{
}

bool CAmmo::AddAmmo (edict_t *ent, int count)
{
	// YUCK
	int max = CAmmo::GetMax(ent);

	if (!max)
		return false;

	if (ent->client->pers.Inventory.Has(this) < max)
	{
		ent->client->pers.Inventory.Add (this, count);

		if (ent->client->pers.Inventory.Has(this) > max)
			ent->client->pers.Inventory.Set(this, max);

		return true;
	}
	return false;
}

bool CAmmo::Pickup (edict_t *ent, edict_t *other)
{
	int			oldcount;
	int			count;
	bool		weapon;

	weapon = (this->Flags & ITEMFLAG_WEAPON);

	if ( weapon && dmFlags.dfInfiniteAmmo )
		count = 1000;
	else if (ent->count)
		count = ent->count;
	else
		count = this->Quantity;

	oldcount = other->client->pers.Inventory.Has(this);

	if (!this->AddAmmo (other, count))
		return false;

	/*if (weapon && !oldcount)
	{
		if (other->client->pers.weapon != ent->item && ( !deathmatch->Integer() || other->client->pers.weapon == FindItem("blaster") ) )
			other->client->newweapon = ent->item;
	}*/

	if (!(ent->spawnflags & (DROPPED_ITEM | DROPPED_PLAYER_ITEM)) && (deathmatch->Integer()))
		this->SetRespawn (ent, 30);
	return true;
}

bool CBasePowerUp::Pickup (edict_t *ent, edict_t *other)
{
	return false;
}

/*void MegaHealth_think (edict_t *self)
{
	if (self->owner->health > self->owner->max_health)
	{
		self->nextthink = level.time + 1;
		self->owner->health -= 1;
		return;
	}

	if (!(self->spawnflags & DROPPED_ITEM) && (deathmatch->Integer()))
		SetRespawn (self, 20);
	else
		G_FreeEdict (self);
}

bool Pickup_Health (edict_t *ent, edict_t *other)
{
	if (!(ent->style & HEALTH_IGNORE_MAX))
		if (other->health >= other->max_health)
			return false;

	other->health += ent->count;

	if (!(ent->style & HEALTH_IGNORE_MAX))
	{
		if (other->health > other->max_health)
			other->health = other->max_health;
	}

	if (ent->style & HEALTH_TIMED)
	{
		ent->think = MegaHealth_think;
		ent->nextthink = level.time + 5;
		ent->owner = other;
		ent->flags |= FL_RESPAWN;
		ent->svFlags |= SVF_NOCLIENT;
		ent->solid = SOLID_NOT;
	}
	else
	{
		if (!(ent->spawnflags & DROPPED_ITEM) && (deathmatch->Integer()))
			SetRespawn (ent, 30);
	}

	return true;
}*/

bool CHealth::Pickup (edict_t *ent, edict_t *other)
{
	if (!(this->HealthFlags & HEALTHFLAG_IGNOREMAX) && (other->health >= other->max_health))
		return false;

	other->health += this->Amount;

	if (!(this->HealthFlags & HEALTHFLAG_IGNOREMAX))
	{
		if (other->health > other->max_health)
			other->health = other->max_health;
	}

	if (!(ent->spawnflags & DROPPED_ITEM) && (deathmatch->Integer()))
		SetRespawn (ent, 30);

	return true;
}

bool CKey::Pickup (edict_t *ent, edict_t *other)
{
	return false;
}

void CItemList::SendItemNames ()
{
	for (int i = 0 ; i < this->numItems ; i++)
		gi.configstring (this->Items[i]->GetConfigStringNumber(), this->Items[i]->Name);
}

/*
============
SpawnItem

Sets the clipping size and plants the object on the floor.

Items can't be immediately dropped to floor, because they might
be on an entity that hasn't spawned yet.
============
*/
void TouchItem (edict_t *ent, edict_t *other, plane_t *plane, cmBspSurface_t *surf)
{
	if (!other->client)
		return;
	if (other->health < 1)
		return;		// dead people can't pickup
	if (!(ent->ccitem->Flags & ITEMFLAG_GRABBABLE))
		return;		// not a grabbable item?

	if (!(ent->spawnflags & ITEM_TARGETS_USED))
	{
		G_UseTargets (ent, other);
		ent->spawnflags |= ITEM_TARGETS_USED;
	}

	if (!ent->ccitem->Pickup(ent,other))
		return;

	// flash the screen
	other->client->bonus_alpha = 0.25;	

	// show icon and name on status bar
	other->client->ps.stats[STAT_PICKUP_ICON] = gi.imageindex(ent->ccitem->Icon);
	other->client->ps.stats[STAT_PICKUP_STRING] = ent->ccitem->GetConfigStringNumber();
	other->client->pickup_msg_time = level.time + 3.0;

	// change selected item
	if (ent->ccitem->Flags & ITEMFLAG_USABLE)
		other->client->pers.Inventory.SelectedItem = other->client->ps.stats[STAT_SELECTED_ITEM] = ent->ccitem->GetIndex();

	if (ent->ccitem->PickupSound)
		Sound(other, CHAN_ITEM, gi.soundindex(ent->ccitem->PickupSound));

	if (!((coop->Integer()) &&  (ent->ccitem->Flags & ITEMFLAG_STAY_COOP)) || (ent->spawnflags & (DROPPED_ITEM | DROPPED_PLAYER_ITEM)))
	{
		if (ent->flags & FL_RESPAWN)
			ent->flags &= ~FL_RESPAWN;
		else
			G_FreeEdict (ent);
	}
}

void DoRespawn (edict_t *ent);
void Use_Item (edict_t *ent, edict_t *other, edict_t *activator);
void DropItemToFloor (edict_t *ent)
{
	CTrace	tr;
	vec3_t		dest;

	ent->mins[0] = ent->mins[1] = ent->mins[2] = -15;
	ent->maxs[0] = ent->maxs[1] = ent->maxs[2] = 15;

	if (ent->model)
		ent->s.modelIndex = gi.modelindex(ent->model);
	else
		ent->s.modelIndex = gi.modelindex(ent->ccitem->WorldModel);
	ent->solid = SOLID_TRIGGER;
	ent->movetype = MOVETYPE_TOSS;  
	ent->touch = TouchItem;

	vec3_t v = {0,0,-128};
	Vec3Add (ent->s.origin, v, dest);

	tr.Trace (ent->s.origin, ent->mins, ent->maxs, dest, ent, CONTENTS_MASK_SOLID);
	if (tr.startSolid)
	{
		//gi.dprintf ("droptofloor: %s startsolid at (%f %f %f)\n", ent->classname, ent->s.origin[0], ent->s.origin[1], ent->s.origin[2]);
		MapPrint (MAPPRINT_WARNING, ent, ent->s.origin, "Entity origin is in solid\n");
		G_FreeEdict (ent);
		return;
	}

	Vec3Copy (tr.endPos, ent->s.origin);

	if (ent->team)
	{
		ent->flags &= ~FL_TEAMSLAVE;
		ent->chain = ent->teamchain;
		ent->teamchain = NULL;

		ent->svFlags |= SVF_NOCLIENT;
		ent->solid = SOLID_NOT;
		if (ent == ent->teammaster)
		{
			ent->nextthink = level.time + FRAMETIME;
			ent->think = DoRespawn;
		}
	}

	if (ent->spawnflags & ITEM_NO_TOUCH)
	{
		ent->solid = SOLID_BBOX;
		ent->touch = NULL;
		ent->s.effects &= ~EF_ROTATE;
		ent->s.renderFx &= ~RF_GLOW;
	}

	if (ent->spawnflags & ITEM_TRIGGER_SPAWN)
	{
		ent->svFlags |= SVF_NOCLIENT;
		ent->solid = SOLID_NOT;
		ent->use = Use_Item;
	}

	gi.linkentity (ent);
}



void CC_SpawnItem (edict_t *ent, CBaseItem *item)
{
	//PrecacheItem (item);

	if (ent->spawnflags)
	{
		if (strcmp(ent->classname, "key_power_cube") != 0)
		{
			ent->spawnflags = 0;
			//gi.dprintf("%s at (%f %f %f) has invalid spawnflags set\n", ent->classname, ent->s.origin[0], ent->s.origin[1], ent->s.origin[2]);
			MapPrint (MAPPRINT_ERROR, ent, ent->s.origin, "Invalid spawnflags (%i, should be 0)\n", ent->spawnflags);
		}
	}

	// some items will be prevented in deathmatch
	if (deathmatch->Integer())
	{
		if (dmFlags.dfNoArmor)
		{
			//if (item->pickup == Pickup_Armor || item->pickup == Pickup_PowerArmor)
			if (item->Flags & ITEMFLAG_ARMOR)
			{
				G_FreeEdict (ent);
				return;
			}
		}
		if (dmFlags.dfNoItems)
		{
			//if (item->pickup == Pickup_Powerup)
			if (item->Flags & ITEMFLAG_POWERUP)
			{
				G_FreeEdict (ent);
				return;
			}
		}
		if (dmFlags.dfNoHealth)
		{
			//if (item->pickup == Pickup_Health || item->pickup == Pickup_Adrenaline || item->pickup == Pickup_AncientHead)
			if (item->Flags & ITEMFLAG_HEALTH)
			{
				G_FreeEdict (ent);
				return;
			}
		}
		if (dmFlags.dfInfiniteAmmo)
		{
			//if ( (item->flags == IT_AMMO) || (strcmp(ent->classname, "weapon_bfg") == 0) )
			if (item->Flags & ITEMFLAG_AMMO)
			{
				G_FreeEdict (ent);
				return;
			}
		}
	}

	if (coop->Integer() && (strcmp(ent->classname, "key_power_cube") == 0))
	{
		ent->spawnflags |= (1 << (8 + level.power_cubes));
		level.power_cubes++;
	}

	// don't let them drop items that stay in a coop game
	/*if ((coop->Integer()) && (item->Flags & IT_STAY_COOP))
	{
		item->drop = NULL;
	}*/

	ent->ccitem = item;
	ent->nextthink = level.time + 2 * FRAMETIME;    // items start after other solids
	ent->think = DropItemToFloor;
	ent->s.effects = item->EffectFlags;
	ent->s.renderFx = RF_GLOW;
}

bool CC_ItemExists (edict_t *ent)
{
	// Check through the itemlist
	uint32 hash = Com_HashGeneric(ent->classname, MAX_ITEMS_HASH);
	CBaseItem *Item;

	for (Item = ItemList->HashedClassnameItemList[hash]; Item; Item=Item->hashClassnameNext)
	{
		if (Q_stricmp(Item->Classname, ent->classname) == 0)
		{
			CC_SpawnItem (ent, Item);
			return true;
		}
	}
	return false;
}

CBaseItem *CC_FindItem (char *name)
{
	// Check through the itemlist
	uint32 hash = Com_HashGeneric(name, MAX_ITEMS_HASH);
	CBaseItem *Item;

	for (Item = ItemList->HashedNameItemList[hash]; Item; Item=Item->hashNameNext)
	{
		if (Q_stricmp(Item->Name, name) == 0)
			return Item;
	}
	return NULL;
}

// Forces an add
void CBaseItem::Add (edict_t *ent, int quantity)
{
	ent->client->pers.Inventory.Add(this, quantity);
}

CBaseItem *CC_FindItemByClassname (char *name)
{
	// Check through the itemlist
	uint32 hash = Com_HashGeneric(name, MAX_ITEMS_HASH);
	CBaseItem *Item;

	for (Item = ItemList->HashedClassnameItemList[hash]; Item; Item=Item->hashClassnameNext)
	{
		if (Q_stricmp(Item->Name, name) == 0)
			return Item;
	}
	return NULL;
}

CBaseItem *CC_GetItemByIndex (int Index)
{
	if (!ItemList->Items[Index] || Index >= MAX_CS_ITEMS || Index < 0)
		return NULL;
	return ItemList->Items[Index];
}

int GetNumItems ()
{
	return ItemList->numItems;
}

void AddAmmoToList ()
{
	CAmmo *Shells = new CAmmo("ammo_shells", "models/items/ammo/shells/medium/tris.md2", 0, "misc/am_pkup.wav", "a_shells", "Shells", ITEMFLAG_AMMO|ITEMFLAG_GRABBABLE, "", 5, AMMOTAG_SHELLS, NULL, -1);
	CAmmo *Bullets = new CAmmo("ammo_bullets", "models/items/ammo/bullets/medium/tris.md2", 0, "misc/am_pkup.wav", "a_bullets", "Bullets", ITEMFLAG_AMMO|ITEMFLAG_GRABBABLE, "", 50, AMMOTAG_BULLETS, NULL, -1);
	CAmmo *Slugs = new CAmmo("ammo_slugs", "models/items/ammo/slugs/medium/tris.md2", 0, "misc/am_pkup.wav", "a_slugs", "Slugs", ITEMFLAG_AMMO|ITEMFLAG_GRABBABLE, "", 5, AMMOTAG_SLUGS, NULL, -1);
	CAmmo *Grenades = new CAmmo("ammo_grenades", "models/items/ammo/grenades/medium/tris.md2", 0, "misc/am_pkup.wav", "a_grenades", "Grenades", ITEMFLAG_AMMO|ITEMFLAG_USABLE|ITEMFLAG_GRABBABLE|ITEMFLAG_WEAPON, "", 5, AMMOTAG_GRENADES, &WeaponGrenades, 1);
	CAmmo *Rockets = new CAmmo("ammo_rockets", "models/items/ammo/rockets/medium/tris.md2", 0, "misc/am_pkup.wav", "a_rockets", "Rockets", ITEMFLAG_AMMO|ITEMFLAG_GRABBABLE, "", 5, AMMOTAG_ROCKETS, NULL, -1);
	CAmmo *Cells = new CAmmo("ammo_cells", "models/items/ammo/cells/medium/tris.md2", 0, "misc/am_pkup.wav", "a_cells", "Cells", ITEMFLAG_AMMO|ITEMFLAG_GRABBABLE, "", 50, AMMOTAG_CELLS, NULL, -1);

	ItemList->AddItemToList (Shells);
	ItemList->AddItemToList (Bullets);
	ItemList->AddItemToList (Slugs);
	ItemList->AddItemToList (Grenades);
	ItemList->AddItemToList (Rockets);
	ItemList->AddItemToList (Cells);

	// Weapons
	CWeaponItem *Blaster = new CWeaponItem(NULL, NULL, 0, NULL, "w_blaster", "Blaster", ITEMFLAG_WEAPON|ITEMFLAG_USABLE, "", &WeaponBlaster, NULL, 0);
	CWeaponItem *Shotgun = new CWeaponItem("weapon_shotgun", "models/weapons/g_shotg/tris.md2", EF_ROTATE, "misc/w_pkup.wav", "w_shotgun", "Shotgun", ITEMFLAG_WEAPON|ITEMFLAG_GRABBABLE|ITEMFLAG_USABLE, "", &WeaponShotgun, Shells, 1);
	CWeaponItem *SuperShotgun = new CWeaponItem("weapon_supershotgun", "models/weapons/g_shotg2/tris.md2", EF_ROTATE, "misc/w_pkup.wav", "w_sshotgun", "Super Shotgun", ITEMFLAG_WEAPON|ITEMFLAG_GRABBABLE|ITEMFLAG_USABLE, "", &WeaponSuperShotgun, Shells, 2);
	CWeaponItem *Machinegun = new CWeaponItem("weapon_machinegun", "models/weapons/g_machn/tris.md2", EF_ROTATE, "misc/w_pkup.wav", "w_machinegun", "Machinegun", ITEMFLAG_WEAPON|ITEMFLAG_GRABBABLE|ITEMFLAG_USABLE, "", &WeaponMachinegun, Bullets, 1);
	CWeaponItem *Chaingun = new CWeaponItem("weapon_chaingun", "models/weapons/g_chain/tris.md2", EF_ROTATE, "misc/w_pkup.wav", "w_chaingun", "Chaingun", ITEMFLAG_WEAPON|ITEMFLAG_GRABBABLE|ITEMFLAG_USABLE, "", &WeaponChaingun, Bullets, 1);
	CWeaponItem *GrenadeLauncher = new CWeaponItem("weapon_grenadelauncher", "models/weapons/g_launch/tris.md2", EF_ROTATE, "misc/w_pkup.wav", "w_glauncher", "Grenade Launcher", ITEMFLAG_WEAPON|ITEMFLAG_GRABBABLE|ITEMFLAG_USABLE, "", &WeaponGrenadeLauncher, Grenades, 1);
	CWeaponItem *RocketLauncher = new CWeaponItem("weapon_rocketlauncher", "models/weapons/g_rocket/tris.md2", EF_ROTATE, "misc/w_pkup.wav", "w_rlauncher", "Rocket Launcher", ITEMFLAG_WEAPON|ITEMFLAG_GRABBABLE|ITEMFLAG_USABLE, "", &WeaponRocketLauncher, Rockets, 1);
	CWeaponItem *HyperBlaster = new CWeaponItem("weapon_hyperblaster", "models/weapons/g_hyperb/tris.md2", EF_ROTATE, "misc/w_pkup.wav", "w_hyperblaster", "HyperBlaster", ITEMFLAG_WEAPON|ITEMFLAG_GRABBABLE|ITEMFLAG_USABLE, "", &WeaponHyperBlaster, Cells, 1);
	CWeaponItem *Railgun = new CWeaponItem("weapon_railgun", "models/weapons/g_rail/tris.md2", EF_ROTATE, "misc/w_pkup.wav", "w_railgun", "Railgun", ITEMFLAG_WEAPON|ITEMFLAG_GRABBABLE|ITEMFLAG_USABLE, "", &WeaponRailgun, Slugs, 1);
	
	ItemList->AddItemToList (Blaster);
	ItemList->AddItemToList (Shotgun);
	ItemList->AddItemToList (SuperShotgun);
	ItemList->AddItemToList (Machinegun);
	ItemList->AddItemToList (Chaingun);
	ItemList->AddItemToList (GrenadeLauncher);
	ItemList->AddItemToList (RocketLauncher);
	ItemList->AddItemToList (HyperBlaster);
	ItemList->AddItemToList (Railgun);
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

void InitItemlist ()
{
	ItemList = new CItemList;

	AddAmmoToList();
	AddHealthToList();
}

void SetItemNames ()
{
	ItemList->SendItemNames();
}
