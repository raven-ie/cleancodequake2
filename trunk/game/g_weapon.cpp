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

/*
=================
check_dodge

This is a support routine used when a client is firing
a non-instant attack weapon.  It checks to see if a
monster's dodge function should be called.
=================
*/
void check_dodge (edict_t *self, vec3_t start, vec3_t dir, int speed)
{
	vec3_t	end;
	vec3_t	v;
	CTrace	tr;
	float	eta;

	// easy mode only ducks one quarter the time
	if (skill->Integer() == 0)
	{
		if (random() > 0.25)
			return;
	}
	Vec3MA (start, 8192, dir, end);
	tr = CTrace (start, end, self, CONTENTS_MASK_SHOT);

#ifdef MONSTER_USE_ROGUE_AI
	if ((tr.ent) && (tr.ent->Monster) && (tr.ent->health > 0) && (tr.ent->Monster->MonsterFlags & MF_HAS_DODGE) && infront(tr.ent, self))
	{
		Vec3Subtract (tr.endPos, start, v);
		eta = (Vec3Length(v) - tr.ent->maxs[0]) / speed;
		tr.ent->Monster->Dodge (self, eta, &tr);

		if (tr.ent->enemy != self)
			tr.ent->enemy = self;
	}
#else
	if ((tr.ent) && (tr.ent->Monster) && (tr.ent->health > 0) && infront(tr.ent, self))
	{
		Vec3Subtract (tr.endPos, start, v);
		eta = (Vec3Length(v) - tr.ent->maxs[0]) / speed;
		tr.ent->Monster->Dodge (self, eta);
	}
#endif
}


/*
=================
fire_hit

Used for all impact (hit/punch/slash) attacks
=================
*/
bool fire_hit (edict_t *self, vec3_t aim, int damage, int kick)
{
	CTrace	tr;
	vec3_t		forward, right, up;
	vec3_t		v;
	vec3_t		point;
	float		range;
	vec3_t		dir;

	//see if enemy is in range
	Vec3Subtract (self->enemy->state.origin, self->state.origin, dir);
	range = Vec3Length(dir);
	if (range > aim[0])
		return false;

	if (aim[1] > self->mins[0] && aim[1] < self->maxs[0])
	{
		// the hit is straight on so back the range up to the edge of their bbox
		range -= self->enemy->maxs[0];
	}
	else
	{
		// this is a side hit so adjust the "right" value out to the edge of their bbox
		if (aim[1] < 0)
			aim[1] = self->enemy->mins[0];
		else
			aim[1] = self->enemy->maxs[0];
	}

	Vec3MA (self->state.origin, range, dir, point);

	tr = CTrace (self->state.origin, point, self, CONTENTS_MASK_SHOT);
	if (tr.fraction < 1)
	{
		if (!tr.ent->takedamage)
			return false;
		// if it will hit any client/monster then hit the one we wanted to hit
		if ((tr.ent->svFlags & SVF_MONSTER) || (tr.ent->client))
			tr.ent = self->enemy;
	}

	Angles_Vectors(self->state.angles, forward, right, up);
	Vec3MA (self->state.origin, range, forward, point);
	Vec3MA (point, aim[1], right, point);
	Vec3MA (point, aim[2], up, point);
	Vec3Subtract (point, self->enemy->state.origin, dir);

	// do the damage
	T_Damage (tr.ent, self, self, dir, point, vec3Origin, damage, kick/2, DAMAGE_NO_KNOCKBACK, MOD_HIT);

	if (!(tr.ent->svFlags & SVF_MONSTER) && (!tr.ent->client))
		return false;

	// do our special form of knockback here
	Vec3MA (self->enemy->absMin, 0.5, self->enemy->size, v);
	Vec3Subtract (v, point, v);
	VectorNormalizef (v, v);
	Vec3MA (self->enemy->velocity, kick, v, self->enemy->velocity);
	if (self->enemy->velocity[2] > 0)
		self->enemy->groundentity = NULL;
	return true;
}


