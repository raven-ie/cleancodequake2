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
// g_combat.c

#include "g_local.h"

/*
============
CanDamage

Returns true if the inflictor can directly damage the target.  Used for
explosions and melee attacks.
============
*/
bool CanDamage (edict_t *targ, edict_t *inflictor)
{
	vec3_t	dest;
	CTrace	trace;

// bmodels need special checking because their origin is 0,0,0
	if (targ->movetype == MOVETYPE_PUSH)
	{
		Vec3Add (targ->absMin, targ->absMax, dest);
		Vec3Scale (dest, 0.5, dest);
		trace = CTrace (inflictor->state.origin, dest, inflictor, CONTENTS_MASK_SOLID);
		if (trace.fraction == 1.0)
			return true;
		if (trace.ent == targ)
			return true;
		return false;
	}
	
	trace = CTrace (inflictor->state.origin, targ->state.origin, inflictor, CONTENTS_MASK_SOLID);
	if (trace.fraction == 1.0)
		return true;

	Vec3Copy (targ->state.origin, dest);
	dest[0] += 15.0;
	dest[1] += 15.0;
	trace = CTrace (inflictor->state.origin, dest, inflictor, CONTENTS_MASK_SOLID);
	if (trace.fraction == 1.0)
		return true;

	Vec3Copy (targ->state.origin, dest);
	dest[0] += 15.0;
	dest[1] -= 15.0;
	trace = CTrace (inflictor->state.origin, dest, inflictor, CONTENTS_MASK_SOLID);
	if (trace.fraction == 1.0)
		return true;

	Vec3Copy (targ->state.origin, dest);
	dest[0] -= 15.0;
	dest[1] += 15.0;
	trace = CTrace (inflictor->state.origin, dest, inflictor, CONTENTS_MASK_SOLID);
	if (trace.fraction == 1.0)
		return true;

	Vec3Copy (targ->state.origin, dest);
	dest[0] -= 15.0;
	dest[1] -= 15.0;
	trace = CTrace (inflictor->state.origin, dest, inflictor, CONTENTS_MASK_SOLID);
	if (trace.fraction == 1.0)
		return true;


	return false;
}


