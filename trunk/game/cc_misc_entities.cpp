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

class CMiscExploBox : public CMapEntity, public CStepPhysics, public CHurtableEntity, public CThinkableEntity, public CTouchableEntity
{
	bool Dropped;
public:
	CMiscExploBox () :
	CBaseEntity(),
	CMapEntity(),
	CThinkableEntity(),
	CHurtableEntity (),
	CStepPhysics()
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
	CMapEntity(Index),
	CThinkableEntity(),
	CHurtableEntity(Index),
	CStepPhysics(Index)
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

#define BARREL_STEPSIZE 8
	void Touch (CBaseEntity *other, plane_t *plane, cmBspSurface_t *surf)
	{
		if ((!other->gameEntity->groundentity) || (other->gameEntity->groundentity == gameEntity))
			return;

		float ratio = (float)other->gameEntity->mass / (float)gameEntity->mass;
		vec3f v = State.GetOrigin() - other->State.GetOrigin();
		float Yaw = (v.ToYaw ()*M_PI*2 / 360);
		vec3f move = vec3f( cosf(Yaw)*(20 * ratio),
							sinf(Yaw)*(20 * ratio),
							0);

		vec3f	oldOrigin = State.GetOrigin(),
				newOrigin = (oldOrigin + move);

		newOrigin.Z += BARREL_STEPSIZE;
		vec3f end = vec3f(newOrigin);

		end.Z -= BARREL_STEPSIZE*2;

		CTrace trace;
		trace = CTrace (newOrigin, GetMins(), GetMaxs(), end, gameEntity, CONTENTS_MASK_MONSTERSOLID);

		if (trace.allSolid)
			return;

		if (trace.startSolid)
		{
			newOrigin[2] -= BARREL_STEPSIZE;
			trace = CTrace (newOrigin, GetMins(), GetMaxs(), end, gameEntity, CONTENTS_MASK_MONSTERSOLID);
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

			CTrace		trace;
			vec3f		origin = State.GetOrigin();

			origin.Z += 1;
			vec3f end = vec3f(origin);
			end.Z -= 256;
			
			trace = CTrace (origin, GetMins(), GetMaxs(), end, gameEntity, CONTENTS_MASK_MONSTERSOLID);

			if (trace.fraction == 1 || trace.allSolid)
				return;

			State.SetOrigin (trace.endPos);
			Link();
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
		return CStepPhysics::Run ();
	};

	void MapSpawn ()
	{
	};
};

LINK_CLASSNAME_TO_CLASS ("misc_explobox",CMiscExploBox);
