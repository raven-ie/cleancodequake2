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
// cc_utils.cpp
// g_utils, CleanCode style
//

#include "cc_local.h"

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
// g_utils.c -- misc utility functions for game module

#include "g_local.h"

void G_ProjectSource (vec3f &point, vec3f &distance, vec3f &forward, vec3f &right, vec3f &result)
{
	result.Set (point.X + forward.X * distance.X + right.X * distance.Y,
				point.Y + forward.Y * distance.X + right.Y * distance.Y,
				point.Z + forward.Z * distance.X + right.Z * distance.Y + distance.Z);
}


CBaseEntity *FindRadius (CBaseEntity *From, vec3f &org, int Radius, uint32 EntityFlags)
{
	vec3f	eOrigin;

	edict_t *from;
	if (!From)
		from = g_edicts;
	else
	{
		from = From->gameEntity;
		from++;
	}

	for ( ; from < &g_edicts[globals.numEdicts]; from++)
	{
		if (!from->inUse)
			continue;
		if (from->solid == SOLID_NOT)
			continue;
		if (!from->Entity)
			continue;
		if (!(from->Entity->EntityFlags & EntityFlags))
			continue;
		
		eOrigin.X = org.X - (from->state.origin[0] + (from->mins[0] + from->maxs[0]) * 0.5);
		eOrigin.Y = org.Y - (from->state.origin[1] + (from->mins[1] + from->maxs[1]) * 0.5);
		eOrigin.Z = org.Z - (from->state.origin[2] + (from->mins[2] + from->maxs[2]) * 0.5);

		if ((int)eOrigin.LengthFast() > Radius)
			continue;
		return from->Entity;
	}

	return NULL;
}

/*
=============
G_PickTarget

Searches all active entities for the next one that holds
the matching string at fieldofs (use the FOFS() macro) in the structure.

Searches beginning at the edict after from, or the beginning if NULL
NULL will be returned if the end of the list is reached.

=============
*/
#if 0
#define MAXCHOICES	8

edict_t *G_PickTarget (char *targetname)
{
	edict_t	*ent = NULL;
	int		num_choices = 0;
	edict_t	*choice[MAXCHOICES];

	if (!targetname)
	{
		//gi.dprintf("G_PickTarget called with NULL targetname\n");
		return NULL;
	}

	while(1)
	{
		ent = G_Find (ent, FOFS(targetname), targetname);
		if (!ent)
			break;
		choice[num_choices++] = ent;
		if (num_choices == MAXCHOICES)
			break;
	}

	if (!num_choices)
	{
		DebugPrintf("G_PickTarget: target %s not found\n", targetname);
		return NULL;
	}

	return choice[rand() % num_choices];
}



void Think_Delay (edict_t *ent)
{
	G_UseTargets (ent, ent->activator);
	G_FreeEdict (ent);
}

/*
==============================
G_UseTargets

the global "activator" should be set to the entity that initiated the firing.

If self.delay is set, a DelayedUse entity will be created that will actually
do the SUB_UseTargets after that many seconds have passed.

Centerprints any self.message to the activator.

Search for (string)targetname in all entities that
match (string)self.target and call their .use function

==============================
*/
void G_UseTargets (edict_t *ent, edict_t *activator)
{
	edict_t		*t;

//
// check for a delay
//
	if (ent->delay)
	{
	// create a temp object to fire at a later time
		t = G_Spawn();
		t->classname = "DelayedUse";

		// Paril: for compatibility
		t->nextthink = level.framenum + (ent->delay * 10);
		t->think = Think_Delay;
		t->activator = activator;
		if (!activator)
			DebugPrintf ("Think_Delay with no activator\n");
		t->message = ent->message;
		t->target = ent->target;
		t->killtarget = ent->killtarget;
		return;
	}
	
	
//
// print the message
//
	if ((ent->message) && !(activator->svFlags & SVF_MONSTER))
	{
		ClientPrintf (activator, PRINT_CENTER, "%s", ent->message);
		if (ent->noise_index)
			PlaySoundFrom (activator, CHAN_AUTO, ent->noise_index);
		else
			PlaySoundFrom (activator, CHAN_AUTO, SoundIndex ("misc/talk1.wav"));
	}

//
// kill killtargets
//
	if (ent->killtarget)
	{
		t = NULL;
		while ((t = G_Find (t, FOFS(targetname), ent->killtarget)) != NULL)
		{
			G_FreeEdict (t);
			if (!ent->inUse)
			{
				DebugPrintf("entity was removed while using killtargets\n");
				return;
			}
		}
	}

//
// fire targets
//
	if (ent->target)
	{
		t = NULL;
		while ((t = G_Find (t, FOFS(targetname), ent->target)) != NULL)
		{
			// doors fire area portals in a specific way
			if (!Q_stricmp(t->classname, "func_areaportal") &&
				(!Q_stricmp(ent->classname, "func_door") || !Q_stricmp(ent->classname, "func_door_rotating")))
				continue;

			if (t == ent)
				DebugPrintf ("WARNING: Entity used itself.\n");
			else if (t->use)
				t->use (t, ent, activator);
			if (!ent->inUse)
			{
				DebugPrintf("entity was removed while using targets\n");
				return;
			}
		}
	}
}
#endif

