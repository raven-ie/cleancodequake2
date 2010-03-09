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
// cc_weaponentities.cpp
// Entities related to weaponry
//

#include "cc_local.h"
#include "cc_weaponmain.h"
#include "cc_tent.h"

void CheckDodge (CBaseEntity *self, vec3f &start, vec3f &dir, sint32 speed)
{
	// easy mode only ducks one quarter the time
	if (CvarList[CV_SKILL].Integer() == 0)
	{
		if (frand() > 0.25)
			return;
	}

	vec3f end = start.MultiplyAngles(8192, dir);
	CTrace tr (start, end, self, CONTENTS_MASK_SHOT);

	if ((tr.ent) && (tr.ent->Entity) && (tr.ent->Entity->EntityFlags & ENT_MONSTER) && (entity_cast<CHurtableEntity>(tr.ent->Entity)->Health > 0) && IsInFront(tr.Ent, self))
	{
		CMonsterEntity *Monster = (entity_cast<CMonsterEntity>(tr.ent->Entity));
		if (Monster->Enemy != self)
			Monster->Enemy = self;

		float eta = ((tr.EndPos - start).LengthFast() - tr.ent->maxs[0]) / speed;
		Monster->Monster->Dodge (self, eta
#if MONSTER_USE_ROGUE_AI
		, &tr
#endif
		);
	}
}

/*
=================
CGrenade
=================
*/
enum // EGrenadeFlags
{
	GRENADE_HAND = BIT(0),
	GRENADE_HELD = BIT(1),
};

CGrenade::CGrenade () :
  CBaseEntity(),
  CBounceProjectile(),
  CTouchableEntity(),
  CThinkableEntity()
{
};

CGrenade::CGrenade (sint32 Index) :
  CBaseEntity(Index),
  CBounceProjectile(Index),
  CTouchableEntity(Index),
  CThinkableEntity(Index)
{
};

IMPLEMENT_SAVE_SOURCE(CGrenade)

void CGrenade::Explode ()
{
	EMeansOfDeath			mod;

	if (GetOwner() && (GetOwner()->EntityFlags & ENT_PLAYER))
		entity_cast<CPlayerEntity>(GetOwner())->PlayerNoiseAt (State.GetOrigin (), PNOISE_IMPACT);

	//FIXME: if we are onground then raise our Z just a bit since we are a point?
	if (Enemy)
	{
		CHurtableEntity *HurtEnemy = entity_cast<CHurtableEntity>(Enemy);

		vec3f v = (HurtEnemy->GetMins() + HurtEnemy->GetMaxs());
		v = (State.GetOrigin () - HurtEnemy->State.GetOrigin().MultiplyAngles (0.5f, v));

		float points = ((float)Damage) - 0.5 * v.Length();
		vec3f dir = HurtEnemy->State.GetOrigin() - State.GetOrigin ();

		HurtEnemy->TakeDamage	(this, GetOwner(), dir, State.GetOrigin (), vec3fOrigin, (sint32)points, (sint32)points,
							DAMAGE_RADIUS, (SpawnFlags & GRENADE_HAND) ? MOD_HANDGRENADE : MOD_GRENADE);
	}

	if (SpawnFlags & GRENADE_HELD)
		mod = MOD_HELD_GRENADE;
	else if (SpawnFlags & GRENADE_HAND)
		mod = MOD_HG_SPLASH;
	else
		mod = MOD_G_SPLASH;

	SplashDamage(GetOwner(), Damage, (Enemy) ? Enemy : NULL, RadiusDamage, mod);

	vec3f origin = State.GetOrigin ().MultiplyAngles (-0.02f, Velocity);
	if (GroundEntity)
		CGrenadeExplosion(CTempEntFlags(CAST_MULTI, CASTFLAG_PHS), origin, !!WaterInfo.Level).Send();
	else
		CRocketExplosion(CTempEntFlags(CAST_MULTI, CASTFLAG_PHS), origin, !!WaterInfo.Level).Send();

	Free (); // "delete" the entity
}

void CGrenade::Touch (CBaseEntity *Other, plane_t *plane, cmBspSurface_t *surf)
{
	if (Other == GetOwner())
		return;

	if (surf && (surf->flags & SURF_TEXINFO_SKY))
	{
		Free (); // "delete" the entity
		return;
	}

	if (!((Other->EntityFlags & ENT_HURTABLE) && entity_cast<CHurtableEntity>(Other)->CanTakeDamage))
	{
		if (SpawnFlags & GRENADE_HAND)
		{
			if (frand() > 0.5)
				PlaySound (CHAN_VOICE, SoundIndex ("weapons/hgrenb1a.wav"));
			else
				PlaySound (CHAN_VOICE, SoundIndex ("weapons/hgrenb2a.wav"));
		}
		else
			PlaySound (CHAN_VOICE, SoundIndex ("weapons/grenlb1b.wav"));
		return;
	}

	Enemy = Other;
	Explode ();
}

void CGrenade::PushInDirection (vec3f vel)
{
	Velocity = vel;
}

void CGrenade::Think ()
{
	Explode();
}

