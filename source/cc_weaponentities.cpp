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

void CheckDodge (CBaseEntity *self, vec3f &start, vec3f &dir, int speed)
{
	vec3f	end;
	CTrace	tr;

	// easy mode only ducks one quarter the time
	if (skill->Integer() == 0)
	{
		if (random() > 0.25)
			return;
	}

	end = start.MultiplyAngles(8192, dir);
	tr = CTrace (start, end, self->gameEntity, CONTENTS_MASK_SHOT);

#ifdef MONSTER_USE_ROGUE_AI
	if ((tr.ent) && (tr.ent->Entity) && (tr.ent->Entity->EntityFlags & ENT_MONSTER) && (tr.ent->health > 0) && IsInFront(tr.Ent, self))
	{
		vec3f v = tr.EndPos - start;
		float eta = (v.LengthFast() - tr.ent->maxs[0]) / speed;
		(dynamic_cast<CMonsterEntity*>(tr.ent->Entity))->Monster->Dodge (self, eta, &tr);

		if (tr.ent->enemy != self->gameEntity)
			tr.ent->enemy = self->gameEntity;
	}
#else
	if ((tr.ent) && (tr.ent->Entity) && (tr.ent->Entity->EntityFlags & ENT_MONSTER) && (tr.ent->health > 0) && IsInFront(tr.Ent, self))
	{
		vec3f v = tr.EndPos - start;
		float eta = (v.LengthFast() - tr.ent->maxs[0]) / speed;
		(dynamic_cast<CMonsterEntity*>(tr.ent->Entity))->Monster->Dodge (self, eta);
	}
#endif
}

/*
=================
CGrenade
=================
*/
enum // EGrenadeFlags
{
	GRENADE_HAND = 1,
	GRENADE_HELD = 2,
};
CGrenade::CGrenade () :
CBounceProjectile(),
CTouchableEntity(),
CThinkableEntity()
{
};

CGrenade::CGrenade (int Index) :
CBounceProjectile(Index),
CTouchableEntity(Index),
CThinkableEntity(Index)
{
};

void CGrenade::Explode ()
{
	EMeansOfDeath			mod;

	vec3f origin = State.GetOrigin ();
	if (gameEntity->owner && gameEntity->owner->client)
		dynamic_cast<CPlayerEntity*>(gameEntity->owner->Entity)->PlayerNoiseAt (origin, PNOISE_IMPACT);

	//FIXME: if we are onground then raise our Z just a bit since we are a point?
	if (gameEntity->enemy)
	{
		CHurtableEntity *Enemy = dynamic_cast<CHurtableEntity*>(gameEntity->enemy->Entity);

		vec3f v = (Enemy->GetMins() + Enemy->GetMaxs());
		v = (origin - Enemy->State.GetOrigin().MultiplyAngles (0.5f, v));

		float points = Damage - 0.5 * v.Length();
		vec3f dir = Enemy->State.GetOrigin() - origin;

		Enemy->TakeDamage	(this, GetOwner(), dir, origin, vec3fOrigin, (int)points, (int)points,
							DAMAGE_RADIUS, (gameEntity->spawnflags & GRENADE_HAND) ? MOD_HANDGRENADE : MOD_GRENADE);
	}

	if (gameEntity->spawnflags & GRENADE_HELD)
		mod = MOD_HELD_GRENADE;
	else if (gameEntity->spawnflags & GRENADE_HAND)
		mod = MOD_HG_SPLASH;
	else
		mod = MOD_G_SPLASH;
	T_RadiusDamage(this, GetOwner(), Damage, (gameEntity->enemy) ? gameEntity->enemy->Entity : NULL, RadiusDamage, mod);

	Vec3MA (origin, -0.02f, gameEntity->velocity, origin);
	if (GroundEntity)
		CTempEnt_Explosions::GrenadeExplosion(origin, gameEntity, !!gameEntity->waterlevel);
	else
		CTempEnt_Explosions::RocketExplosion(origin, gameEntity, !!gameEntity->waterlevel);

	Free (); // "delete" the entity
}

void CGrenade::Touch (CBaseEntity *other, plane_t *plane, cmBspSurface_t *surf)
{
	if (other->gameEntity == gameEntity->owner)
		return;

	if (surf && (surf->flags & SURF_TEXINFO_SKY))
	{
		Free (); // "delete" the entity
		return;
	}

	if (!((other->EntityFlags & ENT_HURTABLE) && dynamic_cast<CHurtableEntity*>(other)->CanTakeDamage))
	{
		if (gameEntity->spawnflags & GRENADE_HAND)
		{
			if (random() > 0.5)
				PlaySound (CHAN_VOICE, SoundIndex ("weapons/hgrenb1a.wav"));
			else
				PlaySound (CHAN_VOICE, SoundIndex ("weapons/hgrenb2a.wav"));
		}
		else
			PlaySound (CHAN_VOICE, SoundIndex ("weapons/grenlb1b.wav"));
		return;
	}

	gameEntity->enemy = other->gameEntity;
	Explode ();
}

void CGrenade::Think ()
{
	Explode();
}

