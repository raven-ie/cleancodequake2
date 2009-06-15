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
// cc_entitytypes.cpp
// Different base entity classes
//

#include "cc_local.h"

CHurtableEntity::CHurtableEntity () :
CBaseEntity()
{
	EntityFlags |= ENT_HURTABLE;
};

CHurtableEntity::CHurtableEntity (int Index) :
CBaseEntity(Index)
{
	EntityFlags |= ENT_HURTABLE;
};

CThinkableEntity::CThinkableEntity () :
CBaseEntity()
{
	EntityFlags |= ENT_THINKABLE;
};

CThinkableEntity::CThinkableEntity (int Index) :
CBaseEntity(Index)
{
	EntityFlags |= ENT_THINKABLE;
};

void CThinkableEntity::RunThink ()
{
	if (NextThink <= 0 || NextThink > level.time+0.001)
		return;
	
	NextThink = 0;
	Think ();
}

CTouchableEntity::CTouchableEntity () :
CBaseEntity()
{
	EntityFlags |= ENT_TOUCHABLE;
};

CTouchableEntity::CTouchableEntity (int Index) :
CBaseEntity(Index)
{
	EntityFlags |= ENT_TOUCHABLE;
};

void CTouchableEntity::Touch(CBaseEntity *other, plane_t *plane, cmBspSurface_t *surf)
{
}

CBounceProjectile::CBounceProjectile () :
CBaseEntity (),
CTouchableEntity(),
CThinkableEntity()
{
}

CBounceProjectile::CBounceProjectile (int Index) :
CBaseEntity (Index),
CTouchableEntity(),
CThinkableEntity()
{
}

int ClipVelocity (vec3_t in, vec3_t normal, vec3_t out, float overbounce);
bool CBounceProjectile::Run ()
{
	CTrace	trace;
	vec3_t		move;
	float		backoff;
	edict_t		*slave;
	bool		wasinwater;
	bool		isinwater;
	vec3_t		old_origin;

	// if not a team captain, so movement will be handled elsewhere
	if (gameEntity->flags & FL_TEAMSLAVE)
		return false;

	if (CBaseEntity::gameEntity->velocity[2] > 0)
		CBaseEntity::gameEntity->groundentity = NULL;

// check for the groundentity going away
	if (CBaseEntity::gameEntity->groundentity && !CBaseEntity::gameEntity->groundentity->inUse)
		CBaseEntity::gameEntity->groundentity = NULL;

// if onground, return without moving
	if ( CBaseEntity::gameEntity->groundentity )
		return false;

	CBaseEntity::State.GetOrigin(old_origin);

// add gravity
	AddGravity ();

// move angles
	vec3_t angles;
	CBaseEntity::State.GetAngles (angles);
	Vec3MA (angles, FRAMETIME, CBaseEntity::gameEntity->avelocity, angles);
	CBaseEntity::State.SetAngles (angles);

// move origin
	Vec3Scale (gameEntity->velocity, FRAMETIME, move);
	trace = PushEntity (move);
	if (!IsInUse())
		return false;

	if (trace.fraction < 1)
	{
		backoff = 1.5;
		ClipVelocity (gameEntity->velocity, trace.plane.normal, gameEntity->velocity, backoff);

		// stop if on ground
		if (trace.plane.normal[2] > 0.9)
		{		
			if (gameEntity->velocity[2] < 60)
			{
				gameEntity->groundentity = trace.ent;
				gameEntity->groundentity_linkcount = trace.ent->linkCount;
				Vec3Copy (vec3Origin, gameEntity->velocity);
				Vec3Copy (vec3Origin, gameEntity->avelocity);
			}
		}
	}
	
// check for water transition
	vec3_t or;
	State.GetOrigin (or);

	wasinwater = (gameEntity->watertype & CONTENTS_MASK_WATER) ? true : false;
	gameEntity->watertype = gi.pointcontents (or);
	isinwater = (gameEntity->watertype & CONTENTS_MASK_WATER) ? true : false;

	if (isinwater)
		gameEntity->waterlevel = 1;
	else
		gameEntity->waterlevel = 0;

	if (!wasinwater && isinwater)
		PlaySoundAt (old_origin, g_edicts, CHAN_AUTO, SoundIndex("misc/h2ohit1.wav"));
	else if (wasinwater && !isinwater)
		PlaySoundAt (or, g_edicts, CHAN_AUTO, SoundIndex("misc/h2ohit1.wav"));

// move teamslaves
	for (slave = gameEntity->teamchain; slave; slave = slave->teamchain)
	{
		State.SetOrigin (slave->state.origin);
		gi.linkentity (slave);
	}

	return true;
}

void CBounceProjectile::AddGravity()
{
	gameEntity->velocity[2] -= gameEntity->gravity * sv_gravity->Float() * FRAMETIME;
}

CTrace CBounceProjectile::PushEntity (vec3_t push)
{
	CTrace		trace;
	vec3_t		start;
	vec3_t		end;
	int			mask;

	State.GetOrigin (start);
	Vec3Add (start, push, end);

retry:
	if (gameEntity->clipMask)
		mask = gameEntity->clipMask;
	else
		mask = CONTENTS_MASK_SOLID;

	trace = CTrace (start, gameEntity->mins, gameEntity->maxs, end, gameEntity, mask);
	
	State.SetOrigin (trace.endPos);
	Link();

	if (trace.fraction != 1.0)
	{
		Impact (&trace);

		// if the pushed entity went away and the pusher is still there
		if (!trace.ent->inUse && IsInUse())
		{
			// move the pusher back and try again
			State.SetOrigin (start);
			Link ();
			goto retry;
		}
	}

	if (IsInUse())
		G_TouchTriggers (gameEntity);

	return trace;
}

void CBounceProjectile::Impact (CTrace *trace)
{
	CBaseEntity	*e2;

	if (!trace->ent->Entity)
		return;

	e2 = dynamic_cast<CBaseEntity*>(trace->ent->Entity);

	if (GetSolid() != SOLID_NOT)
		Touch (e2, &trace->plane, trace->surface);
	
	if ((e2->EntityFlags & ENT_TOUCHABLE) && e2->GetSolid() != SOLID_NOT)
		dynamic_cast<CTouchableEntity*>(e2)->Touch (this, NULL, NULL);
}
