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
// cc_bitch.cpp
// Iron Maiden
//

#include "cc_local.h"
#include "m_chick.h"
#include "cc_bitch.h"

CMaiden Monster_Bitch;

CMaiden::CMaiden ()
{
	Classname = "monster_bitch";
	Scale = MODEL_SCALE;
}

void CMaiden::Moan ()
{
	Entity->PlaySound (CHAN_VOICE, (random() < 0.5) ? SoundIdle1 : SoundIdle2, 1, ATTN_IDLE, 0);
}

CFrame ChickFramesFidget [] =
{
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0,  ConvertDerivedFunction(&CMaiden::Moan)),
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
	CFrame (&CMonster::AI_Stand, 0)
};
CAnim ChickMoveFidget (FRAME_stand201, FRAME_stand230, ChickFramesFidget, ConvertDerivedFunction(&CMaiden::Stand));

void CMaiden::Idle ()
{
	if (AIFlags & AI_STAND_GROUND)
		return;
	if (random() <= 0.3)
		CurrentMove = &ChickMoveFidget;
}

CFrame ChickFramesStand [] =
{
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
CAnim ChickMoveStand (FRAME_stand101, FRAME_stand130, ChickFramesStand);

void CMaiden::Stand ()
{
	CurrentMove = &ChickMoveStand;
}

CFrame ChickFramesStartRun [] =
{
	CFrame (&CMonster::AI_Run, 1),
	CFrame (&CMonster::AI_Run, 0),
	CFrame (&CMonster::AI_Run, 0),
	CFrame (&CMonster::AI_Run, -1), 
	CFrame (&CMonster::AI_Run, -1), 
	CFrame (&CMonster::AI_Run, 0),
	CFrame (&CMonster::AI_Run, 1),
	CFrame (&CMonster::AI_Run, 3),
	CFrame (&CMonster::AI_Run, 6),
	CFrame (&CMonster::AI_Run, 3)
};
CAnim ChickMoveStartRun (FRAME_walk01, FRAME_walk10, ChickFramesStartRun, ConvertDerivedFunction(&CMaiden::Run));

CFrame ChickFramesRun [] =
{
	CFrame (&CMonster::AI_Run, 6),
	CFrame (&CMonster::AI_Run, 8),
	CFrame (&CMonster::AI_Run, 13),
#ifdef MONSTER_USE_ROGUE_AI
	CFrame (&CMonster::AI_Run, 5, &CMonster::DoneDodge), // Make sure to clear the dodge bit. FIXME: needed?
#else
	CFrame (&CMonster::AI_Run, 5),
#endif
	CFrame (&CMonster::AI_Run, 7),
	CFrame (&CMonster::AI_Run, 4),
	CFrame (&CMonster::AI_Run, 11),
	CFrame (&CMonster::AI_Run, 5),
	CFrame (&CMonster::AI_Run, 9),
	CFrame (&CMonster::AI_Run, 7)
};
CAnim ChickMoveRun (FRAME_walk11, FRAME_walk20, ChickFramesRun);

CFrame ChickFramesWalk [] =
{
	CFrame (&CMonster::AI_Walk, 6),
	CFrame (&CMonster::AI_Walk, 8),
	CFrame (&CMonster::AI_Walk, 13),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 7),
	CFrame (&CMonster::AI_Walk, 4),
	CFrame (&CMonster::AI_Walk, 11),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 9),
	CFrame (&CMonster::AI_Walk, 7)
};
CAnim ChickMoveWalk (FRAME_walk11, FRAME_walk20, ChickFramesWalk);

void CMaiden::Walk ()
{
	CurrentMove = &ChickMoveWalk;
}

void CMaiden::Run ()
{
#ifdef MONSTER_USE_ROGUE_AI
	DoneDodge();
#endif

	if (AIFlags & AI_STAND_GROUND)
	{
		CurrentMove = &ChickMoveStand;
		return;
	}

	if (CurrentMove == &ChickMoveWalk ||
		CurrentMove == &ChickMoveStartRun)
		CurrentMove = &ChickMoveRun;
	else
		CurrentMove = &ChickMoveStartRun;
}

