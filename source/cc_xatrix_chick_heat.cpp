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
// cc_xatrix_chick_heat.cpp
// 
//

#include "cc_local.h"

#if XATRIX_FEATURES
#include "cc_bitch.h"
#include "cc_xatrix_chick_heat.h"
#include "cc_tent.h"

CHeatRocket::CHeatRocket () :
CFlyMissileProjectile(),
CTouchableEntity(),
CThinkableEntity()
{
};

CHeatRocket::CHeatRocket (sint32 Index) :
CFlyMissileProjectile(Index),
CTouchableEntity(Index),
CThinkableEntity(Index)
{
};

IMPLEMENT_SAVE_SOURCE(CHeatRocket)

void CHeatRocket::Think ()
{
	CHurtableEntity	*target = NULL, *aquire = NULL;
	int			oldlen = 0;

	// aquire new target
	while ((target = FindRadius<CHurtableEntity, ENT_HURTABLE> (target, State.GetOrigin(), 1024)) != NULL)
	{
		if (target == GetOwner())
			continue;
		if (!(target->EntityFlags & ENT_PLAYER))
			continue;
		if (target->Health <= 0)
			continue;
		if (!IsVisible (target, this))
			continue;
		
		// if we need to reduce the tracking cone
		/*
		{
			vec3_t	vec;
			float	dot;
			vec3_t	forward;
	
			AngleVectors (self->s.angles, forward, NULL, NULL);
			VectorSubtract (target->s.origin, self->s.origin, vec);
			VectorNormalize (vec);
			dot = DotProduct (vec, forward);
	
			if (dot > 0.6)
				continue;
		}
		*/

		if (!IsInFront (target, this))
			continue;

		vec3f vec = State.GetOrigin() - target->State.GetOrigin();
		int len = vec.Length();

		if (aquire == NULL || len < oldlen)
		{
			aquire = target;
			oldlen = len;
		}
	}

	if (aquire != NULL)
	{
		vec3f oldang = State.GetAngles();
		vec3f vec = aquire->State.GetOrigin() - State.GetOrigin();

		State.GetAngles() = vec.ToAngles();
		vec.Normalize();
		Velocity = vec * 500;
	}

	NextThink = Level.Frame + FRAMETIME;
}

void CHeatRocket::Touch (CBaseEntity *Other, plane_t *plane, cmBspSurface_t *surf)
{
	if (Other == GetOwner())
		return;

	if (surf && (surf->flags & SURF_TEXINFO_SKY))
	{
		Free ();
		return;
	}

	if (GetOwner() && (GetOwner()->EntityFlags & ENT_PLAYER))
		entity_cast<CPlayerEntity>(GetOwner())->PlayerNoiseAt (State.GetOrigin (), PNOISE_IMPACT);

	if ((Other->EntityFlags & ENT_HURTABLE) && entity_cast<CHurtableEntity>(Other)->CanTakeDamage)
		entity_cast<CHurtableEntity>(Other)->TakeDamage (this, GetOwner(), Velocity, State.GetOrigin (), (plane) ? plane->normal : vec3fOrigin, Damage, 0, 0, MOD_ROCKET);

	// calculate position for the explosion entity
	vec3f origin = State.GetOrigin ().MultiplyAngles (-0.02f, Velocity);
	SplashDamage(GetOwner(), RadiusDamage, Other, DamageRadius, MOD_R_SPLASH);
	CRocketExplosion(origin, !!WaterInfo.Level).Send();

	Free ();
}

CHeatRocket *CHeatRocket::Spawn	(CBaseEntity *Spawner, vec3f start, vec3f dir,
						sint32 Damage, sint32 speed, float damage_radius, sint32 radius_damage)
{
	CHeatRocket	*Rocket = QNewEntityOf CHeatRocket;

	Rocket->State.GetOrigin() = start;
	Rocket->State.GetAngles() = dir.ToAngles();
	Rocket->Velocity = dir * speed;
	Rocket->State.GetEffects() = EF_ROCKET;
	Rocket->State.GetModelIndex() = ModelIndex ("models/objects/rocket/tris.md2");
	Rocket->SetOwner (Spawner);
	Rocket->NextThink = Level.Frame + FRAMETIME;
	Rocket->Damage = Damage;
	Rocket->RadiusDamage = radius_damage;
	Rocket->DamageRadius = damage_radius;
	Rocket->State.GetSound() = SoundIndex ("weapons/rockfly.wav");
	Rocket->ClassName = "rocket";
	Rocket->GetClipmask() = CONTENTS_MASK_SHOT;
	Rocket->GetSolid() = SOLID_BBOX;
	Rocket->GetMins().Clear ();
	Rocket->GetMaxs().Clear ();
	Rocket->Touchable = true;

	if (Spawner->EntityFlags & ENT_PLAYER)
		CheckDodge (Spawner, start, dir, speed);

	Rocket->Link ();
	return Rocket;
}