/*
=================
fire_lead

This is an internal support routine used for bullet/pellet based weapons.
=================
*/
static void fire_lead (edict_t *self, vec3_t start, vec3_t aimdir, int damage, int kick, int te_impact, int hspread, int vspread, int mod)
{
	CTrace		tr;
	vec3_t		dir;
	vec3_t		forward, right, up;
	vec3_t		end;
	float		r;
	float		u;
	vec3_t		water_start;
	bool		water = false;
	int			content_mask = CONTENTS_MASK_SHOT | CONTENTS_MASK_WATER;

	tr = CTrace (self->state.origin, start, self, CONTENTS_MASK_SHOT);
	if (!(tr.fraction < 1.0))
	{
		VecToAngles (aimdir, dir);
		Angles_Vectors (dir, forward, right, up);

		r = crandom()*hspread;
		u = crandom()*vspread;
		Vec3MA (start, 8192, forward, end);
		Vec3MA (end, r, right, end);
		Vec3MA (end, u, up, end);

		if (gi.pointcontents (start) & CONTENTS_MASK_WATER)
		{
			water = true;
			Vec3Copy (start, water_start);
			content_mask &= ~CONTENTS_MASK_WATER;
		}

		tr = CTrace (start, end, self, content_mask);

		// see if we hit water
		if (tr.contents & CONTENTS_MASK_WATER)
		{
			int		color;

			water = true;
			Vec3Copy (tr.endPos, water_start);

			if (!Vec3Compare (start, tr.endPos))
			{
				if (tr.contents & CONTENTS_WATER)
				{
					if (strcmp(tr.surface->name, "*brwater") == 0)
						color = SPLASH_BROWN_WATER;
					else
						color = SPLASH_BLUE_WATER;
				}
				else if (tr.contents & CONTENTS_SLIME)
					color = SPLASH_SLIME;
				else if (tr.contents & CONTENTS_LAVA)
					color = SPLASH_LAVA;
				else
					color = SPLASH_UNKNOWN;

				if (color != SPLASH_UNKNOWN)
					CTempEnt_Splashes::Splash (tr.endPos, tr.plane.normal, (CTempEnt_Splashes::ESplashType)color);

				// change bullet's course when it enters water
				Vec3Subtract (end, start, dir);
				VecToAngles (dir, dir);
				Angles_Vectors (dir, forward, right, up);
				r = crandom()*hspread*2;
				u = crandom()*vspread*2;
				Vec3MA (water_start, 8192, forward, end);
				Vec3MA (end, r, right, end);
				Vec3MA (end, u, up, end);
			}

			// re-trace ignoring water this time
			tr = CTrace (water_start, end, self, CONTENTS_MASK_SHOT);
		}
	}

	// send gun puff / flash
	if (!((tr.surface) && (tr.surface->flags & SURF_TEXINFO_SKY)))
	{
		if (tr.fraction < 1.0)
		{
			if (tr.ent->takedamage)
			{
				T_Damage (tr.ent, self, self, aimdir, tr.endPos, tr.plane.normal, damage, kick, DAMAGE_BULLET, mod);
			}
			else
			{
				if (strncmp (tr.surface->name, "sky", 3) != 0)
				{
					if (te_impact == TE_GUNSHOT)
						CTempEnt_Splashes::Gunshot (tr.endPos, tr.plane.normal);
					else
						CTempEnt_Splashes::Shotgun (tr.endPos, tr.plane.normal);

					if (self->client)
						PlayerNoise(dynamic_cast<CPlayerEntity*>(self->Entity), tr.endPos, PNOISE_IMPACT);
				}
			}
		}
	}

	// if went through water, determine where the end and make a bubble trail
	if (water)
	{
		vec3_t	pos;

		Vec3Subtract (tr.endPos, water_start, dir);
		VectorNormalizef (dir, dir);
		Vec3MA (tr.endPos, -2, dir, pos);
		if (gi.pointcontents (pos) & CONTENTS_MASK_WATER)
			Vec3Copy (pos, tr.endPos);
		else
			tr = CTrace (pos, water_start, tr.ent, CONTENTS_MASK_WATER);

		Vec3Add (water_start, tr.endPos, pos);
		Vec3Scale (pos, 0.5, pos);

		// FIXME: water_start or pos?
		CTempEnt_Trails::BubbleTrail (pos, tr.endPos);
	}
}


