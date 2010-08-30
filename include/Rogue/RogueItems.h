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
// cc_rogue_items.h
// 
//

#if !defined(CC_GUARD_CC_ROGUE_ITEMS_H) || !INCLUDE_GUARDS
#define CC_GUARD_CC_ROGUE_ITEMS_H

#if ROGUE_FEATURES
void AddRogueItemsToList ();

// Double Damage
class CDoubleDamage : public CBasePowerUp
{
public:
	CDoubleDamage (const char *Classname, const char *WorldModel, sint32 EffectFlags,
			   const char *PickupSound, const char *Icon, const char *Name, EItemFlags Flags,
			   EPowerupFlags PowerupFlags);

	void DoPickup (class CItemEntity *Item, CPlayerEntity *Other);
	void Use (CPlayerEntity *Player);
};

// Infrared Gogglinos
class CIRGoggles : public CBasePowerUp
{
public:
	CIRGoggles (const char *Classname, const char *WorldModel, sint32 EffectFlags,
			   const char *PickupSound, const char *Icon, const char *Name, EItemFlags Flags,
			   EPowerupFlags PowerupFlags);

	void DoPickup (class CItemEntity *Item, CPlayerEntity *Other);
	void Use (CPlayerEntity *Player);
};

// Nuke
class CAMBomb : public CAmmo
{
public:
	CAMBomb (const char *Classname, const char *WorldModel, const char *Icon, const char *Name);

	bool	Pickup (class CItemEntity *Item, CPlayerEntity *Other);
	void	Use (CPlayerEntity *Player);
	void	Drop (CPlayerEntity *Player);
};

#endif

#else
FILE_WARNING
#endif