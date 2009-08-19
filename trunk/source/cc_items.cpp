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
// Crammed for now, split off into filter later.
//

#include "cc_local.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn	CBaseItem::CBaseItem (char *Classname, char *WorldModel, int EffectFlags, char *PickupSound,
/// 	char *Icon, char *Name, EItemFlags Flags, char *Precache) : Index(-1), Classname(Classname),
/// 	WorldModel(WorldModel), EffectFlags(EffectFlags), PickupSound(PickupSound), Icon(Icon),
/// 	Name(Name), Flags(Flags), Precache(Precache)
///
/// \brief	Constructor. 
///
/// \author	Paril
/// \date	5/9/2009
///
/// \param	Classname	 - If non-null, the classname of the item. 
/// \param	WorldModel	 - If non-null, the world model. 
/// \param	EffectFlags	 - The effect flags. 
/// \param	PickupSound	 - If non-null, the pickup sound. 
/// \param	Icon		 - If non-null, the icon. 
/// \param	Name		 - If non-null, the name. 
/// \param	Flags		 - Item flags. 
/// \param	Precache	 - If non-null, the precache. 
////////////////////////////////////////////////////////////////////////////////////////////////////
CBaseItem::CBaseItem (char *Classname, char *WorldModel, int EffectFlags,
			   char *PickupSound, char *Icon, char *Name, EItemFlags Flags,
			   char *Precache) :