/*
=================
fire_bullet

Fires a single round.  Used for machinegun and chaingun.  Would be fine for
pistols, rifles, etc....
=================
*/
void fire_bullet (edict_t *self, vec3_t start, vec3_t aimdir, int damage, int kick, int hspread, int vspread, int mod)
{
	fire_lead (self, start, aimdir, damage, kick, TE_GUNSHOT, hspread, vspread, mod);
}


/*
=================
fire_shotgun

Shoots shotgun pellets.  Used by shotgun and super shotgun.
=================
*/
void fire_shotgun (edict_t *self, vec3_t start, vec3_t aimdir, int damage, int kick, int hspread, int vspread, int count, int mod)
{
	int		i;

	for (i = 0; i < count; i++)
		fire_lead (self, start, aimdir, damage, kick, TE_SHOTGUN, hspread, vspread, mod);
}


/*
=================
fire_blaster

Fires a single blaster bolt.  Used by the blaster and hyper blaster.
=================
*/
void blaster_touch (edict_t *self, edict_t *other, plane_t *plane, cmBspSurface_t *surf)
{
	int		mod;

	if (other == self->owner)
		return;

	if (surf && (surf->flags & SURF_TEXINFO_SKY))
	{
		G_FreeEdict (self);
		return;
	}

	if (self->owner->client)
		PlayerNoise(dynamic_cast<CPlayerEntity*>(self->owner->Entity), self->state.origin, PNOISE_IMPACT);

	if (other->takedamage)
	{
		if (self->spawnflags & 1)
			mod = MOD_HYPERBLASTER;
		else
			mod = MOD_BLASTER;
		T_Damage (other, self, self->owner, self->velocity, self->state.origin, plane ? plane->normal : vec3Origin, self->dmg, 1, DAMAGE_ENERGY, mod);
	}
	else
		CTempEnt_Splashes::Blaster(self->state.origin, plane ? plane->normal : vec3Origin);

	G_FreeEdict (self);
}

void fire_blaster (edict_t *self, vec3_t start, vec3_t dir, int damage, int speed, int effect, bool isHyper)
{
	edict_t		*bolt;
	CTrace	tr;

	VectorNormalizef (dir, dir);

	bolt = G_Spawn();
	bolt->svFlags = SVF_DEADMONSTER;
	// yes, I know it looks weird that projectiles are deadmonsters
	// what this means is that when prediction is used against the object
	// (blaster/hyperblaster shots), the player won't be solid clipped against
	// the object.  Right now trying to run into a firing hyperblaster
	// is very jerky since you are predicted 'against' the shots.
	Vec3Copy (start, bolt->state.origin);
	Vec3Copy (start, bolt->state.oldOrigin);
	VecToAngles (dir, bolt->state.angles);
	Vec3Scale (dir, speed, bolt->velocity);
	bolt->movetype = MOVETYPE_FLYMISSILE;
	bolt->clipMask = CONTENTS_MASK_SHOT;
	bolt->solid = SOLID_BBOX;
	bolt->state.effects |= effect;
	Vec3Clear (bolt->mins);
	Vec3Clear (bolt->maxs);
	bolt->state.modelIndex = ModelIndex ("models/objects/laser/tris.md2");
	bolt->state.sound = SoundIndex ("misc/lasfly.wav");
	bolt->owner = self;
	bolt->touch = blaster_touch;
	bolt->nextthink = level.framenum + 20;
	bolt->think = G_FreeEdict;
	bolt->dmg = damage;
	bolt->classname = "bolt";
	if (isHyper)
		bolt->spawnflags = 1;
	gi.linkentity (bolt);

	if (self->client)
		check_dodge (self, bolt->state.origin, dir, speed);

	tr = CTrace (self->state.origin, bolt->state.origin, bolt, CONTENTS_MASK_SHOT);
	if (tr.fraction < 1.0)
	{
		Vec3MA (bolt->state.origin, -10, dir, bolt->state.origin);
		bolt->touch (bolt, tr.ent, &tr.plane, tr.surface);
	}
}	


