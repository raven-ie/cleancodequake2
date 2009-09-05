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
// cc_medic.cpp
// Medic
//

#include "cc_local.h"
#include "cc_medic.h"
#include "m_medic.h"

#ifdef MONSTER_USE_ROGUE_AI
void CMedic::CleanupHeal (bool ChangeFrame)
{
	// clean up target, if we have one and it's legit
	if (Entity->gameEntity->enemy && Entity->gameEntity->enemy->inUse)
	{
		CMonsterEntity *Enemy = dynamic_cast<CMonsterEntity*>(Entity->gameEntity->enemy->Entity);
		Enemy->Monster->Healer = NULL;
		Enemy->Monster->AIFlags &= ~AI_RESURRECTING;
		Enemy->gameEntity->takedamage = true;
		Enemy->Monster->SetEffects ();
	}

	if (ChangeFrame)
		NextFrame = FRAME_attack52;
}

void CMedic::AbortHeal (bool ChangeFrame, bool Gib, bool Mark)
{
	static vec3f	PainNormal (0, 0, 1);

	// clean up target
	CleanupHeal (ChangeFrame);
	// gib em!
	if ((Mark) && (Entity->gameEntity->enemy) && (Entity->gameEntity->enemy->inUse))
	{
		CMonsterEntity *Enemy = dynamic_cast<CMonsterEntity*>(Entity->gameEntity->enemy->Entity);
		// if the first badMedic slot is filled by a medic, skip it and use the second one
		if ((Enemy->Monster->BadMedic1) && (Enemy->Monster->BadMedic1->IsInUse())
			&& (!strncmp(Enemy->Monster->BadMedic1->gameEntity->classname, "monster_medic", 13)) )
			Enemy->Monster->BadMedic2 = Entity;
		else
			Enemy->Monster->BadMedic1 = Entity;
	}
	if ((Gib) && (Entity->gameEntity->enemy) && (Entity->gameEntity->enemy->inUse))
	{
//		if ((g_showlogic) && (g_showlogic->value))
//			gi.dprintf ("%s - gibbing bad heal target", self->classname);
		CBaseEntity *Enemy = Entity->gameEntity->enemy->Entity;

		int hurt = (Enemy->gameEntity->gib_health) ? -Enemy->gameEntity->gib_health : 500;

		if (Enemy->EntityFlags & ENT_HURTABLE)
		dynamic_cast<CHurtableEntity*>(Enemy)->TakeDamage (Entity, Entity, vec3fOrigin, Enemy->State.GetOrigin(),
					PainNormal, hurt, 0, 0, MOD_UNKNOWN);
	}
	// clean up self

	AIFlags &= ~AI_MEDIC;
	if ((Entity->gameEntity->oldenemy) && (Entity->gameEntity->oldenemy->inUse))
		Entity->gameEntity->enemy = Entity->gameEntity->oldenemy;
	else
		Entity->gameEntity->enemy = NULL;

	MedicTries = 0;
}

bool CMedic::CanReach (CBaseEntity *other)
{
	vec3f	spot1 = Entity->State.GetOrigin();
	vec3f	spot2 = other->State.GetOrigin();

	spot1.Z += Entity->gameEntity->viewheight;
	spot2.Z += other->gameEntity->viewheight;
	CTrace trace (spot1, spot2, Entity->gameEntity, CONTENTS_MASK_SHOT|CONTENTS_MASK_WATER);
	
	if (trace.fraction == 1.0 || trace.ent == other->gameEntity)		// PGM
		return true;
	return false;
}

#endif

CMedic::CMedic ()
{
	Scale = MODEL_SCALE;
}

