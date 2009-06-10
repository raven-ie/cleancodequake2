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
			   char *Precache, class CWeapon *Weapon, class CAmmo *Ammo, int Quantity, char *VWepModel) :
CBaseItem(Classname, WorldModel, EffectFlags, PickupSound, Icon, Name, Flags, Precache),
Weapon(Weapon),
Ammo(Ammo),
Quantity(Quantity),
VWepModel(VWepModel)
{
	if (!Weapon)
		DebugPrintf ("Warning: Weapon with no weapon!\n");
	else
	{
		Weapon->Item = this;
		Weapon->WeaponItem = this;
	}
	if (!Ammo && Quantity)
		DebugPrintf ("Warning: Weapon with no ammo has quantity!\n");
}

CAmmo::CAmmo (char *Classname, char *WorldModel, int EffectFlags,
			   char *PickupSound, char *Icon, char *Name, EItemFlags Flags,
			   char *Precache, int Quantity, EAmmoTag Tag, CWeapon *Weapon, int Amount, char *VWepModel) :
CBaseItem (Classname, WorldModel, EffectFlags, PickupSound, Icon, Name, Flags,
		   Precache),
Quantity(Quantity),
Tag(Tag),
Weapon(Weapon),
Amount(Amount),
VWepModel(VWepModel)
{
	if (Weapon)
		Weapon->Item = this;
}

bool CWeaponItem::Pickup (edict_t *ent, CPlayerEntity *other)
{
	int			index;

	index = this->GetIndex();

	if ( (dmFlags.dfWeaponsStay || game.mode == GAME_COOPERATIVE) 
		&& other->Client.pers.Inventory.Has(index))
	{
		if (!(ent->spawnflags & (DROPPED_ITEM | DROPPED_PLAYER_ITEM) ) )
			return false;	// leave the weapon for others to pickup
	}

	other->Client.pers.Inventory += this;

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
			if (game.mode & GAME_DEATHMATCH)
			{
				if (dmFlags.dfWeaponsStay)
					ent->flags |= FL_RESPAWN;
				else
					SetRespawn (ent, 30);
			}
			if (game.mode == GAME_COOPERATIVE)
				ent->flags |= FL_RESPAWN;
		}
	}
	else if (ent->count)
		this->Ammo->AddAmmo (other, ent->count);

	if (this->Weapon)
	{
		if (other->Client.pers.Weapon != this->Weapon && 
			(other->Client.pers.Inventory.Has(this) == 1) &&
			( !(game.mode & GAME_DEATHMATCH) || (other->Client.pers.Weapon && other->Client.pers.Weapon->WeaponItem == FindItem("blaster")) ) )
			other->Client.NewWeapon = this->Weapon;
	}

	return true;
}

void CWeaponItem::Use (CPlayerEntity *ent)
{
	// see if we're already using it
	if (this->Weapon == ent->Client.pers.Weapon)
		return;

	if (this->Ammo && !g_select_empty->Integer() && !(this->Flags & ITEMFLAG_AMMO))
	{
		if (!ent->Client.pers.Inventory.Has(this->Ammo->GetIndex()))
		{
			ClientPrintf (ent->gameEntity, PRINT_HIGH, "No %s for %s.\n", this->Ammo->Name, this->Name);
			return;
		}

		if (ent->Client.pers.Inventory.Has(this->Ammo->GetIndex()) < this->Quantity)
		{
			ClientPrintf (ent->gameEntity, PRINT_HIGH, "Not enough %s for %s.\n", this->Ammo->Name, this->Name);
			return;
		}
	}

	// change to this weapon when down
	ent->Client.NewWeapon = this->Weapon;
}

