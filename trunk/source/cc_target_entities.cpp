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
// cc_target_entities.cpp
// 
//

#include "cc_local.h"
#include "cc_target_entities.h"

/*QUAKED target_speaker (1 0 0) (-8 -8 -8) (8 8 8) looped-on looped-off reliable
"noise"		wav file to play
"attenuation"
-1 = none, send to whole level
1 = normal fighting sounds
2 = idle sound level
3 = ambient sound level
"volume"	0.0 to 1.0

Normal sounds play each time the target is used.  The reliable flag can be set for crucial voiceovers.

Looped sounds are always atten 3 / vol 1, and the use function toggles it on/off.
Multiple identical looping sounds will just increase volume without any speed cost.
*/

class CTargetSpeaker : public CMapEntity, public CUsableEntity
{
public:
	CTargetSpeaker () :
	  CBaseEntity (),
	  CMapEntity (),
	  CUsableEntity ()
	  {
	  };

	CTargetSpeaker (int Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CUsableEntity (Index)
	  {
	  };

	void Use (CBaseEntity *other, CBaseEntity *activator)
	{
		if (SpawnFlags & 3) // looping sound toggles
			State.SetSound (State.GetSound() ? 0 : gameEntity->noise_index); // start or stop it
		else
			// use a positioned_sound, because this entity won't normally be
			// sent to any clients because it is invisible
			PlayPositionedSound (State.GetOrigin(), (SpawnFlags & 4) ? CHAN_VOICE|CHAN_RELIABLE : CHAN_VOICE, gameEntity->noise_index, gameEntity->volume, gameEntity->attenuation);
	};

	void Spawn ()
	{
		if(!st.noise)
		{
			//gi.dprintf("target_speaker with no noise set at (%f %f %f)\n", ent->state.origin[0], ent->state.origin[1], ent->state.origin[2]);
			MapPrint (MAPPRINT_ERROR, this, State.GetOrigin(), "No noise set\n");
			return;
		}

		char	buffer[MAX_QPATH];
		if (!strstr (st.noise, ".wav"))
			Q_snprintfz (buffer, sizeof(buffer), "%s.wav", st.noise);
		else
			Q_strncpyz (buffer, st.noise, sizeof(buffer));
		gameEntity->noise_index = SoundIndex (buffer);

		if (!gameEntity->volume)
			gameEntity->volume = 1.0;

		if (!gameEntity->attenuation)
			gameEntity->attenuation = 1.0;
		else if (gameEntity->attenuation == -1)	// use -1 so 0 defaults to 1
			gameEntity->attenuation = 0;

		// check for prestarted looping sound
		if (SpawnFlags & 1)
			State.SetSound (gameEntity->noise_index);

		// must link the entity so we get areas and clusters so
		// the server can determine who to send updates to
		Link ();
	};
};

LINK_CLASSNAME_TO_CLASS ("target_speaker", CTargetSpeaker);

class CTargetExplosion : public CMapEntity, public CThinkableEntity, public CUsableEntity
{
public:
	char	*Message;

	CTargetExplosion () :
	  CBaseEntity (),
	  CMapEntity (),
	  CThinkableEntity (),
	  CUsableEntity ()
	{
	};

	CTargetExplosion (int Index) :
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
		vec3f or = State.GetOrigin();
		CTempEnt_Explosions::RocketExplosion (or, this);

		T_RadiusDamage (this, gameEntity->activator->Entity, gameEntity->dmg, NULL, gameEntity->dmg+40, MOD_EXPLOSIVE);

		float save = gameEntity->delay;
		gameEntity->delay = 0;
		UseTargets (gameEntity->activator->Entity, Message);
		gameEntity->delay = save;
	};

	void Use (CBaseEntity *other, CBaseEntity *activator)
	{
		gameEntity->activator = activator->gameEntity;

		if (!gameEntity->delay)
		{
			Think ();
			return;
		}

		// Backwards compatibility
		NextThink = level.framenum + (gameEntity->delay * 10);
	};

	void Spawn ()
	{
		SetSvFlags (SVF_NOCLIENT);

		if (st.message)
			Message = Mem_PoolStrDup (st.message, com_levelPool, 0);
	};
};

LINK_CLASSNAME_TO_CLASS ("target_explosion", CTargetExplosion);