CMonsterEntity *CMedic::FindDeadMonster ()
{
	CMonsterEntity *ent = NULL, *best = NULL;

	vec3f origin = Entity->State.GetOrigin();
	while ((ent = FindRadius<CMonsterEntity, ENT_MONSTER>(ent, origin, 1024)) != NULL)
	{
		if (ent == Entity)
			continue;
		if (ent->Monster->AIFlags & AI_GOOD_GUY)
			continue;
		if (ent->Monster->Healer)
			continue;
		if (ent->gameEntity->health > 0)
			continue;
		if (ent->NextThink)
			continue;
		if (!visible(Entity->gameEntity, ent->gameEntity))
			continue;
#ifdef MONSTER_USE_ROGUE_AI
		// check to make sure we haven't bailed on this guy already
		if ((ent->Monster->BadMedic1 == Entity) || (ent->Monster->BadMedic2 == Entity))
			continue;
		if (ent->Monster->Healer)
			// FIXME - this is correcting a bug that is somewhere else
			// if the healer is a monster, and it's in medic mode .. continue .. otherwise
			//   we will override the healer, if it passes all the other tests
			if ((ent->Monster->Healer->IsInUse()) && (ent->Monster->Healer->gameEntity->health > 0) &&
				(ent->Monster->Healer->GetSvFlags() & SVF_MONSTER) && (ent->Monster->Healer->Monster->AIFlags & AI_MEDIC))
				continue;
#endif
		if (!best)
		{
			best = ent;
			continue;
		}
		if (ent->gameEntity->max_health <= best->gameEntity->max_health)
			continue;
		best = ent;
	}

	return best;
}

void CMedic::Idle ()
{
	Entity->PlaySound (CHAN_VOICE, SoundIdle1, 1, ATTN_IDLE, 0);

	CMonsterEntity *ent = FindDeadMonster();
	if (ent)
	{
		Entity->gameEntity->enemy = ent->gameEntity;
		ent->Monster->Healer = Entity;
		AIFlags |= AI_MEDIC;
		FoundTarget ();
	}
}

void CMedic::Search ()
{
	Entity->PlaySound (CHAN_VOICE, SoundSearch, 1, ATTN_IDLE, 0);

	if (!Entity->gameEntity->oldenemy)
	{
		CMonsterEntity *ent = FindDeadMonster();
		if (ent)
		{
			Entity->gameEntity->oldenemy = Entity->gameEntity->enemy;
			Entity->gameEntity->enemy = ent->gameEntity;
			ent->Monster->Healer = Entity;
			AIFlags |= AI_MEDIC;
			FoundTarget ();
		}
	}
}

void CMedic::Sight ()
{
	Entity->PlaySound (CHAN_VOICE, SoundSight, 1, ATTN_NORM, 0);
}

CFrame MedicFramesStand [] =
{
	CFrame (&CMonster::AI_Stand, 0, &CMonster::Idle),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
};
CAnim MedicMoveStand (FRAME_wait1, FRAME_wait90, MedicFramesStand);

void CMedic::Stand ()
{
	CurrentMove = &MedicMoveStand;
}

CFrame MedicFramesWalk [] =
{
	CFrame (&CMonster::AI_Walk, 6.2f),
	CFrame (&CMonster::AI_Walk, 18.1f),
	CFrame (&CMonster::AI_Walk, 1),
	CFrame (&CMonster::AI_Walk, 9),
	CFrame (&CMonster::AI_Walk, 10),
	CFrame (&CMonster::AI_Walk, 9),
	CFrame (&CMonster::AI_Walk, 11),
	CFrame (&CMonster::AI_Walk, 11.6f),
	CFrame (&CMonster::AI_Walk, 2),
	CFrame (&CMonster::AI_Walk, 9.9f),
	CFrame (&CMonster::AI_Walk, 14),
	CFrame (&CMonster::AI_Walk, 9.3f)
};
CAnim MedicMoveWalk (FRAME_walk1, FRAME_walk12, MedicFramesWalk);

void CMedic::Walk ()
{
	CurrentMove = &MedicMoveWalk;
}

CFrame MedicFramesRun [] =
{
	CFrame (&CMonster::AI_Run, 18),
	CFrame (&CMonster::AI_Run, 22.5f),
	CFrame (&CMonster::AI_Run, 25.4f),
	CFrame (&CMonster::AI_Run, 23.4f),
	CFrame (&CMonster::AI_Run, 24),
	CFrame (&CMonster::AI_Run, 35.6f)
};
CAnim MedicMoveRun (FRAME_run1, FRAME_run6, MedicFramesRun);