CFrame ChickFramesPain1 [] =
{
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0)
};
CAnim ChickMovePain1 (FRAME_pain101, FRAME_pain105, ChickFramesPain1, ConvertDerivedFunction(&CMaiden::Run));

CFrame ChickFramesPain2 [] =
{
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0)
};
CAnim ChickMovePain2 (FRAME_pain201, FRAME_pain205, ChickFramesPain2, ConvertDerivedFunction(&CMaiden::Run));

CFrame ChickFramesPain3 [] =
{
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, -6),
	CFrame (&CMonster::AI_Move, 3),
	CFrame (&CMonster::AI_Move, 11),
	CFrame (&CMonster::AI_Move, 3),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 4),
	CFrame (&CMonster::AI_Move, 1),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, -3),
	CFrame (&CMonster::AI_Move, -4),
	CFrame (&CMonster::AI_Move, 5),
	CFrame (&CMonster::AI_Move, 7),
	CFrame (&CMonster::AI_Move, -2),
	CFrame (&CMonster::AI_Move, 3),
	CFrame (&CMonster::AI_Move, -5),
	CFrame (&CMonster::AI_Move, -2),
	CFrame (&CMonster::AI_Move, -8),
	CFrame (&CMonster::AI_Move, 2)
};
CAnim ChickMovePain3 (FRAME_pain301, FRAME_pain321, ChickFramesPain3, ConvertDerivedFunction(&CMaiden::Run));

void CMaiden::Pain (CBaseEntity *other, float kick, int damage)
{
	if (Entity->gameEntity->health < (Entity->gameEntity->max_health / 2))
		Entity->State.SetSkinNum(1);

	if (level.framenum < Entity->gameEntity->pain_debounce_time)
		return;

	Entity->gameEntity->pain_debounce_time = level.framenum + 30;

	int r = rand()%3;
	switch (r)
	{
	case 0:
	default:
		Entity->PlaySound (CHAN_VOICE, SoundPain1);
		break;
	case 1:
		Entity->PlaySound (CHAN_VOICE, SoundPain2);
		break;
	case 2:
		Entity->PlaySound (CHAN_VOICE, SoundPain3);
		break;
	}

	if (skill->Integer() == 3)
		return;		// no pain anims in nightmare

#ifdef MONSTER_USE_ROGUE_AI
	// PMM - clear this from blindfire
	AIFlags &= ~AI_MANUAL_STEERING;

	// PMM - clear duck flag
	if (AIFlags & AI_DUCKED)
		UnDuck();
#endif

	if (damage <= 10)
		CurrentMove = &ChickMovePain1;
	else if (damage <= 25)
		CurrentMove = &ChickMovePain2;
	else
		CurrentMove = &ChickMovePain3;
}

void CMaiden::Dead ()
{
	Entity->SetMins (vec3f(-16, -16, 0));
	Entity->SetMaxs (vec3f(16, 16, 16));
	Entity->TossPhysics = true;
	Entity->SetSvFlags (Entity->GetSvFlags() | SVF_DEADMONSTER);
	Entity->NextThink = 0;
	Entity->Link ();
}

CFrame ChickFramesDeath2 [] =
{
	CFrame (&CMonster::AI_Move, -6),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, -1),
	CFrame (&CMonster::AI_Move, -5),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, -1),
	CFrame (&CMonster::AI_Move, -2),
	CFrame (&CMonster::AI_Move, 1),
	CFrame (&CMonster::AI_Move, 10),
	CFrame (&CMonster::AI_Move, 2),
	CFrame (&CMonster::AI_Move, 3),
	CFrame (&CMonster::AI_Move, 1),
	CFrame (&CMonster::AI_Move, 2),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 3),
	CFrame (&CMonster::AI_Move, 3),
	CFrame (&CMonster::AI_Move, 1),
	CFrame (&CMonster::AI_Move, -3),
	CFrame (&CMonster::AI_Move, -5),
	CFrame (&CMonster::AI_Move, 4),
	CFrame (&CMonster::AI_Move, 15),
	CFrame (&CMonster::AI_Move, 14),
	CFrame (&CMonster::AI_Move, 1)
};
CAnim ChickMoveDeath2 (FRAME_death201, FRAME_death223, ChickFramesDeath2, ConvertDerivedFunction(&CMaiden::Dead));

