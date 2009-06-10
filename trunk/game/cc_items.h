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
#ifdef CLEANCTF_ENABLED
	ITEMFLAG_TECH				= 1024,
#endif
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class	CBaseItem
///
/// \brief	The base item class. Contains the basic information that an item needs to spawn and be active. 
///
/// \author	Paril
/// \date	5/9/2009
////////////////////////////////////////////////////////////////////////////////////////////////////
class CBaseItem
{
private:
	/// The index of this item in the item list. 
	int			Index;

	/// Must be friends with itemlist so it can set the item.
	friend class CItemList;
public:
	/// The next hash index (classname)
	CBaseItem		*hashClassnameNext;
	/// The next hash index (name)
	CBaseItem		*hashNameNext;
	/// Hashed classname value
	uint32 hashedClassnameValue;
	/// Hashed name value
	uint32 hashedNameValue;

	CBaseItem (char *Classname, char *WorldModel, int EffectFlags,
			   char *PickupSound, char *Icon, char *Name, EItemFlags Flags,
			   char *Precache);

	/// Classname (for maps)
	char		*Classname;
	/// World model
	char		*WorldModel;
	/// Effect flags (EF_ROTATE, etc)
	int			EffectFlags;

	/// The sound on pickup
	char		*PickupSound;
	int			PickupSoundIndex;

	/// HUD Icon
	char		*Icon;
	MediaIndex	IconIndex;
	/// Name on pickup
	char		*Name;

	/// Item flags
	EItemFlags	Flags;

	/// Precached sounds/models/images.
	/// I don't necessarily like the fact that the precache
	/// list here is parsed. Anyone have a better idea? :S
	char		*Precache;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// \fn	virtual bool Pickup (edict_t *ent, edict_t *other) = 0
	///
	/// \brief	Attempts to pickup the item. 
	///
	/// \author	Paril
	/// \date	5/9/2009
	///
	/// \param	ent		 - If non-null, the item entity. 
	/// \param	other	 - If non-null, the player who picked the item up. 
	///
	/// \retval	true if it succeeds, false if it fails. 
	////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual	bool	Pickup (edict_t *ent, edict_t *other) = 0;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// \fn	virtual void Use (edict_t *ent) = 0
	///
	/// \brief	Attempts to uses the item. 
	///
	/// \author	Paril
	/// \date	5/9/2009
	///
	/// \param	ent	 - If non-null, the entity that used the item. 
	////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual	void	Use (edict_t *ent) = 0;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// \fn	virtual void Drop (edict_t *ent) = 0
	///
	/// \brief	Attempts to drops the item. 
	///
	/// \author	Paril
	/// \date	5/9/2009
	///
	/// \param	ent	 - If non-null, the ent. 
	////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual	void	Drop (edict_t *ent) = 0;
	virtual edict_t	*DropItem (edict_t *ent);

	static void		DoRespawn (edict_t *ent);
	virtual void	SetRespawn (edict_t *ent, float delay);

	inline int		GetIndex ();
	inline int		GetConfigStringNumber ();

	virtual void	Add (CPlayerEntity *ent, int quantity);
};

void SpawnItem (edict_t *ent, CBaseItem *item);
void TouchItem (edict_t *ent, edict_t *other, plane_t *plane, cmBspSurface_t *surf);

#include "cc_weapons.h"
#include "cc_health.h"
#include "cc_armor.h"
#include "cc_keys.h"
#include "cc_powerups.h"
#include "cc_itemlist.h"
#ifdef CLEANCTF_ENABLED
#include "cc_ctfitems.h"
#endif