void CMedic::Run ()
{
	if (!(AIFlags & AI_MEDIC))
	{
		CMonsterEntity *ent = FindDeadMonster();
		if (ent)
		{
			Entity->gameEntity->oldenemy = Entity->gameEntity->enemy;
			Entity->gameEntity->enemy = ent->gameEntity;
			ent->Monster->Healer = Entity;
			AIFlags |= AI_MEDIC;
			FoundTarget ();
			return;
		}
	}

	CurrentMove = (AIFlags & AI_STAND_GROUND) ? &MedicMoveStand : &MedicMoveRun;
}


CFrame MedicFramesPain1 [] =
{
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0)
};
CAnim MedicMovePain1 (FRAME_paina1, FRAME_paina8, MedicFramesPain1, &CMonster::Run);

CFrame MedicFramesPain2 [] =
{
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0)
};
CAnim MedicMovePain2 (FRAME_painb1, FRAME_painb15, MedicFramesPain2, &CMonster::Run);

void CMedic::Pain(CBaseEntity *other, float kick, int damage)
{
	if (Entity->gameEntity->health < (Entity->gameEntity->max_health / 2))
		Entity->State.SetSkinNum (1);

	if (level.framenum < Entity->gameEntity->pain_debounce_time)
		return;

	Entity->gameEntity->pain_debounce_time = level.framenum + 30;

	if (skill->Integer() == 3)
		return;		// no pain anims in nightmare

	float r = random();
	CurrentMove = (r < 0.5) ? &MedicMovePain1 : &MedicMovePain2;
	Entity->PlaySound (CHAN_VOICE, (r < 0.5) ? SoundPain1 : SoundPain2, 1, ATTN_NORM, 0);
}

void CMedic::FireBlaster ()
{
	vec3f	start;
	vec3f	forward, right;
	vec3f	end;
	vec3f	dir;
	int		effect = 0;

	switch (Entity->State.GetFrame())
	{
	case FRAME_attack9:
	case FRAME_attack12:
		effect = EF_BLASTER;
		break;
	case FRAME_attack19:
	case FRAME_attack22:
	case FRAME_attack25:
	case FRAME_attack28:
		effect = EF_HYPERBLASTER;
		break;
	default:
		break;
	};

	Entity->State.GetAngles().ToVectors(&forward, &right, NULL);
	G_ProjectSource (Entity->State.GetOrigin(), dumb_and_hacky_monster_MuzzFlashOffset[MZ2_MEDIC_BLASTER_1], forward, right, start);

	end = vec3f(Entity->gameEntity->enemy->state.origin);
	end.Z += Entity->gameEntity->enemy->viewheight;
	dir = end - start;

	MonsterFireBlaster (start, dir, 2, 1000, MZ2_MEDIC_BLASTER_1, effect);
}

void CMedic::Dead ()
{
	Entity->SetMins (vec3f(-16, -16, -24));
	Entity->SetMaxs (vec3f(16, 16, -8));
	Entity->PhysicsType = PHYSICS_TOSS;
	Entity->SetSvFlags (Entity->GetSvFlags() | SVF_DEADMONSTER);
	Entity->NextThink = 0;
	Entity->Link ();
}

CFrame MedicFramesDeath [] =
{
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0)
};
CAnim MedicMoveDeath (FRAME_death1, FRAME_death30, MedicFramesDeath, ConvertDerivedFunction(&CMedic::Dead));

void CMedic::Die (CBaseEntity *inflictor, CBaseEntity *attacker, int damage, vec3_t point)
{
	// if we had a pending patient, free him up for another medic
	if ((Entity->gameEntity->enemy) &&
		(Entity->gameEntity->enemy->Entity->EntityFlags & ENT_MONSTER ) && 
		((dynamic_cast<CMonsterEntity*>(Entity->gameEntity->enemy->Entity))->Monster->Healer == Entity))
		(dynamic_cast<CMonsterEntity*>(Entity->gameEntity->enemy->Entity))->Monster->Healer = NULL;

// check for gib
	if (Entity->gameEntity->health <= Entity->gameEntity->gib_health)
	{
		Entity->PlaySound (CHAN_VOICE, SoundIndex ("misc/udeath.wav"), 1, ATTN_NORM, 0);
		for (int n= 0; n < 2; n++)
			CGibEntity::Spawn (Entity, gMedia.Gib_Bone[0], damage, GIB_ORGANIC);
		for (int n= 0; n < 4; n++)
			CGibEntity::Spawn (Entity, gMedia.Gib_SmallMeat, damage, GIB_ORGANIC);
		Entity->ThrowHead(gMedia.Gib_Head[1], damage, GIB_ORGANIC);
		Entity->gameEntity->deadflag = DEAD_DEAD;
		return;
	}

	if (Entity->gameEntity->deadflag == DEAD_DEAD)
		return;

// regular death
	Entity->PlaySound (CHAN_VOICE, SoundDie, 1, ATTN_NORM, 0);
	Entity->gameEntity->deadflag = DEAD_DEAD;
	Entity->gameEntity->takedamage = true;

	CurrentMove = &MedicMoveDeath;
}