CFrame ChickFramesDeath1 [] =
{
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, -7),
	CFrame (&CMonster::AI_Move, 4),
	CFrame (&CMonster::AI_Move, 11),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0)
};
CAnim ChickMoveDeath1 (FRAME_death101, FRAME_death112, ChickFramesDeath1, ConvertDerivedFunction(&CMaiden::Dead));

void CMaiden::Die (CBaseEntity *inflictor, CBaseEntity *attacker, int damage, vec3_t point)
{
	int		n;

// check for gib
	if (Entity->gameEntity->health <= Entity->gameEntity->gib_health)
	{
		Entity->PlaySound (CHAN_VOICE, SoundIndex ("misc/udeath.wav"));
		for (n= 0; n < 2; n++)
			CGibEntity::Spawn (Entity, gMedia.Gib_Bone[0], damage, GIB_ORGANIC);
		for (n= 0; n < 4; n++)
			CGibEntity::Spawn (Entity, gMedia.Gib_SmallMeat, damage, GIB_ORGANIC);
		Entity->ThrowHead (gMedia.Gib_Head[1], damage, GIB_ORGANIC);
		Entity->gameEntity->deadflag = DEAD_DEAD;
		return;
	}

	if (Entity->gameEntity->deadflag == DEAD_DEAD)
		return;

// regular death
	Entity->gameEntity->deadflag = DEAD_DEAD;
	Entity->gameEntity->takedamage = true;

	n = rand() % 2;
	CurrentMove = (n == 0) ? &ChickMoveDeath1 : &ChickMoveDeath2;
	Entity->PlaySound (CHAN_VOICE, (n == 0) ? SoundDeath1 : SoundDeath2);
}

#ifndef MONSTER_USE_ROGUE_AI
void CMaiden::DuckDown ()
{
	if (AIFlags & AI_DUCKED)
		return;
	AIFlags |= AI_DUCKED;
	Entity->maxs[2] -= 32;
	Entity->gameEntity->takedamage = true;
	PauseTime = level.framenum + 10;
	Entity->Link ();
}

void CMaiden::DuckHold ()
{
	if (level.framenum >= PauseTime)
		AIFlags &= ~AI_HOLD_FRAME;
	else
		AIFlags |= AI_HOLD_FRAME;
}

void CMaiden::DuckUp ()
{
	AIFlags &= ~AI_DUCKED;
	Entity->maxs[2] += 32;
	Entity->gameEntity->takedamage = true;
	Entity->Link ();
}

CFrame ChickFramesDuck [] =
{
	CFrame (&CMonster::AI_Move, 0, ConvertDerivedFunction(&CMaiden::DuckDown)),
	CFrame (&CMonster::AI_Move, 1),
	CFrame (&CMonster::AI_Move, 4, ConvertDerivedFunction(&CMaiden::DuckHold)),
	CFrame (&CMonster::AI_Move, -4),
	CFrame (&CMonster::AI_Move, -5, ConvertDerivedFunction(&CMaiden::DuckUp)),
	CFrame (&CMonster::AI_Move, 3),
	CFrame (&CMonster::AI_Move, 1)
};
#else
CFrame ChickFramesDuck [] =
{
	CFrame (&CMonster::AI_Move, 0, &CMonster::DuckDown),
	CFrame (&CMonster::AI_Move, 1),
	CFrame (&CMonster::AI_Move, 4, &CMonster::DuckHold),
	CFrame (&CMonster::AI_Move, -4),
	CFrame (&CMonster::AI_Move, -5, &CMonster::UnDuck),
	CFrame (&CMonster::AI_Move, 3),
	CFrame (&CMonster::AI_Move, 1)
};
#endif

