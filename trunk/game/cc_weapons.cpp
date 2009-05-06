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
// cc_weapons.cpp
// New item system code
//

#include "cc_local.h"

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
	if (!Ammo && Quantity)
		gi.dprintf ("Warning: Weapon with no ammo has quantity!\n");
}

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
	else if (ent->count)
	{
		this->Ammo->AddAmmo (other, ent->count);
	}

	if (this->Weapon)
	{
		if (other->client->pers.Weapon != this->Weapon && 
			(other->client->pers.Inventory.Has(this) == 1) &&
			( !deathmatch->Integer() || (other->client->pers.Weapon && other->client->pers.Weapon->WeaponItem == FindItem("blaster")) ) )
			other->client->NewWeapon = this->Weapon;
	}

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
	DropItem(ent);
	ent->client->pers.Inventory -= this;

	if (this->Weapon == ent->client->pers.Weapon)
		ent->client->pers.Weapon->NoAmmoWeaponChange(ent);
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

int maxBackpackAmmoValues[AMMOTAG_MAX] =
{
	200,
	300,
	100,
	100,
	300,
	100
};
int maxBandolierAmmoValues[AMMOTAG_MAX] =
{
	150,
	250,
	50,
	50,
	250,
	75
};

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
	int count = this->Quantity;
	edict_t *dropped = DropItem(ent);

	if (ent->client && count > ent->client->pers.Inventory.Has(this))
		count = ent->client->pers.Inventory.Has(this);

	dropped->count = count;

	if (ent->client)
		ent->client->pers.Inventory.Remove (this, count);

	if (ent->client && this->Weapon && ent->client->pers.Weapon && (ent->client->pers.Weapon == this->Weapon) &&
		!ent->client->pers.Inventory.Has(this))
		ent->client->pers.Weapon->NoAmmoWeaponChange(ent);
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

	if (weapon && !oldcount)
	{
		if (other->client->pers.Weapon != this->Weapon && ( !deathmatch->Integer() || (other->client->pers.Weapon && other->client->pers.Weapon->WeaponItem == FindItem("Blaster")) ) )
			other->client->NewWeapon = this->Weapon;
	}

	if (!(ent->spawnflags & (DROPPED_ITEM | DROPPED_PLAYER_ITEM)) && (deathmatch->Integer()))
		this->SetRespawn (ent, 30);
	return true;
}

