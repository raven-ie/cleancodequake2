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
// cc_supertank.h
// Supertank (Boss1)
//

#include "cc_local.h"
#include "m_supertank.h"

CSuperTank Monster_SuperTank;

CSuperTank::CSuperTank ()
{
	Classname = "monster_supertank";
	Scale = MODEL_SCALE;
};

void CSuperTank::Allocate (edict_t *ent)
{
	ent->Monster = new CSuperTank(Monster_SuperTank);
}

void CSuperTank::PlayTreadSound ()
{
	PlaySoundFrom (Entity, CHAN_BODY, TreadSound);
}

void CSuperTank::Search ()
{
	PlaySoundFrom (Entity, CHAN_VOICE, (random() < 0.5) ? SoundSearch1 : SoundSearch2);
}

//
// stand
//

CFrame SuperTankFramesStand []=
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
	CFrame (&CMonster::AI_Stand, 0)
};
CAnim SuperTankMoveStand (FRAME_stand_1, FRAME_stand_60, SuperTankFramesStand);
	
void CSuperTank::Stand ()
{
	CurrentMove = &SuperTankMoveStand;
}

CFrame SuperTankFramesRun [] =
{
	CFrame (&CMonster::AI_Run, 12, ConvertDerivedFunction(&CSuperTank::PlayTreadSound)),
	CFrame (&CMonster::AI_Run, 12),
	CFrame (&CMonster::AI_Run, 12),
	CFrame (&CMonster::AI_Run, 12),
	CFrame (&CMonster::AI_Run, 12),
	CFrame (&CMonster::AI_Run, 12),
	CFrame (&CMonster::AI_Run, 12),
	CFrame (&CMonster::AI_Run, 12),
	CFrame (&CMonster::AI_Run, 12),
	CFrame (&CMonster::AI_Run, 12),
	CFrame (&CMonster::AI_Run, 12),
	CFrame (&CMonster::AI_Run, 12),
	CFrame (&CMonster::AI_Run, 12),
	CFrame (&CMonster::AI_Run, 12),
	CFrame (&CMonster::AI_Run, 12),
	CFrame (&CMonster::AI_Run, 12),
	CFrame (&CMonster::AI_Run, 12),
	CFrame (&CMonster::AI_Run, 12)
};
CAnim SuperTankMoveRun (FRAME_forwrd_1, FRAME_forwrd_18, SuperTankFramesRun);

//
// walk
//

CFrame SuperTankFramesForward [] =
{
	CFrame (&CMonster::AI_Walk, 4, ConvertDerivedFunction(&CSuperTank::PlayTreadSound)),
	CFrame (&CMonster::AI_Walk, 4),
	CFrame (&CMonster::AI_Walk, 4),
	CFrame (&CMonster::AI_Walk, 4),
	CFrame (&CMonster::AI_Walk, 4),
	CFrame (&CMonster::AI_Walk, 4),
	CFrame (&CMonster::AI_Walk, 4),
	CFrame (&CMonster::AI_Walk, 4),
	CFrame (&CMonster::AI_Walk, 4),
	CFrame (&CMonster::AI_Walk, 4),
	CFrame (&CMonster::AI_Walk, 4),
	CFrame (&CMonster::AI_Walk, 4),
	CFrame (&CMonster::AI_Walk, 4),
	CFrame (&CMonster::AI_Walk, 4),
	CFrame (&CMonster::AI_Walk, 4),
	CFrame (&CMonster::AI_Walk, 4),
	CFrame (&CMonster::AI_Walk, 4),
	CFrame (&CMonster::AI_Walk, 4)
};
CAnim SuperTankMoveForward (FRAME_forwrd_1, FRAME_forwrd_18, SuperTankFramesForward);

/*void CSuperTank::Forward (edict_t *self)
{
	CurrentMove = &SuperTankMoveForward;
}*/

void CSuperTank::Walk ()
{
	CurrentMove = &SuperTankMoveForward;
}

void CSuperTank::Run ()
{
	CurrentMove = (AIFlags & AI_STAND_GROUND) ? &SuperTankMoveStand : &SuperTankMoveRun;
}