CAnim ChickMoveDuck (FRAME_duck01, FRAME_duck07, ChickFramesDuck, ConvertDerivedFunction(&CMaiden::Run));

CFrame ChickFramesStartAttack1 [] =
{
	CFrame (&CMonster::AI_Charge, 0,	ConvertDerivedFunction(&CMaiden::PreAttack)),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 4),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, -3),
	CFrame (&CMonster::AI_Charge, 3),
	CFrame (&CMonster::AI_Charge, 5),
	CFrame (&CMonster::AI_Charge, 7),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0, ConvertDerivedFunction(&CMaiden::Attack1))
};
CAnim ChickMoveStartAttack1 (FRAME_attak101, FRAME_attak113, ChickFramesStartAttack1);


CFrame ChickFramesAttack1 [] =
{
	CFrame (&CMonster::AI_Charge, 19, ConvertDerivedFunction(&CMaiden::Rocket)),
	CFrame (&CMonster::AI_Charge, -6),
	CFrame (&CMonster::AI_Charge, -5),
	CFrame (&CMonster::AI_Charge, -2),
	CFrame (&CMonster::AI_Charge, -7),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 1),
	CFrame (&CMonster::AI_Charge, 10, ConvertDerivedFunction(&CMaiden::Reload)),
	CFrame (&CMonster::AI_Charge, 4),
	CFrame (&CMonster::AI_Charge, 5),
	CFrame (&CMonster::AI_Charge, 6),
	CFrame (&CMonster::AI_Charge, 6),
	CFrame (&CMonster::AI_Charge, 4),
	CFrame (&CMonster::AI_Charge, 3, ConvertDerivedFunction(&CMaiden::ReRocket))
};
CAnim ChickMoveAttack1 (FRAME_attak114, FRAME_attak127, ChickFramesAttack1);

CFrame ChickFramesEndAttack1 [] =
{
	CFrame (&CMonster::AI_Charge, -3),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, -6),
	CFrame (&CMonster::AI_Charge, -4),
	CFrame (&CMonster::AI_Charge, -2)
};
CAnim ChickMoveEndAttack1 (FRAME_attak128, FRAME_attak132, ChickFramesEndAttack1, ConvertDerivedFunction(&CMaiden::Run));

#ifdef MONSTER_USE_ROGUE_AI
void CMaiden::Duck (float eta)
{
	if ((CurrentMove == &ChickMoveStartAttack1) ||
		(CurrentMove == &ChickMoveAttack1))
	{
		// if we're shooting, and not on easy, don't dodge
		if (skill->Integer())
		{
			AIFlags &= ~AI_DUCKED;
			return;
		}
	}

	if (!skill->Integer())
		// PMM - stupid dodge
		DuckWaitTime = level.framenum + ((eta + 1 * 10));
	else
		DuckWaitTime = level.framenum + ((eta + (0.1 * (3 - skill->Integer())) * 10));

	// has to be done immediately otherwise she can get stuck
	DuckDown();

	NextFrame = FRAME_duck01;
	CurrentMove = &ChickMoveDuck;
	return;
}

void CMaiden::SideStep ()
{
	if ((CurrentMove == &ChickMoveStartAttack1) ||
		(CurrentMove == &ChickMoveAttack1))
	{
		// if we're shooting, and not on easy, don't dodge
		if (skill->Integer())
		{
			AIFlags &= ~AI_DODGING;
			return;
		}
	}

	if (CurrentMove != &ChickMoveRun)
		CurrentMove = &ChickMoveRun;
}
#else
void CMaiden::Dodge (edict_t *attacker, float eta)
{
	if (random() > 0.25)
		return;

	if (!Entity->gameEntity->enemy)
		Entity->gameEntity->enemy = attacker;

	CurrentMove = &ChickMoveDuck;
}
#endif

