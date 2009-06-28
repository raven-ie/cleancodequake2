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
// cc_misc_entities.cpp
// misc_* entities
//

#include "cc_local.h"

class CMiscExploBox : public CTossProjectile, public CHurtableEntity
{
	bool Dropped;
public:
	CMiscExploBox () :
	CHurtableEntity (),
	CTossProjectile()
	{
		SetSolid (SOLID_BBOX);

		State.SetModelIndex (ModelIndex ("models/objects/barrels/tris.md2"));

		SetMins (vec3f(-16, -16, 0));
		SetMaxs (vec3f(16, 16, 40));

		if (!gameEntity->mass)
			gameEntity->mass = 400;
		if (!gameEntity->health)
			gameEntity->health = 10;
		if (!gameEntity->dmg)
			gameEntity->dmg = 150;

		gameEntity->takedamage = DAMAGE_YES;
		NextThink = level.framenum + FRAMETIME;

		Link ();
	};

	CMiscExploBox (int Index) : 
	CBaseEntity(Index),
	CHurtableEntity(Index),
	CTossProjectile(Index)
	{
		SetSolid (SOLID_BBOX);

		State.SetModelIndex (ModelIndex ("models/objects/barrels/tris.md2"));

		SetMins (vec3f(-16, -16, 0));
		SetMaxs (vec3f(16, 16, 40));

		if (!gameEntity->mass)
			gameEntity->mass = 400;
		if (!gameEntity->health)
			gameEntity->health = 10;
		if (!gameEntity->dmg)
			gameEntity->dmg = 150;

		gameEntity->takedamage = DAMAGE_YES;
		NextThink = level.framenum + FRAMETIME;

		Link ();
	};

	void Touch (CBaseEntity *other, plane_t *plane, cmBspSurface_t *surf)
	{
		float	ratio;
		vec3_t	v;

		if ((!other->gameEntity->groundentity) || (other->gameEntity->groundentity == gameEntity))
			return;

		ratio = (float)other->gameEntity->mass / (float)gameEntity->mass;

		State.GetOrigin (v);
		vec3_t enemyV;
		other->State.GetOrigin (enemyV);
		Vec3Subtract (v, enemyV, v);

		float Yaw = VecToYaw(v);
		Yaw = Yaw*M_PI*2 / 360;
		
		vec3_t move;
		move[0] = cosf(Yaw)*(20 * ratio);
		move[1] = sinf(Yaw)*(20 * ratio);
		move[2] = 0;

		vec3_t oldorg, neworg, end;
		State.GetOrigin (oldorg);
		Vec3Add (oldorg, move, neworg);

		int stepsize = 8;

		neworg[2] += stepsize;
		Vec3Copy (neworg, end);
		end[2] -= stepsize*2;

		CTrace trace;
		vec3_t mins, maxs;
		GetMins (mins);
		GetMaxs (maxs);
		trace = CTrace (neworg, mins, maxs, end, gameEntity, CONTENTS_MASK_MONSTERSOLID);

		if (trace.allSolid)
			return;

		if (trace.startSolid)
		{
			neworg[2] -= stepsize;
			trace = CTrace (neworg, mins, maxs, end, gameEntity, CONTENTS_MASK_MONSTERSOLID);
			if (trace.allSolid || trace.startSolid)
				return;
		}

	// check point traces down for dangling corners
		State.SetOrigin (trace.endPos);

		gameEntity->groundentity = trace.ent;
		gameEntity->groundentity_linkcount = trace.ent->linkCount;

		if (trace.fraction == 1.0)
			gameEntity->groundentity = NULL;

	// the move is ok
		Link ();
		G_TouchTriggers (gameEntity);
	}

	void Think ()
	{
		if (!Dropped)
		{
			Dropped = true;

			vec3_t		end;
			CTrace		trace;
			vec3_t origin, mins, maxs;
			State.GetOrigin (origin);
			GetMins (mins);
			GetMaxs (maxs);

			origin[2] += 1;
			Vec3Copy (origin, end);
			end[2] -= 256;
			
			trace = CTrace (origin, mins, maxs, end, gameEntity, CONTENTS_MASK_MONSTERSOLID);

			if (trace.fraction == 1 || trace.allSolid)
				return;

			State.SetOrigin (trace.endPos);

			return;
		}
		T_RadiusDamage (gameEntity, gameEntity->activator, gameEntity->dmg, NULL, gameEntity->dmg+40, MOD_BARREL);

		vec3_t origin;
		State.GetOrigin (origin);
		CTempEnt_Explosions::GrenadeExplosion (origin, gameEntity);

		Free ();
	};

	void Die (CBaseEntity *inflictor, CBaseEntity *attacker, int damage, vec3_t point)
	{
		gameEntity->takedamage = DAMAGE_NO;
		NextThink = level.framenum + 2;
		gameEntity->activator = attacker->gameEntity;
	};

	void Pain (CBaseEntity *other, float kick, int damage) {};

	bool Run ()
	{
		return CBounceProjectile::Run ();
	};
};

void SP_misc_explobox (edict_t *self)
{
	if (game.mode & GAME_DEATHMATCH)
	{
		G_FreeEdict (self);
		return;
	}

	self->Entity = QNew (com_levelPool, 0) CMiscExploBox(self->state.number);
}