/*
=================
fire_grenade
=================
*/
static void Grenade_Explode (edict_t *ent)
{
	vec3_t		origin;
	int			mod;

	if (ent->owner->client)
		PlayerNoise(dynamic_cast<CPlayerEntity*>(ent->owner->Entity), ent->state.origin, PNOISE_IMPACT);

	//FIXME: if we are onground then raise our Z just a bit since we are a point?
	if (ent->enemy)
	{
		float	points;
		vec3_t	v;
		vec3_t	dir;

		Vec3Add (ent->enemy->mins, ent->enemy->maxs, v);
		Vec3MA (ent->enemy->state.origin, 0.5, v, v);
		Vec3Subtract (ent->state.origin, v, v);
		points = ent->dmg - 0.5 * Vec3Length (v);
		Vec3Subtract (ent->enemy->state.origin, ent->state.origin, dir);
		if (ent->spawnflags & 1)
			mod = MOD_HANDGRENADE;
		else
			mod = MOD_GRENADE;
		T_Damage (ent->enemy, ent, ent->owner, dir, ent->state.origin, vec3Origin, (int)points, (int)points, DAMAGE_RADIUS, mod);
	}

	if (ent->spawnflags & 2)
		mod = MOD_HELD_GRENADE;
	else if (ent->spawnflags & 1)
		mod = MOD_HG_SPLASH;
	else
		mod = MOD_G_SPLASH;
	T_RadiusDamage(ent, ent->owner, ent->dmg, ent->enemy, ent->dmg_radius, mod);

	Vec3Copy (ent->state.origin, origin);

	Vec3MA (ent->state.origin, -0.02f, ent->velocity, origin);
	if (ent->waterlevel)
	{
		if (ent->groundentity)
			CTempEnt_Explosions::GrenadeExplosion(origin, ent, true);
		else
			CTempEnt_Explosions::RocketExplosion(origin, ent, true);
	}
	else
	{
		if (ent->groundentity)
			CTempEnt_Explosions::GrenadeExplosion(origin, ent);
		else
			CTempEnt_Explosions::RocketExplosion(origin, ent);
	}

	G_FreeEdict (ent);
}

static void Grenade_Touch (edict_t *ent, edict_t *other, plane_t *plane, cmBspSurface_t *surf)
{
	if (other == ent->owner)
		return;

	if (surf && (surf->flags & SURF_TEXINFO_SKY))
	{
		G_FreeEdict (ent);
		return;
	}

	if (!other->takedamage)
	{
		if (ent->spawnflags & 1)
		{
			if (random() > 0.5)
				PlaySoundFrom (ent, CHAN_VOICE, SoundIndex ("weapons/hgrenb1a.wav"));
			else
				PlaySoundFrom (ent, CHAN_VOICE, SoundIndex ("weapons/hgrenb2a.wav"));
		}
		else
		{
			PlaySoundFrom (ent, CHAN_VOICE, SoundIndex ("weapons/grenlb1b.wav"));
		}
		return;
	}

	ent->enemy = other;
	Grenade_Explode (ent);
}

