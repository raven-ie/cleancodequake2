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
#include "cc_brushmodels.h"
#define STEPSIZE	18

#ifdef MONSTERS_USE_PATHFINDING

bool VecInFront (vec3_t angles, vec3_t origin1, vec3_t origin2);
int rangevector (vec3_t self, vec3_t other);

void CMonster::FoundPath ()
{
	if (!P_CurrentGoalNode || !P_CurrentNode)
	{
		P_NodePathTimeout = level.framenum + 100; // in 10 seconds we will try again.
		return;
	}
	// Just in case...
	if (P_CurrentGoalNode == P_CurrentNode)
		return;

	P_CurrentPath = GetPath(P_CurrentNode, P_CurrentGoalNode);

	if (!P_CurrentPath)
	{
		P_CurrentNode = P_CurrentGoalNode = NULL;
		P_CurrentNodeIndex = -1;
		P_NodePathTimeout = level.framenum + 100; // in 10 seconds we will try again.
		return;
	}

	P_CurrentNodeIndex = (int32)P_CurrentPath->Path.size()-1;

	// If our first node is behind us and it's not too far away, we can
	// just skip this node and go to the next one.	
	vec3_t angles, origin;
	Entity->State.GetAngles (angles);
	Entity->State.GetOrigin (origin);

	// Revision: Only do this if we have > 2 nodes (it messes up if we have exactly 2)
	if (VecInFront(angles, origin, P_CurrentPath->Path[P_CurrentNodeIndex]->Origin) && P_CurrentPath->Path.size() > 2)
		P_CurrentNodeIndex--;

	P_CurrentNode = P_CurrentPath->Path[P_CurrentNodeIndex];

	float timeOut = level.framenum + 20; // Always at least 2 seconds
	// Calculate approximate distance and check how long we want this to time out for
	switch (rangevector(origin, P_CurrentNode->Origin))
	{
	case RANGE_MELEE:
		timeOut += 60; // 10 seconds max
		break;
	case RANGE_NEAR:
		timeOut += 230; // 25 seconds
		break;
	case RANGE_MID:
		timeOut += 160; // 18 seconds
		break;
	case RANGE_FAR:
		timeOut += 300; // 32 seconds
		break;
	}
	P_NodeFollowTimeout = timeOut;

	FollowingPath = true;
	Run ();
}

void ForcePlatToGoUp (CBaseEntity *Entity);
void CMonster::MoveToPath (float Dist)
{
	if (!Entity->gameEntity->groundentity && !(Entity->gameEntity->flags & (FL_FLY|FL_SWIM)))
		return;

	if (FindTarget() && (Entity->gameEntity->enemy && visible(Entity->gameEntity, Entity->gameEntity->enemy))) // Did we find an enemy while going to our path?
	{
		FollowingPath = false;
		PauseTime = 100000000;

		P_CurrentPath = NULL;
		P_CurrentNode = P_CurrentGoalNode = NULL;
		return;
	}

	vec3_t sub;
	bool doit = false;
	// Check if we hit our new path.
	vec3_t origin;
	Entity->State.GetOrigin(origin);
	Vec3Subtract (origin, P_CurrentNode->Origin, sub);

	//CTempEnt_Trails::FleshCable (origin, P_CurrentNode->Origin, Entity->State.GetNumber());
	if (Vec3Length (sub) < 30)
	{
		bool shouldJump = (P_CurrentNode->Type == NODE_JUMP);
		// Hit the path.
		// If our node isn't the goal...
		if (P_CurrentNodeIndex > 0)
		{
			P_CurrentNodeIndex--; // Head to the next node.
			// Set our new path to the next node
			P_CurrentNode = P_CurrentPath->Path[P_CurrentNodeIndex];
			DebugPrintf ("Hit node %u\n", P_CurrentNodeIndex);
			doit = true;
			switch (P_CurrentNode->Type)
			{
			case NODE_DOOR:
					if (P_CurrentNode->LinkedEntity->use)
						P_CurrentNode->LinkedEntity->use (P_CurrentNode->LinkedEntity, Entity->gameEntity, Entity->gameEntity);
					Stand (); // We stand, and wait.
				break;
			case NODE_PLATFORM:
				{
					CPlatForm *Plat = dynamic_cast<CPlatForm*>(P_CurrentNode->LinkedEntity->Entity); // get the plat
					// If we reached the node, but the platform isn't down, go back two nodes
					if (Plat->MoveState != STATE_BOTTOM)
					{
						if (P_CurrentPath->Path.size() > (uint32)(P_CurrentNodeIndex + 2)) // Can we even go that far?
						{
							P_CurrentNodeIndex += 2;
							P_CurrentNode = P_CurrentPath->Path[P_CurrentNodeIndex];
							DebugPrintf ("Plat in bad spot: going back to %u\n", P_CurrentNodeIndex);
						}
					}
					else
					{
						Stand (); // We stand, and wait.
						if (Plat->MoveState == STATE_BOTTOM)
							Plat->GoUp ();
						else if (Plat->MoveState == STATE_TOP)
							Plat->NextThink = level.framenum + 10;	// the player is still on the plat, so delay going down
						//ForcePlatToGoUp (P_CurrentNode->LinkedEntity->Entity);
					}
				}

				break;
			default:
				break;
			};

			if (P_CurrentNodeIndex > 1) // If we have two more goals to destination
			{
				// In two goals, do we reach the platform node?
				if (P_CurrentPath->Path[P_CurrentNodeIndex-1]->Type == NODE_PLATFORM)
				{
					CPlatForm *Plat = dynamic_cast<CPlatForm*>(P_CurrentPath->Path[P_CurrentNodeIndex-1]->LinkedEntity->Entity); // get the plat
					// Is it at bottom?
					if (Plat->MoveState != STATE_BOTTOM)
						Stand (); // We wait till it comes down
				}
			}

			if (shouldJump)
			{
				vec3_t sub2, forward;
				Vec3Subtract (P_CurrentNode->Origin, origin, sub2);
				Angles_Vectors (sub2, forward, NULL, NULL);
				Vec3MA (Entity->gameEntity->velocity, 1.5, sub2, Entity->gameEntity->velocity);
				Entity->gameEntity->velocity[2] = 300;
				Entity->gameEntity->groundentity = NULL;
				CheckGround();
			}
		}
		else
		{
			if (Entity->gameEntity->enemy)
			{
				vec3_t sub;
				Vec3Subtract (origin, Entity->gameEntity->enemy->state.origin, sub);

				if (Vec3Length(sub) < 250) // If we're still close enough that it's possible
					// to hear him breathing (lol), start back on the trail
				{
					P_CurrentPath = NULL;
					P_CurrentGoalNode = GetClosestNodeTo(Entity->gameEntity->enemy->state.origin);
					FoundPath ();
					return;
				}
				return;
			}
			FollowingPath = false;
			PauseTime = 100000000;
			Stand ();
			P_CurrentPath = NULL;
			P_CurrentNode = P_CurrentGoalNode = NULL;
			return;
		}
	}

	if (P_NodeFollowTimeout < level.framenum && P_CurrentPath && P_CurrentNode)
	{
		// Re-evaluate start and end nodes
		CPathNode *End = P_CurrentPath->Path[0];
		P_CurrentNode = GetClosestNodeTo(origin);
		P_CurrentGoalNode = End;
		P_CurrentPath = NULL;
		FoundPath ();

		int32 timeOut = level.framenum + 20; // Always at least 2 seconds
		// Calculate approximate distance and check how long we want this to time out for
		switch (rangevector(origin, P_CurrentNode->Origin))
		{
		case RANGE_MELEE:
			timeOut += 60; // 10 seconds max
			break;
		case RANGE_NEAR:
			timeOut += 230; // 25 seconds
			break;
		case RANGE_MID:
			timeOut += 160; // 18 seconds
			break;
		case RANGE_FAR:
			timeOut += 300; // 32 seconds
			break;
		}
		P_NodeFollowTimeout = timeOut;
		return;
	}

// bump around...
	if ( doit || (rand()&3)==1 || !StepDirection (IdealYaw, Dist))
	{
		if (Entity->IsInUse())
		{
			float	deltax,deltay;
			vec3_t	d;
			float	tdir, olddir, turnaround;

			olddir = AngleModf ((int)(IdealYaw/45)*45);
			turnaround = AngleModf (olddir - 180);

			deltax = P_CurrentNode->Origin[0] - origin[0];
			deltay = P_CurrentNode->Origin[1] - origin[1];
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
				Entity->gameEntity->flags |= FL_PARTIALGROUND;
		}
	}
}
#endif

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
	int		start, check;

	if (level.sight_client == NULL)
		start = 1;
	else
		start = level.sight_client->State.GetNumber();

	check = start;
	while (1)
	{
		check++;
		if (check > game.maxclients)
			check = 1;
		CPlayerEntity *ent = dynamic_cast<CPlayerEntity*>(g_edicts[check].Entity);
		if (ent->IsInUse()
			&& ent->gameEntity->health > 0
			&& !(ent->gameEntity->flags & FL_NOTARGET) )
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

	Vec3Subtract (self->state.origin, other->state.origin, v);
	len = Vec3Length (v);
	if (len < MELEE_DISTANCE)
		return RANGE_MELEE;
	if (len < 500)
		return RANGE_NEAR;
	if (len < 1000)
		return RANGE_MID;
	return RANGE_FAR;
}

int rangevector (vec3_t self, vec3_t other)
{
	vec3_t	v;
	float	len;

	Vec3Subtract (self, other, v);
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

	Vec3Copy (self->state.origin, spot1);
	spot1[2] += self->viewheight;
	Vec3Copy (other->state.origin, spot2);
	spot2[2] += other->viewheight;
	trace = CTrace (spot1, spot2, self, CONTENTS_MASK_OPAQUE);
	
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
	
	Angles_Vectors (self->state.angles, forward, NULL, NULL);
	Vec3Subtract (other->state.origin, self->state.origin, vec);
	VectorNormalizef (vec, vec);
	dot = Dot3Product (vec, forward);
	
	if (dot > 0.3)
		return true;
	return false;
}

CMonsterEntity::CMonsterEntity () :
CBaseEntity(),
CMapEntity(),
CTossProjectile(),
CPushPhysics(),
CHurtableEntity(),
CThinkableEntity(),
CStepPhysics(),
CTouchableEntity()
{
	EntityFlags |= ENT_MONSTER;
	PhysicsType = PHYSICS_STEP;
};

CMonsterEntity::CMonsterEntity (int Index) :
CBaseEntity(Index),
CMapEntity(Index),
CTossProjectile(Index),
CPushPhysics(Index),
CHurtableEntity(Index),
CThinkableEntity(Index),
CStepPhysics(Index),
CTouchableEntity(Index)
{
	EntityFlags |= ENT_MONSTER;
	PhysicsType = PHYSICS_STEP;
};

void CMonsterEntity::Spawn ()
{
	PhysicsType = PHYSICS_STEP;
};

void CMonsterEntity::Think ()
{
	if (IsHead)
		Free ();
	else if (Monster->Think)
	{
		void	(CMonster::*TheThink) () = Monster->Think;
		(Monster->*TheThink) ();
	}
}

void CMonsterEntity::Die(CBaseEntity *inflictor, CBaseEntity *attacker, int damage, vec3_t point)
{
	Monster->Die (inflictor, attacker, damage, point);
}

void CMonsterEntity::Pain (CBaseEntity *other, float kick, int damage)
{
	Monster->Pain (other, kick, damage);
}

void CMonsterEntity::Touch (CBaseEntity *other, plane_t *plane, cmBspSurface_t *surf)
{
	Monster->Touch (other, plane, surf);
}

void VelocityForDamage (int damage, vec3f &v);
void CMonsterEntity::ThrowHead (MediaIndex gibIndex, int damage, int type)
{
	float	vscale;

	IsHead = true;
	State.SetSkinNum (0);
	State.SetFrame (0);

	SetMins (vec3fOrigin);
	SetMaxs (vec3fOrigin);

	State.SetModelIndex (0, 2);
	State.SetModelIndex (gibIndex);
	SetSolid (SOLID_NOT);
	State.AddEffects (EF_GIB);
	State.RemoveEffects (EF_FLIES);
	State.SetSound (0);
	gameEntity->flags |= FL_NO_KNOCKBACK;
	SetSvFlags (GetSvFlags() & ~SVF_MONSTER);
	gameEntity->takedamage = true;

	if (type == GIB_ORGANIC)
	{
		PhysicsType = PHYSICS_TOSS;
		vscale = 0.5;
	}
	else
	{
		PhysicsType = PHYSICS_BOUNCE;
		vscale = 1.0;
	}

	vec3f vd;
	VelocityForDamage (damage, vd);
	
	vec3f velocity (gameEntity->velocity);
	velocity.MultiplyAngles (vscale, vd);
	gameEntity->velocity[0] = velocity.X;
	gameEntity->velocity[1] = velocity.Y;
	gameEntity->velocity[2] = velocity.Z;

	if (gameEntity->velocity[0] < -300)
		gameEntity->velocity[0] = -300;
	else if (gameEntity->velocity[0] > 300)
		gameEntity->velocity[0] = 300;
	if (gameEntity->velocity[1] < -300)
		gameEntity->velocity[1] = -300;
	else if (gameEntity->velocity[1] > 300)
		gameEntity->velocity[1] = 300;
	if (gameEntity->velocity[2] < 200)
		gameEntity->velocity[2] = 200;	// always some upwards
	else if (gameEntity->velocity[2] > 500)
		gameEntity->velocity[2] = 500;

	gameEntity->avelocity[YAW] = crandom()*600;

	NextThink = level.framenum + 100 + random()*100;

	Link();
}

bool CMonsterEntity::Run ()
{
	switch (PhysicsType)
	{
	case PHYSICS_TOSS:
		return CTossProjectile::Run();
	case PHYSICS_BOUNCE:
		backOff = 1.5f;
		return CBounceProjectile::Run ();
	case PHYSICS_PUSH:
		return CPushPhysics::Run ();
	default:
		return CStepPhysics::Run ();
	};
}