void CGrenade::Spawn (CBaseEntity *Spawner, vec3f start, vec3f aimdir, sint32 Damage, sint32 speed, FrameNumber_t timer, float damage_radius, bool handNade, bool held)
{
	CGrenade	*Grenade = QNewEntityOf CGrenade();
	vec3f		forward, right, up;

	vec3f dir = aimdir.ToAngles();

	dir.ToVectors (&forward, &right, &up);

	Grenade->State.GetOrigin() = start;
	aimdir *= speed;

	Grenade->Velocity = aimdir;
	Grenade->Velocity = Grenade->Velocity.MultiplyAngles (200 + crand() * 10.0f, up);
	Grenade->Velocity = Grenade->Velocity.MultiplyAngles (crand() * 10.0, right);

	Grenade->State.GetEffects() = EF_GRENADE;
	Grenade->State.GetModelIndex() = (!handNade) ? ModelIndex ("models/objects/grenade/tris.md2") : ModelIndex ("models/objects/grenade2/tris.md2");
	Grenade->SetOwner(Spawner);
	Grenade->NextThink = Level.Frame + timer;
	Grenade->Damage = Damage;
	Grenade->RadiusDamage = damage_radius;
	Grenade->ClassName = (!handNade) ? "grenade" : "hgrenade";
	Grenade->GetClipmask() = CONTENTS_MASK_SHOT;
	Grenade->GetSolid() = SOLID_BBOX;
	Grenade->GetMins().Clear ();
	Grenade->GetMaxs().Clear ();
	Grenade->Touchable = true;

	if (handNade)
	{
		Grenade->SpawnFlags = (held) ? (GRENADE_HAND|GRENADE_HELD) : GRENADE_HAND;
		Grenade->State.GetSound() = SoundIndex("weapons/hgrenc1b.wav");
	}

	if (timer <= 0.0)
		Grenade->Explode ();
	else
	{
		if (handNade)
			Spawner->PlaySound (CHAN_WEAPON, SoundIndex ("weapons/hgrent1a.wav"));
		Grenade->Link();
	}
}

bool CGrenade::Run ()
{
	return CBounceProjectile::Run();
}

/*
================
CBlasterProjectile
================
*/

#define HYPER_FLAG		1

CBlasterProjectile::CBlasterProjectile () :
CFlyMissileProjectile(),
CTouchableEntity(),
CThinkableEntity()
{
};

CBlasterProjectile::CBlasterProjectile (sint32 Index) :
CFlyMissileProjectile(Index),
CTouchableEntity(Index),
CThinkableEntity(Index)
{
};

IMPLEMENT_SAVE_SOURCE(CBlasterProjectile)

void CBlasterProjectile::Think ()
{
	Free();
}

void CBlasterProjectile::Touch (CBaseEntity *Other, plane_t *plane, cmBspSurface_t *surf)
{
	if (Other == GetOwner())
		return;

	if (surf && (surf->flags & SURF_TEXINFO_SKY))
	{
		Free (); // "delete" the entity
		return;
	}

	if (GetOwner() && (GetOwner()->EntityFlags & ENT_PLAYER))
		entity_cast<CPlayerEntity>(GetOwner())->PlayerNoiseAt (State.GetOrigin (), PNOISE_IMPACT);

	if ((Other->EntityFlags & ENT_HURTABLE) && entity_cast<CHurtableEntity>(Other)->CanTakeDamage)
		entity_cast<CHurtableEntity>(Other)->TakeDamage (this, GetOwner(), Velocity, State.GetOrigin (), plane ? plane->normal : vec3fOrigin, Damage, 1, DAMAGE_ENERGY, (SpawnFlags & HYPER_FLAG) ? MOD_HYPERBLASTER : MOD_BLASTER);
	else
		CBlasterSplash(State.GetOrigin(), plane ? plane->normal : vec3fOrigin).Send();

	Free (); // "delete" the entity
}

void CBlasterProjectile::Spawn (CBaseEntity *Spawner, vec3f start, vec3f dir,
						sint32 Damage, sint32 speed, sint32 effect, bool isHyper)
{
	CBlasterProjectile		*Bolt = QNewEntityOf CBlasterProjectile;

	Bolt->GetSvFlags() |= (SVF_DEADMONSTER | SVF_PROJECTILE);
	Bolt->State.GetOrigin() = start;
	Bolt->State.GetOldOrigin() = start;
	Bolt->State.GetAngles() = dir.ToAngles();
	Bolt->Velocity = dir.GetNormalizedFast() * speed;

	Bolt->State.GetEffects() = effect;
	Bolt->State.GetModelIndex() = ModelIndex ("models/objects/laser/tris.md2");

	Bolt->State.GetSound() = SoundIndex ("misc/lasfly.wav");
	Bolt->SetOwner (Spawner);
	Bolt->NextThink = Level.Frame + 20;
	Bolt->Damage = Damage;
	Bolt->ClassName = "bolt";
	if (isHyper)
		Bolt->SpawnFlags = HYPER_FLAG;
	Bolt->GetClipmask() = CONTENTS_MASK_SHOT;
	Bolt->GetSolid() = SOLID_BBOX;
	Bolt->GetMins().Clear ();
	Bolt->GetMaxs().Clear ();
	Bolt->Touchable = true;
	Bolt->Link ();

	CTrace tr ((Spawner) ? Spawner->State.GetOrigin() : start, start, Bolt, CONTENTS_MASK_SHOT);
	if (tr.fraction < 1.0)
	{
		start = start.MultiplyAngles (-10, dir.GetNormalizedFast());
		Bolt->State.GetOrigin() = start;
		Bolt->State.GetOldOrigin() = start;

		if (tr.ent->Entity)
			Bolt->Touch (tr.ent->Entity, &tr.plane, tr.surface);
	}
	else if (Spawner && (Spawner->EntityFlags & ENT_PLAYER))
		CheckDodge (Spawner, start, dir, speed);
}

bool CBlasterProjectile::Run ()
{
	return CFlyMissileProjectile::Run();
}

CRocket::CRocket () :
CFlyMissileProjectile(),
CTouchableEntity(),
CThinkableEntity()
{
};

CRocket::CRocket (sint32 Index) :
CFlyMissileProjectile(Index),
CTouchableEntity(Index),
CThinkableEntity(Index)
{
};

IMPLEMENT_SAVE_SOURCE(CRocket)

