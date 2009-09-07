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
// cc_entitytypes.cpp
// Different base entity classes
//

#include "cc_local.h"

CHurtableEntity::CHurtableEntity () :
CBaseEntity(),
CanTakeDamage(false)
{
	EntityFlags |= ENT_HURTABLE;
};

CHurtableEntity::CHurtableEntity (int Index) :
CBaseEntity(Index),
CanTakeDamage(false)
{
	EntityFlags |= ENT_HURTABLE;
};

char *ClientTeam (CPlayerEntity *ent)
{
	char		*p;
	static char	value[512];

	value[0] = 0;

	Q_strncpyz(value, Info_ValueForKey (ent->Client.pers.userinfo, "skin"), sizeof(value));
	p = strchr(value, '/');
	if (!p)
		return value;

	if (dmFlags.dfModelTeams)
	{
		*p = 0;
		return value;
	}

	// if ((int)(dmflags->value) & DF_SKINTEAMS)
	return ++p;
}

bool OnSameTeam (CPlayerEntity *ent1, CPlayerEntity *ent2)
{
	char	ent1Team [512];
	char	ent2Team [512];

	if (!(dmFlags.dfSkinTeams || dmFlags.dfModelTeams))
		return false;

	Q_strncpyz (ent1Team, ClientTeam (ent1), sizeof(ent1Team));
	Q_strncpyz (ent2Team, ClientTeam (ent2), sizeof(ent2Team));

	if (strcmp(ent1Team, ent2Team) == 0)
		return true;
	return false;
}

bool CHurtableEntity::CanDamage (CBaseEntity *inflictor)
{
// bmodels need special checking because their origin is 0,0,0
	if ((EntityFlags & ENT_PHYSICS) && ((dynamic_cast<CPhysicsEntity*>(this))->PhysicsType == PHYSICS_PUSH))
	{
		vec3f dest = GetAbsMin() + GetAbsMax();
		dest.Scale (0.5f);
		CTrace trace (inflictor->State.GetOrigin(), dest, inflictor->gameEntity, CONTENTS_MASK_SOLID);
		if (trace.fraction == 1.0 || trace.Ent == this)
			return true;
		return false;
	}
	
	static const vec3f additions[] = 
	{
		vec3f(0, 0, 0),
		vec3f(15, 15, 0),
		vec3f(15, -15, 0),
		vec3f(-15, 15, 0),
		vec3f(-15, -15, 0)
	};

	for (int i = 0; i < 5; i++)
	{
		CTrace trace (inflictor->State.GetOrigin(), State.GetOrigin() + additions[i], inflictor->gameEntity, CONTENTS_MASK_SOLID);
		if (trace.fraction == 1.0)
			return true;
	};

	return false;
}

bool CHurtableEntity::CheckTeamDamage (CBaseEntity *attacker)
{
#ifdef CLEANCTF_ENABLED
//ZOID
	if ((game.mode & GAME_CTF) && (EntityFlags & ENT_PLAYER) && (attacker->EntityFlags & ENT_PLAYER))
	{
		CPlayerEntity *Targ = dynamic_cast<CPlayerEntity*>(this);
		CPlayerEntity *Attacker = dynamic_cast<CPlayerEntity*>(attacker);
		if (Targ->Client.resp.ctf_team == Attacker->Client.resp.ctf_team &&
			(this != attacker))
			return true;
	}
//ZOID
#endif

	return false;
}

int CHurtableEntity::CheckPowerArmor (vec3f &point, vec3f &normal, int damage, int dflags)
{
	if (!damage)
		return 0;

	if (dflags & DAMAGE_NO_ARMOR)
		return 0;

	static const int	index = NItems::Cells->GetIndex();

	int			power_armor_type, damagePerCell, pa_te_type, power = 0, power_used;

	bool		IsClient = !!(EntityFlags & ENT_PLAYER),
				IsMonster = !!(EntityFlags & ENT_MONSTER);
	CPlayerEntity	*Player = (IsClient) ? dynamic_cast<CPlayerEntity*>(this) : NULL;
	CMonsterEntity	*Monster = (IsMonster) ? dynamic_cast<CMonsterEntity*>(this) : NULL;

	if (IsClient)
	{
		power_armor_type = Player->PowerArmorType ();
		if (power_armor_type != POWER_ARMOR_NONE)
			power = Player->Client.pers.Inventory.Has(index);
	}
	else if (IsMonster)
	{
		power_armor_type = Monster->Monster->PowerArmorType;
		power = Monster->Monster->PowerArmorPower;
	}
	else
		return 0;

	if (!power)
		return 0;

	switch (power_armor_type)
	{
	case POWER_ARMOR_NONE:
	default:
		return 0;
	case POWER_ARMOR_SCREEN:
		{
			vec3f		vec, forward;

			// only works if damage point is in front
			State.GetAngles().ToVectors(&forward, NULL, NULL);
			vec = point - State.GetOrigin();
			vec.Normalize ();
			if (vec.Dot (forward) <= 0.3)
				return 0;

			damagePerCell = 1;
			pa_te_type = TE_SCREEN_SPARKS;
			damage = damage / 3;
		}
		break;
	case POWER_ARMOR_SHIELD:
		damagePerCell = 2;
		pa_te_type = TE_SHIELD_SPARKS;
		damage = (2 * damage) / 3;
		break;
	};

	int save = power * damagePerCell;
	if (!save)
		return 0;
	if (save > damage)
		save = damage;

	CTempEnt_Splashes::ShieldSparks (point, normal, (pa_te_type == TE_SCREEN_SPARKS) ? true : false);
	gameEntity->powerarmor_time = level.framenum + 2;

	power_used = save / damagePerCell;
	if (!power_used)
		power_used = 1;

	if (IsClient)
		Player->Client.pers.Inventory.Remove(index, power_used);
	else if (IsMonster)
		Monster->Monster->PowerArmorPower -= power_used;
	return save;
}

