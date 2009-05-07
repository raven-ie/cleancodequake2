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

	gi.WriteByte (SVC_INVENTORY);
	for (int i=0 ; i<MAX_CS_ITEMS ; i++)
		gi.WriteShort (Array[i]);
	gi.unicast (ent, true);
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
	char *s = gi.args();
	CBaseItem *Item = FindItem(s);
	if (!Item)
	{
		Item = FindItemByClassname(s);

		if (!Item)
		{
			gi.cprintf (ent, PRINT_HIGH, "Unknown item: %s\n", s);
			return;
		}
	}
	if (!(Item->Flags & ITEMFLAG_USABLE))
	{
		gi.cprintf (ent, PRINT_HIGH, "Item is not usable.\n");
		return;
	}
	if (!ent->client->pers.Inventory.Has(Item))
	{
		gi.cprintf (ent, PRINT_HIGH, "Out of item: %s\n", s);
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
	char *s = gi.args();
	CBaseItem *Item = FindItem(s);
	if (!Item)
	{
		Item = FindItemByClassname(s);

		if (!Item)
		{
			gi.cprintf (ent, PRINT_HIGH, "Unknown item: %s\n", s);
			return;
		}
	}
	if (!(Item->Flags & ITEMFLAG_DROPPABLE))
	{
		gi.cprintf (ent, PRINT_HIGH, "Item is not dropable.\n");
		return;
	}
	if (!ent->client->pers.Inventory.Has(Item))
	{
		gi.cprintf (ent, PRINT_HIGH, "Out of item: %s\n", s);
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
		gi.cprintf (ent, PRINT_HIGH, "No item to use.\n");
		return;
	}

	CBaseItem *it = GetItemByIndex(ent->client->pers.Inventory.SelectedItem);
	if (!(it->Flags & ITEMFLAG_USABLE))
	{
		gi.cprintf (ent, PRINT_HIGH, "Item is not usable.\n");
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
		int index = (selectedWeaponIndex + i)%MAX_CS_ITEMS;
		if (!ent->client->pers.Inventory.Has(index))
			continue;
		CBaseItem *Item = GetItemByIndex(index);
		if (!(Item->Flags & ITEMFLAG_USABLE))
			continue;
		if (! (Item->Flags & ITEMFLAG_WEAPON) )
			continue;
		Item->Use (ent);
		if (ent->client->NewWeapon->Item == Item || ent->client->pers.Weapon->Item == Item)
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
		int index = (selectedWeaponIndex + MAX_CS_ITEMS - i)%MAX_CS_ITEMS;
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
		gi.cprintf (ent, PRINT_HIGH, "No item to drop.\n");
		return;
	}

	CBaseItem *Item = GetItemByIndex(ent->client->pers.Inventory.SelectedItem);
	if (!(Item->Flags & ITEMFLAG_DROPPABLE))
	{
		gi.cprintf (ent, PRINT_HIGH, "Item is not dropable.\n");
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