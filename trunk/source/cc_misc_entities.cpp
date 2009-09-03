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

/*QUAKED misc_banner (1 .5 0) (-4 -4 -4) (4 4 4)
The origin is the bottom of the banner.
The banner is 128 tall.
*/
class CMiscBanner : public CMapEntity, public CThinkableEntity
{
public:
	CMiscBanner () :
	  CBaseEntity (),
	  CMapEntity (),
	  CThinkableEntity ()
	{
	};

	CMiscBanner (int Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CThinkableEntity (Index)
	{
	};

	bool Run ()
	{
		return CBaseEntity::Run();
	};

	void Think ()
	{
		State.SetFrame ((State.GetFrame() + 1) % 16);
		NextThink = level.framenum + FRAMETIME;
	};

	void Spawn ()
	{
		SetSolid (SOLID_NOT);
		State.SetModelIndex (ModelIndex ("models/objects/banner/tris.md2"));
		State.SetFrame (rand() % 16);
		Link ();

		NextThink = level.framenum + FRAMETIME;
	};
};

LINK_CLASSNAME_TO_CLASS ("misc_banner", CMiscBanner);

/*QUAKED misc_blackhole (1 .5 0) (-8 -8 -8) (8 8 8)
*/
class CMiscBlackhole : public CMapEntity, public CThinkableEntity, public CUsableEntity
{
public:
	CMiscBlackhole () :
	  CBaseEntity (),
	  CMapEntity (),
	  CThinkableEntity (),
	  CUsableEntity ()
	{
	};

	CMiscBlackhole (int Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CThinkableEntity (Index),
	  CUsableEntity (Index)
	{
	};

	bool Run ()
	{
		return CBaseEntity::Run();
	};

	void Think ()
	{
		State.SetFrame (State.GetFrame() + 1);
		if (State.GetFrame() >= 19)
			State.SetFrame (0);
		NextThink = level.framenum + FRAMETIME;
	};

	void Use (CBaseEntity *other, CBaseEntity *activator)
	{
		Free ();
	};

	void Spawn ()
	{
		SetSolid (SOLID_NOT);
		SetMins (vec3f(-64, -64, 0));
		SetMaxs (vec3f(64, 64, 8));
		State.SetModelIndex (ModelIndex ("models/objects/black/tris.md2"));
		State.SetRenderEffects (RF_TRANSLUCENT);
		NextThink = level.framenum + 2;
		Link ();
	};
};

LINK_CLASSNAME_TO_CLASS ("misc_blackhole", CMiscBlackhole);

/*QUAKED misc_eastertank (1 .5 0) (-32 -32 -16) (32 32 32)
*/
class CMiscEasterTank : public CMapEntity, public CThinkableEntity
{
public:
	CMiscEasterTank () :
	  CBaseEntity (),
	  CMapEntity (),
	  CThinkableEntity ()
	{
	};

	CMiscEasterTank (int Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CThinkableEntity (Index)
	{
	};

	bool Run ()
	{
		return CBaseEntity::Run();
	};

	void Think ()
	{
		State.SetFrame (State.GetFrame() + 1);
		if (State.GetFrame() >= 293)
			State.SetFrame (254);

		NextThink = level.framenum + FRAMETIME;
	};

	void Spawn ()
	{
		SetSolid (SOLID_BBOX);
		SetMins (vec3f(-32, -32, -16));
		SetMaxs (vec3f(32, 32, 32));
		State.SetModelIndex (ModelIndex ("models/monsters/tank/tris.md2"));
		State.SetFrame (254);
		NextThink = level.framenum + 2;
		Link ();
	};
};

LINK_CLASSNAME_TO_CLASS ("misc_eastertank", CMiscEasterTank);

/*QUAKED misc_easterchick (1 .5 0) (-32 -32 0) (32 32 32)
*/
class CMiscEasterChick : public CMapEntity, public CThinkableEntity
{
public:
	CMiscEasterChick () :
	  CBaseEntity (),
	  CMapEntity (),
	  CThinkableEntity ()
	{
	};