void CHurtableEntity::Killed (CBaseEntity *inflictor, CBaseEntity *attacker, int damage, vec3f &point)
{
	if (gameEntity->health < -999)
		gameEntity->health = -999;

	gameEntity->enemy = attacker->gameEntity;

	if ((!DeadFlag) && (EntityFlags & ENT_MONSTER))
	{
		if (!((dynamic_cast<CMonsterEntity*>(this))->Monster->AIFlags & AI_GOOD_GUY))
		{
			level.killed_monsters++;
			if ((game.mode == GAME_COOPERATIVE) && (attacker->EntityFlags & ENT_PLAYER))
				(dynamic_cast<CPlayerEntity*>(attacker))->Client.resp.score++;
			// medics won't heal monsters that they kill themselves
			if (strcmp(attacker->gameEntity->classname, "monster_medic") == 0)
				SetOwner (attacker);
		}
	}

	if ((EntityFlags & ENT_MONSTER) && (!DeadFlag))
	{
		if (EntityFlags & ENT_TOUCHABLE)
			(dynamic_cast<CTouchableEntity*>(this))->Touchable = false;
		if (EntityFlags & ENT_MONSTER)
			(dynamic_cast<CMonsterEntity*>(this))->Monster->MonsterDeathUse();
	}

	if (((EntityFlags & ENT_HURTABLE) && dynamic_cast<CHurtableEntity*>(this)->CanTakeDamage))
		(dynamic_cast<CHurtableEntity*>(this))->Die (inflictor, attacker, damage, point);
}

void CHurtableEntity::TakeDamage (CBaseEntity *inflictor, CBaseEntity *attacker,
								vec3f dir, vec3f point, vec3f normal, int damage,
								int knockback, int dflags, EMeansOfDeath mod)
{
	int			take;
	int			save;
	int			asave = 0;
	int			psave = 0;

	// Needed?
	if (!CanTakeDamage)
		return;

	// friendly fire avoidance
	// if enabled you can't hurt teammates (but you can hurt yourself)
	// knockback still occurs
	if ((this != attacker) && ((game.mode & GAME_DEATHMATCH) && (dmFlags.dfSkinTeams || dmFlags.dfModelTeams) || game.mode == GAME_COOPERATIVE))
	{
		if ((EntityFlags & ENT_PLAYER) && (attacker->EntityFlags & ENT_PLAYER))
		{
			if (OnSameTeam (dynamic_cast<CPlayerEntity*>(this), dynamic_cast<CPlayerEntity*>(attacker)))
			{
				if (dmFlags.dfNoFriendlyFire)
					damage = 0;
				else
					mod |= MOD_FRIENDLY_FIRE;
			}
		}
	}
	meansOfDeath = mod;

	// easy mode takes half damage
	if (skill->Integer() == 0 && !(game.mode & GAME_DEATHMATCH) && (EntityFlags & ENT_PLAYER))
	{
		damage *= 0.5;
		if (!damage)
			damage = 1;
	}

	bool isClient = !!(EntityFlags & ENT_PLAYER);
	CClient *Client = (isClient) ? &(dynamic_cast<CPlayerEntity*>(this)->Client) : NULL;

	dir.Normalize ();

// bonus damage for suprising a monster
	if (!(dflags & DAMAGE_RADIUS) && (EntityFlags & ENT_MONSTER) && (attacker->EntityFlags & ENT_PLAYER) && (!gameEntity->enemy) && (gameEntity->health > 0))
		damage *= 2;

#ifdef CLEANCTF_ENABLED
//ZOID
//strength tech
	if ((game.mode & GAME_CTF) && (attacker->EntityFlags & ENT_PLAYER))
		damage = (dynamic_cast<CPlayerEntity*>(attacker))->CTFApplyStrength(damage);
//ZOID
#endif

	if (Flags & FL_NO_KNOCKBACK)
		knockback = 0;

// figure momentum add
	bool AddVelocity = true;
	if (!knockback || (dflags & DAMAGE_NO_KNOCKBACK))
		AddVelocity = false;

	if (EntityFlags & ENT_PHYSICS)
	{
		CPhysicsEntity *Phys = dynamic_cast<CPhysicsEntity*>(this);
		if (Phys->PhysicsType == PHYSICS_NONE ||
			Phys->PhysicsType == PHYSICS_BOUNCE ||
			Phys->PhysicsType == PHYSICS_PUSH ||
			Phys->PhysicsType == PHYSICS_STOP)
			AddVelocity = false;
	}

	if (AddVelocity)
	{
		vec3f	kvel = dir;
		const float	mass = Clamp<float> (gameEntity->mass, 50.0f, gameEntity->mass);

		if (isClient && (attacker == this))
			kvel.Scale (1600.0 * (float)knockback / mass); // the rocket jump hack...
		else
			kvel.Scale (500.0 * (float)knockback / mass);

		Vec3Add (gameEntity->velocity, kvel, gameEntity->velocity);
	}

	take = damage;
	save = 0;

	// check for godmode
	if ( (Flags & FL_GODMODE) && !(dflags & DAMAGE_NO_PROTECTION) )
	{
		take = 0;
		save = damage;
		CTempEnt_Splashes::Sparks (point, normal, (dflags & DAMAGE_BULLET) ? CTempEnt_Splashes::STBulletSparks : CTempEnt_Splashes::STSparks, CTempEnt_Splashes::SPTSparks);
	}

	// check for invincibility
	if ((isClient && (Client->invincible_framenum > level.framenum) ) && !(dflags & DAMAGE_NO_PROTECTION))
	{
		if (gameEntity->pain_debounce_time < level.framenum)
		{
			PlaySound (CHAN_ITEM, SoundIndex("items/protect4.wav"), 1, ATTN_NORM);
			gameEntity->pain_debounce_time = level.framenum + 20;
		}
		take = 0;
		save = damage;
	}

#ifdef CLEANCTF_ENABLED
//ZOID
//team armor protect
	if ((game.mode & GAME_CTF) && isClient && (attacker->EntityFlags & ENT_PLAYER) &&
		(Client->resp.ctf_team == (dynamic_cast<CPlayerEntity*>(attacker))->Client.resp.ctf_team) &&
		(this != attacker) && dmFlags.dfCtfArmorProtect)
		psave = asave = 0;
	else
	{
//ZOID
#endif
		psave = CheckPowerArmor (point, normal, take, dflags);
		take -= psave;

		if (isClient)
		{
			if (Client->pers.Armor)
			{
				asave = Client->pers.Armor->CheckArmor (dynamic_cast<CPlayerEntity*>(this), point, normal, take, dflags);
				take -= asave;
			}
		}
#ifdef CLEANCTF_ENABLED
	}
#endif

	//treat cheat/powerup savings the same as armor
	asave += save;

#ifdef CLEANCTF_ENABLED
//ZOID
//resistance tech
	if (isClient && (game.mode & GAME_CTF))
		take = (dynamic_cast<CPlayerEntity*>(this))->CTFApplyResistance(take);
//ZOID
#endif

	// team damage avoidance
	if (!(dflags & DAMAGE_NO_PROTECTION) && CheckTeamDamage (attacker))
		return;

#ifdef CLEANCTF_ENABLED
//ZOID
	if ((game.mode & GAME_CTF) && (isClient && (attacker->EntityFlags & ENT_PLAYER)))
		CTFCheckHurtCarrier((dynamic_cast<CPlayerEntity*>(this)), (dynamic_cast<CPlayerEntity*>(attacker)));
//ZOID
#endif

// do the damage
	if (take)
	{
		if ((EntityFlags & ENT_MONSTER) || (isClient))
			CTempEnt_Splashes::Blood (point, normal);
		else
			CTempEnt_Splashes::Sparks (point, normal, (dflags & DAMAGE_BULLET) ? CTempEnt_Splashes::STBulletSparks : CTempEnt_Splashes::STSparks, CTempEnt_Splashes::SPTSparks);

		gameEntity->health -= take;
			
		if (gameEntity->health <= 0)
		{
			if ((EntityFlags & ENT_MONSTER) || (isClient))
				Flags |= FL_NO_KNOCKBACK;
			Killed (inflictor, attacker, take, point);
			return;
		}
	}

	if ((EntityFlags & ENT_MONSTER))
	{
		CMonster *Monster = (dynamic_cast<CMonsterEntity*>(this))->Monster;
		Monster->ReactToDamage (attacker);
		if (!(Monster->AIFlags & AI_DUCKED) && take)
		{
			Pain (attacker, knockback, take);
			if (skill->Integer() == 3)
				gameEntity->pain_debounce_time = level.framenum + 50;
		}
	}
	else if (take)
		Pain (attacker, knockback, take);

	// add to the damage inflicted on a player this frame
	// the total will be turned into screen blends and view angle kicks
	// at the end of the frame
	if (isClient)
	{
		Client->damage_parmor += psave;
		Client->damage_armor += asave;
		Client->damage_blood += take;
		Client->damage_knockback += knockback;
		Client->DamageFrom = point;
	}
}
	