/*QUAKED target_spawner (1 0 0) (-8 -8 -8) (8 8 8)
Set target to the type of entity you want spawned.
Useful for spawning monsters and gibs in the factory levels.

For monsters:
	Set direction to the facing you want it to have.

For gibs:
	Set direction if you want it moving and
	speed how fast it should be moving otherwise it
	will just be dropped
*/
void ED_CallSpawn (edict_t *ent);
class CTargetSpawner : public CMapEntity, public CUsableEntity
{
public:
	vec3f	MoveDir;

	CTargetSpawner () :
	  CBaseEntity (),
	  CMapEntity (),
	  CUsableEntity ()
	{
	};

	CTargetSpawner (int Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CUsableEntity (Index)
	{
	};

	bool Run ()
	{
		return CBaseEntity::Run();
	};

	void Use (CBaseEntity *other, CBaseEntity *activator)
	{
		edict_t *ent = G_Spawn();
		ent->classname = gameEntity->target;
		Vec3Copy (State.GetOrigin(), ent->state.origin);
		Vec3Copy (State.GetAngles(), ent->state.angles);
		ED_CallSpawn (ent);

		CBaseEntity *Entity = ent->Entity;
		Entity->Unlink ();
		KillBox (Entity);
		Entity->Link ();

		if (gameEntity->speed && (Entity->EntityFlags & ENT_PHYSICS))
			dynamic_cast<CPhysicsEntity*>(Entity)->Velocity = MoveDir;
	};

	void Spawn ()
	{
		SetSvFlags (SVF_NOCLIENT);
		if (gameEntity->speed)
		{
			vec3f angles = State.GetAngles();
			G_SetMovedir (angles, MoveDir);
			MoveDir *= gameEntity->speed;
			State.SetAngles (angles);
		}
	};
};

LINK_CLASSNAME_TO_CLASS ("target_spawner", CTargetSpawner);

/*QUAKED target_splash (1 0 0) (-8 -8 -8) (8 8 8)
Creates a particle splash effect when used.

Set "sounds" to one of the following:
  1) sparks
  2) blue water
  3) brown water
  4) slime
  5) lava
  6) blood

"count"	how many pixels in the splash
"dmg"	if set, does a radius damage at this location when it splashes
		useful for lava/sparks
*/
class CTargetSplash : public CMapEntity, public CUsableEntity
{
public:
	vec3f MoveDir;

	CTargetSplash () :
	  CBaseEntity (),
	  CMapEntity (),
	  CUsableEntity ()
	{
	};

	CTargetSplash (int Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CUsableEntity (Index)
	{
	};

	bool Run ()
	{
		return CBaseEntity::Run();
	};

	void Use (CBaseEntity *other, CBaseEntity *activator)
	{
		CTempEnt_Splashes::Splash (State.GetOrigin(), MoveDir, (CTempEnt_Splashes::ESplashType)gameEntity->sounds, gameEntity->count);

		if (gameEntity->dmg)
			T_RadiusDamage (this, activator, gameEntity->dmg, NULL, gameEntity->dmg+40, MOD_SPLASH);
	};

	void Spawn ()
	{
		vec3f angles = State.GetAngles();
		G_SetMovedir (angles, MoveDir);
		State.SetAngles (angles);

		if (!gameEntity->count)
			gameEntity->count = 32;

		SetSvFlags (SVF_NOCLIENT);
	};
};

LINK_CLASSNAME_TO_CLASS ("target_splash", CTargetSplash);

/*QUAKED target_temp_entity (1 0 0) (-8 -8 -8) (8 8 8)
Fire an origin based temp entity event to the clients.
"style"		type byte
*/
class CTargetTempEntity : public CMapEntity, public CUsableEntity
{
public:
	CTargetTempEntity () :
	  CBaseEntity (),
	  CMapEntity (),
	  CUsableEntity ()
	{
	};