	CMiscEasterChick (int Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CThinkableEntity (Index)
	{
	};

	bool Run ()
	{
		return CBaseEntity::Run();
	};

	void Think ()
	{
		State.SetFrame (State.GetFrame() + 1);
		if (State.GetFrame() >= 247)
			State.SetFrame (208);

		NextThink = level.framenum + FRAMETIME;
	};

	void Spawn ()
	{
		SetSolid (SOLID_BBOX);
		SetMins (vec3f(-32, -32, 0));
		SetMaxs (vec3f(32, 32, 32));
		State.SetModelIndex (ModelIndex ("models/monsters/bitch/tris.md2"));
		State.SetFrame (208);
		NextThink = level.framenum + 2;
		Link ();
	};
};

LINK_CLASSNAME_TO_CLASS ("misc_easterchick", CMiscEasterChick);

/*QUAKED misc_easterchick2 (1 .5 0) (-32 -32 0) (32 32 32)
*/
class CMiscEasterChick2 : public CMapEntity, public CThinkableEntity
{
public:
	CMiscEasterChick2 () :
	  CBaseEntity (),
	  CMapEntity (),
	  CThinkableEntity ()
	{
	};

	CMiscEasterChick2 (int Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CThinkableEntity (Index)
	{
	};

	bool Run ()
	{
		return CBaseEntity::Run();
	};

	void Think ()
	{
		State.SetFrame (State.GetFrame() + 1);
		if (State.GetFrame() >= 287)
			State.SetFrame (248);

		NextThink = level.framenum + FRAMETIME;
	};

	void Spawn ()
	{
		SetSolid (SOLID_BBOX);
		SetMins (vec3f(-32, -32, 0));
		SetMaxs (vec3f(32, 32, 32));
		State.SetModelIndex (ModelIndex ("models/monsters/bitch/tris.md2"));
		State.SetFrame (248);
		NextThink = level.framenum + 2;
		Link ();
	};
};

LINK_CLASSNAME_TO_CLASS ("misc_easterchick2", CMiscEasterChick2);

/*QUAKED monster_commander_body (1 .5 0) (-32 -32 0) (32 32 48)
Not really a monster, this is the Tank Commander's decapitated body.
There should be a item_commander_head that has this as it's target.
*/
class CCommanderBody : public CMapEntity, public CThinkableEntity, public CUsableEntity, public CTossProjectile
{
public:
	bool Drop;

	CCommanderBody () :
	  CBaseEntity (),
	  CMapEntity (),
	  CThinkableEntity (),
	  CUsableEntity (),
	  CTossProjectile (),
	  Drop(true)
	{
	};

	CCommanderBody (int Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CThinkableEntity (Index),
	  CUsableEntity (Index),
	  CTossProjectile (Index),
	  Drop(true)
	{
	};

	bool Run ()
	{
		return (PhysicsType == PHYSICS_TOSS) ? CTossProjectile::Run() : CBaseEntity::Run();
	};

	void Think ()
	{
		if (!Drop)
		{
			State.SetFrame (State.GetFrame() + 1);
			if (State.GetFrame() < 24)
				NextThink = level.framenum + FRAMETIME;
			else
				NextThink = 0;

			if (State.GetFrame() == 22)
				PlaySound (CHAN_BODY, SoundIndex ("tank/thud.wav"));
		}
		else
		{
			Drop = false;
			PhysicsType = PHYSICS_TOSS;
			State.SetOrigin (State.GetOrigin() + vec3f(0,0,2));
		}
	};

	void Use (CBaseEntity *other, CBaseEntity *activator)
	{
		NextThink = level.framenum + FRAMETIME;
		PlaySound (CHAN_BODY, SoundIndex ("tank/pain.wav"));
	};

	void Spawn ()
	{
		PhysicsType = PHYSICS_NONE;
		SetSolid (SOLID_BBOX);
		State.SetModelIndex (ModelIndex ("models/monsters/commandr/tris.md2"));
		SetMins (vec3f(-32, -32, 0));
		SetMaxs (vec3f(32, 32, 48));
		gameEntity->takedamage = true;
		gameEntity->flags = FL_GODMODE;
		State.AddRenderEffects (RF_FRAMELERP);
		Link ();

		SoundIndex ("tank/thud.wav");
		SoundIndex ("tank/pain.wav");

		NextThink = level.framenum + 5;
	};
};

LINK_CLASSNAME_TO_CLASS ("monster_commander_body", CCommanderBody);

/*QUAKED misc_deadsoldier (1 .5 0) (-16 -16 0) (16 16 16) ON_BACK ON_STOMACH BACK_DECAP FETAL_POS SIT_DECAP IMPALED
This is the dead player model. Comes in 6 exciting different poses!
*/
void VelocityForDamage (int damage, vec3f &v);
class CMiscDeadSoldier : public CMapEntity, public CHurtableEntity, public CThinkableEntity, public CTossProjectile
{
public:
	CMiscDeadSoldier () :
	  CBaseEntity (),
	  CMapEntity (),
	  CHurtableEntity (),
	  CThinkableEntity (),
	  CTossProjectile ()
	{
	};