void CHurtableEntity::TakeDamage (CBaseEntity *targ, CBaseEntity *inflictor,
								CBaseEntity *attacker, vec3f dir, vec3f point,
								vec3f normal, int damage, int knockback,
								int dflags, EMeansOfDeath mod)
{
	if ((targ->EntityFlags & ENT_HURTABLE) && dynamic_cast<CHurtableEntity*>(targ)->CanTakeDamage)
		(dynamic_cast<CHurtableEntity*>(targ))->TakeDamage (inflictor, attacker, dir, point, normal, damage, knockback, dflags, mod);
}

CThinkableEntity::CThinkableEntity () :
CBaseEntity()
{
	EntityFlags |= ENT_THINKABLE;
};

CThinkableEntity::CThinkableEntity (int Index) :
CBaseEntity(Index)
{
	EntityFlags |= ENT_THINKABLE;
};

void CThinkableEntity::RunThink ()
{
	if (NextThink <= 0 || NextThink > level.framenum)
		return;
	
	NextThink = 0;
	Think ();
}

CTouchableEntity::CTouchableEntity () :
CBaseEntity()
{
	Touchable = true;
	EntityFlags |= ENT_TOUCHABLE;
};

CTouchableEntity::CTouchableEntity (int Index) :
CBaseEntity(Index)
{
	Touchable = true;
	EntityFlags |= ENT_TOUCHABLE;
};

void CTouchableEntity::Touch(CBaseEntity *other, plane_t *plane, cmBspSurface_t *surf)
{
}

CPhysicsEntity::CPhysicsEntity () :
CBaseEntity()
{
	EntityFlags |= ENT_PHYSICS;
	PhysicsType = PHYSICS_NONE;
};

CPhysicsEntity::CPhysicsEntity (int Index) :
CBaseEntity(Index)
{
	EntityFlags |= ENT_PHYSICS;
	PhysicsType = PHYSICS_NONE;
};

void CPhysicsEntity::AddGravity()
{
	gameEntity->velocity[2] -= gameEntity->gravity * sv_gravity->Float() * 0.1f;
}

CTrace CPhysicsEntity::PushEntity (vec3_t push)
{
	CTrace		Trace;
	int			mask;
	vec3f		Start = State.GetOrigin();
	vec3f		End = Start + vec3f(push);

	while (true)
	{
		if (gameEntity->clipMask)
			mask = gameEntity->clipMask;
		else
			mask = CONTENTS_MASK_SOLID;

		Trace = CTrace (State.GetOrigin(), GetMins(), GetMaxs(), End, gameEntity, mask);
		
		State.SetOrigin (Trace.endPos);
		Link();

		if (Trace.fraction != 1.0)
		{
			Impact (&Trace);

			// if the pushed entity went away and the pusher is still there
			if (!Trace.ent->inUse && IsInUse())
			{
				// move the pusher back and try again
				State.SetOrigin (Start);
				Link ();
				continue;
			}
		}
		break;
	}

	if (IsInUse())
		G_TouchTriggers (this);

	return Trace;
}

void CPhysicsEntity::Impact (CTrace *trace)
{
	CBaseEntity	*e2;

	if (!trace->ent->Entity)
		return;

	e2 = dynamic_cast<CBaseEntity*>(trace->ent->Entity);

	if (GetSolid() != SOLID_NOT && (EntityFlags & ENT_TOUCHABLE))
	{
		CTouchableEntity *Touched = dynamic_cast<CTouchableEntity*>(this);

		if (Touched->Touchable)
			Touched->Touch (e2, &trace->plane, trace->surface);
	}

	if ((e2->EntityFlags & ENT_TOUCHABLE) && e2->GetSolid() != SOLID_NOT)
	{
		CTouchableEntity *Touched = dynamic_cast<CTouchableEntity*>(e2);

		if (Touched->Touchable)
			Touched->Touch (this, NULL, NULL);
	}
}

