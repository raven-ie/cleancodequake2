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
#include "g_local.h"

void UpdateChaseCam(CPlayerEntity *ent)
{
	vec3_t o, ownerv, goal;
	vec3_t forward, right;
	CTrace trace;
	int i;
	vec3_t oldgoal;
	vec3_t angles;

	// is our chase target gone?
	if (!ent->Client.chase_target->IsInUse()
		|| ent->Client.chase_target->Client.resp.spectator)
	{
		CPlayerEntity *old = ent->Client.chase_target;
		ChaseNext(ent);
		if (ent->Client.chase_target == old)
		{
			ent->Client.chase_target = NULL;
			ent->Client.PlayerState.GetPMove()->pmFlags &= ~PMF_NO_PREDICTION;
			return;
		}
	}

	CPlayerEntity *targ = ent->Client.chase_target;

	targ->State.GetOrigin (ownerv);
	ent->State.GetOrigin (oldgoal);

	ownerv[2] += targ->gameEntity->viewheight;

	Vec3Copy(targ->Client.v_angle, angles);
	if (angles[PITCH] > 56)
		angles[PITCH] = 56;
	Angles_Vectors (angles, forward, right, NULL);
	VectorNormalizef (forward, forward);
	Vec3MA (ownerv, -30, forward, o);

	vec3_t temp;
	targ->State.GetOrigin (temp);
	if (o[2] < temp[2] + 20)
		o[2] = temp[2] + 20;

	// jump animation lifts
	if (!targ->gameEntity->groundentity)
		o[2] += 16;

	trace = CTrace (ownerv, vec3Origin, vec3Origin, o, targ->gameEntity, CONTENTS_MASK_SOLID);

	Vec3Copy(trace.endPos, goal);

	Vec3MA(goal, 2, forward, goal);

	// pad for floors and ceilings
	Vec3Copy(goal, o);
	o[2] += 6;
	trace = CTrace (goal, vec3Origin, vec3Origin, o, targ->gameEntity, CONTENTS_MASK_SOLID);
	if (trace.fraction < 1)
	{
		Vec3Copy(trace.endPos, goal);
		goal[2] -= 6;
	}

	Vec3Copy(goal, o);
	o[2] -= 6;
	trace = CTrace (goal, vec3Origin, vec3Origin, o, targ->gameEntity, CONTENTS_MASK_SOLID);
	if (trace.fraction < 1)
	{
		Vec3Copy(trace.endPos, goal);
		goal[2] += 6;
	}

	if (targ->gameEntity->deadflag)
		ent->Client.PlayerState.GetPMove()->pmType = PMT_DEAD;
	else
		ent->Client.PlayerState.GetPMove()->pmType = PMT_FREEZE;

	ent->State.SetOrigin(goal);
	for (i=0 ; i<3 ; i++)
		ent->Client.PlayerState.GetPMove()->deltaAngles[i] = ANGLE2SHORT(targ->Client.v_angle[i] - ent->Client.resp.cmd_angles[i]);

	if (targ->gameEntity->deadflag)
		ent->Client.PlayerState.SetViewAngles (vec3f(40, -15, targ->Client.killer_yaw));
	else
	{
		ent->Client.PlayerState.SetViewAngles(targ->Client.v_angle);
		Vec3Copy(targ->Client.v_angle, ent->Client.v_angle);
	}

	ent->gameEntity->viewheight = 0;
	ent->Client.PlayerState.GetPMove()->pmFlags |= PMF_NO_PREDICTION;
	gi.linkentity(ent->gameEntity);

	if ((!ent->Client.showscores && !ent->Client.resp.MenuState.InMenu &&
		!ent->Client.showinventory && !ent->Client.showhelp &&
		!(level.framenum & 31)) || ent->Client.update_chase)
	{
		CStatusBar Chasing;
		char temp[128];
		Q_snprintfz (temp, sizeof(temp), "Chasing %s", targ->Client.pers.netname);

		Chasing.AddVirtualPoint_X (0);
		Chasing.AddVirtualPoint_Y (-68);
		Chasing.AddString (temp, true, false);
		Chasing.SendMsg (ent->gameEntity, false);

		ent->Client.update_chase = false;
	}
}

void ChaseNext(CPlayerEntity *ent)
{
	int i;
	CPlayerEntity *e;

	if (!ent->Client.chase_target)
		return;

	i = ent->Client.chase_target->State.GetNumber();
	do {
		i++;
		if (i > game.maxclients)
			i = 1;
		e = dynamic_cast<CPlayerEntity*>(g_edicts[i].Entity);
		if (!e->IsInUse())
			continue;
		if (!e->Client.resp.spectator)
			break;
	} while (e != ent->Client.chase_target);

	ent->Client.chase_target = e;
	ent->Client.update_chase = true;
}

void ChasePrev(CPlayerEntity *ent)
{
	int i;
	CPlayerEntity *e;

	if (!ent->Client.chase_target)
		return;

	i = ent->Client.chase_target->State.GetNumber();
	do {
		i--;
		if (i < 1)
			i = game.maxclients;
		e = dynamic_cast<CPlayerEntity*>(g_edicts[i].Entity);
		if (!e->IsInUse())
			continue;
		if (!e->Client.resp.spectator)
			break;
	} while (e != ent->Client.chase_target);

	ent->Client.chase_target = e;
	ent->Client.update_chase = true;
}

void GetChaseTarget(CPlayerEntity *ent)
{
	int i;

	for (i = 1; i <= game.maxclients; i++)
	{
		CPlayerEntity *other = dynamic_cast<CPlayerEntity*>(g_edicts[i].Entity);
		if (other->IsInUse() && !other->Client.resp.spectator)
		{
			ent->Client.chase_target = other;
			ent->Client.update_chase = true;
			UpdateChaseCam(ent);
			return;
		}
	}
	ent->PrintToClient(PRINT_CENTER, "No other players to chase.");
}