Index(-1),
Classname(Classname),
WorldModel(WorldModel),
EffectFlags(EffectFlags),
PickupSound(PickupSound),
Icon(Icon),
Name(Name),
Flags(Flags),
Precache(Precache)
{
	if (Classname)
		hashedClassnameValue = Com_HashGeneric(Classname, MAX_ITEMS_HASH);
	if (Name)
		hashedNameValue = Com_HashGeneric(Name, MAX_ITEMS_HASH);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn	void CBaseItem::SetRespawn (edict_t *ent, float delay)
///
/// \brief	Sets a respawn time on the item and makes it invisible. 
///
/// \author	Paril
/// \date	5/9/2009
///
/// \param	ent		 - If non-null, the entity to be respawned. 
/// \param	delay	 - The delay until it's respawned. 
////////////////////////////////////////////////////////////////////////////////////////////////////
void CBaseItem::SetRespawn (CItemEntity *ent, int32 delay)
{
	ent->gameEntity->flags |= FL_RESPAWN;
	ent->SetSvFlags (ent->GetSvFlags() | SVF_NOCLIENT);
	ent->SetSolid (SOLID_NOT);
	ent->NextThink = level.framenum + delay;
	ent->ThinkState = ITS_RESPAWN;
	ent->Link();
}

class CDroppedItemEntity : public CItemEntity
{
public:
	bool AvoidOwner;

	CDroppedItemEntity() :
	CItemEntity()
	{
		AvoidOwner = true;
	};
	CDroppedItemEntity(int Index) :
	CItemEntity(Index)
	{
		AvoidOwner = true;
	};

	void Touch (CBaseEntity *other, plane_t *plane, cmBspSurface_t *surf)
	{
		if (AvoidOwner && (other->gameEntity == gameEntity->owner))
			return;

		CItemEntity::Touch (other, plane, surf);
	};

	void Think ()
	{
		if (AvoidOwner)
		{
			AvoidOwner = false;
			NextThink = level.framenum + 290;
		}
		else
			Free ();
	};
};



////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn	edict_t *CBaseItem::DropItem (edict_t *ent)
///
/// \brief	Creates the item entity.
///
/// \author	Paril
/// \date	5/9/2009
///
/// \param	ent	 - If non-null, the item entity. 
///
/// \retval	null if it fails, else. 
////////////////////////////////////////////////////////////////////////////////////////////////////
CItemEntity *CBaseItem::DropItem (CBaseEntity *ent)
{
	CDroppedItemEntity	*dropped = QNew (com_levelPool, 0) CDroppedItemEntity();
	vec3f	forward, right;

	dropped->gameEntity->classname = Classname;
	dropped->gameEntity->item = this;
	dropped->gameEntity->spawnflags = DROPPED_ITEM;
	dropped->State.SetEffects (EffectFlags);
	dropped->State.SetRenderEffects (RF_GLOW);
	dropped->SetMins (vec3f(-15));
	dropped->SetMaxs (vec3f(15));
	dropped->State.SetModelIndex (ModelIndex(WorldModel));
	dropped->SetSolid (SOLID_TRIGGER);
	dropped->gameEntity->owner = ent->gameEntity;

	if (ent->EntityFlags & ENT_PLAYER)
	{
		CPlayerEntity *Player = dynamic_cast<CPlayerEntity*>(ent);
		CTrace	trace;

		vec3f (Player->Client.v_angle).ToVectors (&forward, &right, NULL);
		vec3f offset (24, 0, -16);

		vec3f result;
		G_ProjectSource (ent->State.GetOrigin(), offset, forward, right, result);

		trace = CTrace (ent->State.GetOrigin(), dropped->GetMins(), dropped->GetMaxs(),
			result, ent->gameEntity, CONTENTS_SOLID);
		dropped->State.SetOrigin (trace.endPos);
	}
	else
	{
		ent->State.GetAngles().ToVectors(&forward, &right, NULL);
		dropped->State.SetOrigin (ent->State.GetOrigin());
	}

	forward.Scale(100);
	dropped->gameEntity->velocity[0] = forward.X;
	dropped->gameEntity->velocity[1] = forward.Y;
	dropped->gameEntity->velocity[2] = 300;

	dropped->NextThink = level.framenum + 10;
	dropped->Link ();

	return dropped;
}

/*
============
SpawnItem

Sets the clipping size and plants the object on the floor.

Items can't be immediately dropped to floor, because they might
be on an entity that hasn't spawned yet.
============
*/
void TouchItem (edict_t *ent, edict_t *other, plane_t *plane, cmBspSurface_t *surf)
{
	if (!other->Entity)
		return;
	if (other->Entity && !(other->Entity->EntityFlags & ENT_PLAYER))
		return;

	if (other->health < 1)
		return;		// dead people can't pickup
	if (!(ent->item->Flags & ITEMFLAG_GRABBABLE))
		return;		// not a grabbable item?

	CPlayerEntity *Player = dynamic_cast<CPlayerEntity*>(other->Entity);

	if (!(ent->spawnflags & ITEM_TARGETS_USED))
	{
		G_UseTargets (ent, other);
		ent->spawnflags |= ITEM_TARGETS_USED;
	}

	if (!ent->item->Pickup(dynamic_cast<CItemEntity*>(ent->Entity),Player))
		return;

	// flash the screen
	Player->Client.bonus_alpha = 64;	

	// show icon and name on status bar
	Player->Client.PlayerState.SetStat(STAT_PICKUP_ICON, ent->item->IconIndex);
	Player->Client.PlayerState.SetStat(STAT_PICKUP_STRING, ent->item->GetConfigStringNumber());
	Player->Client.pickup_msg_time = level.framenum + 30;

	// change selected item
	if (ent->item->Flags & ITEMFLAG_USABLE)
	{
		Player->Client.pers.Inventory.SelectedItem = ent->item->GetIndex();
		Player->Client.PlayerState.SetStat(STAT_SELECTED_ITEM, Player->Client.pers.Inventory.SelectedItem);
	}

	if (ent->item->PickupSound)
		PlaySoundFrom(other, CHAN_ITEM, ent->item->PickupSoundIndex
#ifdef CLEANCTF_ENABLED
		, 1, (ent->item == NItems::RedFlag || ent->item == NItems::BlueFlag) ? ATTN_NONE : ATTN_NORM
#endif
		);

	//if ((game.mode != GAME_COOPERATIVE && (ent->item->Flags & ITEMFLAG_STAY_COOP)) || (ent->spawnflags & (DROPPED_ITEM | DROPPED_PLAYER_ITEM)))
	if (!((game.mode & GAME_COOPERATIVE) &&  (ent->item->Flags & ITEMFLAG_STAY_COOP)) || (ent->spawnflags & (DROPPED_ITEM | DROPPED_PLAYER_ITEM)))
	{
		if (ent->flags & FL_RESPAWN)
			ent->flags &= ~FL_RESPAWN;
		else
			G_FreeEdict (ent);
	}
}

void DoRespawn (edict_t *ent)
{
	if (ent->team)
	{
		edict_t *master;
		master = ent->teammaster;

#ifdef CLEANCTF_ENABLED
//ZOID
//in ctf, when we are weapons stay, only the master of a team of weapons
//is spawned
		if ((game.mode & GAME_CTF) &&
			dmFlags.dfWeaponsStay &&
			master->item && (master->item->Flags & ITEMFLAG_WEAPON))
			ent = master;
		else
		{
			int     count;
//ZOID
#endif
			for (count = 0, ent = master; ent; ent = ent->chain, count++)
				;

			int choice = rand() % count;

			for (count = 0, ent = master; count < choice; ent = ent->chain, count++)
				;
#ifdef CLEANCTF_ENABLED
		}
#endif
	}

	ent->svFlags &= ~SVF_NOCLIENT;
	ent->solid = SOLID_TRIGGER;
	gi.linkentity (ent);

	// send an effect
	ent->state.event = EV_ITEM_RESPAWN;
}

void Use_Item (edict_t *ent, edict_t *other, edict_t *activator)
{
	ent->svFlags &= ~SVF_NOCLIENT;
	ent->use = NULL;

	if (ent->spawnflags & ITEM_NO_TOUCH)
	{
		ent->solid = SOLID_BBOX;
		ent->touch = NULL;
	}
	else
	{
		ent->solid = SOLID_TRIGGER;
		ent->touch = TouchItem;
	}

	gi.linkentity (ent);
}


////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn	inline int CBaseItem::GetIndex ()
///
/// \brief	Gets the item's index. 
///
/// \author	Paril
/// \date	5/9/2009
///
/// \retval	The index. 
////////////////////////////////////////////////////////////////////////////////////////////////////
inline int CBaseItem::GetIndex ()
{
	return Index;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn	inline int CBaseItem::GetConfigStringNumber ()
///
/// \brief	Gets the ConfigString number for the item. 
///
/// \author	Paril
/// \date	5/9/2009
///
/// \retval	The ConfigString number number. 
////////////////////////////////////////////////////////////////////////////////////////////////////
inline int CBaseItem::GetConfigStringNumber ()
{
	return CS_ITEMS+Index;
}