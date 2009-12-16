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
// cc_inventory.h
// Inventory
//

#if !defined(__CC_INVENTORY_H__) || !INCLUDE_GUARDS
#define __CC_INVENTORY_H__

class CInventory
{
private:
	typedef std::map <uint8, sint16, std::less<uint8>, std::generic_allocator<std::pair<uint8, uint16 > > > TInventoryMapType;
	TInventoryMapType Array;

public:
	sint32		SelectedItem;

	void	operator += (CBaseItem *Item);
	void	operator += (uint8 Index);
	void	operator -= (CBaseItem *Item);
	void	operator -= (uint8 Index);

	CInventory ();
	~CInventory ();

	// Note: All of these functions are for directly
	// adding items. This will NOT call the pickup
	// function! You need to add your own checks in for
	// ammo and such if you use these functions.
	void	Add (CBaseItem *Item, sint32 Num);
	void	Remove (CBaseItem *Item, sint32 Num);
	void	Add (uint8 Index, sint32 Num);
	void	Remove (uint8 Index, sint32 Num);

	// Draw the inventory
	void	Draw (CPlayerEntity *ent);

	// Validate items selected with functions below
	void	ValidateSelectedItem ();

	void	SelectNextItem (EItemFlags Flags);
	void	SelectPrevItem (EItemFlags Flags);

	sint32		Has	(uint8 Index);
	sint32		Has	(CBaseItem *Item);

	void	Set	(CBaseItem *Item, sint32 Num);
	void	Set	(uint8 Index, sint32 Num);

	void Save (CFile &File)
	{
		File.Write<size_t> (Array.size());

		for (TInventoryMapType::iterator it = Array.begin(); it != Array.end(); it++)
			File.Write <std::pair <uint8, uint16> > (*it);

		File.Write<sint32> (SelectedItem);
	};

	void Load (CFile &File)
	{
		size_t size = File.Read<size_t> ();

		for (size_t i = 0; i < size; i++)
			Array.insert (File.Read <std::pair <uint8, uint16> > ());

		SelectedItem = File.Read<sint32> ();
	};

	void Clear ()
	{
		Array.clear();
		SelectedItem = -1;
	};
};

void Cmd_Use_f (CPlayerEntity *ent);
void Cmd_UseList_f (CPlayerEntity *ent);
void Cmd_Drop_f (CPlayerEntity *ent);
void Cmd_Inven_f (CPlayerEntity *ent);
void Cmd_InvUse_f (CPlayerEntity *ent);
void Cmd_WeapPrev_f (CPlayerEntity *ent);
void Cmd_WeapNext_f (CPlayerEntity *ent);
void Cmd_WeapLast_f (CPlayerEntity *ent);
void Cmd_InvDrop_f (CPlayerEntity *ent);
void Cmd_SelectNextItem_f (CPlayerEntity *ent);
void Cmd_SelectPrevItem_f (CPlayerEntity *ent);
void Cmd_SelectNextWeapon_f (CPlayerEntity *ent);
void Cmd_SelectPrevWeapon_f (CPlayerEntity *ent);
void Cmd_SelectNextPowerup_f (CPlayerEntity *ent);
void Cmd_SelectPrevPowerup_f (CPlayerEntity *ent);
void Cmd_Give (CPlayerEntity *ent);
void Cmd_Give_f (CPlayerEntity *ent);

#else
FILE_WARNING
#endif