	CTargetTempEntity (int Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CUsableEntity (Index)
	{
	};

	bool Run ()
	{
		return CBaseEntity::Run();
	};

	void Use (CBaseEntity *other, CBaseEntity *activator)
	{
		WriteByte (SVC_TEMP_ENTITY);
		WriteByte (gameEntity->style);
		WritePosition (State.GetOrigin());
		Cast (CASTFLAG_PVS, State.GetOrigin());
	};

	void Spawn ()
	{
	};
};

LINK_CLASSNAME_TO_CLASS ("target_temp_entity", CTargetTempEntity);

/*QUAKED target_changelevel (1 0 0) (-8 -8 -8) (8 8 8)
Changes level to "map" when fired
*/
CTargetChangeLevel::CTargetChangeLevel () :
	CBaseEntity (),
	CMapEntity (),
	CUsableEntity ()
{
};

CTargetChangeLevel::CTargetChangeLevel (int Index) :
	CBaseEntity (Index),
	CMapEntity (Index),
	CUsableEntity (Index)
{
};

bool CTargetChangeLevel::Run ()
{
	return CBaseEntity::Run ();
};

void CTargetChangeLevel::Use (CBaseEntity *other, CBaseEntity *activator)
{
	if (level.intermissiontime)
		return;		// already activated

	if (game.mode == GAME_SINGLEPLAYER)
	{
		if (dynamic_cast<CPlayerEntity*>(g_edicts[1].Entity)->Health <= 0)
			return;
	}

	// if noexit, do a ton of damage to other
	if ((game.mode & GAME_DEATHMATCH) && !dmFlags.dfAllowExit && (other != world->Entity))
	{
		if ((other->EntityFlags & ENT_HURTABLE))
		{
			CHurtableEntity *Other = dynamic_cast<CHurtableEntity*>(other);

			if (Other->CanTakeDamage)
				Other->TakeDamage (this, this, vec3fOrigin, Other->State.GetOrigin(), vec3fOrigin, 10 * Other->MaxHealth, 1000, 0, MOD_EXIT);
		}
		return;
	}

	// if multiplayer, let everyone know who hit the exit
	if (game.mode & GAME_DEATHMATCH)
	{
		if (activator && (activator->EntityFlags & ENT_PLAYER))
		{
			CPlayerEntity *Player = dynamic_cast<CPlayerEntity*>(activator);
			BroadcastPrintf (PRINT_HIGH, "%s exited the level.\n", Player->Client.pers.netname);
		}
	}

	// if going to a new unit, clear cross triggers
	if (strstr(gameEntity->map, "*"))	
		game.serverflags &= ~(SFL_CROSS_TRIGGER_MASK);

	BeginIntermission (this);
};

void CTargetChangeLevel::Spawn ()
{
	if (!gameEntity->map)
	{
		//gi.dprintf("target_changelevel with no map at (%f %f %f)\n", ent->state.origin[0], ent->state.origin[1], ent->state.origin[2]);
		MapPrint (MAPPRINT_ERROR, this, State.GetOrigin(), "No map\n");
		Free ();
		return;
	}

	// ugly hack because *SOMEBODY* screwed up their map
   if((Q_stricmp(level.mapname, "fact1") == 0) && (Q_stricmp(gameEntity->map, "fact3") == 0))
	   gameEntity->map = "fact3$secret1";

	SetSvFlags (SVF_NOCLIENT);
};

LINK_CLASSNAME_TO_CLASS ("target_changelevel", CTargetChangeLevel);

/*QUAKED target_crosslevel_trigger (.5 .5 .5) (-8 -8 -8) (8 8 8) trigger1 trigger2 trigger3 trigger4 trigger5 trigger6 trigger7 trigger8
Once this trigger is touched/used, any trigger_crosslevel_target with the same trigger number is automatically used when a level is started within the same unit.  It is OK to check multiple triggers.  Message, delay, target, and killtarget also work.
*/
class CCTargetCrossLevelTrigger : public CMapEntity, public CUsableEntity
{
public:
	CCTargetCrossLevelTrigger () :
	  CBaseEntity (),
	  CMapEntity (),
	  CUsableEntity ()
	{
	};

	CCTargetCrossLevelTrigger (int Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CUsableEntity (Index)
	{
	};

	bool Run ()
	{
		return CBaseEntity::Run();
	};

	void Use (CBaseEntity *other, CBaseEntity *activator)
	{
		game.serverflags |= SpawnFlags;
		Free ();
	};

	void Spawn ()
	{
		SetSvFlags (SVF_NOCLIENT);
	};
};

LINK_CLASSNAME_TO_CLASS ("target_crosslevel_trigger", CCTargetCrossLevelTrigger);

/*QUAKED target_crosslevel_target (.5 .5 .5) (-8 -8 -8) (8 8 8) trigger1 trigger2 trigger3 trigger4 trigger5 trigger6 trigger7 trigger8
Triggered by a trigger_crosslevel elsewhere within a unit.  If multiple triggers are checked, all must be true.  Delay, target and
killtarget also work.

"delay"		delay before using targets if the trigger has been activated (default 1)
*/
class CTargetCrossLevelTarget : public CMapEntity, public CThinkableEntity, public CUsableEntity
{
public:
	char	*Message;

