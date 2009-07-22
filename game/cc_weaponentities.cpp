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
	vec3f	end, v;
	CTrace	tr;
	float	eta;

	// easy mode only ducks one quarter the time
	if (skill->Integer() == 0)
	{
		if (random() > 0.25)
			return;
	}

	end = start.MultiplyAngles(8192, dir);
	tr = CTrace (start, end, self->gameEntity, CONTENTS_MASK_SHOT);

#ifdef MONSTER_USE_ROGUE_AI
	if ((tr.ent) && (tr.ent->Entity) && (tr.ent->Entity->EntityFlags & ENT_MONSTER) && (tr.ent->health > 0) && infront(tr.ent, self->gameEntity))
	{
		v = tr.EndPos - start;
		eta = (v.LengthFast() - tr.ent->maxs[0]) / speed;
		(dynamic_cast<CMonsterEntity*>(tr.ent->Entity))->Monster->Dodge (self->gameEntity, eta, &tr);

		if (tr.ent->enemy != self->gameEntity)
			tr.ent->enemy = self->gameEntity;
	}
#else
	if ((tr.ent) && (tr.ent->Entity) && (tr.ent->Entity->EntityFlags & ENT_MONSTER) (tr.ent->health > 0) && infront(tr.ent, self->gameEntity))
	{
		v = tr.EndPos - Start;
		eta = (v.LengthFast() - tr.ent->maxs[0]) / speed;
		(dynamic_cast<CMonsterEntity*>(tr.ent->Entity))->Dodge (self->gameEntity, eta);
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
	vec3_t		origin;
	int			mod;

	State.GetOrigin (origin);
	if (gameEntity->owner && gameEntity->owner->client)
		PlayerNoise(dynamic_cast<CPlayerEntity*>(gameEntity->owner->Entity), origin, PNOISE_IMPACT);

	//FIXME: if we are onground then raise our Z just a bit since we are a point?
	if (gameEntity->enemy)
	{
		float	points;
		vec3_t	v;
		vec3_t	dir;

		Vec3Add (gameEntity->enemy->mins, gameEntity->enemy->maxs, v);
		Vec3MA (gameEntity->enemy->state.origin, 0.5, v, v);
		Vec3Subtract (origin, v, v);
		points = Damage - 0.5 * Vec3Length (v);
		Vec3Subtract (gameEntity->enemy->state.origin, origin, dir);
		if (gameEntity->spawnflags & GRENADE_HAND)
			mod = MOD_HANDGRENADE;
		else
			mod = MOD_GRENADE;
		T_Damage (gameEntity->enemy, gameEntity, gameEntity->owner, dir, origin, vec3Origin, (int)points, (int)points, DAMAGE_RADIUS, mod);
	}

	if (gameEntity->spawnflags & GRENADE_HELD)
		mod = MOD_HELD_GRENADE;
	else if (gameEntity->spawnflags & GRENADE_HAND)
		mod = MOD_HG_SPLASH;
	else
		mod = MOD_G_SPLASH;
	T_RadiusDamage(gameEntity, gameEntity->owner, Damage, gameEntity->enemy, RadiusDamage, mod);

	Vec3MA (origin, -0.02f, gameEntity->velocity, origin);
	if (gameEntity->groundentity)
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

	if (!other->gameEntity->takedamage)
	{
		if (gameEntity->spawnflags & GRENADE_HAND)
		{
			if (random() > 0.5)
				PlaySoundFrom (gameEntity, CHAN_VOICE, SoundIndex ("weapons/hgrenb1a.wav"));
			else
				PlaySoundFrom (gameEntity, CHAN_VOICE, SoundIndex ("weapons/hgrenb2a.wav"));
		}
		else
			PlaySoundFrom (gameEntity, CHAN_VOICE, SoundIndex ("weapons/grenlb1b.wav"));
		return;
	}

	gameEntity->enemy = other->gameEntity;
	Explode ();
}

void CGrenade::Think ()
{
	Explode();
}

