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
// New, improved, better, stable item system!
//

#include "cc_local.h"

/*	Cmd_AddCommand ("use",					Cmd_Use_f);
	Cmd_AddCommand ("drop",					Cmd_Drop_f);
	Cmd_AddCommand ("inven",				Cmd_Inven_f);
	Cmd_AddCommand ("invuse",				Cmd_InvUse_f);
	Cmd_AddCommand ("invdrop",				Cmd_InvDrop_f);
	Cmd_AddCommand ("weapprev",				Cmd_WeapPrev_f);
	Cmd_AddCommand ("weapnext",				Cmd_WeapNext_f);
	Cmd_AddCommand ("weaplast",				Cmd_WeapLast_f);
	Cmd_AddCommand ("invnext",				Cmd_SelectNextItem_f);
	Cmd_AddCommand ("invprev",				Cmd_SelectPrevItem_f);
	Cmd_AddCommand ("invnextw",				Cmd_SelectNextWeapon_f);
	Cmd_AddCommand ("invprevw",				Cmd_SelectPrevWeapon_f);
	Cmd_AddCommand ("invnextp",				Cmd_SelectNextPowerup_f);
	Cmd_AddCommand ("invprevp",				Cmd_SelectPrevPowerup_f);*/

CInventory::CInventory ()
{
	memset (Array, 0, sizeof(int)*MAX_CS_ITEMS);
	SelectedItem = -1;
}

void CInventory::Add (CBaseItem *Item, int Num)
{
	Array[Item->GetIndex()] += Num;
}

void CInventory::Remove (CBaseItem *Item, int Num)
{
	Array[Item->GetIndex()] -= Num;
}

void CInventory::Add (int Index, int Num)
{
	Array[Index] += Num;
}

void CInventory::Remove (int Index, int Num)
{
	Array[Index] -= Num;
}

void CInventory::Draw (edict_t *ent)
{
	ent->client->showscores = false;
	ent->client->showhelp = false;

	if (ent->client->showinventory)
	{
		ent->client->showinventory = false;
		return;
	}

	ent->client->showinventory = true;

	WriteByte (SVC_INVENTORY);
	for (int i=0 ; i<MAX_CS_ITEMS ; i++)
		WriteShort (Array[i]);
	Cast (CASTFLAG_RELIABLE, ent);
}

void CInventory::SelectNextItem(EItemFlags Flags)
{
	int			index;

	/*if (ent->client->chase_target) {
		ChaseNext(ent);
		return;
	}*/

	// scan  for the next valid one
	for (int i=1 ; i<=MAX_CS_ITEMS ; i++)
	{
		index = (SelectedItem + i)%MAX_CS_ITEMS;
		if (!Array[index])
			continue;
		CBaseItem *it = GetItemByIndex(index);
		if (!(it->Flags & ITEMFLAG_USABLE))
			continue;
		if (!(it->Flags & Flags))
			continue;

		SelectedItem = index;
		return;
	}

	SelectedItem = -1;
	ValidateSelectedItem ();
}

void CInventory::SelectPrevItem(EItemFlags Flags)
{
	int			index;

	/*if (ent->client->chase_target) {
		ChaseNext(ent);
		return;
	}*/

	// scan  for the next valid one
	for (int i=1 ; i<=MAX_CS_ITEMS ; i++)
	{
		index = (SelectedItem + MAX_CS_ITEMS - i)%MAX_CS_ITEMS;
		if (!Array[index])
			continue;
		CBaseItem *it = GetItemByIndex(index);
		if (!(it->Flags & ITEMFLAG_USABLE))
			continue;
		if (!(it->Flags & Flags))
			continue;

		SelectedItem = index;
		return;
	}

	SelectedItem = -1;
	ValidateSelectedItem ();
}

int CInventory::Has (int Index)
{
	return Array[Index];
}

int CInventory::Has (CBaseItem *Item)
{
	return Array[Item->GetIndex()];
}

void CInventory::Set (CBaseItem *Item, int Num)
{
	Array[Item->GetIndex()] = Num;
}

void CInventory::Set (int Index, int Num)
{
	Array[Index] = Num;
}

void CInventory::ValidateSelectedItem()
{
	if (Array[SelectedItem])
		return;		// valid

	SelectNextItem (-1);
}