/*mframe_t supertank_frames_turn_right [] =
{
	CFrame (&CMonster::AI_Move,	0,	TreadSound,
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0,	NULL
};
mmove_t supertank_move_turn_right = {FRAME_right_1, FRAME_right_18, supertank_frames_turn_right, supertank_run};

mframe_t supertank_frames_turn_left [] =
{
	CFrame (&CMonster::AI_Move,	0,	TreadSound,
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0,	NULL
};
mmove_t supertank_move_turn_left = {FRAME_left_1, FRAME_left_18, supertank_frames_turn_left, supertank_run};*/

CFrame SuperTankFramesPain3 [] =
{
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0)
};
CAnim SuperTankMovePain3 (FRAME_pain3_9, FRAME_pain3_12, SuperTankFramesPain3, &CMonster::Run);

CFrame SuperTankFramesPain2 [] =
{
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0)
};
CAnim SuperTankMovePain2 (FRAME_pain2_5, FRAME_pain2_8, SuperTankFramesPain2, &CMonster::Run);

CFrame SuperTankFramesPain1 [] =
{
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0)
};
CAnim SuperTankMovePain1 (FRAME_pain1_1, FRAME_pain1_4, SuperTankFramesPain1, &CMonster::Run);

CFrame SuperTankFramesDeath1 [] =
{
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0, ConvertDerivedFunction(&CSuperTank::Explode))
};
CAnim SuperTankMoveDeath (FRAME_death_1, FRAME_death_24, SuperTankFramesDeath1, ConvertDerivedFunction(&CSuperTank::Dead));

/*mframe_t supertank_frames_backward[] =
{
	CFrame (&CMonster::AI_Walk, 0,	TreadSound,
	CFrame (&CMonster::AI_Walk, 0),
	CFrame (&CMonster::AI_Walk, 0),
	CFrame (&CMonster::AI_Walk, 0),
	CFrame (&CMonster::AI_Walk, 0),
	CFrame (&CMonster::AI_Walk, 0),
	CFrame (&CMonster::AI_Walk, 0),
	CFrame (&CMonster::AI_Walk, 0),
	CFrame (&CMonster::AI_Walk, 0),
	CFrame (&CMonster::AI_Walk, 0),
	CFrame (&CMonster::AI_Walk, 0),
	CFrame (&CMonster::AI_Walk, 0),
	CFrame (&CMonster::AI_Walk, 0),
	CFrame (&CMonster::AI_Walk, 0),
	CFrame (&CMonster::AI_Walk, 0),
	CFrame (&CMonster::AI_Walk, 0),
	CFrame (&CMonster::AI_Walk, 0),
	CFrame (&CMonster::AI_Walk, 0,	NULL
};
mmove_t	supertank_move_backward = {FRAME_backwd_1, FRAME_backwd_18, supertank_frames_backward, NULL};*/

#ifdef SUPERTANK_USES_GRENADE_LAUNCHER
CFrame SuperTankFramesAttack4[] =
{
	CFrame (&CMonster::AI_Move,	0, ConvertDerivedFunction(&CSuperTank::Grenade)),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0, ConvertDerivedFunction(&CSuperTank::Grenade)),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0)
};
CAnim SuperTankMoveAttack4 (FRAME_attak4_1, FRAME_attak4_6, SuperTankFramesAttack4, &CMonster::Run);

void CSuperTank::Grenade ()
{
	vec3_t	vec;
	vec3_t	start;
	vec3_t	forward, right;
	vec3_t	offset = {32.0f, 37.0f, 50.0f};

	if (Entity->state.frame == FRAME_attak4_4)
		offset[1] = -offset[1];

	Angles_Vectors (Entity->state.angles, forward, right, NULL);
	G_ProjectSource (Entity->state.origin, offset, forward, right, start);

	if (Entity->enemy)
	{
		Vec3Copy (Entity->enemy->state.origin, vec);
		Vec3MA (vec, 0, Entity->enemy->velocity, vec);
		vec[2] += Entity->enemy->viewheight;
		Vec3Subtract (vec, start, forward);
		VectorNormalizef (forward, forward);
	}

	PlaySoundFrom (Entity, CHAN_WEAPON, SoundIndex("gunner/Gunatck3.wav"));

	MonsterFireGrenade (start, forward, 25, 600, -1);
}
#endif