CBounceProjectile::CBounceProjectile () :
backOff(1.5f),
CPhysicsEntity ()
{
	Vec3Copy (vec3Origin, gameEntity->velocity); 
	SetClipmask (CONTENTS_MASK_SHOT);
	SetSolid (SOLID_BBOX);
	SetMins(vec3Origin);
	SetMaxs(vec3Origin);

	PhysicsType = PHYSICS_BOUNCE;
}

CBounceProjectile::CBounceProjectile (int Index) :
backOff(1.5f),
CPhysicsEntity (Index)
{
	Vec3Copy (vec3Origin, gameEntity->velocity); 
	SetClipmask (CONTENTS_MASK_SHOT);
	SetSolid (SOLID_BBOX);
	SetMins(vec3Origin);
	SetMaxs(vec3Origin);

	PhysicsType = PHYSICS_BOUNCE;
}

int ClipVelocity (vec3_t in, vec3_t normal, vec3_t out, float overbounce);
bool CBounceProjectile::Run ()
{
	CTrace	trace;
	vec3_t		move;
	bool		wasinwater;
	bool		isinwater;
	vec3_t		old_origin;

	// if not a team captain, so movement will be handled elsewhere
	if (Flags & FL_TEAMSLAVE)
		return false;

	if (gameEntity->velocity[2] > 0)
		GroundEntity = NULL;

// check for the groundentity going away
	if (GroundEntity && !GroundEntity->IsInUse())
		GroundEntity = NULL;

// if onground, return without moving
	if ( GroundEntity )
		return false;

	State.GetOrigin(old_origin);

// add gravity
	AddGravity ();

// move angles
	vec3_t angles;
	State.GetAngles (angles);
	Vec3MA (angles, 0.1f, gameEntity->avelocity, angles);
	State.SetAngles (angles);

// move origin
	Vec3Scale (gameEntity->velocity, 0.1f, move);
	trace = PushEntity (move);
	if (!IsInUse())
		return false;

	if (trace.fraction < 1)
	{
		ClipVelocity (gameEntity->velocity, trace.plane.normal, gameEntity->velocity, backOff);

		// stop if on ground
		if (trace.plane.normal[2] > 0.9)
		{		
			if (gameEntity->velocity[2] < 60)
			{
				GroundEntity = trace.Ent;
				GroundEntityLinkCount = GroundEntity->GetLinkCount();
				Vec3Copy (vec3Origin, gameEntity->velocity);
				Vec3Copy (vec3Origin, gameEntity->avelocity);
			}
		}
	}
	
// check for water transition
	vec3_t or;
	State.GetOrigin (or);

	wasinwater = (gameEntity->watertype & CONTENTS_MASK_WATER) ? true : false;
	gameEntity->watertype = PointContents (or);
	isinwater = (gameEntity->watertype & CONTENTS_MASK_WATER) ? true : false;

	if (isinwater)
		gameEntity->waterlevel = 1;
	else
		gameEntity->waterlevel = 0;

	if (!wasinwater && isinwater)
		PlaySoundAt (old_origin, g_edicts, CHAN_AUTO, SoundIndex("misc/h2ohit1.wav"));
	else if (wasinwater && !isinwater)
		PlaySoundAt (or, g_edicts, CHAN_AUTO, SoundIndex("misc/h2ohit1.wav"));

// move teamslaves
	for (CBaseEntity *slave = TeamChain; slave; slave = slave->TeamChain)
	{
		slave->State.SetOrigin (State.GetOrigin());
		slave->Link ();
	}

	return true;
}

CTossProjectile::CTossProjectile () :
CBounceProjectile ()
{
	backOff = 1.0f;

	PhysicsType = PHYSICS_TOSS;
}

CTossProjectile::CTossProjectile (int Index) :
CBounceProjectile (Index)
{
	backOff = 1.0f;

	PhysicsType = PHYSICS_TOSS;
}

CFlyMissileProjectile::CFlyMissileProjectile () :
CPhysicsEntity ()
{
	Vec3Copy (vec3Origin, gameEntity->velocity); 
	SetClipmask (CONTENTS_MASK_SHOT);
	SetSolid (SOLID_BBOX);
	SetMins(vec3Origin);
	SetMaxs(vec3Origin);

	PhysicsType = PHYSICS_FLYMISSILE;
}

CFlyMissileProjectile::CFlyMissileProjectile (int Index) :
CPhysicsEntity (Index)
{
	Vec3Copy (vec3Origin, gameEntity->velocity); 
	SetClipmask (CONTENTS_MASK_SHOT);
	SetSolid (SOLID_BBOX);
	SetMins(vec3Origin);
	SetMaxs(vec3Origin);

	PhysicsType = PHYSICS_FLYMISSILE;
}

bool CFlyMissileProjectile::Run ()
{
	CTrace	trace;
	vec3_t		move;
	bool		wasinwater;
	bool		isinwater;
	vec3_t		old_origin;

	// if not a team captain, so movement will be handled elsewhere
	if (Flags & FL_TEAMSLAVE)
		return false;

	if (gameEntity->velocity[2] > 0)
		GroundEntity = NULL;

// check for the groundentity going away
	if (GroundEntity && !GroundEntity->IsInUse())
		GroundEntity = NULL;

// if onground, return without moving
	if ( GroundEntity )
		return false;

	State.GetOrigin(old_origin);

// move angles
	vec3_t angles;
	State.GetAngles (angles);
	Vec3MA (angles, 0.1f, gameEntity->avelocity, angles);
	State.SetAngles (angles);

// move origin
	Vec3Scale (gameEntity->velocity, 0.1f, move);
	trace = PushEntity (move);
	if (!IsInUse())
		return false;

	if (trace.fraction < 1)
	{
		ClipVelocity (gameEntity->velocity, trace.plane.normal, gameEntity->velocity, 1.0);

		// stop if on ground
		if (trace.plane.normal[2] > 0.9)
		{		
			GroundEntity = trace.Ent;
			GroundEntityLinkCount = GroundEntity->GetLinkCount();
			Vec3Copy (vec3Origin, gameEntity->velocity);
			Vec3Copy (vec3Origin, gameEntity->avelocity);
		}
	}
	
// check for water transition
	vec3_t or;
	State.GetOrigin (or);

	wasinwater = (gameEntity->watertype & CONTENTS_MASK_WATER) ? true : false;
	gameEntity->watertype = PointContents (or);
	isinwater = (gameEntity->watertype & CONTENTS_MASK_WATER) ? true : false;

	if (isinwater)
		gameEntity->waterlevel = 1;
	else
		gameEntity->waterlevel = 0;

	if (!wasinwater && isinwater)
		PlaySoundAt (old_origin, g_edicts, CHAN_AUTO, SoundIndex("misc/h2ohit1.wav"));
	else if (wasinwater && !isinwater)
		PlaySoundAt (or, g_edicts, CHAN_AUTO, SoundIndex("misc/h2ohit1.wav"));

// move teamslaves
	for (CBaseEntity *slave = TeamChain; slave; slave = slave->TeamChain)
	{
		slave->State.SetOrigin (State.GetOrigin());
		slave->Link ();
	}

	return true;
}

