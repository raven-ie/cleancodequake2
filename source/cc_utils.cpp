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
#define MAXCHOICES	8

CBaseEntity *CC_PickTarget (char *targetname)
{
	int		num_choices = 0;
	CBaseEntity	*choice[MAXCHOICES];

	if (!targetname)
		return NULL;

	CBaseEntity *ent = NULL;
	while(1)
	{
		ent = CC_Find (ent, FOFS(targetname), targetname);
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

class CDelayedUse : public CThinkableEntity
{
public:
	CBaseEntity	*Activator;

	CDelayedUse () :
	  CBaseEntity (),
	  CThinkableEntity (),
	  Activator(NULL)
	  {
	  };

	CDelayedUse (int Index) :
	  CBaseEntity (Index),
	  CThinkableEntity (Index),
	  Activator(NULL)
	  {
	  };

	void Think ()
	{
		G_UseTargets (this, Activator);
		Free ();
	}
};

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
void G_UseTargets (CBaseEntity *ent, CBaseEntity *activator)
{
//
// check for a delay
//
	if (ent->gameEntity->delay)
	{
	// create a temp object to fire at a later time
		CDelayedUse *t = QNew (com_levelPool, 0) CDelayedUse;
		t->gameEntity->classname = "DelayedUse";

		// Paril: for compatibility
		t->NextThink = level.framenum + (ent->gameEntity->delay * 10);
		t->Activator = activator;
		if (!activator)
			DebugPrintf ("Think_Delay with no activator\n");
		t->gameEntity->message = ent->gameEntity->message;
		t->gameEntity->target = ent->gameEntity->target;
		t->gameEntity->killtarget = ent->gameEntity->killtarget;
		return;
	}

//
// print the message
//
	if ((ent->gameEntity->message) && (activator->EntityFlags & ENT_PLAYER))
	{
		CPlayerEntity *Player = dynamic_cast<CPlayerEntity*>(activator);
		Player->PrintToClient (PRINT_CENTER, "%s", ent->gameEntity->message);
		if (ent->gameEntity->noise_index)
			Player->PlaySound (CHAN_AUTO, ent->gameEntity->noise_index);
		else
			Player->PlaySound (CHAN_AUTO, SoundIndex ("misc/talk1.wav"));
	}

//
// kill killtargets
//
	if (ent->gameEntity->killtarget)
	{
		CBaseEntity *t = NULL;
		while ((t = CC_Find (t, FOFS(targetname), ent->gameEntity->killtarget)) != NULL)
		{
			t->Free ();

			if (!ent->IsInUse())
			{
				DebugPrintf("entity was removed while using killtargets\n");
				return;
			}
		}
	}

//
// fire targets
//
	if (ent->gameEntity->target)
	{
		CBaseEntity *Ent = NULL;
		while ((Ent = CC_Find (Ent, FOFS(targetname), ent->gameEntity->target)) != NULL)
		{
			if (!Ent)
				continue;

			// doors fire area portals in a specific way
			if (!Q_stricmp(Ent->gameEntity->classname, "func_areaportal") &&
				(!Q_stricmp(Ent->gameEntity->classname, "func_door") || !Q_stricmp(Ent->gameEntity->classname, "func_door_rotating")))
				continue;

			if (Ent == ent)
				DebugPrintf ("WARNING: Entity used itself.\n");
			else if (Ent->EntityFlags & ENT_USABLE)
				(dynamic_cast<CUsableEntity*>(Ent))->Use (ent, activator);
			if (!ent->IsInUse())
			{
				DebugPrintf("entity was removed while using targets\n");
				return;
			}
		}
	}
}

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

// Calls the callback for each member of the team in "ent"
void ForEachTeamChain (CBaseEntity *Master, CForEachTeamChainCallback *Callback)
{
	//for (CBaseEntity *e = Master->gameEntity->teammaster->Entity; e; e = e->gameEntity->teamchain->Entity)
	CBaseEntity *e = Master->gameEntity->teammaster->Entity;

	while (true)
	{
		if (!e)
			break;

		Callback->Callback (e);

		if (!e->gameEntity->teamchain)
			break;

		e = e->gameEntity->teamchain->Entity;
	}
}

/*
=============
CC_Find

Searches all active entities for the next one that holds
the matching string at fieldofs (use the FOFS() macro) in the structure.

Searches beginning at the edict after from, or the beginning if NULL
NULL will be returned if the end of the list is reached.

=============
*/
CBaseEntity *CC_Find (CBaseEntity *from, int fieldofs, char *match)
{
	edict_t *gameEnt;
	if (!from)
		gameEnt = g_edicts;
	else
	{
		gameEnt = from->gameEntity;
		gameEnt++;
	}

	for ( ; gameEnt < &g_edicts[globals.numEdicts] ; gameEnt++)
	{
		if (!gameEnt->inUse)
			continue;
		if (!gameEnt->Entity)
			continue;

		char *s = *(char **) ((byte *)gameEnt + fieldofs);
		if (!s)
			continue;
		if (!Q_stricmp (s, match))
			return gameEnt->Entity;
	}

	return NULL;
}

/*
=======================================================================

  SelectSpawnPoint

=======================================================================
*/

/*
================
PlayersRangeFromSpot

Returns the distance to the nearest player from the given spot
================
*/
float	PlayersRangeFromSpot (CBaseEntity *spot)
{
	float	bestplayerdistance = 9999999;

	for (int n = 1; n <= game.maxclients; n++)
	{
		CPlayerEntity *player = dynamic_cast<CPlayerEntity*>(g_edicts[n].Entity);

		if (!player->IsInUse())
			continue;

		if (player->gameEntity->health <= 0)
			continue;

		float length = (spot->State.GetOrigin() - player->State.GetOrigin()).Length();
		if (length < bestplayerdistance)
			bestplayerdistance = length;
	}

	return bestplayerdistance;
}

/*
================
SelectRandomDeathmatchSpawnPoint

go to a random point, but NOT the two points closest
to other players
================
*/
CBaseEntity *SelectRandomDeathmatchSpawnPoint ()
{
	CBaseEntity *spot = NULL, *spot1 = NULL, *spot2 = NULL;
	float range1 = 99999, range2 = 99999;

	int count = 0;
	while ((spot = CC_Find (spot, FOFS(classname), "info_player_deathmatch")) != NULL)
	{
		count++;
		float range = PlayersRangeFromSpot(spot);
		if (range < range1)
		{
			range1 = range;
			spot1 = spot;
		}
		else if (range < range2)
		{
			range2 = range;
			spot2 = spot;
		}
	}

	if (!count)
		return NULL;

	if (count <= 2)
		spot1 = spot2 = NULL;
	else
		count -= 2;

	int selection = rand() % count;
	spot = NULL;

	do
	{
		spot = CC_Find (spot, FOFS(classname), "info_player_deathmatch");
		if (spot == spot1 || spot == spot2)
			selection++;
	} while(selection--);

	return spot;
}

/*
================
SelectFarthestDeathmatchSpawnPoint

================
*/
CBaseEntity *SelectFarthestDeathmatchSpawnPoint ()
{
	CBaseEntity	*bestspot = NULL, *spot = NULL;
	float		bestdistance = 0;

	while ((spot = CC_Find (spot, FOFS(classname), "info_player_deathmatch")) != NULL)
	{
		float bestplayerdistance = PlayersRangeFromSpot (spot);

		if (bestplayerdistance > bestdistance)
		{
			bestspot = spot;
			bestdistance = bestplayerdistance;
		}
	}

	if (bestspot)
		return bestspot;

	// if there is a player just spawned on each and every start spot
	// we have no choice to turn one into a telefrag meltdown
	return CC_Find (NULL, FOFS(classname), "info_player_deathmatch");
}