CFrame MedicFramesAttackHyperBlaster [] =
{
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0,	ConvertDerivedFunction(&CMedic::FireBlaster)),
	CFrame (&CMonster::AI_Charge, 0,	ConvertDerivedFunction(&CMedic::FireBlaster)),
	CFrame (&CMonster::AI_Charge, 0,	ConvertDerivedFunction(&CMedic::FireBlaster)),
	CFrame (&CMonster::AI_Charge, 0,	ConvertDerivedFunction(&CMedic::FireBlaster)),
	CFrame (&CMonster::AI_Charge, 0,	ConvertDerivedFunction(&CMedic::FireBlaster)),
	CFrame (&CMonster::AI_Charge, 0,	ConvertDerivedFunction(&CMedic::FireBlaster)),
	CFrame (&CMonster::AI_Charge, 0,	ConvertDerivedFunction(&CMedic::FireBlaster)),
	CFrame (&CMonster::AI_Charge, 0,	ConvertDerivedFunction(&CMedic::FireBlaster)),
	CFrame (&CMonster::AI_Charge, 0,	ConvertDerivedFunction(&CMedic::FireBlaster)),
	CFrame (&CMonster::AI_Charge, 0,	ConvertDerivedFunction(&CMedic::FireBlaster)),
	CFrame (&CMonster::AI_Charge, 0,	ConvertDerivedFunction(&CMedic::FireBlaster)),
	CFrame (&CMonster::AI_Charge, 0,	ConvertDerivedFunction(&CMedic::FireBlaster))
};
CAnim MedicMoveAttackHyperBlaster (FRAME_attack15, FRAME_attack30, MedicFramesAttackHyperBlaster, &CMonster::Run);

void CMedic::ContinueFiring ()
{
	if (visible (Entity->gameEntity, Entity->gameEntity->enemy) && (random() <= 0.95))
		CurrentMove = &MedicMoveAttackHyperBlaster;
}

CFrame MedicFramesAttackBlaster [] =
{
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 5),
	CFrame (&CMonster::AI_Charge, 5),
	CFrame (&CMonster::AI_Charge, 3),
	CFrame (&CMonster::AI_Charge, 2),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0,	ConvertDerivedFunction(&CMedic::FireBlaster)),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0,	ConvertDerivedFunction(&CMedic::FireBlaster)),	
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0,	ConvertDerivedFunction(&CMedic::ContinueFiring))	// Change to medic_continue... Else, go to frame 32
};
CAnim MedicMoveAttackBlaster (FRAME_attack1, FRAME_attack14, MedicFramesAttackBlaster, &CMonster::Run);

void CMedic::HookLaunch ()
{
	Entity->PlaySound (CHAN_WEAPON, SoundHookLaunch, 1, ATTN_NORM, 0);
}

void ED_CallSpawn (edict_t *ent);

static vec3f	MedicCableOffsets[] =
{
	vec3f(45.0f,  -9.2f, 15.5f),
	vec3f(48.4f,  -9.7f, 15.2f),
	vec3f(47.8f,  -9.8f, 15.8f),
	vec3f(47.3f,  -9.3f, 14.3f),
	vec3f(45.4f, -10.1f, 13.1f),
	vec3f(41.9f, -12.7f, 12.0f),
	vec3f(37.8f, -15.8f, 11.2f),
	vec3f(34.3f, -18.4f, 10.7f),
	vec3f(32.7f, -19.7f, 10.4f),
	vec3f(32.7f, -19.7f, 10.4f)
};

