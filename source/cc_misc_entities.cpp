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

/*QUAKED misc_explobox (0 .5 .8) (-16 -16 0) (16 16 40)
Large exploding box.  You can override its mass (100),
health (80), and dmg (150).
*/
class CMiscExploBox : public CMapEntity, public CStepPhysics, public CHurtableEntity, public CThinkableEntity, public CTouchableEntity
{
	bool Dropped;
public:
	CMiscExploBox () :
	Dropped(false),
	CBaseEntity(),
	CMapEntity(),
	CThinkableEntity(),
	CHurtableEntity (),
	CStepPhysics()
	{
	};

	CMiscExploBox (int Index) : 
	Dropped(false),
	CBaseEntity(Index),
	CMapEntity(Index),
	CThinkableEntity(),
	CHurtableEntity(Index),
	CStepPhysics(Index)
	{
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
		G_TouchTriggers (this);
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
		gameEntity->takedamage = false;
		NextThink = level.framenum + 2;
		gameEntity->activator = attacker->gameEntity;
	};

	void Pain (CBaseEntity *other, float kick, int damage) {};

	bool Run ()
	{
		return CStepPhysics::Run ();
	};

	void Spawn ()
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

		gameEntity->takedamage = true;
		NextThink = level.framenum + FRAMETIME;

		Link ();
	};
};

LINK_CLASSNAME_TO_CLASS ("misc_explobox",CMiscExploBox);

#include "cc_brushmodels.h"
/*QUAKED misc_viper (1 .5 0) (-16 -16 0) (16 16 32)
This is the Viper for the flyby bombing.
It is trigger_spawned, so you must have something use it for it to show up.
There must be a path for it to follow once it is activated.

"speed"		How fast the Viper should fly
*/
class CMiscViper : public CTrainBase
{
	bool MyUse;
public:
	CMiscViper() :
		CBaseEntity (),
		CTrainBase(),
		MyUse(true)
	{
	};

	CMiscViper(int Index) :
		CBaseEntity (Index),
		CTrainBase(Index),
		MyUse(true)
	{
	};

	virtual void Use (CBaseEntity *other, CBaseEntity *activator)
	{
		if (MyUse)
		{
			SetSvFlags (GetSvFlags() & ~SVF_NOCLIENT);
			MyUse = false;
		}
		CTrainBase::Use (other, activator);
	};

	virtual void Spawn ()
	{
		if (!gameEntity->target)
		{
			//gi.dprintf ("misc_viper without a target at (%f %f %f)\n", ent->absMin[0], ent->absMin[1], ent->absMin[2]);
			MapPrint (MAPPRINT_ERROR, this, State.GetOrigin(), "No targetname\n");
			Free ();
			return;
		}

		if (!gameEntity->speed)
			gameEntity->speed = 300;

		PhysicsType = PHYSICS_PUSH;
		SetSolid (SOLID_NOT);
		State.SetModelIndex (ModelIndex ("models/ships/viper/tris.md2"));
		SetMins (vec3f(-16, -16, 0));
		SetMaxs (vec3f(16, 16, 32));

		NextThink = level.framenum + FRAMETIME;
		SetSvFlags (GetSvFlags() | SVF_NOCLIENT);
		Accel = Decel = Speed = gameEntity->speed;

		Link ();
	};
};

LINK_CLASSNAME_TO_CLASS ("misc_viper", CMiscViper);

class CMiscStroggShip : public CMiscViper
{
public:
	CMiscStroggShip () :
	  CBaseEntity (),
	  CMiscViper ()
	  {
	  };

	CMiscStroggShip (int Index) :
	  CBaseEntity (Index),
	  CMiscViper (Index)
	  {
	  };

	void Spawn ()
	{
		CMiscViper::Spawn ();
		State.SetModelIndex (ModelIndex ("models/ships/strogg1/tris.md2"));
	};
};

LINK_CLASSNAME_TO_CLASS ("misc_strogg_ship", CMiscStroggShip);