void CMaiden::Slash ()
{
	vec3_t	aim = {MELEE_DISTANCE, Entity->GetMins().X, 10};
	Entity->PlaySound (CHAN_WEAPON, SoundMeleeSwing);
	CMeleeWeapon::Fire (Entity, aim, (10 + (rand() %6)), 100);
}

void CMaiden::Rocket ()
{
#ifdef MONSTER_USE_ROGUE_AI
	vec3_t	forward, right;
	vec3_t	start;
	vec3_t	dir;
	vec3_t	vec;
	int		rocketSpeed;
	float	dist;
	vec3_t	target;
	bool blindfire = (AIFlags & AI_MANUAL_STEERING) ? true : false;

	vec3_t angles, origin;
	Entity->State.GetAngles(angles);
	Entity->State.GetOrigin(origin);
	Angles_Vectors (angles, forward, right, NULL);
	G_ProjectSource (origin, dumb_and_hacky_monster_MuzzFlashOffset[MZ2_CHICK_ROCKET_1], forward, right, start);

	rocketSpeed = 500 + (100 * skill->Integer());	// PGM rock & roll.... :)

	if (blindfire)
		Vec3Copy (BlindFireTarget, target);
	else
		Vec3Copy (Entity->gameEntity->enemy->state.origin, target);

	if (blindfire)
	{
		Vec3Copy (target, vec);
		Vec3Subtract (vec, start, dir);
	}
	// pmm
	// don't shoot at feet if they're above where i'm shooting from.
	else if(random() < 0.33 || (start[2] < Entity->gameEntity->enemy->absMin[2]))
	{
		Vec3Copy (target, vec);
		vec[2] += Entity->gameEntity->enemy->viewheight;
		Vec3Subtract (vec, start, dir);
	}
	else
	{
		Vec3Copy (target, vec);
		vec[2] = Entity->gameEntity->enemy->absMin[2];
		Vec3Subtract (vec, start, dir);
	}

	// Lead target  (not when blindfiring)
	// 20, 35, 50, 65 chance of leading
	if((!blindfire) && ((random() < (0.2 + ((3 - skill->Integer()) * 0.15)))))
	{
		float	time;

		dist = Vec3Length (dir);
		time = dist/rocketSpeed;
		Vec3MA(vec, time, Entity->gameEntity->enemy->velocity, vec);
		Vec3Subtract(vec, start, dir);
	}


	VectorNormalizeFastf (dir);

	// pmm blindfire doesn't check target (done in checkattack)
	// paranoia, make sure we're not shooting a target right next to us
	CTrace trace = CTrace(start, vec, Entity->gameEntity, CONTENTS_MASK_SHOT);
	if (blindfire)
	{
		// blindfire has different fail criteria for the trace
		if (!(trace.startSolid || trace.allSolid || (trace.fraction < 0.5)))
			MonsterFireRocket (start, dir, 50, rocketSpeed, MZ2_CHICK_ROCKET_1);
		else 
		{
			// geez, this is bad.  she's avoiding about 80% of her blindfires due to hitting things.
			// hunt around for a good shot
			// try shifting the target to the left a little (to help counter her large offset)
			Vec3Copy (target, vec);
			Vec3MA (vec, -10, right, vec);
			Vec3Subtract(vec, start, dir);
			VectorNormalizeFastf (dir);
			trace = CTrace(start, vec, Entity->gameEntity, CONTENTS_MASK_SHOT);
			if (!(trace.startSolid || trace.allSolid || (trace.fraction < 0.5)))
				MonsterFireRocket (start, dir, 50, rocketSpeed, MZ2_CHICK_ROCKET_1);
			else 
			{
				// ok, that failed.  try to the right
				Vec3Copy (target, vec);
				Vec3MA (vec, 10, right, vec);
				Vec3Subtract(vec, start, dir);
				VectorNormalizeFastf (dir);
				trace = CTrace(start, vec, Entity->gameEntity, CONTENTS_MASK_SHOT);
				if (!(trace.startSolid || trace.allSolid || (trace.fraction < 0.5)))
					MonsterFireRocket (start, dir, 50, rocketSpeed, MZ2_CHICK_ROCKET_1);
			}
		}
	}
	else
		MonsterFireRocket (start, dir, 50, rocketSpeed, MZ2_CHICK_ROCKET_1);
#else
	vec3_t	forward, right;
	vec3_t	start;
	vec3_t	dir;
	vec3_t	vec;

	Angles_Vectors (Entity->state.angles, forward, right, NULL);
	G_ProjectSource (Entity->state.origin, dumb_and_hacky_monster_MuzzFlashOffset[MZ2_CHICK_ROCKET_1], forward, right, start);

	Vec3Copy (Entity->gameEntity->enemy->state.origin, vec);
	vec[2] += Entity->gameEntity->enemy->viewheight;
	Vec3Subtract (vec, start, dir);
	VectorNormalizef (dir, dir);

	MonsterFireRocket (start, dir, 50, 500, MZ2_CHICK_ROCKET_1);
#endif
}	

