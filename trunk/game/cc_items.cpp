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
/// \fn	void CBaseItem::DoRespawn (edict_t *ent)
///
/// \brief	Executes the respawn operation. 
///
/// \author	Paril
/// \date	5/9/2009
///
/// \param	ent	 - If non-null, the entity to be respawned. 
////////////////////////////////////////////////////////////////////////////////////////////////////
void CBaseItem::DoRespawn (edict_t *ent)
{
	if (ent->team)
	{
		edict_t	*master;
		int	count;
		int choice;

		master = ent->teammaster;

		for (count = 0, ent = master; ent; ent = ent->chain, count++)
			;

		choice = rand() % count;

		for (count = 0, ent = master; count < choice; ent = ent->chain, count++)
			;
	}

	ent->svFlags &= ~SVF_NOCLIENT;
	ent->solid = SOLID_TRIGGER;
	gi.linkentity (ent);

	// send an effect
	ent->state.event = EV_ITEM_RESPAWN;
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
void CBaseItem::SetRespawn (edict_t *ent, float delay)
{
	ent->flags |= FL_RESPAWN;
	ent->svFlags |= SVF_NOCLIENT;
	ent->solid = SOLID_NOT;
	ent->nextthink = level.framenum + (delay * 10);
	ent->think = &DoRespawn;
	gi.linkentity (ent);
}

static void DropTempTouch (edict_t *ent, edict_t *other, plane_t *plane, cmBspSurface_t *surf)
{
	if (other == ent->owner)
		return;

	TouchItem (ent, other, plane, surf);
}

static void DropMakeTouchable (edict_t *ent)
{
	ent->touch = TouchItem;
	if (game.mode & GAME_DEATHMATCH)
	{
		ent->nextthink = level.framenum + 290;
		ent->think = G_FreeEdict;
	}
}

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
edict_t *CBaseItem::DropItem (edict_t *ent)
{
	edict_t	*dropped;
	vec3_t	forward, right;
	vec3_t	offset;

	dropped = G_Spawn();

	dropped->classname = Classname;
	dropped->item = this;
	dropped->spawnflags = DROPPED_ITEM;
	dropped->state.effects = EffectFlags;
	dropped->state.renderFx = RF_GLOW;
	Vec3Set (dropped->mins, -15, -15, -15);
	Vec3Set (dropped->maxs, 15, 15, 15);
	dropped->state.modelIndex = ModelIndex(WorldModel);
	dropped->solid = SOLID_TRIGGER;
	dropped->movetype = MOVETYPE_TOSS;  
	dropped->touch = DropTempTouch;
	dropped->owner = ent;

	if (ent->Entity && (ent->Entity->EntityFlags & ENT_PLAYER))
	{
		CPlayerEntity *Player = dynamic_cast<CPlayerEntity*>(ent->Entity);
		CTrace	trace;

		Angles_Vectors (Player->Client.v_angle, forward, right, NULL);
		Vec3Set (offset, 24, 0, -16);
		G_ProjectSource (ent->state.origin, offset, forward, right, dropped->state.origin);
		trace = CTrace (ent->state.origin, dropped->mins, dropped->maxs,
			dropped->state.origin, ent, CONTENTS_SOLID);
		Vec3Copy (trace.endPos, dropped->state.origin);
	}
	else
	{
		Angles_Vectors (ent->state.angles, forward, right, NULL);
		Vec3Copy (ent->state.origin, dropped->state.origin);
	}

	Vec3Scale (forward, 100, dropped->velocity);
	dropped->velocity[2] = 300;

	dropped->think = DropMakeTouchable;
	dropped->nextthink = level.framenum + 10;

	gi.linkentity (dropped);

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

	if (!ent->item->Pickup(ent,Player))
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
		, 1, (ent->item == RedFlag || ent->item == BlueFlag) ? ATTN_NONE : ATTN_NORM
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
		int     count;
		int choice;

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
//ZOID
#endif
			for (count = 0, ent = master; ent; ent = ent->chain, count++)
				;

			choice = rand() % count;

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

void DropItemToFloor (edict_t *ent)
{
	CTrace	tr;
	vec3_t		dest;

	ent->mins[0] = ent->mins[1] = ent->mins[2] = -15;
	ent->maxs[0] = ent->maxs[1] = ent->maxs[2] = 15;

	if (ent->model)
		ent->state.modelIndex = ModelIndex(ent->model);
	else
		ent->state.modelIndex = ModelIndex(ent->item->WorldModel);
	ent->solid = SOLID_TRIGGER;
	ent->movetype = MOVETYPE_TOSS;  
	ent->touch = TouchItem;

	vec3_t v = {0,0,-128};
	Vec3Add (ent->state.origin, v, dest);

	tr = CTrace (ent->state.origin, ent->mins, ent->maxs, dest, ent, CONTENTS_MASK_SOLID);
	if (tr.startSolid)
	{
		//gi.dprintf ("droptofloor: %s startsolid at (%f %f %f)\n", ent->classname, ent->state.origin[0], ent->state.origin[1], ent->state.origin[2]);
		MapPrint (MAPPRINT_WARNING, ent, ent->state.origin, "Entity origin is in solid\n");
		G_FreeEdict (ent);
		return;
	}

	Vec3Copy (tr.endPos, ent->state.origin);

	if (ent->team)
	{
		ent->flags &= ~FL_TEAMSLAVE;
		ent->chain = ent->teamchain;
		ent->teamchain = NULL;

		ent->svFlags |= SVF_NOCLIENT;
		ent->solid = SOLID_NOT;
		if (ent == ent->teammaster)
		{
			ent->nextthink = level.framenum + FRAMETIME;
			ent->think = DoRespawn;
		}
	}

	if (ent->spawnflags & ITEM_NO_TOUCH)
	{
		ent->solid = SOLID_BBOX;
		ent->touch = NULL;
		ent->state.effects &= ~EF_ROTATE;
		ent->state.renderFx &= ~RF_GLOW;
	}

	if (ent->spawnflags & ITEM_TRIGGER_SPAWN)
	{
		ent->svFlags |= SVF_NOCLIENT;
		ent->solid = SOLID_NOT;
		ent->use = Use_Item;
	}

	gi.linkentity (ent);
}

void SpawnItem (edict_t *ent, CBaseItem *item)
{
	//PrecacheItem (item);

	if (ent->spawnflags)
	{
		if (strcmp(ent->classname, "key_power_cube") != 0)
		{
			ent->spawnflags = 0;
			//gi.dprintf("%s at (%f %f %f) has invalid spawnflags set\n", ent->classname, ent->state.origin[0], ent->state.origin[1], ent->state.origin[2]);
			MapPrint (MAPPRINT_ERROR, ent, ent->state.origin, "Invalid spawnflags (%i, should be 0)\n", ent->spawnflags);
		}
	}

	// some items will be prevented in deathmatch
	if (game.mode & GAME_DEATHMATCH)
	{
		if (dmFlags.dfNoArmor)
		{
			//if (item->pickup == Pickup_Armor || item->pickup == Pickup_PowerArmor)
			if (item->Flags & ITEMFLAG_ARMOR)
			{
				G_FreeEdict (ent);
				return;
			}
		}
		if (dmFlags.dfNoItems)
		{
			//if (item->pickup == Pickup_Powerup)
			if (item->Flags & ITEMFLAG_POWERUP)
			{
				G_FreeEdict (ent);
				return;
			}
		}
		if (dmFlags.dfNoHealth)
		{
			//if (item->pickup == Pickup_Health || item->pickup == Pickup_Adrenaline || item->pickup == Pickup_AncientHead)
			if (item->Flags & ITEMFLAG_HEALTH)
			{
				G_FreeEdict (ent);
				return;
			}
		}
		if (dmFlags.dfInfiniteAmmo)
		{
			//if ( (item->flags == IT_AMMO) || (strcmp(ent->classname, "weapon_bfg") == 0) )
			if (item->Flags & ITEMFLAG_AMMO)
			{
				G_FreeEdict (ent);
				return;
			}
		}
	}

	if ((game.mode == GAME_COOPERATIVE) && (strcmp(ent->classname, "key_power_cube") == 0))
	{
		ent->spawnflags |= (1 << (8 + level.power_cubes));
		level.power_cubes++;
	}

	// don't let them drop items that stay in a coop game
	/*if ((coop->Integer()) && (item->Flags & IT_STAY_COOP))
	{
		item->drop = NULL;
	}*/

#ifdef CLEANCTF_ENABLED
//ZOID
//Don't spawn the flags unless enabled
	if (!(game.mode & GAME_CTF) &&
		(strcmp(ent->classname, "item_flag_team1") == 0 ||
		strcmp(ent->classname, "item_flag_team2") == 0))
	{
		G_FreeEdict(ent);
		return;
	}
//ZOID
#endif

	ent->item = item;
	ent->nextthink = level.framenum + 2;    // items start after other solids
	ent->think = DropItemToFloor;
	ent->state.effects = item->EffectFlags;
	ent->state.renderFx = RF_GLOW;

#ifdef CLEANCTF_ENABLED
//ZOID
//flags are server animated and have special handling
	if (strcmp(ent->classname, "item_flag_team1") == 0 ||
		strcmp(ent->classname, "item_flag_team2") == 0) {
		ent->think = CTFFlagSetup;
	}
//ZOID
#endif
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