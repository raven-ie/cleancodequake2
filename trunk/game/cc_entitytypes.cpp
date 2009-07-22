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

	if (GetSolid() != SOLID_NOT && (EntityFlags & ENT_TOUCHABLE))
		dynamic_cast<CTouchableEntity*>(this)->Touch (e2, &trace->plane, trace->surface);
	
	if ((e2->EntityFlags & ENT_TOUCHABLE) && e2->GetSolid() != SOLID_NOT)
		dynamic_cast<CTouchableEntity*>(e2)->Touch (this, NULL, NULL);
}

CBounceProjectile::CBounceProjectile () :
backOff(1.5f),
CPhysicsEntity ()
{
	Vec3Copy (vec3Origin, gameEntity->velocity); 
	SetClipmask (CONTENTS_MASK_SHOT);
	SetSolid (SOLID_BBOX);
	SetMins(vec3Origin);
	SetMaxs(vec3Origin);
}

CBounceProjectile::CBounceProjectile (int Index) :
backOff(1.5f),
CPhysicsEntity (Index)
{
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

	if (gameEntity->velocity[2] > 0)
		gameEntity->groundentity = NULL;

// check for the groundentity going away
	if (gameEntity->groundentity && !gameEntity->groundentity->inUse)
		gameEntity->groundentity = NULL;

// if onground, return without moving
	if ( gameEntity->groundentity )
		return false;

	State.GetOrigin(old_origin);

// add gravity
	AddGravity ();

// move angles
	vec3_t angles;
	State.GetAngles (angles);
	Vec3MA (angles, 0.1f, gameEntity->avelocity, angles);
	State.SetAngles (angles);

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
CPhysicsEntity ()
{
	Vec3Copy (vec3Origin, gameEntity->velocity); 
	SetClipmask (CONTENTS_MASK_SHOT);
	SetSolid (SOLID_BBOX);
	SetMins(vec3Origin);
	SetMaxs(vec3Origin);
}

CFlyMissileProjectile::CFlyMissileProjectile (int Index) :
CPhysicsEntity (Index)
{
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

	if (gameEntity->velocity[2] > 0)
		gameEntity->groundentity = NULL;

// check for the groundentity going away
	if (gameEntity->groundentity && !gameEntity->groundentity->inUse)
		gameEntity->groundentity = NULL;

// if onground, return without moving
	if ( gameEntity->groundentity )
		return false;

	State.GetOrigin(old_origin);

// move angles
	vec3_t angles;
	State.GetAngles (angles);
	Vec3MA (angles, 0.1f, gameEntity->avelocity, angles);
	State.SetAngles (angles);

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

CStepPhysics::CStepPhysics () :
CPhysicsEntity ()
{
	Vec3Copy (vec3Origin, gameEntity->velocity); 
	SetClipmask (CONTENTS_MASK_SHOT);
	SetSolid (SOLID_BBOX);
	SetMins(vec3Origin);
	SetMaxs(vec3Origin);
}

CStepPhysics::CStepPhysics (int Index) :
CPhysicsEntity (Index)
{
	Vec3Copy (vec3Origin, gameEntity->velocity); 
	SetClipmask (CONTENTS_MASK_SHOT);
	SetSolid (SOLID_BBOX);
	SetMins(vec3Origin);
	SetMaxs(vec3Origin);
}

void CStepPhysics::CheckGround ()
{
	CTrace		trace;

	if (gameEntity->velocity[2] > 100)
	{
		gameEntity->groundentity = NULL;
		return;
	}

// if the hull point one-quarter unit down is solid the entity is on ground
	vec3f point = State.GetOrigin();
	point.Z -= 0.25f;

	trace = CTrace (State.GetOrigin(), GetMins(), GetMaxs(), point, gameEntity, CONTENTS_MASK_MONSTERSOLID);

	// check steepness
	if ( trace.plane.normal[2] < 0.7 && !trace.startSolid)
	{
		gameEntity->groundentity = NULL;
		return;
	}

	if (!trace.startSolid && !trace.allSolid)
	{
		State.SetOrigin (trace.endPos);
		gameEntity->groundentity = trace.ent;
		gameEntity->groundentity_linkcount = trace.ent->linkCount;
		gameEntity->velocity[2] = 0;
	}
}

#define SV_STOPSPEED		100
#define SV_FRICTION			6
#define SV_WATERFRICTION	1

void CStepPhysics::AddRotationalFriction ()
{
	vec3_t angles;
	State.GetAngles (angles);
	Vec3MA (angles, 0.1f, gameEntity->avelocity, angles);
	State.SetAngles (angles);

	float adjustment = 0.1f * SV_STOPSPEED * SV_FRICTION;
	for (int n = 0; n < 3; n++)
	{
		if (gameEntity->avelocity[n] > 0)
		{
			gameEntity->avelocity[n] -= adjustment;
			if (gameEntity->avelocity[n] < 0)
				gameEntity->avelocity[n] = 0;
		}
		else
		{
			gameEntity->avelocity[n] += adjustment;
			if (gameEntity->avelocity[n] > 0)
				gameEntity->avelocity[n] = 0;
		}
	}
}

#define MAX_CLIP_PLANES	5
int CStepPhysics::FlyMove (float time, int mask)
{
	edict_t		*hit;
	int			bumpcount, numbumps;
	vec3_t		dir;
	float		d;
	int			numplanes;
	vec3_t		planes[MAX_CLIP_PLANES];
	vec3_t		primal_velocity, original_velocity, new_velocity;
	int			i, j;
	CTrace		trace;
	vec3_t		end;
	float		time_left;
	int			blocked;
	
	numbumps = 4;
	
	blocked = 0;
	Vec3Copy (gameEntity->velocity, original_velocity);
	Vec3Copy (gameEntity->velocity, primal_velocity);
	numplanes = 0;
	
	time_left = time;

	gameEntity->groundentity = NULL;
	for (bumpcount=0 ; bumpcount<numbumps ; bumpcount++)
	{
		vec3_t origin;
		State.GetOrigin (origin);
		for (i=0 ; i<3 ; i++)
			end[i] = origin[i] + time_left * gameEntity->velocity[i];

		trace = CTrace (origin, gameEntity->mins, gameEntity->maxs, end, gameEntity, mask);

		if (trace.allSolid)
		{	// entity is trapped in another solid
			Vec3Copy (vec3Origin, gameEntity->velocity);
			return 3;
		}

		if (trace.fraction > 0)
		{	// actually covered some distance
			State.SetOrigin (trace.endPos);
			Vec3Copy (gameEntity->velocity, original_velocity);
			numplanes = 0;
		}

		if (trace.fraction == 1)
			 break;		// moved the entire distance

		hit = trace.ent;

		if (trace.plane.normal[2] > 0.7)
		{
			blocked |= 1;		// floor
			if ( hit->solid == SOLID_BSP)
			{
				gameEntity->groundentity = hit;
				gameEntity->groundentity_linkcount = hit->linkCount;
			}
		}
		if (!trace.plane.normal[2])
			blocked |= 2;		// step

//
// run the impact function
//
		Impact (&trace);
		if (!IsInUse())
			break;		// removed by the impact function

		
		time_left -= time_left * trace.fraction;
		
	// cliped to another plane
		if (numplanes >= MAX_CLIP_PLANES)
		{	// this shouldn't really happen
			Vec3Copy (vec3Origin, gameEntity->velocity);
			return 3;
		}

		Vec3Copy (trace.plane.normal, planes[numplanes]);
		numplanes++;

//
// modify original_velocity so it parallels all of the clip planes
//
		for (i=0 ; i<numplanes ; i++)
		{
			ClipVelocity (original_velocity, planes[i], new_velocity, 1);

			for (j=0 ; j<numplanes ; j++)
				if ((j != i) && !Vec3Compare (planes[i], planes[j]))
				{
					if (Dot3Product (new_velocity, planes[j]) < 0)
						break;	// not ok
				}
			if (j == numplanes)
				break;
		}
		
		if (i != numplanes)
		{	// go along this plane
			Vec3Copy (new_velocity, gameEntity->velocity);
		}
		else
		{	// go along the crease
			if (numplanes != 2)
			{
//				gi.dprintf ("clip velocity, numplanes == %i\n",numplanes);
				Vec3Copy (vec3Origin, gameEntity->velocity);
				return 7;
			}
			CrossProduct (planes[0], planes[1], dir);
			d = Dot3Product (dir, gameEntity->velocity);
			Vec3Scale (dir, d, gameEntity->velocity);
		}

//
// if original velocity is against the original velocity, stop dead
// to avoid tiny occilations in sloping corners
//
		if (Dot3Product (gameEntity->velocity, primal_velocity) <= 0)
		{
			Vec3Copy (vec3Origin, gameEntity->velocity);
			return blocked;
		}
	}

	return blocked;
}

bool CStepPhysics::Run ()
{
	bool		hitsound = false;
	float		*vel;
	float		speed, newspeed, control;
	float		friction;
	int			mask;

	if (PhysicsDisabled)
		return false;

	// airborn monsters should always check for ground
	if (!gameEntity->groundentity && (EntityFlags & ENT_MONSTER))
		(dynamic_cast<CMonsterEntity*>(this))->Monster->CheckGround ();
	else
		CheckGround (); // Specific non-monster checkground

	edict_t *groundentity = gameEntity->groundentity;

	bool wasonground = (groundentity) ? true : false;
		
	if (gameEntity->avelocity[0] || gameEntity->avelocity[1] || gameEntity->avelocity[2])
		AddRotationalFriction ();

	// add gravity except:
	//   flying monsters
	//   swimming monsters who are in the water
	if (!wasonground)
	{
		if (!(gameEntity->flags & FL_FLY))
		{
			if (!((gameEntity->flags & FL_SWIM) && (gameEntity->waterlevel > 2)))
			{
				if (gameEntity->velocity[2] < sv_gravity->Float()*-0.1)
					hitsound = true;
				if (gameEntity->waterlevel == 0)
					AddGravity ();
			}
		}
	}

	// friction for flying monsters that have been given vertical velocity
	if ((gameEntity->flags & FL_FLY) && (gameEntity->velocity[2] != 0))
	{
		speed = Q_fabs(gameEntity->velocity[2]);
		control = (speed < SV_STOPSPEED) ? SV_STOPSPEED : speed;
		friction = SV_FRICTION/3;
		newspeed = speed - (0.1f * control * friction);
		if (newspeed < 0)
			newspeed = 0;
		newspeed /= speed;
		gameEntity->velocity[2] *= newspeed;
	}

	// friction for flying monsters that have been given vertical velocity
	if ((gameEntity->flags & FL_SWIM) && (gameEntity->velocity[2] != 0))
	{
		speed = Q_fabs(gameEntity->velocity[2]);
		control = (speed < SV_STOPSPEED) ? SV_STOPSPEED : speed;
		newspeed = speed - (0.1f * control * SV_WATERFRICTION * gameEntity->waterlevel);
		if (newspeed < 0)
			newspeed = 0;
		newspeed /= speed;
		gameEntity->velocity[2] *= newspeed;
	}

	if (gameEntity->velocity[2] || gameEntity->velocity[1] || gameEntity->velocity[0])
	{
		// apply friction
		// let dead monsters who aren't completely onground slide
		if ((wasonground) || (gameEntity->flags & (FL_SWIM|FL_FLY)) && !(gameEntity->health <= 0.0 && ((EntityFlags & ENT_MONSTER) && !(dynamic_cast<CMonsterEntity*>(this))->Monster->CheckBottom())))
		{
			vel = gameEntity->velocity;
			speed = sqrtf(vel[0]*vel[0] +vel[1]*vel[1]);
			if (speed)
			{
				friction = SV_FRICTION;

				control = (speed < SV_STOPSPEED) ? SV_STOPSPEED : speed;
				newspeed = speed - 0.1f*control*friction;

				if (newspeed < 0)
					newspeed = 0;
				newspeed /= speed;

				vel[0] *= newspeed;
				vel[1] *= newspeed;
			}
		}

		if (EntityFlags & ENT_MONSTER)
			mask = CONTENTS_MASK_MONSTERSOLID;
		else
			mask = CONTENTS_MASK_SOLID;
		FlyMove (0.1f, mask);

		Link();
		G_TouchTriggers (gameEntity);
		if (!IsInUse())
			return false;

		if (gameEntity->groundentity && !wasonground && hitsound)
			PlaySoundFrom (gameEntity, CHAN_AUTO, SoundIndex("world/land.wav"));
	}
	return true;
}