void CGrenade::Spawn (CBaseEntity *Spawner, vec3_t start, vec3_t aimdir, int damage, int speed, float timer, float damage_radius, bool handNade, bool held)
{
	CGrenade	*Grenade = QNew (com_levelPool, 0) CGrenade();
	vec3_t		dir;
	vec3_t		forward, right, up;

	VecToAngles (aimdir, dir);
	Angles_Vectors (dir, forward, right, up);

	Grenade->State.SetOrigin (start);
	Vec3Scale (aimdir, speed, Grenade->gameEntity->velocity);
	Vec3MA (Grenade->gameEntity->velocity, 200 + crandom() * 10.0, up, Grenade->gameEntity->velocity);
	Vec3MA (Grenade->gameEntity->velocity, crandom() * 10.0, right, Grenade->gameEntity->velocity);
	Vec3Set (Grenade->gameEntity->avelocity, 300, 300, 300);
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
			PlaySoundFrom (Spawner->gameEntity, CHAN_WEAPON, SoundIndex ("weapons/hgrent1a.wav"));
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

// Remove this line
#define DEFINITELY_NOT_KEVIN
#define VectorLength Vec3Length

// CL_CalcParticleLOD
// Used for particles that have a constant particle number:
// ie:		while (i < 40)
//				i--

#ifdef DEFINITELY_NOT_KEVIN
#define VIEWORG g_edicts[1].state.origin
#else
#define VIEWORG cl.refdef.vieworg
#endif

// Set to the cvar if you want
#define LOD_DISTANCE	2000

int CL_CalcParticleLOD (int amount, 
#ifdef DEFINITELY_NOT_KEVIN
						vec3f origin
#else
						vec3_t origin
#endif
						)
{
	vec3_t subt;
	float len;

	Vec3Subtract (origin, VIEWORG, subt);
	len = VectorLength (subt);

	len = (len - 750) / 100;

	if (len < 0)
		len = 0;
	else if (len > LOD_DISTANCE)
		len = LOD_DISTANCE;

	amount -= len;

	return amount;
}

void CBlasterProjectile::Think ()
{
	//int amount = CL_CalcParticleLOD (40, State.GetOrigin());
	//DebugPrintf ("%i\n", amount);
	//NextThink = level.framenum + FRAMETIME;
	Free();
}

int ClipVelocity (vec3_t in, vec3_t normal, vec3_t out, float overbounce);
void CBlasterProjectile::Touch (CBaseEntity *other, plane_t *plane, cmBspSurface_t *surf)
{
	if (other->gameEntity == gameEntity->owner)
		return;

	if (surf && (surf->flags & SURF_TEXINFO_SKY))
	{
		Free (); // "delete" the entity
		return;
	}

	vec3_t origin;
	State.GetOrigin (origin);
	if (gameEntity->owner->client)
		PlayerNoise(dynamic_cast<CPlayerEntity*>(gameEntity->owner->Entity), origin, PNOISE_IMPACT);

	if (other->gameEntity->takedamage)
		T_Damage (other->gameEntity, gameEntity, gameEntity->owner, gameEntity->velocity, origin, plane ? plane->normal : vec3Origin, Damage, 1, DAMAGE_ENERGY, (gameEntity->spawnflags & 1) ? MOD_HYPERBLASTER : MOD_BLASTER);
	else
		CTempEnt_Splashes::Blaster(origin, plane ? plane->normal : vec3Origin);

	Free (); // "delete" the entity
}

void ED_CallSpawn (edict_t *ent);
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
	Vec3Scale (dir, speed, Bolt->gameEntity->velocity);
	vec3f Scaled = dir;
	Scaled.Scale(speed);
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
	vec3_t		origin;
	int			n;

	if (other->gameEntity == gameEntity->owner)
		return;

	if (surf && (surf->flags & SURF_TEXINFO_SKY))
	{
		Free ();
		return;
	}

	State.GetOrigin (origin);
	if (gameEntity->owner->client)
		PlayerNoise(dynamic_cast<CPlayerEntity*>(gameEntity->owner->Entity), origin, PNOISE_IMPACT);

	if (other->gameEntity->takedamage)
		T_Damage (other->gameEntity, gameEntity, gameEntity->owner, gameEntity->velocity, origin, plane->normal, Damage, 0, 0, MOD_ROCKET);
	else
	{
		// don't throw any debris in net games
		if (game.mode == GAME_SINGLEPLAYER)
		{
			if ((surf) && !(surf->flags & (SURF_TEXINFO_WARP|SURF_TEXINFO_TRANS33|SURF_TEXINFO_TRANS66|SURF_TEXINFO_FLOWING)))
			{
				n = rand() % 5;
				while(n--)
					ThrowDebris (gameEntity, "models/objects/debris2/tris.md2", 2, origin);
			}
		}
	}

	// calculate position for the explosion entity
	Vec3MA (origin, -0.02f, gameEntity->velocity, origin);
	T_RadiusDamage(gameEntity, gameEntity->owner, RadiusDamage, other->gameEntity, DamageRadius, MOD_R_SPLASH);
	CTempEnt_Explosions::RocketExplosion(origin, gameEntity, !!gameEntity->waterlevel);

	Free ();
}

