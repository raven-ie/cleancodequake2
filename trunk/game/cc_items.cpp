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
	ent->s.event = EV_ITEM_RESPAWN;
}

void CBaseItem::SetRespawn (edict_t *ent, float delay)
{
	ent->flags |= FL_RESPAWN;
	ent->svFlags |= SVF_NOCLIENT;
	ent->solid = SOLID_NOT;
	ent->nextthink = level.time + delay;
	ent->think = &DoRespawn;
	gi.linkentity (ent);
}

void TouchItem (edict_t *ent, edict_t *other, plane_t *plane, cmBspSurface_t *surf);
static void DropTempTouch (edict_t *ent, edict_t *other, plane_t *plane, cmBspSurface_t *surf)
{
	if (other == ent->owner)
		return;

	TouchItem (ent, other, plane, surf);
}

static void DropMakeTouchable (edict_t *ent)
{
	ent->touch = TouchItem;
	if (deathmatch->Integer())
	{
		ent->nextthink = level.time + 29;
		ent->think = G_FreeEdict;
	}
}

edict_t *CBaseItem::DropItem (edict_t *ent)
{
	edict_t	*dropped;
	vec3_t	forward, right;
	vec3_t	offset;

	dropped = G_Spawn();

	dropped->classname = this->Classname;
	dropped->item = this;
	dropped->spawnflags = DROPPED_ITEM;
	dropped->s.effects = this->EffectFlags;
	dropped->s.renderFx = RF_GLOW;
	Vec3Set (dropped->mins, -15, -15, -15);
	Vec3Set (dropped->maxs, 15, 15, 15);
	dropped->s.modelIndex = ModelIndex(this->WorldModel);
	dropped->solid = SOLID_TRIGGER;
	dropped->movetype = MOVETYPE_TOSS;  
	dropped->touch = DropTempTouch;
	dropped->owner = ent;

	if (ent->client)
	{
		CTrace	trace;

		Angles_Vectors (ent->client->v_angle, forward, right, NULL);
		Vec3Set (offset, 24, 0, -16);
		G_ProjectSource (ent->s.origin, offset, forward, right, dropped->s.origin);
		trace.Trace (ent->s.origin, dropped->mins, dropped->maxs,
			dropped->s.origin, ent, CONTENTS_SOLID);
		Vec3Copy (trace.endPos, dropped->s.origin);
	}
	else
	{
		Angles_Vectors (ent->s.angles, forward, right, NULL);
		Vec3Copy (ent->s.origin, dropped->s.origin);
	}

	Vec3Scale (forward, 100, dropped->velocity);
	dropped->velocity[2] = 300;

	dropped->think = DropMakeTouchable;
	dropped->nextthink = level.time + 1;

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
	if (!other->client)
		return;
	if (other->health < 1)
		return;		// dead people can't pickup
	if (!(ent->item->Flags & ITEMFLAG_GRABBABLE))
		return;		// not a grabbable item?

	if (!(ent->spawnflags & ITEM_TARGETS_USED))
	{
		G_UseTargets (ent, other);
		ent->spawnflags |= ITEM_TARGETS_USED;
	}

	if (!ent->item->Pickup(ent,other))
		return;

	// flash the screen
	other->client->bonus_alpha = 0.25;	

	// show icon and name on status bar
	other->client->ps.stats[STAT_PICKUP_ICON] = ImageIndex(ent->item->Icon);
	other->client->ps.stats[STAT_PICKUP_STRING] = ent->item->GetConfigStringNumber();
	other->client->pickup_msg_time = level.time + 3.0;

	// change selected item
	if (ent->item->Flags & ITEMFLAG_USABLE)
		other->client->pers.Inventory.SelectedItem = other->client->ps.stats[STAT_SELECTED_ITEM] = ent->item->GetIndex();

	if (ent->item->PickupSound)
		Sound(other, CHAN_ITEM, SoundIndex(ent->item->PickupSound));

	if (!((coop->Integer()) &&  (ent->item->Flags & ITEMFLAG_STAY_COOP)) || (ent->spawnflags & (DROPPED_ITEM | DROPPED_PLAYER_ITEM)))
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
	ent->s.event = EV_ITEM_RESPAWN;
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
		ent->s.modelIndex = ModelIndex(ent->model);
	else
		ent->s.modelIndex = ModelIndex(ent->item->WorldModel);
	ent->solid = SOLID_TRIGGER;
	ent->movetype = MOVETYPE_TOSS;  
	ent->touch = TouchItem;

	vec3_t v = {0,0,-128};
	Vec3Add (ent->s.origin, v, dest);

	tr.Trace (ent->s.origin, ent->mins, ent->maxs, dest, ent, CONTENTS_MASK_SOLID);
	if (tr.startSolid)
	{
		//gi.dprintf ("droptofloor: %s startsolid at (%f %f %f)\n", ent->classname, ent->s.origin[0], ent->s.origin[1], ent->s.origin[2]);
		MapPrint (MAPPRINT_WARNING, ent, ent->s.origin, "Entity origin is in solid\n");
		G_FreeEdict (ent);
		return;
	}

	Vec3Copy (tr.endPos, ent->s.origin);

	if (ent->team)
	{
		ent->flags &= ~FL_TEAMSLAVE;
		ent->chain = ent->teamchain;
		ent->teamchain = NULL;

		ent->svFlags |= SVF_NOCLIENT;
		ent->solid = SOLID_NOT;
		if (ent == ent->teammaster)
		{
			ent->nextthink = level.time + FRAMETIME;
			ent->think = DoRespawn;
		}
	}

	if (ent->spawnflags & ITEM_NO_TOUCH)
	{
		ent->solid = SOLID_BBOX;
		ent->touch = NULL;
		ent->s.effects &= ~EF_ROTATE;
		ent->s.renderFx &= ~RF_GLOW;
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
			//gi.dprintf("%s at (%f %f %f) has invalid spawnflags set\n", ent->classname, ent->s.origin[0], ent->s.origin[1], ent->s.origin[2]);
			MapPrint (MAPPRINT_ERROR, ent, ent->s.origin, "Invalid spawnflags (%i, should be 0)\n", ent->spawnflags);
		}
	}

	// some items will be prevented in deathmatch
	if (deathmatch->Integer())
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

	if (coop->Integer() && (strcmp(ent->classname, "key_power_cube") == 0))
	{
		ent->spawnflags |= (1 << (8 + level.power_cubes));
		level.power_cubes++;
	}

	// don't let them drop items that stay in a coop game
	/*if ((coop->Integer()) && (item->Flags & IT_STAY_COOP))
	{
		item->drop = NULL;
	}*/

	ent->item = item;
	ent->nextthink = level.time + 2 * FRAMETIME;    // items start after other solids
	ent->think = DropItemToFloor;
	ent->s.effects = item->EffectFlags;
	ent->s.renderFx = RF_GLOW;
}