void fire_grenade (edict_t *self, vec3_t start, vec3_t aimdir, int damage, int speed, float timer, float damage_radius)
{
	edict_t	*grenade;
	vec3_t	dir;
	vec3_t	forward, right, up;

	VecToAngles (aimdir, dir);
	Angles_Vectors (dir, forward, right, up);

	grenade = G_Spawn();
	Vec3Copy (start, grenade->state.origin);
	Vec3Scale (aimdir, speed, grenade->velocity);
	Vec3MA (grenade->velocity, 200 + crandom() * 10.0, up, grenade->velocity);
	Vec3MA (grenade->velocity, crandom() * 10.0, right, grenade->velocity);
	Vec3Set (grenade->avelocity, 300, 300, 300);
	grenade->movetype = MOVETYPE_BOUNCE;
	grenade->clipMask = CONTENTS_MASK_SHOT;
	grenade->solid = SOLID_BBOX;
	grenade->state.effects |= EF_GRENADE;
	Vec3Clear (grenade->mins);
	Vec3Clear (grenade->maxs);
	grenade->state.modelIndex = ModelIndex ("models/objects/grenade/tris.md2");
	grenade->owner = self;
	grenade->touch = Grenade_Touch;
	grenade->nextthink = level.framenum + timer;
	grenade->think = Grenade_Explode;
	grenade->dmg = damage;
	grenade->dmg_radius = damage_radius;
	grenade->classname = "grenade";

	gi.linkentity (grenade);
}

void fire_grenade2 (edict_t *self, vec3_t start, vec3_t aimdir, int damage, int speed, float timer, float damage_radius, bool held)
{
	edict_t	*grenade;
	vec3_t	dir;
	vec3_t	forward, right, up;

	VecToAngles (aimdir, dir);
	Angles_Vectors (dir, forward, right, up);

	grenade = G_Spawn();
	Vec3Copy (start, grenade->state.origin);
	Vec3Scale (aimdir, speed, grenade->velocity);
	Vec3MA (grenade->velocity, 200 + crandom() * 10.0, up, grenade->velocity);
	Vec3MA (grenade->velocity, crandom() * 10.0, right, grenade->velocity);
	Vec3Set (grenade->avelocity, 300, 300, 300);
	grenade->movetype = MOVETYPE_BOUNCE;
	grenade->clipMask = CONTENTS_MASK_SHOT;
	grenade->solid = SOLID_BBOX;
	grenade->state.effects |= EF_GRENADE;
	Vec3Clear (grenade->mins);
	Vec3Clear (grenade->maxs);
	grenade->state.modelIndex = ModelIndex ("models/objects/grenade2/tris.md2");
	grenade->owner = self;
	grenade->touch = Grenade_Touch;
	grenade->nextthink = level.framenum + timer;
	grenade->think = Grenade_Explode;
	grenade->dmg = damage;
	grenade->dmg_radius = damage_radius;
	grenade->classname = "hgrenade";
	if (held)
		grenade->spawnflags = 3;
	else
		grenade->spawnflags = 1;
	grenade->state.sound = SoundIndex("weapons/hgrenc1b.wav");

	if (timer <= 0.0)
		Grenade_Explode (grenade);
	else
	{
		PlaySoundFrom (self, CHAN_WEAPON, SoundIndex ("weapons/hgrent1a.wav"));
		gi.linkentity (grenade);
	}
}


