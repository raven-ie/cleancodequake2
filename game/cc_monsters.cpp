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
// cc_monsters.cpp
// Monsters
//

#include "cc_local.h"
#define STEPSIZE	18

/*
=================
AI_SetSightClient

Called once each frame to set level.sight_client to the
player to be checked for in findtarget.

If all clients are either dead or in notarget, sight_client
will be null.

In coop games, sight_client will cycle between the clients.
=================
*/
void AI_SetSightClient (void)
{
	edict_t	*ent;
	int		start, check;

	if (level.sight_client == NULL)
		start = 1;
	else
		start = level.sight_client - g_edicts;

	check = start;
	while (1)
	{
		check++;
		if (check > game.maxclients)
			check = 1;
		ent = &g_edicts[check];
		if (ent->inUse
			&& ent->health > 0
			&& !(ent->flags & FL_NOTARGET) )
		{
			level.sight_client = ent;
			return;		// got one
		}
		if (check == start)
		{
			level.sight_client = NULL;
			return;		// nobody to see
		}
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
int range (edict_t *self, edict_t *other)
{
	vec3_t	v;
	float	len;

	Vec3Subtract (self->s.origin, other->s.origin, v);
	len = Vec3Length (v);
	if (len < MELEE_DISTANCE)
		return RANGE_MELEE;
	if (len < 500)
		return RANGE_NEAR;
	if (len < 1000)
		return RANGE_MID;
	return RANGE_FAR;
}

/*
=============
visible

returns 1 if the entity is visible to self, even if not infront ()
=============
*/
bool visible (edict_t *self, edict_t *other)
{
	vec3_t	spot1;
	vec3_t	spot2;
	CTrace	trace;

	Vec3Copy (self->s.origin, spot1);
	spot1[2] += self->viewheight;
	Vec3Copy (other->s.origin, spot2);
	spot2[2] += other->viewheight;
	trace.Trace (spot1, spot2, self, CONTENTS_MASK_OPAQUE);
	
	if (trace.fraction == 1.0)
		return true;
	return false;
}


/*
=============
infront

returns 1 if the entity is in front (in sight) of self
=============
*/
bool infront (edict_t *self, edict_t *other)
{
	vec3_t	vec;
	float	dot;
	vec3_t	forward;
	
	Angles_Vectors (self->s.angles, forward, NULL, NULL);
	Vec3Subtract (other->s.origin, self->s.origin, vec);
	VectorNormalizef (vec, vec);
	dot = DotProduct (vec, forward);
	
	if (dot > 0.3)
		return true;
	return false;
}


void Monster_Think (edict_t *ent)
{
	if (!ent->Monster)
		return;
	ent->nextthink = level.time + .1;

	CMonster *Monster = ent->Monster;
	float	thinktime;

	thinktime = Monster->NextThink;
	if (thinktime > level.time+0.001)
		return;
	
	Monster->NextThink = 0;
	if (Monster->Think)
		Monster->RunThink ();
}

void CMonster::RunThink ()
{
	(this->*Think) ();
}

void CMonster::ChangeYaw ()
{
	float current = AngleModf (Entity->s.angles[YAW]);

	if (current == IdealYaw)
		return;

	float move = IdealYaw - current;
	if (IdealYaw > current)
	{
		if (move >= 180)
			move = move - 360;
	}
	else
	{
		if (move <= -180)
			move = move + 360;
	}
	if (move > 0)
	{
		if (move > YawSpeed)
			move = YawSpeed;
	}
	else
	{
		if (move < -YawSpeed)
			move = -YawSpeed;
	}
	
	Entity->s.angles[YAW] = AngleModf (current + move);
}

bool CMonster::CheckBottom ()
{
	vec3_t	mins, maxs, start, stop;
	CTrace	trace;
	int		x, y;
	float	mid, bottom;
	
	Vec3Add (Entity->s.origin, Entity->mins, mins);
	Vec3Add (Entity->s.origin, Entity->maxs, maxs);

// if all of the points under the corners are solid world, don't bother
// with the tougher checks
// the corners must be within 16 of the midpoint
	start[2] = mins[2] - 1;
	for	(x=0 ; x<=1 ; x++)
	{
		for	(y=0 ; y<=1 ; y++)
		{
			start[0] = x ? maxs[0] : mins[0];
			start[1] = y ? maxs[1] : mins[1];
			if (gi.pointcontents (start) != CONTENTS_SOLID)
				goto realcheck;
		}
	}

	return true;		// we got out easy

realcheck:
//
// check it for real...
//
	start[2] = mins[2];
	
// the midpoint must be within 16 of the bottom
	start[0] = stop[0] = (mins[0] + maxs[0])*0.5;
	start[1] = stop[1] = (mins[1] + maxs[1])*0.5;
	stop[2] = start[2] - 2*STEPSIZE;
	trace.Trace (start, stop, Entity, CONTENTS_MASK_MONSTERSOLID);

	if (trace.fraction == 1.0)
		return false;
	mid = bottom = trace.endPos[2];
	
// the corners must be within 16 of the midpoint	
	for	(x=0 ; x<=1 ; x++)
		for	(y=0 ; y<=1 ; y++)
		{
			start[0] = stop[0] = x ? maxs[0] : mins[0];
			start[1] = stop[1] = y ? maxs[1] : mins[1];
			
			trace.Trace (start, stop, Entity, CONTENTS_MASK_MONSTERSOLID);
			
			if (trace.fraction != 1.0 && trace.endPos[2] > bottom)
				bottom = trace.endPos[2];
			if (trace.fraction == 1.0 || mid - trace.endPos[2] > STEPSIZE)
				return false;
		}

	return true;
}

void CMonster::MoveToGoal (float Dist)
{
	edict_t *goal = Entity->goalentity;

	if (!Entity->groundentity && !(Entity->flags & (FL_FLY|FL_SWIM)))
		return;

// if the next step hits the enemy, return immediately
	if (Entity->enemy && CloseEnough (Entity->enemy, Dist) )
		return;

// bump around...
	if ( (rand()&3)==1 || !StepDirection (IdealYaw, Dist))
	{
		if (Entity->inUse)
			NewChaseDir (goal, Dist);
	}
}

bool CMonster::WalkMove (float Yaw, float Dist)
{
	vec3_t	move;
	
	if (!Entity->groundentity && !(Entity->flags & (FL_FLY|FL_SWIM)))
		return false;

	Yaw = Yaw*M_PI*2 / 360;
	
	move[0] = cosf(Yaw)*Dist;
	move[1] = sinf(Yaw)*Dist;
	move[2] = 0;

	return MoveStep(move, true);
}

bool CMonster::MoveStep (vec3_t move, bool ReLink)
{
	float		dz;
	vec3_t		oldorg, neworg, end;
	CTrace		trace;
	int			i;
	float		stepsize;
	vec3_t		test;
	int			contents;

// try the move	
	Vec3Copy (Entity->s.origin, oldorg);
	Vec3Add (Entity->s.origin, move, neworg);

// flying monsters don't step up
	if ( Entity->flags & (FL_SWIM | FL_FLY) )
	{
	// try one move with vertical motion, then one without
		for (i=0 ; i<2 ; i++)
		{
			Vec3Add (Entity->s.origin, move, neworg);
			if (i == 0 && Entity->enemy)
			{
				if (!Entity->goalentity)
					Entity->goalentity = Entity->enemy;
				dz = Entity->s.origin[2] - Entity->goalentity->s.origin[2];
				if (Entity->goalentity->client)
				{
					if (dz > 40)
						neworg[2] -= 8;
					if (!((Entity->flags & FL_SWIM) && (Entity->waterlevel < 2)))
						if (dz < 30)
							neworg[2] += 8;
				}
				else
				{
					if (dz > 8)
						neworg[2] -= 8;
					else if (dz > 0)
						neworg[2] -= dz;
					else if (dz < -8)
						neworg[2] += 8;
					else
						neworg[2] += dz;
				}
			}
			trace.Trace (Entity->s.origin, Entity->mins, Entity->maxs, neworg, Entity, CONTENTS_MASK_MONSTERSOLID);
	
			// fly monsters don't enter water voluntarily
			if (Entity->flags & FL_FLY)
			{
				if (!Entity->waterlevel)
				{
					test[0] = trace.endPos[0];
					test[1] = trace.endPos[1];
					test[2] = trace.endPos[2] + Entity->mins[2] + 1;
					contents = gi.pointcontents(test);
					if (contents & CONTENTS_MASK_WATER)
						return false;
				}
			}

			// swim monsters don't exit water voluntarily
			if (Entity->flags & FL_SWIM)
			{
				if (Entity->waterlevel < 2)
				{
					test[0] = trace.endPos[0];
					test[1] = trace.endPos[1];
					test[2] = trace.endPos[2] + Entity->mins[2] + 1;
					contents = gi.pointcontents(test);
					if (!(contents & CONTENTS_MASK_WATER))
						return false;
				}
			}

			if (trace.fraction == 1)
			{
				Vec3Copy (trace.endPos, Entity->s.origin);
				if (ReLink)
				{
					gi.linkentity (Entity);
					G_TouchTriggers (Entity);
				}
				return true;
			}
			
			if (!Entity->enemy)
				break;
		}
		
		return false;
	}

	// push down from a step height above the wished position
	if (!(AIFlags & AI_NOSTEP))
		stepsize = STEPSIZE;
	else
		stepsize = 1;

	neworg[2] += stepsize;
	Vec3Copy (neworg, end);
	end[2] -= stepsize*2;

	trace.Trace (neworg, Entity->mins, Entity->maxs, end, Entity, CONTENTS_MASK_MONSTERSOLID);

	if (trace.allSolid)
		return false;

	if (trace.startSolid)
	{
		neworg[2] -= stepsize;
		trace.Trace (neworg, Entity->mins, Entity->maxs, end, Entity, CONTENTS_MASK_MONSTERSOLID);
		if (trace.allSolid || trace.startSolid)
			return false;
	}


	// don't go in to water
	if (Entity->waterlevel == 0)
	{
		test[0] = trace.endPos[0];
		test[1] = trace.endPos[1];
		test[2] = trace.endPos[2] + Entity->mins[2] + 1;	
		contents = gi.pointcontents(test);

		if (contents & CONTENTS_MASK_WATER)
			return false;
	}

	if (trace.fraction == 1)
	{
	// if monster had the ground pulled out, go ahead and fall
		if ( Entity->flags & FL_PARTIALGROUND )
		{
			Vec3Add (Entity->s.origin, move, Entity->s.origin);
			if (ReLink)
			{
				gi.linkentity (Entity);
				G_TouchTriggers (Entity);
			}
			Entity->groundentity = NULL;
			return true;
		}
	
		return false;		// walked off an edge
	}

// check point traces down for dangling corners
	Vec3Copy (trace.endPos, Entity->s.origin);
	
	if (!CheckBottom ())
	{
		if ( Entity->flags & FL_PARTIALGROUND )
		{	// entity had floor mostly pulled out from underneath it
			// and is trying to correct
			if (ReLink)
			{
				gi.linkentity (Entity);
				G_TouchTriggers (Entity);
			}
			return true;
		}
		Vec3Copy (oldorg, Entity->s.origin);
		return false;
	}

	//if ( Entity->flags & FL_PARTIALGROUND )
	Entity->flags &= ~FL_PARTIALGROUND;

	Entity->groundentity = trace.ent;
	Entity->groundentity_linkcount = trace.ent->linkCount;

// the move is ok
	if (ReLink)
	{
		gi.linkentity (Entity);
		G_TouchTriggers (Entity);
	}
	return true;
}

bool CMonster::CloseEnough (edict_t *Goal, float Dist)
{
	for (int i=0 ; i<3 ; i++)
	{
		if (Goal->absMin[i] > Entity->absMax[i] + Dist)
			return false;
		if (Goal->absMax[i] < Entity->absMin[i] - Dist)
			return false;
	}
	return true;
}

void CMonster::NewChaseDir (edict_t *Enemy, float Dist)
{
	float	deltax,deltay;
	vec3_t	d;
	float	tdir, olddir, turnaround;

	//FIXME: how did we get here with no enemy
	if (!Enemy)
		return;

	olddir = AngleModf ((int)(IdealYaw/45)*45);
	turnaround = AngleModf (olddir - 180);

	deltax = Enemy->s.origin[0] - Entity->s.origin[0];
	deltay = Enemy->s.origin[1] - Entity->s.origin[1];
	if (deltax>10)
		d[1]= 0;
	else if (deltax<-10)
		d[1]= 180;
	else
		d[1]= DI_NODIR;
	if (deltay<-10)
		d[2]= 270;
	else if (deltay>10)
		d[2]= 90;
	else
		d[2]= DI_NODIR;

// try direct route
	if (d[1] != DI_NODIR && d[2] != DI_NODIR)
	{
		if (d[1] == 0)
			tdir = d[2] == 90 ? 45 : 315;
		else
			tdir = d[2] == 90 ? 135 : 215;
			
		if (tdir != turnaround && StepDirection(tdir, Dist))
			return;
	}

// try other directions
	if ( ((rand()&3) & 1) ||  abs(deltay)>abs(deltax))
	{
		tdir=d[1];
		d[1]=d[2];
		d[2]=tdir;
	}

	if (d[1]!=DI_NODIR && d[1]!=turnaround 
	&& StepDirection(d[1], Dist))
			return;

	if (d[2]!=DI_NODIR && d[2]!=turnaround
	&& StepDirection(d[2], Dist))
			return;

/* there is no direct path to the player, so pick another direction */

	if (olddir!=DI_NODIR && StepDirection(olddir, Dist))
			return;

	if (rand()&1) 	/*randomly determine direction of search*/
	{
		for (tdir=0 ; tdir<=315 ; tdir += 45)
			if (tdir!=turnaround && StepDirection(tdir, Dist) )
					return;
	}
	else
	{
		for (tdir=315 ; tdir >=0 ; tdir -= 45)
			if (tdir!=turnaround && StepDirection(tdir, Dist) )
					return;
	}

	if (turnaround != DI_NODIR && StepDirection(turnaround, Dist) )
			return;

	IdealYaw = olddir;		// can't move

// if a bridge was pulled out from underneath a monster, it may not have
// a valid standing position at all

	if (!CheckBottom ())
		Entity->flags |= FL_PARTIALGROUND;
}

bool CMonster::StepDirection (float Yaw, float Dist)
{
	vec3_t		move, oldorigin;
	float		delta;
	
	IdealYaw = Yaw;
	ChangeYaw ();
	
	Yaw = Yaw*M_PI*2 / 360;
	move[0] = cosf(Yaw)*Dist;
	move[1] = sinf(Yaw)*Dist;
	move[2] = 0;

	Vec3Copy (Entity->s.origin, oldorigin);
	if (MoveStep (move, false))
	{
		delta = Entity->s.angles[YAW] - IdealYaw;
		// not turned far enough, so don't take the step
		if (delta > 45 && delta < 315)
			Vec3Copy (oldorigin, Entity->s.origin);
		gi.linkentity (Entity);
		G_TouchTriggers (Entity);
		return true;
	}
	gi.linkentity (Entity);
	G_TouchTriggers (Entity);
	return false;
}

void CMonster::WalkMonsterStartGo ()
{
	if (!(Entity->spawnflags & 2) && level.time < 1)
	{
		DropToFloor ();

		if (Entity->groundentity)
		{
			if (!WalkMove (0, 0))
				MapPrint (MAPPRINT_WARNING, Entity, Entity->s.origin, "In solid\n");
		}
	}
	
	if (!YawSpeed)
		YawSpeed = 20;
	Entity->viewheight = 25;

	MonsterStartGo ();

	if (Entity->spawnflags & 2)
		MonsterTriggeredStart ();
}

void CMonster::WalkMonsterStart ()
{
	Think = &CMonster::WalkMonsterStartGo;
	NextThink = level.time + .1;
	MonsterStart ();
}

void CMonster::SwimMonsterStartGo ()
{
	if (!YawSpeed)
		YawSpeed = 10;
	Entity->viewheight = 10;

	MonsterStartGo ();

	if (Entity->spawnflags & 2)
		MonsterTriggeredStart ();
}

void CMonster::SwimMonsterStart ()
{
	Entity->flags |= FL_SWIM;
	Think = &CMonster::SwimMonsterStartGo;
	MonsterStart ();
}

void CMonster::FlyMonsterStartGo ()
{
	if (!WalkMove (0, 0))
		MapPrint (MAPPRINT_WARNING, Entity, Entity->s.origin, "Entity in solid\n");

	if (!YawSpeed)
		YawSpeed = 10;
	Entity->viewheight = 25;

	MonsterStartGo ();

	if (Entity->spawnflags & 2)
		MonsterTriggeredStart ();
}

void CMonster::FlyMonsterStart ()
{
	Entity->flags |= FL_FLY;
	Think = &CMonster::FlyMonsterStartGo;
	MonsterStart ();
}

void CMonster::MonsterStartGo ()
{
	vec3_t	v;

	if (Entity->health <= 0)
		return;

	// check for target to combat_point and change to combattarget
	if (Entity->target)
	{
		bool		notcombat = false, fixup = false;
		edict_t		*target = NULL;

		while ((target = G_Find (target, FOFS(targetname), Entity->target)) != NULL)
		{
			if (strcmp(target->classname, "point_combat") == 0)
			{
				Entity->combattarget = Entity->target;
				fixup = true;
			}
			else
				notcombat = true;
		}
		if (notcombat && Entity->combattarget)
			MapPrint (MAPPRINT_WARNING, Entity, Entity->s.origin, "Target with mixed types\n");
			//gi.dprintf("%s at (%f %f %f) has target with mixed types\n", self->classname, self->s.origin[0], self->s.origin[1], self->s.origin[2]);
		if (fixup)
			Entity->target = NULL;
	}

	// validate combattarget
	if (Entity->combattarget)
	{
		edict_t		*target;

		target = NULL;
		while ((target = G_Find (target, FOFS(targetname), Entity->combattarget)) != NULL)
		{
			if (strcmp(target->classname, "point_combat") != 0)
				MapPrint (MAPPRINT_WARNING, Entity, Entity->s.origin, "Has a bad combattarget (\"%s\")\n", Entity->combattarget);
		}
	}

	if (Entity->target)
	{
		Entity->goalentity = Entity->movetarget = G_PickTarget(Entity->target);
		if (!Entity->movetarget)
		{
			//gi.dprintf ("%s can't find target %s at (%f %f %f)\n", self->classname, self->target, self->s.origin[0], self->s.origin[1], self->s.origin[2]);
			MapPrint (MAPPRINT_WARNING, Entity, Entity->s.origin, "Can't find target\n");
			Entity->target = NULL;
			PauseTime = 100000000;
			Stand ();
		}
		else if (strcmp (Entity->movetarget->classname, "path_corner") == 0)
		{
			Vec3Subtract (Entity->goalentity->s.origin, Entity->s.origin, v);
			IdealYaw = Entity->s.angles[YAW] = VecToYaw(v);
			Walk ();
			Entity->target = NULL;
		}
		else
		{
			Entity->goalentity = Entity->movetarget = NULL;
			PauseTime = 100000000;
			Stand ();
		}
	}
	else
	{
		PauseTime = 100000000;
		Stand ();
	}

	Think = &CMonster::MonsterThink;
	NextThink = level.time + FRAMETIME;
}

void CMonster::MonsterStart ()
{
	if (deathmatch->Integer())
	{
		G_FreeEdict (Entity);
		return;
	}

	if ((Entity->spawnflags & 4) && !(AIFlags & AI_GOOD_GUY))
	{
		Entity->spawnflags &= ~4;
		Entity->spawnflags |= 1;
	}

	if (!(AIFlags & AI_GOOD_GUY))
		level.total_monsters++;

	NextThink = level.time + FRAMETIME;
	Entity->svFlags |= SVF_MONSTER;
	Entity->s.renderFx |= RF_FRAMELERP;
	Entity->takedamage = DAMAGE_AIM;
	Entity->air_finished = level.time + 12;
	Entity->use = &CMonster::MonsterUse;
	Entity->max_health = Entity->health;
	Entity->clipMask = CONTENTS_MASK_MONSTERSOLID;

	Entity->deadflag = DEAD_NO;
	Entity->svFlags &= ~SVF_DEADMONSTER;

	Vec3Copy (Entity->s.origin, Entity->s.oldOrigin);

	if (st.item)
	{
		Entity->item = FindItemByClassname (st.item);
		if (!Entity->item)
			MapPrint (MAPPRINT_WARNING, Entity, Entity->s.origin, "Bad item: \"%s\"\n", st.item);
			//gi.dprintf("%s at (%f %f %f) has bad item: %s\n", self->classname, self->s.origin[0], self->s.origin[1], self->s.origin[2], st.item);
	}

	// randomize what frame they start on
	if (CurrentMove)
		Entity->s.frame = CurrentMove->FirstFrame + (rand() % (CurrentMove->LastFrame - CurrentMove->FirstFrame + 1));

	Entity->think = Monster_Think;
	Entity->nextthink = level.time + .1;
}

void CMonster::MonsterTriggeredStart ()
{
	Entity->solid = SOLID_NOT;
	Entity->movetype = MOVETYPE_NONE;
	Entity->svFlags |= SVF_NOCLIENT;
	NextThink = 0;
	Think = NULL;
	Entity->use = &CMonster::MonsterTriggeredSpawnUse;
}

void _cdecl CMonster::MonsterUse (edict_t *self, edict_t *other, edict_t *activator)
{
	if (self->enemy || !self->Monster)
		return;
	if (self->health <= 0)
		return;
	if (activator->flags & FL_NOTARGET)
		return;
	if (!(activator->client) && !(self->Monster->AIFlags & AI_GOOD_GUY))
		return;
	
// delay reaction so if the monster is teleported, its sound is still heard
	self->enemy = activator;
	self->Monster->FoundTarget ();
}

void _cdecl CMonster::MonsterTriggeredSpawnUse (edict_t *self, edict_t *other, edict_t *activator)
{
	if (!self->Monster)
		return;

	// we have a one frame delay here so we don't telefrag the guy who activated us
	self->Monster->Think = &CMonster::MonsterTriggeredSpawn;
	self->Monster->NextThink = level.time + FRAMETIME;
	if (activator->client)
		self->enemy = activator;
	self->use = &CMonster::MonsterUse;
}

void CMonster::MonsterTriggeredSpawn ()
{
	Entity->s.origin[2] += 1;
	KillBox (Entity);

	Entity->solid = SOLID_BBOX;
	Entity->movetype = MOVETYPE_STEP;
	Entity->svFlags &= ~SVF_NOCLIENT;
	Entity->air_finished = level.time + 12;
	gi.linkentity (Entity);

	MonsterStartGo ();

	if (Entity->enemy && !(Entity->spawnflags & 1) && !(Entity->enemy->flags & FL_NOTARGET))
		FoundTarget ();
	else
		Entity->enemy = NULL;
}

void CMonster::MonsterFireBullet (vec3_t start, vec3_t dir, int damage, int kick, int hspread, int vspread, int flashtype)
{
	fire_bullet (Entity, start, dir, damage, kick, hspread, vspread, MOD_UNKNOWN);

	gi.WriteByte (SVC_MUZZLEFLASH2);
	gi.WriteShort (Entity - g_edicts);
	gi.WriteByte (flashtype);
	gi.multicast (start, MULTICAST_PVS);
}

void CMonster::MonsterFireShotgun (vec3_t start, vec3_t aimdir, int damage, int kick, int hspread, int vspread, int count, int flashtype)
{
	fire_shotgun (Entity, start, aimdir, damage, kick, hspread, vspread, count, MOD_UNKNOWN);

	gi.WriteByte (SVC_MUZZLEFLASH2);
	gi.WriteShort (Entity - g_edicts);
	gi.WriteByte (flashtype);
	gi.multicast (start, MULTICAST_PVS);
}

void CMonster::MonsterFireBlaster (vec3_t start, vec3_t dir, int damage, int speed, int flashtype, int effect)
{
	fire_blaster (Entity, start, dir, damage, speed, effect, false);

	gi.WriteByte (SVC_MUZZLEFLASH2);
	gi.WriteShort (Entity - g_edicts);
	gi.WriteByte (flashtype);
	gi.multicast (start, MULTICAST_PVS);
}	

void CMonster::MonsterFireGrenade (vec3_t start, vec3_t aimdir, int damage, int speed, int flashtype)
{
	fire_grenade (Entity, start, aimdir, damage, speed, 2.5, damage+40);

	gi.WriteByte (SVC_MUZZLEFLASH2);
	gi.WriteShort (Entity - g_edicts);
	gi.WriteByte (flashtype);
	gi.multicast (start, MULTICAST_PVS);
}

void CMonster::MonsterFireRocket (vec3_t start, vec3_t dir, int damage, int speed, int flashtype)
{
	fire_rocket (Entity, start, dir, damage, speed, damage+20, damage);

	gi.WriteByte (SVC_MUZZLEFLASH2);
	gi.WriteShort (Entity - g_edicts);
	gi.WriteByte (flashtype);
	gi.multicast (start, MULTICAST_PVS);
}	

void CMonster::MonsterFireRailgun (vec3_t start, vec3_t aimdir, int damage, int kick, int flashtype)
{
	fire_rail (Entity, start, aimdir, damage, kick);

	gi.WriteByte (SVC_MUZZLEFLASH2);
	gi.WriteShort (Entity - g_edicts);
	gi.WriteByte (flashtype);
	gi.multicast (start, MULTICAST_PVS);
}

void CMonster::MonsterFireBfg (vec3_t start, vec3_t aimdir, int damage, int speed, int kick, float damage_radius, int flashtype)
{
	fire_bfg (Entity, start, aimdir, damage, speed, damage_radius);

	gi.WriteByte (SVC_MUZZLEFLASH2);
	gi.WriteShort (Entity - g_edicts);
	gi.WriteByte (flashtype);
	gi.multicast (start, MULTICAST_PVS);
}

bool CMonster::CheckAttack ()
{
	vec3_t	spot1, spot2;
	float	chance;
	CTrace	tr;

	if (Entity->enemy->health > 0)
	{
	// see if any entities are in the way of the shot
		Vec3Copy (Entity->s.origin, spot1);
		spot1[2] += Entity->viewheight;
		Vec3Copy (Entity->enemy->s.origin, spot2);
		spot2[2] += Entity->enemy->viewheight;

		tr.Trace (spot1, spot2, Entity, CONTENTS_SOLID|CONTENTS_MONSTER|CONTENTS_SLIME|CONTENTS_LAVA|CONTENTS_WINDOW);

		// do we have a clear shot?
		if (tr.ent != Entity->enemy)
			return false;
	}
	
	// melee attack
	if (EnemyRange == RANGE_MELEE)
	{
		// don't always melee in easy mode
		if (skill->Integer() == 0 && (rand()&3) )
			return false;
		if (MonsterFlags & MF_HAS_MELEE)
			AttackState = AS_MELEE;
		else
			AttackState = AS_MISSILE;
		return true;
	}
	
// missile attack
	if (!(MonsterFlags & MF_HAS_ATTACK))
		return false;
		
	if (level.time < AttackFinished)
		return false;
		
	if (EnemyRange == RANGE_FAR)
		return false;

	if (AIFlags & AI_STAND_GROUND)
	{
		chance = 0.4f;
	}
	else if (EnemyRange == RANGE_MELEE)
	{
		chance = 0.2f;
	}
	else if (EnemyRange == RANGE_NEAR)
	{
		chance = 0.1f;
	}
	else if (EnemyRange == RANGE_MID)
	{
		chance = 0.02f;
	}
	else
	{
		return false;
	}

	if (skill->Integer() == 0)
		chance *= 0.5;
	else if (skill->Integer() >= 2)
		chance *= 2;

	if (random () < chance)
	{
		AttackState = AS_MISSILE;
		AttackFinished = level.time + 2*random();
		return true;
	}

	if (Entity->flags & FL_FLY)
	{
		if (random() < 0.3)
			AttackState = AS_SLIDING;
		else
			AttackState = AS_STRAIGHT;
	}

	return false;
}

void CMonster::DropToFloor ()
{
	vec3_t		end;
	CTrace		trace;

	Entity->s.origin[2] += 1;
	Vec3Copy (Entity->s.origin, end);
	end[2] -= 256;
	
	trace.Trace (Entity->s.origin, Entity->mins, Entity->maxs, end, Entity, CONTENTS_MASK_MONSTERSOLID);

	if (trace.fraction == 1 || trace.allSolid)
		return;

	Vec3Copy (trace.endPos, Entity->s.origin);

	gi.linkentity (Entity);
	CheckGround ();
	CatagorizePosition ();
}

void CMonster::AI_Charge(float Dist)
{
	vec3_t	v;

	Vec3Subtract (Entity->enemy->s.origin, Entity->s.origin, v);
	IdealYaw = VecToYaw(v);
	ChangeYaw ();

	if (Dist)
		WalkMove (Entity->s.angles[YAW], Dist);
}

bool CMonster::AI_CheckAttack()
{
	vec3_t		temp;
	bool		hesDeadJim = false;

// this causes monsters to run blindly to the combat point w/o firing
	if (Entity->goalentity)
	{
		if (AIFlags & AI_COMBAT_POINT)
			return false;

		if (AIFlags & AI_SOUND_TARGET)
		{
			if ((level.time - Entity->enemy->teleport_time) > 5.0)
			{
				if (Entity->goalentity == Entity->enemy)
					if (Entity->movetarget)
						Entity->goalentity = Entity->movetarget;
					else
						Entity->goalentity = NULL;
				AIFlags &= ~AI_SOUND_TARGET;
				if (AIFlags & AI_TEMP_STAND_GROUND)
					AIFlags &= ~(AI_STAND_GROUND | AI_TEMP_STAND_GROUND);
			}
			else
			{
				Entity->show_hostile = level.time + 1;
				return false;
			}
		}
	}

// see if the enemy is dead
	if ((!Entity->enemy) || (!Entity->enemy->inUse))
		hesDeadJim = true;
	else if (AIFlags & AI_MEDIC)
	{
		if (Entity->enemy->health > 0)
		{
			hesDeadJim = true;
			AIFlags &= ~AI_MEDIC;
		}
	}
	else
	{
		if (AIFlags & AI_BRUTAL)
		{
			if (Entity->enemy->health <= -80)
				hesDeadJim = true;
		}
		else
		{
			if (Entity->enemy->health <= 0)
				hesDeadJim = true;
		}
	}

	if (hesDeadJim)
	{
		Entity->enemy = NULL;
	// FIXME: look all around for other targets
		if (Entity->oldenemy && Entity->oldenemy->health > 0)
		{
			Entity->enemy = Entity->oldenemy;
			Entity->oldenemy = NULL;
			HuntTarget ();
		}
		else
		{
			if (Entity->movetarget)
			{
				Entity->goalentity = Entity->movetarget;
				Walk ();
			}
			else
			{
				// we need the pausetime otherwise the stand code
				// will just revert to walking with no target and
				// the monsters will wonder around aimlessly trying
				// to hunt the world entity
				PauseTime = level.time + 100000000;
				Stand ();
			}
			return true;
		}
	}

	Entity->show_hostile = level.time + 1;		// wake up other monsters

// check knowledge of enemy
	EnemyVis = visible(Entity, Entity->enemy);
	if (EnemyVis)
	{
		SearchTime = level.time + 5;
		Vec3Copy (Entity->enemy->s.origin, LastSighting);
	}

	EnemyInfront = infront(Entity, Entity->enemy);
	EnemyRange = range(Entity, Entity->enemy);
	Vec3Subtract (Entity->enemy->s.origin, Entity->s.origin, temp);
	EnemyYaw = VecToYaw(temp);

	// JDC self->ideal_yaw = enemy_yaw;

	if (AttackState == AS_MISSILE)
	{
		AI_Run_Missile ();
		return true;
	}
	else if (AttackState == AS_MELEE)
	{
		AI_Run_Melee ();
		return true;
	}

	// if enemy is not currently visible, we will never attack
	if (!EnemyVis)
		return false;

	if (Entity->enemy->client && Entity->enemy->flags & FL_GODMODE)
		return false;
	return CheckAttack ();
}

void CMonster::AI_Move (float Dist)
{
	WalkMove (Entity->s.angles[YAW], Dist);
}

void CMonster::AI_Run(float Dist)
{
	vec3_t		v;
	edict_t		*tempgoal;
	edict_t		*save;
	bool		isNew;
	edict_t		*marker;
	float		d1, d2;
	CTrace		tr;
	vec3_t		v_forward, v_right;
	float		left, center, right;
	vec3_t		left_target, right_target;

	// if we're going to a combat point, just proceed
	if (AIFlags & AI_COMBAT_POINT)
	{
		MoveToGoal (Dist);
		return;
	}

	if (AIFlags & AI_SOUND_TARGET)
	{
		Vec3Subtract (Entity->s.origin, Entity->enemy->s.origin, v);
		if (Vec3Length(v) < 64)
		{
			AIFlags |= (AI_STAND_GROUND | AI_TEMP_STAND_GROUND);
			Stand ();
			return;
		}

		MoveToGoal (Dist);

		if (!FindTarget ())
			return;
	}

	if (AI_CheckAttack ())
		return;

	if (AttackState == AS_SLIDING)
	{
		AI_Run_Slide (Dist);
		return;
	}

	if (EnemyVis)
	{
//		if (self.aiflags & AI_LOST_SIGHT)
//			dprint("regained sight\n");
		MoveToGoal (Dist);
		AIFlags &= ~AI_LOST_SIGHT;
		Vec3Copy (Entity->enemy->s.origin, LastSighting);
		TrailTime = level.time;
		return;
	}

	// coop will change to another enemy if visible
	if (coop->Integer())
	{	// FIXME: insane guys get mad with this, which causes crashes!
		if (FindTarget ())
			return;
	}

	if (SearchTime && (level.time > (SearchTime + 20)))
	{
		MoveToGoal (Dist);
		SearchTime = 0;
//		dprint("search timeout\n");
		return;
	}

	save = Entity->goalentity;
	tempgoal = G_Spawn();
	Entity->goalentity = tempgoal;

	isNew = false;

	if (!(AIFlags & AI_LOST_SIGHT))
	{
		// just lost sight of the player, decide where to go first
//		dprint("lost sight of player, last seen at "); dprint(vtos(self.last_sighting)); dprint("\n");
		AIFlags |= (AI_LOST_SIGHT | AI_PURSUIT_LAST_SEEN);
		AIFlags &= ~(AI_PURSUE_NEXT | AI_PURSUE_TEMP);
		isNew = true;
	}

	if (AIFlags & AI_PURSUE_NEXT)
	{
		AIFlags &= ~AI_PURSUE_NEXT;
//		dprint("reached current goal: "); dprint(vtos(self.origin)); dprint(" "); dprint(vtos(self.last_sighting)); dprint(" "); dprint(ftos(vlen(self.origin - self.last_sighting))); dprint("\n");

		// give ourself more time since we got this far
		SearchTime = level.time + 5;

		if (AIFlags & AI_PURSUE_TEMP)
		{
//			dprint("was temp goal; retrying original\n");
			AIFlags &= ~AI_PURSUE_TEMP;
			marker = NULL;
			Vec3Copy (SavedGoal, LastSighting);
			isNew = true;
		}
		else if (AIFlags & AI_PURSUIT_LAST_SEEN)
		{
			AIFlags &= ~AI_PURSUIT_LAST_SEEN;
			marker = PlayerTrail_PickFirst (Entity);
		}
		else
			marker = PlayerTrail_PickNext (Entity);

		if (marker)
		{
			Vec3Copy (marker->s.origin, LastSighting);
			TrailTime = marker->timestamp;
			Entity->s.angles[YAW] = IdealYaw = marker->s.angles[YAW];
//			dprint("heading is "); dprint(ftos(self.ideal_yaw)); dprint("\n");

//			debug_drawline(self.origin, self.last_sighting, 52);
			isNew = true;
		}
	}

	Vec3Subtract (Entity->s.origin, LastSighting, v);
	d1 = Vec3Length(v);
	if (d1 <= Dist)
	{
		AIFlags |= AI_PURSUE_NEXT;
		Dist = d1;
	}

	Vec3Copy (LastSighting, Entity->goalentity->s.origin);

	if (isNew)
	{
//		gi.dprintf("checking for course correction\n");

		tr.Trace (Entity->s.origin, Entity->mins, Entity->maxs, LastSighting, Entity, CONTENTS_MASK_PLAYERSOLID);
		if (tr.fraction < 1)
		{
			Vec3Subtract (Entity->goalentity->s.origin, Entity->s.origin, v);
			d1 = Vec3Length(v);
			center = tr.fraction;
			d2 = d1 * ((center+1)/2);
			Entity->s.angles[YAW] = IdealYaw = VecToYaw(v);
			Angles_Vectors(Entity->s.angles, v_forward, v_right, NULL);

			Vec3Set (v, d2, -16, 0);
			G_ProjectSource (Entity->s.origin, v, v_forward, v_right, left_target);
			tr.Trace(Entity->s.origin, Entity->mins, Entity->maxs, left_target, Entity, CONTENTS_MASK_PLAYERSOLID);
			left = tr.fraction;

			Vec3Set (v, d2, 16, 0);
			G_ProjectSource (Entity->s.origin, v, v_forward, v_right, right_target);
			tr.Trace(Entity->s.origin, Entity->mins, Entity->maxs, right_target, Entity, CONTENTS_MASK_PLAYERSOLID);
			right = tr.fraction;

			center = (d1*center)/d2;
			if (left >= center && left > right)
			{
				if (left < 1)
				{
					Vec3Set (v, d2 * left * 0.5, -16, 0);
					G_ProjectSource (Entity->s.origin, v, v_forward, v_right, left_target);
//					gi.dprintf("incomplete path, go part way and adjust again\n");
				}
				Vec3Copy (LastSighting, SavedGoal);
				AIFlags |= AI_PURSUE_TEMP;
				Vec3Copy (left_target, Entity->goalentity->s.origin);
				Vec3Copy (left_target, LastSighting);
				Vec3Subtract (Entity->goalentity->s.origin, Entity->s.origin, v);
				Entity->s.angles[YAW] = IdealYaw = VecToYaw(v);
//				gi.dprintf("adjusted left\n");
//				debug_drawline(self.origin, self.last_sighting, 152);
			}
			else if (right >= center && right > left)
			{
				if (right < 1) {
					Vec3Set (v, d2 * right * 0.5, 16, 0);
					G_ProjectSource (Entity->s.origin, v, v_forward, v_right, right_target);
//					gi.dprintf("incomplete path, go part way and adjust again\n");
				}
				Vec3Copy (LastSighting, SavedGoal);
				AIFlags |= AI_PURSUE_TEMP;
				Vec3Copy (right_target, Entity->goalentity->s.origin);
				Vec3Copy (right_target, LastSighting);
				Vec3Subtract (Entity->goalentity->s.origin, Entity->s.origin, v);
				Entity->s.angles[YAW] = IdealYaw = VecToYaw(v);
//				gi.dprintf("adjusted right\n");
//				debug_drawline(self.origin, self.last_sighting, 152);
			}
		}
//		else gi.dprintf("course was fine\n");
	}

	MoveToGoal (Dist);

	G_FreeEdict(tempgoal);

	if (Entity)
		Entity->goalentity = save;
}

void CMonster::AI_Run_Melee ()
{
	IdealYaw = EnemyYaw;
	ChangeYaw ();

	if (FacingIdeal())
	{
		Melee ();
		AttackState = AS_STRAIGHT;
	}
}

void CMonster::AI_Run_Missile()
{
	IdealYaw = EnemyYaw;
	ChangeYaw ();

	if (FacingIdeal())
	{
		Attack ();
		AttackState = AS_STRAIGHT;
	}
}

void CMonster::AI_Run_Slide(float Dist)
{
	IdealYaw = EnemyYaw;
	ChangeYaw ();

	if (WalkMove (IdealYaw + ((Lefty) ? 90 : -90), Dist))
		return;
		
	Lefty = !Lefty;
	WalkMove (IdealYaw - ((Lefty) ? 90 : -90), Dist);
}

void CMonster::AI_Stand (float Dist)
{
	if (Dist)
		WalkMove (Entity->s.angles[YAW], Dist);

	if (AIFlags & AI_STAND_GROUND)
	{
		if (Entity->enemy)
		{
			vec3_t v;
			Vec3Subtract (Entity->enemy->s.origin, Entity->s.origin, v);
			IdealYaw = VecToYaw(v);
			if (Entity->s.angles[YAW] != IdealYaw && AIFlags & AI_TEMP_STAND_GROUND)
			{
				AIFlags &= ~(AI_STAND_GROUND | AI_TEMP_STAND_GROUND);
				Run ();
			}
			ChangeYaw ();
			AI_CheckAttack();
		}
		else
			FindTarget ();
		return;
	}

	if (FindTarget ())
		return;
	
	if (level.time > PauseTime)
	{
		Walk ();
		return;
	}

	if (!(Entity->spawnflags & 1) && (MonsterFlags & MF_HAS_IDLE) && (level.time > IdleTime))
	{
		if (IdleTime)
		{
			Idle ();
			IdleTime = level.time + 15 + (random() * 15);
		}
		else
			IdleTime = level.time + (random() * 15);
	}
}

void CMonster::ReactToDamage (edict_t *attacker)
{
	if (!(attacker->client) && !(attacker->Monster))
		return;

	if (attacker == Entity || attacker == Entity->enemy)
		return;

	// if we are a good guy monster and our attacker is a player
	// or another good guy, do not get mad at them
	if (AIFlags & AI_GOOD_GUY)
	{
		if (attacker->client || (attacker->Monster->AIFlags & AI_GOOD_GUY))
			return;
	}

	// we now know that we are not both good guys

	// if attacker is a client, get mad at them because he's good and we're not
	if (attacker->client)
	{
		AIFlags &= ~AI_SOUND_TARGET;

		// this can only happen in coop (both new and old enemies are clients)
		// only switch if can't see the current enemy
		if (Entity->enemy && Entity->enemy->client)
		{
			if (visible(Entity, Entity->enemy))
			{
				Entity->oldenemy = attacker;
				return;
			}
			Entity->oldenemy = Entity->enemy;
		}
		Entity->enemy = attacker;
		if (!(AIFlags & AI_DUCKED))
			FoundTarget ();
		return;
	}

	// it's the same base (walk/swim/fly) type and a different classname and it's not a tank
	// (they spray too much), get mad at them
	//if (((Entity->flags & (FL_FLY|FL_SWIM)) == (attacker->flags & (FL_FLY|FL_SWIM))) &&
	//	 (strcmp (Entity->classname, attacker->classname) != 0))// &&
		// (strcmp(attacker->classname, "monster_tank") != 0) &&
		// (strcmp(attacker->classname, "monster_supertank") != 0) &&
		// (strcmp(attacker->classname, "monster_makron") != 0) &&
		// (strcmp(attacker->classname, "monster_jorg") != 0) )
	{
		if (Entity->enemy && Entity->enemy->client)
			Entity->oldenemy = Entity->enemy;
		Entity->enemy = attacker;
		if (!(AIFlags & AI_DUCKED))
			FoundTarget ();
	}
	// if they *meant* to shoot us, then shoot back
	/*else */if (attacker->enemy == Entity)
	{
		if (Entity->enemy && Entity->enemy->client)
			Entity->oldenemy = Entity->enemy;
		Entity->enemy = attacker;
		if (!(AIFlags & AI_DUCKED))
			FoundTarget ();
	}
	// otherwise get mad at whoever they are mad at (help our buddy) unless it is us!
	/*else if (attacker->enemy && attacker->enemy != Entity)
	{
		if (Entity->enemy && Entity->enemy->client)
			Entity->oldenemy = Entity->enemy;
		Entity->enemy = attacker->enemy;
		if (!(AIFlags & AI_DUCKED))
			FoundTarget ();
	}*/
}

void CMonster::AI_Turn(float Dist)
{
	if (Dist)
		WalkMove (Entity->s.angles[YAW], Dist);

	if (FindTarget ())
		return;
	
	ChangeYaw ();
}

void CMonster::AI_Walk(float Dist)
{
	MoveToGoal (Dist);

	// check for noticing a player
	if (FindTarget ())
		return;

	if ((MonsterFlags & MF_HAS_SEARCH) && (level.time > IdleTime))
	{
		if (IdleTime)
		{
			Search ();
			IdleTime = level.time + 15 + (random() * 15);
		}
		else
			IdleTime = level.time + (random() * 15);
	}
}

// These are intended to be virtually replaced.
void CMonster::Stand ()
{
}

void CMonster::Idle ()
{
	if (MonsterFlags & MF_HAS_IDLE)
		gi.dprintf ("Warning: Monster with no idle has MF_HAS_IDLE!\n");
}

void CMonster::Search ()
{
	if (MonsterFlags & MF_HAS_SEARCH)
		gi.dprintf ("Warning: Monster with no search has MF_HAS_SEARCH!\n");
}

void CMonster::Walk ()
{
}

void CMonster::Run ()
{
}

void CMonster::Dodge (edict_t *other, float eta)
{
}

void CMonster::Attack()
{
	if (MonsterFlags & MF_HAS_ATTACK)
		gi.dprintf ("Warning: Monster with no attack has MF_HAS_ATTACK!\n");
}

void CMonster::Melee ()
{
	if (MonsterFlags & MF_HAS_MELEE)
		gi.dprintf ("Warning: Monster with no melee has MF_HAS_MELEE!\n");
}

void CMonster::Sight ()
{
	if (MonsterFlags & MF_HAS_SIGHT)
		gi.dprintf ("Warning: Monster with no sight has MF_HAS_SIGHT!\n");
}

void CMonster::MonsterDeathUse ()
{
	Entity->flags &= ~(FL_FLY|FL_SWIM);
	AIFlags &= AI_GOOD_GUY;

	if (Entity->item)
	{
		Entity->item->Drop (Entity);
		Entity->item = NULL;
	}

	if (Entity->deathtarget)
		Entity->target = Entity->deathtarget;

	if (!Entity->target)
		return;

	G_UseTargets (Entity, Entity->enemy);
}

void CMonster::MonsterThink ()
{
	MoveFrame ();
	if (Entity->linkCount != LinkCount)
	{
		LinkCount = Entity->linkCount;
		CheckGround ();
	}
	CatagorizePosition ();
	WorldEffects ();
	SetEffects ();
}

void CMonster::MoveFrame ()
{
	int		index;
	CAnim	*Move = CurrentMove;

	Entity->nextthink = level.time + FRAMETIME;

	if ((NextFrame) && (NextFrame >= Move->FirstFrame) && (NextFrame <= Move->LastFrame))
	{
		Entity->s.frame = NextFrame;
		NextFrame = 0;
	}
	else
	{
		if (Entity->s.frame == Move->LastFrame)
		{
			if (Move->EndFunc)
			{
				void (CMonster::*EndFunc) () = Move->EndFunc;
				(this->*EndFunc) ();

				// regrab move, endfunc is very likely to change it
				Move = CurrentMove;

				// check for death
				if (Entity->svFlags & SVF_DEADMONSTER)
					return;
			}
		}

		if (Entity->s.frame < Move->FirstFrame || Entity->s.frame > Move->LastFrame)
		{
			AIFlags &= ~AI_HOLD_FRAME;
			Entity->s.frame = Move->FirstFrame;
		}
		else
		{
			if (!(AIFlags & AI_HOLD_FRAME))
			{
				Entity->s.frame++;
				if (Entity->s.frame > Move->LastFrame)
					Entity->s.frame = Move->FirstFrame;
			}
		}
	}

	index = Entity->s.frame - Move->FirstFrame;

	void (CMonster::*AIFunc) (float Dist) = Move->Frames[index].AIFunc;
	if (AIFunc)
		(this->*AIFunc) ((AIFlags & AI_HOLD_FRAME) ? 0 : (Move->Frames[index].Dist * Scale));

	void (CMonster::*Function) () = Move->Frames[index].Function;
	if (Function)
		(this->*Function) ();
}

void CMonster::FoundTarget ()
{
	// let other monsters see this monster for a while
	if (Entity->enemy->client)
	{
		level.sight_entity = Entity;
		level.sight_entity_framenum = level.framenum;
		level.sight_entity->light_level = 128;
	}

	Entity->show_hostile = level.time + 1;		// wake up other monsters

	Vec3Copy(Entity->enemy->s.origin, LastSighting);
	TrailTime = level.time;

	if (!Entity->combattarget)
	{
		HuntTarget ();
		return;
	}

	Entity->goalentity = Entity->movetarget = G_PickTarget(Entity->combattarget);
	if (!Entity->movetarget)
	{
		Entity->goalentity = Entity->movetarget = Entity->enemy;
		HuntTarget ();
		MapPrint (MAPPRINT_ERROR, Entity, Entity->s.origin, "combattarget %s not found\n", Entity->combattarget);
		return;
	}

	// clear out our combattarget, these are a one shot deal
	Entity->combattarget = NULL;
	AIFlags |= AI_COMBAT_POINT;

	// clear the targetname, that point is ours!
	Entity->movetarget->targetname = NULL;
	PauseTime = 0;

	// run for it
	Run ();
}

void CMonster::SetEffects()
{
	Entity->s.effects = 0;
	Entity->s.renderFx = RF_FRAMELERP;

	if (AIFlags & AI_RESURRECTING)
	{
		Entity->s.effects |= EF_COLOR_SHELL;
		Entity->s.renderFx |= RF_SHELL_RED;
	}

	if (Entity->health <= 0)
		return;

	if (Entity->powerarmor_time > level.time)
	{
		if (PowerArmorType == POWER_ARMOR_SCREEN)
			Entity->s.effects |= EF_POWERSCREEN;
		else if (PowerArmorType == POWER_ARMOR_SHIELD)
		{
			Entity->s.effects |= EF_COLOR_SHELL;
			Entity->s.renderFx |= RF_SHELL_GREEN;
		}
	}
}

void CMonster::WorldEffects()
{
	if (Entity->health > 0)
	{
		if (!(Entity->flags & FL_SWIM))
		{
			if (Entity->waterlevel < 3)
				Entity->air_finished = level.time + 12;
			else if (Entity->air_finished < level.time)
			{
				if (Entity->pain_debounce_time < level.time)
				{
					int dmg = 2 + 2 * floor(level.time - Entity->air_finished);
					if (dmg > 15)
						dmg = 15;
					T_Damage (Entity, world, world, vec3Origin, Entity->s.origin, vec3Origin, dmg, 0, DAMAGE_NO_ARMOR, MOD_WATER);
					Entity->pain_debounce_time = level.time + 1;
				}
			}
		}
		else
		{
			if (Entity->waterlevel > 0)
				Entity->air_finished = level.time + 9;
			else if (Entity->air_finished < level.time)
			{	// suffocate!
				if (Entity->pain_debounce_time < level.time)
				{
					int dmg = 2 + 2 * floor(level.time - Entity->air_finished);
					if (dmg > 15)
						dmg = 15;
					T_Damage (Entity, world, world, vec3Origin, Entity->s.origin, vec3Origin, dmg, 0, DAMAGE_NO_ARMOR, MOD_WATER);
					Entity->pain_debounce_time = level.time + 1;
				}
			}
		}
	}
	
	if (Entity->waterlevel == 0)
	{
		if (Entity->flags & FL_INWATER)
		{	
			Sound (Entity, CHAN_BODY, SoundIndex("player/watr_out.wav"));
			Entity->flags &= ~FL_INWATER;
		}
		return;
	}

	if ((Entity->watertype & CONTENTS_LAVA) && !(Entity->flags & FL_IMMUNE_LAVA))
	{
		if (Entity->damage_debounce_time < level.time)
		{
			Entity->damage_debounce_time = level.time + 0.2;
			T_Damage (Entity, world, world, vec3Origin, Entity->s.origin, vec3Origin, 10*Entity->waterlevel, 0, 0, MOD_LAVA);
		}
	}
	if ((Entity->watertype & CONTENTS_SLIME) && !(Entity->flags & FL_IMMUNE_SLIME))
	{
		if (Entity->damage_debounce_time < level.time)
		{
			Entity->damage_debounce_time = level.time + 1;
			T_Damage (Entity, world, world, vec3Origin, Entity->s.origin, vec3Origin, 4*Entity->waterlevel, 0, 0, MOD_SLIME);
		}
	}
	
	if ( !(Entity->flags & FL_INWATER) )
	{	
		if (!(Entity->svFlags & SVF_DEADMONSTER))
		{
			if (Entity->watertype & CONTENTS_LAVA)
			{
				if (random() <= 0.5)
					Sound (Entity, CHAN_BODY, SoundIndex("player/lava1.wav"));
				else
					Sound (Entity, CHAN_BODY, SoundIndex("player/lava2.wav"));
			}
			else
				Sound (Entity, CHAN_BODY, SoundIndex("player/watr_in.wav"));
		}

		Entity->flags |= FL_INWATER;
		Entity->damage_debounce_time = 0;
	}
}

void CMonster::CatagorizePosition()
{
	vec3_t		point;
	int			cont;
//
// get waterlevel
//
	point[0] = Entity->s.origin[0];
	point[1] = Entity->s.origin[1];
	point[2] = Entity->s.origin[2] + Entity->mins[2] + 1;	
	cont = gi.pointcontents (point);

	if (!(cont & CONTENTS_MASK_WATER))
	{
		Entity->waterlevel = 0;
		Entity->watertype = 0;
		return;
	}

	Entity->watertype = cont;
	Entity->waterlevel = 1;
	point[2] += 26;
	cont = gi.pointcontents (point);
	if (!(cont & CONTENTS_MASK_WATER))
		return;

	Entity->waterlevel = 2;
	point[2] += 22;
	cont = gi.pointcontents (point);
	if (cont & CONTENTS_MASK_WATER)
		Entity->waterlevel = 3;
}

void CMonster::CheckGround()
{
	vec3_t		point;
	CTrace		trace;

	if (Entity->flags & (FL_SWIM|FL_FLY))
		return;

	if (Entity->velocity[2] > 100)
	{
		Entity->groundentity = NULL;
		return;
	}

// if the hull point one-quarter unit down is solid the entity is on ground
	point[0] = Entity->s.origin[0];
	point[1] = Entity->s.origin[1];
	point[2] = Entity->s.origin[2] - 0.25;

	trace.Trace (Entity->s.origin, Entity->mins, Entity->maxs, point, Entity, CONTENTS_MASK_MONSTERSOLID);

	// check steepness
	if ( trace.plane.normal[2] < 0.7 && !trace.startSolid)
	{
		Entity->groundentity = NULL;
		return;
	}

	if (!trace.startSolid && !trace.allSolid)
	{
		Vec3Copy (trace.endPos, Entity->s.origin);
		Entity->groundentity = trace.ent;
		Entity->groundentity_linkcount = trace.ent->linkCount;
		Entity->velocity[2] = 0;
	}
}

void CMonster::HuntTarget()
{
	vec3_t	vec;

	Entity->goalentity = Entity->enemy;
	if (AIFlags & AI_STAND_GROUND)
		Stand ();
	else
		Run ();
	Vec3Subtract (Entity->enemy->s.origin, Entity->s.origin, vec);
	IdealYaw = VecToYaw(vec);
	// wait a while before first attack
	if (!(AIFlags & AI_STAND_GROUND))
		AttackFinished = level.time + 1;
}

bool CMonster::FindTarget()
{
	edict_t		*client;
	bool		heardit;
	int			r;

	if (AIFlags & AI_GOOD_GUY)
	{
		if (Entity->goalentity && Entity->goalentity->inUse && Entity->goalentity->classname)
		{
			if (strcmp(Entity->goalentity->classname, "target_actor") == 0)
				return false;
		}

		//FIXME look for monsters?
		return false;
	}

	// if we're going to a combat point, just proceed
	if (AIFlags & AI_COMBAT_POINT)
		return false;

// if the first spawnflag bit is set, the monster will only wake up on
// really seeing the player, not another monster getting angry or hearing
// something

// revised behavior so they will wake up if they "see" a player make a noise
// but not weapon impact/explosion noises

	heardit = false;
	if ((level.sight_entity_framenum >= (level.framenum - 1)) && !(Entity->spawnflags & 1) )
	{
		client = level.sight_entity;
		if (client->enemy == Entity->enemy)
			return false;
	}
	else if (level.sound_entity_framenum >= (level.framenum - 1))
	{
		client = level.sound_entity;
		heardit = true;
	}
	else if (!(Entity->enemy) && (level.sound2_entity_framenum >= (level.framenum - 1)) && !(Entity->spawnflags & 1) )
	{
		client = level.sound2_entity;
		heardit = true;
	}
	else
	{
		client = level.sight_client;
		if (!client)
			return false;	// no clients to get mad at
	}

	// if the entity went away, forget it
	if (!client->inUse)
		return false;

	if (client == Entity->enemy)
		return true;	// JDC false;

	if (client->client)
	{
		if (client->flags & FL_NOTARGET)
			return false;
	}
	else if (client->svFlags & SVF_MONSTER)
	{
		if (!client->enemy)
			return false;
		if (client->enemy->flags & FL_NOTARGET)
			return false;
	}
	else if (heardit)
	{
		if (client->owner->flags & FL_NOTARGET)
			return false;
	}
	else
		return false;

	if (!heardit)
	{
		r = range (Entity, client);

		if (r == RANGE_FAR)
			return false;

// this is where we would check invisibility

		// is client in an spot too dark to be seen?
		if (client->light_level <= 5)
			return false;

		if (!visible (Entity, client))
			return false;

		if (r == RANGE_NEAR)
		{
			if (client->show_hostile < level.time && !infront (Entity, client))
				return false;
		}
		else if (r == RANGE_MID)
		{
			if (!infront (Entity, client))
				return false;
		}

		Entity->enemy = client;

		if (strcmp(Entity->enemy->classname, "player_noise") != 0)
		{
			AIFlags &= ~AI_SOUND_TARGET;

			if (!Entity->enemy->client)
			{
				Entity->enemy = Entity->enemy->enemy;
				if (!Entity->enemy->client)
				{
					Entity->enemy = NULL;
					return false;
				}
			}
		}
	}
	else	// heardit
	{
		vec3_t	temp;

		if (Entity->spawnflags & 1)
		{
			if (!visible (Entity, client))
				return false;
		}
		else
		{
			if (!gi.inPHS(Entity->s.origin, client->s.origin))
				return false;
		}

		Vec3Subtract (client->s.origin, Entity->s.origin, temp);

		if (Vec3Length(temp) > 1000)	// too far to hear
			return false;

		// check area portals - if they are different and not connected then we can't hear it
		if (client->areaNum != Entity->areaNum)
		{
			if (!gi.AreasConnected(Entity->areaNum, client->areaNum))
				return false;
		}

		IdealYaw = VecToYaw(temp);
		ChangeYaw ();

		// hunt the sound for a bit; hopefully find the real player
		AIFlags |= AI_SOUND_TARGET;
		Entity->enemy = client;
	}

//
// got one
//
	FoundTarget ();

	if (!(AIFlags & AI_SOUND_TARGET) && (MonsterFlags & MF_HAS_SIGHT))
		Sight ();

	return true;
}

bool CMonster::FacingIdeal()
{
	float delta = AngleModf (Entity->s.angles[YAW] - IdealYaw);
	if (delta > 45 && delta < 315)
		return false;
	return true;
}

void CMonster::FliesOff()
{
	Entity->s.effects &= ~EF_FLIES;
	Entity->s.sound = 0;
}

void CMonster::FliesOn ()
{
	if (Entity->waterlevel)
		return;
	Entity->s.effects |= EF_FLIES;
	Entity->s.sound = SoundIndex ("infantry/inflies1.wav");
	Think = &CMonster::FliesOff;
	NextThink = level.time + 60;
}

void CMonster::CheckFlies ()
{
	if (Entity->waterlevel)
		return;

	if (random() > 0.5)
		return;

	Think = &CMonster::FliesOn;
	NextThink = level.time + 5 + 10 * random();
}

void Monster_Die (edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
{
	self->Monster->Die(inflictor, attacker, damage, point);
}

void Monster_Pain (edict_t *self, edict_t *other, float kick, int damage)
{
	self->Monster->Pain(other, kick, damage);
}

static uint32 lastMonsterID;
CMonster::CMonster ()
{
	MonsterID = lastMonsterID++;
}

void CMonster::Init (edict_t *ent)
{
	Entity = ent;
	Spawn ();

	Entity->die = Monster_Die;
	Entity->pain = Monster_Pain;

	Entity->think = Monster_Think;
	Entity->nextthink = level.time + .1;
}