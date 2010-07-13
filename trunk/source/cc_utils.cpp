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

void G_ProjectSource (const vec3f &Point, const vec3f &distance, const vec3f &forward, const vec3f &right, vec3f &result, const vec3f &up)
{
	result.Set (Point.X + forward.X * distance.X + right.X * distance.Y + up.X * distance.Z,
				Point.Y + forward.Y * distance.X + right.Y * distance.Y + up.Y * distance.Z,
				Point.Z + forward.Z * distance.X + right.Z * distance.Y + up.Z * distance.Z);
}

IBaseEntity *FindRadius (IBaseEntity *From, vec3f &org, sint32 Radius, uint32 EntityFlags, bool CheckNonSolid)
{
	for (SEntity *from = (!From) ? Game.Entities : (From->GetGameEntity() + 1); from < &Game.Entities[GameAPI.GetNumEdicts()]; from++)
	{
		if (!from->Entity)
			continue;
		IBaseEntity *Entity = from->Entity;
		if (!Entity->GetInUse())
			continue;
		if (CheckNonSolid && (Entity->GetSolid() == SOLID_NOT))
			continue;
		if (!(Entity->EntityFlags & EntityFlags))
			continue;

		if ((sint32)(org - (Entity->State.GetOrigin() + (Entity->GetMins()+ Entity->GetMaxs()) * 0.5)).LengthFast() > Radius)
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

IBaseEntity *CC_PickTarget (std::string targetname)
{
	static TTargetList choices;
	choices.clear();

	if (targetname.empty())
		return NULL;

	IMapEntity *Entity = NULL;
	while(1)
	{
		Entity = CC_Find<IMapEntity, ENT_MAP, EntityMemberOffset(IMapEntity,TargetName)> (Entity, targetname.c_str());
		if (!Entity)
			break;

		choices.push_back (Entity);
	}

	if (!choices.size())
	{
		MapPrint (MAPPRINT_ERROR, NULL, vec3fOrigin, "Target \"%s\" not found\n", targetname.c_str());
		return NULL;
	}

	return choices[irandom(choices.size())];
}

TTargetList CC_GetTargets (std::string targetname)
{
	static TTargetList choices;
	choices.clear();

	if (targetname.empty())
		return choices;

	IMapEntity *Entity = NULL;
	while(1)
	{
		Entity = CC_Find<IMapEntity, ENT_MAP, EntityMemberOffset(IMapEntity,TargetName)> (Entity, targetname.c_str());
		
		if (!Entity)
			break;

		choices.push_back (Entity);
	}

	if (!choices.size())
	{
		MapPrint (MAPPRINT_ERROR, NULL, vec3fOrigin, "Target \"%s\" not found\n", targetname.c_str());
		return choices;
	}

	return choices;
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
void	G_TouchTriggers (IBaseEntity *Entity)
{
	// dead things don't activate triggers!
	if (Entity->EntityFlags & ENT_HURTABLE)
	{
		IHurtableEntity *Hurt = entity_cast<IHurtableEntity>(Entity);
		if ((Hurt->CanTakeDamage) && (Hurt->Health <= 0))
			return;
	}

	TBoxEdictsEntityList Entities = BoxEdicts (Entity->GetAbsMin(), Entity->GetAbsMax(), true);

	// be careful, it is possible to have an entity in this
	// list removed before we get to it (killtriggered)
	for (TBoxEdictsEntityList::iterator it = Entities.begin(); it < Entities.end(); ++it)
	{
		IBaseEntity *TouchedEntity = (*it);

		if (!TouchedEntity || !TouchedEntity->GetInUse())
			continue;

		if (TouchedEntity->EntityFlags & ENT_TOUCHABLE)
		{
			(entity_cast<ITouchableEntity>(TouchedEntity))->Touch (Entity, NULL, NULL);
			continue;
		}

		if (!Entity->GetInUse())
			break;
	}
}

// Calls the callback for each member of the team in "ent"
void CForEachTeamChainCallback::Query (IBaseEntity *Master)
{
	for (IBaseEntity *e = Master->Team.Master; e; e = e->Team.Chain)
		Callback (e);
}

void CForEachPlayerCallback::Query (bool MustBeInUse)
{
	for (uint8 i = 1; i <= Game.MaxClients; i++)
	{
		CPlayerEntity *Player = entity_cast<CPlayerEntity>(Game.Entities[i].Entity);

		if (MustBeInUse && (!Player->GetInUse() || Player->Client.Persistent.State != SVCS_SPAWNED))
			continue;

		Index = i;
		Callback (Player);
	}
}

void CForEachEntityCallback::Query (uint32 FlagsCanHave, uint32 FlagsCantHave, bool MustBeInUse)
{
	for (TEntitiesContainer::iterator it = Level.Entities.Closed.begin(); it != Level.Entities.Closed.end(); ++it)
	{
		IBaseEntity *Entity = (*it)->Entity;

		if (MustBeInUse && (!Entity->GetInUse()))
			continue;
		if (FlagsCanHave && !(Entity->EntityFlags & FlagsCanHave))
			continue;
		if (FlagsCantHave && (Entity->EntityFlags & FlagsCantHave))
			continue;

		Callback (Entity);
	}
}
/*
=============
range

returns the range catagorization of an entity reletive to self
0	melee range, will become hostile even if back is turned
1	visibility and infront, or visibility and show hostile
2	infront and show hostile
3	only triggered by damage
=============
*/
ERangeType Range (IBaseEntity *self, IBaseEntity *Other)
{
	return Range(self->State.GetOrigin(), Other->State.GetOrigin());
}

/*
=============
visible

returns 1 if the entity is visible to self, even if not infront ()
=============
*/
bool IsVisible (IBaseEntity *self, IBaseEntity *Other)
{
	if (!self || !Other)
		return false;

	vec3f start = self->State.GetOrigin() + vec3f(0, 0, self->ViewHeight),
		  end = Other->State.GetOrigin() + vec3f(0, 0, Other->ViewHeight);
	return (CTrace (start, end,
		self, CONTENTS_MASK_OPAQUE).Fraction == 1.0);
}

bool IsVisible (vec3f left, vec3f right, IBaseEntity *self)
{
	return (CTrace (left, right,
		self, CONTENTS_MASK_OPAQUE).Fraction == 1.0);
}

/*
=============
infront

returns 1 if the entity is in front (in sight) of self
=============
*/

bool IsInFront (IBaseEntity *self, IBaseEntity *Other)
{	
	vec3f forward;
	self->State.GetAngles().ToVectors (&forward, NULL, NULL);
	return (((Other->State.GetOrigin() - self->State.GetOrigin()).GetNormalized() | forward) > 0.3f);
}

bool IsInBack (IBaseEntity *self, IBaseEntity *Other)
{	
	vec3f forward;
	self->State.GetAngles().ToVectors (&forward, NULL, NULL);
	return (((Other->State.GetOrigin() - self->State.GetOrigin()).GetNormalized() | forward) < -0.3f);
}

bool IsBelow (IBaseEntity *self, IBaseEntity *Other)
{
	if (((Other->State.GetOrigin() - self->State.GetOrigin()).GetNormalized() | vec3f(0, 0, -1)) > 0.95)  // 18 degree arc below
		return true;
	return false;
}

IBaseEntity *LoadEntity (uint32 Index)
{
	return (Index == -1) ? NULL : Game.Entities[Index].Entity;
}

#if defined(WIN32)
#include <crtdbg.h>
#endif

bool AssertExpression (const bool expr, const char *msg)
{
#if ALLOW_ASSERTS
	if (!expr)
	{
		// Print it to the console
		DebugPrintf ("Assertion failed: %s\n", msg);

		// On Win32, open up the Crt debug report thingy
#if defined(WIN32)
#if defined(_DEBUG)
		if (_CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, NULL, msg) == 1)
			_CrtDbgBreak(); // Call break if we told it to break
#else
		assert (0);
#endif
#else
		// If you hit this, you're on non-Windows.
		// Check msg for more information.
		assert (0);
#endif
		return true;
	}
#endif

	return false;
}
