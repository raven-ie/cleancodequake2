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
// cc_turret_entities.cpp
// Turret entities
//

#include "cc_local.h"
#include "cc_brushmodels.h"
#include "cc_infantry.h"
#include "cc_turret_entities.h"

void AnglesNormalize(vec3f &vec)
{
	while(vec.X > 360)
		vec.X -= 360;
	while(vec.X < 0)
		vec.X += 360;
	while(vec.Y > 360)
		vec.Y -= 360;
	while(vec.Y < 0)
		vec.Y += 360;
}

float SnapToEights(float x)
{
	x *= 8.0;
	if (x > 0.0)
		x += 0.5;
	else
		x -= 0.5;
	return 0.125 * (int)x;
}

CTurretEntityBase::CTurretEntityBase () :
CBaseEntity (),
CMapEntity (),
CBrushModel (),
CBlockableEntity ()
{
};

CTurretEntityBase::CTurretEntityBase (int Index) :
CBaseEntity (Index),
CMapEntity (Index),
CBrushModel (Index),
CBlockableEntity (Index)
{
};

bool CTurretEntityBase::Run ()
{
	return CBrushModel::Run ();
};

void CTurretEntityBase::Blocked (CBaseEntity *other)
{
	if ((other->EntityFlags & ENT_HURTABLE) && dynamic_cast<CHurtableEntity*>(other)->CanTakeDamage)
		dynamic_cast<CHurtableEntity*>(other)->TakeDamage (this, (TeamMaster->GetOwner()) ? TeamMaster->GetOwner() : TeamMaster,
					vec3fOrigin, other->State.GetOrigin(), vec3fOrigin, TeamMaster->gameEntity->dmg, 10, 0, MOD_CRUSH);
}

/*QUAKED turret_breach (0 0 0) ?
This portion of the turret can change both pitch and yaw.
The model  should be made with a flat pitch.
It (and the associated base) need to be oriented towards 0.
Use "angle" to set the starting angle.

"speed"		default 50
"dmg"		default 10
"angle"		point this forward
"target"	point this at an info_notnull at the muzzle tip
"minpitch"	min acceptable pitch angle : default -30
"maxpitch"	max acceptable pitch angle : default 30
"minyaw"	min acceptable yaw angle   : default 0
"maxyaw"	max acceptable yaw angle   : default 360
*/

CTurretBreach::CTurretBreach () :
CBaseEntity (),
CTurretEntityBase (),
FinishInit(true),
ShouldFire(false)
{
};

CTurretBreach::CTurretBreach (int Index) :
CBaseEntity (Index),
CTurretEntityBase (Index),
FinishInit(true),
ShouldFire(false)
{
};

void CTurretBreach::Fire ()
{
	vec3f f, r, u;
	State.GetAngles().ToVectors (&f, &r, &u);
	vec3f start = State.GetOrigin().MultiplyAngles (gameEntity->move_origin[0], f);
	start = start.MultiplyAngles (gameEntity->move_origin[1], r);
	start = start.MultiplyAngles (gameEntity->move_origin[2], u);

	int damage = 100 + random() * 50;
	CRocket::Spawn (TeamMaster->GetOwner(), start, f, damage, 550 + 50 * skill->Integer(), 150, damage);
	PlayPositionedSound (start, CHAN_WEAPON, SoundIndex("weapons/rocklf1a.wav"));
}

class CAvelocityForEachTeamChainCallback : public CForEachTeamChainCallback
{
public:
	float avelYaw;

	CAvelocityForEachTeamChainCallback (float avelYaw) :
	avelYaw(avelYaw)
	{
	};

	void Callback (CBaseEntity *Entity)
	{
		if (Entity->EntityFlags & ENT_PHYSICS)
			dynamic_cast<CPhysicsEntity*>(Entity)->AngularVelocity.Y = avelYaw;
	};
};