/*QUAKED misc_blackhole (1 .5 0) (-8 -8 -8) (8 8 8)
*/

void misc_blackhole_use (edict_t *ent, edict_t *other, edict_t *activator)
{
	G_FreeEdict (ent);
}

void misc_blackhole_think (edict_t *self)
{
	if (++self->state.frame < 19)
		self->nextthink = level.framenum + FRAMETIME;
	else
	{		
		self->state.frame = 0;
		self->nextthink = level.framenum + FRAMETIME;
	}
}

void SP_misc_blackhole (edict_t *ent)
{
	ent->movetype = MOVETYPE_NONE;
	ent->solid = SOLID_NOT;
	Vec3Set (ent->mins, -64, -64, 0);
	Vec3Set (ent->maxs, 64, 64, 8);
	ent->state.modelIndex = ModelIndex ("models/objects/black/tris.md2");
	ent->state.renderFx = RF_TRANSLUCENT;
	ent->use = misc_blackhole_use;
	ent->think = misc_blackhole_think;
	ent->nextthink = level.framenum + 2;
	gi.linkentity (ent);
}

/*QUAKED misc_eastertank (1 .5 0) (-32 -32 -16) (32 32 32)
*/

void misc_eastertank_think (edict_t *self)
{
	if (++self->state.frame < 293)
		self->nextthink = level.framenum + FRAMETIME;
	else
	{		
		self->state.frame = 254;
		self->nextthink = level.framenum + FRAMETIME;
	}
}

void SP_misc_eastertank (edict_t *ent)
{
	ent->movetype = MOVETYPE_NONE;
	ent->solid = SOLID_BBOX;
	Vec3Set (ent->mins, -32, -32, -16);
	Vec3Set (ent->maxs, 32, 32, 32);
	ent->state.modelIndex = ModelIndex ("models/monsters/tank/tris.md2");
	ent->state.frame = 254;
	ent->think = misc_eastertank_think;
	ent->nextthink = level.framenum + 2;
	gi.linkentity (ent);
}

/*QUAKED misc_easterchick (1 .5 0) (-32 -32 0) (32 32 32)
*/


void misc_easterchick_think (edict_t *self)
{
	if (++self->state.frame < 247)
		self->nextthink = level.framenum + FRAMETIME;
	else
	{		
		self->state.frame = 208;
		self->nextthink = level.framenum + FRAMETIME;
	}
}

void SP_misc_easterchick (edict_t *ent)
{
	ent->movetype = MOVETYPE_NONE;
	ent->solid = SOLID_BBOX;
	Vec3Set (ent->mins, -32, -32, 0);
	Vec3Set (ent->maxs, 32, 32, 32);
	ent->state.modelIndex = ModelIndex ("models/monsters/bitch/tris.md2");
	ent->state.frame = 208;
	ent->think = misc_easterchick_think;
	ent->nextthink = level.framenum + 2;
	gi.linkentity (ent);
}

/*QUAKED misc_easterchick2 (1 .5 0) (-32 -32 0) (32 32 32)
*/


void misc_easterchick2_think (edict_t *self)
{
	if (++self->state.frame < 287)
		self->nextthink = level.framenum + FRAMETIME;
	else
	{		
		self->state.frame = 248;
		self->nextthink = level.framenum + FRAMETIME;
	}
}

void SP_misc_easterchick2 (edict_t *ent)
{
	ent->movetype = MOVETYPE_NONE;
	ent->solid = SOLID_BBOX;
	Vec3Set (ent->mins, -32, -32, 0);
	Vec3Set (ent->maxs, 32, 32, 32);
	ent->state.modelIndex = ModelIndex ("models/monsters/bitch/tris.md2");
	ent->state.frame = 248;
	ent->think = misc_easterchick2_think;
	ent->nextthink = level.framenum + 2;
	gi.linkentity (ent);
}


/*QUAKED monster_commander_body (1 .5 0) (-32 -32 0) (32 32 48)
Not really a monster, this is the Tank Commander's decapitated body.
There should be a item_commander_head that has this as it's target.
*/