	CMiscDeadSoldier (int Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CHurtableEntity (Index),
	  CThinkableEntity (Index),
	  CTossProjectile (Index)
	{
	};

	bool Run ()
	{
		switch (PhysicsType)
		{
		case PHYSICS_NONE:
		default:
			return CBaseEntity::Run();
		case PHYSICS_TOSS:
			return CTossProjectile::Run ();
		case PHYSICS_BOUNCE:
			return CBounceProjectile::Run ();
		};
	};

	void Pain (CBaseEntity *other, float kick, int damage)
	{
	};
	void Die (CBaseEntity *inflictor, CBaseEntity *attacker, int damage, vec3_t point)
	{
		if (gameEntity->health > -80)
			return;

		PlaySound (CHAN_BODY, SoundIndex ("misc/udeath.wav"));
		for (int n = 0; n < 4; n++)
			CGibEntity::Spawn (this, gMedia.Gib_SmallMeat, damage, GIB_ORGANIC);
		TossHead (gMedia.Gib_Head[1], damage, GIB_ORGANIC);
	};

	void TossHead (MediaIndex gibIndex, int damage, int type)
	{
		float	vscale;

		State.SetSkinNum (0);
		State.SetFrame (0);

		SetMins (vec3fOrigin);
		SetMaxs (vec3fOrigin);

		State.SetModelIndex (0, 2);
		State.SetModelIndex (gibIndex);
		SetSolid (SOLID_NOT);
		State.AddEffects (EF_GIB);
		State.RemoveEffects (EF_FLIES);
		State.SetSound (0);
		gameEntity->flags |= FL_NO_KNOCKBACK;
		SetSvFlags (GetSvFlags() & ~SVF_MONSTER);
		gameEntity->takedamage = true;

		if (type == GIB_ORGANIC)
		{
			PhysicsType = PHYSICS_TOSS;
			vscale = 0.5;
			backOff = 1.0f;
		}
		else
		{
			PhysicsType = PHYSICS_BOUNCE;
			backOff = 1.5f;
			vscale = 1.0;
		}

		vec3f vd;
		VelocityForDamage (damage, vd);
		
		vec3f velocity (gameEntity->velocity);
		velocity.MultiplyAngles (vscale, vd);
		gameEntity->velocity[0] = velocity.X;
		gameEntity->velocity[1] = velocity.Y;
		gameEntity->velocity[2] = velocity.Z;

		if (gameEntity->velocity[0] < -300)
			gameEntity->velocity[0] = -300;
		else if (gameEntity->velocity[0] > 300)
			gameEntity->velocity[0] = 300;
		if (gameEntity->velocity[1] < -300)
			gameEntity->velocity[1] = -300;
		else if (gameEntity->velocity[1] > 300)
			gameEntity->velocity[1] = 300;
		if (gameEntity->velocity[2] < 200)
			gameEntity->velocity[2] = 200;	// always some upwards
		else if (gameEntity->velocity[2] > 500)
			gameEntity->velocity[2] = 500;

		gameEntity->avelocity[YAW] = crandom()*600;

		NextThink = level.framenum + 100 + random()*100;

		Link();
	};

	void Think ()
	{
		Free ();
	};

	void Spawn ()
	{
		if (game.mode & GAME_DEATHMATCH)
		{	// auto-remove for deathmatch
			Free ();
			return;
		}

		PhysicsType = PHYSICS_NONE;
		SetSolid (SOLID_BBOX);
		State.SetModelIndex (ModelIndex ("models/deadbods/dude/tris.md2"));

		// Defaults to frame 0
		if (gameEntity->spawnflags & 2)
			State.SetFrame (1);
		else if (gameEntity->spawnflags & 4)
			State.SetFrame (2);
		else if (gameEntity->spawnflags & 8)
			State.SetFrame (3);
		else if (gameEntity->spawnflags & 16)
			State.SetFrame (4);
		else if (gameEntity->spawnflags & 32)
			State.SetFrame (5);
		else
			State.SetFrame (0);

		SetMins (vec3f(-16, -16, 0));
		SetMaxs (vec3f(16, 16, 16));
		gameEntity->deadflag = DEAD_DEAD;
		gameEntity->takedamage = true;
		SetSvFlags (GetSvFlags() | (SVF_MONSTER|SVF_DEADMONSTER));

		Link ();
	};
};

LINK_CLASSNAME_TO_CLASS ("misc_deadsoldier", CMiscDeadSoldier);

/*QUAKED misc_bigviper (1 .5 0) (-176 -120 -24) (176 120 72) 
This is a large stationary viper as seen in Paul's intro
*/
class CMiscBigViper : public CMapEntity
{
public:
	CMiscBigViper () :
	  CBaseEntity (),
	  CMapEntity ()
	{
	};