void CWeaponItem::Drop (CPlayerEntity *ent)
{
	DropItem(ent->gameEntity);

	//if (ent->Client)
	//{
		ent->Client.pers.Inventory -= this;

		if (this->Weapon == ent->Client.pers.Weapon)
			ent->Client.pers.Weapon->NoAmmoWeaponChange(ent);
	//}
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

int CAmmo::GetMax (CPlayerEntity *ent)
{
	return ent->Client.pers.maxAmmoValues[this->Tag];
}

void CAmmo::Use (CPlayerEntity *ent)
{
	if (!(this->Flags & ITEMFLAG_WEAPON))
		return;

	// see if we're already using it
	if (this->Weapon == ent->Client.pers.Weapon)
		return;

	if (!g_select_empty->Integer())
	{
		if (!ent->Client.pers.Inventory.Has(this->GetIndex()))
		{
			ClientPrintf (ent->gameEntity, PRINT_HIGH, "No %s for %s.\n", this->Name, this->Name);
			return;
		}

		if (ent->Client.pers.Inventory.Has(this->GetIndex()) < this->Amount)
		{
			ClientPrintf (ent->gameEntity, PRINT_HIGH, "Not enough %s.\n", this->Name);
			return;
		}
	}

	// change to this weapon when down
	ent->Client.NewWeapon = this->Weapon;
}

void CAmmo::Drop (CPlayerEntity *ent)
{
	int count = this->Quantity;
	edict_t *dropped = DropItem(ent->gameEntity);

	if (count > ent->Client.pers.Inventory.Has(this))
		count = ent->Client.pers.Inventory.Has(this);

	dropped->count = count;

	ent->Client.pers.Inventory.Remove (this, count);

	if (this->Weapon && ent->Client.pers.Weapon && (ent->Client.pers.Weapon == this->Weapon) &&
		!ent->Client.pers.Inventory.Has(this))
		ent->Client.pers.Weapon->NoAmmoWeaponChange(ent);
}

bool CAmmo::AddAmmo (CPlayerEntity *ent, int count)
{
	// YUCK
	int max = GetMax(ent);

	if (!max)
		return false;

	if (ent->Client.pers.Inventory.Has(this) < max)
	{
		ent->Client.pers.Inventory.Add (this, count);

		if (ent->Client.pers.Inventory.Has(this) > max)
			ent->Client.pers.Inventory.Set(this, max);

		return true;
	}
	return false;
}

bool CAmmo::Pickup (edict_t *ent, CPlayerEntity *other)
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
		count = Quantity;

	oldcount = other->Client.pers.Inventory.Has(this);

	if (!this->AddAmmo (other, count))
		return false;

	if (weapon && !oldcount)
	{
		if (other->Client.pers.Weapon != Weapon && (!(game.mode & GAME_DEATHMATCH) ||
			(other->Client.pers.Weapon && other->Client.pers.Weapon->WeaponItem == FindItem("Blaster"))))
			other->Client.NewWeapon = Weapon;
	}

	if (!(ent->spawnflags & (DROPPED_ITEM | DROPPED_PLAYER_ITEM)) && (game.mode & GAME_DEATHMATCH))
		SetRespawn (ent, 30);
	return true;
}

// Really didn't want to do this...
static CWeaponItem *Blaster;
static CWeaponItem *Shotgun;
static CWeaponItem *SuperShotgun;
static CWeaponItem *Machinegun;
static CWeaponItem *Chaingun;
static CWeaponItem *GrenadeLauncher;
static CWeaponItem *RocketLauncher;
static CWeaponItem *HyperBlaster;
static CWeaponItem *Railgun;
static CWeaponItem *BFG;
static CAmmo *Grenades;
#ifdef CLEANCTF_ENABLED
static CWeaponItem	*Grapple;
#endif

