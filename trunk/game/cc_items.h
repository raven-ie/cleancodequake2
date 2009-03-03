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
// cc_items.h
// New, improved, better, stable item system!
//

// Testing purposes, do not remove!
/*
typedef struct gitem_s
{
	char		*classname;	// spawning name
	bool		(*pickup)(struct edict_s *ent, struct edict_s *other);
	void		(*use)(struct edict_s *ent, struct gitem_s *item);
	void		(*drop)(struct edict_s *ent, struct gitem_s *item);
	void		(*weaponthink)(struct edict_s *ent);
	char		*pickup_sound;
	char		*world_model;
	int			world_model_flags;
	char		*view_model;

	// client side info
	char		*icon;
	char		*pickup_name;	// for printing on pickup
	int			count_width;		// number of digits to display by icon

	int			quantity;		// for ammo how much, for weapons how much is used per shot
	char		*ammo;			// for weapons
	int			flags;			// IT_* flags

	int			weapmodel;		// weapon model index (for weapons)

	void		*info;
	int			tag;

	char		*precaches;		// string of all models, sounds, and images this item will use
} gitem_t;
*/

// Named not to interfere with IT_
enum EItemFlags
{
	ITEMFLAG_WEAPON				= 1,
	ITEMFLAG_AMMO				= 2,
	ITEMFLAG_HEALTH				= 4,
	ITEMFLAG_ARMOR				= 8,
	ITEMFLAG_STAY_COOP			= 16,
	ITEMFLAG_KEY				= 32,
	ITEMFLAG_POWERUP			= 64
};

// Generic item.
// Abstract class.
// Contains the basic information that an item needs to spawn and be active.
class CBaseItem
{
private:
	int			Index;

public:
	CBaseItem ();
	CBaseItem (int _Index) { Index = _Index; };

	char		*Classname;
	char		*WorldModel;
	int			EffectFlags;

	char		*PickupSound;

	char		*Icon;
	char		*Name;

	EItemFlags	Flags;

	// I don't necessarily like the fact that the precache
	// list here is parsed. Anyone have a better idea? :S
	char		*Precache;

	// Functions
	virtual	bool	Pickup (edict_t *ent, edict_t *other) = 0;
	virtual	void	Use (edict_t *ent, CBaseItem *Item) = 0;
	virtual	void	Drop (edict_t *ent, CBaseItem *Item) = 0;

	int			GetIndex () { return Index; };
};

// Class for weapon items.
// Named not to interfere with the CWeapon system, when
// it is completed.
// This class is missing ViewWeapon and the like
// because the CWeapon system will handle all of that in the weapon
// class itself. That way we don't have a bunch of useless SHIT spread
// all around the source code.
class CWeaponItem : public CBaseItem
{
public:
	CWeaponItem(int _Index) : CBaseItem(_Index) {};
	// Commented for now
	//CBaseWeapon		*Weapon;

	// Until CWeapon system is done, weapons act like NULL items.
	bool	Pickup (edict_t *ent, edict_t *other) { return false; };
	void	Use (edict_t *ent, CBaseItem *Item) {};
	void	Drop (edict_t *ent, CBaseItem *Item) {};
};

// Class for ammo.
class CAmmo : public CBaseItem
{
public:
	CAmmo(int _Index) : CBaseItem(_Index) {};
	int			Quantity; // Number gotten when we pick this mother upper

	// Only thing different about ammo is how it's picked up.
	bool	Pickup (edict_t *ent, edict_t *other) { return false; };
	void	Use (edict_t *ent, CBaseItem *Item) {};
	void	Drop (edict_t *ent, CBaseItem *Item) {};
};

// Class for powerups.
// Powerups are funny because they are.
class CPowerUp : public CBaseItem
{
public:
	CPowerUp(int _Index) : CBaseItem(_Index) {};
	int			Time; // Time the powerup lasts for

	bool	Pickup (edict_t *ent, edict_t *other) { return false; };
	void	Use (edict_t *ent, CBaseItem *Item) {};
	void	Drop (edict_t *ent, CBaseItem *Item) {};
};

// Class for health.
class CHealth : public CBaseItem
{
public:
	CHealth(int _Index) : CBaseItem(_Index) {};
	int			Amount; // You spin me right round baby right round

	bool	Pickup (edict_t *ent, edict_t *other) { return false; };
	void	Use (edict_t *ent, CBaseItem *Item) {};
	void	Drop (edict_t *ent, CBaseItem *Item) {};
};

// Class for keys.
class CKey : public CBaseItem
{
public:
	CKey(int _Index) : CBaseItem(_Index) {};

	bool	Pickup (edict_t *ent, edict_t *other) { return false; };
	void	Use (edict_t *ent, CBaseItem *Item) {};
	void	Drop (edict_t *ent, CBaseItem *Item) {};
};


// Just to conform to the configstrings
#define MAX_ITEMS		256
#define MAX_ITEMS_HASH	(MAX_COMMANDS/2)

// Generic itemlist.
class CItemList
{
public:
	void FillBasicData (char *Classname, char *WorldModel, int EffectFlags, char *PickupFlags, char *Icon, char *Name, EItemFlags Flags, char *Precache);
	void AddWeapon (char *Classname, char *WorldModel, int EffectFlags, char *PickupFlags, char *Icon, char *Name, EItemFlags Flags, char *Precache); //, CBaseWeapon *Weapon);
	void AddAmmo (char *Classname, char *WorldModel, int EffectFlags, char *PickupFlags, char *Icon, char *Name, EItemFlags Flags, char *Precache, int Quantity);
	void AddPowerup (char *Classname, char *WorldModel, int EffectFlags, char *PickupFlags, char *Icon, char *Name, EItemFlags Flags, char *Precache, int Time);
	void AddHealth (char *Classname, char *WorldModel, int EffectFlags, char *PickupFlags, char *Icon, char *Name, EItemFlags Flags, char *Precache, int Amount);
	void AddKey (char *Classname, char *WorldModel, int EffectFlags, char *PickupFlags, char *Icon, char *Name, EItemFlags Flags, char *Precache);

	void SendItemNames ();

	int			numItems;
	// FIXME: Use dynanamic memory? Good idea? :S
	CBaseItem	*Items[MAX_ITEMS];

	// There are two hash tables for items; hashed by classname, and hashed by item name.
	CBaseItem	*HashedClassnameItemList[MAX_ITEMS_HASH];
	CBaseItem	*HashedNameItemList[MAX_ITEMS_HASH];
};

void InitItemlist ();