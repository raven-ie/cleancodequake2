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
CBounceProjectile()
{
};

CGrenade::CGrenade (int Index) :
CBounceProjectile(Index)
{
};

void CGrenade::Explode ()
{
	vec3_t		origin;
	int			mod;

	State.GetOrigin (origin);
	if (gameEntity->owner->client)
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

	Vec3MA (origin, -0.02, gameEntity->velocity, origin);
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
	CGrenade	*Grenade = new CGrenade();
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

/*
================
CBlasterProjectile
================
*/

CBlasterProjectile::CBlasterProjectile () :
CFlyMissileProjectile()
{
};

CBlasterProjectile::CBlasterProjectile (int Index) :
CFlyMissileProjectile(Index)
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

void check_dodge (edict_t *self, vec3_t start, vec3_t dir, int speed);
void CBlasterProjectile::Spawn (CBaseEntity *Spawner, vec3_t start, vec3_t dir,
						int damage, int speed, int effect, bool isHyper)
{
	CBlasterProjectile		*Bolt = new CBlasterProjectile;

	VectorNormalizef (dir, dir);

	Bolt->SetSvFlags (SVF_PROJECTILE);
	Bolt->State.SetOrigin (start);
	Bolt->State.SetOldOrigin (start);
	vec3_t dirAngles;
	VecToAngles (dir, dirAngles);
	Bolt->State.SetAngles (dirAngles);
	Vec3Scale (dir, speed, Bolt->gameEntity->velocity);
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

	if (Spawner->EntityFlags & ENT_PLAYER)
		check_dodge (Spawner->gameEntity, start, dir, speed);

	vec3_t origin;
	Spawner->State.GetOrigin (origin);
	CTrace tr = CTrace (origin, start, Bolt->gameEntity, CONTENTS_MASK_SHOT);
	if (tr.fraction < 1.0)
	{
		Vec3MA (start, -10, dir, start);

		if (tr.ent->Entity)
			Bolt->Touch (tr.ent->Entity, &tr.plane, tr.surface);
	}
}

CRocket::CRocket () :
CFlyMissileProjectile()
{
};

CRocket::CRocket (int Index) :
CFlyMissileProjectile(Index)
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
	Vec3MA (origin, -0.02, gameEntity->velocity, origin);
	T_RadiusDamage(gameEntity, gameEntity->owner, RadiusDamage, other->gameEntity, DamageRadius, MOD_R_SPLASH);
	CTempEnt_Explosions::RocketExplosion(origin, gameEntity, !!gameEntity->waterlevel);

	Free ();
}

void CRocket::Spawn	(CBaseEntity *Spawner, vec3_t start, vec3_t dir,
						int damage, int speed, float damage_radius, int radius_damage)
{
	CRocket	*Rocket = new CRocket;

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
		check_dodge (Spawner->gameEntity, start, dir, speed);

	Rocket->Link ();
}