void AddAmmoToList ()
{
	CAmmo *Shells = new CAmmo("ammo_shells", "models/items/ammo/shells/medium/tris.md2", 0, "misc/am_pkup.wav", "a_shells", "Shells", ITEMFLAG_DROPPABLE|ITEMFLAG_AMMO|ITEMFLAG_GRABBABLE, "", 10, AMMOTAG_SHELLS, NULL, -1);
	CAmmo *Bullets = new CAmmo("ammo_bullets", "models/items/ammo/bullets/medium/tris.md2", 0, "misc/am_pkup.wav", "a_bullets", "Bullets", ITEMFLAG_DROPPABLE|ITEMFLAG_AMMO|ITEMFLAG_GRABBABLE, "", 50, AMMOTAG_BULLETS, NULL, -1);
	CAmmo *Slugs = new CAmmo("ammo_slugs", "models/items/ammo/slugs/medium/tris.md2", 0, "misc/am_pkup.wav", "a_slugs", "Slugs", ITEMFLAG_DROPPABLE|ITEMFLAG_AMMO|ITEMFLAG_GRABBABLE, "", 5, AMMOTAG_SLUGS, NULL, -1);
	CAmmo *Grenades = new CAmmo("ammo_grenades", "models/items/ammo/grenades/medium/tris.md2", 0, "misc/am_pkup.wav", "a_grenades", "Grenades", ITEMFLAG_DROPPABLE|ITEMFLAG_AMMO|ITEMFLAG_USABLE|ITEMFLAG_GRABBABLE|ITEMFLAG_WEAPON, "", 5, AMMOTAG_GRENADES, &WeaponGrenades, 1);
	CAmmo *Rockets = new CAmmo("ammo_rockets", "models/items/ammo/rockets/medium/tris.md2", 0, "misc/am_pkup.wav", "a_rockets", "Rockets", ITEMFLAG_DROPPABLE|ITEMFLAG_AMMO|ITEMFLAG_GRABBABLE, "", 5, AMMOTAG_ROCKETS, NULL, -1);
	CAmmo *Cells = new CAmmo("ammo_cells", "models/items/ammo/cells/medium/tris.md2", 0, "misc/am_pkup.wav", "a_cells", "Cells", ITEMFLAG_DROPPABLE|ITEMFLAG_AMMO|ITEMFLAG_GRABBABLE, "", 50, AMMOTAG_CELLS, NULL, -1);

	ItemList->AddItemToList (Shells);
	ItemList->AddItemToList (Bullets);
	ItemList->AddItemToList (Slugs);
	ItemList->AddItemToList (Grenades);
	ItemList->AddItemToList (Rockets);
	ItemList->AddItemToList (Cells);

	// Weapons
	CWeaponItem *Blaster = new CWeaponItem(NULL, NULL, 0, NULL, "w_blaster", "Blaster", ITEMFLAG_WEAPON|ITEMFLAG_USABLE, "", &WeaponBlaster, NULL, 0);
	CWeaponItem *Shotgun = new CWeaponItem("weapon_shotgun", "models/weapons/g_shotg/tris.md2", EF_ROTATE, "misc/w_pkup.wav", "w_shotgun", "Shotgun", ITEMFLAG_DROPPABLE|ITEMFLAG_WEAPON|ITEMFLAG_GRABBABLE|ITEMFLAG_STAY_COOP|ITEMFLAG_USABLE, "", &WeaponShotgun, Shells, 1);
	CWeaponItem *SuperShotgun = new CWeaponItem("weapon_supershotgun", "models/weapons/g_shotg2/tris.md2", EF_ROTATE, "misc/w_pkup.wav", "w_sshotgun", "Super Shotgun", ITEMFLAG_DROPPABLE|ITEMFLAG_WEAPON|ITEMFLAG_GRABBABLE|ITEMFLAG_STAY_COOP|ITEMFLAG_USABLE, "", &WeaponSuperShotgun, Shells, 2);
	CWeaponItem *Machinegun = new CWeaponItem("weapon_machinegun", "models/weapons/g_machn/tris.md2", EF_ROTATE, "misc/w_pkup.wav", "w_machinegun", "Machinegun", ITEMFLAG_DROPPABLE|ITEMFLAG_WEAPON|ITEMFLAG_GRABBABLE|ITEMFLAG_STAY_COOP|ITEMFLAG_USABLE, "", &WeaponMachinegun, Bullets, 1);
	CWeaponItem *Chaingun = new CWeaponItem("weapon_chaingun", "models/weapons/g_chain/tris.md2", EF_ROTATE, "misc/w_pkup.wav", "w_chaingun", "Chaingun", ITEMFLAG_DROPPABLE|ITEMFLAG_WEAPON|ITEMFLAG_GRABBABLE|ITEMFLAG_STAY_COOP|ITEMFLAG_USABLE, "", &WeaponChaingun, Bullets, 1);
	CWeaponItem *GrenadeLauncher = new CWeaponItem("weapon_grenadelauncher", "models/weapons/g_launch/tris.md2", EF_ROTATE, "misc/w_pkup.wav", "w_glauncher", "Grenade Launcher", ITEMFLAG_DROPPABLE|ITEMFLAG_WEAPON|ITEMFLAG_GRABBABLE|ITEMFLAG_STAY_COOP|ITEMFLAG_USABLE, "", &WeaponGrenadeLauncher, Grenades, 1);
	CWeaponItem *RocketLauncher = new CWeaponItem("weapon_rocketlauncher", "models/weapons/g_rocket/tris.md2", EF_ROTATE, "misc/w_pkup.wav", "w_rlauncher", "Rocket Launcher", ITEMFLAG_DROPPABLE|ITEMFLAG_WEAPON|ITEMFLAG_GRABBABLE|ITEMFLAG_STAY_COOP|ITEMFLAG_USABLE, "", &WeaponRocketLauncher, Rockets, 1);
	CWeaponItem *HyperBlaster = new CWeaponItem("weapon_hyperblaster", "models/weapons/g_hyperb/tris.md2", EF_ROTATE, "misc/w_pkup.wav", "w_hyperblaster", "HyperBlaster", ITEMFLAG_DROPPABLE|ITEMFLAG_WEAPON|ITEMFLAG_GRABBABLE|ITEMFLAG_STAY_COOP|ITEMFLAG_USABLE, "", &WeaponHyperBlaster, Cells, 1);
	CWeaponItem *Railgun = new CWeaponItem("weapon_railgun", "models/weapons/g_rail/tris.md2", EF_ROTATE, "misc/w_pkup.wav", "w_railgun", "Railgun", ITEMFLAG_DROPPABLE|ITEMFLAG_WEAPON|ITEMFLAG_GRABBABLE|ITEMFLAG_STAY_COOP|ITEMFLAG_USABLE, "", &WeaponRailgun, Slugs, 1);
	CWeaponItem *BFG = new CWeaponItem("weapon_bfg", "models/weapons/g_bfg/tris.md2", EF_ROTATE, "misc/w_pkup.wav", "w_bfg", "BFG10k", ITEMFLAG_DROPPABLE|ITEMFLAG_WEAPON|ITEMFLAG_GRABBABLE|ITEMFLAG_STAY_COOP|ITEMFLAG_USABLE, "", &WeaponBFG, Cells, 50);
	
	ItemList->AddItemToList (Blaster);
	ItemList->AddItemToList (Shotgun);
	ItemList->AddItemToList (SuperShotgun);
	ItemList->AddItemToList (Machinegun);
	ItemList->AddItemToList (Chaingun);
	ItemList->AddItemToList (GrenadeLauncher);
	ItemList->AddItemToList (RocketLauncher);
	ItemList->AddItemToList (HyperBlaster);
	ItemList->AddItemToList (Railgun);
	ItemList->AddItemToList (BFG);
}