/*mframe_t supertank_frames_attack3[]=
{
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0,	NULL
};
mmove_t supertank_move_attack3 = {FRAME_attak3_1, FRAME_attak3_27, supertank_frames_attack3, supertank_run};*/

CFrame SuperTankFramesAttack2[]=
{
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0,	ConvertDerivedFunction(&CSuperTank::Rocket)),
	CFrame (&CMonster::AI_Charge,		0),
	CFrame (&CMonster::AI_Charge,		0),
	CFrame (&CMonster::AI_Charge,		0,	ConvertDerivedFunction(&CSuperTank::Rocket)),
	CFrame (&CMonster::AI_Charge,		0),
	CFrame (&CMonster::AI_Charge,		0),
	CFrame (&CMonster::AI_Charge,		0,	ConvertDerivedFunction(&CSuperTank::Rocket)),
	CFrame (&CMonster::AI_Charge,		0),
	CFrame (&CMonster::AI_Charge,		0),
	CFrame (&CMonster::AI_Charge,		0),
	CFrame (&CMonster::AI_Charge,		0),
	CFrame (&CMonster::AI_Charge,		0),
	CFrame (&CMonster::AI_Charge,		0),
	CFrame (&CMonster::AI_Charge,		0),
	CFrame (&CMonster::AI_Charge,		0),
	CFrame (&CMonster::AI_Charge,		0),
	CFrame (&CMonster::AI_Charge,		0),
	CFrame (&CMonster::AI_Charge,		0),
	CFrame (&CMonster::AI_Charge,		0),
	CFrame (&CMonster::AI_Charge,		0)
};
CAnim SuperTankMoveAttack2 (FRAME_attak2_1, FRAME_attak2_27, SuperTankFramesAttack2, &CMonster::Run);

CFrame SuperTankFramesAttack1[]=
{
	CFrame (&CMonster::AI_Charge,	0,	ConvertDerivedFunction(&CSuperTank::MachineGun)),
	CFrame (&CMonster::AI_Charge,	0,	ConvertDerivedFunction(&CSuperTank::MachineGun)),
	CFrame (&CMonster::AI_Charge,	0,	ConvertDerivedFunction(&CSuperTank::MachineGun)),
	CFrame (&CMonster::AI_Charge,	0,	ConvertDerivedFunction(&CSuperTank::MachineGun)),
	CFrame (&CMonster::AI_Charge,	0,	ConvertDerivedFunction(&CSuperTank::MachineGun)),
	CFrame (&CMonster::AI_Charge,	0,	ConvertDerivedFunction(&CSuperTank::MachineGun)),
};
CAnim SuperTankMoveAttack1 (FRAME_attak1_1, FRAME_attak1_6, SuperTankFramesAttack1, ConvertDerivedFunction(&CSuperTank::ReAttack1));

CFrame SuperTankFramesEndAttack1[]=
{
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0)
};
CAnim SuperTankMoveEndAttack1 (FRAME_attak1_7, FRAME_attak1_20, SuperTankFramesEndAttack1, &CMonster::Run);

void CSuperTank::ReAttack1 ()
{
	if (visible(Entity, Entity->enemy))
		CurrentMove = (random() < 0.9) ? &SuperTankMoveAttack1 : &SuperTankMoveEndAttack1;
	else
		CurrentMove = &SuperTankMoveEndAttack1;
}