	CMiscBigViper (int Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index)
	{
	};

	bool Run ()
	{
		return CBaseEntity::Run();
	};

	void Spawn ()
	{
		SetSolid (SOLID_BBOX);
		SetMins (vec3f(-176, -120, -24));
		SetMaxs (vec3f(176, 120, 72));
		State.SetModelIndex (ModelIndex ("models/ships/bigviper/tris.md2"));
		Link ();
	};
};

LINK_CLASSNAME_TO_CLASS ("misc_bigviper", CMiscBigViper);

/*QUAKED misc_viper_bomb (1 0 0) (-8 -8 -8) (8 8 8)
"dmg"	how much boom should the bomb make?
*/
class CMiscViperBomb : public CMapEntity, public CThinkableEntity, public CTouchableEntity, public CUsableEntity, public CTossProjectile
{
public:
	bool PreThinkable;
	int32 TimeStamp;
	vec3f MoveDir;
	bool Usable;
	bool Touchable;

	CMiscViperBomb () :
	  CBaseEntity (),
	  CMapEntity (),
	  CThinkableEntity (),
	  CTouchableEntity (),
	  CUsableEntity (),
	  CTossProjectile (),
	  PreThinkable (false),
	  TimeStamp (0),
	  MoveDir(),
	  Usable(true),
	  Touchable(false)
	{
	};