bool CHeatRocket::Run ()
{
	return CFlyMissileProjectile::Run();
}

CHeatMaiden::CHeatMaiden (uint32 ID) :
CMaiden (ID)
{
	MonsterName = "Heat-seeking Iron Maiden";
};

void CHeatMaiden::Rocket ()
{
#if MONSTER_USE_ROGUE_AI
	vec3f	forward, right, start, dir, vec, target;
	bool blindfire = (AIFlags & AI_MANUAL_STEERING) ? true : false;

	Entity->State.GetAngles().ToVectors (&forward, &right, NULL);
	G_ProjectSource (Entity->State.GetOrigin(), dumb_and_hacky_monster_MuzzFlashOffset[MZ2_CHICK_ROCKET_1], forward, right, start);

	sint32 rocketSpeed = 500 + (100 * CvarList[CV_SKILL].Integer());	// PGM rock & roll.... :)

	target = (blindfire) ? BlindFireTarget : Entity->Enemy->State.GetOrigin();
	if (blindfire)
	{
		vec = target;
		dir = vec - start;
	}
	// pmm
	// don't shoot at feet if they're above where i'm shooting from.
	else if(frand() < 0.33 || (start[2] < Entity->Enemy->GetAbsMin().Z))
	{
		vec = target;
		vec.Z += Entity->Enemy->ViewHeight;
		dir = vec - start;
	}
	else
	{
		vec = target;
		vec.Z = Entity->Enemy->GetAbsMin().Z;
		dir = vec - start;
	}

	// Lead target  (not when blindfiring)
	// 20, 35, 50, 65 chance of leading
	if((!blindfire) && ((frand() < (0.2 + ((3 - CvarList[CV_SKILL].Integer()) * 0.15)))))
	{
		vec = vec.MultiplyAngles (dir.Length() / rocketSpeed, entity_cast<CPhysicsEntity>(Entity->Enemy)->Velocity);
		dir = vec - start;
	}

	dir.Normalize ();

	// pmm blindfire doesn't check target (done in checkattack)
	// paranoia, make sure we're not shooting a target right next to us
	CTrace trace (start, vec, Entity, CONTENTS_MASK_SHOT);
	if (blindfire)
	{
		// blindfire has different fail criteria for the trace
		if (!(trace.startSolid || trace.allSolid || (trace.fraction < 0.5)))
			MonsterFireHeatRocket (start, dir, 50, rocketSpeed, MZ2_CHICK_ROCKET_1);
		else 
		{
			// geez, this is bad.  she's avoiding about 80% of her blindfires due to hitting things.
			// hunt around for a good shot
			// try shifting the target to the left a little (to help counter her large offset)
			vec = target;
			vec = vec.MultiplyAngles (-10, right);
			dir = vec - start;
			dir.NormalizeFast();
			trace (start, vec, Entity, CONTENTS_MASK_SHOT);
			if (!(trace.startSolid || trace.allSolid || (trace.fraction < 0.5)))
				MonsterFireHeatRocket (start, dir, 50, rocketSpeed, MZ2_CHICK_ROCKET_1);
			else 
			{
				// ok, that failed.  try to the right
				vec = target;
				vec = vec.MultiplyAngles (10, right);
				dir = vec - start;
				dir.NormalizeFast();
				trace (start, vec, Entity, CONTENTS_MASK_SHOT);
				if (!(trace.startSolid || trace.allSolid || (trace.fraction < 0.5)))
					MonsterFireHeatRocket (start, dir, 50, rocketSpeed, MZ2_CHICK_ROCKET_1);
			}
		}
	}
	else
		MonsterFireHeatRocket (start, dir, 50, rocketSpeed, MZ2_CHICK_ROCKET_1);
#else
	vec3f	forward, right, start, dir, vec;

	Entity->State.GetAngles().ToVectors (&forward, &right, NULL);
	G_ProjectSource (Entity->State.GetOrigin(), dumb_and_hacky_monster_MuzzFlashOffset[MZ2_CHICK_ROCKET_1], forward, right, start);

	vec = Entity->Enemy->State.GetOrigin();
	vec.Z += Entity->Enemy->ViewHeight;
	dir = vec - start;
	dir.NormalizeFast ();

	MonsterFireHeatRocket (start, dir, 50, 500, MZ2_CHICK_ROCKET_1);
#endif
}

void CHeatMaiden::Spawn ()
{
	CMaiden::Spawn ();
	Entity->State.GetSkinNum() = 3;
}

LINK_MONSTER_CLASSNAME_TO_CLASS ("monster_chick_heat", CHeatMaiden);
#endif