void CTurretBreach::Think ()
{
	if (FinishInit)
	{
		FinishInit = false;
		// get and save info for muzzle location
		if (!gameEntity->target)
		{
			//gi.dprintf("%s at (%f %f %f) needs a target\n", self->classname, self->state.origin[0], self->state.origin[1], self->state.origin[2]);
			MapPrint (MAPPRINT_ERROR, this, State.GetOrigin(), "Needs a target\n");
		}
		else
		{
			CBaseEntity *targ = CC_PickTarget (gameEntity->target);
			vec3f moveOrg = (targ->State.GetOrigin() - State.GetOrigin());
			Vec3Copy (moveOrg, gameEntity->move_origin);
			targ->Free();
		}

		TeamMaster->gameEntity->dmg = gameEntity->dmg;
		Think ();
	}
	else
	{
		vec3f current_angles = State.GetAngles ();
		AnglesNormalize(current_angles);

		vec3f moveangles = vec3f(gameEntity->move_angles);
		AnglesNormalize(moveangles);
		if (moveangles.X > 180)
			moveangles.X -= 360;
		Vec3Copy (moveangles, gameEntity->move_angles);

		// clamp angles to mins & maxs
		if (gameEntity->move_angles[PITCH] > Positions[0].X)
			gameEntity->move_angles[PITCH] = Positions[0].X;
		else if (gameEntity->move_angles[PITCH] < Positions[1].X)
			gameEntity->move_angles[PITCH] = Positions[1].X;

		if ((gameEntity->move_angles[YAW] < Positions[0].Y) || (gameEntity->move_angles[YAW] > Positions[1].Y))
		{
			float dmin = Q_fabs(Positions[0].Y - gameEntity->move_angles[YAW]);
			if (dmin < -180)
				dmin += 360;
			else if (dmin > 180)
				dmin -= 360;

			float dmax = Q_fabs(Positions[1].Y - gameEntity->move_angles[YAW]);
			if (dmax < -180)
				dmax += 360;
			else if (dmax > 180)
				dmax -= 360;

			gameEntity->move_angles[YAW] = Positions[(Q_fabs(dmin) < Q_fabs(dmax)) ? 0 : 1].Y;
		}

		vec3f delta = (vec3f(gameEntity->move_angles) - current_angles);
		if (delta.X < -180)
			delta.X += 360;
		else if (delta.X > 180)
			delta.X -= 360;
		if (delta.Y < -180)
			delta.Y += 360;
		else if (delta.Y > 180)
			delta.Y -= 360;
		delta.Z = 0;

		if (delta.X > gameEntity->speed * 0.1f)
			delta.X = gameEntity->speed * 0.1f;
		if (delta.X < -1 * gameEntity->speed * 0.1f)
			delta.X = -1 * gameEntity->speed * 0.1f;
		if (delta.Y > gameEntity->speed * 0.1f)
			delta.Y = gameEntity->speed * 0.1f;
		if (delta.Y < -1 * gameEntity->speed * 0.1f)
			delta.Y = -1 * gameEntity->speed * 0.1f;

		//Vec3Scale (delta, 1, gameEntity->avelocity);
		AngularVelocity = delta;

		NextThink = level.framenum + FRAMETIME;

		CAvelocityForEachTeamChainCallback (AngularVelocity.Y).Query (this);

		// if we have adriver, adjust his velocities
		if (GetOwner())
		{
			float	angle;
			float	target_z;
			float	diff;
			vec3f	target;
			vec3f	dir;

			// angular is easy, just copy ours
			CPhysicsEntity *Owned = dynamic_cast<CPhysicsEntity*>(GetOwner());
			Owned->AngularVelocity.X = AngularVelocity.Z;
			Owned->AngularVelocity.Y = AngularVelocity.Y;

			// x & y
			angle = State.GetAngles().Y + gameEntity->owner->move_origin[1];
			angle *= (((float)M_PI)*2 / 360);
			target.X = SnapToEights(State.GetOrigin().X + cosf(angle) * gameEntity->owner->move_origin[0]);
			target.Y = SnapToEights(State.GetOrigin().Y + sinf(angle) * gameEntity->owner->move_origin[0]);
			target.Z = gameEntity->owner->state.origin[2];

			dir = target - gameEntity->owner->Entity->State.GetOrigin();
			
			CPhysicsEntity *Owner = dynamic_cast<CPhysicsEntity*>(GetOwner());

			Owner->Velocity.X = dir.X * 1.0 / 1;
			Owner->Velocity.Y = dir.Y * 1.0 / 1;

			// z
			angle = State.GetAngles().X * (M_PI*2 / 360);
			target_z = SnapToEights(State.GetOrigin().Z + gameEntity->owner->move_origin[0] * tan(angle) + gameEntity->owner->move_origin[2]);

			diff = target_z - gameEntity->owner->state.origin[2];
			Owner->Velocity.Z = diff * 1.0 / 1;

			if (ShouldFire)
			{
				Fire ();
				ShouldFire = false;
			}
		}
	}
}