	CMiscViperBomb (int Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CThinkableEntity (Index),
	  CTouchableEntity (Index),
	  CUsableEntity (Index),
	  CTossProjectile (Index),
	  PreThinkable (false),
	  TimeStamp (0),
	  MoveDir(),
	  Usable(true),
	  Touchable(false)
	{
	};

	bool Run ()
	{
		return (PhysicsType == PHYSICS_TOSS) ? CTossProjectile::Run() : CBaseEntity::Run();
	};

	void Think ()
	{
	};
	void PreThink ()
	{
		gameEntity->groundentity = NULL;

		float diff = TimeStamp - level.framenum;
		if (diff < -1.0)
			diff = -1.0;

		vec3f v = MoveDir;
		v.Scale (1.0 + diff);
		v.Z = diff;

		diff = State.GetAngles().Z;

		vec3f angles = v.ToAngles();
		angles.Z = diff + 10;
		State.SetAngles (angles);
	};

	void Touch (CBaseEntity *other, plane_t *plane, cmBspSurface_t *surf)
	{
		G_UseTargets (this, gameEntity->activator->Entity);

		State.SetOrigin (vec3f(State.GetOrigin().X, State.GetOrigin().Y, GetAbsMin().Z + 1));
		T_RadiusDamage (gameEntity, gameEntity, gameEntity->dmg, NULL, gameEntity->dmg+40, MOD_BOMB);
		BecomeExplosion (true);
	};

	void Use (CBaseEntity *other, CBaseEntity *activator)
	{
		if (!Usable)
			return;

		SetSolid (SOLID_BBOX);
		SetSvFlags (GetSvFlags() & ~SVF_NOCLIENT);
		State.AddEffects (EF_ROCKET);
		Usable = false;
		PhysicsType = PHYSICS_TOSS;
		PreThinkable = true;
		Touchable = true;
		gameEntity->activator = activator->gameEntity;

		CMiscViper *viper = dynamic_cast<CMiscViper*>(CC_Find (NULL, FOFS(classname), "misc_viper"));

		vec3f vel = viper->Dir;
		vel.Scale (viper->Speed);

		Vec3Copy (vel, gameEntity->velocity);

		TimeStamp = level.framenum;
		MoveDir = viper->Dir;
	};


	void Spawn ()
	{
		PhysicsType = PHYSICS_NONE;
		SetSolid (SOLID_NOT);
		SetMins (vec3f(-8, -8, -8));
		SetMaxs (vec3f(8, 8, 8));

		State.SetModelIndex (ModelIndex ("models/objects/bomb/tris.md2"));

		if (!gameEntity->dmg)
			gameEntity->dmg = 1000;

		SetSvFlags (GetSvFlags() | SVF_NOCLIENT);
		Link ();
	};
};

LINK_CLASSNAME_TO_CLASS ("misc_viper_bomb", CMiscViperBomb);

/*QUAKED misc_satellite_dish (1 .5 0) (-64 -64 0) (64 64 128)
*/
class CMiscSattelite : public CMapEntity, public CThinkableEntity, public CUsableEntity
{
public:
	CMiscSattelite () :
	  CBaseEntity (),
	  CMapEntity (),
	  CThinkableEntity (),
	  CUsableEntity ()
	{
	};

	CMiscSattelite (int Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CThinkableEntity (Index),
	  CUsableEntity (Index)
	{
	};

	bool Run ()
	{
		return CBaseEntity::Run();
	};

	void Think ()
	{
		State.SetFrame (State.GetFrame() + 1);
		if (State.GetFrame() < 38)
			NextThink = level.framenum + FRAMETIME;
	};

	void Use (CBaseEntity *other, CBaseEntity *activator)
	{
		NextThink = level.framenum + FRAMETIME;
	};

	void Spawn ()
	{
		SetSolid (SOLID_BBOX);
		SetMins (vec3f(-64, -64, 0));
		SetMaxs (vec3f(64, 64, 128));
		State.SetModelIndex (ModelIndex ("models/objects/satellite/tris.md2"));
		Link ();
	};
};

LINK_CLASSNAME_TO_CLASS ("misc_satellite_dish", CMiscSattelite);

/*QUAKED light_mine1 (0 1 0) (-2 -2 -12) (2 2 12)
*/
class CLightMine1 : public CMapEntity
{
public:
	CLightMine1 () :
	  CBaseEntity (),
	  CMapEntity ()
	{
	};

	CLightMine1 (int Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index)
	{
	};

	bool Run ()
	{
		return CBaseEntity::Run();
	};

	void Spawn ()
	{
		SetSolid (SOLID_BBOX);
		State.SetModelIndex (ModelIndex ("models/objects/minelite/light1/tris.md2"));
		Link ();
	};
};

LINK_CLASSNAME_TO_CLASS ("light_mine1", CLightMine1);

/*QUAKED light_mine2 (0 1 0) (-2 -2 -12) (2 2 12)
*/
class CLightMine2 : public CMapEntity
{
public:
	CLightMine2 () :
	  CBaseEntity (),
	  CMapEntity ()
	{
	};

	CLightMine2 (int Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index)
	{
	};

	bool Run ()
	{
		return CBaseEntity::Run();
	};

