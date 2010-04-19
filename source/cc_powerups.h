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
// cc_powerups.h
// Powerups
//

#if !defined(CC_GUARD_POWERUPS_H) || !INCLUDE_GUARDS
#define CC_GUARD_POWERUPS_H

// Class for powerups.
// Powerups are funny because they are
CC_ENUM (uint8, EPowerupFlags)
{
	POWERFLAG_STORE			=	BIT(0),
	POWERFLAG_STACK			=	BIT(1),
	POWERFLAG_BUTNOTINCOOP	=	BIT(2), // must be ORed with stack
};

class CBasePowerUp : public CBaseItem
{
public:
	EPowerupFlags PowerupFlags;

	CBasePowerUp (const char *Classname, const char *WorldModel, sint32 EffectFlags,
			   const char *PickupSound, const char *Icon, const char *Name, EItemFlags Flags,
			   const char *Precache, EPowerupFlags PowerupFlags);

	virtual bool	Pickup (class CItemEntity *Item, CPlayerEntity *Other);
	virtual void	Use (CPlayerEntity *Player);
	virtual void	Drop (CPlayerEntity *Player);

	virtual void	DoPickup (class CItemEntity *Item, CPlayerEntity *Other);
};

// Mega Health powerup
class CMegaHealth : public CBasePowerUp
{
public:
	CMegaHealth (const char *Classname, const char *WorldModel, sint32 EffectFlags,
			   const char *PickupSound, const char *Icon, const char *Name, EItemFlags Flags,
			   const char *Precache, EPowerupFlags PowerupFlags);

	void DoPickup (class CItemEntity *Item, CPlayerEntity *Other);
};

// Backpack powerup
class CBackPack : public CBasePowerUp
{
public:
	CBackPack (const char *Classname, const char *WorldModel, sint32 EffectFlags,
			   const char *PickupSound, const char *Icon, const char *Name, EItemFlags Flags,
			   const char *Precache, EPowerupFlags PowerupFlags);

	void DoPickup (class CItemEntity *Item, CPlayerEntity *Other);
};

// Quad Damage
class CQuadDamage : public CBasePowerUp
{
public:
	CQuadDamage (const char *Classname, const char *WorldModel, sint32 EffectFlags,
			   const char *PickupSound, const char *Icon, const char *Name, EItemFlags Flags,
			   const char *Precache, EPowerupFlags PowerupFlags);

	void DoPickup (class CItemEntity *Item, CPlayerEntity *Other);
	void Use (CPlayerEntity *Player);
};

// Invul
class CInvulnerability : public CBasePowerUp
{
public:
	CInvulnerability (const char *Classname, const char *WorldModel, sint32 EffectFlags,
			   const char *PickupSound, const char *Icon, const char *Name, EItemFlags Flags,
			   const char *Precache, EPowerupFlags PowerupFlags);

	void DoPickup (class CItemEntity *Item, CPlayerEntity *Other);
	void Use (CPlayerEntity *Player);
};

// Sil�ncier
class CSilencer : public CBasePowerUp
{
public:
	CSilencer (const char *Classname, const char *WorldModel, sint32 EffectFlags,
			   const char *PickupSound, const char *Icon, const char *Name, EItemFlags Flags,
			   const char *Precache, EPowerupFlags PowerupFlags);

	void DoPickup (class CItemEntity *Item, CPlayerEntity *Other);
	void Use (CPlayerEntity *Player);
};

// Rebreather (verb, "to breathe again")
class CRebreather : public CBasePowerUp
{
public:
	CRebreather (const char *Classname, const char *WorldModel, sint32 EffectFlags,
			   const char *PickupSound, const char *Icon, const char *Name, EItemFlags Flags,
			   const char *Precache, EPowerupFlags PowerupFlags);

	void DoPickup (class CItemEntity *Item, CPlayerEntity *Other);
	void Use (CPlayerEntity *Player);
};

// Rebreather (verb, "to breathe again")
class CEnvironmentSuit : public CBasePowerUp
{
public:
	CEnvironmentSuit (const char *Classname, const char *WorldModel, sint32 EffectFlags,
			   const char *PickupSound, const char *Icon, const char *Name, EItemFlags Flags,
			   const char *Precache, EPowerupFlags PowerupFlags);

	void DoPickup (class CItemEntity *Item, CPlayerEntity *Other);
	void Use (CPlayerEntity *Player);
};

// Bandolier
class CBandolier : public CBasePowerUp
{
public:
	CBandolier (const char *Classname, const char *WorldModel, sint32 EffectFlags,
			   const char *PickupSound, const char *Icon, const char *Name, EItemFlags Flags,
			   const char *Precache, EPowerupFlags PowerupFlags);

	void DoPickup (class CItemEntity *Item, CPlayerEntity *Other);
};

// Adrenaline
class CAdrenaline : public CBasePowerUp
{
public:
	CAdrenaline (const char *Classname, const char *WorldModel, sint32 EffectFlags,
			   const char *PickupSound, const char *Icon, const char *Name, EItemFlags Flags,
			   const char *Precache, EPowerupFlags PowerupFlags);

	void DoPickup (class CItemEntity *Item, CPlayerEntity *Other);
};

// Ancient Head
class CAncientHead : public CBasePowerUp
{
public:
	CAncientHead (const char *Classname, const char *WorldModel, sint32 EffectFlags,
			   const char *PickupSound, const char *Icon, const char *Name, EItemFlags Flags,
			   const char *Precache, EPowerupFlags PowerupFlags);

	void DoPickup (class CItemEntity *Item, CPlayerEntity *Other);
};

// Power Shield
class CPowerShield : public CBasePowerUp
{
public:
	CPowerShield (const char *Classname, const char *WorldModel, sint32 EffectFlags,
			   const char *PickupSound, const char *Icon, const char *Name, EItemFlags Flags,
			   const char *Precache, EPowerupFlags PowerupFlags);

	void DoPickup (class CItemEntity *Item, CPlayerEntity *Other);
	void Use (CPlayerEntity *Player);
	void Drop (CPlayerEntity *Player);
};

void AddPowerupsToList();

#else
FILE_WARNING
#endif
