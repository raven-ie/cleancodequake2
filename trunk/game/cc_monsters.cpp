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

	P_CurrentNodeIndex = P_CurrentPath->Path.size()-1;

	// If our first node is behind us and it's not too far away, we can
	// just skip this node and go to the next one.	
	if (VecInFront(Entity->state.angles, Entity->state.origin, P_CurrentPath->Path[P_CurrentNodeIndex]->Origin))
	//{
		P_CurrentNodeIndex--;
		//P_CurrentPath->Path.pop_back();
		//gi.dprintf ("TEST\n");
	//}

	P_CurrentNode = P_CurrentPath->Path[P_CurrentNodeIndex];

	float timeOut = level.framenum + 20; // Always at least 2 seconds
	// Calculate approximate distance and check how long we want this to time out for
	switch (rangevector(Entity->state.origin, P_CurrentNode->Origin))
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

void plat_go_up (edict_t *ent);
void CMonster::MoveToPath (float Dist)
{
	if (!Entity->groundentity && !(Entity->flags & (FL_FLY|FL_SWIM)))
		return;

	if (FindTarget() && (Entity->enemy && visible(Entity, Entity->enemy))) // Did we find an enemy while going to our path?
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
	Vec3Subtract (Entity->state.origin, P_CurrentNode->Origin, sub);

	CTempEnt_Trails::FleshCable (Entity->state.origin, P_CurrentNode->Origin, Entity-g_edicts);
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
			if (P_CurrentNode->Type == NODE_DOOR || P_CurrentNode->Type == NODE_PLATFORM)
			{
				// If we reached the node, but the platform isn't down, go back two nodes
				if (P_CurrentNode->Type == NODE_PLATFORM && P_CurrentNode->LinkedEntity->moveinfo.state != STATE_BOTTOM)
				{
					if (P_CurrentPath->Path.size() >= (uint32)(P_CurrentNodeIndex + 2)) // Can we even go that far?
					{
						P_CurrentNodeIndex += 2;
						P_CurrentNode = P_CurrentPath->Path[P_CurrentNodeIndex];
						DebugPrintf ("Plat in bad spot: going back to %u\n", P_CurrentNodeIndex);
					}
				}
				else
				{
					//P_CurrentNode->LinkedEntity->use (P_CurrentNode->LinkedEntity, Entity, Entity);
					if (P_CurrentNode->Type == NODE_PLATFORM)
					{
						if (P_CurrentNode->LinkedEntity->moveinfo.state == STATE_BOTTOM)
							plat_go_up (P_CurrentNode->LinkedEntity);
						else if (P_CurrentNode->LinkedEntity->moveinfo.state == STATE_TOP)
							P_CurrentNode->LinkedEntity->nextthink = level.framenum + 10;	// the player is still on the plat, so delay going down
					}
					else
						P_CurrentNode->LinkedEntity->use (P_CurrentNode->LinkedEntity, Entity, Entity);
					Stand (); // We stand, and wait.
				}
			}

			if (P_CurrentNodeIndex > 1) // If we have two more goals to destination
			{
				// In two goals, do we reach the platform node?
				if (P_CurrentPath->Path[P_CurrentNodeIndex-1]->Type == NODE_PLATFORM)
				{
					// Is it at bottom?
					if (P_CurrentPath->Path[P_CurrentNodeIndex-1]->LinkedEntity->moveinfo.state != STATE_BOTTOM)
						Stand (); // We wait till it comes down
				}
			}

			if (shouldJump)
			{
				vec3_t sub2, forward;
				Vec3Subtract (P_CurrentNode->Origin, Entity->state.origin, sub2);
				Angles_Vectors (sub2, forward, NULL, NULL);
				Vec3MA (Entity->velocity, 1.5, sub2, Entity->velocity);
				Entity->velocity[2] = 300;
				Entity->groundentity = NULL;
				CheckGround();
			}
		}
		else
		{
			if (Entity->enemy)
			{
				vec3_t sub;
				Vec3Subtract (Entity->state.origin, Entity->enemy->state.origin, sub);

				if (Vec3Length(sub) < 250) // If we're still close enough that it's possible
					// to hear him breathing (lol), startback on the trail
				{
					P_CurrentPath = NULL;
					P_CurrentGoalNode = GetClosestNodeTo(Entity->enemy->state.origin);
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
		P_CurrentNode = GetClosestNodeTo(Entity->state.origin);
		P_CurrentGoalNode = End;
		P_CurrentPath = NULL;
		FoundPath ();

		int32 timeOut = level.framenum + 20; // Always at least 2 seconds
		// Calculate approximate distance and check how long we want this to time out for
		switch (rangevector(Entity->state.origin, P_CurrentNode->Origin))
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
		if (Entity->inUse)
		{
			float	deltax,deltay;
			vec3_t	d;
			float	tdir, olddir, turnaround;

			olddir = AngleModf ((int)(IdealYaw/45)*45);
			turnaround = AngleModf (olddir - 180);

			deltax = P_CurrentNode->Origin[0] - Entity->state.origin[0];
			deltay = P_CurrentNode->Origin[1] - Entity->state.origin[1];
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
	edict_t	*ent;
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
		ent = &g_edicts[check];
		if (ent->inUse
			&& ent->health > 0
			&& !(ent->flags & FL_NOTARGET) )
		{
			level.sight_client = dynamic_cast<CPlayerEntity*>(ent->Entity);
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
	dot = DotProduct (vec, forward);
	
	if (dot > 0.3)
		return true;
	return false;
}


void Monster_Think (edict_t *ent)
{
	if (!ent->Monster)
		return;
	ent->nextthink = level.framenum + 1;

	CMonster *Monster = ent->Monster;

	int32 	thinktime = Monster->NextThink;
	if (thinktime > level.framenum)
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
	float current = AngleModf (Entity->state.angles[YAW]);

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
	
	Entity->state.angles[YAW] = AngleModf (current + move);
}

bool CMonster::CheckBottom ()
{
	vec3_t	mins, maxs, start, stop;
	CTrace	trace;
	int		x, y;
	float	mid, bottom;
	
	Vec3Add (Entity->state.origin, Entity->mins, mins);
	Vec3Add (Entity->state.origin, Entity->maxs, maxs);

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
	trace = CTrace(start, stop, Entity, CONTENTS_MASK_MONSTERSOLID);

	if (trace.fraction == 1.0)
		return false;
	mid = bottom = trace.endPos[2];
	
// the corners must be within 16 of the midpoint	
	for	(x=0 ; x<=1 ; x++)
		for	(y=0 ; y<=1 ; y++)
		{
			start[0] = stop[0] = x ? maxs[0] : mins[0];
			start[1] = stop[1] = y ? maxs[1] : mins[1];
			
			trace = CTrace(start, stop, Entity, CONTENTS_MASK_MONSTERSOLID);
			
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
	Vec3Copy (Entity->state.origin, oldorg);
	Vec3Add (Entity->state.origin, move, neworg);

// flying monsters don't step up
	if ( Entity->flags & (FL_SWIM | FL_FLY) )
	{
	// try one move with vertical motion, then one without
		for (i=0 ; i<2 ; i++)
		{
			Vec3Add (Entity->state.origin, move, neworg);
			if (i == 0 && Entity->enemy)
			{
				if (!Entity->goalentity)
					Entity->goalentity = Entity->enemy;
				dz = Entity->state.origin[2] - Entity->goalentity->state.origin[2];
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
			trace = CTrace(Entity->state.origin, Entity->mins, Entity->maxs, neworg, Entity, CONTENTS_MASK_MONSTERSOLID);
	
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
				Vec3Copy (trace.endPos, Entity->state.origin);
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

	trace = CTrace(neworg, Entity->mins, Entity->maxs, end, Entity, CONTENTS_MASK_MONSTERSOLID);

	if (trace.allSolid)
		return false;

	if (trace.startSolid)
	{
		neworg[2] -= stepsize;
		trace = CTrace(neworg, Entity->mins, Entity->maxs, end, Entity, CONTENTS_MASK_MONSTERSOLID);
		if (trace.allSolid || trace.startSolid)
			return false;
	}


	// don't go in to water
	/*if (Entity->waterlevel == 0)
	{
		test[0] = trace.endPos[0];
		test[1] = trace.endPos[1];
		test[2] = trace.endPos[2] + Entity->mins[2] + 1;	
		contents = gi.pointcontents(test);

		if (contents & CONTENTS_MASK_WATER)
			return false;
	}*/

	if (trace.fraction == 1)
	{
	// if monster had the ground pulled out, go ahead and fall
		if ( Entity->flags & FL_PARTIALGROUND )
		{
			Vec3Add (Entity->state.origin, move, Entity->state.origin);
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
	Vec3Copy (trace.endPos, Entity->state.origin);
	
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
		// Check to see if there's floor just after this
		vec3_t org, up, end2;
		Angles_Vectors (Entity->state.angles, NULL, NULL, up);
		Vec3Add (Entity->state.origin, move, org);

		// Go down
		Vec3MA (org, -STEPSIZE, up, end2);

		// Trace
		trace = CTrace (org, end2, Entity, CONTENTS_MASK_SOLID);

		// Couldn't make the move
		if (trace.fraction == 1.0)
		{
			Vec3Copy (oldorg, Entity->state.origin);
			return false;
		}
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

	deltax = Enemy->state.origin[0] - Entity->state.origin[0];
	deltay = Enemy->state.origin[1] - Entity->state.origin[1];
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

	Vec3Copy (Entity->state.origin, oldorigin);
	if (MoveStep (move, false))
	{
		delta = Entity->state.angles[YAW] - IdealYaw;
		// not turned far enough, so don't take the step
		if (delta > 45 && delta < 315)
			Vec3Copy (oldorigin, Entity->state.origin);
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
	if (!(Entity->spawnflags & 2) && level.framenum < 10)
	{
		DropToFloor ();

		if (Entity->groundentity)
		{
			if (!WalkMove (0, 0))
				MapPrint (MAPPRINT_WARNING, Entity, Entity->state.origin, "In solid\n");
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
	NextThink = level.framenum + FRAMETIME;
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
		MapPrint (MAPPRINT_WARNING, Entity, Entity->state.origin, "Entity in solid\n");

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
			MapPrint (MAPPRINT_WARNING, Entity, Entity->state.origin, "Target with mixed types\n");
			//gi.dprintf("%s at (%f %f %f) has target with mixed types\n", self->classname, self->state.origin[0], self->state.origin[1], self->state.origin[2]);
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
				MapPrint (MAPPRINT_WARNING, Entity, Entity->state.origin, "Has a bad combattarget (\"%s\")\n", Entity->combattarget);
		}
	}

	if (Entity->target)
	{
		Entity->goalentity = Entity->movetarget = G_PickTarget(Entity->target);
		if (!Entity->movetarget)
		{
			//gi.dprintf ("%s can't find target %s at (%f %f %f)\n", self->classname, self->target, self->state.origin[0], self->state.origin[1], self->state.origin[2]);
			MapPrint (MAPPRINT_WARNING, Entity, Entity->state.origin, "Can't find target\n");
			Entity->target = NULL;
			PauseTime = 100000000;
			Stand ();
		}
		else if (strcmp (Entity->movetarget->classname, "path_corner") == 0)
		{
			Vec3Subtract (Entity->goalentity->state.origin, Entity->state.origin, v);
			IdealYaw = Entity->state.angles[YAW] = VecToYaw(v);
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
	NextThink = level.framenum + FRAMETIME;
}

void CMonster::MonsterStart ()
{
	if (game.mode & GAME_DEATHMATCH)
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

	NextThink = level.framenum + FRAMETIME;
	Entity->svFlags |= SVF_MONSTER;
	Entity->state.renderFx |= RF_FRAMELERP;
	Entity->takedamage = DAMAGE_AIM;
	Entity->air_finished = level.framenum + 120;
	Entity->use = &CMonster::MonsterUse;
	Entity->max_health = Entity->health;
	Entity->clipMask = CONTENTS_MASK_MONSTERSOLID;

	Entity->deadflag = DEAD_NO;
	Entity->svFlags &= ~SVF_DEADMONSTER;

	Vec3Copy (Entity->state.origin, Entity->state.oldOrigin);

	if (st.item)
	{
		Entity->item = FindItemByClassname (st.item);
		if (!Entity->item)
			MapPrint (MAPPRINT_WARNING, Entity, Entity->state.origin, "Bad item: \"%s\"\n", st.item);
			//gi.dprintf("%s at (%f %f %f) has bad item: %s\n", self->classname, self->state.origin[0], self->state.origin[1], self->state.origin[2], st.item);
	}

	// randomize what frame they start on
	if (CurrentMove)
		Entity->state.frame = CurrentMove->FirstFrame + (rand() % (CurrentMove->LastFrame - CurrentMove->FirstFrame + 1));

#ifdef MONSTER_USE_ROGUE_AI
	BaseHeight = Entity->maxs[2];
#endif

	Entity->think = Monster_Think;
	Entity->nextthink = level.framenum + FRAMETIME;
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
	self->Monster->NextThink = level.framenum + FRAMETIME;
	if (activator->client)
		self->enemy = activator;
	self->use = &CMonster::MonsterUse;
}

void CMonster::MonsterTriggeredSpawn ()
{
	Entity->state.origin[2] += 1;
	KillBox (Entity);

	Entity->solid = SOLID_BBOX;
	Entity->movetype = MOVETYPE_STEP;
	Entity->svFlags &= ~SVF_NOCLIENT;
	Entity->air_finished = level.framenum + 120;
	gi.linkentity (Entity);

	MonsterStartGo ();

	if (Entity->enemy && !(Entity->spawnflags & 1) && !(Entity->enemy->flags & FL_NOTARGET))
		FoundTarget ();
	else
		Entity->enemy = NULL;
}

#ifdef MONSTERS_ARENT_STUPID
bool CMonster::FriendlyInLine (vec3_t Origin, vec3_t Direction)
{
	vec3_t dir, forward, end;
	VecToAngles (Direction, dir);
	Angles_Vectors (dir, forward, NULL, NULL);

	Vec3MA (Origin, 8192, forward, end);
	CTrace trace = CTrace(Origin, end, Entity, CONTENTS_MONSTER);

	if (trace.fraction <= 0.5 && trace.ent && trace.ent->Monster && (trace.ent->enemy != Entity))
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

	if (Entity->enemy->flags & FL_NOTARGET)
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

	while ( (strogg = findradius (strogg, Entity->state.origin, dist)) != NULL)
	{
		if (strogg->health < 1 || !(strogg->takedamage))
			continue;

		if (strogg == Entity)
			continue;

		if (strogg->client || !strogg->Monster)
			continue;

		if (strogg->enemy)
			continue;
		
#ifdef MONSTERS_USE_PATHFINDING
		// Set us up for pathing
		P_CurrentNode = GetClosestNodeTo(strogg->state.origin);
		P_CurrentGoalNode = GetClosestNodeTo(Entity->enemy->state.origin);
		strogg->Monster->FoundPath ();
#else
		//strogg->enemy = Entity->enemy;
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

	fire_bullet (Entity, start, dir, damage, kick, hspread, vspread, MOD_UNKNOWN);

	if (flashtype != -1)
		CTempEnt::MonsterFlash (start, Entity - g_edicts, flashtype);
}

void CMonster::MonsterFireShotgun (vec3_t start, vec3_t aimdir, int damage, int kick, int hspread, int vspread, int count, int flashtype)
{
#ifdef MONSTERS_ARENT_STUPID
	if (FriendlyInLine (start, aimdir))
		return;
#endif

	fire_shotgun (Entity, start, aimdir, damage, kick, hspread, vspread, count, MOD_UNKNOWN);

	if (flashtype != -1)
		CTempEnt::MonsterFlash (start, Entity - g_edicts, flashtype);
}

void CMonster::MonsterFireBlaster (vec3_t start, vec3_t dir, int damage, int speed, int flashtype, int effect)
{
#ifdef MONSTERS_ARENT_STUPID
	if (FriendlyInLine (start, dir))
		return;
#endif

	fire_blaster (Entity, start, dir, damage, speed, effect, false);

	if (flashtype != -1)
		CTempEnt::MonsterFlash (start, Entity - g_edicts, flashtype);
}	

void CMonster::MonsterFireGrenade (vec3_t start, vec3_t aimdir, int damage, int speed, int flashtype)
{
#ifdef MONSTERS_ARENT_STUPID
	if (FriendlyInLine (start, aimdir))
		return;
#endif

	fire_grenade (Entity, start, aimdir, damage, speed, 2.5, damage+40);

	if (flashtype != -1)
		CTempEnt::MonsterFlash (start, Entity - g_edicts, flashtype);
}

void CMonster::MonsterFireRocket (vec3_t start, vec3_t dir, int damage, int speed, int flashtype)
{
#ifdef MONSTERS_ARENT_STUPID
	if (FriendlyInLine (start, dir))
		return;
#endif

	fire_rocket (Entity, start, dir, damage, speed, damage+20, damage);

	if (flashtype != -1)
		CTempEnt::MonsterFlash (start, Entity - g_edicts, flashtype);
}	

void CMonster::MonsterFireRailgun (vec3_t start, vec3_t aimdir, int damage, int kick, int flashtype)
{
#ifdef MONSTERS_ARENT_STUPID
	if (FriendlyInLine (start, aimdir))
		return;
#endif

	if (!(gi.pointcontents (start) & CONTENTS_MASK_SOLID))
		fire_rail (Entity, start, aimdir, damage, kick);

	if (flashtype != -1)
		CTempEnt::MonsterFlash (start, Entity - g_edicts, flashtype);
}

void CMonster::MonsterFireBfg (vec3_t start, vec3_t aimdir, int damage, int speed, int kick, float damage_radius, int flashtype)
{
#ifdef MONSTERS_ARENT_STUPID
	if (FriendlyInLine (start, aimdir))
		return;
#endif

	fire_bfg (Entity, start, aimdir, damage, speed, damage_radius);

	if (flashtype != -1)
		CTempEnt::MonsterFlash (start, Entity - g_edicts, flashtype);
}

bool CMonster::CheckAttack ()
{
#ifndef MONSTER_USE_ROGUE_AI
	vec3_t	spot1, spot2;
	float	chance;
	CTrace	tr;

	if (Entity->enemy->health > 0)
	{
	// see if any entities are in the way of the shot
		Vec3Copy (Entity->state.origin, spot1);
		spot1[2] += Entity->viewheight;
		Vec3Copy (Entity->enemy->state.origin, spot2);
		spot2[2] += Entity->enemy->viewheight;

		tr = CTrace(spot1, spot2, Entity, CONTENTS_SOLID|CONTENTS_MONSTER|CONTENTS_SLIME|CONTENTS_LAVA|CONTENTS_WINDOW);

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

	if (Entity->flags & FL_FLY)
	{
		if (random() < 0.3)
			AttackState = AS_SLIDING;
		else
			AttackState = AS_STRAIGHT;
	}

	return false;
#else
	vec3_t	spot1, spot2;
	float	chance;
	CTrace	tr;

	if (Entity->enemy->health > 0)
	{
	// see if any entities are in the way of the shot
		Vec3Copy (Entity->state.origin, spot1);
		spot1[2] += Entity->viewheight;
		Vec3Copy (Entity->enemy->state.origin, spot2);
		spot2[2] += Entity->enemy->viewheight;

		tr = CTrace (spot1, spot2, Entity, CONTENTS_SOLID|CONTENTS_MONSTER|CONTENTS_SLIME|CONTENTS_LAVA|CONTENTS_WINDOW);

		// do we have a clear shot?
		if (tr.ent != Entity->enemy)
		{	
			// PGM - we want them to go ahead and shoot at info_notnulls if they can.
			if(Entity->enemy->solid != SOLID_NOT || tr.fraction < 1.0)		//PGM
			{
				// PMM - if we can't see our target, and we're not blocked by a monster, go into blind fire if available
				if ((!(tr.ent->svFlags & SVF_MONSTER)) && (!visible(Entity, Entity->enemy)))
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
							tr = CTrace (spot1, BlindFireTarget, Entity, CONTENTS_MONSTER);
							if (tr.allSolid || tr.startSolid || ((tr.fraction < 1.0) && (tr.ent != Entity->enemy)))
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
	if ((random () < chance) || (Entity->enemy->solid == SOLID_NOT))
	{
		AttackState = AS_MISSILE;
		AttackFinished = level.framenum + ((2*random())*10);
		return true;
	}

	// PMM -daedalus should strafe more .. this can be done here or in a customized
	// check_attack code for the hover.
	if (Entity->flags & FL_FLY)
	{
		// originally, just 0.3
		float strafe_chance;
		if (!(strcmp(Entity->classname, "monster_daedalus")))
			strafe_chance = 0.8f;
		else
			strafe_chance = 0.6f;

		// if enemy is tesla, never strafe
		if ((Entity->enemy) && (Entity->enemy->classname) && (!strcmp(Entity->enemy->classname, "tesla")))
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

	Entity->state.origin[2] += 1;
	Vec3Copy (Entity->state.origin, end);
	end[2] -= 256;
	
	trace = CTrace (Entity->state.origin, Entity->mins, Entity->maxs, end, Entity, CONTENTS_MASK_MONSTERSOLID);

	if (trace.fraction == 1 || trace.allSolid)
		return;

	Vec3Copy (trace.endPos, Entity->state.origin);

	gi.linkentity (Entity);
	CheckGround ();
	CatagorizePosition ();
}

void CMonster::AI_Charge(float Dist)
{
#ifndef MONSTER_USE_ROGUE_AI
	vec3_t	v;

	Vec3Subtract (Entity->enemy->state.origin, Entity->state.origin, v);
	IdealYaw = VecToYaw(v);
	ChangeYaw ();

	if (Dist)
		WalkMove (Entity->state.angles[YAW], Dist);
#else
	vec3_t	v;
	// PMM
	float	ofs;
	// PMM

	// PMM - made AI_MANUAL_STEERING affect things differently here .. they turn, but
	// don't set the ideal_yaw

	// This is put in there so monsters won't move towards the origin after killing
	// a tesla. This could be problematic, so keep an eye on it.
	if(!Entity->enemy || !Entity->enemy->inUse)		//PGM
		return;									//PGM

	// PMM - save blindfire target
	if (visible(Entity, Entity->enemy))
		Vec3Copy (Entity->enemy->state.origin, BlindFireTarget);
	// pmm 

	if (!(AIFlags & AI_MANUAL_STEERING))
	{
		Vec3Subtract (Entity->enemy->state.origin, Entity->state.origin, v);
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
			// if we're fighting a tesla, NEVER circle strafe
			if ((Entity->enemy) && (Entity->enemy->classname) && (!strcmp(Entity->enemy->classname, "tesla")))
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
			WalkMove (Entity->state.angles[YAW], Dist);
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
	if (Entity->goalentity)
	{
		if (AIFlags & AI_COMBAT_POINT)
			return false;

		if (AIFlags & AI_SOUND_TARGET)
		{
			if (Entity->enemy)
			{
				if ((level.framenum - Entity->enemy->teleport_time) > 50)
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
					Entity->show_hostile = level.framenum + 10;
					return false;
				}
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
				PauseTime = level.framenum + 100000000;
				Stand ();
			}
			return true;
		}
	}

	Entity->show_hostile = level.framenum + 10;		// wake up other monsters

// check knowledge of enemy
	EnemyVis = visible(Entity, Entity->enemy);
	if (EnemyVis)
	{
		SearchTime = level.framenum + 50;
		Vec3Copy (Entity->enemy->state.origin, LastSighting);
	}

	EnemyInfront = infront(Entity, Entity->enemy);
	EnemyRange = range(Entity, Entity->enemy);
	Vec3Subtract (Entity->enemy->state.origin, Entity->state.origin, temp);
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
	if (Entity->goalentity)
	{
		if (AIFlags & AI_COMBAT_POINT)
			return false;

		if (AIFlags & AI_SOUND_TARGET)
		{
			if ((level.framenum - Entity->enemy->teleport_time) > 50)
			{
				if (Entity->goalentity == Entity->enemy)
				{
					if (Entity->movetarget)
						Entity->goalentity = Entity->movetarget;
					else
						Entity->goalentity = NULL;
				}

				AIFlags &= ~AI_SOUND_TARGET;
				if (AIFlags & AI_TEMP_STAND_GROUND)
					AIFlags &= ~(AI_STAND_GROUND | AI_TEMP_STAND_GROUND);
			}
			else
			{
				Entity->show_hostile = level.framenum + 10;
				return false;
			}
		}
	}

	EnemyVis = false;

// see if the enemy is dead
	hesDeadJim = false;
	if ((!Entity->enemy) || (!Entity->enemy->inUse))
	{
		hesDeadJim = true;
	}
	else if (AIFlags & AI_MEDIC)
	{
		if (!(Entity->enemy->inUse) || (Entity->enemy->health > 0))
		{
			hesDeadJim = true;
//			self->monsterinfo.aiflags &= ~AI_MEDIC;
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
		AIFlags &= ~AI_MEDIC;
		Entity->enemy = NULL;
	// FIXME: look all around for other targets
		if (Entity->oldenemy && Entity->oldenemy->health > 0)
		{
			Entity->enemy = Entity->oldenemy;
			Entity->oldenemy = NULL;
			HuntTarget ();
		}
//ROGUE - multiple teslas make monsters lose track of the player.
		else if(LastPlayerEnemy && LastPlayerEnemy->health > 0)
		{
//			if ((g_showlogic) && (g_showlogic->value))
//				gi.dprintf("resorting to last_player_enemy...\n");
			Entity->enemy = LastPlayerEnemy;
			Entity->oldenemy = NULL;
			LastPlayerEnemy = NULL;
			HuntTarget ();
		}
//ROGUE
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
				PauseTime = level.framenum + 100000000;
				Stand ();
			}
			return true;
		}
	}

	Entity->show_hostile = level.framenum + 10;		// wake up other monsters

// check knowledge of enemy
	EnemyVis = visible(Entity, Entity->enemy);
	if (EnemyVis)
	{
		SearchTime = level.framenum + 50;
		Vec3Copy (Entity->enemy->state.origin, LastSighting);
		// PMM
		AIFlags &= ~AI_LOST_SIGHT;
		TrailTime = level.framenum;
		Vec3Copy (Entity->enemy->state.origin, BlindFireTarget);
		BlindFireDelay = 0;
		// pmm
	}

	EnemyInfront = infront(Entity, Entity->enemy);
	EnemyRange = range(Entity, Entity->enemy);
	Vec3Subtract (Entity->enemy->state.origin, Entity->state.origin, temp);
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
	WalkMove (Entity->state.angles[YAW], Dist);
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
		if (Entity->enemy)
		{
			Vec3Subtract (Entity->state.origin, Entity->enemy->state.origin, v);
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
		Vec3Copy (Entity->enemy->state.origin, LastSighting);
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

	save = Entity->goalentity;
	tempgoal = G_Spawn();
	Entity->goalentity = tempgoal;

	isNew = false;

	if (!(AIFlags & AI_LOST_SIGHT))
	{
		// just lost sight of the player, decide where to go first
		//gi.dprintf("lost sight of player, last seen at %f %f %f\n", LastSighting[0], LastSighting[1], LastSighting[2]);

#ifdef MONSTERS_USE_PATHFINDING
		// Set us up for pathing
		P_CurrentNode = GetClosestNodeTo(Entity->state.origin);
		P_CurrentGoalNode = GetClosestNodeTo(Entity->enemy->state.origin);
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
			Entity->state.angles[YAW] = IdealYaw = marker->state.angles[YAW];
//			dprint("heading is "); dprint(ftos(self.ideal_yaw)); dprint("\n");

//			debug_drawline(self.origin, self.last_sighting, 52);
			isNew = true;
		}
	}

	Vec3Subtract (Entity->state.origin, LastSighting, v);
	d1 = Vec3Length(v);
	if (d1 <= Dist)
	{
		AIFlags |= AI_PURSUE_NEXT;
		Dist = d1;
	}

	Vec3Copy (LastSighting, Entity->goalentity->state.origin);

	if (isNew)
	{
//		gi.dprintf("checking for course correction\n");

		tr = CTrace (Entity->state.origin, Entity->mins, Entity->maxs, LastSighting, Entity, CONTENTS_MASK_PLAYERSOLID);
		if (tr.fraction < 1)
		{
			Vec3Subtract (Entity->goalentity->state.origin, Entity->state.origin, v);
			d1 = Vec3Length(v);
			center = tr.fraction;
			d2 = d1 * ((center+1)/2);
			Entity->state.angles[YAW] = IdealYaw = VecToYaw(v);
			Angles_Vectors(Entity->state.angles, v_forward, v_right, NULL);

			Vec3Set (v, d2, -16, 0);
			G_ProjectSource (Entity->state.origin, v, v_forward, v_right, left_target);
			tr = CTrace(Entity->state.origin, Entity->mins, Entity->maxs, left_target, Entity, CONTENTS_MASK_PLAYERSOLID);
			left = tr.fraction;

			Vec3Set (v, d2, 16, 0);
			G_ProjectSource (Entity->state.origin, v, v_forward, v_right, right_target);
			tr = CTrace(Entity->state.origin, Entity->mins, Entity->maxs, right_target, Entity, CONTENTS_MASK_PLAYERSOLID);
			right = tr.fraction;

			center = (d1*center)/d2;
			if (left >= center && left > right)
			{
				if (left < 1)
				{
					Vec3Set (v, d2 * left * 0.5, -16, 0);
					G_ProjectSource (Entity->state.origin, v, v_forward, v_right, left_target);
//					gi.dprintf("incomplete path, go part way and adjust again\n");
				}
				Vec3Copy (LastSighting, SavedGoal);
				AIFlags |= AI_PURSUE_TEMP;
				Vec3Copy (left_target, Entity->goalentity->state.origin);
				Vec3Copy (left_target, LastSighting);
				Vec3Subtract (Entity->goalentity->state.origin, Entity->state.origin, v);
				Entity->state.angles[YAW] = IdealYaw = VecToYaw(v);
//				gi.dprintf("adjusted left\n");
//				debug_drawline(self.origin, self.last_sighting, 152);
			}
			else if (right >= center && right > left)
			{
				if (right < 1) {
					Vec3Set (v, d2 * right * 0.5, 16, 0);
					G_ProjectSource (Entity->state.origin, v, v_forward, v_right, right_target);
//					gi.dprintf("incomplete path, go part way and adjust again\n");
				}
				Vec3Copy (LastSighting, SavedGoal);
				AIFlags |= AI_PURSUE_TEMP;
				Vec3Copy (right_target, Entity->goalentity->state.origin);
				Vec3Copy (right_target, LastSighting);
				Vec3Subtract (Entity->goalentity->state.origin, Entity->state.origin, v);
				Entity->state.angles[YAW] = IdealYaw = VecToYaw(v);
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
#else
	vec3_t		v;
	edict_t		*tempgoal;
	edict_t		*save;
	bool	isNew;
	edict_t		*marker;
	float		d1, d2;
	CTrace		tr;
	vec3_t		v_forward, v_right;
	float		left, center, right;
	vec3_t		left_target, right_target;
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
	if (Entity->maxs[2] != BaseHeight)
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

	if (AIFlags & AI_SOUND_TARGET)
	{
		// PMM - paranoia checking
		if (Entity->enemy)
			Vec3Subtract (Entity->state.origin, Entity->enemy->state.origin, v);

		if ((!Entity->enemy) || (Vec3Length(v) < 64))
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
		if(!Entity->inUse)
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

		if ((Entity->enemy) && (Entity->enemy->inUse) && (EnemyVis))
		{
			AIFlags &= ~AI_LOST_SIGHT;
			Vec3Copy (Entity->enemy->state.origin, LastSighting);
			TrailTime = level.framenum;
			//PMM
			Vec3Copy (Entity->enemy->state.origin, BlindFireTarget);
			BlindFireDelay = 0;
			//pmm
		}
		return;
	}
	//PMM

	// PGM - added a little paranoia checking here... 9/22/98
	if ((Entity->enemy) && (Entity->enemy->inUse) && (EnemyVis))
	{
		// PMM - check for alreadyMoved
		if (!alreadyMoved)
			MoveToGoal (Dist);
		if(!Entity->inUse)
			return;			// PGM - g_touchtrigger free problem

		AIFlags &= ~AI_LOST_SIGHT;
		Vec3Copy (Entity->enemy->state.origin, LastSighting);
		TrailTime = level.framenum;
		// PMM
		Vec3Copy (Entity->enemy->state.origin, BlindFireTarget);
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

	save = Entity->goalentity;
	tempgoal = G_Spawn();
	Entity->goalentity = tempgoal;

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
		P_CurrentNode = GetClosestNodeTo(Entity->state.origin);
		P_CurrentGoalNode = GetClosestNodeTo(Entity->enemy->state.origin);
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
			Entity->state.angles[YAW] = IdealYaw = marker->state.angles[YAW];

			isNew = true;
		}
	}

	Vec3Subtract (Entity->state.origin, LastSighting, v);
	d1 = Vec3Length(v);
	if (d1 <= Dist)
	{
		AIFlags |= AI_PURSUE_NEXT;
		Dist = d1;
	}

	Vec3Copy (LastSighting, Entity->goalentity->state.origin);

	if (isNew)
	{
		tr = CTrace(Entity->state.origin, Entity->mins, Entity->maxs, LastSighting, Entity, CONTENTS_MASK_PLAYERSOLID);
		if (tr.fraction < 1)
		{
			Vec3Subtract (Entity->goalentity->state.origin, Entity->state.origin, v);
			d1 = Vec3Length(v);
			center = tr.fraction;
			d2 = d1 * ((center+1)/2);
			Entity->state.angles[YAW] = IdealYaw = VecToYaw(v);
			Angles_Vectors(Entity->state.angles, v_forward, v_right, NULL);

			Vec3Set(v, d2, -16, 0);
			G_ProjectSource (Entity->state.origin, v, v_forward, v_right, left_target);
			tr = CTrace(Entity->state.origin, Entity->mins, Entity->maxs, left_target, Entity, CONTENTS_MASK_PLAYERSOLID);
			left = tr.fraction;

			Vec3Set(v, d2, 16, 0);
			G_ProjectSource (Entity->state.origin, v, v_forward, v_right, right_target);
			tr = CTrace(Entity->state.origin, Entity->mins, Entity->maxs, right_target, Entity, CONTENTS_MASK_PLAYERSOLID);
			right = tr.fraction;

			center = (d1*center)/d2;
			if (left >= center && left > right)
			{
				if (left < 1)
				{
					Vec3Set(v, d2 * left * 0.5, -16, 0);
					G_ProjectSource (Entity->state.origin, v, v_forward, v_right, left_target);
				}
				Vec3Copy (LastSighting, SavedGoal);
				AIFlags |= AI_PURSUE_TEMP;
				Vec3Copy (left_target, Entity->goalentity->state.origin);
				Vec3Copy (left_target, LastSighting);
				Vec3Subtract (Entity->goalentity->state.origin, Entity->state.origin, v);
				Entity->state.angles[YAW] = IdealYaw = VecToYaw(v);
			}
			else if (right >= center && right > left)
			{
				if (right < 1)
				{
					Vec3Set(v, d2 * right * 0.5, 16, 0);
					G_ProjectSource (Entity->state.origin, v, v_forward, v_right, right_target);
				}
				Vec3Copy (LastSighting, SavedGoal);
				AIFlags |= AI_PURSUE_TEMP;
				Vec3Copy (right_target, Entity->goalentity->state.origin);
				Vec3Copy (right_target, LastSighting);
				Vec3Subtract (Entity->goalentity->state.origin, Entity->state.origin, v);
				Entity->state.angles[YAW] = IdealYaw = VecToYaw(v);
			}
		}
	}

	MoveToGoal (Dist);
	if(!Entity->inUse)
		return;			// PGM - g_touchtrigger free problem

	G_FreeEdict(tempgoal);

	if (Entity)
		Entity->goalentity = save;
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
	if (!(Entity->flags & FL_FLY))
		Dist = min (Dist, 8.0f);
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
		WalkMove (Entity->state.angles[YAW], Dist);

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
		if (Entity->enemy)
		{
			vec3_t v;
			Vec3Subtract (Entity->enemy->state.origin, Entity->state.origin, v);
			IdealYaw = VecToYaw(v);
			if (Entity->state.angles[YAW] != IdealYaw && AIFlags & AI_TEMP_STAND_GROUND)
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

	if (!(Entity->spawnflags & 1) && (MonsterFlags & MF_HAS_IDLE) && (level.framenum > IdleTime))
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
	vec3_t	v;
	// PMM
	bool retval;

	if (Dist)
		WalkMove (Entity->state.angles[YAW], Dist);

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
			// Is it at bottom?
			if (P_CurrentPath->Path[P_CurrentNodeIndex-1]->LinkedEntity->moveinfo.state == STATE_BOTTOM)
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
		if (Entity->enemy)
		{
			Vec3Subtract (Entity->enemy->state.origin, Entity->state.origin, v);
			IdealYaw = VecToYaw(v);
			if (Entity->state.angles[YAW] != IdealYaw && AIFlags & AI_TEMP_STAND_GROUND)
			{
				AIFlags &= ~(AI_STAND_GROUND | AI_TEMP_STAND_GROUND);
				Run ();
			}
			if (!(AIFlags & AI_MANUAL_STEERING))
				ChangeYaw ();
			// PMM
			// find out if we're going to be shooting
			retval = AI_CheckAttack ();
			// record sightings of player
			if ((Entity->enemy) && (Entity->enemy->inUse) && (visible(Entity, Entity->enemy)))
			{
				AIFlags &= ~AI_LOST_SIGHT;
				Vec3Copy (Entity->enemy->state.origin, LastSighting);
				Vec3Copy (Entity->enemy->state.origin, BlindFireTarget);
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

	if (!(Entity->spawnflags & 1) && (MonsterFlags & MF_HAS_IDLE) && (level.framenum > IdleTime))
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

#ifdef MONSTERS_ARENT_STUPID
	// Help our buddy!
	if (attacker->Monster && attacker->enemy && attacker->enemy != Entity)
	{
		if (Entity->enemy && Entity->enemy->client)
			Entity->oldenemy = Entity->enemy;
		Entity->enemy = attacker->enemy;
		if (!(AIFlags & AI_DUCKED))
			FoundTarget ();
	}
	return;
#else
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
	Entity->flags &= ~(FL_FLY|FL_SWIM);
	AIFlags &= AI_GOOD_GUY;

	if (Entity->item)
	{
		Entity->item->DropItem (Entity);
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
	Entity->nextthink = level.framenum + FRAMETIME;

	// Backwards animation support
	if (Move->FirstFrame > Move->LastFrame)
	{
		if ((NextFrame) && (NextFrame >= Move->LastFrame) && (NextFrame <= Move->FirstFrame))
		{
			Entity->state.frame = NextFrame;
			NextFrame = 0;
		}
		else
		{
			if (Entity->state.frame == Move->LastFrame)
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

			if (Entity->state.frame < Move->LastFrame || Entity->state.frame > Move->FirstFrame)
			{
				AIFlags &= ~AI_HOLD_FRAME;
				Entity->state.frame = Move->FirstFrame;
			}
			else
			{
				if (!(AIFlags & AI_HOLD_FRAME))
				{
					Entity->state.frame--;
					if (Entity->state.frame < Move->LastFrame)
						Entity->state.frame = Move->FirstFrame;
				}
			}
		}

		index = Move->FirstFrame - Entity->state.frame;

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
			Entity->state.frame = NextFrame;
			NextFrame = 0;
		}
		else
		{
			if (Entity->state.frame == Move->LastFrame)
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

			if (Entity->state.frame < Move->FirstFrame || Entity->state.frame > Move->LastFrame)
			{
				AIFlags &= ~AI_HOLD_FRAME;
				Entity->state.frame = Move->FirstFrame;
			}
			else
			{
				if (!(AIFlags & AI_HOLD_FRAME))
				{
					Entity->state.frame++;
					if (Entity->state.frame > Move->LastFrame)
						Entity->state.frame = Move->FirstFrame;
				}
			}
		}

		index = Entity->state.frame - Move->FirstFrame;

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
	if (Entity->enemy->client)
	{
		level.sight_entity = Entity;
		level.sight_entity_framenum = level.framenum;
		level.sight_entity->light_level = 128;
	}

	Entity->show_hostile = level.framenum + 10;		// wake up other monsters
#endif


	Vec3Copy(Entity->enemy->state.origin, LastSighting);
	TrailTime = level.framenum;

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
		MapPrint (MAPPRINT_ERROR, Entity, Entity->state.origin, "combattarget %s not found\n", Entity->combattarget);
		return;
	}

	// clear out our combattarget, these are a one shot deal
	Entity->combattarget = NULL;
	AIFlags |= AI_COMBAT_POINT;

	// clear the targetname, that point is ours!
	Entity->movetarget->targetname = NULL;
	PauseTime = 0;
#ifdef MONSTER_USE_ROGUE_AI
	// PMM
	Vec3Copy (Entity->enemy->state.origin, BlindFireTarget);
	BlindFireDelay = 0;
	// PMM
#endif

	// run for it
	Run ();
}

void CMonster::SetEffects()
{
	Entity->state.effects = 0;
	Entity->state.renderFx = RF_FRAMELERP;

	if (AIFlags & AI_RESURRECTING)
	{
		Entity->state.effects |= EF_COLOR_SHELL;
		Entity->state.renderFx |= RF_SHELL_RED;
	}

	if (Entity->health <= 0)
		return;

	if (Entity->powerarmor_time > level.framenum)
	{
		if (PowerArmorType == POWER_ARMOR_SCREEN)
			Entity->state.effects |= EF_POWERSCREEN;
		else if (PowerArmorType == POWER_ARMOR_SHIELD)
		{
			Entity->state.effects |= EF_COLOR_SHELL;
			Entity->state.renderFx |= RF_SHELL_GREEN;
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
				Entity->air_finished = level.framenum + 120;
			else if (Entity->air_finished < level.framenum)
			{
				if (Entity->pain_debounce_time < level.framenum)
				{
					int dmg = 2 + 2 * (level.framenum - Entity->air_finished);
					if (dmg > 15)
						dmg = 15;
					T_Damage (Entity, world, world, vec3Origin, Entity->state.origin, vec3Origin, dmg, 0, DAMAGE_NO_ARMOR, MOD_WATER);
					Entity->pain_debounce_time = level.framenum + 10;
				}
			}
		}
		else
		{
			if (Entity->waterlevel > 0)
				Entity->air_finished = level.framenum + 90;
			else if (Entity->air_finished < level.framenum)
			{	// suffocate!
				if (Entity->pain_debounce_time < level.framenum)
				{
					int dmg = 2 + 2 * (level.framenum - Entity->air_finished);
					if (dmg > 15)
						dmg = 15;
					T_Damage (Entity, world, world, vec3Origin, Entity->state.origin, vec3Origin, dmg, 0, DAMAGE_NO_ARMOR, MOD_WATER);
					Entity->pain_debounce_time = level.framenum + 10;
				}
			}
		}
	}
	
	if (Entity->waterlevel == 0)
	{
		if (Entity->flags & FL_INWATER)
		{	
			PlaySoundFrom (Entity, CHAN_BODY, SoundIndex("player/watr_out.wav"));
			Entity->flags &= ~FL_INWATER;
		}
		return;
	}

	if ((Entity->watertype & CONTENTS_LAVA) && !(Entity->flags & FL_IMMUNE_LAVA))
	{
		if (Entity->damage_debounce_time < level.framenum)
		{
			Entity->damage_debounce_time = level.framenum + 2;
			T_Damage (Entity, world, world, vec3Origin, Entity->state.origin, vec3Origin, 10*Entity->waterlevel, 0, 0, MOD_LAVA);
		}
	}
	if ((Entity->watertype & CONTENTS_SLIME) && !(Entity->flags & FL_IMMUNE_SLIME))
	{
		if (Entity->damage_debounce_time < level.framenum)
		{
			Entity->damage_debounce_time = level.framenum + 10;
			T_Damage (Entity, world, world, vec3Origin, Entity->state.origin, vec3Origin, 4*Entity->waterlevel, 0, 0, MOD_SLIME);
		}
	}
	
	if ( !(Entity->flags & FL_INWATER) )
	{	
		if (!(Entity->svFlags & SVF_DEADMONSTER))
		{
			if (Entity->watertype & CONTENTS_LAVA)
			{
				if (random() <= 0.5)
					PlaySoundFrom (Entity, CHAN_BODY, SoundIndex("player/lava1.wav"));
				else
					PlaySoundFrom (Entity, CHAN_BODY, SoundIndex("player/lava2.wav"));
			}
			else
				PlaySoundFrom (Entity, CHAN_BODY, SoundIndex("player/watr_in.wav"));
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
	point[0] = Entity->state.origin[0];
	point[1] = Entity->state.origin[1];
	point[2] = Entity->state.origin[2] + Entity->mins[2] + 1;	
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
	point[0] = Entity->state.origin[0];
	point[1] = Entity->state.origin[1];
	point[2] = Entity->state.origin[2] - 0.25;

	trace = CTrace (Entity->state.origin, Entity->mins, Entity->maxs, point, Entity, CONTENTS_MASK_MONSTERSOLID);

	// check steepness
	if ( trace.plane.normal[2] < 0.7 && !trace.startSolid)
	{
		Entity->groundentity = NULL;
		return;
	}

	if (!trace.startSolid && !trace.allSolid)
	{
		Vec3Copy (trace.endPos, Entity->state.origin);
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
	Vec3Subtract (Entity->enemy->state.origin, Entity->state.origin, vec);
	IdealYaw = VecToYaw(vec);
	// wait a while before first attack
	if (!(AIFlags & AI_STAND_GROUND))
		AttackFinished = level.framenum + 1;
}

bool CMonster::FindTarget()
{
	bool		heardit;
	int			r;
	CPlayerEntity *client;

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

#ifdef MONSTERS_USE_PATHFINDING
	if ((level.SoundEntityFramenum >= (level.framenum - 1)) && level.NoiseNode)
	{
		vec3_t temp;
		if (Entity->spawnflags & 1)
		{
			CTrace trace = CTrace(Entity->state.origin, level.NoiseNode->Origin, Entity, CONTENTS_MASK_SOLID);

			if (trace.fraction < 1.0)
				return false;
		}
		else
		{
			if (!gi.inPHS(Entity->state.origin, level.NoiseNode->Origin))
				return false;
		}

		Vec3Subtract (level.NoiseNode->Origin, Entity->state.origin, temp);

		if (Vec3Length(temp) > 1000)	// too far to hear
			return false;

		IdealYaw = VecToYaw(temp);
		ChangeYaw ();

		// hunt the sound for a bit; hopefully find the real player
		//AIFlags |= AI_SOUND_TARGET;

		P_CurrentNode = GetClosestNodeTo(Entity->state.origin);
		P_CurrentGoalNode = level.NoiseNode;
		FoundPath ();

		// Check if we can see the entity too
		if (!Entity->enemy && (level.SoundEntityFramenum >= (level.framenum - 1)) && !(Entity->spawnflags & 1) )
		{
			client = level.SoundEntity;

			if (client)
			{
				vec3_t	temp;

				if (client->gameEntity->flags & FL_NOTARGET)
					return false;

				client->State.GetOrigin (temp);
				if (Entity->spawnflags & 1)
				{
					if (!visible (Entity, client->gameEntity))
						return false;
				}
				else
				{
					if (!gi.inPHS(Entity->state.origin, temp))
						return false;
				}

				Vec3Subtract (temp, Entity->state.origin, temp);

				if (Vec3Length(temp) > 1000)	// too far to hear
					return false;

				// check area portals - if they are different and not connected then we can't hear it
				if (client->GetAreaNum() != Entity->areaNum)
				{
					if (!gi.AreasConnected(Entity->areaNum, client->GetAreaNum()))
						return false;
				}

				// hunt the sound for a bit; hopefully find the real player
				Entity->enemy = client->gameEntity;

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
	if ((level.sight_entity_framenum >= (level.framenum - 1)) && !(Entity->spawnflags & 1) )
	{
		client = level.sight_entity;
		if (client->enemy == Entity->enemy)
			return false;
	}
#endif

#ifndef MONSTERS_USE_PATHFINDING
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

	if (visible(Entity, client->gameEntity) && client->gameEntity == Entity->enemy)
		return true;	// JDC false;

	if (client)
	{
		if (client->gameEntity->flags & FL_NOTARGET)
			return false;
	}
	else
		return false;

	edict_t *old = Entity->enemy;
	if (!heardit)
	{
		r = range (Entity, client->gameEntity);

		if (r == RANGE_FAR)
			return false;

// this is where we would check invisibility

		// is client in an spot too dark to be seen?
		if (client->gameEntity->light_level <= 5)
			return false;

		if (!visible (Entity, client->gameEntity))
			return false;

		if (r == RANGE_NEAR)
		{
			if (client->gameEntity->show_hostile < level.framenum && !infront (Entity, client->gameEntity))
				return false;
		}
		else if (r == RANGE_MID)
		{
			if (!infront (Entity, client->gameEntity))
				return false;
		}

		Entity->enemy = client->gameEntity;

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
	else	// heardit
	{
		vec3_t	temp;

		client->State.GetOrigin (temp);
		if (Entity->spawnflags & 1)
		{
			if (!visible (Entity, client->gameEntity))
				return false;
		}
		else
		{
			if (!gi.inPHS(Entity->state.origin, temp))
				return false;
		}

		Vec3Subtract (temp, Entity->state.origin, temp);

		if (Vec3Length(temp) > 1000)	// too far to hear
			return false;

		// check area portals - if they are different and not connected then we can't hear it
		if (client->GetAreaNum() != Entity->areaNum)
		{
			if (!gi.AreasConnected(Entity->areaNum, client->GetAreaNum()))
				return false;
		}

		IdealYaw = VecToYaw(temp);
		ChangeYaw ();

		// hunt the sound for a bit; hopefully find the real player
		//AIFlags |= AI_SOUND_TARGET;
		Entity->enemy = client->gameEntity;
	}

//
// got one
//
	FoundTarget ();
#ifdef MONSTERS_ARENT_STUPID
	AlertNearbyStroggs ();
#endif

	if ((Entity->enemy != old) && MonsterFlags & MF_HAS_SIGHT)
		Sight ();

	return true;
}

bool CMonster::FacingIdeal()
{
	float delta = AngleModf (Entity->state.angles[YAW] - IdealYaw);
	if (delta > 45 && delta < 315)
		return false;
	return true;
}

void CMonster::FliesOff()
{
	Entity->state.effects &= ~EF_FLIES;
	Entity->state.sound = 0;
}

void CMonster::FliesOn ()
{
	if (Entity->waterlevel)
		return;
	Entity->state.effects |= EF_FLIES;
	Entity->state.sound = SoundIndex ("infantry/inflies1.wav");
	Think = &CMonster::FliesOff;
	NextThink = level.framenum + 600;
}

void CMonster::CheckFlies ()
{
	if (Entity->waterlevel)
		return;

	if (random() > 0.5)
		return;

	Think = &CMonster::FliesOn;
	NextThink = level.framenum + ((5 + 10 * random()) * 10);
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
	Entity->nextthink = level.framenum + 1;
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
	if (Entity->health < 1)
		return;

	if ((MonsterFlags & MF_HAS_DUCK) && (MonsterFlags & MF_HAS_UNDUCK))
		ducker = true;
	if ((MonsterFlags & MF_HAS_SIDESTEP) && !(AIFlags & AI_STAND_GROUND))
		dodger = true;

	if ((!ducker) && (!dodger))
		return;

	if (!Entity->enemy)
	{
		Entity->enemy = attacker;
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
		height = Entity->absMax[2]-32-1;  // the -1 is because the absmax is s.origin + maxs + 1

		// FIXME, make smarter
		// if we only duck, and ducking won't help or we're already ducking, do nothing
		//
		// need to add monsterinfo.abort_duck() and monsterinfo.next_duck_time

		if ((!dodger) && ((tr->endPos[2] <= height) || (AIFlags & AI_DUCKED)))
			return;
	}
	else
		height = Entity->absMax[2];

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

			Angles_Vectors (Entity->state.angles, NULL, right, NULL);
			Vec3Subtract (tr->endPos, Entity->state.origin, diff);

			if (DotProduct (right, diff) < 0)
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

	Entity->maxs[2] = BaseHeight - 32;
	Entity->takedamage = DAMAGE_YES;
	if (DuckWaitTime < level.framenum)
		DuckWaitTime = level.framenum + 10;
	gi.linkentity (Entity);
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

	Entity->maxs[2] = BaseHeight;
	Entity->takedamage = DAMAGE_AIM;
	NextDuckTime = level.framenum + 5;
	gi.linkentity (Entity);
}
#endif