void G_SetMovedir (vec3f &angles, vec3f &movedir)
{
	if (angles.Y == -1)
		movedir.Set (0, 0, 1);
	else if (angles.Y == -2)
		movedir.Set (0, 0, -1);
	else
		angles.ToVectors (&movedir, NULL, NULL);

	angles.Clear();
}

/*
============
G_TouchTriggers

============
*/
void	G_TouchTriggers (CBaseEntity *ent)
{
	edict_t		*touch[MAX_CS_EDICTS];
	memset(touch, 0, sizeof(touch));

	// dead things don't activate triggers!
	if ((ent->EntityFlags & ENT_HURTABLE) && (ent->gameEntity->health <= 0))
		return;

	int num = BoxEdicts (ent->GetAbsMin(), ent->GetAbsMax(), touch, MAX_CS_EDICTS, true);

	// be careful, it is possible to have an entity in this
	// list removed before we get to it (killtriggered)
	for (int i=0 ; i<num ; i++)
	{
		edict_t *hit = touch[i];
		CBaseEntity *Entity = hit->Entity;

		if (!Entity || !Entity->IsInUse())
			continue;

		if (Entity->EntityFlags & ENT_TOUCHABLE)
		{
			(dynamic_cast<CTouchableEntity*>(Entity))->Touch (ent, NULL, NULL);
			continue;
		}

		if (!ent->IsInUse())
			break;
	}
}

/*
============
G_TouchSolids

Call after linking a new trigger in during gameplay
to force all entities it covers to immediately touch it
============
*/
void	G_TouchSolids (CBaseEntity *ent)
{
	edict_t		*touch[MAX_CS_EDICTS];

	int num = BoxEdicts (ent->GetAbsMin(), ent->GetAbsMax(), touch, MAX_CS_EDICTS, false);

	// be careful, it is possible to have an entity in this
	// list removed before we get to it (killtriggered)
	for (int i=0 ; i<num ; i++)
	{
		edict_t *hit = touch[i];
		CBaseEntity *Entity = hit->Entity;

		if (!Entity || !Entity->IsInUse())
			continue;

		if (Entity->EntityFlags & ENT_TOUCHABLE)
		{
			(dynamic_cast<CTouchableEntity*>(Entity))->Touch (ent, NULL, NULL);
			continue;
		}

		if (!ent->IsInUse())
			break;
	}
}




/*
==============================================================================

Kill box

==============================================================================
*/

/*
=================
KillBox

Kills all entities that would touch the proposed new positioning
of ent.  Ent should be unlinked before calling this!
=================
*/
bool KillBox (CBaseEntity *ent)
{
	CTrace		tr;

	while (1)
	{
		tr = CTrace (ent->State.GetOrigin(), ent->GetMins(), ent->GetMaxs(), ent->State.GetOrigin(), NULL, CONTENTS_MASK_PLAYERSOLID);
		if (!tr.ent)
			break;

		// nail it
		T_Damage (tr.ent, ent->gameEntity, ent->gameEntity, vec3Origin, ent->State.GetOrigin(), vec3Origin, 100000, 0, DAMAGE_NO_PROTECTION, MOD_TELEFRAG);

		// if we didn't kill it, fail
		if (tr.ent->solid)
			return false;
	}

	return true;		// all clear
}

// Returns a random team member of ent
CBaseEntity *GetRandomTeamMember (CBaseEntity *Entity, CBaseEntity *Master)
{
	CBaseEntity *Member = Master;
	int count = 0;

	//for (count = 0, Member = Master; Member->gameEntity; Member = Member->gameEntity->chain->Entity, count++);
	while (Member)
	{
		// Sanity
		if (!Member->gameEntity)
			break;

		count++;
		if (!Member->gameEntity->chain)
			break;
		else
			Member = Member->gameEntity->chain->Entity;
	}

	int choice = rand() % count;
	//for (count = 0, Member = Master; count < choice; Member = Member->gameEntity->chain->Entity, count++);
	count = 0;
	Member = Master;
	while (Member)
	{
		if (count == choice)
			break;

		// Sanity
		if (!Member->gameEntity)
			break;

		count++;
		if (!Member->gameEntity->chain)
			break;
		else
			Member = Member->gameEntity->chain->Entity;
	}

	return Member;
}