	CTargetCrossLevelTarget () :
	  CBaseEntity (),
	  CMapEntity (),
	  CThinkableEntity ()
	{
	};

	CTargetCrossLevelTarget (int Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CThinkableEntity (Index)
	{
	};

	void Use (CBaseEntity *, CBaseEntity *)
	{
	};

	bool Run ()
	{
		return CBaseEntity::Run();
	};

	void Think ()
	{
		if (SpawnFlags == (game.serverflags & SFL_CROSS_TRIGGER_MASK & SpawnFlags))
		{
			UseTargets (this, Message);
			Free ();
		}
	};

	void Spawn ()
	{
		if (!gameEntity->delay)
			gameEntity->delay = 1;
		SetSvFlags (SVF_NOCLIENT);
		
		// Paril: backwards compatibility
		NextThink = level.framenum + (gameEntity->delay * 10);

		if (st.message)
			Message = Mem_PoolStrDup (st.message, com_levelPool, 0);
	};
};

LINK_CLASSNAME_TO_CLASS ("target_crosslevel_target", CTargetCrossLevelTarget);

/*QUAKED target_secret (1 0 1) (-8 -8 -8) (8 8 8)
Counts a secret found.
These are single use targets.
*/
class CTargetSecret : public CMapEntity, public CUsableEntity
{
public:
	char	*Message;

	CTargetSecret () :
	  CBaseEntity (),
	  CMapEntity (),
	  CUsableEntity ()
	{
	};

	CTargetSecret (int Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CUsableEntity (Index)
	{
	};

	bool Run ()
	{
		return CBaseEntity::Run();
	};

	void Use (CBaseEntity *other, CBaseEntity *activator)
	{
		PlaySound (CHAN_VOICE, gameEntity->noise_index);

		level.found_secrets++;

		UseTargets (activator, Message);
		Free ();
	};

	void Spawn ()
	{
		if (game.mode & GAME_DEATHMATCH)
		{	// auto-remove for deathmatch
			Free ();
			return;
		}

		if (!st.noise)
			st.noise = "misc/secret.wav";
		gameEntity->noise_index = SoundIndex (st.noise);
		SetSvFlags (SVF_NOCLIENT);
		level.total_secrets++;
		// map bug hack

		if (!Q_stricmp(level.mapname, "mine3") && (State.GetOrigin().X == 280 && State.GetOrigin().Y == -2048 && State.GetOrigin().Z == -624))
			Message = "You have found a secret area.";
		else if (st.message)
			Message = Mem_PoolStrDup (st.message, com_levelPool, 0);
	};
};

LINK_CLASSNAME_TO_CLASS ("target_secret", CTargetSecret);

/*QUAKED target_goal (1 0 1) (-8 -8 -8) (8 8 8)
Counts a goal completed.
These are single use targets.
*/
class CTargetGoal : public CMapEntity, public CUsableEntity
{
public:
	char	*Message;

	CTargetGoal () :
	  CBaseEntity (),
	  CMapEntity (),
	  CUsableEntity ()
	{
	};

	CTargetGoal (int Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CUsableEntity (Index)
	{
	};

	bool Run ()
	{
		return CBaseEntity::Run();
	};

	void Use (CBaseEntity *other, CBaseEntity *activator)
	{
		PlaySound (CHAN_VOICE, gameEntity->noise_index);

		level.found_goals++;

		if (level.found_goals == level.total_goals)
			ConfigString (CS_CDTRACK, "0");

		UseTargets (activator, Message);
		Free ();
	};

	void Spawn ()
	{
		if (game.mode & GAME_DEATHMATCH)
		{	// auto-remove for deathmatch
			Free ();
			return;
		}

		if (!st.noise)
			st.noise = "misc/secret.wav";
		gameEntity->noise_index = SoundIndex (st.noise);
		SetSvFlags (SVF_NOCLIENT);
		level.total_goals++;

		if (st.message)
			Message = Mem_PoolStrDup (st.message, com_levelPool, 0);
	};
};

LINK_CLASSNAME_TO_CLASS ("target_goal", CTargetGoal);

/*QUAKED target_blaster (1 0 0) (-8 -8 -8) (8 8 8) NOTRAIL NOEFFECTS
Fires a blaster bolt in the set direction when triggered.

dmg		default is 15
speed	default is 1000
*/
class CTargetBlaster : public CMapEntity, public CUsableEntity
{
public:
	vec3f MoveDir;