void CMedic::CableAttack ()
{
	vec3f	offset, start, end, f, r;
	CTrace	tr;
	vec3f	dir, angles;
	float	distance;

	if (!Entity->gameEntity->enemy->inUse)
		return;

	Entity->State.GetAngles().ToVectors (&f, &r, NULL);
	offset = MedicCableOffsets[Entity->State.GetFrame() - FRAME_attack42];
	G_ProjectSource (Entity->State.GetOrigin(), offset, f, r, start);

	// check for max distance
	dir = start - vec3f(Entity->gameEntity->enemy->state.origin);
	distance = dir.Length();
	if (distance > 256)
		return;

	// check for min/max pitch
	angles = dir.ToAngles();
	if (angles.X < -180)
		angles.X += 360;
	if (fabs(angles.X) > 45)
		return;

	tr = CTrace (start, vec3f(Entity->gameEntity->enemy->state.origin), Entity->gameEntity, CONTENTS_MASK_SHOT);
	if (tr.fraction != 1.0 && tr.ent != Entity->gameEntity->enemy)
		return;

	CMonsterEntity *Monster;
	switch (Entity->State.GetFrame())
	{
	case FRAME_attack43:
		Entity->PlaySound (CHAN_AUTO, SoundHookHit);
		(dynamic_cast<CMonsterEntity*>(Entity->gameEntity->enemy->Entity))->Monster->AIFlags |= AI_RESURRECTING;
		break;
	case FRAME_attack50:
		Entity->gameEntity->enemy->spawnflags = 0;
		(dynamic_cast<CMonsterEntity*>(Entity->gameEntity->enemy->Entity))->Monster->AIFlags = 0;
		Entity->gameEntity->enemy->target = NULL;
		Entity->gameEntity->enemy->targetname = NULL;
		Entity->gameEntity->enemy->combattarget = NULL;
		Entity->gameEntity->enemy->deathtarget = NULL;


		Monster = (dynamic_cast<CMonsterEntity*>(Entity->gameEntity->enemy->Entity));
		//Entity->gameEntity->enemy->owner = Entity->gameEntity;
		Monster->Monster->Healer = Entity;
		//ED_CallSpawn (Entity->gameEntity->enemy);
		Monster->Monster->Spawn ();
		Monster->Monster->Healer = NULL;
		//Entity->gameEntity->enemy->owner = NULL;
		Monster->NextThink = level.framenum;
		Monster->Think ();
		Monster->Monster->AIFlags |= AI_RESURRECTING;
		Monster->gameEntity->enemy = NULL;
		if (Entity->gameEntity->oldenemy && Entity->gameEntity->oldenemy->client)
		{
			Monster->gameEntity->enemy = Entity->gameEntity->oldenemy;
			Monster->Monster->FoundTarget ();
		}
		break;
	case FRAME_attack44:
		Entity->PlaySound (CHAN_WEAPON, SoundHookHeal, 1, ATTN_NORM, 0);
	default:
		break;
	}
	// adjust start for beam origin being in middle of a segment
	start = start.MultiplyAngles (8, f);

	// adjust end z for end spot since the monster is currently dead
	end = vec3f (Entity->gameEntity->enemy->state.origin);
	end.Z = Entity->gameEntity->enemy->absMin[2] + Entity->gameEntity->enemy->size[2] / 2;

	CTempEnt_Trails::FleshCable (start, end, Entity->State.GetNumber());
}

void CMedic::HookRetract ()
{
	Entity->PlaySound (CHAN_WEAPON, SoundHookRetract, 1, ATTN_NORM, 0);
	(dynamic_cast<CMonsterEntity*>(Entity->gameEntity->enemy->Entity))->Monster->AIFlags &= ~AI_RESURRECTING;
}

