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
// cc_itemlist.cpp
// Itemlist
// Basically the link between items and the real world
//

#include "cc_local.h"

CItemList *ItemList;

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

void CItemList::SendItemNames ()
{
	for (int i = 0 ; i < this->numItems ; i++)
		gi.configstring (this->Items[i]->GetConfigStringNumber(), this->Items[i]->Name);
}

bool ItemExists (edict_t *ent)
{
	// Check through the itemlist
	uint32 hash = Com_HashGeneric(ent->classname, MAX_ITEMS_HASH);
	CBaseItem *Item;

	for (Item = ItemList->HashedClassnameItemList[hash]; Item; Item=Item->hashClassnameNext)
	{
		if (Q_stricmp(Item->Classname, ent->classname) == 0)
		{
			SpawnItem (ent, Item);
			return true;
		}
	}
	return false;
}

CBaseItem *FindItem (char *name)
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

CBaseItem *FindItemByClassname (char *name)
{
	// Check through the itemlist
	uint32 hash = Com_HashGeneric(name, MAX_ITEMS_HASH);
	CBaseItem *Item;

	for (Item = ItemList->HashedClassnameItemList[hash]; Item; Item=Item->hashClassnameNext)
	{
		if (Q_stricmp(Item->Classname, name) == 0)
			return Item;
	}
	return NULL;
}

CBaseItem *GetItemByIndex (int Index)
{
	if (!ItemList->Items[Index] || Index >= MAX_CS_ITEMS || Index < 0)
		return NULL;
	return ItemList->Items[Index];
}

inline int GetNumItems ()
{
	return ItemList->numItems;
}

void InitItemlist ()
{
	ItemList = new CItemList;

	AddAmmoToList();
	AddHealthToList();
	AddArmorToList();
	AddPowerupsToList();
}

void SetItemNames ()
{
	ItemList->SendItemNames();
}