void commander_body_think (edict_t *self)
{
	if (++self->state.frame < 24)
		self->nextthink = level.framenum + FRAMETIME;
	else
		self->nextthink = 0;

	if (self->state.frame == 22)
		PlaySoundFrom (self, CHAN_BODY, SoundIndex ("tank/thud.wav"));
}

void commander_body_use (edict_t *self, edict_t *other, edict_t *activator)
{
	self->think = commander_body_think;
	self->nextthink = level.framenum + FRAMETIME;
	PlaySoundFrom (self, CHAN_BODY, SoundIndex ("tank/pain.wav"));
}

void commander_body_drop (edict_t *self)
{
	self->movetype = MOVETYPE_TOSS;
	self->state.origin[2] += 2;
}

void SP_monster_commander_body (edict_t *self)
{
	self->movetype = MOVETYPE_NONE;
	self->solid = SOLID_BBOX;
	self->model = "models/monsters/commandr/tris.md2";
	self->state.modelIndex = ModelIndex (self->model);
	Vec3Set (self->mins, -32, -32, 0);
	Vec3Set (self->maxs, 32, 32, 48);
	self->use = commander_body_use;
	self->takedamage = true;
	self->flags = FL_GODMODE;
	self->state.renderFx |= RF_FRAMELERP;
	gi.linkentity (self);

	SoundIndex ("tank/thud.wav");
	SoundIndex ("tank/pain.wav");

	self->think = commander_body_drop;
	self->nextthink = level.framenum + 5;
}


/*QUAKED misc_banner (1 .5 0) (-4 -4 -4) (4 4 4)
The origin is the bottom of the banner.
The banner is 128 tall.
*/
void misc_banner_think (edict_t *ent)
{
	ent->state.frame = (ent->state.frame + 1) % 16;
	ent->nextthink = level.framenum + FRAMETIME;
}

void SP_misc_banner (edict_t *ent)
{
	ent->movetype = MOVETYPE_NONE;
	ent->solid = SOLID_NOT;
	ent->state.modelIndex = ModelIndex ("models/objects/banner/tris.md2");
	ent->state.frame = rand() % 16;
	gi.linkentity (ent);

	ent->think = misc_banner_think;
	ent->nextthink = level.framenum + FRAMETIME;
}