void CGrenade::Spawn (CBaseEntity *Spawner, vec3f start, vec3f aimdir, int damage, int speed, float timer, float damage_radius, bool handNade, bool held)
{
	CGrenade	*Grenade = QNew (com_levelPool, 0) CGrenade();
	vec3f		forward, right, up;

	vec3f dir = aimdir.ToAngles();
	VecToAngles (aimdir, dir);

	dir.ToVectors (&forward, &right, &up);

	Grenade->State.SetOrigin (start);
	aimdir *= speed;
	Grenade->gameEntity->velocity[0] = aimdir.X;
	Grenade->gameEntity->velocity[1] = aimdir.Y;
	Grenade->gameEntity->velocity[2] = aimdir.Z;

	vec3f velocity = aimdir;
	velocity = velocity.MultiplyAngles (200 + crandom() * 10.0f, up);
	velocity = velocity.MultiplyAngles (crandom() * 10.0, right);
	Grenade->gameEntity->velocity[0] = velocity.X;
	Grenade->gameEntity->velocity[1] = velocity.Y;
	Grenade->gameEntity->velocity[2] = velocity.Z;

	Vec3Set (Grenade->AngularVelocity, 300, 300, 300);
	Grenade->State.SetEffects (EF_GRENADE);
	Grenade->State.SetModelIndex((!handNade) ? ModelIndex ("models/objects/grenade/tris.md2") : ModelIndex ("models/objects/grenade2/tris.md2"));
	Grenade->SetOwner(Spawner);
	Grenade->NextThink = level.framenum + (timer * 10);
	Grenade->Damage = damage;
	Grenade->RadiusDamage = damage_radius;
	Grenade->gameEntity->classname = (!handNade) ? "grenade" : "hgrenade";
	if (handNade)
	{
		Grenade->gameEntity->spawnflags = (held) ? (GRENADE_HAND|GRENADE_HELD) : GRENADE_HAND;
		Grenade->State.SetSound (SoundIndex("weapons/hgrenc1b.wav"));
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

CBlasterProjectile::CBlasterProjectile () :
CFlyMissileProjectile(),
CTouchableEntity(),
CThinkableEntity()
{
};

CBlasterProjectile::CBlasterProjectile (int Index) :
CFlyMissileProjectile(Index),
CTouchableEntity(Index),
CThinkableEntity(Index)
{
};

void CBlasterProjectile::Think ()
{
	Free();
}

void CBlasterProjectile::Touch (CBaseEntity *other, plane_t *plane, cmBspSurface_t *surf)
{
	if (other->gameEntity == gameEntity->owner)
		return;

	if (surf && (surf->flags & SURF_TEXINFO_SKY))
	{
		Free (); // "delete" the entity
		return;
	}

	vec3f origin = State.GetOrigin ();
	if (gameEntity->owner->client)
		dynamic_cast<CPlayerEntity*>(gameEntity->owner->Entity)->PlayerNoiseAt (origin, PNOISE_IMPACT);

	if ((other->EntityFlags & ENT_HURTABLE) && dynamic_cast<CHurtableEntity*>(other)->CanTakeDamage)
		dynamic_cast<CHurtableEntity*>(other)->TakeDamage (this, GetOwner(), gameEntity->velocity, origin, plane ? plane->normal : vec3fOrigin, Damage, 1, DAMAGE_ENERGY, (gameEntity->spawnflags & 1) ? MOD_HYPERBLASTER : MOD_BLASTER);
	else
		CTempEnt_Splashes::Blaster(origin, plane ? plane->normal : vec3fOrigin);

	Free (); // "delete" the entity
}

void CBlasterProjectile::Spawn (CBaseEntity *Spawner, vec3f start, vec3f dir,
						int damage, int speed, int effect, bool isHyper)
{
	CBlasterProjectile		*Bolt = QNew (com_levelPool, 0) CBlasterProjectile;

	if (Spawner && Spawner->EntityFlags & ENT_PLAYER)
		CheckDodge (Spawner, start, dir, speed);

	dir.NormalizeFast();

	Bolt->SetSvFlags (SVF_PROJECTILE);
	Bolt->State.SetOrigin (start);
	Bolt->State.SetOldOrigin (start);
	Bolt->State.SetAngles (dir.ToAngles());
	vec3f Scaled = dir * speed;
	Bolt->gameEntity->velocity[0] = Scaled.X;
	Bolt->gameEntity->velocity[1] = Scaled.Y;
	Bolt->gameEntity->velocity[2] = Scaled.Z;

	Bolt->State.SetEffects (effect);
	Bolt->State.SetModelIndex (ModelIndex ("models/objects/laser/tris.md2"));

	Bolt->State.SetSound (SoundIndex ("misc/lasfly.wav"));
	Bolt->SetOwner (Spawner);
	Bolt->NextThink = level.framenum + 20;
	Bolt->Damage = damage;
	Bolt->gameEntity->classname = "bolt";
	if (isHyper)
		Bolt->gameEntity->spawnflags = 1;
	Bolt->Link ();

	CTrace tr = CTrace ((Spawner) ? Spawner->State.GetOrigin() : start, start, Bolt->gameEntity, CONTENTS_MASK_SHOT);
	if (tr.fraction < 1.0)
	{
		start = start.MultiplyAngles (-10, dir);
		Bolt->State.SetOrigin (start);
		Bolt->State.SetOldOrigin (start);

		if (tr.ent->Entity)
			Bolt->Touch (tr.ent->Entity, &tr.plane, tr.surface);
	}
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

CRocket::CRocket (int Index) :
CFlyMissileProjectile(Index),
CTouchableEntity(Index),
CThinkableEntity(Index)
{
};

void CRocket::Think ()
{
	Free (); // "delete" the entity
}

void CRocket::Touch (CBaseEntity *other, plane_t *plane, cmBspSurface_t *surf)
{
	if (other->gameEntity == gameEntity->owner)
		return;

	if (surf && (surf->flags & SURF_TEXINFO_SKY))
	{
		Free ();
		return;
	}

	vec3f origin = State.GetOrigin ();
	if (gameEntity->owner->client)
		dynamic_cast<CPlayerEntity*>(gameEntity->owner->Entity)->PlayerNoiseAt (origin, PNOISE_IMPACT);

	if ((other->EntityFlags & ENT_HURTABLE) && dynamic_cast<CHurtableEntity*>(other)->CanTakeDamage)
		dynamic_cast<CHurtableEntity*>(other)->TakeDamage (this, GetOwner(), gameEntity->velocity, origin, (plane) ? plane->normal : vec3fOrigin, Damage, 0, 0, MOD_ROCKET);
/*	else
	{
		// don't throw any debris in net games
		if (game.mode == GAME_SINGLEPLAYER)
		{
			if ((surf) && !(surf->flags & (SURF_TEXINFO_WARP|SURF_TEXINFO_TRANS33|SURF_TEXINFO_TRANS66|SURF_TEXINFO_FLOWING)))
			{
				for (int n = 0; n < randomMT()%5; n++)
					ThrowDebris (gameEntity, "models/objects/debris2/tris.md2", 2, origin);
			}
		}
	}*/

	// calculate position for the explosion entity
	Vec3MA (origin, -0.02f, gameEntity->velocity, origin);
	T_RadiusDamage(this, GetOwner(), RadiusDamage, other, DamageRadius, MOD_R_SPLASH);
	CTempEnt_Explosions::RocketExplosion(origin, gameEntity, !!gameEntity->waterlevel);

	Free ();
}

void CRocket::Spawn	(CBaseEntity *Spawner, vec3f start, vec3f dir,
						int damage, int speed, float damage_radius, int radius_damage)
{
	CRocket	*Rocket = QNew (com_levelPool, 0) CRocket;

	Rocket->State.SetOrigin (start);

	Rocket->State.SetAngles (dir.ToAngles());
	vec3f vel = dir * speed;
	Rocket->gameEntity->velocity[0] = vel.X;
	Rocket->gameEntity->velocity[1] = vel.Y;
	Rocket->gameEntity->velocity[2] = vel.Z;
	Rocket->State.SetEffects (EF_ROCKET);
	Rocket->State.SetModelIndex (ModelIndex ("models/objects/rocket/tris.md2"));
	Rocket->SetOwner (Spawner);
	Rocket->NextThink = level.framenum + 80000/speed;
	Rocket->Damage = damage;
	Rocket->RadiusDamage = radius_damage;
	Rocket->DamageRadius = damage_radius;
	Rocket->State.SetSound (SoundIndex ("weapons/rockfly.wav"));
	Rocket->gameEntity->classname = "rocket";

	if (Spawner->EntityFlags & ENT_PLAYER)
		CheckDodge (Spawner, start, dir, speed);

	Rocket->Link ();
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

CBFGBolt::CBFGBolt (int Index) :
CFlyMissileProjectile(Index),
CTouchableEntity(Index),
CThinkableEntity(Index)
{
	Exploded = false;
};

void CBFGBolt::Think ()
{
	// bfg_explode
	if (Exploded)
	{
		if (State.GetFrame() == 0)
		{
			vec3f origin = State.GetOrigin ();
			// the BFG effect
			CHurtableEntity *ent = NULL;
			while ((ent = FindRadius<CHurtableEntity, ENT_HURTABLE> (ent, origin, DamageRadius)) != NULL)
			{
				if (!ent->CanTakeDamage)
					continue;
				if (ent == GetOwner())
					continue;
				if (!ent->CanDamage (this) || !ent->CanDamage (GetOwner()))
					continue;

				vec3f v = ent->GetMins() + ent->GetMaxs();
				v = ent->State.GetOrigin().MultiplyAngles (0.5f, v);
				v = origin - v;
				float dist = v.Length();
				float points = Damage * (1.0f - sqrtf(dist/DamageRadius));
				if (ent == GetOwner())
					points = points * 0.5;

				CTempEnt_Explosions::BFGExplosion (ent->State.GetOrigin());
				ent->TakeDamage (this, GetOwner(), gameEntity->velocity, ent->State.GetOrigin(), vec3fOrigin, (int)points, 0, DAMAGE_ENERGY, MOD_BFG_EFFECT);
			}
		}

		NextThink = level.framenum + FRAMETIME;
		State.SetFrame (State.GetFrame() + 1);
		if (State.GetFrame() == 5)
			Free ();

		return;
	}
	else // bfg_think
	{
		if (FreeTime < level.framenum)
		{
			Free();
			return;
		}

		CHurtableEntity	*ent = NULL;
		edict_t	*ignore;

		vec3f origin = State.GetOrigin ();
		const int dmg = (game.mode & GAME_DEATHMATCH) ? 5 : 10;

		while ((ent = FindRadius<CHurtableEntity, ENT_HURTABLE> (ent, origin, 256)) != NULL)
		{
			if (ent == gameEntity->Entity) // Stupid...
				continue;

			if (ent == GetOwner())
				continue;

			if (!ent->CanTakeDamage)
				continue;

			//if (!(ent->svFlags & SVF_MONSTER) && (!ent->client))
			//	continue;

	#ifdef CLEANCTF_ENABLED
	//ZOID
			//don't target players in CTF
			if ((game.mode & GAME_CTF) && (ent->EntityFlags & ENT_PLAYER) &&
				(GetOwner() && (GetOwner()->EntityFlags & ENT_PLAYER)))
			{
				if ((dynamic_cast<CPlayerEntity*>(ent))->Client.resp.ctf_team == (dynamic_cast<CPlayerEntity*>(GetOwner()))->Client.resp.ctf_team)
					continue;
			}
	//ZOID
	#endif

			vec3f point = ent->GetAbsMin().MultiplyAngles (0.5f, ent->GetSize());

			vec3f dir = point - origin;
			dir.Normalize ();

			ignore = gameEntity;
			vec3f start = origin;
			vec3f end = start.MultiplyAngles (2048, dir);
			CTrace tr;
			while(1)
			{
				tr = CTrace (start, end, ignore, CONTENTS_SOLID|CONTENTS_MONSTER|CONTENTS_DEADMONSTER);

				if (!tr.ent || !tr.Ent)
					break;

				// hurt it if we can
				//if ((tr.ent->takedamage) && !(tr.ent->flags & FL_IMMUNE_LASER) && (tr.ent != gameEntity->owner))
				if (((tr.Ent->EntityFlags & ENT_HURTABLE) && dynamic_cast<CHurtableEntity*>(tr.Ent)->CanTakeDamage) && !(tr.Ent->Flags & FL_IMMUNE_LASER) && (tr.Ent != GetOwner()))
					dynamic_cast<CHurtableEntity*>(tr.Ent)->TakeDamage (this, GetOwner(), dir, tr.EndPos, vec3fOrigin, dmg, 1, DAMAGE_ENERGY, MOD_BFG_LASER);

				// if we hit something that's not a monster or player we're done
				//if (!(tr.ent->svFlags & SVF_MONSTER) && (!tr.ent->client))
				if (!(tr.Ent->EntityFlags & ENT_MONSTER) && !(tr.Ent->EntityFlags & ENT_PLAYER))
				{
					CTempEnt_Splashes::Sparks (tr.EndPos, tr.Plane.normal, CTempEnt_Splashes::STLaserSparks, (CTempEnt_Splashes::ESplashType)State.GetSkinNum(), 4);
					break;
				}

				ignore = tr.ent;
				start = tr.EndPos;
			}

			CTempEnt_Trails::BFGLaser(origin, tr.EndPos);
		}

		NextThink = level.framenum + FRAMETIME;
	}
}

void CBFGBolt::Touch (CBaseEntity *other, plane_t *plane, cmBspSurface_t *surf)
{
	if (Exploded)
		return;

	if (other->gameEntity == gameEntity->owner)
		return;

	if (surf && (surf->flags & SURF_TEXINFO_SKY))
	{
		Free();
		return;
	}

	vec3f boltOrigin = State.GetOrigin();
	if (gameEntity->owner->client)
		dynamic_cast<CPlayerEntity*>(gameEntity->owner->Entity)->PlayerNoiseAt (boltOrigin, PNOISE_IMPACT);

	// core explosion - prevents firing it into the wall/floor
	if ((other->EntityFlags & ENT_HURTABLE) && dynamic_cast<CHurtableEntity*>(other)->CanTakeDamage)
		dynamic_cast<CHurtableEntity*>(other)->TakeDamage (this, GetOwner(), gameEntity->velocity, boltOrigin, (plane) ? plane->normal : vec3fOrigin, 200, 0, 0, MOD_BFG_BLAST);
	T_RadiusDamage(this, GetOwner(), 200, other, 100, MOD_BFG_BLAST);

	PlaySound (CHAN_VOICE, SoundIndex ("weapons/bfg__x1b.wav"));
	SetSolid (SOLID_NOT);

	Exploded = true;
	boltOrigin = boltOrigin.MultiplyAngles (-0.1f, gameEntity->velocity);
	State.SetOrigin (boltOrigin);
	Vec3Clear (gameEntity->velocity);
	State.SetModelIndex(ModelIndex ("sprites/s_bfg3.sp2"));
	State.SetFrame(0);
	State.SetSound(0);
	State.SetEffects (EF_BFG);
	NextThink = level.framenum + FRAMETIME;
	gameEntity->enemy = other->gameEntity;

	CTempEnt_Explosions::BFGExplosion (boltOrigin, true);
}

void CBFGBolt::Spawn	(CBaseEntity *Spawner, vec3f start, vec3f dir,
						int damage, int speed, float damage_radius)
{
	CBFGBolt	*BFG = QNew (com_levelPool, 0) CBFGBolt;

	BFG->State.SetOrigin (start);

	BFG->State.SetAngles (dir.ToAngles());
	vec3f vel = dir;
	vel.NormalizeFast();
	vel *= speed;
	BFG->gameEntity->velocity[0] = vel.X;
	BFG->gameEntity->velocity[1] = vel.Y;
	BFG->gameEntity->velocity[2] = vel.Z;
	BFG->State.SetEffects (EF_BFG | EF_ANIM_ALLFAST);
	BFG->State.SetModelIndex (ModelIndex ("sprites/s_bfg1.sp2"));
	BFG->SetOwner (Spawner);
	BFG->NextThink = level.framenum + FRAMETIME;
	BFG->Damage = damage;
	BFG->DamageRadius = damage_radius;
	BFG->State.SetSound (SoundIndex ("weapons/bfg__l1a.wav"));
	BFG->gameEntity->classname = "bfg blast";
	BFG->FreeTime = level.framenum + 80000/speed;

	BFG->Link ();
}

bool CBFGBolt::Run ()
{
	return CFlyMissileProjectile::Run();
}

CTrace CHitScan::DoTrace(vec3f &start, vec3f &end, CBaseEntity *ignore, int mask)
{
	return CTrace (start, end, (ignore) ? ignore->gameEntity : NULL, mask);
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

	// Calculate end
	if (!ModifyEnd(aimdir, start, end))
		end = start.MultiplyAngles (8192, aimdir);

	from = start;

	int Mask = CONTENTS_MASK_SHOT|CONTENTS_MASK_WATER;
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
		Mask = CONTENTS_MASK_SHOT;

		// Find the exit point
		int tries = 20; // Cover about 2000 units
		vec3f	stWater;

		stWater = from;
		lastWaterStart.Clear();
		
		while (tries > 0)
		{
			stWater = stWater.MultiplyAngles (HITSCANSTEP, aimdir);

			int contents = PointContents(stWater);
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
		if (Trace.ent && Trace.Ent && ((Trace.Ent->EntityFlags & ENT_HURTABLE) && dynamic_cast<CHurtableEntity*>(Trace.Ent)->CanTakeDamage))
		{
			// Convert to base entity
			CHurtableEntity *Target = dynamic_cast<CHurtableEntity*>(Trace.Ent);

			// Hurt it
			// Revision
			// Startsolid mistake..
			if (Trace.startSolid)
			{
				vec3f origin = Target->State.GetOrigin();
				if (!DoDamage (Entity, Target, aimdir, origin, Trace.Plane.normal))
					break; // We wanted to stop

				// Set up the start from where we are now
				vec3f oldFrom = from;
				from = origin;

				DoEffect (from, oldFrom, Water);

				// and ignore the bastard
				Ignore = Target;

				// Continue our loop
				continue;
			}

			if (!DoDamage (Entity, Target, aimdir, Trace.EndPos, Trace.Plane.normal))
				break; // We wanted to stop

			// Set up the start from where we are now
			vec3f oldFrom = from;
			from = Trace.EndPos;

			DoEffect (from, oldFrom, Water);

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
			from = Trace.EndPos;
			Mask = CONTENTS_MASK_SHOT;

			// Find the exit point
			int tries = 20; // Cover about 2000 units
			vec3f	stWater = from;
			lastWaterStart.Clear();
			
			while (tries > 0)
			{
				stWater = stWater.MultiplyAngles (HITSCANSTEP, aimdir);

				int contents = PointContents(stWater);
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

			// Draw the effect we have so far
			/*DoEffect (from, Trace.endPos, false);

			// Keep going
			Vec3MA (Trace.endPos, 0.1f, aimdir, from);

			// Water hit effect
			DoWaterHit (&Trace);*/

			// FIXME: This is the easiest fix currently.
			Mask = CONTENTS_MASK_SHOT;

			// There's a problem with the code above in that
			// the trace isn't fully connected with something like
			// a railgun. For now, this should work, since
			// I don't know any maps with real water underneath
			// transparent water.
			continue;
		}
		// Assume solid
		else
		{
			// Draw the effect
			DoEffect (from, Trace.EndPos, false);
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
	CTempEnt_Trails::RailTrail (start, end);
}

void CRailGunShot::Fire(CBaseEntity *Entity, vec3f start, vec3f aimdir, int damage, int kick)
{
	CRailGunShot(damage, kick).DoFire (Entity, start, aimdir);
}

bool CBullet::DoDamage (CBaseEntity *Attacker, CHurtableEntity *Target, vec3f &dir, vec3f &point, vec3f &normal)
{
	Target->TakeDamage (Attacker, Attacker, dir, point, normal, Damage, Kick, DAMAGE_BULLET, MeansOfDeath);
	return ThroughAndThrough;
}

void CBullet::DoSolidHit	(CTrace *Trace)
{
	if (!(Trace->surface->flags & SURF_TEXINFO_SKY))
		CTempEnt_Splashes::Gunshot (Trace->EndPos, Trace->Plane.normal);
}

bool CBullet::ModifyEnd (vec3f &aimDir, vec3f &start, vec3f &end)
{
	vec3f dir = aimDir.ToAngles();
	vec3f forward, right, up;
	dir.ToVectors (&forward, &right, &up);

	float r = crandom()*hSpread;
	float u = crandom()*vSpread;
	end = start.MultiplyAngles (8192, forward);
	end = end.MultiplyAngles (r, right);
	end = end.MultiplyAngles (u, up);
	return true;
}

void CBullet::DoEffect	(vec3f &start, vec3f &end, bool water)
{
	if (water)
		CTempEnt_Trails::BubbleTrail(start, end);
}

void CBullet::DoWaterHit	(CTrace *Trace)
{
	CTempEnt_Splashes::ESplashType color;
	if (Trace->contents & CONTENTS_WATER)
	{
		if (strcmp(Trace->surface->name, "*brwater") == 0)
			color = CTempEnt_Splashes::SPTMud;
		else
			color = CTempEnt_Splashes::SPTWater;
	}
	else if (Trace->contents & CONTENTS_SLIME)
		color = CTempEnt_Splashes::SPTSlime;
	else if (Trace->contents & CONTENTS_LAVA)
		color = CTempEnt_Splashes::SPTLava;
	else
		return;

	CTempEnt_Splashes::Splash (Trace->EndPos, Trace->Plane.normal, color);
}

void CBullet::Fire(CBaseEntity *Entity, vec3f start, vec3f aimdir, int damage, int kick, int hSpread, int vSpread, int mod)
{
	CBullet(damage, kick, hSpread, vSpread, mod).DoFire (Entity, start, aimdir);
}

// An overload to handle transparent water
void CBullet::DoFire(CBaseEntity *Entity, vec3f start, vec3f aimdir)
{
	vec3f end, from;
	vec3f lastWaterStart, lastWaterEnd;

	// Calculate end
	if (!ModifyEnd(aimdir, start, end))
		end = start.MultiplyAngles (8192, aimdir);

	from = start;

	int Mask = CONTENTS_MASK_SHOT|CONTENTS_MASK_WATER;
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

		Mask = CONTENTS_MASK_SHOT;

		// Find the exit point
		int tries = 20; // Cover about 2000 units
		vec3f	stWater = from;
		lastWaterStart.Clear ();
		
		while (tries > 0)
		{
			stWater = stWater.MultiplyAngles (HITSCANSTEP, aimdir);

			int contents = PointContents(stWater);
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
		if (Trace.ent && Trace.Ent && ((Trace.Ent->EntityFlags & ENT_HURTABLE) && dynamic_cast<CHurtableEntity*>(Trace.Ent)->CanTakeDamage))
		{
			// Convert to base entity
			CHurtableEntity *Target = dynamic_cast<CHurtableEntity*>(Trace.Ent);

			// Hurt it
			// Revision
			// Startsolid mistake..
			if (Trace.startSolid)
			{
				vec3f origin = Target->State.GetOrigin();
				if (!DoDamage (Entity, Target, aimdir, origin, Trace.Plane.normal))
					break; // We wanted to stop

				// Set up the start from where we are now
				vec3f oldFrom = from;
				from = origin;
				DoEffect (from, oldFrom, Water);

				// and ignore the bastard
				Ignore = Target;

				// Continue our loop
				continue;
			}
			if (!DoDamage (Entity, Target, aimdir, Trace.EndPos, Trace.Plane.normal))
				break; // We wanted to stop

			// Set up the start from where we are now
			vec3f oldFrom = from;
			from = Trace.EndPos;
			DoEffect (from, oldFrom, Water);

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
					lastWaterStart = lastWaterEnd.MultiplyAngles (5, aimdir);

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
			from = Trace.EndPos;
			Mask = CONTENTS_MASK_SHOT;

			// Find the exit point
			int tries = 20; // Cover about 2000 units
			vec3f	stWater = from;
			lastWaterStart.Clear();
			
			while (tries > 0)
			{
				stWater = stWater.MultiplyAngles(HITSCANSTEP, aimdir);

				int contents = PointContents(stWater);
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
			int tries = 20; // Cover about 2000 units
			vec3f	stWater = from;
			lastWaterStart.Clear();
			
			while (tries > 0)
			{
				stWater = stWater.MultiplyAngles (HITSCANSTEP, aimdir);

				int contents = PointContents(stWater);
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
		// Assume solid
		else
		{
			// Draw the effect
			DoEffect (from, Trace.EndPos, false);
			DoSolidHit (&Trace);
			break; // We're done
		}
	}
}

void CShotgunPellets::DoSolidHit	(CTrace *Trace)
{
	if (!(Trace->surface->flags & SURF_TEXINFO_SKY))
		CTempEnt_Splashes::Shotgun (Trace->EndPos, Trace->Plane.normal);
}

void CShotgunPellets::Fire(CBaseEntity *Entity, vec3f start, vec3f aimdir, int damage, int kick, int hSpread, int vSpread, int Count, int mod)
{
	for (int i = 0; i < Count; i++)
		CShotgunPellets(damage, kick, hSpread, vSpread, mod).DoFire (Entity, start, aimdir);
}

bool CMeleeWeapon::Fire(CBaseEntity *Entity, vec3f aim, int damage, int kick)
{
	vec3f		forward, right, up, point, dir;
	float		range;

	//see if enemy is in range
	CBaseEntity *Enemy = Entity->gameEntity->enemy->Entity;

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

	CTrace tr = CTrace (Entity->State.GetOrigin(), NULL, NULL, point, Entity->gameEntity, CONTENTS_MASK_SHOT);
	if (tr.fraction == 1.0)
		return false;

	if (!((tr.Ent->EntityFlags & ENT_HURTABLE) && dynamic_cast<CHurtableEntity*>(tr.Ent)->CanTakeDamage))
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
	if ((Hit->EntityFlags & ENT_HURTABLE) && dynamic_cast<CHurtableEntity*>(Hit)->CanTakeDamage)
		dynamic_cast<CHurtableEntity*>(Hit)->TakeDamage (Entity, Entity, dir, point, vec3fOrigin, damage, kick/2, DAMAGE_NO_KNOCKBACK, MOD_HIT);

	if (!(Hit->EntityFlags & ENT_MONSTER) && (!(Hit->EntityFlags & ENT_PLAYER)))
		return false;

	// do our special form of knockback here
	vec3f v = Enemy->GetAbsMin().MultiplyAngles (0.5f, Enemy->GetSize()) - point;
	v.Normalize ();
	v = v.MultiplyAngles (kick, v);
	Vec3Copy (v, Enemy->gameEntity->velocity);
	if (Enemy->gameEntity->velocity[2] > 0)
		Enemy->GroundEntity = NULL;
	return true;
}

#ifdef CLEANCTF_ENABLED

CGrappleEntity::CGrappleEntity () :
CBaseEntity(),
CFlyMissileProjectile(),
CTouchableEntity()
{
};

CGrappleEntity::CGrappleEntity (int Index) :
CBaseEntity(Index),
CFlyMissileProjectile(Index),
CTouchableEntity(Index)
{
};

void CGrappleEntity::GrappleDrawCable()
{
	vec3f	start, end, f, r, origin = Player->State.GetOrigin ();
	
	Player->Client.ViewAngle.ToVectors (&f, &r, NULL);
	Player->P_ProjectSource (vec3f(16, 16, Player->gameEntity->viewheight-8), f, r, start);

	vec3f offset = start - origin;
	vec3f dir = start - State.GetOrigin();

	// don't draw cable if close
	if (dir.LengthFast() < 64)
		return;

	// adjust start for beam origin being in middle of a segment
	end = State.GetOrigin();
	CTempEnt_Trails::GrappleCable (origin, end, Player->State.GetNumber(), offset);
};

void SV_AddGravity (edict_t *ent);
void CGrappleEntity::GrapplePull()
{
	float volume = (Player->Client.silencer_shots) ? 0.2f : 1.0;

	if ((Player->Client.pers.Weapon->Item == NItems::Grapple) &&
		!Player->Client.NewWeapon &&
		(Player->Client.weaponstate != WS_FIRING) &&
		(Player->Client.weaponstate != WS_ACTIVATING))
	{
		ResetGrapple();
		return;
	}

	if (gameEntity->enemy)
	{
		if (gameEntity->enemy->solid == SOLID_NOT)
		{
			ResetGrapple();
			return;
		}
		if (gameEntity->enemy->solid == SOLID_BBOX)
		{
			State.SetOrigin ((gameEntity->enemy->Entity->GetSize() * 0.5f) + gameEntity->enemy->Entity->State.GetOrigin() + gameEntity->enemy->Entity->GetMins());
			Link ();
		}
		else
			Vec3Copy (gameEntity->enemy->velocity, gameEntity->velocity);
		if (gameEntity->enemy && ((gameEntity->enemy->Entity->EntityFlags & ENT_HURTABLE) && dynamic_cast<CHurtableEntity*>(gameEntity->enemy->Entity)->CanTakeDamage))
		{
			CHurtableEntity *Hurt = dynamic_cast<CHurtableEntity*>(gameEntity->enemy->Entity);
			if (!Hurt->CheckTeamDamage (Player))
			{
				Hurt->TakeDamage (this, Player, gameEntity->velocity, State.GetOrigin(), vec3fOrigin, 1, 1, 0, MOD_GRAPPLE);
				PlaySound (CHAN_WEAPON, SoundIndex("weapons/grapple/grhurt.wav"), volume);
			}
		}
		if (gameEntity->enemy && (gameEntity->enemy->Entity->EntityFlags & ENT_HURTABLE) && dynamic_cast<CHurtableEntity*>(gameEntity->enemy->Entity)->DeadFlag)
		{ // he died
			ResetGrapple();
			return;
		}
	}

	GrappleDrawCable();

	if (Player->Client.ctf_grapplestate > CTF_GRAPPLE_STATE_FLY)
	{
		// pull player toward grapple
		// this causes icky stuff with prediction, we need to extend
		// the prediction layer to include two new fields in the player
		// move stuff: a point and a velocity.  The client should add
		// that velociy in the direction of the point
		vec3f forward, up;

		Player->Client.ViewAngle.ToVectors (&forward, &up, NULL);
		vec3f v = Player->State.GetOrigin ();
		v.Z += Player->gameEntity->viewheight;
		vec3f hookdir = State.GetOrigin() - v;

		float vlen = hookdir.LengthFast();

		if ((Player->Client.ctf_grapplestate == CTF_GRAPPLE_STATE_PULL) &&
			vlen < 64)
		{
			Player->Client.PlayerState.GetPMove()->pmFlags |= PMF_NO_PREDICTION;
			Player->PlaySound (CHAN_WEAPON, SoundIndex("weapons/grapple/grhang.wav"), volume);
			Player->Client.ctf_grapplestate = CTF_GRAPPLE_STATE_HANG;
		}

		hookdir.NormalizeFast ();
		hookdir *= CTF_GRAPPLE_PULL_SPEED;
		Player->gameEntity->velocity[0] = hookdir.X;
		Player->gameEntity->velocity[1] = hookdir.Y;
		Player->gameEntity->velocity[2] = hookdir.Z;
		SV_AddGravity(Player->gameEntity);
	}
};

void CGrappleEntity::ResetGrapple ()
{
	Player->Client.ctf_grapple = NULL;
	Player->Client.ctf_grapplereleasetime = level.framenum;
	Player->Client.ctf_grapplestate = CTF_GRAPPLE_STATE_HANG+1; // we're firing, not on hook
	Player->Client.PlayerState.GetPMove()->pmFlags &= ~PMF_NO_PREDICTION;
	Free ();
};

void CGrappleEntity::Spawn (CPlayerEntity *Spawner, vec3f start, vec3f dir, int damage, int speed)
{
	CGrappleEntity *Grapple = QNew (com_levelPool, 0) CGrappleEntity;
	Grapple->Player = Spawner;
	Grapple->Damage = damage;

	dir.NormalizeFast();

	Grapple->State.SetOrigin (start);
	Grapple->State.SetOldOrigin (start);
	Grapple->State.SetAngles (dir.ToAngles());
	vec3f vel = dir * speed;
	Grapple->gameEntity->velocity[0] = vel.X;
	Grapple->gameEntity->velocity[1] = vel.Y;
	Grapple->gameEntity->velocity[2] = vel.Z;
	Grapple->SetClipmask (CONTENTS_MASK_SHOT);
	Grapple->SetSolid (SOLID_BBOX);
	Grapple->SetMins (vec3fOrigin);
	Grapple->SetMaxs (vec3fOrigin);
	Grapple->State.SetModelIndex (ModelIndex ("models/weapons/grapple/hook/tris.md2"));
	Grapple->SetOwner (Spawner);
	Spawner->Client.ctf_grapple = Grapple;
	Spawner->Client.ctf_grapplestate = CTF_GRAPPLE_STATE_FLY; // we're firing, not on hook
	Grapple->Link ();

	vec3f origin = Spawner->State.GetOrigin();
	CTrace tr = CTrace (origin, Grapple->State.GetOrigin(), Grapple->gameEntity, CONTENTS_MASK_SHOT);
	if (tr.fraction < 1.0)
	{
		Grapple->State.SetOrigin (Grapple->State.GetOrigin().MultiplyAngles (-10, dir));
		Grapple->Touch (tr.ent->Entity, NULL, NULL);
	}
};

void CGrappleEntity::Touch (CBaseEntity *other, plane_t *plane, cmBspSurface_t *surf)
{
	if (other == Player)
		return;

	if (Player->Client.ctf_grapplestate != CTF_GRAPPLE_STATE_FLY)
		return;

	if (surf && (surf->flags & SURF_TEXINFO_SKY))
	{
		ResetGrapple();
		return;
	}

	Vec3Copy(vec3Origin, gameEntity->velocity);
	Player->PlayerNoiseAt (State.GetOrigin(), PNOISE_IMPACT);

	if ((other->EntityFlags & ENT_HURTABLE) && dynamic_cast<CHurtableEntity*>(other)->CanTakeDamage)
	{
		dynamic_cast<CHurtableEntity*>(other)->TakeDamage (other, this, Player, gameEntity->velocity, State.GetOrigin(), (plane) ? plane->normal : vec3fOrigin, Damage, 1, 0, MOD_GRAPPLE);
		ResetGrapple();
		return;
	}

	Player->Client.ctf_grapplestate = CTF_GRAPPLE_STATE_PULL; // we're on hook
	gameEntity->enemy = other->gameEntity;

	SetSolid (SOLID_NOT);

	float volume = (Player->Client.silencer_shots) ? 0.2f : 1.0f;
	Player->PlaySound (CHAN_WEAPON, SoundIndex("weapons/grapple/grpull.wav"), volume);
	PlaySound (CHAN_WEAPON, SoundIndex("weapons/grapple/grhit.wav"), volume);

	CTempEnt_Splashes::Sparks (State.GetOrigin(), (!plane) ? vec3fOrigin : plane->normal);
};

bool CGrappleEntity::Run ()
{
	return CFlyMissileProjectile::Run();
};
#endif