void CMaiden::PreAttack ()
{
	Entity->PlaySound (CHAN_VOICE, SoundMissilePrelaunch);
}

void CMaiden::Reload ()
{
	Entity->PlaySound (CHAN_VOICE, SoundMissileReload);
}

void CMaiden::ReRocket()
{
#ifdef MONSTER_USE_ROGUE_AI
	if (AIFlags & AI_MANUAL_STEERING)
		AIFlags &= ~AI_MANUAL_STEERING;
	else
#endif
	if (Entity->gameEntity->enemy->health > 0)
	{
		if (range (Entity->gameEntity, Entity->gameEntity->enemy) > RANGE_MELEE && visible (Entity->gameEntity, Entity->gameEntity->enemy) && (random() <= (0.6 + (0.05*skill->Float()))))
		{
			CurrentMove = &ChickMoveAttack1;
			return;
		}
	}	
	CurrentMove = &ChickMoveEndAttack1;
}

void CMaiden::Attack1()
{
	CurrentMove = &ChickMoveAttack1;
}

CFrame ChickFramesSlash [] =
{
	CFrame (&CMonster::AI_Charge, 1),
	CFrame (&CMonster::AI_Charge, 7, ConvertDerivedFunction(&CMaiden::Slash)),
	CFrame (&CMonster::AI_Charge, -7),
	CFrame (&CMonster::AI_Charge, 1),
	CFrame (&CMonster::AI_Charge, -1),
	CFrame (&CMonster::AI_Charge, 1),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 1),
	CFrame (&CMonster::AI_Charge, -2, ConvertDerivedFunction(&CMaiden::ReSlash))
};
CAnim ChickMoveSlash (FRAME_attak204, FRAME_attak212, ChickFramesSlash);

CFrame ChickFramesEndSlash [] =
{
	CFrame (&CMonster::AI_Charge, -6),
	CFrame (&CMonster::AI_Charge, -1),
	CFrame (&CMonster::AI_Charge, -6),
	CFrame (&CMonster::AI_Charge, 0)
};
CAnim ChickMoveEndSlash (FRAME_attak213, FRAME_attak216, ChickFramesEndSlash, ConvertDerivedFunction(&CMaiden::Run));

void CMaiden::ReSlash()
{
	if (Entity->gameEntity->enemy->health > 0 && (range (Entity->gameEntity, Entity->gameEntity->enemy) == RANGE_MELEE) && (random() <= 0.9))
		CurrentMove = &ChickMoveSlash;
	else
		CurrentMove = &ChickMoveEndSlash;
}

void CMaiden::DoSlash()
{
	CurrentMove = &ChickMoveSlash;
}