void CSuperTank::Pain (edict_t *other, float kick, int damage)
{
	if (Entity->health < (Entity->max_health / 2))
			Entity->state.skinNum = 1;

	if (level.framenum < Entity->pain_debounce_time)
			return;

	// Lessen the chance of him going into his pain frames
	if (damage <= 25 && random() < 0.2)
		return;

	// Don't go into pain if he's firing his rockets
	if (skill->Integer() >= 2 && (Entity->state.frame >= FRAME_attak2_1) && (Entity->state.frame <= FRAME_attak2_14) )
		return;

	Entity->pain_debounce_time = level.framenum + 30;

	if (skill->Integer() == 3)
		return;		// no pain anims in nightmare

	if (damage <= 10)
	{
		PlaySoundFrom (Entity, CHAN_VOICE, SoundPain1);
		CurrentMove = &SuperTankMovePain1;
	}
	else if (damage <= 25)
	{
		PlaySoundFrom (Entity, CHAN_VOICE, SoundPain3);
		CurrentMove = &SuperTankMovePain2;
	}
	else
	{
		PlaySoundFrom (Entity, CHAN_VOICE, SoundPain2);
		CurrentMove = &SuperTankMovePain3;
	}

#ifdef MONSTER_USE_ROGUE_AI
	// PMM - blindfire cleanup
	AIFlags &= ~AI_MANUAL_STEERING;
	// pmm
#endif
};


void CSuperTank::Rocket ()
{
	vec3_t	forward, right;
	vec3_t	start;
	vec3_t	dir;
	vec3_t	vec;
	int		FlashNumber;
	vec3_t target;
#ifdef MONSTER_USE_ROGUE_AI
	bool blindfire = false;
#endif

	// pmm - blindfire check
#ifdef MONSTER_USE_ROGUE_AI
	if (AIFlags & AI_MANUAL_STEERING)
		blindfire = true;
#endif

	switch (Entity->state.frame)
	{
	case FRAME_attak2_8:
		FlashNumber = MZ2_SUPERTANK_ROCKET_1;
		break;
	case FRAME_attak2_11:
		FlashNumber = MZ2_SUPERTANK_ROCKET_2;
		break;
	default:
		FlashNumber = MZ2_SUPERTANK_ROCKET_3;

#ifdef MONSTER_USE_ROGUE_AI
		// PMM - blindfire cleanup
		if (AIFlags & AI_MANUAL_STEERING)
			AIFlags &= ~AI_MANUAL_STEERING;
		// pmm
#endif
		break;
	}

	Angles_Vectors (Entity->state.angles, forward, right, NULL);
	G_ProjectSource (Entity->state.origin, dumb_and_hacky_monster_MuzzFlashOffset[FlashNumber], forward, right, start);

		// PMM
#ifdef MONSTER_USE_ROGUE_AI
	if (blindfire)
		Vec3Copy (BlindFireTarget, target);
	else
#endif
		Vec3Copy (Entity->enemy->state.origin, target);
	// pmm

//	VectorCopy (self->enemy->state.origin, vec);
//	vec[2] += self->enemy->viewheight;
//	VectorSubtract (vec, start, dir);

//PGM
	// PMM - blindfire shooting
#ifdef MONSTER_USE_ROGUE_AI
	if (blindfire)
	{
		Vec3Copy (target, vec);
		Vec3Subtract (vec, start, dir);
	}
	// pmm
	// don't shoot at feet if they're above me.
	else
#endif
	if(random() < 0.66 || (start[2] < Entity->enemy->absMin[2]))
	{
//		gi.dprintf("normal shot\n");
		Vec3Copy (Entity->enemy->state.origin, vec);
		vec[2] += Entity->enemy->viewheight;
		Vec3Subtract (vec, start, dir);
	}
	else
	{
//		gi.dprintf("shooting at feet!\n");
		Vec3Copy (Entity->enemy->state.origin, vec);
		vec[2] = Entity->enemy->absMin[2];
		Vec3Subtract (vec, start, dir);
	}
//PGM
	
//======
	VectorNormalizeFastf (dir);

	// pmm blindfire doesn't check target (done in checkattack)
	// paranoia, make sure we're not shooting a target right next to us
	CTrace trace = CTrace(start, vec, Entity, CONTENTS_MASK_SHOT);
	#ifdef MONSTER_USE_ROGUE_AI
	if (blindfire)
	{
		// blindfire has different fail criteria for the trace
		if (!(trace.startSolid || trace.allSolid || (trace.fraction < 0.5)))
			MonsterFireRocket (start, dir, 50, 500, FlashNumber);
		else 
		{
			// try shifting the target to the left a little (to help counter large offset)
			Vec3Copy (target, vec);
			Vec3MA (vec, -20, right, vec);
			Vec3Subtract(vec, start, dir);
			VectorNormalizeFastf (dir);
			trace = CTrace(start, vec, Entity, CONTENTS_MASK_SHOT);
			if (!(trace.startSolid || trace.allSolid || (trace.fraction < 0.5)))
				MonsterFireRocket (start, dir, 50, 500, FlashNumber);
			else 
			{
				// ok, that failed.  try to the right
				Vec3Copy (target, vec);
				Vec3MA (vec, 20, right, vec);
				Vec3Subtract(vec, start, dir);
				VectorNormalizeFastf (dir);
				trace = CTrace(start, vec, Entity, CONTENTS_MASK_SHOT);
				if (!(trace.startSolid || trace.allSolid || (trace.fraction < 0.5)))
					MonsterFireRocket (start, dir, 50, 500, FlashNumber);
			}
		}
	}
	else
#endif
	{
		trace = CTrace(start, vec, Entity, CONTENTS_MASK_SHOT);
		if(trace.ent == Entity->enemy || trace.ent == world)
		{
			if(trace.fraction > 0.5 || (trace.ent && trace.ent->client))
				MonsterFireRocket (start, dir, 50, 500, FlashNumber);
	//		else
	//			gi.dprintf("didn't make it halfway to target...aborting\n");
		}
	}
}	