/*
============
Killed
============
*/
void Killed (edict_t *targ, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
{
	if (targ->health < -999)
		targ->health = -999;

	targ->enemy = attacker;

	if ((targ->svFlags & SVF_MONSTER) && (targ->deadflag != DEAD_DEAD) && targ->Monster)
	{
//		targ->svFlags |= SVF_DEADMONSTER;	// now treat as a different content type
		if (!(targ->Monster->AIFlags & AI_GOOD_GUY))
		{
			level.killed_monsters++;
			if (game.mode == GAME_COOPERATIVE && (attacker->client))
				(dynamic_cast<CPlayerEntity*>(attacker->Entity))->Client.resp.score++;
			// medics won't heal monsters that they kill themselves
			if (strcmp(attacker->classname, "monster_medic") == 0)
				targ->owner = attacker;
		}
	}

	if (!(targ->movetype == MOVETYPE_PUSH || targ->movetype == MOVETYPE_STOP || targ->movetype == MOVETYPE_NONE) && 
		(targ->svFlags & SVF_MONSTER) && (targ->deadflag != DEAD_DEAD))
	{
		targ->touch = NULL;
		if (targ->Monster)
			targ->Monster->MonsterDeathUse();
	}

	if (targ->Entity && (targ->Entity->EntityFlags & ENT_HURTABLE) && inflictor->Entity && attacker->Entity)
		(dynamic_cast<CHurtableEntity*>(targ->Entity))->Die (inflictor->Entity, attacker->Entity, damage, point);
	else
		targ->die (targ, inflictor, attacker, damage, point);
}

/*
============
T_Damage

targ		entity that is being damaged
inflictor	entity that is causing the damage
attacker	entity that caused the inflictor to damage targ
	example: targ=monster, inflictor=rocket, attacker=player

dir			direction of the attack
point		point at which the damage is being inflicted
normal		normal vector from that point
damage		amount of damage being inflicted
knockback	force to be applied against targ as a result of the damage

dflags		these flags are used to control how T_Damage works
	DAMAGE_RADIUS			damage was indirect (from a nearby explosion)
	DAMAGE_NO_ARMOR			armor does not protect from this damage
	DAMAGE_ENERGY			damage is from an energy based weapon
	DAMAGE_NO_KNOCKBACK		do not affect velocity, just view angles
	DAMAGE_BULLET			damage is from a bullet (used for ricochets)
	DAMAGE_NO_PROTECTION	kills godmode, armor, everything
============
*/
static int CheckPowerArmor (edict_t *ent, vec3_t point, vec3_t normal, int damage, int dflags)
{
	int			save;
	int			power_armor_type;
	int			index = FindItem("Cells")->GetIndex();
	int			damagePerCell;
	int			pa_te_type;
	int			power = 0;
	int			power_used;
	bool		IsClient = (ent->Entity && (ent->Entity->EntityFlags & ENT_PLAYER));
	CPlayerEntity	*Player = NULL;

	if (IsClient)
		Player = dynamic_cast<CPlayerEntity*>(ent->Entity);

	if (!damage)
		return 0;

	if (dflags & DAMAGE_NO_ARMOR)
		return 0;

	if (IsClient)
	{
		power_armor_type = Player->PowerArmorType ();
		if (power_armor_type != POWER_ARMOR_NONE)
			power = Player->Client.pers.Inventory.Has(FindItem("Cells"));
	}
	else if ((ent->svFlags & SVF_MONSTER) && ent->Monster)
	{
		power_armor_type = ent->Monster->PowerArmorType;
		power = ent->Monster->PowerArmorPower;
	}
	else
		return 0;

	if (power_armor_type == POWER_ARMOR_NONE)
		return 0;
	if (!power)
		return 0;

	if (power_armor_type == POWER_ARMOR_SCREEN)
	{
		vec3_t		vec;
		float		dot;
		vec3_t		forward;

		// only works if damage point is in front
		Angles_Vectors (ent->state.angles, forward, NULL, NULL);
		Vec3Subtract (point, ent->state.origin, vec);
		VectorNormalizef (vec, vec);
		dot = DotProduct (vec, forward);
		if (dot <= 0.3)
			return 0;

		damagePerCell = 1;
		pa_te_type = TE_SCREEN_SPARKS;
		damage = damage / 3;
	}
	else
	{
		damagePerCell = 2;
		pa_te_type = TE_SHIELD_SPARKS;
		damage = (2 * damage) / 3;
	}

	save = power * damagePerCell;
	if (!save)
		return 0;
	if (save > damage)
		save = damage;

	CTempEnt_Splashes::ShieldSparks (point, normal, (pa_te_type == TE_SCREEN_SPARKS) ? true : false);
	ent->powerarmor_time = level.framenum + 2;

	power_used = save / damagePerCell;
	if (!power_used)
		power_used = 1;

	if (IsClient)
		Player->Client.pers.Inventory.Remove(index, power_used);
	else if (ent->Monster)
		ent->Monster->PowerArmorPower -= power_used;
	return save;
}

bool CheckTeamDamage (edict_t *targ, edict_t *attacker)
{
#ifdef CLEANCTF_ENABLED
//ZOID
	if ((game.mode & GAME_CTF) && targ->client && attacker->client)
	{
		CPlayerEntity *Targ = dynamic_cast<CPlayerEntity*>(targ->Entity);
		CPlayerEntity *Attacker = dynamic_cast<CPlayerEntity*>(attacker->Entity);
		if (Targ->Client.resp.ctf_team == Attacker->Client.resp.ctf_team &&
			targ != attacker)
			return true;
	}
//ZOID
#endif

	return false;
}

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

void T_Damage (edict_t *targ, edict_t *inflictor, edict_t *attacker, vec3_t dir, vec3_t point, vec3_t normal, int damage, int knockback, int dflags, int mod)
{
	gclient_t	*client;
	int			take;
	int			save;
	int			asave = 0;
	int			psave = 0;

	if (!targ->takedamage)
		return;

	// friendly fire avoidance
	// if enabled you can't hurt teammates (but you can hurt yourself)
	// knockback still occurs
	if ((targ != attacker) && ((game.mode & GAME_DEATHMATCH) && (dmFlags.dfSkinTeams || dmFlags.dfModelTeams) || game.mode == GAME_COOPERATIVE))
	{
		if (targ->client && attacker->client)
		{
			if (OnSameTeam (dynamic_cast<CPlayerEntity*>(targ->Entity), dynamic_cast<CPlayerEntity*>(attacker->Entity)))
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
	if (skill->Integer() == 0 && !(game.mode & GAME_DEATHMATCH) && targ->client)
	{
		damage *= 0.5;
		if (!damage)
			damage = 1;
	}

	client = targ->client;

	VectorNormalizef (dir, dir);

// bonus damage for suprising a monster
	if (!(dflags & DAMAGE_RADIUS) && (targ->svFlags & SVF_MONSTER) && (attacker->client) && (!targ->enemy) && (targ->health > 0))
		damage *= 2;

#ifdef CLEANCTF_ENABLED
//ZOID
//strength tech
	if ((game.mode & GAME_CTF) && attacker->client)
		damage = (dynamic_cast<CPlayerEntity*>(attacker->Entity))->CTFApplyStrength(damage);
//ZOID
#endif

	if (targ->flags & FL_NO_KNOCKBACK)
		knockback = 0;

// figure momentum add
	if (!(dflags & DAMAGE_NO_KNOCKBACK))
	{
		if ((knockback) && (targ->movetype != MOVETYPE_NONE) && (targ->movetype != MOVETYPE_BOUNCE) && (targ->movetype != MOVETYPE_PUSH) && (targ->movetype != MOVETYPE_STOP))
		{
			vec3_t	kvel;
			float	mass;

			if (targ->mass < 50)
				mass = 50;
			else
				mass = targ->mass;

			if (targ->client  && attacker == targ)
				Vec3Scale (dir, 1600.0 * (float)knockback / mass, kvel);	// the rocket jump hack...
			else
				Vec3Scale (dir, 500.0 * (float)knockback / mass, kvel);

			Vec3Add (targ->velocity, kvel, targ->velocity);
		}
	}

	take = damage;
	save = 0;

	// check for godmode
	if ( (targ->flags & FL_GODMODE) && !(dflags & DAMAGE_NO_PROTECTION) )
	{
		take = 0;
		save = damage;
		CTempEnt_Splashes::Sparks (point, normal, (dflags & DAMAGE_BULLET) ? CTempEnt_Splashes::STBulletSparks : CTempEnt_Splashes::STSparks, CTempEnt_Splashes::SPTSparks);
	}

	// check for invincibility
	if ((client && (dynamic_cast<CPlayerEntity*>(targ->Entity))->Client.invincible_framenum > level.framenum ) && !(dflags & DAMAGE_NO_PROTECTION))
	{
		if (targ->pain_debounce_time < level.framenum)
		{
			PlaySoundFrom (targ, CHAN_ITEM, SoundIndex("items/protect4.wav"), 1, ATTN_NORM);
			targ->pain_debounce_time = level.framenum + 20;
		}
		take = 0;
		save = damage;
	}

#ifdef CLEANCTF_ENABLED
//ZOID
//team armor protect
	if ((game.mode & GAME_CTF) && targ->client && attacker->client)
	{
		if ((dynamic_cast<CPlayerEntity*>(targ->Entity))->Client.resp.ctf_team == (dynamic_cast<CPlayerEntity*>(attacker->Entity))->Client.resp.ctf_team &&
		targ != attacker && dmFlags.dfCtfArmorProtect)
		psave = asave = 0;
	}
	else
	{
//ZOID
#endif
		psave = CheckPowerArmor (targ, point, normal, take, dflags);
		take -= psave;

		if (targ->client)
		{
			CPlayerEntity *Targ = (dynamic_cast<CPlayerEntity*>(targ->Entity));

			if (Targ->Client.pers.Armor)
			{
				asave = Targ->Client.pers.Armor->CheckArmor (Targ, point, normal, take, dflags);
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
	if (client)
		take = (dynamic_cast<CPlayerEntity*>(targ->Entity))->CTFApplyResistance(take);
//ZOID
#endif

	// team damage avoidance
	if (!(dflags & DAMAGE_NO_PROTECTION) && CheckTeamDamage (targ, attacker))
		return;

#ifdef CLEANCTF_ENABLED
//ZOID
	if (targ->client && attacker->client)
		CTFCheckHurtCarrier((dynamic_cast<CPlayerEntity*>(targ->Entity)), (dynamic_cast<CPlayerEntity*>(attacker->Entity)));
//ZOID
#endif

// do the damage
	if (take)
	{
		if ((targ->svFlags & SVF_MONSTER) || (client))
			CTempEnt_Splashes::Blood (point, normal);
		else
			CTempEnt_Splashes::Sparks (point, normal, (dflags & DAMAGE_BULLET) ? CTempEnt_Splashes::STBulletSparks : CTempEnt_Splashes::STSparks, CTempEnt_Splashes::SPTSparks);

		targ->health = targ->health - take;
			
		if (targ->health <= 0)
		{
			if ((targ->svFlags & SVF_MONSTER) || (client))
				targ->flags |= FL_NO_KNOCKBACK;
			Killed (targ, inflictor, attacker, take, point);
			return;
		}
	}

	if (targ->Monster)
	{
		targ->Monster->ReactToDamage (attacker);
		if (!(targ->Monster->AIFlags & AI_DUCKED) && take)
		{
			targ->Monster->Pain (attacker, knockback, take);
			if (skill->Integer() == 3)
				targ->pain_debounce_time = level.framenum + 50;
		}
	}
	/*else if (client)
	{
		if (!(targ->flags & FL_GODMODE) && (take))
			targ->pain (targ, attacker, knockback, take);
	}
	else if (take)
	{
		if (targ->pain)
			targ->pain (targ, attacker, knockback, take);
	}*/
	else if (take && targ->Entity && (targ->Entity->EntityFlags & ENT_HURTABLE) && attacker->Entity)
		(dynamic_cast<CHurtableEntity*>(targ->Entity))->Pain (attacker->Entity, knockback, take);
	else if (targ->pain && take)
		targ->pain (targ, attacker, knockback, take);

	// add to the damage inflicted on a player this frame
	// the total will be turned into screen blends and view angle kicks
	// at the end of the frame
	if (client)
	{
		CPlayerEntity *Player = dynamic_cast<CPlayerEntity*>(targ->Entity);
		Player->Client.damage_parmor += psave;
		Player->Client.damage_armor += asave;
		Player->Client.damage_blood += take;
		Player->Client.damage_knockback += knockback;
		Vec3Copy (point, Player->Client.damage_from);
	}
}


/*
============
T_RadiusDamage
============
*/
void T_RadiusDamage (edict_t *inflictor, edict_t *attacker, float damage, edict_t *ignore, float radius, int mod)
{
	float	points;
	edict_t	*ent = NULL;
	vec3_t	v;
	vec3_t	dir;

	while ((ent = findradius(ent, inflictor->state.origin, radius)) != NULL)
	{
		if (ent == ignore)
			continue;
		if (!ent->takedamage)
			continue;

		Vec3Add (ent->mins, ent->maxs, v);
		Vec3MA (ent->state.origin, 0.5, v, v);
		Vec3Subtract (inflictor->state.origin, v, v);
		points = damage - 0.5 * Vec3Length (v);
		if (ent == attacker)
			points = points * 0.5;
		if (points > 0)
		{
			if (CanDamage (ent, inflictor))
			{
				Vec3Subtract (ent->state.origin, inflictor->state.origin, dir);
				T_Damage (ent, inflictor, attacker, dir, inflictor->state.origin, vec3Origin, (int)points, (int)points, DAMAGE_RADIUS, mod);
			}
		}
	}
}