void CInventory::operator += (CBaseItem *Item)
{
	Add(Item, 1);
}

void CInventory::operator += (int Index)
{
	Add(GetItemByIndex(Index), 1);
}

void CInventory::operator -= (CBaseItem *Item)
{
	Remove(Item, 1);
}

void CInventory::operator -= (int Index)
{
	Remove (GetItemByIndex(Index), 1);
}

/*
==================
Cmd_Use_f

Use an inventory item
==================
*/
void Cmd_Use_f (edict_t *ent)
{
	char *s = ArgGetConcatenatedString();
	CBaseItem *Item = FindItem(s);
	if (!Item)
	{
		Item = FindItemByClassname(s);

		if (!Item)
		{
			ClientPrintf (ent, PRINT_HIGH, "Unknown item: %s\n", s);
			return;
		}
	}
	if (!(Item->Flags & ITEMFLAG_USABLE))
	{
		ClientPrintf (ent, PRINT_HIGH, "Item is not usable.\n");
		return;
	}
	if (!ent->client->pers.Inventory.Has(Item))
	{
		ClientPrintf (ent, PRINT_HIGH, "Out of item: %s\n", s);
		return;
	}

	Item->Use (ent);
}


/*
==================
Cmd_Drop_f

Drop an inventory item
==================
*/
void Cmd_Drop_f (edict_t *ent)
{
	char *s = ArgGetConcatenatedString();
	CBaseItem *Item = FindItem(s);
	if (!Item)
	{
		Item = FindItemByClassname(s);

		if (!Item)
		{
			ClientPrintf (ent, PRINT_HIGH, "Unknown item: %s\n", s);
			return;
		}
	}
	if (!(Item->Flags & ITEMFLAG_DROPPABLE))
	{
		ClientPrintf (ent, PRINT_HIGH, "Item is not dropable.\n");
		return;
	}
	if (!ent->client->pers.Inventory.Has(Item))
	{
		ClientPrintf (ent, PRINT_HIGH, "Out of item: %s\n", s);
		return;
	}

	Item->Drop (ent);
	ent->client->pers.Inventory.ValidateSelectedItem();
}


/*
=================
Cmd_Inven_f
=================
*/
void Cmd_Inven_f (edict_t *ent)
{
	ent->client->pers.Inventory.Draw(ent);
}

/*
=================
Cmd_InvUse_f
=================
*/
void Cmd_InvUse_f (edict_t *ent)
{
	ent->client->pers.Inventory.ValidateSelectedItem ();

	if (ent->client->pers.Inventory.SelectedItem == -1)
	{
		ClientPrintf (ent, PRINT_HIGH, "No item to use.\n");
		return;
	}

	CBaseItem *it = GetItemByIndex(ent->client->pers.Inventory.SelectedItem);
	if (!(it->Flags & ITEMFLAG_USABLE))
	{
		ClientPrintf (ent, PRINT_HIGH, "Item is not usable.\n");
		return;
	}
	it->Use (ent);
}

/*
=================
Cmd_WeapPrev_f
=================
*/
void Cmd_WeapPrev_f (edict_t *ent)
{
	if (!ent->client->pers.Weapon)
		return;

	int selectedWeaponIndex = ent->client->pers.Weapon->Item->GetIndex();

	// scan  for the next valid one
	for (int i=0 ; i<=GetNumItems() ; i++)
	{
		int index = (selectedWeaponIndex + MAX_ITEMS - i)%MAX_CS_ITEMS;
		if (!ent->client->pers.Inventory.Has(index))
			continue;
		CBaseItem *Item = GetItemByIndex(index);
		if (!(Item->Flags & ITEMFLAG_USABLE))
			continue;
		if (! (Item->Flags & ITEMFLAG_WEAPON) )
			continue;
		Item->Use (ent);
		if (ent->client->NewWeapon && ent->client->NewWeapon->Item == Item || ent->client->pers.Weapon->Item == Item)
			return;	// successful
	}
}