/*
=================
fire_rocket
=================
*/
void rocket_touch (edict_t *ent, edict_t *other, plane_t *plane, cmBspSurface_t *surf)
{
	vec3_t		origin;
	int			n;

	if (other == ent->owner)
		return;

	if (surf && (surf->flags & SURF_TEXINFO_SKY))
	{
		G_FreeEdict (ent);
		return;
	}

	if (ent->owner->client)
		PlayerNoise(dynamic_cast<CPlayerEntity*>(ent->owner->Entity), ent->state.origin, PNOISE_IMPACT);

	// calculate position for the explosion entity
	Vec3MA (ent->state.origin, -0.02f, ent->velocity, origin);

	if (other->takedamage)
	{
		T_Damage (other, ent, ent->owner, ent->velocity, ent->state.origin, plane->normal, ent->dmg, 0, 0, MOD_ROCKET);
	}
	else
	{
		// don't throw any debris in net games
		if (game.mode == GAME_SINGLEPLAYER)
		{
			if ((surf) && !(surf->flags & (SURF_TEXINFO_WARP|SURF_TEXINFO_TRANS33|SURF_TEXINFO_TRANS66|SURF_TEXINFO_FLOWING)))
			{
				n = rand() % 5;
				while(n--)
					ThrowDebris (ent, "models/objects/debris2/tris.md2", 2, ent->state.origin);
			}
		}
	}

	T_RadiusDamage(ent, ent->owner, ent->radius_dmg, other, ent->dmg_radius, MOD_R_SPLASH);

	if (ent->waterlevel)
		CTempEnt_Explosions::RocketExplosion(origin, ent, true);
	else
		CTempEnt_Explosions::RocketExplosion(origin, ent, false);

	G_FreeEdict (ent);
}

void fire_rocket (edict_t *self, vec3_t start, vec3_t dir, int damage, int speed, float damage_radius, int radius_damage)
{
	edict_t	*rocket;

	rocket = G_Spawn();
	Vec3Copy (start, rocket->state.origin);
	Vec3Copy (dir, rocket->movedir);
	VecToAngles (dir, rocket->state.angles);
	Vec3Scale (dir, speed, rocket->velocity);
	rocket->movetype = MOVETYPE_FLYMISSILE;
	rocket->clipMask = CONTENTS_MASK_SHOT;
	rocket->solid = SOLID_BBOX;
	rocket->state.effects |= EF_ROCKET;
	Vec3Clear (rocket->mins);
	Vec3Clear (rocket->maxs);
	rocket->state.modelIndex = ModelIndex ("models/objects/rocket/tris.md2");
	rocket->owner = self;
	rocket->touch = rocket_touch;
	rocket->nextthink = level.framenum + 80000/speed;
	rocket->think = G_FreeEdict;
	rocket->dmg = damage;
	rocket->radius_dmg = radius_damage;
	rocket->dmg_radius = damage_radius;
	rocket->state.sound = SoundIndex ("weapons/rockfly.wav");
	rocket->classname = "rocket";

	if (self->client)
		check_dodge (self, rocket->state.origin, dir, speed);

	gi.linkentity (rocket);
}


/*
=================
fire_rail
=================
*/
void fire_rail (edict_t *self, vec3_t start, vec3_t aimdir, int damage, int kick)
{
	vec3_t		from;
	vec3_t		end;
	CTrace	tr;
	vec3_t		waterHit;
	edict_t		*ignore;
	int			mask;
	bool	water;

	Vec3MA (start, 8192, aimdir, end);
	Vec3Copy (start, from);
	ignore = self;
	water = false;
	mask = CONTENTS_MASK_SHOT|CONTENTS_WATER|CONTENTS_SLIME|CONTENTS_LAVA;
	while (ignore)
	{
		tr = CTrace (from, end, ignore, mask);

		if (tr.contents & (CONTENTS_WATER|CONTENTS_SLIME|CONTENTS_LAVA))
		{
			water = true;
			Vec3Copy (tr.endPos, waterHit);

			// re-trace ignoring water
			mask = CONTENTS_MASK_SHOT;
			Vec3MA (tr.endPos, 8192, aimdir, end);
		}
		else
		{
			//ZOID--added so rail goes through SOLID_BBOX entities (gibs, etc)
			if ((tr.ent->svFlags & SVF_MONSTER) || (tr.ent->client) ||
				(tr.ent->solid == SOLID_BBOX))
				ignore = tr.ent;
			else
				ignore = NULL;

			if ((tr.ent != self) && (tr.ent->takedamage))
				T_Damage (tr.ent, self, self, aimdir, tr.endPos, tr.plane.normal, damage, kick, 0, MOD_RAILGUN);
		}

		Vec3Copy (tr.endPos, from);
	}

	// send gun puff / flash
	if (water)
	{
		CTempEnt_Trails::RailTrail (start, tr.endPos);
		CTempEnt_Trails::RailTrail (tr.endPos, start);
	}
	else
		CTempEnt_Trails::RailTrail (start, tr.endPos);

	if (self->client)
		PlayerNoise(dynamic_cast<CPlayerEntity*>(self->Entity), tr.endPos, PNOISE_IMPACT);
}


