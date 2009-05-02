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

/*CFrame TestFrames[] =
{
	CFrame(NULL, 7, NULL),
	CFrame(NULL, 15, NULL)
};
CAnim TestAnim(0, 40, TestFrames, NULL);*/

void Monster_Think (edict_t *ent)
{
	if (!ent->Monster)
		return;
	ent->nextthink = level.time + .1;

	CMonster *Monster = ent->Monster;
	float	thinktime;

	thinktime = Monster->NextThink;
	if (thinktime <= 0)
		return;
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
		delta = Entity->s.angles[YAW] - Entity->ideal_yaw;
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

	if ((Entity->spawnflags & 4) && !(Entity->monsterinfo.aiflags & AI_GOOD_GUY))
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

	Entity->s.skinNum = 0;
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
	if (!(activator->client) && !(activator->monsterinfo.aiflags & AI_GOOD_GUY))
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
		if (AIFlags & AI_HAS_MELEE)
			AttackState = AS_MELEE;
		else
			AttackState = AS_MISSILE;
		return true;
	}
	
// missile attack
	if (!(AIFlags & AI_HAS_ATTACK))
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
	bool		hesDeadJim;

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

// look for other coop players here
//	if (coop && self->monsterinfo.search_time < level.time)
//	{
//		if (FindTarget (self))
//			return true;
//	}

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

	return CheckAttack ();
}