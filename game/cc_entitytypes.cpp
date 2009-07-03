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
	if (NextThink <= 0 || NextThink > level.framenum)
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

CPhysicsEntity::CPhysicsEntity () :
CBaseEntity()
{
};

CPhysicsEntity::CPhysicsEntity (int Index) :
CBaseEntity(Index)
{
};

void CPhysicsEntity::AddGravity()
{
	gameEntity->velocity[2] -= gameEntity->gravity * sv_gravity->Float() * 0.1f;
}

CTrace CPhysicsEntity::PushEntity (vec3_t push)
{
	CTrace		Trace;
	int			mask;
	vec3f		Start = State.GetOrigin();
	vec3f		End = Start + vec3f(push);

retry:
	if (gameEntity->clipMask)
		mask = gameEntity->clipMask;
	else
		mask = CONTENTS_MASK_SOLID;

	Trace = CTrace (State.GetOrigin(), GetMins(), GetMaxs(), End, gameEntity, mask);
	
	State.SetOrigin (Trace.endPos);
	Link();

	if (Trace.fraction != 1.0)
	{
		Impact (&Trace);

		// if the pushed entity went away and the pusher is still there
		if (!Trace.ent->inUse && IsInUse())
		{
			// move the pusher back and try again
			State.SetOrigin (Start);
			Link ();
			goto retry;
		}
	}

	if (IsInUse())
		G_TouchTriggers (gameEntity);

	return Trace;
}

void CPhysicsEntity::Impact (CTrace *trace)
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

CBounceProjectile::CBounceProjectile () :
backOff(1.5f),
CPhysicsEntity (),
CTouchableEntity(),
CThinkableEntity()
{
	State.SetOrigin (vec3Origin);
	State.SetAngles (vec3Origin);
	Vec3Copy (vec3Origin, gameEntity->velocity); 
	SetClipmask (CONTENTS_MASK_SHOT);
	SetSolid (SOLID_BBOX);
	SetMins(vec3Origin);
	SetMaxs(vec3Origin);
}

CBounceProjectile::CBounceProjectile (int Index) :
backOff(1.5f),
CPhysicsEntity (Index),
CTouchableEntity(),
CThinkableEntity()
{
	State.SetOrigin (vec3Origin);
	State.SetAngles (vec3Origin);
	Vec3Copy (vec3Origin, gameEntity->velocity); 
	SetClipmask (CONTENTS_MASK_SHOT);
	SetSolid (SOLID_BBOX);
	SetMins(vec3Origin);
	SetMaxs(vec3Origin);
}

int ClipVelocity (vec3_t in, vec3_t normal, vec3_t out, float overbounce);
bool CBounceProjectile::Run ()
{
	CTrace	trace;
	vec3_t		move;
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
	Vec3MA (angles, 0.1f, CBaseEntity::gameEntity->avelocity, angles);
	CBaseEntity::State.SetAngles (angles);

// move origin
	Vec3Scale (gameEntity->velocity, 0.1f, move);
	trace = PushEntity (move);
	if (!IsInUse())
		return false;

	if (trace.fraction < 1)
	{
		ClipVelocity (gameEntity->velocity, trace.plane.normal, gameEntity->velocity, backOff);

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

CTossProjectile::CTossProjectile () :
CBounceProjectile ()
{
	backOff = 1.0f;
}

CTossProjectile::CTossProjectile (int Index) :
CBounceProjectile (Index)
{
	backOff = 1.0f;
}

CFlyMissileProjectile::CFlyMissileProjectile () :
CPhysicsEntity (),
CTouchableEntity (),
CThinkableEntity ()
{
	State.SetOrigin (vec3Origin);
	State.SetAngles (vec3Origin);
	Vec3Copy (vec3Origin, gameEntity->velocity); 
	SetClipmask (CONTENTS_MASK_SHOT);
	SetSolid (SOLID_BBOX);
	SetMins(vec3Origin);
	SetMaxs(vec3Origin);
}

CFlyMissileProjectile::CFlyMissileProjectile (int Index) :
CPhysicsEntity (Index),
CTouchableEntity (),
CThinkableEntity ()
{
	State.SetOrigin (vec3Origin);
	State.SetAngles (vec3Origin);
	Vec3Copy (vec3Origin, gameEntity->velocity); 
	SetClipmask (CONTENTS_MASK_SHOT);
	SetSolid (SOLID_BBOX);
	SetMins(vec3Origin);
	SetMaxs(vec3Origin);
}

bool CFlyMissileProjectile::Run ()
{
	CTrace	trace;
	vec3_t		move;
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

// move angles
	vec3_t angles;
	CBaseEntity::State.GetAngles (angles);
	Vec3MA (angles, 0.1f, CBaseEntity::gameEntity->avelocity, angles);
	CBaseEntity::State.SetAngles (angles);

// move origin
	Vec3Scale (gameEntity->velocity, 0.1f, move);
	trace = PushEntity (move);
	if (!IsInUse())
		return false;

	if (trace.fraction < 1)
	{
		ClipVelocity (gameEntity->velocity, trace.plane.normal, gameEntity->velocity, 1.0);

		// stop if on ground
		if (trace.plane.normal[2] > 0.9)
		{		
			gameEntity->groundentity = trace.ent;
			gameEntity->groundentity_linkcount = trace.ent->linkCount;
			Vec3Copy (vec3Origin, gameEntity->velocity);
			Vec3Copy (vec3Origin, gameEntity->avelocity);
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