CStepPhysics::CStepPhysics () :
CPhysicsEntity ()
{
	Vec3Copy (vec3Origin, gameEntity->velocity); 
	SetClipmask (CONTENTS_MASK_SHOT);
	SetSolid (SOLID_BBOX);
	SetMins(vec3Origin);
	SetMaxs(vec3Origin);

	PhysicsType = PHYSICS_STEP;
}

CStepPhysics::CStepPhysics (int Index) :
CPhysicsEntity (Index)
{
	Vec3Copy (vec3Origin, gameEntity->velocity); 
	SetClipmask (CONTENTS_MASK_SHOT);
	SetSolid (SOLID_BBOX);
	SetMins(vec3Origin);
	SetMaxs(vec3Origin);

	PhysicsType = PHYSICS_STEP;
}

void CStepPhysics::CheckGround ()
{
	CTrace		trace;

	if (gameEntity->velocity[2] > 100)
	{
		GroundEntity = NULL;
		return;
	}

// if the hull point one-quarter unit down is solid the entity is on ground
	vec3f point = State.GetOrigin();
	point.Z -= 0.25f;

	trace = CTrace (State.GetOrigin(), GetMins(), GetMaxs(), point, gameEntity, CONTENTS_MASK_MONSTERSOLID);

	// check steepness
	if ( trace.plane.normal[2] < 0.7 && !trace.startSolid)
	{
		GroundEntity = NULL;
		return;
	}

	if (!trace.startSolid && !trace.allSolid)
	{
		State.SetOrigin (trace.EndPos);
		GroundEntity = trace.Ent;
		GroundEntityLinkCount = trace.Ent->GetLinkCount();
		gameEntity->velocity[2] = 0;
	}
}

#define SV_STOPSPEED		100
#define SV_FRICTION			6
#define SV_WATERFRICTION	1

void CStepPhysics::AddRotationalFriction ()
{
	vec3_t angles;
	State.GetAngles (angles);
	Vec3MA (angles, 0.1f, gameEntity->avelocity, angles);
	State.SetAngles (angles);

	float adjustment = 0.1f * SV_STOPSPEED * SV_FRICTION;
	for (int n = 0; n < 3; n++)
	{
		if (gameEntity->avelocity[n] > 0)
		{
			gameEntity->avelocity[n] -= adjustment;
			if (gameEntity->avelocity[n] < 0)
				gameEntity->avelocity[n] = 0;
		}
		else
		{
			gameEntity->avelocity[n] += adjustment;
			if (gameEntity->avelocity[n] > 0)
				gameEntity->avelocity[n] = 0;
		}
	}
}

#define MAX_CLIP_PLANES	5
int CStepPhysics::FlyMove (float time, int mask)
{
	edict_t		*hit;
	int			bumpcount, numbumps;
	vec3_t		dir;
	float		d;
	int			numplanes;
	vec3_t		planes[MAX_CLIP_PLANES];
	vec3_t		primal_velocity, original_velocity, new_velocity;
	int			i, j;
	CTrace		trace;
	vec3_t		end;
	float		time_left;
	int			blocked;
	
	numbumps = 4;
	
	blocked = 0;
	Vec3Copy (gameEntity->velocity, original_velocity);
	Vec3Copy (gameEntity->velocity, primal_velocity);
	numplanes = 0;
	
	time_left = time;

	GroundEntity = NULL;
	for (bumpcount=0 ; bumpcount<numbumps ; bumpcount++)
	{
		vec3_t origin;
		State.GetOrigin (origin);
		for (i=0 ; i<3 ; i++)
			end[i] = origin[i] + time_left * gameEntity->velocity[i];

		trace = CTrace (origin, gameEntity->mins, gameEntity->maxs, end, gameEntity, mask);

		if (trace.allSolid)
		{	// entity is trapped in another solid
			Vec3Copy (vec3Origin, gameEntity->velocity);
			return 3;
		}

		if (trace.fraction > 0)
		{	// actually covered some distance
			State.SetOrigin (trace.endPos);
			Vec3Copy (gameEntity->velocity, original_velocity);
			numplanes = 0;
		}

		if (trace.fraction == 1)
			 break;		// moved the entire distance

		hit = trace.ent;

		if (trace.plane.normal[2] > 0.7)
		{
			blocked |= 1;		// floor
			if ( hit->solid == SOLID_BSP)
			{
				GroundEntity = hit->Entity;
				GroundEntityLinkCount = GroundEntity->GetLinkCount();
			}
		}
		if (!trace.plane.normal[2])
			blocked |= 2;		// step

//
// run the impact function
//
		Impact (&trace);
		if (!IsInUse())
			break;		// removed by the impact function

		
		time_left -= time_left * trace.fraction;
		
	// cliped to another plane
		if (numplanes >= MAX_CLIP_PLANES)
		{	// this shouldn't really happen
			Vec3Copy (vec3Origin, gameEntity->velocity);
			return 3;
		}

		Vec3Copy (trace.plane.normal, planes[numplanes]);
		numplanes++;

//
// modify original_velocity so it parallels all of the clip planes
//
		for (i=0 ; i<numplanes ; i++)
		{
			ClipVelocity (original_velocity, planes[i], new_velocity, 1);

			for (j=0 ; j<numplanes ; j++)
				if ((j != i) && !Vec3Compare (planes[i], planes[j]))
				{
					if (Dot3Product (new_velocity, planes[j]) < 0)
						break;	// not ok
				}
			if (j == numplanes)
				break;
		}
		
		if (i != numplanes)
		{	// go along this plane
			Vec3Copy (new_velocity, gameEntity->velocity);
		}
		else
		{	// go along the crease
			if (numplanes != 2)
			{
//				gi.dprintf ("clip velocity, numplanes == %i\n",numplanes);
				Vec3Copy (vec3Origin, gameEntity->velocity);
				return 7;
			}
			CrossProduct (planes[0], planes[1], dir);
			d = Dot3Product (dir, gameEntity->velocity);
			Vec3Scale (dir, d, gameEntity->velocity);
		}

//
// if original velocity is against the original velocity, stop dead
// to avoid tiny occilations in sloping corners
//
		if (Dot3Product (gameEntity->velocity, primal_velocity) <= 0)
		{
			Vec3Copy (vec3Origin, gameEntity->velocity);
			return blocked;
		}
	}

	return blocked;
}