/*
=================
Cmd_WeapNext_f
=================
*/
void Cmd_WeapNext_f (edict_t *ent)
{
	if (!ent->client->pers.Weapon)
		return;

	int selectedWeaponIndex = ent->client->pers.Weapon->Item->GetIndex();

	// scan  for the next valid one
	for (int i=0 ; i<=GetNumItems() ; i++)
	{
		int index = (selectedWeaponIndex + i)%MAX_CS_ITEMS;
		if (!ent->client->pers.Inventory.Has(index))
			continue;
		CBaseItem *Item = GetItemByIndex(index);
		if (!(Item->Flags & ITEMFLAG_USABLE))
			continue;
		if (! (Item->Flags & ITEMFLAG_WEAPON) )
			continue;
		Item->Use (ent);
		if (ent->client->NewWeapon && ent->client->NewWeapon->Item == Item || ent->client->pers.Weapon->Item == Item)
			return;	// successful
	}
}

/*
=================
Cmd_WeapLast_f
=================
*/
void Cmd_WeapLast_f (edict_t *ent)
{
/*	gclient_t	*cl;
	int			index;
	gitem_t		*it;

	cl = ent->client;

	if (!cl->pers.weapon || !cl->pers.lastweapon)
		return;

	index = ITEM_INDEX(cl->pers.lastweapon);
	if (!cl->pers.inventory[index])
		return;
	it = &itemlist[index];
	if (!it->use)
		return;
	if (! (it->flags & IT_WEAPON) )
		return;
	it->use (ent, it);*/
}

/*
=================
Cmd_InvDrop_f
=================
*/
void Cmd_InvDrop_f (edict_t *ent)
{
	ent->client->pers.Inventory.ValidateSelectedItem ();

	if (ent->client->pers.Inventory.SelectedItem == -1)
	{
		ClientPrintf (ent, PRINT_HIGH, "No item to drop.\n");
		return;
	}

	CBaseItem *Item = GetItemByIndex(ent->client->pers.Inventory.SelectedItem);
	if (!(Item->Flags & ITEMFLAG_DROPPABLE))
	{
		ClientPrintf (ent, PRINT_HIGH, "Item is not dropable.\n");
		return;
	}
	Item->Drop (ent);
	ent->client->pers.Inventory.ValidateSelectedItem();
}

void Cmd_SelectNextItem_f (edict_t *ent)
{
	ent->client->pers.Inventory.SelectNextItem (-1);
}
void Cmd_SelectPrevItem_f (edict_t *ent)
{
	ent->client->pers.Inventory.SelectPrevItem (-1);
}
void Cmd_SelectNextWeapon_f (edict_t *ent)
{
	ent->client->pers.Inventory.SelectNextItem (ITEMFLAG_WEAPON);
}
void Cmd_SelectPrevWeapon_f (edict_t *ent)
{
	ent->client->pers.Inventory.SelectPrevItem (ITEMFLAG_WEAPON);
}
void Cmd_SelectNextPowerup_f (edict_t *ent)
{
	ent->client->pers.Inventory.SelectNextItem (ITEMFLAG_POWERUP);
}
void Cmd_SelectPrevPowerup_f (edict_t *ent)
{
	ent->client->pers.Inventory.SelectPrevItem (ITEMFLAG_POWERUP);
}