	CTargetBlaster () :
	  CBaseEntity (),
	  CMapEntity (),
	  CUsableEntity ()
	{
	};

	CTargetBlaster (int Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CUsableEntity (Index)
	{
	};

	bool Run ()
	{
		return CBaseEntity::Run();
	};

	void Use (CBaseEntity *other, CBaseEntity *activator)
	{
		CBlasterProjectile::Spawn (this, State.GetOrigin(), MoveDir, gameEntity->dmg, gameEntity->speed, (SpawnFlags & 2) ? 0 : ((SpawnFlags & 1) ? EF_HYPERBLASTER : EF_BLASTER), true);
		PlaySound (CHAN_VOICE, gameEntity->noise_index);
	};

	void Spawn ()
	{
		vec3f ang = State.GetAngles();
		G_SetMovedir (ang, MoveDir);
		State.SetAngles (ang);

		gameEntity->noise_index = SoundIndex ("weapons/laser2.wav");

		if (!gameEntity->dmg)
			gameEntity->dmg = 15;
		if (!gameEntity->speed)
			gameEntity->speed = 1000;

		SetSvFlags (SVF_NOCLIENT);
	};
};

LINK_CLASSNAME_TO_CLASS ("target_blaster", CTargetBlaster);

#define	START_ON		1
#define	RED				2
#define	GREEN			4
#define	BLUE			8
#define	YELLOW			16
#define	ORANGE			32
#define	FAT				64

/*QUAKED target_laser (0 .5 .8) (-8 -8 -8) (8 8 8) START_ON RED GREEN BLUE YELLOW ORANGE FAT
When triggered, fires a laser.  You can either set a target
or a direction.
*/
class CTargetLaser : public CMapEntity, public CThinkableEntity, public CUsableEntity
{
public:
	bool StartLaser;
	bool Usable;
	vec3f MoveDir;

	CTargetLaser () :
	  CBaseEntity (),
	  CMapEntity (),
	  CThinkableEntity (),
	  CUsableEntity (),
	  StartLaser(true),
	  Usable(false)
	{
	};