bool CStepPhysics::Run ()
{
	bool		hitsound = false;
	float		*vel;
	float		speed, newspeed, control;
	float		friction;

	if (PhysicsDisabled)
		return false;

	// airborn monsters should always check for ground
	if (!GroundEntity && (EntityFlags & ENT_MONSTER))
		(dynamic_cast<CMonsterEntity*>(this))->Monster->CheckGround ();
	else
		CheckGround (); // Specific non-monster checkground

	bool wasonground = (GroundEntity) ? true : false;
		
	if (gameEntity->avelocity[0] || gameEntity->avelocity[1] || gameEntity->avelocity[2])
		AddRotationalFriction ();

	// add gravity except:
	//   flying monsters
	//   swimming monsters who are in the water
	if (!wasonground)
	{
		if (!(Flags & FL_FLY))
		{
			if (!((Flags & FL_SWIM) && (gameEntity->waterlevel > 2)))
			{
				if (gameEntity->velocity[2] < sv_gravity->Float()*-0.1)
					hitsound = true;
				if (gameEntity->waterlevel == 0)
					AddGravity ();
			}
		}
	}

	// friction for flying monsters that have been given vertical velocity
	if ((Flags & FL_FLY) && (gameEntity->velocity[2] != 0))
	{
		speed = Q_fabs(gameEntity->velocity[2]);
		control = (speed < SV_STOPSPEED) ? SV_STOPSPEED : speed;
		friction = SV_FRICTION/3;
		newspeed = speed - (0.1f * control * friction);
		if (newspeed < 0)
			newspeed = 0;
		newspeed /= speed;
		gameEntity->velocity[2] *= newspeed;
	}

	// friction for flying monsters that have been given vertical velocity
	if ((Flags & FL_SWIM) && (gameEntity->velocity[2] != 0))
	{
		speed = Q_fabs(gameEntity->velocity[2]);
		control = (speed < SV_STOPSPEED) ? SV_STOPSPEED : speed;
		newspeed = speed - (0.1f * control * SV_WATERFRICTION * gameEntity->waterlevel);
		if (newspeed < 0)
			newspeed = 0;
		newspeed /= speed;
		gameEntity->velocity[2] *= newspeed;
	}

	if (gameEntity->velocity[2] || gameEntity->velocity[1] || gameEntity->velocity[0])
	{
		// apply friction
		// let dead monsters who aren't completely onground slide
		if ((wasonground) || (Flags & (FL_SWIM|FL_FLY)) && !(gameEntity->health <= 0.0 && ((EntityFlags & ENT_MONSTER) && !(dynamic_cast<CMonsterEntity*>(this))->Monster->CheckBottom())))
		{
			vel = gameEntity->velocity;
			speed = sqrtf(vel[0]*vel[0] +vel[1]*vel[1]);
			if (speed)
			{
				friction = SV_FRICTION;

				control = (speed < SV_STOPSPEED) ? SV_STOPSPEED : speed;
				newspeed = speed - 0.1f*control*friction;

				if (newspeed < 0)
					newspeed = 0;
				newspeed /= speed;

				vel[0] *= newspeed;
				vel[1] *= newspeed;
			}
		}

		FlyMove (0.1f,  (EntityFlags & ENT_MONSTER) ? CONTENTS_MASK_MONSTERSOLID : CONTENTS_MASK_SOLID);

		Link();
		G_TouchTriggers (this);
		if (!IsInUse())
			return false;

		if (GroundEntity && !wasonground && hitsound)
			PlaySound (CHAN_AUTO, SoundIndex("world/land.wav"));
	}
	return true;
}

// Move physics
CPushPhysics::CPushPhysics() :
CPhysicsEntity()
{
	PhysicsType = PHYSICS_PUSH;
};

CPushPhysics::CPushPhysics(int Index) :
CPhysicsEntity(Index)
{
	PhysicsType = PHYSICS_PUSH;
};

typedef struct
{
	edict_t	*ent;
	vec3_t	origin;
	vec3_t	angles;
	float	deltayaw;
} pushed_t;
pushed_t	pushed[MAX_CS_EDICTS], *pushed_p;

edict_t	*obstacle;

/*
============
SV_AddGravity

============
*/
void SV_AddGravity (edict_t *ent)
{
	ent->velocity[2] -= ent->gravity * sv_gravity->Float() * 0.1f;
}

/*
============
SV_TestEntityPosition

============
*/
edict_t	*SV_TestEntityPosition (edict_t *ent)
{
	CTrace	trace;
	int			mask;

	if (ent->clipMask)
		mask = ent->clipMask;
	else
		mask = CONTENTS_MASK_SOLID;
	trace = CTrace (ent->state.origin, ent->mins, ent->maxs, ent->state.origin, ent, mask);
	
	if (trace.startSolid)
		return g_edicts;
		
	return NULL;
}

/*
==================
ClipVelocity

Slide off of the impacting object
returns the blocked flags (1 = floor, 2 = step / wall)
==================
*/
#define STOP_EPSILON	0.1

int ClipVelocity (vec3_t in, vec3_t normal, vec3_t out, float overbounce)
{
	float	backoff;
	float	change;
	int		i, blocked;
	
	blocked = 0;
	if (normal[2] > 0)
		blocked |= 1;		// floor
	if (!normal[2])
		blocked |= 2;		// step
	
	backoff = Dot3Product (in, normal) * overbounce;

	for (i=0 ; i<3 ; i++)
	{
		change = normal[i]*backoff;
		out[i] = in[i] - change;
		if (out[i] > -STOP_EPSILON && out[i] < STOP_EPSILON)
			out[i] = 0;
	}

	return blocked;
}