/*
==================
Cmd_Give_f

Give items to a client
Old-style "give"
==================
*/
void TouchItem (edict_t *ent, edict_t *other, plane_t *plane, cmBspSurface_t *surf);
void Cmd_Give_f (edict_t *ent)
{
	char		*name;
	int			i;
	bool	give_all;
	CBaseItem *it;

	name = ArgGets(1);

	if (Q_stricmp(name, "all") == 0)
		give_all = true;
	else
		give_all = false;

	if (give_all || Q_stricmp(name, "health") == 0)
	{
		if (ArgCount() == 3)
			ent->health = ArgGeti(2);
		else
			ent->health = ent->max_health;
		if (!give_all)
			return;
	}

	if (give_all || Q_stricmp(name, "weapons") == 0)
	{
		for (i=0 ; i<GetNumItems() ; i++)
		{
			it = GetItemByIndex(i);
			if (!(it->Flags & ITEMFLAG_GRABBABLE))
				continue;
			if (!(it->Flags & ITEMFLAG_WEAPON))
				continue;
			ent->client->pers.Inventory += it;
		}
		if (!give_all)
			return;
	}

	if (give_all || Q_stricmp(name, "ammo") == 0)
	{
		for (i=0 ; i<GetNumItems() ; i++)
		{
			it = GetItemByIndex(i);
			if (!(it->Flags & ITEMFLAG_GRABBABLE))
				continue;
			if (!(it->Flags & ITEMFLAG_AMMO))
				continue;

			CAmmo *Ammo = static_cast<CAmmo*>(it);
			Ammo->AddAmmo (ent, 1000);
		}
		if (!give_all)
			return;
	}

	if (give_all || Q_stricmp(name, "armor") == 0)
	{
		ent->client->pers.Inventory.Set(FindItem("Jacket Armor")->GetIndex(), 0);
		ent->client->pers.Inventory.Set(FindItem("Combat Armor")->GetIndex(), 0);

		CArmor *Armor = dynamic_cast<CArmor*>(FindItem("Body Armor"));
		ent->client->pers.Inventory.Set(Armor->GetIndex(), Armor->maxCount);
		ent->client->pers.Armor = Armor;

		if (ArgCount() >= 3)
			ent->client->pers.Inventory.Set(Armor->GetIndex(), ArgGeti(2));

		if (!give_all)
			return;
	}

	if (give_all || Q_stricmp(name, "Power Shield") == 0)
	{
		it = FindItem("Power Shield");
		edict_t *it_ent = G_Spawn();
		it_ent->classname = it->Classname;
		SpawnItem (it_ent, it);
		TouchItem (it_ent, ent, NULL, NULL);
		if (it_ent->inUse)
			G_FreeEdict(it_ent);

		if (!give_all)
			return;
	}

	if (give_all)
	{
		for (i=0 ; i<GetNumItems() ; i++)
		{
			it = GetItemByIndex(i);
			if (!(it->Flags & ITEMFLAG_GRABBABLE))
				continue;
			if (it->Flags & (ITEMFLAG_HEALTH|ITEMFLAG_ARMOR|ITEMFLAG_WEAPON|ITEMFLAG_AMMO))
				continue;
			ent->client->pers.Inventory.Set (i, 1);
		}
		return;
	}

	it = FindItem (name);
	if (!it)
	{
		name = ArgGetConcatenatedString();
		it = FindItem(name);

		if (!it)
		{
			it = FindItemByClassname (name);
			if (!it)
			{
				ClientPrintf (ent, PRINT_HIGH, "Unknown Item\n");
				return;
			}
		}
	}

	if (!(it->Flags & ITEMFLAG_GRABBABLE))
	{
		ClientPrintf (ent, PRINT_HIGH, "Non-Pickup Item\n");
		return;
	}

	if (it->Flags & ITEMFLAG_AMMO)
	{
		CAmmo *Ammo = dynamic_cast<CAmmo*>(it);
		if (ArgCount() == 3)
			ent->client->pers.Inventory.Set (Ammo, ArgGeti(2));
		else
			ent->client->pers.Inventory.Add(Ammo, Ammo->Quantity);
	}
	else
	{
		edict_t *it_ent = G_Spawn();
		it_ent->classname = it->Classname;
		SpawnItem (it_ent, it);
		TouchItem (it_ent, ent, NULL, NULL);
		if (it_ent->inUse)
			G_FreeEdict(it_ent);
	}
}

// Paril
// This is a different style of "give".
// Allows you to spawn the item instead of giving it.
// Also, you can spawn monsters and other goodies from here.
void ED_CallSpawn (edict_t *ent);
void Cmd_Give (edict_t *ent)
{
	// Handle give all from old give.
	if (Q_stricmp(ArgGets(1), "all") == 0)
	{
		Cmd_Give_f (ent);
		return;
	}

	// Calculate spawning direction
	vec3_t Origin, Angles, forward;

	Vec3Copy (ent->s.angles, Angles);
	Angles[0] = 0; // No pitch
	Angles[2] = 0; // No roll

	Angles_Vectors (Angles, forward, NULL, NULL);
	Vec3Copy (ent->s.origin, Origin);
	Vec3MA (Origin, 150, forward, Origin);

	if (gi.pointcontents(Origin) & CONTENTS_SOLID)
		return;

	edict_t *Spawned = G_Spawn();

	Spawned->classname = ArgGetConcatenatedString();
	ED_CallSpawn (Spawned);
	if (Spawned)
	{
		Vec3Copy (Origin, Spawned->s.origin);
		Vec3Copy (Angles, Spawned->s.angles);

		gi.linkentity(Spawned);
	}
}