void CSuperTank::MachineGun ()
{
	vec3_t	vec;
	vec3_t	start;
	vec3_t	forward, right;
	int		FlashNumber = MZ2_SUPERTANK_MACHINEGUN_1 + (Entity->state.frame - FRAME_attak1_1);
	vec3_t dir = {0, Entity->state.angles[1], 0};

	Angles_Vectors (dir, forward, right, NULL);
	G_ProjectSource (Entity->state.origin, dumb_and_hacky_monster_MuzzFlashOffset[FlashNumber], forward, right, start);

	if (Entity->enemy)
	{
		Vec3Copy (Entity->enemy->state.origin, vec);
		Vec3MA (vec, 0, Entity->enemy->velocity, vec);
		vec[2] += Entity->enemy->viewheight;
		Vec3Subtract (vec, start, forward);
		VectorNormalizef (forward, forward);
	}

	MonsterFireBullet (start, forward, 6, 4, DEFAULT_BULLET_HSPREAD, DEFAULT_BULLET_VSPREAD, FlashNumber);
}	


void CSuperTank::Attack ()
{
	vec3_t	vec;
	float	range;

#ifdef MONSTER_USE_ROGUE_AI
	// PMM 
	if (AttackState == AS_BLIND)
	{
		float chance, r;
		// setup shot probabilities
		if (BlindFireDelay < 1.0)
			chance = 1.0;
		else if (BlindFireDelay < 7.5)
			chance = 0.4f;
		else
			chance = 0.1f;

		r = random();

		BlindFireDelay += 3.2 + 2.0 + random()*3.0;

		// don't shoot at the origin
		if (Vec3Compare (BlindFireTarget, vec3Origin))
			return;

		// don't shoot if the dice say not to
		if (r > chance)
			return;

		// turn on manual steering to signal both manual steering and blindfire
		AIFlags |= AI_MANUAL_STEERING;
		CurrentMove = &SuperTankMoveAttack2;
		AttackFinished = level.framenum + 30 + 20*random();
		return;
	}
	// pmm
#endif

	Vec3Subtract (Entity->enemy->state.origin, Entity->state.origin, vec);
	range = Vec3Length (vec);

#ifndef SUPERTANK_USES_GRENADE_LAUNCHER
	if (range <= 160)
		CurrentMove = &SuperTankMoveAttack1;
	else // fire rockets more often at distance
		CurrentMove = (random() < 0.3) ? &SuperTankMoveAttack1 : &SuperTankMoveAttack2;
#else
	if (range <= 160)
		CurrentMove = (random() < 0.3) ? &SuperTankMoveAttack4 : &SuperTankMoveAttack1;
	else if (range <= 650)
		CurrentMove = (random() < 0.6) ? &SuperTankMoveAttack4 : &SuperTankMoveAttack1;
	else
		CurrentMove = (random() < 0.3) ? &SuperTankMoveAttack1 : &SuperTankMoveAttack2;
#endif
}