CFrame MedicFramesAttackCable [] =
{
	CFrame (&CMonster::AI_Move, 2),
	CFrame (&CMonster::AI_Move, 3),
	CFrame (&CMonster::AI_Move, 5),
	CFrame (&CMonster::AI_Move, 4.4f),
	CFrame (&CMonster::AI_Charge, 4.7f),
	CFrame (&CMonster::AI_Charge, 5),
	CFrame (&CMonster::AI_Charge, 6),
	CFrame (&CMonster::AI_Charge, 4),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Move, 0,		ConvertDerivedFunction(&CMedic::HookLaunch)),
	CFrame (&CMonster::AI_Move, 0,		ConvertDerivedFunction(&CMedic::CableAttack)),
	CFrame (&CMonster::AI_Move, 0,		ConvertDerivedFunction(&CMedic::CableAttack)),
	CFrame (&CMonster::AI_Move, 0,		ConvertDerivedFunction(&CMedic::CableAttack)),
	CFrame (&CMonster::AI_Move, 0,		ConvertDerivedFunction(&CMedic::CableAttack)),
	CFrame (&CMonster::AI_Move, 0,		ConvertDerivedFunction(&CMedic::CableAttack)),
	CFrame (&CMonster::AI_Move, 0,		ConvertDerivedFunction(&CMedic::CableAttack)),
	CFrame (&CMonster::AI_Move, 0,		ConvertDerivedFunction(&CMedic::CableAttack)),
	CFrame (&CMonster::AI_Move, 0,		ConvertDerivedFunction(&CMedic::CableAttack)),
	CFrame (&CMonster::AI_Move, 0,		ConvertDerivedFunction(&CMedic::CableAttack)),
	CFrame (&CMonster::AI_Move, -15,	ConvertDerivedFunction(&CMedic::HookRetract)),
	CFrame (&CMonster::AI_Move, -1.5f),
	CFrame (&CMonster::AI_Move, -1.2f),
	CFrame (&CMonster::AI_Move, -3),
	CFrame (&CMonster::AI_Move, -2),
	CFrame (&CMonster::AI_Move, 0.3f),
	CFrame (&CMonster::AI_Move, 0.7f),
	CFrame (&CMonster::AI_Move, 1.2f),
	CFrame (&CMonster::AI_Move, 1.3f)
};
CAnim MedicMoveAttackCable (FRAME_attack33, FRAME_attack60, MedicFramesAttackCable, &CMonster::Run);

void CMedic::Attack()
{
	CurrentMove = (AIFlags & AI_MEDIC) ? &MedicMoveAttackCable : &MedicMoveAttackBlaster;
}

bool CMedic::CheckAttack ()
{
	if (AIFlags & AI_MEDIC)
	{
		Attack();
		return true;
	}

	return CMonster::CheckAttack ();
}

CFrame MedicFramesDuck [] =
{
	CFrame (&CMonster::AI_Move, -1),
	CFrame (&CMonster::AI_Move, -1),
	CFrame (&CMonster::AI_Move, -1, &CMonster::DuckDown),
	CFrame (&CMonster::AI_Move, -1,	&CMonster::DuckHold),
	CFrame (&CMonster::AI_Move, -1),
	CFrame (&CMonster::AI_Move, -1),
	CFrame (&CMonster::AI_Move, -1
#ifndef MONSTER_USE_ROGUE_AI
	,	&CMonster::DuckUp // in Rogue AI, the UP is down
#else
	),
#endif
	CFrame (&CMonster::AI_Move, -1),
	CFrame (&CMonster::AI_Move, -1),
	CFrame (&CMonster::AI_Move, -1),
	CFrame (&CMonster::AI_Move, -1),
	CFrame (&CMonster::AI_Move, -1),
	CFrame (&CMonster::AI_Move, -1),
	CFrame (&CMonster::AI_Move, -1
#ifdef MONSTER_USE_ROGUE_AI
	, &CMonster::UnDuck
#endif
	),
	CFrame (&CMonster::AI_Move, -1),
	CFrame (&CMonster::AI_Move, -1)
};
CAnim MedicMoveDuck (FRAME_duck1, FRAME_duck16, MedicFramesDuck, &CMonster::Run);

#ifndef MONSTER_USE_ROGUE_AI
void CMedic::DuckDown ()
{
	if (AIFlags & AI_DUCKED)
		return;
	AIFlags |= AI_DUCKED;
	vec3f maxs = Entity->GetMaxs();
	maxs.Z -= 32;
	Entity->SetMaxs(maxs);
	PauseTime = level.time + 1;
	Entity->Link();
}