void AddAmmoToList ()
{
	CAmmo *Shells = new CAmmo("ammo_shells", "models/items/ammo/shells/medium/tris.md2", 0, "misc/am_pkup.wav", "a_shells", "Shells", ITEMFLAG_DROPPABLE|ITEMFLAG_AMMO|ITEMFLAG_GRABBABLE, "", 10, AMMOTAG_SHELLS, NULL, -1, NULL);
	CAmmo *Bullets = new CAmmo("ammo_bullets", "models/items/ammo/bullets/medium/tris.md2", 0, "misc/am_pkup.wav", "a_bullets", "Bullets", ITEMFLAG_DROPPABLE|ITEMFLAG_AMMO|ITEMFLAG_GRABBABLE, "", 50, AMMOTAG_BULLETS, NULL, -1, NULL);
	CAmmo *Slugs = new CAmmo("ammo_slugs", "models/items/ammo/slugs/medium/tris.md2", 0, "misc/am_pkup.wav", "a_slugs", "Slugs", ITEMFLAG_DROPPABLE|ITEMFLAG_AMMO|ITEMFLAG_GRABBABLE, "", 10, AMMOTAG_SLUGS, NULL, -1, NULL);
	Grenades = new CAmmo("ammo_grenades", "models/items/ammo/grenades/medium/tris.md2", 0, "misc/am_pkup.wav", "a_grenades", "Grenades", ITEMFLAG_DROPPABLE|ITEMFLAG_AMMO|ITEMFLAG_USABLE|ITEMFLAG_GRABBABLE|ITEMFLAG_WEAPON, "", 5, AMMOTAG_GRENADES, &WeaponGrenades, 1, "#a_grenades.md2");
	CAmmo *Rockets = new CAmmo("ammo_rockets", "models/items/ammo/rockets/medium/tris.md2", 0, "misc/am_pkup.wav", "a_rockets", "Rockets", ITEMFLAG_DROPPABLE|ITEMFLAG_AMMO|ITEMFLAG_GRABBABLE, "", 5, AMMOTAG_ROCKETS, NULL, -1, NULL);
	CAmmo *Cells = new CAmmo("ammo_cells", "models/items/ammo/cells/medium/tris.md2", 0, "misc/am_pkup.wav", "a_cells", "Cells", ITEMFLAG_DROPPABLE|ITEMFLAG_AMMO|ITEMFLAG_GRABBABLE, "", 50, AMMOTAG_CELLS, NULL, -1, NULL);

	ItemList->AddItemToList (Shells);
	ItemList->AddItemToList (Bullets);
	ItemList->AddItemToList (Slugs);
	ItemList->AddItemToList (Grenades);
	ItemList->AddItemToList (Rockets);
	ItemList->AddItemToList (Cells);

	// Weapons
	Blaster = new CWeaponItem(NULL, NULL, 0, NULL, "w_blaster", "Blaster", ITEMFLAG_WEAPON|ITEMFLAG_USABLE, "", &WeaponBlaster, NULL, 0, "#w_blaster.md2");
	Shotgun = new CWeaponItem("weapon_shotgun", "models/weapons/g_shotg/tris.md2", EF_ROTATE, "misc/w_pkup.wav", "w_shotgun", "Shotgun", ITEMFLAG_DROPPABLE|ITEMFLAG_WEAPON|ITEMFLAG_GRABBABLE|ITEMFLAG_STAY_COOP|ITEMFLAG_USABLE, "", &WeaponShotgun, Shells, 1, "#w_shotgun.md2");
	SuperShotgun = new CWeaponItem("weapon_supershotgun", "models/weapons/g_shotg2/tris.md2", EF_ROTATE, "misc/w_pkup.wav", "w_sshotgun", "Super Shotgun", ITEMFLAG_DROPPABLE|ITEMFLAG_WEAPON|ITEMFLAG_GRABBABLE|ITEMFLAG_STAY_COOP|ITEMFLAG_USABLE, "", &WeaponSuperShotgun, Shells, 2, "#w_sshotgun.md2");
	Machinegun = new CWeaponItem("weapon_machinegun", "models/weapons/g_machn/tris.md2", EF_ROTATE, "misc/w_pkup.wav", "w_machinegun", "Machinegun", ITEMFLAG_DROPPABLE|ITEMFLAG_WEAPON|ITEMFLAG_GRABBABLE|ITEMFLAG_STAY_COOP|ITEMFLAG_USABLE, "", &WeaponMachinegun, Bullets, 1, "#w_machinegun.md2");
	Chaingun = new CWeaponItem("weapon_chaingun", "models/weapons/g_chain/tris.md2", EF_ROTATE, "misc/w_pkup.wav", "w_chaingun", "Chaingun", ITEMFLAG_DROPPABLE|ITEMFLAG_WEAPON|ITEMFLAG_GRABBABLE|ITEMFLAG_STAY_COOP|ITEMFLAG_USABLE, "", &WeaponChaingun, Bullets, 1, "#w_chaingun.md2");
	GrenadeLauncher = new CWeaponItem("weapon_grenadelauncher", "models/weapons/g_launch/tris.md2", EF_ROTATE, "misc/w_pkup.wav", "w_glauncher", "Grenade Launcher", ITEMFLAG_DROPPABLE|ITEMFLAG_WEAPON|ITEMFLAG_GRABBABLE|ITEMFLAG_STAY_COOP|ITEMFLAG_USABLE, "", &WeaponGrenadeLauncher, Grenades, 1, "#w_glauncher.md2");
	RocketLauncher = new CWeaponItem("weapon_rocketlauncher", "models/weapons/g_rocket/tris.md2", EF_ROTATE, "misc/w_pkup.wav", "w_rlauncher", "Rocket Launcher", ITEMFLAG_DROPPABLE|ITEMFLAG_WEAPON|ITEMFLAG_GRABBABLE|ITEMFLAG_STAY_COOP|ITEMFLAG_USABLE, "", &WeaponRocketLauncher, Rockets, 1, "#w_rlauncher.md2");
	HyperBlaster = new CWeaponItem("weapon_hyperblaster", "models/weapons/g_hyperb/tris.md2", EF_ROTATE, "misc/w_pkup.wav", "w_hyperblaster", "HyperBlaster", ITEMFLAG_DROPPABLE|ITEMFLAG_WEAPON|ITEMFLAG_GRABBABLE|ITEMFLAG_STAY_COOP|ITEMFLAG_USABLE, "", &WeaponHyperBlaster, Cells, 1, "#w_hyperblaster.md2");
	Railgun = new CWeaponItem("weapon_railgun", "models/weapons/g_rail/tris.md2", EF_ROTATE, "misc/w_pkup.wav", "w_railgun", "Railgun", ITEMFLAG_DROPPABLE|ITEMFLAG_WEAPON|ITEMFLAG_GRABBABLE|ITEMFLAG_STAY_COOP|ITEMFLAG_USABLE, "", &WeaponRailgun, Slugs, 1, "#w_railgun.md2");
	BFG = new CWeaponItem("weapon_bfg", "models/weapons/g_bfg/tris.md2", EF_ROTATE, "misc/w_pkup.wav", "w_bfg", "BFG10k", ITEMFLAG_DROPPABLE|ITEMFLAG_WEAPON|ITEMFLAG_GRABBABLE|ITEMFLAG_STAY_COOP|ITEMFLAG_USABLE, "", &WeaponBFG, Cells, 50, "#w_bfg.md2");

#ifdef CLEANCTF_ENABLED
	Grapple = new CWeaponItem (NULL, NULL, 0, NULL, "w_grapple", "Grapple", ITEMFLAG_WEAPON|ITEMFLAG_USABLE, "", &WeaponGrapple, NULL, 0, "#w_grapple.md2");
#endif

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

#ifdef CLEANCTF_ENABLED
	ItemList->AddItemToList (Grapple);
#endif
}