void CTurretBreach::Spawn ()
{
	SetSolid (SOLID_BSP);
	PhysicsType = PHYSICS_PUSH;
	SetModel (gameEntity, gameEntity->model);

	if (!gameEntity->speed)
		gameEntity->speed = 50;
	if (!gameEntity->dmg)
		gameEntity->dmg = 10;

	if (!st.minpitch)
		st.minpitch = -30;
	if (!st.maxpitch)
		st.maxpitch = 30;
	if (!st.maxyaw)
		st.maxyaw = 360;

	Positions[0].X = -1 * st.minpitch;
	Positions[0].Y = st.minyaw;
	Positions[1].X = -1 * st.maxpitch;
	Positions[1].Y = st.maxyaw;

	gameEntity->move_angles[YAW] = State.GetAngles().Y;

	NextThink = level.framenum + FRAMETIME;
	Link ();
}

LINK_CLASSNAME_TO_CLASS ("turret_breach", CTurretBreach);

/*QUAKED turret_base (0 0 0) ?
This portion of the turret changes yaw only.
MUST be teamed with a turret_breach.
*/

CTurretBase::CTurretBase () :
CBaseEntity (),
CTurretEntityBase ()
{
};

CTurretBase::CTurretBase (int Index) :
CBaseEntity (Index),
CTurretEntityBase (Index)
{
};

void CTurretBase::Spawn ()
{
	SetSolid (SOLID_BSP);
	PhysicsType = PHYSICS_PUSH;
	SetModel (gameEntity, gameEntity->model);
	Link ();
}

LINK_CLASSNAME_TO_CLASS ("turret_base", CTurretBase);

// Paril Note
// This may seem like a bit of a jump to call this guy a "monster",
// but he does have a lot of monster-like qualities.
// The only difference you'll see in-game is that in CleanCode
// the turret driver will not gib when you kill him; he'll do the
// infantry's death animation, like I hope they intended!

/*QUAKED turret_driver (1 .5 0) (-16 -16 -24) (16 16 32)
Must NOT be on the team with the rest of the turret parts.
Instead it must target the turret_breach.
*/

CTurretDriver::CTurretDriver () :
CInfantry (),
TargetedBreach(NULL)
{
};

void CTurretDriver::Die (CBaseEntity *inflictor, CBaseEntity *attacker, int damage, vec3f &point)
{
// check for gib
	if (Entity->gameEntity->health <= Entity->gameEntity->gib_health)
	{
		Entity->PlaySound (CHAN_VOICE, SoundIndex ("misc/udeath.wav"));
		for (int n= 0; n < 2; n++)
			CGibEntity::Spawn (Entity, gMedia.Gib_Bone[0], damage, GIB_ORGANIC);
		for (int n= 0; n < 4; n++)
			CGibEntity::Spawn (Entity, gMedia.Gib_SmallMeat, damage, GIB_ORGANIC);
		Entity->ThrowHead (gMedia.Gib_Head[1], damage, GIB_ORGANIC);
		Entity->DeadFlag = true;
		return;
	}

	if (Entity->DeadFlag == true)
		return;

	CBaseEntity	*ent;

	// level the gun
	TargetedBreach->gameEntity->move_angles[0] = 0;

	// remove the driver from the end of them team chain
	for (ent = TargetedBreach->TeamMaster; ent->TeamChain != Entity; ent = ent->TeamChain)
		;
	ent->TeamChain = NULL;
	Entity->TeamMaster = NULL;
	Entity->Flags &= ~FL_TEAMSLAVE;

	TargetedBreach->SetOwner (NULL);
	TargetedBreach->TeamMaster->SetOwner (NULL);

	CInfantry::Die (inflictor, attacker, damage, point);
	Think = &CMonster::MonsterThink;
	Entity->NextThink = level.framenum + FRAMETIME;
}

void CTurretDriver::Pain (CBaseEntity *other, float kick, int damage)
{
	if (Entity->gameEntity->health < (Entity->gameEntity->max_health / 2))
		Entity->State.SetSkinNum(1);

	if (level.framenum < Entity->gameEntity->pain_debounce_time)
		return;

	Entity->gameEntity->pain_debounce_time = level.framenum + 30;
	Entity->PlaySound (CHAN_VOICE, (!irandom(2) == 0) ? SoundPain1 : SoundPain2);
}