void CMonster::ChangeYaw ()
{
	vec3f angles = Entity->State.GetAngles();
	float current = AngleModf (angles.Y);

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
	
	angles.Y = AngleModf(current+move);
	Entity->State.SetAngles(angles);
}

bool CMonster::CheckBottom ()
{
	vec3f	mins, maxs, start, stop;
	CTrace	trace;
	int		x, y;
	float	mid, bottom;
	
	mins = Entity->State.GetOrigin() + Entity->GetMins();
	maxs = Entity->State.GetOrigin() + Entity->GetMaxs();

// if all of the points under the corners are solid world, don't bother
// with the tougher checks
// the corners must be within 16 of the midpoint
	bool gotOutEasy = false;
	start[2] = mins[2] - 1;
	for	(x=0 ; x<=1 && !gotOutEasy; x++)
	{
		for	(y=0 ; y<=1 && !gotOutEasy; y++)
		{
			start.X = x ? maxs.X : mins.X;
			start.Y = y ? maxs.Y : mins.Y;

			vec3_t startVec = {start.X, start.Y, start.Z};
			if (PointContents (startVec) != CONTENTS_SOLID)
				gotOutEasy = true;
		}

	}

	if (gotOutEasy)
		return true;		// we got out easy

//
// check it for real...
//
	start.Z = mins.Z;
	
// the midpoint must be within 16 of the bottom
	start.X = stop.X = (mins.X + maxs.X)*0.5;
	start.Y = stop.Y = (mins.Y + maxs.Y)*0.5;
	stop.Z = start.Z - 2*STEPSIZE;
	trace = CTrace(start, stop, Entity->gameEntity, CONTENTS_MASK_MONSTERSOLID);

	if (trace.fraction == 1.0)
		return false;
	mid = bottom = trace.endPos[2];
	
// the corners must be within 16 of the midpoint	
	for	(x=0 ; x<=1 ; x++)
		for	(y=0 ; y<=1 ; y++)
		{
			start.X = stop.X = x ? maxs.X : mins.X;
			start.Y = stop.Y = y ? maxs.Y : mins.Y;
			
			trace = CTrace(start, stop, Entity->gameEntity, CONTENTS_MASK_MONSTERSOLID);
			
			if (trace.fraction != 1.0 && trace.endPos[2] > bottom)
				bottom = trace.endPos[2];
			if (trace.fraction == 1.0 || mid - trace.endPos[2] > STEPSIZE)
				return false;
		}

	return true;
}

void CMonster::MoveToGoal (float Dist)
{
	edict_t *goal = Entity->gameEntity->goalentity;

	if (!Entity->gameEntity->groundentity && !(Entity->gameEntity->flags & (FL_FLY|FL_SWIM)))
		return;

// if the next step hits the enemy, return immediately
	if (Entity->gameEntity->enemy && CloseEnough (Entity->gameEntity->enemy, Dist) )
		return;

// bump around...
	if ( (rand()&3)==1 || !StepDirection (IdealYaw, Dist))
	{
		if (Entity->IsInUse())
			NewChaseDir (goal, Dist);
	}
}

bool CMonster::WalkMove (float Yaw, float Dist)
{
	vec3_t	move;
	
	if (!Entity->gameEntity->groundentity && !(Entity->gameEntity->flags & (FL_FLY|FL_SWIM)))
		return false;

	Yaw = Yaw*M_PI*2 / 360;
	
	move[0] = cosf(Yaw)*Dist;
	move[1] = sinf(Yaw)*Dist;
	move[2] = 0;

	return MoveStep(move, true);
}