void CMedic::DuckHold ()
{
	if (level.time >= PauseTime)
		AIFlags &= ~AI_HOLD_FRAME;
	else
		AIFlags |= AI_HOLD_FRAME;
}

void CMedic::DuckUp ()
{
	AIFlags &= ~AI_DUCKED;
	vec3f maxs = Entity->GetMaxs();
	maxs.Z += 32;
	Entity->SetMaxs(maxs);
	Entity->Link();
}

void CMedic::Dodge (edict_t *attacker, float eta)
{
	if (random() > 0.25)
		return;

	if (!Entity->gameEntity->enemy)
		Entity->gameEntity->enemy = attacker;

	CurrentMove = &MedicMoveDuck;
}
#else
void CMedic::Duck (float eta)
{
//	don't dodge if you're healing
	if (AIFlags & AI_MEDIC)
		return;

	if ((CurrentMove == &MedicMoveAttackHyperBlaster) ||
		(CurrentMove == &MedicMoveAttackCable) ||
		(CurrentMove == &MedicMoveAttackBlaster))
	{
		// he ignores skill
		AIFlags &= ~AI_DUCKED;
		return;
	}

	DuckWaitTime = level.framenum + (skill->Integer() == 0) ? ((eta + 1) * 10) : ((eta + (0.1 * (3 - skill->Integer()))) * 10);

	// has to be done immediately otherwise he can get stuck
	DuckDown();

	NextFrame = FRAME_duck1;
	CurrentMove = &MedicMoveDuck;
}

void CMedic::SideStep ()
{
	if ((CurrentMove == &MedicMoveAttackHyperBlaster) ||
		(CurrentMove == &MedicMoveAttackCable) ||
		(CurrentMove == &MedicMoveAttackBlaster))
	{
		// if we're shooting, and not on easy, don't dodge
		if (skill->Integer())
		{
			AIFlags &= ~AI_DODGING;
			return;
		}
	}

	if (CurrentMove != &MedicMoveRun)
		CurrentMove = &MedicMoveRun;
}
#endif

/*QUAKED monster_medic (1 .5 0) (-16 -16 -24) (16 16 32) Ambush Trigger_Spawn Sight
*/
void CMedic::Spawn ()
{
	SoundIdle1 = SoundIndex ("medic/idle.wav");
	SoundPain1 = SoundIndex ("medic/medpain1.wav");
	SoundPain2 = SoundIndex ("medic/medpain2.wav");
	SoundDie = SoundIndex ("medic/meddeth1.wav");
	SoundSight = SoundIndex ("medic/medsght1.wav");
	SoundSearch = SoundIndex ("medic/medsrch1.wav");
	SoundHookLaunch = SoundIndex ("medic/medatck2.wav");
	SoundHookHit = SoundIndex ("medic/medatck3.wav");
	SoundHookHeal = SoundIndex ("medic/medatck4.wav");
	SoundHookRetract = SoundIndex ("medic/medatck5.wav");

	SoundIndex ("medic/medatck1.wav");

	Entity->SetSolid (SOLID_BBOX);
	Entity->State.SetModelIndex(ModelIndex ("models/monsters/medic/tris.md2"));
	Entity->SetMins (vec3f(-24, -24, -24));
	Entity->SetMaxs (vec3f(24, 24, 32));

	Entity->gameEntity->health = 300;
	Entity->gameEntity->gib_health = -130;
	Entity->gameEntity->mass = 400;

	MonsterFlags |= (MF_HAS_DODGE | MF_HAS_ATTACK | MF_HAS_SIGHT | MF_HAS_IDLE | MF_HAS_SEARCH
#ifdef MONSTER_USE_ROGUE_AI
		| MF_HAS_DUCK | MF_HAS_UNDUCK | MF_HAS_SIDESTEP
#endif
		);

	Entity->Link ();
	CurrentMove = &MedicMoveStand;

	WalkMonsterStart ();
}

LINK_MONSTER_CLASSNAME_TO_CLASS ("monster_medic", CMedic);