void CTurretDriver::TurretThink ()
{
	Entity->NextThink = level.framenum + FRAMETIME;

	if (Entity->gameEntity->enemy && (!Entity->gameEntity->enemy->Entity->IsInUse() || Entity->gameEntity->enemy->health <= 0))
		Entity->gameEntity->enemy = NULL;

	if (!Entity->gameEntity->enemy)
	{
		if (!FindTarget ())
			return;
		TrailTime = level.framenum;
		AIFlags &= ~AI_LOST_SIGHT;
	}
	else
	{
		if (IsVisible (Entity, Entity->gameEntity->enemy->Entity))
		{
			if (AIFlags & AI_LOST_SIGHT)
			{
				TrailTime = level.framenum;
				AIFlags &= ~AI_LOST_SIGHT;
			}
		}
		else
		{
			AIFlags |= AI_LOST_SIGHT;
			return;
		}
	}

	// let the turret know where we want it to aim
	vec3f dir = (Entity->gameEntity->enemy->Entity->State.GetOrigin() +
		vec3f(0, 0, Entity->gameEntity->enemy->viewheight)) -
		TargetedBreach->State.GetOrigin();

	vec3f ang = dir.ToAngles ();
	Vec3Copy (ang, TargetedBreach->gameEntity->move_angles);

	// decide if we should shoot
	if (level.framenum < AttackFinished)
		return;

	float reaction_time = (3 - skill->Integer()) * 1.0;
	if ((level.framenum - TrailTime) < (reaction_time * 10))
		return;

	AttackFinished = level.framenum + ((reaction_time + 1.0) * 10);

	TargetedBreach->ShouldFire = true;
}

void CTurretDriver::TurretLink ()
{
	Think = static_cast<void (__thiscall CMonster::* )(void)>(&CTurretDriver::TurretThink);
	Entity->NextThink = level.framenum + FRAMETIME;

	TargetedBreach = dynamic_cast<CTurretBreach*>(CC_PickTarget (Entity->gameEntity->target));
	TargetedBreach->SetOwner (Entity);
	TargetedBreach->TeamMaster->SetOwner (Entity);
	Entity->State.SetAngles (TargetedBreach->State.GetAngles());

	vec3f vec = (TargetedBreach->State.GetOrigin() - Entity->State.GetOrigin());
	vec.Z = 0;
	Entity->gameEntity->move_origin[0] = vec.Length();

	vec = (Entity->State.GetOrigin() - TargetedBreach->State.GetOrigin());
	vec = vec.ToAngles();
	AnglesNormalize(vec);
	Entity->gameEntity->move_origin[1] = vec.Y;

	Entity->gameEntity->move_origin[2] = Entity->State.GetOrigin().Z - TargetedBreach->State.GetOrigin().Z;

	// add the driver to the end of them team chain
	CBaseEntity	*ent;
	for (ent = TargetedBreach->TeamMaster; ent->TeamChain; ent = ent->TeamChain)
		;
	ent->TeamChain = Entity;
	Entity->TeamMaster = TargetedBreach->TeamMaster;
	Entity->Flags |= FL_TEAMSLAVE;
}

void CTurretDriver::Spawn ()
{
	if (deathmatch->Integer())
	{
		Entity->Free ();
		return;
	}

	Entity->PhysicsType = PHYSICS_PUSH;
	Entity->SetSolid (SOLID_BBOX);
	Entity->State.SetModelIndex (ModelIndex("models/monsters/infantry/tris.md2"));
	Entity->SetMins (vec3f(-16, -16, -24));
	Entity->SetMaxs (vec3f(16, 16, 32));

	Entity->gameEntity->health = Entity->gameEntity->max_health = 100;
	Entity->gameEntity->gib_health = -40;
	Entity->gameEntity->mass = 200;
	Entity->gameEntity->viewheight = 24;

	SoundPain1 = SoundIndex ("infantry/infpain1.wav");
	SoundPain2 = SoundIndex ("infantry/infpain2.wav");
	SoundDie1 = SoundIndex ("infantry/infdeth1.wav");
	SoundDie2 = SoundIndex ("infantry/infdeth2.wav");

	Entity->Flags |= FL_NO_KNOCKBACK;

	level.total_monsters++;

	Entity->SetSvFlags (Entity->GetSvFlags() | SVF_MONSTER);
	Entity->State.AddRenderEffects (RF_FRAMELERP);
	Entity->CanTakeDamage = true;
	Entity->SetClipmask (CONTENTS_MASK_MONSTERSOLID);
	Entity->State.SetOldOrigin (Entity->State.GetOrigin ());
	AIFlags |= (AI_STAND_GROUND);

	if (st.item)
	{
		Entity->gameEntity->item = FindItemByClassname (st.item);
		if (!Entity->gameEntity->item)
			MapPrint (MAPPRINT_WARNING, Entity, Entity->State.GetOrigin(), "has bad item: %s\n", st.item);
	}

	Think = static_cast<void (__thiscall CMonster::* )(void)>(&CTurretDriver::TurretLink);
	Entity->NextThink = level.framenum + FRAMETIME;

	Entity->Link ();
}

LINK_MONSTER_CLASSNAME_TO_CLASS ("turret_driver", CTurretDriver);