	CTargetLaser (int Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CThinkableEntity (Index),
	  CUsableEntity (Index),
	  StartLaser(true),
	  Usable(false)
	{
	};

	bool Run ()
	{
		return CBaseEntity::Run();
	};

	void Think ()
	{
		if (StartLaser)
		{
			Start ();
			return;
		}

		edict_t	*ignore;
		vec3f	start;
		vec3f	end;

		if (gameEntity->enemy)
		{
			vec3f last_movedir = MoveDir;
			vec3f point = gameEntity->enemy->Entity->GetAbsMin().MultiplyAngles (0.5f, gameEntity->enemy->Entity->GetSize());

			MoveDir = point - State.GetOrigin();
			MoveDir.Normalize ();
			if (MoveDir != last_movedir)
				SpawnFlags |= 0x80000000;
		}

		ignore = gameEntity;
		start = State.GetOrigin();
		end = start.MultiplyAngles (2048, MoveDir);
		CTrace tr;
		while(1)
		{
			tr = CTrace (start, end, ignore, CONTENTS_SOLID|CONTENTS_MONSTER|CONTENTS_DEADMONSTER);

			if (!tr.ent)
				break;
			if (!tr.ent->Entity)
				break;

			CBaseEntity *Entity = tr.ent->Entity;
			// hurt it if we can
			if (((Entity->EntityFlags & ENT_HURTABLE) && dynamic_cast<CHurtableEntity*>(Entity)->CanTakeDamage) && !(Entity->Flags & FL_IMMUNE_LASER))
				dynamic_cast<CHurtableEntity*>(Entity)->TakeDamage (this, (gameEntity->activator) ? gameEntity->activator->Entity : NULL, MoveDir, tr.EndPos, vec3fOrigin, gameEntity->dmg, 1, DAMAGE_ENERGY, MOD_TARGET_LASER);

			// if we hit something that's not a monster or player or is immune to lasers, we're done
			if (!(Entity->EntityFlags & ENT_MONSTER) && (!(Entity->EntityFlags & ENT_PLAYER)))
			{
				if (SpawnFlags & 0x80000000)
				{
					SpawnFlags &= ~0x80000000;
					CTempEnt_Splashes::Sparks (tr.EndPos,
						tr.Plane.normal, 
						CTempEnt_Splashes::STLaserSparks,
						(CTempEnt_Splashes::ESplashType)(State.GetSkinNum() & 255),
						(SpawnFlags & 0x80000000) ? 8 : 4);
				}
				break;
			}

			ignore = tr.ent;
			start = tr.EndPos;
		}

		State.SetOldOrigin (tr.EndPos);
		NextThink = level.framenum + FRAMETIME;
	};

	void Use (CBaseEntity *other, CBaseEntity *activator)
	{
		if (!Usable)
			return;

		gameEntity->activator = activator->gameEntity;
		if (SpawnFlags & 1)
			Off ();
		else
			On ();
	};

	void On ()
	{
		if (!gameEntity->activator)
			gameEntity->activator = gameEntity;
		SpawnFlags |= 0x80000001;
		SetSvFlags (GetSvFlags() & ~SVF_NOCLIENT);
		Think ();
	};
	void Off ()
	{
		SpawnFlags &= ~1;
		SetSvFlags (GetSvFlags() | SVF_NOCLIENT);
		NextThink = 0;
	};
	void Start ()
	{
		SetSolid (SOLID_NOT);
		State.AddRenderEffects (RF_BEAM|RF_TRANSLUCENT);
		State.SetModelIndex (1);			// must be non-zero

		// set the beam diameter
		if (SpawnFlags & FAT)
			State.SetFrame (16);
		else
			State.SetFrame (4);

		// set the color
		if (SpawnFlags & RED)
			State.SetSkinNum (Color_RGBAToHex (NSColor::PatriotRed, NSColor::PatriotRed, NSColor::Red, NSColor::Red));
		else if (SpawnFlags & GREEN)
			State.SetSkinNum (Color_RGBAToHex (NSColor::Green, NSColor::Lime, NSColor::FireSpeechGreen, NSColor::Harlequin));
		else if (SpawnFlags & BLUE)
			State.SetSkinNum (Color_RGBAToHex (NSColor::PatriotBlue, NSColor::PatriotBlue, NSColor::NeonBlue, NSColor::NeonBlue));
		else if (SpawnFlags & YELLOW)
			State.SetSkinNum (Color_RGBAToHex (NSColor::ParisDaisy, NSColor::Gorse, NSColor::Lemon, NSColor::Gold));
		else if (SpawnFlags & ORANGE)
			State.SetSkinNum (Color_RGBAToHex (NSColor::HarvestGold, NSColor::RobRoy, NSColor::TulipTree, NSColor::FireBush));

		if (!gameEntity->enemy)
		{
			if (gameEntity->target)
			{
				CBaseEntity *ent = CC_Find (NULL, FOFS(targetname), gameEntity->target);
				if (!ent)
					//gi.dprintf ("%s at (%f %f %f): %s is a bad target\n", self->classname, self->state.origin[0], self->state.origin[1], self->state.origin[2], self->target);
					MapPrint (MAPPRINT_WARNING, this, State.GetOrigin(), "\"%s\" is a bad target\n", gameEntity->target);
				gameEntity->enemy = ent->gameEntity;
			}
			else
			{
				vec3f angles = State.GetAngles ();
				G_SetMovedir (angles, MoveDir);
				State.SetAngles (angles);
			}
		}

		Usable = true;
		StartLaser = false;

		if (!gameEntity->dmg)
			gameEntity->dmg = 1;

		SetMins (vec3f(-8, -8, -8));
		SetMaxs (vec3f(8, 8, 8));
		Link ();

		if (SpawnFlags & START_ON)
			On ();
		else
			Off ();
	};

	void Spawn ()
	{
		// let everything else get spawned before we start firing
		NextThink = level.framenum + 10;
	};
};

LINK_CLASSNAME_TO_CLASS ("target_laser", CTargetLaser);

/*QUAKED target_help (1 0 1) (-16 -16 -24) (16 16 24) help1
When fired, the "message" key becomes the current personal computer string, and the message light will be set on all clients status bars.
*/
class CTargeHelp : public CMapEntity, public CUsableEntity
{
public:
	char		*Message;