bool Push (CBaseEntity *Entity, vec3_t move, vec3_t amove)
{
	int			i, e;
	edict_t		*check, *block;
	vec3_t		mins, maxs;
	pushed_t	*p;
	vec3_t		org, org2, move2, forward, right, up;

	// clamp the move to 1/8 units, so the position will
	// be accurate for client side prediction
	for (i=0 ; i<3 ; i++)
	{
		float	temp;
		temp = move[i]*8.0;
		if (temp > 0.0)
			temp += 0.5;
		else
			temp -= 0.5;
		move[i] = 0.125 * (int)temp;
	}

	vec3_t absMinTemp, absMaxTemp;
	Entity->GetAbsMin (absMinTemp);
	Entity->GetAbsMax (absMaxTemp);
	// find the bounding box
	for (i=0 ; i<3 ; i++)
	{
		mins[i] = absMinTemp[i] + move[i];
		maxs[i] = absMaxTemp[i] + move[i];
	}

// we need this for pushing things later
	Vec3Subtract (vec3Origin, amove, org);
	Angles_Vectors (org, forward, right, up);

// save the pusher's original position
	pushed_p->ent = Entity->gameEntity;
	Entity->State.GetOrigin (pushed_p->origin);
	Entity->State.GetAngles (pushed_p->angles);
	if (Entity->gameEntity->client)
		pushed_p->deltayaw = (dynamic_cast<CPlayerEntity*>(Entity))->Client.PlayerState.GetPMove()->deltaAngles[YAW];
	pushed_p++;

// move the pusher to it's final position
	Entity->State.SetOrigin (Entity->State.GetOrigin() + vec3f(move));
	Entity->State.SetAngles (Entity->State.GetAngles() + vec3f(amove));
	Entity->Link ();

// see if any solid entities are inside the final position
	check = g_edicts+1;
	for (e = 1; e < globals.numEdicts; e++, check++)
	{
		if (!check->inUse)
			continue;
		if (!check->Entity)
			continue;

		CBaseEntity *Check = check->Entity;
		if (Check->EntityFlags & ENT_PHYSICS)
		{
			CPhysicsEntity *CheckPhys = dynamic_cast<CPhysicsEntity*>(Check);
			if (CheckPhys->PhysicsType == PHYSICS_PUSH
			|| CheckPhys->PhysicsType == PHYSICS_STOP
			|| CheckPhys->PhysicsType == PHYSICS_NONE
			|| CheckPhys->PhysicsType == PHYSICS_NOCLIP)
				continue;
		}
		//else if (!(Check->EntityFlags & ENT_PLAYER))
		//	continue;

		if (!check->area.prev)
			continue;		// not linked in anywhere

	// if the entity is standing on the pusher, it will definitely be moved
		if (Check->GroundEntity != Entity)
		{
			// see if the ent needs to be tested
			if (check->absMin[0] >= maxs[0]
			|| check->absMin[1] >= maxs[1]
			|| check->absMin[2] >= maxs[2]
			|| check->absMax[0] <= mins[0]
			|| check->absMax[1] <= mins[1]
			|| check->absMax[2] <= mins[2])
				continue;

			// see if the ent's bbox is inside the pusher's final position
			if (!SV_TestEntityPosition (check))
				continue;
		}

		if ((dynamic_cast<CPhysicsEntity*>(Entity)->PhysicsType == PHYSICS_PUSH) || (Check->GroundEntity == Entity))
		{
			// move this entity
			pushed_p->ent = check;
			Vec3Copy (check->state.origin, pushed_p->origin);
			Vec3Copy (check->state.angles, pushed_p->angles);

			// try moving the contacted entity 
			Vec3Add (check->state.origin, move, check->state.origin);
			if (check->client)
			{
				(dynamic_cast<CPlayerEntity*>(Check))->Client.PlayerState.GetPMove()->deltaAngles[YAW] += amove[YAW];

				//r1: dead-body-on-lift / other random view distortion fix
				pushed_p->deltayaw = (dynamic_cast<CPlayerEntity*>(pushed_p->ent->Entity))->Client.PlayerState.GetPMove()->deltaAngles[YAW];
			}

			else
				check->state.angles[YAW] += amove[YAW];

			pushed_p++;

			// figure movement due to the pusher's amove
			vec3_t temporigin;
			Entity->State.GetOrigin (temporigin);
			Vec3Subtract (check->state.origin, temporigin, org);
			org2[0] = Dot3Product (org, forward);
			org2[1] = -Dot3Product (org, right);
			org2[2] = Dot3Product (org, up);
			Vec3Subtract (org2, org, move2);
			Vec3Add (check->state.origin, move2, check->state.origin);

			// may have pushed them off an edge
			if (Check->GroundEntity != Entity)
				Check->GroundEntity = NULL;

			block = SV_TestEntityPosition (check);
			if (!block)
			{	// pushed ok
				gi.linkentity (check);
				// impact?
				continue;
			}

			// if it is ok to leave in the old position, do it
			// this is only relevent for riding entities, not pushed
			// FIXME: this doesn't acount for rotation
			Vec3Subtract (check->state.origin, move, check->state.origin);
			block = SV_TestEntityPosition (check);
			if (!block)
			{
				pushed_p--;
				continue;
			}
		}
		
		// save off the obstacle so we can call the block function
		obstacle = check;

		// move back any entities we already moved
		// go backwards, so if the same entity was pushed
		// twice, it goes back to the original position
		for (p=pushed_p-1 ; p>=pushed ; p--)
		{
			Vec3Copy (p->origin, p->ent->state.origin);
			Vec3Copy (p->angles, p->ent->state.angles);
			if (p->ent->client)
			{
				(dynamic_cast<CPlayerEntity*>(p->ent->Entity))->Client.PlayerState.GetPMove()->deltaAngles[YAW] = p->deltayaw;
			}
			gi.linkentity (p->ent);
		}
		return false;
	}

//FIXME: is there a better way to handle this?
	// see if anything we moved has touched a trigger
	for (p=pushed_p-1 ; p>=pushed ; p--)
		G_TouchTriggers (p->ent->Entity);

	return true;
}