CFrame ChickFramesStartSlash [] =
{	
	CFrame (&CMonster::AI_Charge, 1),
	CFrame (&CMonster::AI_Charge, 8),
	CFrame (&CMonster::AI_Charge, 3)
};
CAnim ChickMoveStartSlash (FRAME_attak201, FRAME_attak203, ChickFramesStartSlash, ConvertDerivedFunction(&CMaiden::DoSlash));

void CMaiden::Melee()
{
	CurrentMove = &ChickMoveStartSlash;
}

void CMaiden::Attack()
{
#ifdef MONSTER_USE_ROGUE_AI
	float r, chance;

	DoneDodge ();

	// PMM 
	if (AttackState == AS_BLIND)
	{
		// setup shot probabilities
		if (BlindFireDelay < 1.0f)
			chance = 1.0;
		else if (BlindFireDelay < 7.5f)
			chance = 0.4f;
		else
			chance = 0.1f;

		r = random();

		// minimum of 2 seconds, plus 0-3, after the shots are done
		BlindFireDelay += 2.0 + (4.5 * random());

		// don't shoot at the origin
		if (Vec3Compare (BlindFireTarget, vec3Origin))
			return;

		// don't shoot if the dice say not to
		if (r > chance)
			return;

		// turn on manual steering to signal both manual steering and blindfire
		AIFlags |= AI_MANUAL_STEERING;
		CurrentMove = &ChickMoveStartAttack1;
		AttackFinished = level.framenum + ((2*random())*10);
		return;
	}
	// pmm
#endif
	CurrentMove = &ChickMoveStartAttack1;
}

void CMaiden::Sight()
{
	Entity->PlaySound (CHAN_VOICE, SoundSight);
}

void CMaiden::Spawn ()
{
	Entity->TossPhysics = false;
	Entity->SetSolid (SOLID_BBOX);
	Entity->State.SetModelIndex ( ModelIndex("models/monsters/bitch/tris.md2"));
	Entity->SetMins (vec3f(-16, -16, 0));
	Entity->SetMaxs (vec3f(16, 16, 56));

	SoundMissilePrelaunch	= SoundIndex ("chick/chkatck1.wav");	
	SoundMissileLaunch	= SoundIndex ("chick/chkatck2.wav");	
	SoundMeleeSwing		= SoundIndex ("chick/chkatck3.wav");	
	SoundMeleeHit			= SoundIndex ("chick/chkatck4.wav");	
	SoundMissileReload	= SoundIndex ("chick/chkatck5.wav");	
	SoundDeath1			= SoundIndex ("chick/chkdeth1.wav");	
	SoundDeath2			= SoundIndex ("chick/chkdeth2.wav");	
	SoundFallDown			= SoundIndex ("chick/chkfall1.wav");	
	SoundIdle1				= SoundIndex ("chick/chkidle1.wav");	
	SoundIdle2				= SoundIndex ("chick/chkidle2.wav");	
	SoundPain1				= SoundIndex ("chick/chkpain1.wav");	
	SoundPain2				= SoundIndex ("chick/chkpain2.wav");	
	SoundPain3				= SoundIndex ("chick/chkpain3.wav");	
	SoundSight				= SoundIndex ("chick/chksght1.wav");	
	SoundSearch			= SoundIndex ("chick/chksrch1.wav");	

	Entity->gameEntity->health = 175;
	Entity->gameEntity->gib_health = -70;
	Entity->gameEntity->mass = 200;

	MonsterFlags = (MF_HAS_MELEE | MF_HAS_ATTACK | MF_HAS_IDLE | MF_HAS_SIGHT
#ifdef MONSTER_USE_ROGUE_AI
		| MF_HAS_DODGE | MF_HAS_DUCK | MF_HAS_UNDUCK | MF_HAS_SIDESTEP
#endif
		);

#ifdef MONSTER_USE_ROGUE_AI
	BlindFire = true;
#endif

	Entity->Link ();

	CurrentMove = &ChickMoveStand;
	WalkMonsterStart ();
}

LINK_MONSTER_CLASSNAME_TO_CLASS ("monster_bitch", CMaiden);