void CRocket::Spawn	(CBaseEntity *Spawner, vec3_t start, vec3_t dir,
						int damage, int speed, float damage_radius, int radius_damage)
{
	CRocket	*Rocket = QNew (com_levelPool, 0) CRocket;

	Rocket->State.SetOrigin (start);

	vec3_t dirAngles;
	VecToAngles (dir, dirAngles);
	Rocket->State.SetAngles (dirAngles);
	Vec3Scale (dir, speed, Rocket->gameEntity->velocity);
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
	{
		vec3f astart = vec3f(start);
		vec3f adir = vec3f(dir);
		CheckDodge (Spawner, astart, adir, speed);
	}

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
		edict_t	*ent;
		float	points;
		vec3_t	v;
		float	dist;
		vec3_t	origin;

		State.GetOrigin (origin);

		if (State.GetFrame() == 0)
		{
			// the BFG effect
			ent = NULL;
			while ((ent = findradius(ent, origin, DamageRadius)) != NULL)
			{
				if (!ent->takedamage)
					continue;
				if (ent == gameEntity->owner)
					continue;
				if (!CanDamage (ent, gameEntity))
					continue;
				if (!CanDamage (ent, gameEntity->owner))
					continue;

				Vec3Add (ent->mins, ent->maxs, v);
				Vec3MA (ent->state.origin, 0.5, v, v);
				Vec3Subtract (origin, v, v);
				dist = Vec3Length(v);
				points = Damage * (1.0f - sqrtf(dist/DamageRadius));
				if (ent == gameEntity->owner)
					points = points * 0.5;

				CTempEnt_Explosions::BFGExplosion (ent->state.origin);
				T_Damage (ent, gameEntity, gameEntity->owner, gameEntity->velocity, ent->state.origin, vec3Origin, (int)points, 0, DAMAGE_ENERGY, MOD_BFG_EFFECT);
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

		edict_t	*ent;
		edict_t	*ignore;
		vec3_t	point;
		vec3_t	dir;
		vec3_t	start;
		vec3_t	end;
		int		dmg;
		CTrace	tr;
		vec3_t	origin;

		State.GetOrigin (origin);

		if (game.mode & GAME_DEATHMATCH)
			dmg = 5;
		else
			dmg = 10;

		ent = NULL;
		while ((ent = findradius(ent, origin, 256)) != NULL)
		{
			if (ent == gameEntity)
				continue;

			if (ent == gameEntity->owner)
				continue;

			if (!ent->takedamage)
				continue;

			if (!(ent->svFlags & SVF_MONSTER) && (!ent->client))
				continue;

	#ifdef CLEANCTF_ENABLED
	//ZOID
			//don't target players in CTF
			if ((game.mode & GAME_CTF) && ent->Entity && (ent->Entity->EntityFlags & ENT_PLAYER) &&
				gameEntity->owner->client)
			{
				if ((dynamic_cast<CPlayerEntity*>(ent->Entity))->Client.resp.ctf_team == (dynamic_cast<CPlayerEntity*>(gameEntity->owner->Entity))->Client.resp.ctf_team)
				continue;
			}
	//ZOID
	#endif

			Vec3MA (ent->absMin, 0.5, ent->size, point);

			Vec3Subtract (point, origin, dir);
			VectorNormalizef (dir, dir);

			ignore = gameEntity;
			Vec3Copy (origin, start);
			Vec3MA (start, 2048, dir, end);
			while(1)
			{
				tr = CTrace (start, end, ignore, CONTENTS_SOLID|CONTENTS_MONSTER|CONTENTS_DEADMONSTER);

				if (!tr.ent)
					break;

				// hurt it if we can
				if ((tr.ent->takedamage) && !(tr.ent->flags & FL_IMMUNE_LASER) && (tr.ent != gameEntity->owner))
					T_Damage (tr.ent, gameEntity, gameEntity->owner, dir, tr.endPos, vec3Origin, dmg, 1, DAMAGE_ENERGY, MOD_BFG_LASER);

				// if we hit something that's not a monster or player we're done
				if (!(tr.ent->svFlags & SVF_MONSTER) && (!tr.ent->client))
				{
					CTempEnt_Splashes::Sparks (tr.endPos, tr.plane.normal, CTempEnt_Splashes::STLaserSparks, (CTempEnt_Splashes::ESplashType)gameEntity->state.skinNum, 4);
					break;
				}

				ignore = tr.ent;
				Vec3Copy (tr.endPos, start);
			}

			CTempEnt_Trails::BFGLaser(origin, tr.endPos);
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

	vec3_t boltOrigin;
	State.GetOrigin(boltOrigin);
	if (gameEntity->owner->client)
		PlayerNoise(dynamic_cast<CPlayerEntity*>(gameEntity->owner->Entity), boltOrigin, PNOISE_IMPACT);

	// core explosion - prevents firing it into the wall/floor
	if (other->gameEntity->takedamage)
		T_Damage (other->gameEntity, gameEntity, gameEntity->owner, gameEntity->velocity, boltOrigin, plane->normal, 200, 0, 0, MOD_BFG_BLAST);
	T_RadiusDamage(gameEntity, gameEntity->owner, 200, other->gameEntity, 100, MOD_BFG_BLAST);

	PlaySoundFrom (gameEntity, CHAN_VOICE, SoundIndex ("weapons/bfg__x1b.wav"));
	gameEntity->solid = SOLID_NOT;
	Exploded = true;
	Vec3MA (boltOrigin, -0.1f, gameEntity->velocity, boltOrigin);
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

void CBFGBolt::Spawn	(CBaseEntity *Spawner, vec3_t start, vec3_t dir,
						int damage, int speed, float damage_radius)
{
	CBFGBolt	*BFG = QNew (com_levelPool, 0) CBFGBolt;

	BFG->State.SetOrigin (start);

	vec3_t dirAngles;
	VecToAngles (dir, dirAngles);
	BFG->State.SetAngles (dirAngles);
	Vec3Scale (dir, speed, BFG->gameEntity->velocity);
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

CTrace CHitScan::DoTrace(vec3_t start, vec3_t end, CBaseEntity *ignore, int mask)
{
	return CTrace (start, end, (ignore) ? ignore->gameEntity : NULL, mask);
}

bool CHitScan::DoDamage (CBaseEntity *Attacker, CBaseEntity *Target, vec3_t dir, vec3_t point, vec3_t normal)
{
	return true;
}

void CHitScan::DoEffect	(vec3_t start, vec3_t end, bool water)
{
};

void CHitScan::DoSolidHit	(CTrace *Trace)
{
};

bool CHitScan::ModifyEnd(vec3_t aimDir, vec3_t start, vec3_t end)
{
	return false;
};

void CHitScan::DoWaterHit (CTrace *Trace)
{
}

#define HITSCANSTEP 100
void CHitScan::DoFire(CBaseEntity *Entity, vec3_t start, vec3_t aimdir)
{
	vec3_t end, from;
	vec3_t lastWaterStart, lastWaterEnd;

	// Calculate end
	if (!ModifyEnd(aimdir, start, end))
		Vec3MA (start, 8192, aimdir, end);

	Vec3Copy (start, from);

	int Mask = CONTENTS_MASK_SHOT|CONTENTS_MASK_WATER;
	bool Water = false;
	CBaseEntity *Ignore = Entity;

	Vec3Copy (start, lastWaterStart);

	bool hitOutOfWater = false;
	if (gi.pointcontents(start) & CONTENTS_MASK_WATER)
	{
		// Copy up our point for the effect
		Vec3Copy (start, lastWaterEnd);
	
		// Special case if we started in water
		Water = true;

		Mask = CONTENTS_MASK_SHOT;

		// Find the exit point
		int tries = 20; // Cover about 2000 units
		vec3_t	stWater;

		Vec3Copy (from, stWater);
		Vec3Clear (lastWaterStart);
		
		while (tries > 0)
		{
			Vec3MA (stWater, HITSCANSTEP, aimdir, stWater);

			int contents = gi.pointcontents(stWater);
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
			vec3_t tempOrigin;
			Vec3MA (stWater, -(HITSCANSTEP + 5), aimdir, tempOrigin);
			CTrace tempTrace = DoTrace (stWater, tempOrigin, NULL, CONTENTS_MASK_WATER);

			if (tempTrace.contents & CONTENTS_MASK_WATER)// All is good
			{
				// This is our end
				Vec3Copy (tempTrace.endPos, lastWaterStart);
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
		if (Trace.ent && Trace.ent->Entity && Trace.ent->takedamage)
		{
			// Convert to base entity
			CBaseEntity *Target = dynamic_cast<CBaseEntity*>(Trace.ent->Entity);

			// Hurt it
			// Revision
			// Startsolid mistake..
			if (Trace.startSolid)
			{
				vec3_t origin;
				Target->State.GetOrigin(origin);
				if (!DoDamage (Entity, Target, aimdir, origin, Trace.plane.normal))
					break; // We wanted to stop

				// Set up the start from where we are now
				vec3_t oldFrom;
				Vec3Copy (from, oldFrom);
				Vec3Copy (origin, from);

				DoEffect (from, oldFrom, Water);

				// and ignore the bastard
				Ignore = Target;

				// Continue our loop
				continue;
			}

			if (!DoDamage (Entity, Target, aimdir, Trace.endPos, Trace.plane.normal))
				break; // We wanted to stop

			// Set up the start from where we are now
			vec3_t oldFrom;
			Vec3Copy (from, oldFrom);
			Vec3Copy (Trace.endPos, from);

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
				if (Vec3Compare(lastWaterStart, vec3Origin))
				{
					// We hit the ground!
					// Swap start and end points
					Vec3Copy (lastWaterEnd, lastWaterStart);
					Vec3MA (lastWaterStart, 5, aimdir, lastWaterStart);

					// Set end point
					Vec3Copy (Trace.endPos, lastWaterEnd);

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
						Vec3Copy (lastWaterEnd, lastWaterStart);
					else
						hitOutOfWater = false;

					// Set end point
					Vec3Copy (Trace.endPos, lastWaterEnd);

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
			Vec3Copy (Trace.endPos, lastWaterEnd);

			// Tell the system we're in water
			Water = true;

			// Draw the effect
			DoEffect (lastWaterStart, lastWaterEnd, false);

			DoWaterHit (&Trace);

			// Set up the start from where we are now
			Vec3Copy (Trace.endPos, from);

			Mask = CONTENTS_MASK_SHOT;

			// Find the exit point
			int tries = 20; // Cover about 2000 units
			vec3_t	stWater;

			Vec3Copy (from, stWater);
			Vec3Clear (lastWaterStart);
			
			while (tries > 0)
			{
				Vec3MA (stWater, HITSCANSTEP, aimdir, stWater);

				int contents = gi.pointcontents(stWater);
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
				vec3_t tempOrigin;
				Vec3MA (stWater, -(HITSCANSTEP + 5), aimdir, tempOrigin);
				CTrace tempTrace = DoTrace (stWater, tempOrigin, NULL, CONTENTS_MASK_WATER);

				if (tempTrace.contents & CONTENTS_MASK_WATER) // All is good
				{
					// This is our end
					Vec3Copy (tempTrace.endPos, lastWaterStart);
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
			DoEffect (from, Trace.endPos, false);
			DoSolidHit (&Trace);
			break; // We're done
		}
	}
}

bool CRailGunShot::DoDamage (CBaseEntity *Attacker, CBaseEntity *Target, vec3_t dir, vec3_t point, vec3_t normal)
{
	T_Damage (Target->gameEntity, Attacker->gameEntity, Attacker->gameEntity, dir, point, normal, Damage, Kick, 0, MOD_RAILGUN);
	return ThroughAndThrough;
}

void CRailGunShot::DoEffect	(vec3_t start, vec3_t end, bool water)
{
	CTempEnt_Trails::RailTrail (start, end);
}

void CRailGunShot::Fire(CBaseEntity *Entity, vec3_t start, vec3_t aimdir, int damage, int kick)
{
	CRailGunShot(damage, kick).DoFire (Entity, start, aimdir);
}

bool CBullet::DoDamage (CBaseEntity *Attacker, CBaseEntity *Target, vec3_t dir, vec3_t point, vec3_t normal)
{
	T_Damage (Target->gameEntity, Attacker->gameEntity, Attacker->gameEntity, dir, point, normal, Damage, Kick, DAMAGE_BULLET, MeansOfDeath);
	return ThroughAndThrough;
}

void CBullet::DoSolidHit	(CTrace *Trace)
{
	if (strncmp (Trace->surface->name, "sky", 3) != 0)
		CTempEnt_Splashes::Gunshot (Trace->endPos, Trace->plane.normal);
}

bool CBullet::ModifyEnd (vec3_t aimDir, vec3_t start, vec3_t end)
{
	vec3_t dir, forward, right, up;
	VecToAngles (aimDir, dir);
	Angles_Vectors (dir, forward, right, up);

	float r = crandom()*hSpread;
	float u = crandom()*vSpread;
	Vec3MA (start, 8192, forward, end);
	Vec3MA (end, r, right, end);
	Vec3MA (end, u, up, end);
	return true;
}

void CBullet::DoEffect	(vec3_t start, vec3_t end, bool water)
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

	CTempEnt_Splashes::Splash (Trace->endPos, Trace->plane.normal, color);
}

void CBullet::Fire(CBaseEntity *Entity, vec3_t start, vec3_t aimdir, int damage, int kick, int hSpread, int vSpread, int mod)
{
	CBullet(damage, kick, hSpread, vSpread, mod).DoFire (Entity, start, aimdir);
}

// An overload to handle transparent water
void CBullet::DoFire(CBaseEntity *Entity, vec3_t start, vec3_t aimdir)
{
	vec3_t end, from;
	vec3_t lastWaterStart, lastWaterEnd;

	// Calculate end
	if (!ModifyEnd(aimdir, start, end))
		Vec3MA (start, 8192, aimdir, end);

	Vec3Copy (start, from);

	int Mask = CONTENTS_MASK_SHOT|CONTENTS_MASK_WATER;
	bool Water = false;
	CBaseEntity *Ignore = Entity;

	Vec3Copy (start, lastWaterStart);

	bool hitOutOfWater = false;
	if (gi.pointcontents(start) & CONTENTS_MASK_WATER)
	{
		// Copy up our point for the effect
		Vec3Copy (start, lastWaterEnd);
	
		// Special case if we started in water
		Water = true;

		Mask = CONTENTS_MASK_SHOT;

		// Find the exit point
		int tries = 20; // Cover about 2000 units
		vec3_t	stWater;

		Vec3Copy (from, stWater);
		Vec3Clear (lastWaterStart);
		
		while (tries > 0)
		{
			Vec3MA (stWater, HITSCANSTEP, aimdir, stWater);

			int contents = gi.pointcontents(stWater);
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
			vec3_t tempOrigin;
			Vec3MA (stWater, -(HITSCANSTEP + 5), aimdir, tempOrigin);
			CTrace tempTrace = DoTrace (stWater, tempOrigin, NULL, CONTENTS_MASK_WATER);

			if (tempTrace.contents & CONTENTS_MASK_WATER)// All is good
			{
				// This is our end
				Vec3Copy (tempTrace.endPos, lastWaterStart);
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
		if (Trace.ent && Trace.ent->Entity && Trace.ent->takedamage)
		{
			// Convert to base entity
			CBaseEntity *Target = dynamic_cast<CBaseEntity*>(Trace.ent->Entity);

			// Hurt it
			// Revision
			// Startsolid mistake..
			if (Trace.startSolid)
			{
				vec3_t origin;
				Target->State.GetOrigin(origin);
				if (!DoDamage (Entity, Target, aimdir, origin, Trace.plane.normal))
					break; // We wanted to stop

				// Set up the start from where we are now
				vec3_t oldFrom;
				Vec3Copy (from, oldFrom);
				Vec3Copy (origin, from);

				DoEffect (from, oldFrom, Water);

				// and ignore the bastard
				Ignore = Target;

				// Continue our loop
				continue;
			}
			if (!DoDamage (Entity, Target, aimdir, Trace.endPos, Trace.plane.normal))
				break; // We wanted to stop

			// Set up the start from where we are now
			vec3_t oldFrom;
			Vec3Copy (from, oldFrom);
			Vec3Copy (Trace.endPos, from);

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
				if (Vec3Compare(lastWaterStart, vec3Origin))
				{
					// We hit the ground!
					// Swap start and end points
					Vec3Copy (lastWaterEnd, lastWaterStart);
					Vec3MA (lastWaterStart, 5, aimdir, lastWaterStart);

					// Set end point
					Vec3Copy (Trace.endPos, lastWaterEnd);

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
						Vec3Copy (lastWaterEnd, lastWaterStart);
					else
						hitOutOfWater = false;

					// Set end point
					Vec3Copy (Trace.endPos, lastWaterEnd);

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
			Vec3Copy (Trace.endPos, lastWaterEnd);

			// Tell the system we're in water
			Water = true;

			// Draw the effect
			DoEffect (lastWaterStart, lastWaterEnd, false);

			DoWaterHit (&Trace);

			// Set up the start from where we are now
			Vec3Copy (Trace.endPos, from);

			Mask = CONTENTS_MASK_SHOT;

			// Find the exit point
			int tries = 20; // Cover about 2000 units
			vec3_t	stWater;

			Vec3Copy (from, stWater);
			Vec3Clear (lastWaterStart);
			
			while (tries > 0)
			{
				Vec3MA (stWater, HITSCANSTEP, aimdir, stWater);

				int contents = gi.pointcontents(stWater);
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
				vec3_t tempOrigin;
				Vec3MA (stWater, -(HITSCANSTEP + 5), aimdir, tempOrigin);
				CTrace tempTrace = DoTrace (stWater, tempOrigin, NULL, CONTENTS_MASK_WATER);

				if (tempTrace.contents & CONTENTS_MASK_WATER) // All is good
				{
					// This is our end
					Vec3Copy (tempTrace.endPos, lastWaterStart);
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
			Vec3Copy (Trace.endPos, lastWaterEnd);

			// Draw the effect we have so far
			DoEffect (from, Trace.endPos, false);

			// Water hit effect
			DoWaterHit (&Trace);

			// Set up water drawing
			Water = true;
			Mask = CONTENTS_MASK_SHOT;

			// Find the exit point
			int tries = 20; // Cover about 2000 units
			vec3_t	stWater;

			Vec3Copy (from, stWater);
			Vec3Clear (lastWaterStart);
			
			while (tries > 0)
			{
				Vec3MA (stWater, HITSCANSTEP, aimdir, stWater);

				int contents = gi.pointcontents(stWater);
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
				vec3_t tempOrigin;
				Vec3MA (stWater, -(HITSCANSTEP + 5), aimdir, tempOrigin);
				CTrace tempTrace = DoTrace (stWater, tempOrigin, NULL, CONTENTS_MASK_WATER);

				if (tempTrace.contents & CONTENTS_MASK_WATER) // All is good
				{
					// This is our end
					Vec3Copy (tempTrace.endPos, lastWaterStart);
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
			DoEffect (from, Trace.endPos, false);
			DoSolidHit (&Trace);
			break; // We're done
		}
	}
}

void CShotgunPellets::DoSolidHit	(CTrace *Trace)
{
	if (strncmp (Trace->surface->name, "sky", 3) != 0)
		CTempEnt_Splashes::Shotgun (Trace->endPos, Trace->plane.normal);
}

void CShotgunPellets::Fire(CBaseEntity *Entity, vec3_t start, vec3_t aimdir, int damage, int kick, int hSpread, int vSpread, int Count, int mod)
{
	for (int i = 0; i < Count; i++)
		CShotgunPellets(damage, kick, hSpread, vSpread, mod).DoFire (Entity, start, aimdir);
}

bool CMeleeWeapon::Fire(CBaseEntity *Entity, vec3_t aim, int damage, int kick)
{
	CTrace		tr;
	vec3_t		forward, right, up;
	vec3_t		v;
	vec3_t		point;
	float		range;
	vec3_t		dir;

	Entity->State.GetOrigin(dir);
	//see if enemy is in range
	Vec3Subtract (Entity->gameEntity->enemy->state.origin, dir, dir);
	range = Vec3Length(dir);
	if (range > aim[0])
		return false;

	if (aim[1] > Entity->GetMins().X && aim[1] < Entity->GetMaxs().X)
	{
		// the hit is straight on so back the range up to the edge of their bbox
		range -= Entity->gameEntity->enemy->maxs[0];
	}
	else
	{
		// this is a side hit so adjust the "right" value out to the edge of their bbox
		if (aim[1] < 0)
			aim[1] = Entity->gameEntity->enemy->mins[0];
		else
			aim[1] = Entity->gameEntity->enemy->maxs[0];
	}

	vec3_t origin;
	Entity->State.GetOrigin(origin);
	Vec3MA (origin, range, dir, point);

	tr = CTrace (origin, point, Entity->gameEntity, CONTENTS_MASK_SHOT);
	if (tr.fraction < 1)
	{
		if (!tr.ent->takedamage)
			return false;
		// if it will hit any client/monster then hit the one we wanted to hit
		if ((tr.ent->svFlags & SVF_MONSTER) || (tr.ent->client))
			tr.ent = Entity->gameEntity->enemy;
	}

	vec3_t angles;
	Entity->State.GetAngles(angles);

	Angles_Vectors(angles, forward, right, up);
	Vec3MA (origin, range, forward, point);
	Vec3MA (point, aim[1], right, point);
	Vec3MA (point, aim[2], up, point);
	Vec3Subtract (point, origin, dir);

	// do the damage
	T_Damage (tr.ent, Entity->gameEntity, Entity->gameEntity, dir, point, vec3Origin, damage, kick/2, DAMAGE_NO_KNOCKBACK, MOD_HIT);

	if (!(tr.ent->svFlags & SVF_MONSTER) && (!tr.ent->client))
		return false;

	// do our special form of knockback here
	Vec3MA (Entity->gameEntity->enemy->absMin, 0.5, Entity->gameEntity->enemy->size, v);
	Vec3Subtract (v, point, v);
	VectorNormalizef (v, v);
	Vec3MA (Entity->gameEntity->enemy->velocity, kick, v, Entity->gameEntity->enemy->velocity);
	if (Entity->gameEntity->enemy->velocity[2] > 0)
		Entity->gameEntity->enemy->groundentity = NULL;
	return true;
}