/*
=================
fire_bfg
=================
*/
void bfg_explode (edict_t *self)
{
	edict_t	*ent;
	float	points;
	vec3_t	v;
	float	dist;

	if (self->state.frame == 0)
	{
		// the BFG effect
		ent = NULL;
		while ((ent = findradius(ent, self->state.origin, self->dmg_radius)) != NULL)
		{
			if (!ent->takedamage)
				continue;
			if (ent == self->owner)
				continue;
			if (!CanDamage (ent, self))
				continue;
			if (!CanDamage (ent, self->owner))
				continue;

			Vec3Add (ent->mins, ent->maxs, v);
			Vec3MA (ent->state.origin, 0.5, v, v);
			Vec3Subtract (self->state.origin, v, v);
			dist = Vec3Length(v);
			points = self->radius_dmg * (1.0f - sqrtf(dist/self->dmg_radius));
			if (ent == self->owner)
				points = points * 0.5;

			CTempEnt_Explosions::BFGExplosion (ent->state.origin);
			T_Damage (ent, self, self->owner, self->velocity, ent->state.origin, vec3Origin, (int)points, 0, DAMAGE_ENERGY, MOD_BFG_EFFECT);
		}
	}

	self->nextthink = level.framenum + FRAMETIME;
	self->state.frame++;
	if (self->state.frame == 5)
		self->think = G_FreeEdict;
}

void bfg_touch (edict_t *self, edict_t *other, plane_t *plane, cmBspSurface_t *surf)
{
	if (other == self->owner)
		return;

	if (surf && (surf->flags & SURF_TEXINFO_SKY))
	{
		G_FreeEdict (self);
		return;
	}

	if (self->owner->client)
		PlayerNoise(dynamic_cast<CPlayerEntity*>(self->owner->Entity), self->state.origin, PNOISE_IMPACT);

	// core explosion - prevents firing it into the wall/floor
	if (other->takedamage)
		T_Damage (other, self, self->owner, self->velocity, self->state.origin, plane->normal, 200, 0, 0, MOD_BFG_BLAST);
	T_RadiusDamage(self, self->owner, 200, other, 100, MOD_BFG_BLAST);

	PlaySoundFrom (self, CHAN_VOICE, SoundIndex ("weapons/bfg__x1b.wav"));
	self->solid = SOLID_NOT;
	self->touch = NULL;
	Vec3MA (self->state.origin, -0.1f, self->velocity, self->state.origin);
	Vec3Clear (self->velocity);
	self->state.modelIndex = ModelIndex ("sprites/s_bfg3.sp2");
	self->state.frame = 0;
	self->state.sound = 0;
	self->state.effects &= ~EF_ANIM_ALLFAST;
	self->think = bfg_explode;
	self->nextthink = level.framenum + FRAMETIME;
	self->enemy = other;

	CTempEnt_Explosions::BFGExplosion (self->state.origin, true);
}