	CTargeHelp () :
	  CBaseEntity (),
	  CMapEntity (),
	  CUsableEntity ()
	{
	};

	CTargeHelp (int Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CUsableEntity (Index)
	{
	};

	bool Run ()
	{
		return CBaseEntity::Run();
	};

	void Use (CBaseEntity *other, CBaseEntity *activator)
	{
		Q_strncpyz ((SpawnFlags & 1) ? game.helpmessage1 : game.helpmessage2, Message, sizeof(game.helpmessage1)-1);
		game.helpchanged++;
	};

	void Spawn ()
	{
		if (game.mode & GAME_DEATHMATCH)
		{	// auto-remove for deathmatch
			Free ();
			return;
		}

		if (!st.message)
		{
			MapPrint (MAPPRINT_ERROR, this, State.GetOrigin(), "No message\n");
			Free ();
			return;
		}

		Message = Mem_PoolStrDup (st.message, com_levelPool, 0);
	};
};

LINK_CLASSNAME_TO_CLASS ("target_help", CTargeHelp);

//==========================================================

/*QUAKED target_earthquake (1 0 0) (-8 -8 -8) (8 8 8)
When triggered, this initiates a level-wide earthquake.
All players and monsters are affected.
"speed"		severity of the quake (default:200)
"count"		duration of the quake (default:5)
*/
class CTargetEarthquake : public CMapEntity, public CThinkableEntity, public CUsableEntity
{
public:
	FrameNumber_t		LastShakeTime;
	FrameNumber_t		TimeStamp;

	CTargetEarthquake () :
	  CBaseEntity (),
	  CMapEntity (),
	  CThinkableEntity (),
	  CUsableEntity (),
	  LastShakeTime (0)
	{
	};

	CTargetEarthquake (int Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CThinkableEntity (Index),
	  CUsableEntity (Index),
	  LastShakeTime (0)
	{
	};

	bool Run ()
	{
		return CBaseEntity::Run();
	};

	void Think ()
	{
		int		i;
		edict_t	*e;

		if (LastShakeTime < level.framenum)
		{
			PlayPositionedSound (State.GetOrigin(), CHAN_AUTO, gameEntity->noise_index, 1.0, ATTN_NONE);
			LastShakeTime = level.framenum + 5;
		}

		for (i=1, e=g_edicts+i; i < globals.numEdicts; i++,e++)
		{
			if (!e->inUse)
				continue;
			if (!e->Entity)
				continue;

			CBaseEntity *Entity = e->Entity;

			if (!Entity->GroundEntity)
				continue;

			if (!(Entity->EntityFlags & ENT_PLAYER))
				continue;

			CPlayerEntity *Player = dynamic_cast<CPlayerEntity*>(Entity);

			Player->GroundEntity = NULL;
			Player->Velocity.X += crandom()* 150;
			Player->Velocity.Y += crandom()* 150;
			Player->Velocity.Z = gameEntity->speed * (100.0 / Player->gameEntity->mass);
		}

		if (level.framenum < TimeStamp)
			NextThink = level.framenum + FRAMETIME;
	};

	void Use (CBaseEntity *other, CBaseEntity *activator)
	{
		// Paril, Backwards compatibility
		TimeStamp = level.framenum + (gameEntity->count * 10);
		NextThink = level.framenum + FRAMETIME;
		LastShakeTime = 0;
	};

	void Spawn ()
	{
		if (!gameEntity->targetname)
			MapPrint (MAPPRINT_ERROR, this, State.GetOrigin(), "No targetname\n");
			//gi.dprintf("untargeted %s at (%f %f %f)\n", self->classname, self->state.origin[0], self->state.origin[1], self->state.origin[2]);

		if (!gameEntity->count)
			gameEntity->count = 5;

		if (!gameEntity->speed)
			gameEntity->speed = 200;

		SetSvFlags (GetSvFlags() | SVF_NOCLIENT);

		gameEntity->noise_index = SoundIndex ("world/quake.wav");
	};
};

LINK_CLASSNAME_TO_CLASS ("target_earthquake", CTargetEarthquake);