/*QUAKED misc_deadsoldier (1 .5 0) (-16 -16 0) (16 16 16) ON_BACK ON_STOMACH BACK_DECAP FETAL_POS SIT_DECAP IMPALED
This is the dead player model. Comes in 6 exciting different poses!
*/
void misc_deadsoldier_die (edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
{
//	int		n;

	if (self->health > -80)
		return;

	PlaySoundFrom (self, CHAN_BODY, SoundIndex ("misc/udeath.wav"));
	//for (n= 0; n < 4; n++)
		//ThrowGib (self, gMedia.Gib_SmallMeat, damage, GIB_ORGANIC);
	//ThrowHead (self, gMedia.Gib_Head[1], damage, GIB_ORGANIC);
}

void SP_misc_deadsoldier (edict_t *ent)
{
	if (game.mode & GAME_DEATHMATCH)
	{	// auto-remove for deathmatch
		G_FreeEdict (ent);
		return;
	}

	ent->movetype = MOVETYPE_NONE;
	ent->solid = SOLID_BBOX;
	ent->state.modelIndex=ModelIndex ("models/deadbods/dude/tris.md2");

	// Defaults to frame 0
	if (ent->spawnflags & 2)
		ent->state.frame = 1;
	else if (ent->spawnflags & 4)
		ent->state.frame = 2;
	else if (ent->spawnflags & 8)
		ent->state.frame = 3;
	else if (ent->spawnflags & 16)
		ent->state.frame = 4;
	else if (ent->spawnflags & 32)
		ent->state.frame = 5;
	else
		ent->state.frame = 0;

	Vec3Set (ent->mins, -16, -16, 0);
	Vec3Set (ent->maxs, 16, 16, 16);
	ent->deadflag = DEAD_DEAD;
	ent->takedamage = true;
	ent->svFlags |= SVF_MONSTER|SVF_DEADMONSTER;
	ent->die = misc_deadsoldier_die;

	gi.linkentity (ent);
}

/*QUAKED misc_bigviper (1 .5 0) (-176 -120 -24) (176 120 72) 
This is a large stationary viper as seen in Paul's intro
*/
void SP_misc_bigviper (edict_t *ent)
{
	ent->movetype = MOVETYPE_NONE;
	ent->solid = SOLID_BBOX;
	Vec3Set (ent->mins, -176, -120, -24);
	Vec3Set (ent->maxs, 176, 120, 72);
	ent->state.modelIndex = ModelIndex ("models/ships/bigviper/tris.md2");
	gi.linkentity (ent);
}


/*QUAKED misc_viper_bomb (1 0 0) (-8 -8 -8) (8 8 8)
"dmg"	how much boom should the bomb make?
*/
void misc_viper_bomb_touch (edict_t *self, edict_t *other, plane_t *plane, cmBspSurface_t *surf)
{
	G_UseTargets (self, self->activator);

	self->state.origin[2] = self->absMin[2] + 1;
	T_RadiusDamage (self, self, self->dmg, NULL, self->dmg+40, MOD_BOMB);
//	BecomeExplosion2 (self);
}

void misc_viper_bomb_prethink (edict_t *self)
{
	vec3_t	v;
	float	diff;

	self->groundentity = NULL;

	diff = self->timestamp - level.framenum;
	if (diff < -1.0)
		diff = -1.0;

	Vec3Scale (self->moveinfo.dir, 1.0 + diff, v);
	v[2] = diff;

	diff = self->state.angles[2];
	VecToAngles (v, self->state.angles);
	self->state.angles[2] = diff + 10;
}

void misc_viper_bomb_use (edict_t *self, edict_t *other, edict_t *activator)
{
	edict_t	*viper;

	self->solid = SOLID_BBOX;
	self->svFlags &= ~SVF_NOCLIENT;
	self->state.effects |= EF_ROCKET;
	self->use = NULL;
	self->movetype = MOVETYPE_TOSS;
	self->prethink = misc_viper_bomb_prethink;
	self->touch = misc_viper_bomb_touch;
	self->activator = activator;

	viper = G_Find (NULL, FOFS(classname), "misc_viper");
	Vec3Scale (viper->moveinfo.dir, viper->moveinfo.speed, self->velocity);

	self->timestamp = level.framenum;
	Vec3Copy (viper->moveinfo.dir, self->moveinfo.dir);
}

void SP_misc_viper_bomb (edict_t *self)
{
	self->movetype = MOVETYPE_NONE;
	self->solid = SOLID_NOT;
	Vec3Set (self->mins, -8, -8, -8);
	Vec3Set (self->maxs, 8, 8, 8);

	self->state.modelIndex = ModelIndex ("models/objects/bomb/tris.md2");

	if (!self->dmg)
		self->dmg = 1000;

	//self->use = misc_viper_bomb_use;
	self->svFlags |= SVF_NOCLIENT;

	gi.linkentity (self);
}

/*QUAKED misc_satellite_dish (1 .5 0) (-64 -64 0) (64 64 128)
*/
void misc_satellite_dish_think (edict_t *self)
{
	self->state.frame++;
	if (self->state.frame < 38)
		self->nextthink = level.framenum + FRAMETIME;
}

void misc_satellite_dish_use (edict_t *self, edict_t *other, edict_t *activator)
{
	self->state.frame = 0;
	self->think = misc_satellite_dish_think;
	self->nextthink = level.framenum + FRAMETIME;
}

void SP_misc_satellite_dish (edict_t *ent)
{
	ent->movetype = MOVETYPE_NONE;
	ent->solid = SOLID_BBOX;
	Vec3Set (ent->mins, -64, -64, 0);
	Vec3Set (ent->maxs, 64, 64, 128);
	ent->state.modelIndex = ModelIndex ("models/objects/satellite/tris.md2");
	ent->use = misc_satellite_dish_use;
	gi.linkentity (ent);
}


/*QUAKED light_mine1 (0 1 0) (-2 -2 -12) (2 2 12)
*/
void SP_light_mine1 (edict_t *ent)
{
	ent->movetype = MOVETYPE_NONE;
	ent->solid = SOLID_BBOX;
	ent->state.modelIndex = ModelIndex ("models/objects/minelite/light1/tris.md2");
	gi.linkentity (ent);
}


/*QUAKED light_mine2 (0 1 0) (-2 -2 -12) (2 2 12)
*/
void SP_light_mine2 (edict_t *ent)
{
	ent->movetype = MOVETYPE_NONE;
	ent->solid = SOLID_BBOX;
	ent->state.modelIndex = ModelIndex ("models/objects/minelite/light2/tris.md2");
	gi.linkentity (ent);
}


/*QUAKED misc_gib_arm (1 0 0) (-8 -8 -8) (8 8 8)
Intended for use with the target_spawner
*/
void SP_misc_gib_arm (edict_t *ent)
{
	ent->state.modelIndex = gMedia.Gib_Arm;
	ent->solid = SOLID_NOT;
	ent->state.effects |= EF_GIB;
	ent->takedamage = true;
//	ent->die = gib_die;
	ent->movetype = MOVETYPE_TOSS;
	ent->svFlags |= SVF_MONSTER;
	ent->deadflag = DEAD_DEAD;
	ent->avelocity[0] = random()*200;
	ent->avelocity[1] = random()*200;
	ent->avelocity[2] = random()*200;
	ent->think = G_FreeEdict;
	ent->nextthink = level.framenum + 300;
	gi.linkentity (ent);
}

/*QUAKED misc_gib_leg (1 0 0) (-8 -8 -8) (8 8 8)
Intended for use with the target_spawner
*/
void SP_misc_gib_leg (edict_t *ent)
{
	ent->state.modelIndex = gMedia.Gib_Leg;
	ent->solid = SOLID_NOT;
	ent->state.effects |= EF_GIB;
	ent->takedamage = true;
//	ent->die = gib_die;
	ent->movetype = MOVETYPE_TOSS;
	ent->svFlags |= SVF_MONSTER;
	ent->deadflag = DEAD_DEAD;
	ent->avelocity[0] = random()*200;
	ent->avelocity[1] = random()*200;
	ent->avelocity[2] = random()*200;
	ent->think = G_FreeEdict;
	ent->nextthink = level.framenum + 300;
	gi.linkentity (ent);
}

/*QUAKED misc_gib_head (1 0 0) (-8 -8 -8) (8 8 8)
Intended for use with the target_spawner
*/
void SP_misc_gib_head (edict_t *ent)
{
	ent->state.modelIndex = gMedia.Gib_Head[0];
	ent->solid = SOLID_NOT;
	ent->state.effects |= EF_GIB;
	ent->takedamage = true;
//	ent->die = gib_die;
	ent->movetype = MOVETYPE_TOSS;
	ent->svFlags |= SVF_MONSTER;
	ent->deadflag = DEAD_DEAD;
	ent->avelocity[0] = random()*200;
	ent->avelocity[1] = random()*200;
	ent->avelocity[2] = random()*200;
	ent->think = G_FreeEdict;
	ent->nextthink = level.framenum + 300;
	gi.linkentity (ent);
}

//=================================================================================

class CMiscModel : public CMapEntity
{
public:
	CMiscModel () :
	  CBaseEntity (),
	  CMapEntity ()
	  {
	  };

	CMiscModel (int Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index)
	  {
	  };

	void Spawn ()
	{
		Free ();
	};
};

LINK_CLASSNAME_TO_CLASS ("misc_model", CMiscModel);