#define WEAP_BLASTER			1 
#define WEAP_SHOTGUN			2 
#define WEAP_SUPERSHOTGUN		3 
#define WEAP_MACHINEGUN			4 
#define WEAP_CHAINGUN			5 
#define WEAP_GRENADES			6 
#define WEAP_GRENADELAUNCHER	7 
#define WEAP_ROCKETLAUNCHER		8 
#define WEAP_HYPERBLASTER		9 
#define WEAP_RAILGUN			10
#define WEAP_BFG				11
#ifdef CLEANCTF_ENABLED
#define WEAP_GRAPPLE			12
#endif

void DoWeaponVweps ()
{
	int takeAway = ModelIndex(Blaster->VWepModel) - 1;

	Blaster->Weapon->vwepIndex = ModelIndex(Blaster->VWepModel) - takeAway;
	Shotgun->Weapon->vwepIndex = ModelIndex(Shotgun->VWepModel) - takeAway;
	SuperShotgun->Weapon->vwepIndex = ModelIndex(SuperShotgun->VWepModel) - takeAway;
	Machinegun->Weapon->vwepIndex = ModelIndex(Machinegun->VWepModel) - takeAway;
	Chaingun->Weapon->vwepIndex = ModelIndex(Chaingun->VWepModel) - takeAway;
	GrenadeLauncher->Weapon->vwepIndex = ModelIndex(GrenadeLauncher->VWepModel) - takeAway;
	Grenades->Weapon->vwepIndex = ModelIndex(Grenades->VWepModel) - takeAway;
	RocketLauncher->Weapon->vwepIndex = ModelIndex(RocketLauncher->VWepModel) - takeAway;
	HyperBlaster->Weapon->vwepIndex = ModelIndex(HyperBlaster->VWepModel) - takeAway;
	Railgun->Weapon->vwepIndex = ModelIndex(Railgun->VWepModel) - takeAway;
	BFG->Weapon->vwepIndex = ModelIndex(BFG->VWepModel) - takeAway;
#ifdef CLEANCTF_ENABLED
	Grapple->Weapon->vwepIndex = ModelIndex(Grapple->VWepModel) - takeAway;
#endif
}