	void Spawn ()
	{
		SetSolid (SOLID_BBOX);
		State.SetModelIndex (ModelIndex ("models/objects/minelite/light2/tris.md2"));
		Link ();
	};
};

LINK_CLASSNAME_TO_CLASS ("light_mine2", CLightMine2);

/*QUAKED misc_gib_arm (1 0 0) (-8 -8 -8) (8 8 8)
Intended for use with the target_spawner
*/
class CMiscGibArm : public CMapEntity, public CThinkableEntity, public CTossProjectile
{
public:
	CMiscGibArm () :
	  CBaseEntity (),
	  CMapEntity (),
	  CThinkableEntity (),
	  CTossProjectile ()
	{
	};

	CMiscGibArm (int Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CThinkableEntity (Index),
	  CTossProjectile (Index)
	{
	};

	bool Run ()
	{
		return CTossProjectile::Run();
	};

	void Think ()
	{
		Free ();
	};

	void Spawn ()
	{
		State.SetModelIndex (gMedia.Gib_Arm);
		SetSolid (SOLID_NOT);
		State.AddEffects (EF_GIB);
		gameEntity->takedamage = true;
		PhysicsType = PHYSICS_TOSS;
		SetSvFlags (GetSvFlags() | SVF_MONSTER);
		gameEntity->deadflag = DEAD_DEAD;
		gameEntity->avelocity[0] = random()*200;
		gameEntity->avelocity[1] = random()*200;
		gameEntity->avelocity[2] = random()*200;
		NextThink = level.framenum + 300;
		Link ();
	};
};

LINK_CLASSNAME_TO_CLASS ("misc_gib_arm", CMiscGibArm);

/*QUAKED misc_gib_leg (1 0 0) (-8 -8 -8) (8 8 8)
Intended for use with the target_spawner
*/
class CMiscGibLeg : public CMapEntity, public CThinkableEntity, public CTossProjectile
{
public:
	CMiscGibLeg () :
	  CBaseEntity (),
	  CMapEntity (),
	  CThinkableEntity (),
	  CTossProjectile ()
	{
	};

	CMiscGibLeg (int Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CThinkableEntity (Index),
	  CTossProjectile (Index)
	{
	};

	bool Run ()
	{
		return CTossProjectile::Run();
	};

	void Think ()
	{
		Free ();
	};

	void Spawn ()
	{
		State.SetModelIndex (gMedia.Gib_Leg);
		SetSolid (SOLID_NOT);
		State.AddEffects (EF_GIB);
		gameEntity->takedamage = true;
		PhysicsType = PHYSICS_TOSS;
		SetSvFlags (GetSvFlags() | SVF_MONSTER);
		gameEntity->deadflag = DEAD_DEAD;
		gameEntity->avelocity[0] = random()*200;
		gameEntity->avelocity[1] = random()*200;
		gameEntity->avelocity[2] = random()*200;
		NextThink = level.framenum + 300;
		Link ();
	};
};

LINK_CLASSNAME_TO_CLASS ("misc_gib_leg", CMiscGibLeg);

/*QUAKED misc_gib_head (1 0 0) (-8 -8 -8) (8 8 8)
Intended for use with the target_spawner
*/
class CMiscGibHead : public CMapEntity, public CThinkableEntity, public CTossProjectile
{
public:
	CMiscGibHead () :
	  CBaseEntity (),
	  CMapEntity (),
	  CThinkableEntity (),
	  CTossProjectile ()
	{
	};

	CMiscGibHead (int Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CThinkableEntity (Index),
	  CTossProjectile (Index)
	{
	};

	bool Run ()
	{
		return CTossProjectile::Run();
	};

	void Think ()
	{
		Free ();
	};

	void Spawn ()
	{
		State.SetModelIndex (gMedia.Gib_Head[0]);
		SetSolid (SOLID_NOT);
		State.AddEffects (EF_GIB);
		gameEntity->takedamage = true;
		PhysicsType = PHYSICS_TOSS;
		SetSvFlags (GetSvFlags() | SVF_MONSTER);
		gameEntity->deadflag = DEAD_DEAD;
		gameEntity->avelocity[0] = random()*200;
		gameEntity->avelocity[1] = random()*200;
		gameEntity->avelocity[2] = random()*200;
		NextThink = level.framenum + 300;
		Link ();
	};
};

LINK_CLASSNAME_TO_CLASS ("misc_gib_head", CMiscGibHead);

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