void CRocket::Think ()
{
	Free (); // "delete" the entity
}

void CRocket::Touch (CBaseEntity *Other, plane_t *plane, cmBspSurface_t *surf)
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
	CRocketExplosion(CTempEntFlags(CAST_MULTI, CASTFLAG_PHS), origin, !!WaterInfo.Level).Send();

	Free ();
}

CRocket *CRocket::Spawn	(CBaseEntity *Spawner, vec3f start, vec3f dir,
						sint32 Damage, sint32 speed, float damage_radius, sint32 radius_damage)
{
	CRocket	*Rocket = QNewEntityOf CRocket;

	Rocket->State.GetOrigin() = start;
	Rocket->State.GetAngles() = dir.ToAngles();
	Rocket->Velocity = dir * speed;
	Rocket->State.GetEffects() = EF_ROCKET;
	Rocket->State.GetModelIndex() = ModelIndex ("models/objects/rocket/tris.md2");
	Rocket->SetOwner (Spawner);
	Rocket->NextThink = Level.Frame + 80000/speed;
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

bool CRocket::Run ()
{
	return CFlyMissileProjectile::Run();
}

CBFGBolt::CBFGBolt () :
CFlyMissileProjectile(),
CTouchableEntity(),
CThinkableEntity()
{
	Exploded = false;
};

CBFGBolt::CBFGBolt (sint32 Index) :
CFlyMissileProjectile(Index),
CTouchableEntity(Index),
CThinkableEntity(Index)
{
	Exploded = false;
};

IMPLEMENT_SAVE_SOURCE(CBFGBolt)

void CBFGBolt::Think ()
{
	// bfg_explode
	if (Exploded)
	{
		if (State.GetFrame() == 0)
		{
			// the BFG effect
			CHurtableEntity *Entity = NULL;
			while ((Entity = FindRadius<CHurtableEntity, ENT_HURTABLE> (Entity, State.GetOrigin (), DamageRadius)) != NULL)
			{
				if (!Entity->CanTakeDamage)
					continue;
				if (Entity == GetOwner())
					continue;
				if (!Entity->CanDamage (this) || !Entity->CanDamage (GetOwner()))
					continue;

				float dist = (State.GetOrigin() - Entity->State.GetOrigin().MultiplyAngles (0.5f, (Entity->GetMins() + Entity->GetMaxs()))).Length();
				float points = Damage * (1.0f - sqrtf(dist/DamageRadius));
	
				if (Entity == GetOwner())
					points = points * 0.5;

				CBFGExplosion(Entity->State.GetOrigin()).Send();
				Entity->TakeDamage (this, GetOwner(), Velocity, Entity->State.GetOrigin(), vec3fOrigin, (sint32)points, 0, DAMAGE_ENERGY, MOD_BFG_EFFECT);
			}
		}

		NextThink = Level.Frame + FRAMETIME;
		State.GetFrame()++;
		if (State.GetFrame() == 5)
			Free ();

		return;
	}
	else // bfg_think
	{
		if (FreeTime < Level.Frame)
		{
			Free();
			return;
		}

		CHurtableEntity	*Entity = NULL;

		const sint32 dmg = (Game.GameMode & GAME_DEATHMATCH) ? 5 : 10;

		while ((Entity = FindRadius<CHurtableEntity, ENT_HURTABLE> (Entity, State.GetOrigin (), 256)) != NULL)
		{
			if (Entity == GetOwner())
				continue;

			if (!Entity->CanTakeDamage)
				continue;

	#if CLEANCTF_ENABLED
	//ZOID
			//don't target players in CTF
			if ((Game.GameMode & GAME_CTF) && (Entity->EntityFlags & ENT_PLAYER) &&
				(GetOwner() && (GetOwner()->EntityFlags & ENT_PLAYER)))
			{
				if ((entity_cast<CPlayerEntity>(Entity))->Client.Respawn.CTF.Team == (entity_cast<CPlayerEntity>(GetOwner()))->Client.Respawn.CTF.Team)
					continue;
			}
	//ZOID
	#endif

			vec3f point = Entity->GetAbsMin().MultiplyAngles (0.5f, Entity->GetSize());

			vec3f dir = point - State.GetOrigin();
			dir.Normalize ();

			CBaseEntity *ignore = this;
			vec3f start = State.GetOrigin();
			vec3f end = start.MultiplyAngles (2048, dir);
			CTrace tr;

			CBFGLaser LaserTempEnt (vec3fOrigin, vec3fOrigin);

			while(1)
			{
				tr (start, end, ignore, CONTENTS_SOLID|CONTENTS_MONSTER|CONTENTS_DEADMONSTER);

				if (!tr.ent || !tr.Ent)
					break;

				// hurt it if we can
				if (((tr.Ent->EntityFlags & ENT_HURTABLE) && entity_cast<CHurtableEntity>(tr.Ent)->CanTakeDamage) && !(tr.Ent->Flags & FL_IMMUNE_LASER) && (tr.Ent != GetOwner()))
					entity_cast<CHurtableEntity>(tr.Ent)->TakeDamage (this, GetOwner(), dir, tr.EndPos, vec3fOrigin, dmg, 1, DAMAGE_ENERGY, MOD_BFG_LASER);

				// if we hit something that's not a monster or player we're done
				//if (!(tr.ent->svFlags & SVF_MONSTER) && (!tr.ent->client))
				if (!(tr.Ent->EntityFlags & ENT_MONSTER) && !(tr.Ent->EntityFlags & ENT_PLAYER))
				{
					CSparks (tr.EndPos, tr.plane.normal, ST_LASER_SPARKS, State.GetSkinNum(), 4).Send();
					break;
				}

				ignore = tr.Ent;
				start = tr.EndPos;
			}

			LaserTempEnt.StartAt(State.GetOrigin()).EndAt(tr.EndPos).Send();
		}

		NextThink = Level.Frame + FRAMETIME;
	}
}

void CBFGBolt::Touch (CBaseEntity *Other, plane_t *plane, cmBspSurface_t *surf)
{
	if (Exploded)
		return;

	if (Other == GetOwner())
		return;

	if (surf && (surf->flags & SURF_TEXINFO_SKY))
	{
		Free();
		return;
	}

	if (GetOwner() && (GetOwner()->EntityFlags & ENT_PLAYER))
		entity_cast<CPlayerEntity>(GetOwner())->PlayerNoiseAt (State.GetOrigin(), PNOISE_IMPACT);

	// core explosion - prevents firing it into the wall/floor
	if ((Other->EntityFlags & ENT_HURTABLE) && entity_cast<CHurtableEntity>(Other)->CanTakeDamage)
		entity_cast<CHurtableEntity>(Other)->TakeDamage (this, GetOwner(), Velocity, State.GetOrigin(), (plane) ? plane->normal : vec3fOrigin, 200, 0, 0, MOD_BFG_BLAST);
	SplashDamage(GetOwner(), 200, Other, 100, MOD_BFG_BLAST);

	PlaySound (CHAN_VOICE, SoundIndex ("weapons/bfg__x1b.wav"));
	GetSolid() = SOLID_NOT;

	Exploded = true;
	State.GetOrigin() = State.GetOrigin().MultiplyAngles (-0.1f, Velocity);
	Velocity.Clear ();
	State.GetModelIndex() = ModelIndex ("sprites/s_bfg3.sp2");
	State.GetFrame() = 0;
	State.GetSound() = 0;
	State.GetEffects() = EF_BFG;
	NextThink = Level.Frame + FRAMETIME;
	Enemy = Other;

	CBFGExplosion(State.GetOrigin(), true).Send();
}

void CBFGBolt::Spawn	(CBaseEntity *Spawner, vec3f start, vec3f dir,
						sint32 Damage, sint32 speed, float damage_radius)
{
	CBFGBolt	*BFG = QNewEntityOf CBFGBolt;

	BFG->State.GetOrigin() = start;
	BFG->State.GetAngles() = dir.ToAngles();
	BFG->Velocity = (dir.GetNormalizedFast()) * speed;
	BFG->State.GetEffects() = EF_BFG | EF_ANIM_ALLFAST;
	BFG->State.GetModelIndex() = ModelIndex ("sprites/s_bfg1.sp2");
	BFG->SetOwner (Spawner);
	BFG->NextThink = Level.Frame + FRAMETIME;
	BFG->Damage = Damage;
	BFG->DamageRadius = damage_radius;
	BFG->State.GetSound() = SoundIndex ("weapons/bfg__l1a.wav");
	BFG->ClassName = "bfg blast";
	BFG->FreeTime = Level.Frame + 80000/speed;
	BFG->GetClipmask() = CONTENTS_MASK_SHOT;
	BFG->GetSolid() = SOLID_BBOX;
	BFG->GetMins().Clear ();
	BFG->GetMaxs().Clear ();
	BFG->Touchable = true;

	BFG->Link ();
}

bool CBFGBolt::Run ()
{
	return CFlyMissileProjectile::Run();
}

CTrace CHitScan::DoTrace(vec3f &start, vec3f &end, CBaseEntity *ignore, sint32 mask)
{
	return CTrace (start, end, (ignore) ? ignore : NULL, mask);
}

bool CHitScan::DoDamage (CBaseEntity *Attacker, CHurtableEntity *Target, vec3f &dir, vec3f &point, vec3f &normal)
{
	return true;
}

void CHitScan::DoEffect	(vec3f &start, vec3f &end, bool water)
{
};

void CHitScan::DoSolidHit	(CTrace *Trace)
{
};

bool CHitScan::ModifyEnd(vec3f &aimDir, vec3f &start, vec3f &end)
{
	return false;
};

void CHitScan::DoWaterHit (CTrace *Trace)
{
}

#define HITSCANSTEP 100
void CHitScan::DoFire(CBaseEntity *Entity, vec3f start, vec3f aimdir)
{
	vec3f end, from;
	vec3f lastWaterStart, lastWaterEnd;
	
	vec3f lastDrawFrom;
	bool DrawIsWater = false;

	// Calculate end
	if (!ModifyEnd(aimdir, start, end))
		end = start.MultiplyAngles (8192, aimdir);

	from = start;
	lastDrawFrom = from;

	sint32 Mask = CONTENTS_MASK_SHOT|CONTENTS_MASK_WATER;
	bool Water = false;
	CBaseEntity *Ignore = Entity;

	lastWaterStart = start;

	bool hitOutOfWater = false;
	if (PointContents(start) & CONTENTS_MASK_WATER)
	{
		// Copy up our point for the effect
		lastWaterEnd = start;
	
		// Special case if we started in water
		Water = true;
		DrawIsWater = true;
		Mask = CONTENTS_MASK_SHOT;

		// Find the exit point
		sint32 tries = 20; // Cover about 2000 units
		vec3f	stWater;

		stWater = from;
		lastWaterStart.Clear();
		
		while (tries > 0)
		{
			stWater = stWater.MultiplyAngles (HITSCANSTEP, aimdir);

			EBrushContents contents = PointContents(stWater);
			if (contents == 0) // "Clear" or solid
				break; // Got it
			else if (contents & CONTENTS_MASK_SOLID)
			{
				// This is a special case in case we run into a solid.
				// This basically means that the trace is done, so we can skip ahead right to the solid (act like
				// we're not in water)
				tries = 0;
				break;
			}
			tries --;
		}

		if (tries != 0)
		{
			// We reached air
			// Trace backwards and grab the water brush
			vec3f tempOrigin;
			tempOrigin = stWater.MultiplyAngles (-(HITSCANSTEP + 5), aimdir);
			CTrace tempTrace = DoTrace (stWater, tempOrigin, NULL, CONTENTS_MASK_WATER);

			if (tempTrace.contents & CONTENTS_MASK_WATER)// All is good
			{
				// This is our end
				lastWaterStart = tempTrace.EndPos;
				hitOutOfWater = true;
			}
		}
		// We didn't reach air if we got here.
		// Let water handle it, it will act as solid.
	}

	// Main loop
	while (1)
	{
		// Trace from start to our end
		CTrace Trace = DoTrace (from, end, Ignore, Mask);

		// Did we hit an entity?
		if (Trace.ent && Trace.Ent && ((Trace.Ent->EntityFlags & ENT_HURTABLE) && entity_cast<CHurtableEntity>(Trace.Ent)->CanTakeDamage))
		{
			// Convert to base entity
			CHurtableEntity *Target = entity_cast<CHurtableEntity>(Trace.Ent);

			// Hurt it
			// Revision
			// Startsolid mistake..
			if (Trace.startSolid)
			{
				vec3f origin = Target->State.GetOrigin();
				if (!DoDamage (Entity, Target, aimdir, origin, Trace.plane.normal))
				{
					DoEffect (origin, lastDrawFrom, DrawIsWater);
					break; // We wanted to stop
				}

				// Set up the start from where we are now
				vec3f oldFrom = from;
				from = origin;

				// Revision: Stop on solids
				if (ThroughAndThrough && Target->GetSolid() == SOLID_BSP)
				{
					// Draw the effect
					//DoEffect (from, oldFrom, false);

					DoSolidHit (&Trace);
					break;
				}

				//DoEffect (from, oldFrom, Water);

				// and ignore the bastard
				Ignore = Target;

				// Continue our loop
				continue;
			}

			if (!DoDamage (Entity, Target, aimdir, Trace.EndPos, Trace.plane.normal))
				break; // We wanted to stop

			// Set up the start from where we are now
			vec3f oldFrom = from;
			from = Trace.EndPos;

			// Revision: Stop on solids
			if (ThroughAndThrough && Target->GetSolid() == SOLID_BSP)
			{
				// Draw the effect
				//DoEffect (from, oldFrom, false);

				DoEffect (from, oldFrom, DrawIsWater);
				DoSolidHit (&Trace);
				break;
			}

			//DoEffect (from, oldFrom, Water);

			// and ignore the bastard
			Ignore = Target;

			// Continue our loop
			continue;
		}
		// If we hit something in water...
		else if (Water)
		{
			Water = false;
			// Assume solid
			//if (Trace.contents & CONTENTS_MASK_SOLID)
			{
				// If we didn't grab water last time...
				if (lastWaterStart == vec3fOrigin)
				{
					// We hit the ground!
					// Swap start and end points
					lastWaterStart = lastWaterEnd;
					lastWaterStart = lastWaterStart.MultiplyAngles (5, aimdir);

					// Set end point
					lastWaterEnd = Trace.EndPos;

					// Draw the effect
					DoEffect (lastWaterStart, lastWaterEnd, true);

					DoSolidHit (&Trace);

					break; // We're done
				}
				// Otherwise we had found an exit point
				else
				{
					// Draw from water surface to water end
					DoEffect (lastWaterEnd, lastWaterStart, true);
	
					// We hit the ground!
					// Swap start and end points
					if (!hitOutOfWater)
						lastWaterStart = lastWaterEnd;
					else
						hitOutOfWater = false;

					// Set end point
					lastWaterEnd = Trace.EndPos;

					// Draw the effect
					DoEffect (lastWaterStart, lastWaterEnd, false);

					DoSolidHit (&Trace);

					break; // We're done
				}
			}
			continue;
		}
		// If we hit non-transparent water
		else if ((Trace.contents & CONTENTS_MASK_WATER) &&
			(Trace.surface && !(Trace.surface->flags & (SURF_TEXINFO_TRANS33|SURF_TEXINFO_TRANS66))))
		{
			// Copy up our point for the effect
			lastWaterEnd = Trace.EndPos;

			// Tell the system we're in water
			Water = true;

			// Draw the effect
			DoEffect (lastWaterStart, lastWaterEnd, false);

			DoWaterHit (&Trace);

			// Set up the start from where we are now
			from = lastDrawFrom = Trace.EndPos;
			Mask = CONTENTS_MASK_SHOT;

			// Find the exit point
			sint32 tries = 20; // Cover about 2000 units
			vec3f	stWater = from;
			lastWaterStart.Clear();
			
			while (tries > 0)
			{
				stWater = stWater.MultiplyAngles (HITSCANSTEP, aimdir);

				EBrushContents contents = PointContents(stWater);
				if (contents == 0) // "Clear" or solid
					break; // Got it
				else if (contents & CONTENTS_MASK_SOLID)
				{
					// This is a special case in case we run into a solid.
					// This basically means that the trace is done, so we can skip ahead right to the solid (act like
					// we're not in water)
					tries = 0;
					break;
				}
				tries --;
			}

			if (tries != 0)
			{
				// We reached air
				// Trace backwards and grab the water brush
				vec3f tempOrigin = stWater.MultiplyAngles (-(HITSCANSTEP + 5), aimdir);
				CTrace tempTrace = DoTrace (stWater, tempOrigin, NULL, CONTENTS_MASK_WATER);

				if (tempTrace.contents & CONTENTS_MASK_WATER) // All is good
				{
					// This is our end
					lastWaterStart = tempTrace.EndPos;
					continue; // Head to the next area
				}
			}
			// We didn't reach air if we got here.
			// Let water handle it, it will act as solid.

			// Continue the loop
			continue;
		}
		// Transparent water
		else if ((Trace.contents & CONTENTS_MASK_WATER) &&
			(Trace.surface && (Trace.surface->flags & (SURF_TEXINFO_TRANS33|SURF_TEXINFO_TRANS66))))
		{
			// This won't count as "water" since we can see through it.
			// It has the same PVS, meaning we don't need to
			// do complex tracing.

			if (ThroughAndThrough)
			{
				// Keep going
				from = Trace.EndPos.MultiplyAngles (0.1f, aimdir);

				// Water hit effect
				DoWaterHit (&Trace);
			}
			else
			{
				// Copy up our point for the effect
				lastWaterEnd = Trace.EndPos;

				// Draw the effect we have so far
				DoEffect (from, Trace.EndPos, false);

				// Water hit effect
				DoWaterHit (&Trace);

				// Set up water drawing
				Water = true;
				Mask = CONTENTS_MASK_SHOT;

				// Find the exit point
				sint32 tries = 20; // Cover about 2000 units
				vec3f	stWater = from;
				lastWaterStart.Clear();
				
				while (tries > 0)
				{
					stWater = stWater.MultiplyAngles (HITSCANSTEP, aimdir);

					EBrushContents contents = PointContents(stWater);
					if (contents == 0) // "Clear" or solid
						break; // Got it
					else if (contents & CONTENTS_MASK_SOLID)
					{
						// This is a special case in case we run into a solid.
						// This basically means that the trace is done, so we can skip ahead right to the solid (act like
						// we're not in water)
						tries = 0;
						break;
					}
					tries --;
				}

				if (tries != 0)
				{
					// We reached air
					// Trace backwards and grab the water brush
					vec3f tempOrigin = stWater.MultiplyAngles (-(HITSCANSTEP + 5), aimdir);
					CTrace tempTrace = DoTrace (stWater, tempOrigin, NULL, CONTENTS_MASK_WATER);

					if (tempTrace.contents & CONTENTS_MASK_WATER) // All is good
					{
						// This is our end
						lastWaterStart = tempTrace.EndPos;
						continue; // Head to the next area
					}
				}
				// We didn't reach air if we got here.
				// Let water handle it, it will act as solid.

				// Continue the loop
			}

			continue;
		}
		// Assume solid
		else
		{
			// Draw the effect
			DoEffect (lastDrawFrom, Trace.EndPos, DrawIsWater);
			DoSolidHit (&Trace);
			break; // We're done
		}
	}
}

bool CRailGunShot::DoDamage (CBaseEntity *Attacker, CHurtableEntity *Target, vec3f &dir, vec3f &point, vec3f &normal)
{
	if (Attacker != Target) // Hurt self protection
		Target->TakeDamage (Attacker, Attacker, dir, point, normal, Damage, Kick, 0, MOD_RAILGUN);
	return ThroughAndThrough;
}

void CRailGunShot::DoEffect	(vec3f &start, vec3f &end, bool water)
{
	CRailTrail(start, end).Send();
}

void CRailGunShot::Fire(CBaseEntity *Entity, vec3f start, vec3f aimdir, sint32 Damage, sint32 kick)
{
	CRailGunShot(Damage, kick).DoFire (Entity, start, aimdir);
}

bool CBullet::DoDamage (CBaseEntity *Attacker, CHurtableEntity *Target, vec3f &dir, vec3f &point, vec3f &normal)
{
	Target->TakeDamage (Attacker, Attacker, dir, point, normal, Damage, Kick, DAMAGE_BULLET, MeansOfDeath);
	return ThroughAndThrough;
}

void CBullet::DoSolidHit	(CTrace *Trace)
{
	if (!(Trace->surface->flags & SURF_TEXINFO_SKY))
		CGunshotRicochet(Trace->EndPos, Trace->plane.normal).Send();
}

bool CBullet::ModifyEnd (vec3f &aimDir, vec3f &start, vec3f &end)
{
	vec3f forward, right, up;
	aimDir.ToAngles().ToVectors (&forward, &right, &up);

	end = start.MultiplyAngles (8192, forward).MultiplyAngles (crand()*hSpread, right).MultiplyAngles (crand()*vSpread, up);
	return true;
}

void CBullet::DoEffect	(vec3f &start, vec3f &end, bool water)
{
	if (water)
		CBubbleTrail(start, end).Send();
}

void CBullet::DoWaterHit	(CTrace *Trace)
{
	ESplashType Color;
	if (Trace->contents & CONTENTS_WATER)
	{
		if (strcmp(Trace->surface->name, "*brwater") == 0)
			Color = SPT_MUD;
		else
			Color = SPT_WATER;
	}
	else if (Trace->contents & CONTENTS_SLIME)
		Color = SPT_SLIME;
	else if (Trace->contents & CONTENTS_LAVA)
		Color = SPT_LAVA;
	else
		return;

	CSplash(Trace->EndPos, Trace->plane.normal, Color).Send();
}

void CBullet::Fire(CBaseEntity *Entity, vec3f start, vec3f aimdir, sint32 Damage, sint32 kick, sint32 hSpread, sint32 vSpread, sint32 mod)
{
	CBullet(Damage, kick, hSpread, vSpread, mod).DoFire (Entity, start, aimdir);
}

// An overload to handle transparent water
void CBullet::DoFire(CBaseEntity *Entity, vec3f start, vec3f aimdir)
{
	CHitScan::DoFire (Entity, start, aimdir);
}

void CShotgunPellets::DoSolidHit	(CTrace *Trace)
{
	if (!(Trace->surface->flags & SURF_TEXINFO_SKY))
		CShotgunRicochet(Trace->EndPos, Trace->plane.normal).Send();
}

extern bool LastPelletShot;
void CShotgunPellets::Fire(CBaseEntity *Entity, vec3f start, vec3f aimdir, sint32 Damage, sint32 kick, sint32 hSpread, sint32 vSpread, sint32 Count, sint32 mod)
{
	LastPelletShot = false;
	for (sint32 i = 0; i < Count; i++)
	{
		if (i == Count - 1)
			LastPelletShot = true;

		CShotgunPellets(Damage, kick, hSpread, vSpread, mod).DoFire (Entity, start, aimdir);
	}
}

bool CMeleeWeapon::Fire(CBaseEntity *Entity, vec3f aim, sint32 Damage, sint32 kick)
{
	vec3f		forward, right, up, point, dir;
	float		range;

	//see if enemy is in range
	CBaseEntity *Enemy = Entity->Enemy;

	if (!Enemy)
		return false;

	dir = Enemy->State.GetOrigin() - Entity->State.GetOrigin();
	range = dir.Length();
	if (range > aim.X)
		return false;

	if (aim.Y > Entity->GetMins().X && aim.Y < Entity->GetMaxs().X)
		// the hit is straight on so back the range up to the edge of their bbox
		range -= Enemy->GetMaxs().X;
	else
	{
		// this is a side hit so adjust the "right" value out to the edge of their bbox
		if (aim.Y < 0)
			aim.Y = Enemy->GetMins().X;
		else
			aim.Y = Enemy->GetMaxs().X;
	}

	point = Entity->State.GetOrigin().MultiplyAngles (range, dir);

	CTrace tr (Entity->State.GetOrigin(), point, Entity, CONTENTS_MASK_SHOT);
	if (tr.fraction == 1.0)
		return false;

	if (!((tr.Ent->EntityFlags & ENT_HURTABLE) && entity_cast<CHurtableEntity>(tr.Ent)->CanTakeDamage))
		return false;

	// if it will hit any client/monster then hit the one we wanted to hit
	CBaseEntity *Hit = tr.Ent;
	if ((tr.Ent->EntityFlags & ENT_MONSTER) || ((tr.Ent->EntityFlags & ENT_PLAYER)))
		Hit = Enemy;

	Entity->State.GetAngles().ToVectors (&forward, &right, &up);
	point = Entity->State.GetOrigin().MultiplyAngles (range, forward);
	point = point.MultiplyAngles (aim.Y, right);
	point = point.MultiplyAngles (aim.Z, up);
	dir = point - Enemy->State.GetOrigin();

	// do the damage
	if ((Hit->EntityFlags & ENT_HURTABLE) && entity_cast<CHurtableEntity>(Hit)->CanTakeDamage)
		entity_cast<CHurtableEntity>(Hit)->TakeDamage (Entity, Entity, dir, point, vec3fOrigin, Damage, kick/2, DAMAGE_NO_KNOCKBACK, MOD_HIT);

	if (!(Hit->EntityFlags & ENT_MONSTER) && (!(Hit->EntityFlags & ENT_PLAYER)))
		return false;

	// do our special form of knockback here
	if (Enemy->EntityFlags & ENT_PHYSICS)
	{
		CPhysicsEntity *PhysEnemy = entity_cast<CPhysicsEntity>(Enemy);
		vec3f v = PhysEnemy->GetAbsMin().MultiplyAngles (0.5f, PhysEnemy->GetSize()) - point;
		v.Normalize ();
		PhysEnemy->Velocity = v.MultiplyAngles (kick, v);
		if (PhysEnemy->Velocity.Z > 0)
			PhysEnemy->GroundEntity = NULL;
	}
	return true;
}

#if CLEANCTF_ENABLED

CGrappleEntity::CGrappleEntity () :
CBaseEntity(),
CFlyMissileProjectile(),
CTouchableEntity()
{
};

CGrappleEntity::CGrappleEntity (sint32 Index) :
CBaseEntity(Index),
CFlyMissileProjectile(Index),
CTouchableEntity(Index)
{
};

void CGrappleEntity::GrappleDrawCable()
{
	vec3f	start, end, f, r, origin = Player->State.GetOrigin ();
	
	Player->Client.ViewAngle.ToVectors (&f, &r, NULL);
	Player->P_ProjectSource (vec3f(16, 16, Player->ViewHeight-8), f, r, start);

	vec3f offset = start - origin;
	vec3f dir = start - State.GetOrigin();

	// don't draw cable if close
	if (dir.LengthFast() < 64)
		return;

	// adjust start for beam origin being in middle of a segment
	end = State.GetOrigin();
	CGrappleCable(origin, end, Player->State.GetNumber(), offset).Send();
};

void CGrappleEntity::GrapplePull()
{
	uint8 volume = (Player->Client.Timers.SilencerShots) ? 51 : 255;

	if ((Player->Client.Persistent.Weapon->Item == NItems::Grapple) &&
		!Player->Client.NewWeapon &&
		(Player->Client.WeaponState != WS_FIRING) &&
		(Player->Client.WeaponState != WS_ACTIVATING))
	{
		ResetGrapple();
		return;
	}

	if (Enemy)
	{
		if (Enemy->GetSolid() == SOLID_NOT)
		{
			ResetGrapple();
			return;
		}

		if (Enemy->GetSolid() == SOLID_BBOX)
		{
			State.GetOrigin() = (Enemy->GetSize() * 0.5f) + Enemy->State.GetOrigin() + Enemy->GetMins();
			Link ();
		}
		else if (Enemy->EntityFlags & ENT_PHYSICS)
			Velocity = entity_cast<CPhysicsEntity>(Enemy)->Velocity;

		if (Enemy && ((Enemy->EntityFlags & ENT_HURTABLE) && entity_cast<CHurtableEntity>(Enemy)->CanTakeDamage))
		{
			CHurtableEntity *Hurt = entity_cast<CHurtableEntity>(Enemy);
			if (!Hurt->CheckTeamDamage (Player))
			{
				Hurt->TakeDamage (this, Player, Velocity, State.GetOrigin(), vec3fOrigin, 1, 1, 0, MOD_GRAPPLE);
				PlaySound (CHAN_WEAPON, SoundIndex("weapons/grapple/grhurt.wav"), volume);
			}
		}
		if (Enemy && (Enemy->EntityFlags & ENT_HURTABLE) && entity_cast<CHurtableEntity>(Enemy)->DeadFlag)
		{ // he died
			ResetGrapple();
			return;
		}
	}

	GrappleDrawCable();

	if (Player->Client.Grapple.State > CTF_GRAPPLE_STATE_FLY)
	{
		// pull player toward grapple
		// this causes icky stuff with prediction, we need to extend
		// the prediction layer to include two new fields in the player
		// move stuff: a point and a velocity.  The client should add
		// that velociy in the direction of the point
		vec3f forward, up;

		Player->Client.ViewAngle.ToVectors (&forward, &up, NULL);
		vec3f v = Player->State.GetOrigin ();
		v.Z += Player->ViewHeight;
		vec3f hookdir = State.GetOrigin() - v;

		float vlen = hookdir.LengthFast();

		if ((Player->Client.Grapple.State == CTF_GRAPPLE_STATE_PULL) &&
			vlen < 64)
		{
			Player->Client.PlayerState.GetPMove()->pmFlags |= PMF_NO_PREDICTION;
			Player->PlaySound (CHAN_WEAPON, SoundIndex("weapons/grapple/grhang.wav"), volume);
			Player->Client.Grapple.State = CTF_GRAPPLE_STATE_HANG;
		}

		hookdir.NormalizeFast ();
		hookdir *= CTF_GRAPPLE_PULL_SPEED;
		Player->Velocity = hookdir;
		Player->AddGravity();
	}
};

void CGrappleEntity::ResetGrapple ()
{
	Player->Client.Grapple.Entity = NULL;
	Player->Client.Grapple.ReleaseTime = Level.Frame;
	Player->Client.Grapple.State = CTF_GRAPPLE_STATE_HANG+1; // we're firing, not on hook
	Player->Client.PlayerState.GetPMove()->pmFlags &= ~PMF_NO_PREDICTION;
	Free ();
};

void CGrappleEntity::Spawn (CPlayerEntity *Spawner, vec3f start, vec3f dir, sint32 Damage, sint32 speed)
{
	CGrappleEntity *Grapple = QNewEntityOf CGrappleEntity;
	Grapple->Player = Spawner;
	Grapple->Damage = Damage;

	dir.NormalizeFast();

	Grapple->State.GetOrigin() = start;
	Grapple->State.GetOldOrigin() = start;
	Grapple->State.GetAngles() = dir.ToAngles();
	Grapple->Velocity = dir * speed;
	Grapple->GetClipmask() = CONTENTS_MASK_SHOT;
	Grapple->GetSolid() = SOLID_BBOX;
	Grapple->GetMins().Clear ();
	Grapple->GetMaxs().Clear ();
	Grapple->State.GetModelIndex() = ModelIndex ("models/weapons/grapple/hook/tris.md2");
	Grapple->SetOwner (Spawner);
	Spawner->Client.Grapple.Entity = Grapple;
	Spawner->Client.Grapple.State = CTF_GRAPPLE_STATE_FLY; // we're firing, not on hook
	Grapple->Touchable = true;
	Grapple->Link ();

	CTrace tr (Spawner->State.GetOrigin(), Grapple->State.GetOrigin(), Grapple, CONTENTS_MASK_SHOT);
	if (tr.fraction < 1.0)
	{
		Grapple->State.GetOrigin() = Grapple->State.GetOrigin().MultiplyAngles (-10, dir);
		Grapple->Touch (tr.ent->Entity, NULL, NULL);
	}
};

void CGrappleEntity::Touch (CBaseEntity *Other, plane_t *plane, cmBspSurface_t *surf)
{
	if (Other == Player)
		return;

	if (Player->Client.Grapple.State != CTF_GRAPPLE_STATE_FLY)
		return;

	if (surf && (surf->flags & SURF_TEXINFO_SKY))
	{
		ResetGrapple();
		return;
	}

	Velocity.Clear ();
	Player->PlayerNoiseAt (State.GetOrigin(), PNOISE_IMPACT);

	if ((Other->EntityFlags & ENT_HURTABLE) && entity_cast<CHurtableEntity>(Other)->CanTakeDamage)
	{
		entity_cast<CHurtableEntity>(Other)->TakeDamage (Other, this, Player, Velocity, State.GetOrigin(), (plane) ? plane->normal : vec3fOrigin, Damage, 1, 0, MOD_GRAPPLE);
		ResetGrapple();
		return;
	}

	Player->Client.Grapple.State = CTF_GRAPPLE_STATE_PULL; // we're on hook
	Enemy = Other;

	GetSolid() = SOLID_NOT;

	uint8 volume = (Player->Client.Timers.SilencerShots) ? 51 : 255;
	Player->PlaySound (CHAN_WEAPON, SoundIndex("weapons/grapple/grpull.wav"), volume);
	PlaySound (CHAN_WEAPON, SoundIndex("weapons/grapple/grhit.wav"), volume);

	CSparks(State.GetOrigin(), (!plane) ? vec3fOrigin : plane->normal).Send();
};

bool CGrappleEntity::Run ()
{
	return CFlyMissileProjectile::Run();
};
#endif

