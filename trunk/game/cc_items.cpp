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
//

#include "cc_local.h"

CItemList *ItemList;

void CItemList::FillBasicData (char *Classname, char *WorldModel, int EffectFlags, char *PickupSound, char *Icon, char *Name, EItemFlags Flags, char *Precache)
{
	int Index = numItems;
	// Fill in the hash tables as appropriate
	int Hash = Com_HashGeneric(Classname, MAX_ITEMS_HASH);
	HashedClassnameItemList[Hash] = Items[Index];
	Hash = Com_HashGeneric(Name, MAX_ITEMS_HASH);
	HashedNameItemList[Hash] = Items[Index];

	// Fill in the other data
	Items[Index]->Classname = Classname;
	Items[Index]->WorldModel = WorldModel;
	Items[Index]->EffectFlags = EffectFlags;
	Items[Index]->PickupSound = PickupSound;
	Items[Index]->Icon = Icon;
	Items[Index]->Name = Name;
	Items[Index]->Flags = Flags;
	Items[Index]->Precache = Precache;
	numItems++;
}

void CItemList::AddWeapon (char *Classname, char *WorldModel, int EffectFlags, char *PickupSound, char *Icon, char *Name, EItemFlags Flags, char *Precache)
{
	CWeaponItem *Item = new CWeaponItem(numItems);
	Items[numItems] = Item;
	FillBasicData(Classname, WorldModel, EffectFlags, PickupSound, Icon, Name, Flags, Precache);
}

void CItemList::AddAmmo (char *Classname, char *WorldModel, int EffectFlags, char *PickupSound, char *Icon, char *Name, EItemFlags Flags, char *Precache, int Quantity)
{
	CAmmo *Item = new CAmmo(numItems);
	Items[numItems] = Item;
	FillBasicData(Classname, WorldModel, EffectFlags, PickupSound, Icon, Name, Flags, Precache);

	Item->Quantity = Quantity;
}

void CItemList::AddPowerup (char *Classname, char *WorldModel, int EffectFlags, char *PickupSound, char *Icon, char *Name, EItemFlags Flags, char *Precache, int Time)
{
	CPowerUp *Item = new CPowerUp(numItems);
	Items[numItems] = Item;
	FillBasicData(Classname, WorldModel, EffectFlags, PickupSound, Icon, Name, Flags, Precache);

	Item->Time = Time;
}

void CItemList::AddHealth (char *Classname, char *WorldModel, int EffectFlags, char *PickupSound, char *Icon, char *Name, EItemFlags Flags, char *Precache, int Amount)
{
	CHealth *Item = new CHealth(numItems);
	Items[numItems] = Item;
	FillBasicData(Classname, WorldModel, EffectFlags, PickupSound, Icon, Name, Flags, Precache);

	Item->Amount = Amount;
}

void CItemList::AddKey (char *Classname, char *WorldModel, int EffectFlags, char *PickupSound, char *Icon, char *Name, EItemFlags Flags, char *Precache)
{
	CKey *Item = new CKey(numItems);
	Items[numItems] = Item;
	FillBasicData(Classname, WorldModel, EffectFlags, PickupSound, Icon, Name, Flags, Precache);
}

void InitItemlist ()
{
	ItemList = new CItemList;
	ItemList->numItems = 0;

	ItemList->AddAmmo ("ammo_shells", "models/items/ammo/shells/medium/tris.md2", 0, "pickup", "icon", "Shells", ITEMFLAG_AMMO, "precache", 25);
}