bool CPushPhysics::Run ()
{
	vec3_t		move, amove;
	CBaseEntity		*part;

	// if not a team captain, so movement will be handled elsewhere
	if ( Flags & FL_TEAMSLAVE)
		return false;

	// make sure all team slaves can move before commiting
	// any moves or calling any think functions
	// if the move is blocked, all moved objects will be backed out
	pushed_p = pushed;
	for (part = this; part; part = part->TeamChain)
	{
		if (part->gameEntity->velocity[0] || part->gameEntity->velocity[1] || part->gameEntity->velocity[2] ||
			part->gameEntity->avelocity[0] || part->gameEntity->avelocity[1] || part->gameEntity->avelocity[2])
		{
			// object is moving
			Vec3Scale (part->gameEntity->velocity, 1, move);
			Vec3Scale (part->gameEntity->avelocity, 1, amove);

			if (!Push (part, move, amove))
				break;
		}
	}
	if (pushed_p > &pushed[MAX_CS_EDICTS])
		GameError ("pushed_p > &pushed[MAX_CS_EDICTS], memory corrupted");

	if (part)
	{
		// the move failed, bump all nextthink times and back out moves
		for (CBaseEntity *mv = this; mv; mv = mv->TeamChain)
		{
			if (mv->EntityFlags & ENT_THINKABLE)
			{
				CThinkableEntity *Thinkable = dynamic_cast<CThinkableEntity*>(mv);

				if (Thinkable->NextThink > 0)
					Thinkable->NextThink += FRAMETIME;
			}
		}

		// if the pusher has a "blocked" function, call it
		// otherwise, just stay in place until the obstacle is gone
		if ((part->EntityFlags & ENT_BLOCKABLE) && obstacle->Entity)
			(dynamic_cast<CBlockableEntity*>(part))->Blocked (obstacle->Entity);
	}
	else
	{
		// the move succeeded, so call all think functions
		for (part = this; part; part = part->TeamChain)
		{
			if (part->EntityFlags & ENT_THINKABLE)
			{
				CThinkableEntity *Thinkable = dynamic_cast<CThinkableEntity*>(part);
				Thinkable->RunThink ();
			}
		}
	}
	return true;
}

CStopPhysics::CStopPhysics () :
CPushPhysics()
{
	PhysicsType = PHYSICS_STOP;
};

CStopPhysics::CStopPhysics (int Index) :
CPushPhysics(Index)
{
	PhysicsType = PHYSICS_STOP;
};

bool CStopPhysics::Run ()
{
	return CPushPhysics::Run ();
}

CBlockableEntity::CBlockableEntity () :
CBaseEntity ()
{
	EntityFlags |= ENT_BLOCKABLE;
}

CBlockableEntity::CBlockableEntity (int Index) :
CBaseEntity (Index)
{
	EntityFlags |= ENT_BLOCKABLE;
}

CUsableEntity::CUsableEntity () :
CBaseEntity ()
{
	EntityFlags |= ENT_USABLE;
}

CUsableEntity::CUsableEntity (int Index) :
CBaseEntity (Index)
{
	EntityFlags |= ENT_USABLE;
}

class CDelayedUse : public CThinkableEntity, public CUsableEntity
{
public:
	CBaseEntity	*Activator;
	const char	*Message;

	CDelayedUse () :
	  CBaseEntity (),
	  CThinkableEntity (),
	  Activator(NULL)
	  {
	  };

	CDelayedUse (int Index) :
	  CBaseEntity (Index),
	  CThinkableEntity (Index),
	  Activator(NULL)
	  {
	  };

	void Use (CBaseEntity *, CBaseEntity *)
	{
	};

	void Think ()
	{
		UseTargets (Activator, Message);
		Free ();
	}
};

void CUsableEntity::UseTargets (CBaseEntity *activator, const char *Message)
{
//
// check for a delay
//
	if (gameEntity->delay)
	{
	// create a temp object to fire at a later time
		CDelayedUse *t = QNew (com_levelPool, 0) CDelayedUse;
		t->gameEntity->classname = "DelayedUse";

		// Paril: for compatibility
		t->NextThink = level.framenum + (gameEntity->delay * 10);
		t->Activator = activator;
		if (!activator)
			DebugPrintf ("DelayedUse with no activator\n");
		t->Message = Message;
		t->gameEntity->target = gameEntity->target;
		t->gameEntity->killtarget = gameEntity->killtarget;
		return;
	}

//
// print the message
//
	if ((Message) && (activator->EntityFlags & ENT_PLAYER))
	{
		CPlayerEntity *Player = dynamic_cast<CPlayerEntity*>(activator);
		Player->PrintToClient (PRINT_CENTER, "%s", Message);
		if (gameEntity->noise_index)
			Player->PlaySound (CHAN_AUTO, gameEntity->noise_index);
		else
			Player->PlaySound (CHAN_AUTO, SoundIndex ("misc/talk1.wav"));
	}

//
// kill killtargets
//
	if (gameEntity->killtarget)
	{
		CBaseEntity *t = NULL;
		while ((t = CC_Find (t, FOFS(targetname), gameEntity->killtarget)) != NULL)
		{
			t->Free ();

			if (!IsInUse())
			{
				DebugPrintf("entity was removed while using killtargets\n");
				return;
			}
		}
	}

//
// fire targets
//
	if (gameEntity->target)
	{
		CBaseEntity *Ent = NULL;
		while ((Ent = CC_Find (Ent, FOFS(targetname), gameEntity->target)) != NULL)
		{
			if (!Ent)
				continue;

			// doors fire area portals in a specific way
			if (!Q_stricmp(Ent->gameEntity->classname, "func_areaportal") &&
				(!Q_stricmp(Ent->gameEntity->classname, "func_door") || !Q_stricmp(Ent->gameEntity->classname, "func_door_rotating")))
				continue;

			if (Ent == this)
				DebugPrintf ("WARNING: Entity used itself.\n");
			else if (Ent->EntityFlags & ENT_USABLE)
				(dynamic_cast<CUsableEntity*>(Ent))->Use (this, activator);
			if (!IsInUse())
			{
				DebugPrintf("entity was removed while using targets\n");
				return;
			}
		}
	}
}