bool CMonster::MoveStep (vec3_t move, bool ReLink)
{
	vec3_t		oldorg, neworg, end;
	CTrace		trace;
	float		stepsize;

// try the move	
	Entity->State.GetOrigin(oldorg);
	Vec3Add (oldorg, move, neworg);

// flying monsters don't step up
	if ( Entity->gameEntity->flags & (FL_SWIM | FL_FLY) )
	{
	// try one move with vertical motion, then one without
		for (int i=0 ; i<2 ; i++)
		{
			Vec3Add (oldorg, move, neworg);
			if (i == 0 && Entity->gameEntity->enemy)
			{
				if (!Entity->gameEntity->goalentity)
					Entity->gameEntity->goalentity = Entity->gameEntity->enemy;
				float dz = oldorg[2] - Entity->gameEntity->goalentity->state.origin[2];
				if (Entity->gameEntity->goalentity->client)
				{
					if (dz > 40)
						neworg[2] -= 8;
					if (!((Entity->gameEntity->flags & FL_SWIM) && (Entity->gameEntity->waterlevel < 2)))
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
			trace = CTrace(vec3f(oldorg), Entity->GetMins(), Entity->GetMaxs(), vec3f(neworg), Entity->gameEntity, CONTENTS_MASK_MONSTERSOLID);
	
			// fly monsters don't enter water voluntarily
			if (Entity->gameEntity->flags & FL_FLY)
			{
				if (!Entity->gameEntity->waterlevel)
				{
					vec3f test (trace.EndPos);
					test.Z += Entity->GetMins().Z + 1;
					if (PointContents(test) & CONTENTS_MASK_WATER)
						return false;
				}
			}

			// swim monsters don't exit water voluntarily
			if (Entity->gameEntity->flags & FL_SWIM)
			{
				if (Entity->gameEntity->waterlevel < 2)
				{
					vec3f test (trace.EndPos);
					test.Z += Entity->GetMins().Z + 1;
					if (!(PointContents(test) & CONTENTS_MASK_WATER))
						return false;
				}
			}

			if (trace.fraction == 1)
			{
				Entity->State.SetOrigin (trace.endPos);
				if (ReLink)
				{
					Entity->Link ();
					G_TouchTriggers (Entity);
				}
				return true;
			}
			
			if (!Entity->gameEntity->enemy)
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

	trace = CTrace(vec3f(neworg), Entity->GetMins(), Entity->GetMaxs(), vec3f(end), Entity->gameEntity, CONTENTS_MASK_MONSTERSOLID);

	if (trace.allSolid)
		return false;

	if (trace.startSolid)
	{
		neworg[2] -= stepsize;
		trace = CTrace(vec3f(neworg), Entity->GetMins(), Entity->GetMaxs(), vec3f(end), Entity->gameEntity, CONTENTS_MASK_MONSTERSOLID);
		if (trace.allSolid || trace.startSolid)
			return false;
	}


	// don't go in to water
	/*if (Entity->gameEntity->waterlevel == 0)
	{
		test[0] = trace.endPos[0];
		test[1] = trace.endPos[1];
		test[2] = trace.endPos[2] + Entity->mins[2] + 1;	
		contents = PointContents(test);

		if (contents & CONTENTS_MASK_WATER)
			return false;
	}*/

	if (trace.fraction == 1)
	{
	// if monster had the ground pulled out, go ahead and fall
		if ( Entity->gameEntity->flags & FL_PARTIALGROUND )
		{
			vec3_t or;
			Entity->State.GetOrigin(or);
			Vec3Add (or, move, or);
			Entity->State.SetOrigin (or);
			if (ReLink)
			{
				Entity->Link ();
				G_TouchTriggers (Entity);
			}
			Entity->gameEntity->groundentity = NULL;
			return true;
		}
	
		return false;		// walked off an edge
	}

// check point traces down for dangling corners
	Entity->State.SetOrigin(trace.endPos);
	
	if (!CheckBottom ())
	{
		if ( Entity->gameEntity->flags & FL_PARTIALGROUND )
		{	// entity had floor mostly pulled out from underneath it
			// and is trying to correct
			if (ReLink)
			{
				Entity->Link ();
				G_TouchTriggers (Entity);
			}
			return true;
		}
		vec3_t angles;

		Entity->State.GetAngles(angles);
		// Check to see if there's floor just after this
		vec3_t org, up, end2;
		Angles_Vectors (angles, NULL, NULL, up);

		vec3_t origin;
		Entity->State.GetOrigin (origin);
		Vec3Add (origin, move, org);

		// Go down
		Vec3MA (org, -STEPSIZE, up, end2);

		// Trace
		trace = CTrace (org, end2, Entity->gameEntity, CONTENTS_MASK_SOLID);

		// Couldn't make the move
		if (trace.fraction == 1.0)
		{
			Entity->State.SetOrigin (oldorg);
			return false;
		}
	}

	//if ( Entity->gameEntity->flags & FL_PARTIALGROUND )
	Entity->gameEntity->flags &= ~FL_PARTIALGROUND;

	Entity->gameEntity->groundentity = trace.ent;
	Entity->gameEntity->groundentity_linkcount = trace.ent->linkCount;

// the move is ok
	if (ReLink)
	{
		Entity->Link ();
		G_TouchTriggers (Entity);
	}
	return true;
}

bool CMonster::CloseEnough (edict_t *Goal, float Dist)
{
	for (int i=0 ; i<3 ; i++)
	{
		if (Goal->absMin[i] > Entity->GetAbsMax()[i] + Dist)
			return false;
		if (Goal->absMax[i] < Entity->GetAbsMin()[i] - Dist)
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

	deltax = Enemy->state.origin[0] - Entity->State.GetOrigin().X;
	deltay = Enemy->state.origin[1] - Entity->State.GetOrigin().Y;
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
		Entity->gameEntity->flags |= FL_PARTIALGROUND;
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

	Entity->State.GetOrigin (oldorigin);
	if (MoveStep (move, false))
	{
		delta = Entity->State.GetAngles().Y - IdealYaw;
		// not turned far enough, so don't take the step
		if (delta > 45 && delta < 315)
			Entity->State.SetOrigin (oldorigin);
		Entity->Link ();
		G_TouchTriggers (Entity);
		return true;
	}
	Entity->Link ();
	G_TouchTriggers (Entity);
	return false;
}

void CMonster::WalkMonsterStartGo ()
{
	if (!(Entity->gameEntity->spawnflags & 2) && level.framenum < 10)
	{
		DropToFloor ();

		if (Entity->gameEntity->groundentity)
		{
			if (!WalkMove (0, 0))
				MapPrint (MAPPRINT_WARNING, Entity, Entity->State.GetOrigin(), "In solid\n");
		}
	}
	
	if (!YawSpeed)
		YawSpeed = 20;
	Entity->gameEntity->viewheight = 25;

	MonsterStartGo ();

	if (Entity->gameEntity->spawnflags & 2)
		MonsterTriggeredStart ();
}

void CMonster::WalkMonsterStart ()
{
	Think = &CMonster::WalkMonsterStartGo;
	Entity->NextThink = level.framenum + FRAMETIME;
	MonsterStart ();
}

void CMonster::SwimMonsterStartGo ()
{
	if (!YawSpeed)
		YawSpeed = 10;
	Entity->gameEntity->viewheight = 10;

	MonsterStartGo ();

	if (Entity->gameEntity->spawnflags & 2)
		MonsterTriggeredStart ();
}

void CMonster::SwimMonsterStart ()
{
	Entity->gameEntity->flags |= FL_SWIM;
	Think = &CMonster::SwimMonsterStartGo;
	MonsterStart ();
}

void CMonster::FlyMonsterStartGo ()
{
	if (!WalkMove (0, 0))
		MapPrint (MAPPRINT_WARNING, Entity, Entity->State.GetOrigin(), "Entity in solid\n");

	if (!YawSpeed)
		YawSpeed = 10;
	Entity->gameEntity->viewheight = 25;

	MonsterStartGo ();

	if (Entity->gameEntity->spawnflags & 2)
		MonsterTriggeredStart ();
}

void CMonster::FlyMonsterStart ()
{
	Entity->gameEntity->flags |= FL_FLY;
	Think = &CMonster::FlyMonsterStartGo;
	MonsterStart ();
}

void CMonster::MonsterStartGo ()
{
	if (Entity->gameEntity->health <= 0)
		return;

	// check for target to combat_point and change to combattarget
	if (Entity->gameEntity->target)
	{
		bool		notcombat = false, fixup = false;
		CBaseEntity		*target = NULL;

		while ((target = CC_Find (target, FOFS(targetname), Entity->gameEntity->target)) != NULL)
		{
			if (strcmp(target->gameEntity->classname, "point_combat") == 0)
			{
				Entity->gameEntity->combattarget = Entity->gameEntity->target;
				fixup = true;
			}
			else
				notcombat = true;
		}
		if (notcombat && Entity->gameEntity->combattarget)
			MapPrint (MAPPRINT_WARNING, Entity, Entity->State.GetOrigin(), "Target with mixed types\n");
			//gi.dprintf("%s at (%f %f %f) has target with mixed types\n", self->classname, self->state.origin[0], self->state.origin[1], self->state.origin[2]);
		if (fixup)
			Entity->gameEntity->target = NULL;
	}

	// validate combattarget
	if (Entity->gameEntity->combattarget)
	{
		CBaseEntity		*target = NULL;
		while ((target = CC_Find (target, FOFS(targetname), Entity->gameEntity->combattarget)) != NULL)
		{
			if (strcmp(target->gameEntity->classname, "point_combat") != 0)
				MapPrint (MAPPRINT_WARNING, Entity, Entity->State.GetOrigin(), "Has a bad combattarget (\"%s\")\n", Entity->gameEntity->combattarget);
		}
	}

	if (Entity->gameEntity->target)
	{
		CBaseEntity *Target = CC_PickTarget(Entity->gameEntity->target);
		Entity->gameEntity->goalentity = Entity->gameEntity->movetarget = Target->gameEntity;
		if (!Entity->gameEntity->movetarget)
		{
			//gi.dprintf ("%s can't find target %s at (%f %f %f)\n", self->classname, self->target, self->state.origin[0], self->state.origin[1], self->state.origin[2]);
			MapPrint (MAPPRINT_WARNING, Entity, Entity->State.GetOrigin(), "Can't find target\n");
			Entity->gameEntity->target = NULL;
			PauseTime = 100000000;
			Stand ();
		}
		else if (strcmp (Entity->gameEntity->movetarget->classname, "path_corner") == 0)
		{
			vec3_t origin, v;

			Entity->State.GetOrigin(origin);
			Vec3Subtract (Entity->gameEntity->goalentity->state.origin, origin, v);
			vec3f angles = Entity->State.GetAngles();
			IdealYaw = angles.Y = VecToYaw(v);
			Entity->State.SetAngles(angles);
			Walk ();
			Entity->gameEntity->target = NULL;
		}
		else
		{
			Entity->gameEntity->goalentity = Entity->gameEntity->movetarget = NULL;
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
	Entity->NextThink = level.framenum + FRAMETIME;
}

void CMonster::MonsterStart ()
{
	if (game.mode & GAME_DEATHMATCH)
	{
		Entity->Free ();
		return;
	}

	if ((Entity->gameEntity->spawnflags & 4) && !(AIFlags & AI_GOOD_GUY))
	{
		Entity->gameEntity->spawnflags &= ~4;
		Entity->gameEntity->spawnflags |= 1;
	}

	if (!(AIFlags & AI_GOOD_GUY))
		level.total_monsters++;

	Entity->NextThink = level.framenum + FRAMETIME;
	Entity->SetSvFlags (Entity->GetSvFlags() | SVF_MONSTER);
	Entity->State.AddRenderEffects (RF_FRAMELERP);
	Entity->gameEntity->takedamage = true;
	Entity->gameEntity->air_finished = level.framenum + 120;
	Entity->gameEntity->use = &CMonster::MonsterUse;
	Entity->gameEntity->max_health = Entity->gameEntity->health;
	Entity->gameEntity->clipMask = CONTENTS_MASK_MONSTERSOLID;

	Entity->gameEntity->deadflag = DEAD_NO;
	Entity->SetSvFlags (Entity->GetSvFlags() & ~SVF_DEADMONSTER);

	Entity->State.SetOldOrigin(Entity->State.GetOrigin());

	if (st.item)
	{
		Entity->gameEntity->item = FindItemByClassname (st.item);
		if (!Entity->gameEntity->item)
			MapPrint (MAPPRINT_WARNING, Entity, Entity->State.GetOrigin(), "Bad item: \"%s\"\n", st.item);
			//gi.dprintf("%s at (%f %f %f) has bad item: %s\n", self->classname, self->state.origin[0], self->state.origin[1], self->state.origin[2], st.item);
	}

	// randomize what frame they start on
	if (CurrentMove)
		Entity->State.SetFrame(CurrentMove->FirstFrame + (rand() % (CurrentMove->LastFrame - CurrentMove->FirstFrame + 1)));

#ifdef MONSTER_USE_ROGUE_AI
	BaseHeight = Entity->GetMaxs().Z;
#endif

	Entity->NextThink = level.framenum + FRAMETIME;
}

void CMonster::MonsterTriggeredStart ()
{
	Entity->SetSolid (SOLID_NOT);
	Entity->PhysicsDisabled = true;
	Entity->SetSvFlags (Entity->GetSvFlags() | SVF_NOCLIENT);
	Entity->NextThink = 0;
	Think = NULL;
	Entity->gameEntity->use = &CMonster::MonsterTriggeredSpawnUse;
}

void _cdecl CMonster::MonsterUse (edict_t *self, edict_t *other, edict_t *activator)
{
	if (self->enemy || !(self->Entity && (self->Entity->EntityFlags & ENT_MONSTER)))
		return;
	if (self->health <= 0)
		return;
	if (activator->flags & FL_NOTARGET)
		return;
	if (!(activator->client) && !((dynamic_cast<CMonsterEntity*>(self->Entity))->Monster->AIFlags & AI_GOOD_GUY))
		return;
	
// delay reaction so if the monster is teleported, its sound is still heard
	self->enemy = activator;
	(dynamic_cast<CMonsterEntity*>(self->Entity))->Monster->FoundTarget ();
}

void _cdecl CMonster::MonsterTriggeredSpawnUse (edict_t *self, edict_t *other, edict_t *activator)
{
	if (!(self->Entity && (self->Entity->EntityFlags & ENT_MONSTER)))
		return;

	// we have a one frame delay here so we don't telefrag the guy who activated us
	(dynamic_cast<CMonsterEntity*>(self->Entity))->Monster->Think = &CMonster::MonsterTriggeredSpawn;
	(dynamic_cast<CMonsterEntity*>(self->Entity))->Monster->Entity->NextThink = level.framenum + FRAMETIME;
	if (activator->client)
		self->enemy = activator;
	self->use = &CMonster::MonsterUse;
}

void CMonster::MonsterTriggeredSpawn ()
{
	vec3f newOrigin = Entity->State.GetOrigin();
	newOrigin.Z += 1;
	Entity->State.SetOrigin (newOrigin);
	KillBox (Entity);

	Entity->SetSolid (SOLID_BBOX);
	Entity->PhysicsDisabled = false;
	Entity->SetSvFlags (Entity->GetSvFlags() & ~SVF_NOCLIENT);
	Entity->gameEntity->air_finished = level.framenum + 120;
	Entity->Link ();

	MonsterStartGo ();

	if (Entity->gameEntity->enemy && !(Entity->gameEntity->spawnflags & 1) && !(Entity->gameEntity->enemy->flags & FL_NOTARGET))
		FoundTarget ();
	else
		Entity->gameEntity->enemy = NULL;
}

#ifdef MONSTERS_ARENT_STUPID
bool CMonster::FriendlyInLine (vec3_t Origin, vec3_t Direction)
{
	vec3_t dir, forward, end;
	VecToAngles (Direction, dir);
	Angles_Vectors (dir, forward, NULL, NULL);

	Vec3MA (Origin, 8192, forward, end);
	CTrace trace = CTrace(Origin, end, Entity->gameEntity, CONTENTS_MONSTER);

	if (trace.fraction <= 0.5 && trace.ent && (trace.ent->Entity && (trace.ent->Entity->EntityFlags & ENT_MONSTER)) && (trace.ent->enemy != Entity->gameEntity))
		return true;
	return false;
}
#endif

#ifdef MONSTERS_ARENT_STUPID
/*
=================
AlertNearbyStroggs

Alerts nearby Stroggs of possible enemy targets
=================
*/

void CMonster::AlertNearbyStroggs ()
{
	float dist;
	edict_t		*strogg = NULL;

	if (Entity->gameEntity->enemy->flags & FL_NOTARGET)
		return;

	switch (skill->Integer())
	{
	case 0:
		return;
	case 1:
		dist = 300;
		break;
	case 2:
		dist = 500;
		break;
	default:
		dist = 750 + (skill->Integer()) * 75;
		break;
	}

	if (dist > 2400)
		dist = 2400;

	vec3_t origin;
	Entity->State.GetOrigin (origin);
	while ( (strogg = findradius (strogg, origin, dist)) != NULL)
	{
		if (strogg->health < 1 || !(strogg->takedamage))
			continue;

		if (strogg == Entity->gameEntity)
			continue;

		if (strogg->client || !(strogg->Entity && (strogg->Entity->EntityFlags & ENT_MONSTER)))
			continue;

		if (strogg->enemy)
			continue;
		
#ifdef MONSTERS_USE_PATHFINDING
		// Set us up for pathing
		// Revision: if we aren't visible, that is.
		CMonsterEntity *FoundStrogg = dynamic_cast<CMonsterEntity*>(strogg->Entity); 
		if (!visible(strogg, Entity->gameEntity->enemy))
		{
			FoundStrogg->Monster->P_CurrentNode = GetClosestNodeTo(strogg->state.origin);
			FoundStrogg->Monster->P_CurrentGoalNode = GetClosestNodeTo(Entity->gameEntity->enemy->state.origin);
			//FoundStrogg->gameEntity->enemy = Entity->gameEntity->enemy;
			FoundStrogg->Monster->FoundPath ();
		}
		else
		{
			FoundStrogg->gameEntity->enemy = Entity->gameEntity->enemy;
			FoundStrogg->Monster->FoundTarget ();
			FoundStrogg->Monster->Sight ();
		}
#else
		//strogg->enemy = Entity->gameEntity->enemy;
		//strogg->Monster->FoundTarget ();
#endif
	}
}
#endif

void CMonster::MonsterFireBullet (vec3_t start, vec3_t dir, int damage, int kick, int hspread, int vspread, int flashtype)
{
#ifdef MONSTERS_ARENT_STUPID
	if (FriendlyInLine (start, dir))
		return;
#endif

	//fire_bullet (Entity, start, dir, damage, kick, hspread, vspread, MOD_UNKNOWN);
	CBullet::Fire (Entity, start, dir, damage, kick, hspread, vspread, MOD_UNKNOWN);

	if (flashtype != -1)
		CTempEnt::MonsterFlash (start, Entity->State.GetNumber(), flashtype);
}

void CMonster::MonsterFireShotgun (vec3_t start, vec3_t aimdir, int damage, int kick, int hspread, int vspread, int count, int flashtype)
{
#ifdef MONSTERS_ARENT_STUPID
	if (FriendlyInLine (start, aimdir))
		return;
#endif

	//fire_shotgun (Entity, start, aimdir, damage, kick, hspread, vspread, count, MOD_UNKNOWN);
	CShotgunPellets::Fire (Entity, start, aimdir, damage, kick, hspread, vspread, count, MOD_UNKNOWN);

	if (flashtype != -1)
		CTempEnt::MonsterFlash (start, Entity->State.GetNumber(), flashtype);
}

void CMonster::MonsterFireBlaster (vec3_t start, vec3_t dir, int damage, int speed, int flashtype, int effect)
{
#ifdef MONSTERS_ARENT_STUPID
	if (FriendlyInLine (start, dir))
		return;
#endif

	//fire_blaster (Entity, start, dir, damage, speed, effect, false);
	CBlasterProjectile::Spawn (Entity, vec3f(start), vec3f(dir), damage, speed, effect, false);

	if (flashtype != -1)
		CTempEnt::MonsterFlash (start, Entity->State.GetNumber(), flashtype);
}	

void CMonster::MonsterFireGrenade (vec3_t start, vec3_t aimdir, int damage, int speed, int flashtype)
{
#ifdef MONSTERS_ARENT_STUPID
	if (FriendlyInLine (start, aimdir))
		return;
#endif

	//fire_grenade (Entity, start, aimdir, damage, speed, 2.5, damage+40);
	CGrenade::Spawn (Entity, start, aimdir, damage, speed, 2.5, damage+40);

	if (flashtype != -1)
		CTempEnt::MonsterFlash (start, Entity->State.GetNumber(), flashtype);
}

void CMonster::MonsterFireRocket (vec3_t start, vec3_t dir, int damage, int speed, int flashtype)
{
#ifdef MONSTERS_ARENT_STUPID
	if (FriendlyInLine (start, dir))
		return;
#endif

	//fire_rocket (Entity, start, dir, damage, speed, damage+20, damage);
	CRocket::Spawn (Entity, start, dir, damage, speed, damage+20, damage);

	if (flashtype != -1)
		CTempEnt::MonsterFlash (start, Entity->State.GetNumber(), flashtype);
}	

void CMonster::MonsterFireRailgun (vec3_t start, vec3_t aimdir, int damage, int kick, int flashtype)
{
#ifdef MONSTERS_ARENT_STUPID
	if (FriendlyInLine (start, aimdir))
		return;
#endif

	if (!(PointContents (start) & CONTENTS_MASK_SOLID))
		//fire_rail (Entity, start, aimdir, damage, kick);
		CRailGunShot::Fire (Entity, start, aimdir, damage, kick);

	if (flashtype != -1)
		CTempEnt::MonsterFlash (start, Entity->State.GetNumber(), flashtype);
}

void CMonster::MonsterFireBfg (vec3_t start, vec3_t aimdir, int damage, int speed, int kick, float damage_radius, int flashtype)
{
#ifdef MONSTERS_ARENT_STUPID
	if (FriendlyInLine (start, aimdir))
		return;
#endif

	//fire_bfg (Entity, start, aimdir, damage, speed, damage_radius);
	CBFGBolt::Spawn (Entity, start, aimdir, damage, speed, damage_radius);

	if (flashtype != -1)
		CTempEnt::MonsterFlash (start, Entity->State.GetNumber(), flashtype);
}

bool CMonster::CheckAttack ()
{
#ifndef MONSTER_USE_ROGUE_AI
	vec3_t	spot1, spot2;
	float	chance;
	CTrace	tr;

	if (Entity->gameEntity->enemy->health > 0)
	{
	// see if any entities are in the way of the shot
		Entity->State.GetOrigin(spot1);
		spot1[2] += Entity->gameEntity->viewheight;
		Vec3Copy (Entity->gameEntity->enemy->state.origin, spot2);
		spot2[2] += Entity->gameEntity->enemy->viewheight;

		tr = CTrace(spot1, spot2, Entity->gameEntity, CONTENTS_SOLID|CONTENTS_MONSTER|CONTENTS_SLIME|CONTENTS_LAVA|CONTENTS_WINDOW);

		// do we have a clear shot?
		if (tr.ent != Entity->gameEntity->enemy)
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
		
	if (level.framenum < AttackFinished)
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
		AttackFinished = level.framenum + ((2*random())*10);
		return true;
	}

	if (Entity->gameEntity->flags & FL_FLY)
	{
		if (random() < 0.3)
			AttackState = AS_SLIDING;
		else
			AttackState = AS_STRAIGHT;
	}

	return false;
#else
	float	chance;

	if (Entity->gameEntity->enemy->health > 0)
	{
	// see if any entities are in the way of the shot
		vec3_t	spot1, spot2;
		Entity->State.GetOrigin (spot1);
		spot1[2] += Entity->gameEntity->viewheight;
		Vec3Copy (Entity->gameEntity->enemy->state.origin, spot2);
		spot2[2] += Entity->gameEntity->enemy->viewheight;

		CTrace tr (spot1, spot2, Entity->gameEntity, CONTENTS_SOLID|CONTENTS_MONSTER|CONTENTS_SLIME|CONTENTS_LAVA|CONTENTS_WINDOW);

		// do we have a clear shot?
		if (tr.ent != Entity->gameEntity->enemy)
		{	
			// PGM - we want them to go ahead and shoot at info_notnulls if they can.
			if(Entity->gameEntity->enemy->solid != SOLID_NOT || tr.fraction < 1.0)		//PGM
			{
				// PMM - if we can't see our target, and we're not blocked by a monster, go into blind fire if available
				if ((!(tr.ent->svFlags & SVF_MONSTER)) && (!visible(Entity->gameEntity, Entity->gameEntity->enemy)))
				{
					if ((BlindFire) && (BlindFireDelay <= 20.0))
					{
						if (level.framenum < AttackFinished)
							return false;
						if (level.framenum < (TrailTime + BlindFireDelay))
							// wait for our time
							return false;
						else
						{
							// make sure we're not going to shoot a monster
							tr = CTrace (spot1, BlindFireTarget, Entity->gameEntity, CONTENTS_MONSTER);
							if (tr.allSolid || tr.startSolid || ((tr.fraction < 1.0) && (tr.ent != Entity->gameEntity->enemy)))
								return false;

							AttackState = AS_BLIND;
							return true;
						}
					}
				}
				// pmm
				return false;
			}
		}
	}
	
	// melee attack
	if (EnemyRange == RANGE_MELEE)
	{
		// don't always melee in easy mode
		if (skill->Integer() == 0 && (rand()&3) )
		{
			// PMM - fix for melee only monsters & strafing
			AttackState = AS_STRAIGHT;
			return false;
		}
		if (MonsterFlags & MF_HAS_MELEE)
			AttackState = AS_MELEE;
		else
			AttackState = AS_MISSILE;
		return true;
	}
	
// missile attack
	if (!(MonsterFlags & MF_HAS_ATTACK))
	{
		// PMM - fix for melee only monsters & strafing
		AttackState = AS_STRAIGHT;
		return false;
	}
	
	if (level.framenum < AttackFinished)
		return false;
		
	if (EnemyRange == RANGE_FAR)
		return false;

	if (AIFlags & AI_STAND_GROUND)
		chance = 0.4f;
	else if (EnemyRange == RANGE_MELEE)
		chance = 0.2f;
	else if (EnemyRange == RANGE_NEAR)
		chance = 0.1f;
	else if (EnemyRange == RANGE_MID)
		chance = 0.02f;
	else
		return false;

	if (skill->Integer() == 0)
		chance *= 0.5;
	else if (skill->Integer() >= 2)
		chance *= 2;

	// PGM - go ahead and shoot every time if it's a info_notnull
	if ((random () < chance) || (Entity->gameEntity->enemy->solid == SOLID_NOT))
	{
		AttackState = AS_MISSILE;
		AttackFinished = level.framenum + ((2*random())*10);
		return true;
	}

	// PMM -daedalus should strafe more .. this can be done here or in a customized
	// check_attack code for the hover.
	if (Entity->gameEntity->flags & FL_FLY)
	{
		// originally, just 0.3
		float strafe_chance;
		if (!(strcmp(Entity->gameEntity->classname, "monster_daedalus")))
			strafe_chance = 0.8f;
		else
			strafe_chance = 0.6f;

		// if enemy is tesla, never strafe
		if ((Entity->gameEntity->enemy) && (Entity->gameEntity->enemy->classname) && (!strcmp(Entity->gameEntity->enemy->classname, "tesla")))
			strafe_chance = 0;

		if (random() < strafe_chance)
			AttackState = AS_SLIDING;
		else
			AttackState = AS_STRAIGHT;
	}
// do we want the monsters strafing?
	else
	{
		if (random() < 0.4)
			AttackState = AS_SLIDING;
		else
			AttackState = AS_STRAIGHT;
	}

//-PMM

	return false;
#endif
}

void CMonster::DropToFloor ()
{
	vec3_t		end;
	CTrace		trace;
	vec3_t		origin;

	Entity->State.GetOrigin(origin);
	origin[2] += 1;
	Entity->State.SetOrigin(origin);
	Vec3Copy (origin, end);
	end[2] -= 256;
	
	trace = CTrace (Entity->State.GetOrigin(), Entity->GetMins(), Entity->GetMaxs(), vec3f(end), Entity->gameEntity, CONTENTS_MASK_MONSTERSOLID);

	if (trace.fraction == 1 || trace.allSolid)
		return;

	Entity->State.SetOrigin (trace.endPos);

	Entity->Link ();
	CheckGround ();
	CatagorizePosition ();
}

void CMonster::AI_Charge(float Dist)
{
#ifndef MONSTER_USE_ROGUE_AI
	vec3_t	v;

	vec3_t origin;
	Entity->State.GetOrigin (origin);
	Vec3Subtract (Entity->gameEntity->enemy->state.origin, origin, v);
	IdealYaw = VecToYaw(v);
	ChangeYaw ();

	if (Dist)
		WalkMove (Entity->State.GetAngles().Y, Dist);
#else
	// PMM - made AI_MANUAL_STEERING affect things differently here .. they turn, but
	// don't set the ideal_yaw

	// This is put in there so monsters won't move towards the origin after killing
	// a tesla. This could be problematic, so keep an eye on it.
	if(!Entity->gameEntity->enemy || !Entity->gameEntity->enemy->inUse)		//PGM
		return;									//PGM

	// PMM - save blindfire target
	if (visible(Entity->gameEntity, Entity->gameEntity->enemy))
		Vec3Copy (Entity->gameEntity->enemy->state.origin, BlindFireTarget);
	// pmm 

	if (!(AIFlags & AI_MANUAL_STEERING))
	{
		vec3_t origin, v;
		Entity->State.GetOrigin (origin);
		Vec3Subtract (Entity->gameEntity->enemy->state.origin, origin, v);
		IdealYaw = VecToYaw(v);
	}
	ChangeYaw ();

	if (Dist)
	{
		if (AIFlags & AI_CHARGING)
		{
			MoveToGoal (Dist);
			return;
		}
		// circle strafe support
		if (AttackState == AS_SLIDING)
		{
			float ofs;
			// if we're fighting a tesla, NEVER circle strafe
			if ((Entity->gameEntity->enemy) && (Entity->gameEntity->enemy->classname) && (!strcmp(Entity->gameEntity->enemy->classname, "tesla")))
				ofs = 0;
			else if (Lefty)
				ofs = 90;
			else
				ofs = -90;
			
			if (WalkMove (IdealYaw + ofs, Dist))
				return;
				
			Lefty = !Lefty;
			WalkMove (IdealYaw - ofs, Dist);
		}
		else
			WalkMove (Entity->State.GetAngles().Y, Dist);
	}
// PMM
#endif
}

bool CMonster::AI_CheckAttack()
{
#ifndef MONSTER_USE_ROGUE_AI
	vec3_t		temp;
	bool		hesDeadJim = false;

// this causes monsters to run blindly to the combat point w/o firing
	if (Entity->gameEntity->goalentity)
	{
		if (AIFlags & AI_COMBAT_POINT)
			return false;

		if (AIFlags & AI_SOUND_TARGET)
		{
			if (Entity->gameEntity->enemy)
			{
				if ((level.framenum - Entity->gameEntity->enemy->teleport_time) > 50)
				{
					if (Entity->gameEntity->goalentity == Entity->gameEntity->enemy)
						if (Entity->gameEntity->movetarget)
							Entity->gameEntity->goalentity = Entity->gameEntity->movetarget;
						else
							Entity->gameEntity->goalentity = NULL;
					AIFlags &= ~AI_SOUND_TARGET;
					if (AIFlags & AI_TEMP_STAND_GROUND)
						AIFlags &= ~(AI_STAND_GROUND | AI_TEMP_STAND_GROUND);
				}
				else
				{
					Entity->gameEntity->show_hostile = level.framenum + 10;
					return false;
				}
			}
		}
	}

// see if the enemy is dead
	if ((!Entity->gameEntity->enemy) || (!Entity->gameEntity->enemy->inUse))
		hesDeadJim = true;
	else if (AIFlags & AI_MEDIC)
	{
		if (Entity->gameEntity->enemy->health > 0)
		{
			hesDeadJim = true;
			AIFlags &= ~AI_MEDIC;
		}
	}
	else
	{
		if (AIFlags & AI_BRUTAL)
		{
			if (Entity->gameEntity->enemy->health <= -80)
				hesDeadJim = true;
		}
		else
		{
			if (Entity->gameEntity->enemy->health <= 0)
				hesDeadJim = true;
		}
	}

	if (hesDeadJim)
	{
		Entity->gameEntity->enemy = NULL;
	// FIXME: look all around for other targets
		if (Entity->gameEntity->oldenemy && Entity->gameEntity->oldenemy->health > 0)
		{
			Entity->gameEntity->enemy = Entity->gameEntity->oldenemy;
			Entity->gameEntity->oldenemy = NULL;
			HuntTarget ();
		}
		else
		{
			if (Entity->gameEntity->movetarget)
			{
				Entity->gameEntity->goalentity = Entity->gameEntity->movetarget;
				Walk ();
			}
			else
			{
				// we need the pausetime otherwise the stand code
				// will just revert to walking with no target and
				// the monsters will wonder around aimlessly trying
				// to hunt the world entity
				PauseTime = level.framenum + 100000000;
				Stand ();
			}
			return true;
		}
	}

	Entity->gameEntity->show_hostile = level.framenum + 10;		// wake up other monsters

// check knowledge of enemy
	EnemyVis = visible(Entity->gameEntity, Entity->gameEntity->enemy);
	if (EnemyVis)
	{
		SearchTime = level.framenum + 50;
		Vec3Copy (Entity->gameEntity->enemy->state.origin, LastSighting);
	}

	EnemyInfront = infront(Entity->gameEntity, Entity->gameEntity->enemy);
	EnemyRange = range(Entity->gameEntity, Entity->gameEntity->enemy);
	vec3_t origin;
	Entity->State.GetOrigin (origin);
	Vec3Subtract (Entity->gameEntity->enemy->state.origin, origin, temp);
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
#else
	vec3_t		temp;
	bool	hesDeadJim;
	// PMM
	bool	retval;

// this causes monsters to run blindly to the combat point w/o firing
	if (Entity->gameEntity->goalentity)
	{
		if (AIFlags & AI_COMBAT_POINT)
			return false;

		if (AIFlags & AI_SOUND_TARGET)
		{
			if ((level.framenum - Entity->gameEntity->enemy->teleport_time) > 50)
			{
				if (Entity->gameEntity->goalentity == Entity->gameEntity->enemy)
				{
					if (Entity->gameEntity->movetarget)
						Entity->gameEntity->goalentity = Entity->gameEntity->movetarget;
					else
						Entity->gameEntity->goalentity = NULL;
				}

				AIFlags &= ~AI_SOUND_TARGET;
				if (AIFlags & AI_TEMP_STAND_GROUND)
					AIFlags &= ~(AI_STAND_GROUND | AI_TEMP_STAND_GROUND);
			}
			else
			{
				Entity->gameEntity->show_hostile = level.framenum + 10;
				return false;
			}
		}
	}

	EnemyVis = false;

// see if the enemy is dead
	hesDeadJim = false;
	if ((!Entity->gameEntity->enemy) || (!Entity->gameEntity->enemy->inUse))
	{
		hesDeadJim = true;
	}
	else if (AIFlags & AI_MEDIC)
	{
		if (!(Entity->gameEntity->enemy->inUse) || (Entity->gameEntity->enemy->health > 0))
		{
			hesDeadJim = true;
//			self->monsterinfo.aiflags &= ~AI_MEDIC;
		}
	}
	else
	{
		if (AIFlags & AI_BRUTAL)
		{
			if (Entity->gameEntity->enemy->health <= -80)
				hesDeadJim = true;
		}
		else
		{
			if (Entity->gameEntity->enemy->health <= 0)
				hesDeadJim = true;
		}
	}

	if (hesDeadJim)
	{
		AIFlags &= ~AI_MEDIC;
		Entity->gameEntity->enemy = NULL;
	// FIXME: look all around for other targets
		if (Entity->gameEntity->oldenemy && Entity->gameEntity->oldenemy->health > 0)
		{
			Entity->gameEntity->enemy = Entity->gameEntity->oldenemy;
			Entity->gameEntity->oldenemy = NULL;
			HuntTarget ();
		}
//ROGUE - multiple teslas make monsters lose track of the player.
		else if(LastPlayerEnemy && LastPlayerEnemy->health > 0)
		{
//			if ((g_showlogic) && (g_showlogic->value))
//				gi.dprintf("resorting to last_player_enemy...\n");
			Entity->gameEntity->enemy = LastPlayerEnemy;
			Entity->gameEntity->oldenemy = NULL;
			LastPlayerEnemy = NULL;
			HuntTarget ();
		}
//ROGUE
		else
		{
			if (Entity->gameEntity->movetarget)
			{
				Entity->gameEntity->goalentity = Entity->gameEntity->movetarget;
				Walk ();
			}
			else
			{
				// we need the pausetime otherwise the stand code
				// will just revert to walking with no target and
				// the monsters will wonder around aimlessly trying
				// to hunt the world entity
				PauseTime = level.framenum + 100000000;
				Stand ();
			}
			return true;
		}
	}

	Entity->gameEntity->show_hostile = level.framenum + 10;		// wake up other monsters

// check knowledge of enemy
	EnemyVis = visible(Entity->gameEntity, Entity->gameEntity->enemy);
	if (EnemyVis)
	{
		SearchTime = level.framenum + 50;
		Vec3Copy (Entity->gameEntity->enemy->state.origin, LastSighting);
		// PMM
		AIFlags &= ~AI_LOST_SIGHT;
		TrailTime = level.framenum;
		Vec3Copy (Entity->gameEntity->enemy->state.origin, BlindFireTarget);
		BlindFireDelay = 0;
		// pmm
	}

	EnemyInfront = infront(Entity->gameEntity, Entity->gameEntity->enemy);
	EnemyRange = range(Entity->gameEntity, Entity->gameEntity->enemy);

	vec3_t origin;
	Entity->State.GetOrigin(origin);
	Vec3Subtract (Entity->gameEntity->enemy->state.origin, origin, temp);
	EnemyYaw = VecToYaw(temp);

	// JDC self->ideal_yaw = enemy_yaw;

	// PMM -- reordered so the monster specific checkattack is called before the run_missle/melee/checkvis
	// stuff .. this allows for, among other things, circle strafing and attacking while in ai_run
	retval = CheckAttack ();
	if (retval)
	{
		// PMM
		if (AttackState == AS_MISSILE)
		{
			AI_Run_Missile ();
			return true;
		}
		if (AttackState == AS_MELEE)
		{
			AI_Run_Melee ();
			return true;
		}
		// PMM -- added so monsters can shoot blind
		if (AttackState == AS_BLIND)
		{
			AI_Run_Missile ();
			return true;
		}
		// pmm

		// if enemy is not currently visible, we will never attack
		if (!EnemyVis)
			return false;
		// PMM
	}
	return retval;
#endif
}

void CMonster::AI_Move (float Dist)
{
	WalkMove (Entity->State.GetAngles().Y, Dist);
}

void CMonster::AI_Run(float Dist)
{
#ifndef MONSTER_USE_ROGUE_AI
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

#ifdef MONSTERS_USE_PATHFINDING
	if (FollowingPath)
	{
		MoveToPath(Dist);
		return;
	}
#endif

	// if we're going to a combat point, just proceed
	if (AIFlags & AI_COMBAT_POINT)
	{
		MoveToGoal (Dist);
		return;
	}

	if (AIFlags & AI_SOUND_TARGET)
	{
		if (Entity->gameEntity->enemy)
		{
			vec3_t origin;
			Entity->State.GetOrigin(origin);
			Vec3Subtract (origin, Entity->gameEntity->enemy->state.origin, v);
			if (Vec3Length(v) < 64)
			{
				AIFlags |= (AI_STAND_GROUND | AI_TEMP_STAND_GROUND);
				Stand ();
				return;
			}

			MoveToGoal (Dist);
		}

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
		//if (AIFlags & AI_LOST_SIGHT)
			//gi.dprintf("regained sight\n");
		MoveToGoal (Dist);
		AIFlags &= ~AI_LOST_SIGHT;
		Vec3Copy (Entity->gameEntity->enemy->state.origin, LastSighting);
		TrailTime = level.framenum;
		return;
	}

	// coop will change to another enemy if visible
	if (game.mode == GAME_COOPERATIVE)
	{	// FIXME: insane guys get mad with this, which causes crashes!
		if (FindTarget ())
			return;
	}

	//if (SearchTime && (level.framenum > (SearchTime + 200)))
	//{
	//	MoveToGoal (Dist);
	//	SearchTime = 0;
//		dprint("search timeout\n");
	//	return;
	//}

	save = Entity->gameEntity->goalentity;
	tempgoal = G_Spawn();
	Entity->gameEntity->goalentity = tempgoal;

	isNew = false;

	if (!(AIFlags & AI_LOST_SIGHT))
	{
		// just lost sight of the player, decide where to go first
		//gi.dprintf("lost sight of player, last seen at %f %f %f\n", LastSighting[0], LastSighting[1], LastSighting[2]);

#ifdef MONSTERS_USE_PATHFINDING
		// Set us up for pathing
		vec3_t origin;
		Entity->State.GetOrigin(origin);
		P_CurrentNode = GetClosestNodeTo(origin);
		P_CurrentGoalNode = GetClosestNodeTo(Entity->gameEntity->enemy->state.origin);
		FoundPath ();
#endif

		AIFlags |= (AI_LOST_SIGHT | AI_PURSUIT_LAST_SEEN);
		AIFlags &= ~(AI_PURSUE_NEXT | AI_PURSUE_TEMP);
		isNew = true;
	}

	if (AIFlags & AI_PURSUE_NEXT)
	{
		AIFlags &= ~AI_PURSUE_NEXT;
//		dprint("reached current goal: "); dprint(vtos(self.origin)); dprint(" "); dprint(vtos(self.last_sighting)); dprint(" "); dprint(ftos(vlen(self.origin - self.last_sighting))); dprint("\n");

		// give ourself more time since we got this far
		SearchTime = level.framenum + 50;

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
			Vec3Copy (marker->state.origin, LastSighting);
			TrailTime = marker->timestamp;
			vec3_t angles;
			Entity->State.GetAngles(angles);
			angles[YAW] = IdealYaw = marker->state.angles[YAW];
			Entity->State.SetAngles(angles);
//			dprint("heading is "); dprint(ftos(self.ideal_yaw)); dprint("\n");

//			debug_drawline(self.origin, self.last_sighting, 52);
			isNew = true;
		}
	}

	vec3_t origin;
	Entity->State.GetOrigin();
	Vec3Subtract (origin, LastSighting, v);
	d1 = Vec3Length(v);
	if (d1 <= Dist)
	{
		AIFlags |= AI_PURSUE_NEXT;
		Dist = d1;
	}

	Vec3Copy (LastSighting, Entity->gameEntity->goalentity->state.origin);

	if (isNew)
	{
//		gi.dprintf("checking for course correction\n");

		tr = CTrace (Entity->State.GetOrigin(), Entity->GetMins(), Entity->GetMaxs(), vec3f(LastSighting), Entity->gameEntity, CONTENTS_MASK_PLAYERSOLID);
		if (tr.fraction < 1)
		{
			Vec3Subtract (Entity->gameEntity->goalentity->state.origin, origin, v);
			d1 = Vec3Length(v);
			center = tr.fraction;
			d2 = d1 * ((center+1)/2);
			vec3_t ang;
			Entity->State.GetAngles(ang);
			ang[YAW] = IdealYaw = VecToYaw(v);
			Entity->State.SetAngles(ang);

			Angles_Vectors(ang, v_forward, v_right, NULL);

			Vec3Set (v, d2, -16, 0);
			G_ProjectSource (origin, v, v_forward, v_right, left_target);
			tr = CTrace(Entity->State.GetOrigin(), Entity->GetMins(), Entity->GetMaxs(), vec3f(left_target), Entity->gameEntity, CONTENTS_MASK_PLAYERSOLID);
			left = tr.fraction;

			Vec3Set (v, d2, 16, 0);
			G_ProjectSource (origin, v, v_forward, v_right, right_target);
			tr = CTrace(Entity->State.GetOrigin(), Entity->GetMins(), Entity->GetMaxs(), vec3f(right_target), Entity->gameEntity, CONTENTS_MASK_PLAYERSOLID);
			right = tr.fraction;

			center = (d1*center)/d2;
			if (left >= center && left > right)
			{
				if (left < 1)
				{
					Vec3Set (v, d2 * left * 0.5, -16, 0);
					G_ProjectSource (origin, v, v_forward, v_right, left_target);
//					gi.dprintf("incomplete path, go part way and adjust again\n");
				}
				Vec3Copy (LastSighting, SavedGoal);
				AIFlags |= AI_PURSUE_TEMP;
				Vec3Copy (left_target, Entity->gameEntity->goalentity->state.origin);
				Vec3Copy (left_target, LastSighting);
				Vec3Subtract (Entity->gameEntity->goalentity->state.origin, origin, v);

				vec3_t ang;
				Entity->State.GetAngles(ang);
				ang[YAW] = IdealYaw = VecToYaw(v);
				Entity->State.SetAngles(ang);
//				gi.dprintf("adjusted left\n");
//				debug_drawline(self.origin, self.last_sighting, 152);
			}
			else if (right >= center && right > left)
			{
				if (right < 1) {
					Vec3Set (v, d2 * right * 0.5, 16, 0);
					G_ProjectSource (origin, v, v_forward, v_right, right_target);
//					gi.dprintf("incomplete path, go part way and adjust again\n");
				}
				Vec3Copy (LastSighting, SavedGoal);
				AIFlags |= AI_PURSUE_TEMP;
				Vec3Copy (right_target, Entity->gameEntity->goalentity->state.origin);
				Vec3Copy (right_target, LastSighting);
				Vec3Subtract (Entity->gameEntity->goalentity->state.origin, origin, v);
				vec3_t ang;
				Entity->State.GetAngles(ang);
				ang[YAW] = IdealYaw = VecToYaw(v);
				Entity->State.SetAngles(ang);
//				gi.dprintf("adjusted right\n");
//				debug_drawline(self.origin, self.last_sighting, 152);
			}
		}
//		else gi.dprintf("course was fine\n");
	}

	MoveToGoal (Dist);

	G_FreeEdict(tempgoal);

	if (Entity)
		Entity->gameEntity->goalentity = save;
#else
	vec3_t		v;
	edict_t		*tempgoal;
	edict_t		*save;
	bool	isNew;
	edict_t		*marker;
	//PMM
	bool	retval;
	bool	alreadyMoved = false;

 #ifdef MONSTERS_USE_PATHFINDING
	if (FollowingPath)
	{
		MoveToPath(Dist);
		return;
	}
#endif

	// if we're going to a combat point, just proceed
	if (AIFlags & AI_COMBAT_POINT)
	{
		MoveToGoal (Dist);
		return;
	}

	// PMM
	if (AIFlags & AI_DUCKED)
	{
//		if ((g_showlogic) && (g_showlogic->value))
//			gi.dprintf ("%s - duck flag cleaned up!\n", self->classname);
		AIFlags &= ~AI_DUCKED;
	}
	if (Entity->GetMaxs().Z != BaseHeight)
	{
//		if ((g_showlogic) && (g_showlogic->value))
//			gi.dprintf ("%s - ducked height corrected!\n", self->classname);
		UnDuck ();
	}
//	if ((self->monsterinfo.aiflags & AI_MANUAL_STEERING) && (strcmp(self->classname, "monster_turret")))
//	{
//		if ((g_showlogic) && (g_showlogic->value))
//			gi.dprintf ("%s - manual steering in ai_run!\n", self->classname);
//	}
	// pmm

	vec3_t origin;
	Entity->State.GetOrigin(origin);
	if (AIFlags & AI_SOUND_TARGET)
	{

		// PMM - paranoia checking
		if (Entity->gameEntity->enemy)
			Vec3Subtract (origin, Entity->gameEntity->enemy->state.origin, v);

		if ((!Entity->gameEntity->enemy) || (Vec3Length(v) < 64))
		// pmm
		{
			AIFlags |= (AI_STAND_GROUND | AI_TEMP_STAND_GROUND);
			Stand ();
			return;
		}

		MoveToGoal (Dist);
		// PMM - prevent double moves for sound_targets
		alreadyMoved = true;
		// pmm
		if(!Entity->IsInUse())
			return;			// PGM - g_touchtrigger free problem

		if (!FindTarget ())
			return;
	}

	// PMM -- moved ai_checkattack up here so the monsters can attack while strafing or charging
	// PMM -- if we're dodging, make sure to keep the attack_state AS_SLIDING
	retval = AI_CheckAttack ();

	// PMM - don't strafe if we can't see our enemy
	if ((!EnemyVis) && (AttackState == AS_SLIDING))
		AttackState = AS_STRAIGHT;

	// unless we're dodging (dodging out of view looks smart)
	if (AIFlags & AI_DODGING)
		AttackState = AS_SLIDING;
	// pmm

	if (AttackState == AS_SLIDING)
	{
		// PMM - protect against double moves
		if (!alreadyMoved)
			AI_Run_Slide (Dist);
		// PMM
		// we're using attack_state as the return value out of ai_run_slide to indicate whether or not the
		// move succeeded.  If the move succeeded, and we're still sliding, we're done in here (since we've
		// had our chance to shoot in ai_checkattack, and have moved).
		// if the move failed, our state is as_straight, and it will be taken care of below
		if ((!retval) && (AttackState == AS_SLIDING))
			return;
	}
	else if (AIFlags & AI_CHARGING)
	{
		IdealYaw = EnemyYaw;
		if (!(AIFlags & AI_MANUAL_STEERING))
			ChangeYaw ();
	}
	if (retval)
	{
		// PMM - is this useful?  Monsters attacking usually call the ai_charge routine..
		// the only monster this affects should be the soldier
		if ((Dist != 0) && (!alreadyMoved) && (AttackState == AS_STRAIGHT) && (!(AIFlags & AI_STAND_GROUND)))
			MoveToGoal (Dist);

		if ((Entity->gameEntity->enemy) && (Entity->gameEntity->enemy->inUse) && (EnemyVis))
		{
			AIFlags &= ~AI_LOST_SIGHT;
			Vec3Copy (Entity->gameEntity->enemy->state.origin, LastSighting);
			TrailTime = level.framenum;
			//PMM
			Vec3Copy (Entity->gameEntity->enemy->state.origin, BlindFireTarget);
			BlindFireDelay = 0;
			//pmm
		}
		return;
	}
	//PMM

	// PGM - added a little paranoia checking here... 9/22/98
	if ((Entity->gameEntity->enemy) && (Entity->gameEntity->enemy->inUse) && (EnemyVis))
	{
		// PMM - check for alreadyMoved
		if (!alreadyMoved)
			MoveToGoal (Dist);
		if(!Entity->IsInUse())
			return;			// PGM - g_touchtrigger free problem

		AIFlags &= ~AI_LOST_SIGHT;
		Vec3Copy (Entity->gameEntity->enemy->state.origin, LastSighting);
		TrailTime = level.framenum;
		// PMM
		Vec3Copy (Entity->gameEntity->enemy->state.origin, BlindFireTarget);
		BlindFireDelay = 0;
		// pmm
		return;
	}

// PMM - moved down here to allow monsters to get on hint paths
	// coop will change to another enemy if visible
	if (game.mode == GAME_COOPERATIVE)
	{	// FIXME: insane guys get mad with this, which causes crashes!
		if (FindTarget ())
			return;
	}
// pmm

	if ((SearchTime) && (level.framenum > (SearchTime + 200)))
	{
		// PMM - double move protection
		if (!alreadyMoved)
			MoveToGoal (Dist);
		SearchTime = 0;
		return;
	}

	save = Entity->gameEntity->goalentity;
	tempgoal = G_Spawn();
	Entity->gameEntity->goalentity = tempgoal;

	isNew = false;

	if (!(AIFlags & AI_LOST_SIGHT))
	{
#ifdef MONSTERS_USE_PATHFINDING
		P_NodePathTimeout = level.framenum + 100; // Do "blind fire" first.
#endif

		// just lost sight of the player, decide where to go first
		AIFlags |= (AI_LOST_SIGHT | AI_PURSUIT_LAST_SEEN);
		AIFlags &= ~(AI_PURSUE_NEXT | AI_PURSUE_TEMP);
		isNew = true;
	}
#ifdef MONSTERS_USE_PATHFINDING
	else if ((AIFlags & AI_LOST_SIGHT) && P_NodePathTimeout < level.framenum)
	{
		// Set us up for pathing
		P_CurrentNode = GetClosestNodeTo(origin);
		P_CurrentGoalNode = GetClosestNodeTo(Entity->gameEntity->enemy->state.origin);
		FoundPath ();
	}
#endif

	if (AIFlags & AI_PURSUE_NEXT)
	{
		AIFlags &= ~AI_PURSUE_NEXT;

		// give ourself more time since we got this far
		SearchTime = level.framenum + 50;

		if (AIFlags & AI_PURSUE_TEMP)
		{
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
			Vec3Copy (marker->state.origin, LastSighting);
			TrailTime = marker->timestamp;

			vec3f angles = Entity->State.GetAngles();
			angles.Y = IdealYaw = marker->state.angles[YAW];
			Entity->State.SetAngles(angles);

			isNew = true;
		}
	}

	Vec3Subtract (origin, LastSighting, v);
	float d1 = Vec3Length(v);
	if (d1 <= Dist)
	{
		AIFlags |= AI_PURSUE_NEXT;
		Dist = d1;
	}

	Vec3Copy (LastSighting, Entity->gameEntity->goalentity->state.origin);

	if (isNew)
	{
		CTrace tr (vec3f(origin), Entity->GetMins(), Entity->GetMaxs(), vec3f(LastSighting), Entity->gameEntity, CONTENTS_MASK_PLAYERSOLID);
		if (tr.fraction < 1)
		{
			float center = tr.fraction;
			Vec3Subtract (Entity->gameEntity->goalentity->state.origin, origin, v);
			d1 = Vec3Length(v);

			float d2 = d1 * ((center+1)/2);
			vec3_t angles;
			vec3_t origin;

			Entity->State.GetOrigin (origin);
			Entity->State.GetAngles (angles);
			angles[YAW] = IdealYaw = VecToYaw(v);
			Entity->State.SetAngles(angles);

			vec3_t v_forward, v_right;
			Angles_Vectors(angles, v_forward, v_right, NULL);

			vec3_t left_target;
			Vec3Set(v, d2, -16, 0);
			G_ProjectSource (origin, v, v_forward, v_right, left_target);
			tr = CTrace(Entity->State.GetOrigin(), Entity->GetMins(), Entity->GetMaxs(), vec3f(left_target), Entity->gameEntity, CONTENTS_MASK_PLAYERSOLID);
			float left = tr.fraction;

			vec3_t right_target;
			Vec3Set(v, d2, 16, 0);
			G_ProjectSource (origin, v, v_forward, v_right, right_target);
			tr = CTrace(Entity->State.GetOrigin(), Entity->GetMins(), Entity->GetMaxs(), vec3f(right_target), Entity->gameEntity, CONTENTS_MASK_PLAYERSOLID);
			float right = tr.fraction;

			center = (d1*center)/d2;
			if (left >= center && left > right)
			{
				if (left < 1)
				{
					Vec3Set(v, d2 * left * 0.5, -16, 0);
					G_ProjectSource (origin, v, v_forward, v_right, left_target);
				}
				Vec3Copy (LastSighting, SavedGoal);
				AIFlags |= AI_PURSUE_TEMP;
				Vec3Copy (left_target, Entity->gameEntity->goalentity->state.origin);
				Vec3Copy (left_target, LastSighting);
				Vec3Subtract (Entity->gameEntity->goalentity->state.origin, origin, v);

				angles[YAW] = IdealYaw = VecToYaw(v);
				Entity->State.SetAngles(angles);
			}
			else if (right >= center && right > left)
			{
				if (right < 1)
				{
					Vec3Set(v, d2 * right * 0.5, 16, 0);
					G_ProjectSource (origin, v, v_forward, v_right, right_target);
				}
				Vec3Copy (LastSighting, SavedGoal);
				AIFlags |= AI_PURSUE_TEMP;
				Vec3Copy (right_target, Entity->gameEntity->goalentity->state.origin);
				Vec3Copy (right_target, LastSighting);
				Vec3Subtract (Entity->gameEntity->goalentity->state.origin, origin, v);
				angles[YAW] = IdealYaw = VecToYaw(v);
				Entity->State.SetAngles(angles);
			}
		}
	}

	MoveToGoal (Dist);
	if(!Entity->IsInUse())
		return;			// PGM - g_touchtrigger free problem

	G_FreeEdict(tempgoal);

	if (Entity)
		Entity->gameEntity->goalentity = save;
#endif
}

void CMonster::AI_Run_Melee ()
{
#ifndef MONSTER_USE_ROGUE_AI
	IdealYaw = EnemyYaw;
	ChangeYaw ();

	if (FacingIdeal())
	{
		Melee ();
		AttackState = AS_STRAIGHT;
	}
#else
	IdealYaw = EnemyYaw;
	if (!(AIFlags & AI_MANUAL_STEERING))
		ChangeYaw ();

	if (FacingIdeal())
	{
		Melee ();
		AttackState = AS_STRAIGHT;
	}
#endif
}

void CMonster::AI_Run_Missile()
{
#ifndef MONSTER_USE_ROGUE_AI
	IdealYaw = EnemyYaw;
	ChangeYaw ();

	if (FacingIdeal())
	{
		Attack ();
		AttackState = AS_STRAIGHT;
	}
#else
	IdealYaw = EnemyYaw;
	if (!(AIFlags & AI_MANUAL_STEERING))
		ChangeYaw ();

	if (FacingIdeal())
	{
		Attack ();
		if ((AttackState == AS_MISSILE) || (AttackState == AS_BLIND))
			AttackState = AS_STRAIGHT;
	}
#endif
}

void CMonster::AI_Run_Slide(float Dist)
{
#ifndef MONSTER_USE_ROGUE_AI
	IdealYaw = EnemyYaw;
	ChangeYaw ();

	if (WalkMove (IdealYaw + ((Lefty) ? 90 : -90), Dist))
		return;
		
	Lefty = !Lefty;
	WalkMove (IdealYaw - ((Lefty) ? 90 : -90), Dist);
#else
	float	ofs;
	float	angle;

	IdealYaw = EnemyYaw;
	
	angle = 90;
	
	if (Lefty)
		ofs = angle;
	else
		ofs = -angle;

	if (!(AIFlags & AI_MANUAL_STEERING))
		ChangeYaw ();

	// PMM - clamp maximum sideways move for non flyers to make them look less jerky
	if (!(Entity->gameEntity->flags & FL_FLY))
		Dist = Min<> (Dist, 8.0f);
	if (WalkMove (IdealYaw + ofs, Dist))
		return;
	// PMM - if we're dodging, give up on it and go straight
	if (AIFlags & AI_DODGING)
	{
		DoneDodge ();
		// by setting as_straight, caller will know to try straight move
		AttackState = AS_STRAIGHT;
		return;
	}

	Lefty = !Lefty;
	if (WalkMove (IdealYaw - ofs, Dist))
		return;
	// PMM - if we're dodging, give up on it and go straight
	if (AIFlags & AI_DODGING)
		DoneDodge ();

	// PMM - the move failed, so signal the caller (ai_run) to try going straight
	AttackState = AS_STRAIGHT;
#endif
}

void CMonster::AI_Stand (float Dist)
{
#ifndef MONSTER_USE_ROGUE_AI
	if (Dist)
		WalkMove (Entity->State.GetAngles().Y, Dist);

#ifdef MONSTERS_USE_PATHFINDING
	if (FollowingPath)
	{
		// Assuming we got here because we're waiting for something.
		if (P_CurrentNode->Type == NODE_DOOR)
		{
			if (P_CurrentNode->LinkedEntity->moveinfo.state == 0)
				Run(); // We can go again!
		}
		else
		{
			// ...this shouldn't happen. FIND OUT WHY PLZ
			FollowingPath = false;
		}
		return;
	}
#endif

	if (AIFlags & AI_STAND_GROUND)
	{
		if (Entity->gameEntity->enemy)
		{
			vec3_t v;
			Entity->State.GetOrigin(v);
			Vec3Subtract (Entity->gameEntity->enemy->state.origin, v, v);
			IdealYaw = VecToYaw(v);
			if (Entity->State.GetAngles().Y != IdealYaw && AIFlags & AI_TEMP_STAND_GROUND)
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
	
	if (level.framenum > PauseTime)
	{
		Walk ();
		return;
	}

	if (!(Entity->gameEntity->spawnflags & 1) && (MonsterFlags & MF_HAS_IDLE) && (level.framenum > IdleTime))
	{
		if (IdleTime)
		{
			Idle ();
			IdleTime = level.framenum + 150 + (random() * 150);
		}
		else
			IdleTime = level.framenum + (random() * 150);
	}
#else
	if (Dist)
		WalkMove (Entity->State.GetAngles().Y, Dist);

#ifdef MONSTERS_USE_PATHFINDING
	if (FollowingPath)
	{
		// Assuming we got here because we're waiting for something.
		if (P_CurrentNode->Type == NODE_DOOR || P_CurrentNode->Type == NODE_PLATFORM)
		{
			if (P_CurrentNode->LinkedEntity->moveinfo.state == STATE_TOP)
				Run(); // We can go again!
		}
		// In two goals, do we reach the platform node?
		else if (P_CurrentPath->Path[P_CurrentNodeIndex-1]->Type == NODE_PLATFORM)
		{
			CPlatForm *Plat = dynamic_cast<CPlatForm*>(P_CurrentPath->Path[P_CurrentNodeIndex-1]->LinkedEntity->Entity); // get the plat
			// Is it at bottom?
			if (Plat->MoveState == STATE_BOTTOM)
				Run (); // Go!
		}
		else
		{
			// ...this shouldn't happen. FIND OUT WHY PLZ
			FollowingPath = false;
		}
		return;
	}
#endif

	if (AIFlags & AI_STAND_GROUND)
	{
		if (Entity->gameEntity->enemy)
		{
			vec3_t origin, v;
			Entity->State.GetOrigin (origin);

			Vec3Subtract (Entity->gameEntity->enemy->state.origin, origin, v);
			IdealYaw = VecToYaw(v);
			if (Entity->State.GetAngles().Y != IdealYaw && (AIFlags & AI_TEMP_STAND_GROUND))
			{
				AIFlags &= ~(AI_STAND_GROUND | AI_TEMP_STAND_GROUND);
				Run ();
			}
			if (!(AIFlags & AI_MANUAL_STEERING))
				ChangeYaw ();
			// PMM
			// find out if we're going to be shooting
			bool retval = AI_CheckAttack ();
			// record sightings of player
			if ((Entity->gameEntity->enemy) && (Entity->gameEntity->enemy->inUse) && (visible(Entity->gameEntity, Entity->gameEntity->enemy)))
			{
				AIFlags &= ~AI_LOST_SIGHT;
				Vec3Copy (Entity->gameEntity->enemy->state.origin, LastSighting);
				Vec3Copy (Entity->gameEntity->enemy->state.origin, BlindFireTarget);
				TrailTime = level.framenum;
				BlindFireDelay = 0;
			}
			// check retval to make sure we're not blindfiring
			else if (!retval)
			{
				FindTarget ();
				return;
			}
//			CheckAttack ();
			// pmm
		}
		else
			FindTarget ();
		return;
	}

	if (FindTarget ())
		return;
	
	if (level.framenum > PauseTime)
	{
		Walk ();
		return;
	}

	if (!(Entity->gameEntity->spawnflags & 1) && (MonsterFlags & MF_HAS_IDLE) && (level.framenum > IdleTime))
	{
		if (IdleTime)
		{
			Idle ();
			IdleTime = level.framenum + 150 + random() * 150;
		}
		else
		{
			IdleTime = level.framenum + random() * 150;
		}
	}
#endif
}

void CMonster::ReactToDamage (edict_t *attacker)
{
	if (!(attacker->client) && !(attacker->Entity && (attacker->Entity->EntityFlags & ENT_MONSTER)))
		return;

	if (attacker == Entity->gameEntity || attacker == Entity->gameEntity->enemy)
		return;

	// if we are a good guy monster and our attacker is a player
	// or another good guy, do not get mad at them
	if (AIFlags & AI_GOOD_GUY)
	{
		if (attacker->client || ((dynamic_cast<CMonsterEntity*>(attacker->Entity))->Monster->AIFlags & AI_GOOD_GUY))
			return;
	}

	// we now know that we are not both good guys

	// if attacker is a client, get mad at them because he's good and we're not
	if (attacker->client)
	{
		AIFlags &= ~AI_SOUND_TARGET;

		// this can only happen in coop (both new and old enemies are clients)
		// only switch if can't see the current enemy
		if (Entity->gameEntity->enemy && Entity->gameEntity->enemy->client)
		{
			if (visible(Entity->gameEntity, Entity->gameEntity->enemy))
			{
				Entity->gameEntity->oldenemy = attacker;
				return;
			}
			Entity->gameEntity->oldenemy = Entity->gameEntity->enemy;
		}
		Entity->gameEntity->enemy = attacker;
		if (!(AIFlags & AI_DUCKED))
			FoundTarget ();
		return;
	}

#ifdef MONSTERS_ARENT_STUPID
	// Help our buddy!
	if ((attacker->Entity && (attacker->Entity->EntityFlags & ENT_MONSTER)) && attacker->enemy && attacker->enemy != Entity->gameEntity)
	{
		if (Entity->gameEntity->enemy && Entity->gameEntity->enemy->client)
			Entity->gameEntity->oldenemy = Entity->gameEntity->enemy;
		Entity->gameEntity->enemy = attacker->enemy;
		if (!(AIFlags & AI_DUCKED))
			FoundTarget ();
	}
	return;
#else
	// it's the same base (walk/swim/fly) type and a different classname and it's not a tank
	// (they spray too much), get mad at them
	//if (((Entity->gameEntity->flags & (FL_FLY|FL_SWIM)) == (attacker->flags & (FL_FLY|FL_SWIM))) &&
	//	 (strcmp (Entity->gameEntity->classname, attacker->classname) != 0))// &&
		// (strcmp(attacker->classname, "monster_tank") != 0) &&
		// (strcmp(attacker->classname, "monster_supertank") != 0) &&
		// (strcmp(attacker->classname, "monster_makron") != 0) &&
		// (strcmp(attacker->classname, "monster_jorg") != 0) )
	{
		if (Entity->gameEntity->enemy && Entity->gameEntity->enemy->client)
			Entity->gameEntity->oldenemy = Entity->gameEntity->enemy;
		Entity->gameEntity->enemy = attacker;
		if (!(AIFlags & AI_DUCKED))
			FoundTarget ();
	}
	// if they *meant* to shoot us, then shoot back
	/*else */if (attacker->enemy == Entity)
	{
		if (Entity->gameEntity->enemy && Entity->gameEntity->enemy->client)
			Entity->gameEntity->oldenemy = Entity->gameEntity->enemy;
		Entity->gameEntity->enemy = attacker;
		if (!(AIFlags & AI_DUCKED))
			FoundTarget ();
	}
#endif
}

void CMonster::AI_Walk(float Dist)
{
	MoveToGoal (Dist);

	// check for noticing a player
	if (FindTarget ())
		return;

	if ((MonsterFlags & MF_HAS_SEARCH) && (level.framenum > IdleTime))
	{
		if (IdleTime)
		{
			Search ();
			IdleTime = level.framenum + 150 + (random() * 150);
		}
		else
			IdleTime = level.framenum + (random() * 150);
	}
}

// These are intended to be virtually replaced.
void CMonster::Stand ()
{
}

void CMonster::Idle ()
{
	if (MonsterFlags & MF_HAS_IDLE)
		DebugPrintf ("Warning: Monster with no idle has MF_HAS_IDLE!\n");
}

void CMonster::Search ()
{
	if (MonsterFlags & MF_HAS_SEARCH)
		DebugPrintf ("Warning: Monster with no search has MF_HAS_SEARCH!\n");
}

void CMonster::Walk ()
{
}

void CMonster::Run ()
{
}

#ifndef MONSTER_USE_ROGUE_AI
void CMonster::Dodge (edict_t *other, float eta)
{
}
#endif

void CMonster::Attack()
{
	if (MonsterFlags & MF_HAS_ATTACK)
		DebugPrintf ("Warning: Monster with no attack has MF_HAS_ATTACK!\n");
}

void CMonster::Melee ()
{
	if (MonsterFlags & MF_HAS_MELEE)
		DebugPrintf ("Warning: Monster with no melee has MF_HAS_MELEE!\n");
}

void CMonster::Sight ()
{
	if (MonsterFlags & MF_HAS_SIGHT)
		DebugPrintf ("Warning: Monster with no sight has MF_HAS_SIGHT!\n");
}

void CMonster::MonsterDeathUse ()
{
	Entity->gameEntity->flags &= ~(FL_FLY|FL_SWIM);
	AIFlags &= AI_GOOD_GUY;

	if (Entity->gameEntity->item)
	{
		Entity->gameEntity->item->DropItem (Entity);
		Entity->gameEntity->item = NULL;
	}

	if (Entity->gameEntity->deathtarget)
		Entity->gameEntity->target = Entity->gameEntity->deathtarget;

	if (!Entity->gameEntity->target)
		return;

	G_UseTargets (Entity, Entity->gameEntity->enemy->Entity);
}

void CMonster::MonsterThink ()
{
	Entity->NextThink = level.framenum + FRAMETIME;
	MoveFrame ();
	if (Entity->GetLinkCount() != LinkCount)
	{
		LinkCount = Entity->GetLinkCount();
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

	// Backwards animation support
	if (Move->FirstFrame > Move->LastFrame)
	{
		if ((NextFrame) && (NextFrame >= Move->LastFrame) && (NextFrame <= Move->FirstFrame))
		{
			Entity->State.SetFrame(NextFrame);
			NextFrame = 0;
		}
		else
		{
			if (Entity->State.GetFrame() == Move->LastFrame)
			{
				if (Move->EndFunc)
				{
					void (CMonster::*EndFunc) () = Move->EndFunc;
					(this->*EndFunc) ();

					// regrab move, endfunc is very likely to change it
					Move = CurrentMove;

					// check for death
					if (Entity->GetSvFlags() & SVF_DEADMONSTER)
						return;
				}
			}

			if (Entity->State.GetFrame() < Move->LastFrame || Entity->State.GetFrame() > Move->FirstFrame)
			{
				AIFlags &= ~AI_HOLD_FRAME;
				Entity->State.SetFrame (Move->FirstFrame);
			}
			else
			{
				if (!(AIFlags & AI_HOLD_FRAME))
				{
					Entity->State.SetFrame(Entity->State.GetFrame() - 1);
					if (Entity->State.GetFrame() < Move->LastFrame)
						Entity->State.SetFrame(Move->FirstFrame);
				}
			}
		}

		index = Move->FirstFrame - Entity->State.GetFrame();

		void (CMonster::*AIFunc) (float Dist) = Move->Frames[index].AIFunc;
		if (AIFunc)
			(this->*AIFunc) ((AIFlags & AI_HOLD_FRAME) ? 0 : (Move->Frames[index].Dist * Scale));

		void (CMonster::*Function) () = Move->Frames[index].Function;
		if (Function)
			(this->*Function) ();
	}
	else
	{
		if ((NextFrame) && (NextFrame >= Move->FirstFrame) && (NextFrame <= Move->LastFrame))
		{
			Entity->State.SetFrame (NextFrame);
			NextFrame = 0;
		}
		else
		{
			if (Entity->State.GetFrame() == Move->LastFrame)
			{
				if (Move->EndFunc)
				{
					void (CMonster::*EndFunc) () = Move->EndFunc;
					(this->*EndFunc) ();

					// regrab move, endfunc is very likely to change it
					Move = CurrentMove;

					// check for death
					if (Entity->GetSvFlags() & SVF_DEADMONSTER)
						return;
				}
			}

			if (Entity->State.GetFrame() < Move->FirstFrame || Entity->State.GetFrame() > Move->LastFrame)
			{
				AIFlags &= ~AI_HOLD_FRAME;
				Entity->State.SetFrame (Move->FirstFrame);
			}
			else
			{
				if (!(AIFlags & AI_HOLD_FRAME))
				{
					Entity->State.SetFrame(Entity->State.GetFrame()+1);
					if (Entity->State.GetFrame() > Move->LastFrame)
						Entity->State.SetFrame(Move->FirstFrame);
				}
			}
		}

		index = Entity->State.GetFrame() - Move->FirstFrame;

		void (CMonster::*AIFunc) (float Dist) = Move->Frames[index].AIFunc;
		if (AIFunc)
			(this->*AIFunc) ((AIFlags & AI_HOLD_FRAME) ? 0 : (Move->Frames[index].Dist * Scale));

		void (CMonster::*Function) () = Move->Frames[index].Function;
		if (Function)
			(this->*Function) ();
	}
}

void CMonster::FoundTarget ()
{
	// let other monsters see this monster for a while
#ifndef MONSTERS_ARENT_STUPID
	if (Entity->gameEntity->enemy->client)
	{
		level.sight_entity = Entity;
		level.sight_entity_framenum = level.framenum;
		level.sight_entity->light_level = 128;
	}

	Entity->gameEntity->show_hostile = level.framenum + 10;		// wake up other monsters
#endif


	Vec3Copy(Entity->gameEntity->enemy->state.origin, LastSighting);
	TrailTime = level.framenum;

	if (!Entity->gameEntity->combattarget)
	{
		HuntTarget ();
		return;
	}

	CBaseEntity *Target = CC_PickTarget(Entity->gameEntity->combattarget);
	Entity->gameEntity->goalentity = Entity->gameEntity->movetarget = Target->gameEntity;
	if (!Entity->gameEntity->movetarget)
	{
		Entity->gameEntity->goalentity = Entity->gameEntity->movetarget = Entity->gameEntity->enemy;
		HuntTarget ();
		MapPrint (MAPPRINT_ERROR, Entity, Entity->State.GetOrigin(), "combattarget %s not found\n", Entity->gameEntity->combattarget);
		return;
	}

	// clear out our combattarget, these are a one shot deal
	Entity->gameEntity->combattarget = NULL;
	AIFlags |= AI_COMBAT_POINT;

	// clear the targetname, that point is ours!
	Entity->gameEntity->movetarget->targetname = NULL;
	PauseTime = 0;
#ifdef MONSTER_USE_ROGUE_AI
	// PMM
	Vec3Copy (Entity->gameEntity->enemy->state.origin, BlindFireTarget);
	BlindFireDelay = 0;
	// PMM
#endif

	// run for it
	Run ();
}

void CMonster::SetEffects()
{
	Entity->State.SetEffects (0);
	Entity->State.SetRenderEffects(RF_FRAMELERP);

	if (AIFlags & AI_RESURRECTING)
	{
		Entity->State.AddEffects(EF_COLOR_SHELL);
		Entity->State.AddRenderEffects(RF_SHELL_RED);
	}

	if (Entity->gameEntity->health <= 0)
		return;

	if (Entity->gameEntity->powerarmor_time > level.framenum)
	{
		if (PowerArmorType == POWER_ARMOR_SCREEN)
			Entity->State.AddEffects (EF_POWERSCREEN);
		else if (PowerArmorType == POWER_ARMOR_SHIELD)
		{
			Entity->State.AddEffects (EF_COLOR_SHELL);
			Entity->State.AddRenderEffects (RF_SHELL_GREEN);
		}
	}
}

void CMonster::WorldEffects()
{
	vec3_t origin;
	Entity->State.GetOrigin(origin);
	if (Entity->gameEntity->health > 0)
	{
		if (!(Entity->gameEntity->flags & FL_SWIM))
		{
			if (Entity->gameEntity->waterlevel < 3)
				Entity->gameEntity->air_finished = level.framenum + 120;
			else if (Entity->gameEntity->air_finished < level.framenum)
			{
				if (Entity->gameEntity->pain_debounce_time < level.framenum)
				{
					int dmg = 2 + 2 * (level.framenum - Entity->gameEntity->air_finished);
					if (dmg > 15)
						dmg = 15;
					T_Damage (Entity->gameEntity, world, world, vec3Origin, origin, vec3Origin, dmg, 0, DAMAGE_NO_ARMOR, MOD_WATER);
					Entity->gameEntity->pain_debounce_time = level.framenum + 10;
				}
			}
		}
		else
		{
			if (Entity->gameEntity->waterlevel > 0)
				Entity->gameEntity->air_finished = level.framenum + 90;
			else if (Entity->gameEntity->air_finished < level.framenum)
			{	// suffocate!
				if (Entity->gameEntity->pain_debounce_time < level.framenum)
				{
					int dmg = 2 + 2 * (level.framenum - Entity->gameEntity->air_finished);
					if (dmg > 15)
						dmg = 15;
					T_Damage (Entity->gameEntity, world, world, vec3Origin, origin, vec3Origin, dmg, 0, DAMAGE_NO_ARMOR, MOD_WATER);
					Entity->gameEntity->pain_debounce_time = level.framenum + 10;
				}
			}
		}
	}
	
	if (Entity->gameEntity->waterlevel == 0)
	{
		if (Entity->gameEntity->flags & FL_INWATER)
		{	
			Entity->PlaySound (CHAN_BODY, SoundIndex("player/watr_out.wav"));
			Entity->gameEntity->flags &= ~FL_INWATER;
		}
		return;
	}

	if ((Entity->gameEntity->watertype & CONTENTS_LAVA) && !(Entity->gameEntity->flags & FL_IMMUNE_LAVA))
	{
		if (Entity->gameEntity->damage_debounce_time < level.framenum)
		{
			Entity->gameEntity->damage_debounce_time = level.framenum + 2;
			T_Damage (Entity->gameEntity, world, world, vec3Origin, origin, vec3Origin, 10*Entity->gameEntity->waterlevel, 0, 0, MOD_LAVA);
		}
	}
	if ((Entity->gameEntity->watertype & CONTENTS_SLIME) && !(Entity->gameEntity->flags & FL_IMMUNE_SLIME))
	{
		if (Entity->gameEntity->damage_debounce_time < level.framenum)
		{
			Entity->gameEntity->damage_debounce_time = level.framenum + 10;
			T_Damage (Entity->gameEntity, world, world, vec3Origin, origin, vec3Origin, 4*Entity->gameEntity->waterlevel, 0, 0, MOD_SLIME);
		}
	}
	
	if ( !(Entity->gameEntity->flags & FL_INWATER) )
	{	
		if (!(Entity->GetSvFlags() & SVF_DEADMONSTER))
		{
			if (Entity->gameEntity->watertype & CONTENTS_LAVA)
			{
				if (random() <= 0.5)
					Entity->PlaySound (CHAN_BODY, SoundIndex("player/lava1.wav"));
				else
					Entity->PlaySound (CHAN_BODY, SoundIndex("player/lava2.wav"));
			}
			else
				Entity->PlaySound (CHAN_BODY, SoundIndex("player/watr_in.wav"));
		}

		Entity->gameEntity->flags |= FL_INWATER;
		Entity->gameEntity->damage_debounce_time = 0;
	}
}

void CMonster::CatagorizePosition()
{
	vec3_t		point;
	int			cont;
//
// get waterlevel
//
	Entity->State.GetOrigin(point);
	point[2] += Entity->GetMins().Z + 1;	
	cont = PointContents (point);

	if (!(cont & CONTENTS_MASK_WATER))
	{
		Entity->gameEntity->waterlevel = 0;
		Entity->gameEntity->watertype = 0;
		return;
	}

	Entity->gameEntity->watertype = cont;
	Entity->gameEntity->waterlevel = 1;
	point[2] += 26;
	cont = PointContents (point);
	if (!(cont & CONTENTS_MASK_WATER))
		return;

	Entity->gameEntity->waterlevel = 2;
	point[2] += 22;
	cont = PointContents (point);
	if (cont & CONTENTS_MASK_WATER)
		Entity->gameEntity->waterlevel = 3;
}

void CMonster::CheckGround()
{
	vec3_t		point;
	CTrace		trace;

	if (Entity->gameEntity->flags & (FL_SWIM|FL_FLY))
		return;

	if (Entity->gameEntity->velocity[2] > 100)
	{
		Entity->gameEntity->groundentity = NULL;
		return;
	}

// if the hull point one-quarter unit down is solid the entity is on ground
	Entity->State.GetOrigin (point);
	point[2] -= 0.25;

	trace = CTrace (Entity->State.GetOrigin(), Entity->GetMins(), Entity->GetMaxs(), vec3f(point), Entity->gameEntity, CONTENTS_MASK_MONSTERSOLID);

	// check steepness
	if ( trace.plane.normal[2] < 0.7 && !trace.startSolid)
	{
		Entity->gameEntity->groundentity = NULL;
		return;
	}

	if (!trace.startSolid && !trace.allSolid)
	{
		Entity->State.SetOrigin (trace.endPos);
		Entity->gameEntity->groundentity = trace.ent;
		Entity->gameEntity->groundentity_linkcount = trace.ent->linkCount;
		Entity->gameEntity->velocity[2] = 0;
	}
}

void CMonster::HuntTarget()
{
	vec3_t	vec;

	Entity->gameEntity->goalentity = Entity->gameEntity->enemy;
	if (AIFlags & AI_STAND_GROUND)
		Stand ();
	else
		Run ();

	Entity->State.GetOrigin(vec);
	Vec3Subtract (Entity->gameEntity->enemy->state.origin, vec, vec);
	IdealYaw = VecToYaw(vec);
	// wait a while before first attack
	if (!(AIFlags & AI_STAND_GROUND))
		AttackFinished = level.framenum + 1;
}

bool CMonster::FindTarget()
{
	bool		heardit;
	CPlayerEntity *client;

	if (AIFlags & AI_GOOD_GUY)
	{
		if (Entity->gameEntity->goalentity && Entity->gameEntity->goalentity->inUse && Entity->gameEntity->goalentity->classname)
		{
			if (strcmp(Entity->gameEntity->goalentity->classname, "target_actor") == 0)
				return false;
		}

		//FIXME look for monsters?
		return false;
	}

	// if we're going to a combat point, just proceed
	if (AIFlags & AI_COMBAT_POINT)
		return false;

#ifdef MONSTERS_USE_PATHFINDING
	if ((level.SoundEntityFramenum >= (level.framenum - 1)) && level.NoiseNode)
	{
		vec3_t temp;
		vec3_t origin;

		Entity->State.GetOrigin(origin);
		if (Entity->gameEntity->spawnflags & 1)
		{
			CTrace trace = CTrace(origin, level.NoiseNode->Origin, Entity->gameEntity, CONTENTS_MASK_SOLID);

			if (trace.fraction < 1.0)
				return false;
		}
		else
		{
			if (!InHearableArea(origin, level.NoiseNode->Origin))
				return false;
		}

		Vec3Subtract (level.NoiseNode->Origin, origin, temp);

		if (Vec3Length(temp) > 1000)	// too far to hear
			return false;

		IdealYaw = VecToYaw(temp);
		ChangeYaw ();

		// hunt the sound for a bit; hopefully find the real player
		//AIFlags |= AI_SOUND_TARGET;

		P_CurrentNode = GetClosestNodeTo(origin);
		P_CurrentGoalNode = level.NoiseNode;
		FoundPath ();

		// Check if we can see the entity too
		if (!Entity->gameEntity->enemy && (level.SoundEntityFramenum >= (level.framenum - 1)) && !(Entity->gameEntity->spawnflags & 1) )
		{
			client = level.SoundEntity;

			if (client)
			{
				vec3_t	temp;

				if (client->gameEntity->flags & FL_NOTARGET)
					return false;

				client->State.GetOrigin (temp);
				Entity->State.GetOrigin (origin);
				if (Entity->gameEntity->spawnflags & 1)
				{
					if (!visible (Entity->gameEntity, client->gameEntity))
						return false;
				}
				else
				{
					if (!InHearableArea(origin, temp))
						return false;
				}

				Vec3Subtract (temp, origin, temp);

				if (Vec3Length(temp) > 1000)	// too far to hear
					return false;

				// check area portals - if they are different and not connected then we can't hear it
				if (client->GetAreaNum() != Entity->GetAreaNum())
				{
					if (!gi.AreasConnected(Entity->GetAreaNum(), client->GetAreaNum()))
						return false;
				}

				// hunt the sound for a bit; hopefully find the real player
				Entity->gameEntity->enemy = client->gameEntity;

				FoundTarget ();
				AlertNearbyStroggs ();

				if (MonsterFlags & MF_HAS_SIGHT)
					Sight ();
			}
		}

		return true;
	}
#endif

// if the first spawnflag bit is set, the monster will only wake up on
// really seeing the player, not another monster getting angry or hearing
// something

// revised behavior so they will wake up if they "see" a player make a noise
// but not weapon impact/explosion noises

	heardit = false;
#ifndef MONSTERS_USE_PATHFINDING
	if ((level.sight_entity_framenum >= (level.framenum - 1)) && !(Entity->gameEntity->spawnflags & 1) )
	{
		client = level.sight_entity;
		if (client->enemy == Entity->gameEntity->enemy)
			return false;
	}
#endif

#ifndef MONSTERS_USE_PATHFINDING
	else if (level.sound_entity_framenum >= (level.framenum - 1))
	{
		client = level.sound_entity;
		heardit = true;
	}
	else if (!(Entity->gameEntity->enemy) && (level.sound2_entity_framenum >= (level.framenum - 1)) && !(Entity->gameEntity->spawnflags & 1) )
	{
		client = level.sound2_entity;
		heardit = true;
	}
#else
	if (level.SoundEntityFramenum >= (level.framenum - 1))
	{
		client = level.SoundEntity;
		heardit = true;
	}
#endif
	else
	{
		client = level.sight_client;
		if (!client)
			return false;	// no clients to get mad at
	}

	if (!client)
		client = level.sight_client;
	if (!client)
		return false;

	// if the entity went away, forget it
	if (!client->IsInUse())
		return false;

	if (visible(Entity->gameEntity, client->gameEntity) && (client->gameEntity == Entity->gameEntity->enemy))
		return true;	// JDC false;

	if (client)
	{
		if (client->gameEntity->flags & FL_NOTARGET)
			return false;
	}
	else
		return false;

	edict_t *old = Entity->gameEntity->enemy;
	if (!heardit)
	{
		int r = range (Entity->gameEntity, client->gameEntity);

		if (r == RANGE_FAR)
			return false;

// this is where we would check invisibility

		// is client in an spot too dark to be seen?
		if (client->gameEntity->light_level <= 5)
			return false;

		if (!visible (Entity->gameEntity, client->gameEntity))
			return false;

		if (r == RANGE_NEAR)
		{
			if (client->gameEntity->show_hostile < level.framenum && !infront (Entity->gameEntity, client->gameEntity))
				return false;
		}
		else if (r == RANGE_MID)
		{
			if (!infront (Entity->gameEntity, client->gameEntity))
				return false;
		}

		Entity->gameEntity->enemy = client->gameEntity;

		AIFlags &= ~AI_SOUND_TARGET;

		if (!Entity->gameEntity->enemy->client)
		{
			Entity->gameEntity->enemy = Entity->gameEntity->enemy->enemy;
			if (!Entity->gameEntity->enemy->client)
			{
				Entity->gameEntity->enemy = NULL;
				return false;
			}
		}
	}
	else	// heardit
	{
		vec3_t	temp;
		vec3_t origin;

		client->State.GetOrigin (temp);
		Entity->State.GetOrigin(origin);
		if (Entity->gameEntity->spawnflags & 1)
		{
			if (!visible (Entity->gameEntity, client->gameEntity))
				return false;
		}
		else
		{
			if (!InHearableArea(origin, temp))
				return false;
		}

		Vec3Subtract (temp, origin, temp);

		if (Vec3Length(temp) > 1000)	// too far to hear
			return false;

		// check area portals - if they are different and not connected then we can't hear it
		if (client->GetAreaNum() != Entity->GetAreaNum())
		{
			if (!gi.AreasConnected(Entity->GetAreaNum(), client->GetAreaNum()))
				return false;
		}

		IdealYaw = VecToYaw(temp);
		ChangeYaw ();

		// hunt the sound for a bit; hopefully find the real player
		//AIFlags |= AI_SOUND_TARGET;
		Entity->gameEntity->enemy = client->gameEntity;
	}

//
// got one
//
	FoundTarget ();
#ifdef MONSTERS_ARENT_STUPID
	AlertNearbyStroggs ();
#endif

	if ((Entity->gameEntity->enemy != old) && (MonsterFlags & MF_HAS_SIGHT))
		Sight ();

	return true;
}

bool CMonster::FacingIdeal()
{
	float delta = AngleModf (Entity->State.GetAngles().Y - IdealYaw);
	if (delta > 45 && delta < 315)
		return false;
	return true;
}

void CMonster::FliesOff()
{
	Entity->State.RemoveEffects(EF_FLIES);
	Entity->State.SetSound (0);
}

void CMonster::FliesOn ()
{
	if (Entity->gameEntity->waterlevel)
		return;
	Entity->State.AddEffects (EF_FLIES);
	Entity->State.SetSound (SoundIndex ("infantry/inflies1.wav"));
	Think = &CMonster::FliesOff;
	Entity->NextThink = level.framenum + 600;
}

void CMonster::CheckFlies ()
{
	if (Entity->gameEntity->waterlevel)
		return;

	if (random() > 0.5)
		return;

	Think = &CMonster::FliesOn;
	Entity->NextThink = level.framenum + ((5 + 10 * random()) * 10);
}

static uint32 lastMonsterID;
CMonster::CMonster ()
{
	MonsterID = lastMonsterID++;
}

#ifdef MONSTER_USE_ROGUE_AI
void CMonster::DoneDodge ()
{
	AIFlags &= ~AI_DODGING;
}

void CMonster::SideStep ()
{
}

void CMonster::Dodge (edict_t *attacker, float eta, CTrace *tr)
{
	float	r = random();
	float	height;
	bool	ducker = false, dodger = false;

	// this needs to be here since this can be called after the monster has "died"
	if (Entity->gameEntity->health < 1)
		return;

	if ((MonsterFlags & MF_HAS_DUCK) && (MonsterFlags & MF_HAS_UNDUCK))
		ducker = true;
	if ((MonsterFlags & MF_HAS_SIDESTEP) && !(AIFlags & AI_STAND_GROUND))
		dodger = true;

	if ((!ducker) && (!dodger))
		return;

	if (!Entity->gameEntity->enemy)
	{
		Entity->gameEntity->enemy = attacker;
		FoundTarget ();
	}

	// PMM - don't bother if it's going to hit anyway; fix for weird in-your-face etas (I was
	// seeing numbers like 13 and 14)
	if ((eta < 0.1) || (eta > 5))
	{
//		if ((g_showlogic) && (g_showlogic->value))
//			gi.dprintf ("timeout\n");
		return;
	}

	// skill level determination..
	if (r > (0.25*((skill->Integer())+1)))
	{
		return;
	}

	// stop charging, since we're going to dodge (somehow) instead
//	soldier_stop_charge (self);

	if (ducker)
	{
		height = Entity->GetAbsMax().Z-32-1;  // the -1 is because the absmax is s.origin + maxs + 1

		// FIXME, make smarter
		// if we only duck, and ducking won't help or we're already ducking, do nothing
		//
		// need to add monsterinfo.abort_duck() and monsterinfo.next_duck_time

		if ((!dodger) && ((tr->endPos[2] <= height) || (AIFlags & AI_DUCKED)))
			return;
	}
	else
		height = Entity->GetAbsMax().Z;

	if (dodger)
	{
		// if we're already dodging, just finish the sequence, i.e. don't do anything else
		if (AIFlags & AI_DODGING)
		{
//			if ((g_showlogic) && (g_showlogic->value))
//				gi.dprintf ("already dodging\n");
			return;
		}

		// if we're ducking already, or the shot is at our knees
		if ((tr->endPos[2] <= height) || (AIFlags & AI_DUCKED))
		{
			vec3_t right, diff;
			vec3_t angles, origin;

			Entity->State.GetAngles (angles);
			Entity->State.GetOrigin (origin);

			Angles_Vectors (angles, NULL, right, NULL);
			Vec3Subtract (tr->endPos, origin, diff);

			if (Dot3Product (right, diff) < 0)
				Lefty = false;
			else
				Lefty = true;
	
			// if we are currently ducked, unduck

			if ((ducker) && (AIFlags & AI_DUCKED))
			{
//				if ((g_showlogic) && (g_showlogic->value))
//					gi.dprintf ("unducking - ");
				UnDuck();
			}

			AIFlags |= AI_DODGING;
			AttackState = AS_SLIDING;

			// call the monster specific code here
			SideStep ();
			return;
		}
	}

	if (ducker)
	{
		if (NextDuckTime > level.framenum)
		{
//			if ((g_showlogic) && (g_showlogic->value))
//				gi.dprintf ("ducked too often, not ducking\n");
			return;
		}

//		if ((g_showlogic) && (g_showlogic->value))
//			gi.dprintf ("ducking!\n");

		DoneDodge ();
		// set this prematurely; it doesn't hurt, and prevents extra iterations
		AIFlags |= AI_DUCKED;

		Duck (eta);
	}
}

void CMonster::DuckDown ()
{
	AIFlags |= AI_DUCKED;

	vec3f tempMaxs = Entity->GetMaxs();
	tempMaxs.Z = BaseHeight - 32;
	Entity->SetMaxs(tempMaxs);
	Entity->gameEntity->takedamage = true;
	if (DuckWaitTime < level.framenum)
		DuckWaitTime = level.framenum + 10;
	Entity->Link ();
}

void CMonster::DuckHold ()
{
	if (level.framenum >= DuckWaitTime)
		AIFlags &= ~AI_HOLD_FRAME;
	else
		AIFlags |= AI_HOLD_FRAME;
}

void CMonster::Duck (float ETA)
{
}

void CMonster::UnDuck ()
{
	AIFlags &= ~AI_DUCKED;

	vec3f tempMaxs = Entity->GetMaxs();
	tempMaxs.Z = BaseHeight;
	Entity->SetMaxs(tempMaxs);
	Entity->gameEntity->takedamage = true;
	NextDuckTime = level.framenum + 5;
	Entity->Link ();
}
#endif