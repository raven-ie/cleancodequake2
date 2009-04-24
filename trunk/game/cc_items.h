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

typedef int EItemFlags;
enum// EItemFlags
{
	ITEMFLAG_NONE				= 0,
	ITEMFLAG_WEAPON				= 1,
	ITEMFLAG_AMMO				= 2,
	ITEMFLAG_HEALTH				= 4,
	ITEMFLAG_ARMOR				= 8,
	ITEMFLAG_STAY_COOP			= 16,
	ITEMFLAG_KEY				= 32,
	ITEMFLAG_POWERUP			= 64,
	ITEMFLAG_GRABBABLE			= 128,
	ITEMFLAG_USABLE				= 256,
	ITEMFLAG_DROPPABLE			= 512,
};

// Generic item.
// Abstract class.
// Contains the basic information that an item needs to spawn and be active.
class CBaseItem
{
private:
	int			Index;

	// IST DAS MEIN FRIENDHEID?!
	friend class CItemList;
public:
	CBaseItem (char *Classname, char *WorldModel, int EffectFlags,
			   char *PickupSound, char *Icon, char *Name, EItemFlags Flags,
			   char *Precache);

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
	virtual	void	Use (edict_t *ent) = 0;
	virtual	void	Drop (edict_t *ent) = 0;

	virtual edict_t	*DropItem (edict_t *ent);

	static void		DoRespawn (edict_t *ent);
	virtual void	SetRespawn (edict_t *ent, float delay);

	inline int		GetIndex () { return Index; };
	inline int		GetConfigStringNumber () { return CS_ITEMS+Index; };

	virtual void	Add (edict_t *ent, int quantity);

	CBaseItem		*hashClassnameNext, *hashNameNext;
	uint32 hashedClassnameValue, hashedNameValue;
};

void SpawnItem (edict_t *ent, CBaseItem *item);

#include "cc_weapons.h"
#include "cc_health.h"
#include "cc_armor.h"
#include "cc_keys.h"
#include "cc_powerups.h"
#include "cc_itemlist.h"