//
// death
//

void CSuperTank::Dead ()
{
	Vec3Set (Entity->mins, -60, -60, 0);
	Vec3Set (Entity->maxs, 60, 60, 72);
	Entity->movetype = MOVETYPE_TOSS;
	Entity->svFlags |= SVF_DEADMONSTER;
	Entity->nextthink = 0;
	gi.linkentity (Entity);
}

void CSuperTank::Explode ()
{
	vec3_t	org;
	int		n;

	Think = ConvertDerivedFunction(&CSuperTank::Explode);
	Vec3Copy (Entity->state.origin, org);
	org[2] += 24 + (rand()&15);
	switch (Entity->count++)
	{
	case 0:
		org[0] -= 24;
		org[1] -= 24;
		break;
	case 1:
		org[0] += 24;
		org[1] += 24;
		break;
	case 2:
		org[0] += 24;
		org[1] -= 24;
		break;
	case 3:
		org[0] -= 24;
		org[1] += 24;
		break;
	case 4:
		org[0] -= 48;
		org[1] -= 48;
		break;
	case 5:
		org[0] += 48;
		org[1] += 48;
		break;
	case 6:
		org[0] -= 48;
		org[1] += 48;
		break;
	case 7:
		org[0] += 48;
		org[1] -= 48;
		break;
	case 8:
		Entity->state.sound = 0;
		for (n= 0; n < 4; n++)
			ThrowGib (Entity, gMedia.Gib_SmallMeat, 500, GIB_ORGANIC);
		for (n= 0; n < 8; n++)
			ThrowGib (Entity, gMedia.Gib_SmallMetal, 500, GIB_METALLIC);
		ThrowGib (Entity, gMedia.Gib_Chest, 500, GIB_ORGANIC);
		ThrowHead (Entity, gMedia.Gib_Gear, 500, GIB_METALLIC);
		Entity->deadflag = DEAD_DEAD;
		return;
	}

	CTempEnt_Explosions::RocketExplosion (org, Entity);

	NextThink = level.framenum + FRAMETIME;
}


void CSuperTank::Die (edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
{
	PlaySoundFrom (Entity, CHAN_VOICE, SoundDeath);
	Entity->deadflag = DEAD_DEAD;
	Entity->takedamage = DAMAGE_NO;
	Entity->count = 0;
	CurrentMove = &SuperTankMoveDeath;
}

//
// monster_supertank
//

/*QUAKED monster_supertank (1 .5 0) (-64 -64 0) (64 64 72) Ambush Trigger_Spawn Sight
*/
void CSuperTank::Spawn ()
{
	SoundPain1 = SoundIndex ("bosstank/btkpain1.wav");
	SoundPain2 = SoundIndex ("bosstank/btkpain2.wav");
	SoundPain3 = SoundIndex ("bosstank/btkpain3.wav");
	SoundDeath = SoundIndex ("bosstank/btkdeth1.wav");
	SoundSearch1 = SoundIndex ("bosstank/btkunqv1.wav");
	SoundSearch2 = SoundIndex ("bosstank/btkunqv2.wav");

	TreadSound = SoundIndex ("bosstank/btkengn1.wav");

	Entity->movetype = MOVETYPE_STEP;
	Entity->solid = SOLID_BBOX;
	Entity->state.modelIndex = ModelIndex ("models/monsters/boss1/tris.md2");
	Vec3Set (Entity->mins, -64, -64, 0);
	Vec3Set (Entity->maxs, 64, 64, 112);

	Entity->health = 1500;
	Entity->gib_health = -500;
	Entity->mass = 800;

#ifdef MONSTER_USE_ROGUE_AI
	// PMM
	AIFlags |= AI_IGNORE_SHOTS;
	BlindFire = true;
	//pmm
#endif

	MonsterFlags = (MF_HAS_ATTACK | MF_HAS_SEARCH);

	gi.linkentity (Entity);
	
	Stand();

	WalkMonsterStart();
}