void bfg_think (edict_t *self)
{
	edict_t	*ent;
	edict_t	*ignore;
	vec3_t	point;
	vec3_t	dir;
	vec3_t	start;
	vec3_t	end;
	int		dmg;
	CTrace	tr;

	if (game.mode & GAME_DEATHMATCH)
		dmg = 5;
	else
		dmg = 10;

	ent = NULL;
	while ((ent = findradius(ent, self->state.origin, 256)) != NULL)
	{
		if (ent == self)
			continue;

		if (ent == self->owner)
			continue;

		if (!ent->takedamage)
			continue;

		if (!(ent->svFlags & SVF_MONSTER) && (!ent->client) && (strcmp(ent->classname, "misc_explobox") != 0))
			continue;

#ifdef CLEANCTF_ENABLED
//ZOID
		//don't target players in CTF
		if ((game.mode & GAME_CTF) && ent->Entity && (ent->Entity->EntityFlags & ENT_PLAYER) &&
			self->owner->client)
		{
			if ((dynamic_cast<CPlayerEntity*>(ent->Entity))->Client.resp.ctf_team == (dynamic_cast<CPlayerEntity*>(self->owner->Entity))->Client.resp.ctf_team)
			continue;
		}
//ZOID
#endif

		Vec3MA (ent->absMin, 0.5, ent->size, point);

		Vec3Subtract (point, self->state.origin, dir);
		VectorNormalizef (dir, dir);

		ignore = self;
		Vec3Copy (self->state.origin, start);
		Vec3MA (start, 2048, dir, end);
		while(1)
		{
			tr = CTrace (start, end, ignore, CONTENTS_SOLID|CONTENTS_MONSTER|CONTENTS_DEADMONSTER);

			if (!tr.ent)
				break;

			// hurt it if we can
			if ((tr.ent->takedamage) && !(tr.ent->flags & FL_IMMUNE_LASER) && (tr.ent != self->owner))
				T_Damage (tr.ent, self, self->owner, dir, tr.endPos, vec3Origin, dmg, 1, DAMAGE_ENERGY, MOD_BFG_LASER);

			// if we hit something that's not a monster or player we're done
			if (!(tr.ent->svFlags & SVF_MONSTER) && (!tr.ent->client))
			{
				CTempEnt_Splashes::Sparks (tr.endPos, tr.plane.normal, CTempEnt_Splashes::STLaserSparks, (CTempEnt_Splashes::ESplashType)self->state.skinNum, 4);
				break;
			}

			ignore = tr.ent;
			Vec3Copy (tr.endPos, start);
		}

		CTempEnt_Trails::BFGLaser(self->state.origin, tr.endPos);
	}

	self->nextthink = level.framenum + FRAMETIME;
}


void fire_bfg (edict_t *self, vec3_t start, vec3_t dir, int damage, int speed, float damage_radius)
{
	edict_t	*bfg;

	bfg = G_Spawn();
	Vec3Copy (start, bfg->state.origin);
	Vec3Copy (dir, bfg->movedir);
	VecToAngles (dir, bfg->state.angles);
	Vec3Scale (dir, speed, bfg->velocity);
	bfg->movetype = MOVETYPE_FLYMISSILE;
	bfg->clipMask = CONTENTS_MASK_SHOT;
	bfg->solid = SOLID_BBOX;
	bfg->state.effects |= EF_BFG | EF_ANIM_ALLFAST;
	Vec3Clear (bfg->mins);
	Vec3Clear (bfg->maxs);
	bfg->state.modelIndex = ModelIndex ("sprites/s_bfg1.sp2");
	bfg->owner = self;
	bfg->touch = bfg_touch;
	bfg->nextthink = level.framenum + 80000/speed;
	bfg->think = G_FreeEdict;
	bfg->radius_dmg = damage;
	bfg->dmg_radius = damage_radius;
	bfg->classname = "bfg blast";
	bfg->state.sound = SoundIndex ("weapons/bfg__l1a.wav");

	bfg->think = bfg_think;
	bfg->nextthink = level.framenum + FRAMETIME;
	bfg->teammaster = bfg;
	bfg->teamchain = NULL;

	if (self->client)
		check_dodge (self, bfg->state.origin, dir, speed);

	gi.linkentity (bfg);
}
