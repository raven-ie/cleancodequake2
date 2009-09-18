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
// cc_icarus.cpp
// Icarus
//

#include "cc_local.h"
#include "m_hover.h"
#include "cc_icarus.h"

CIcarus::CIcarus ()
{
	Scale = MODEL_SCALE;
}

void CIcarus::Sight ()
{
	Entity->PlaySound (CHAN_VOICE, SoundSight);
}

void CIcarus::Search ()
{
	Entity->PlaySound (CHAN_VOICE, (random() < 0.5) ? SoundSearch1 : SoundSearch2);
}

CFrame HoverFramesStand [] =
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
	CFrame (&CMonster::AI_Stand, 0)
};
CAnim HoverMoveStand (FRAME_stand01, FRAME_stand30, HoverFramesStand);

/*mframe_t hover_frames_stop1 [] =
{
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
mmove_t hover_move_stop1 = {FRAME_stop101, FRAME_stop109, hover_frames_stop1, NULL};

mframe_t hover_frames_stop2 [] =
{
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0,	NULL
};
mmove_t hover_move_stop2 = {FRAME_stop201, FRAME_stop208, hover_frames_stop2, NULL};

mframe_t hover_frames_takeoff [] =
{
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	-2),
	CFrame (&CMonster::AI_Move,	5),
	CFrame (&CMonster::AI_Move,	-1),
	CFrame (&CMonster::AI_Move,	1),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	-1),
	CFrame (&CMonster::AI_Move,	-1),
	CFrame (&CMonster::AI_Move,	-1),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	2),
	CFrame (&CMonster::AI_Move,	2),
	CFrame (&CMonster::AI_Move,	1),
	CFrame (&CMonster::AI_Move,	1),
	CFrame (&CMonster::AI_Move,	-6),
	CFrame (&CMonster::AI_Move,	-9),
	CFrame (&CMonster::AI_Move,	1),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	2),
	CFrame (&CMonster::AI_Move,	2),
	CFrame (&CMonster::AI_Move,	1),
	CFrame (&CMonster::AI_Move,	1),
	CFrame (&CMonster::AI_Move,	1),
	CFrame (&CMonster::AI_Move,	2),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	2),
	CFrame (&CMonster::AI_Move,	3),
	CFrame (&CMonster::AI_Move,	2),
	CFrame (&CMonster::AI_Move,	0,	NULL
};
mmove_t hover_move_takeoff = {FRAME_takeof01, FRAME_takeof30, hover_frames_takeoff, NULL};*/

CFrame HoverFramesPain3 [] =
{
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
CAnim HoverMovePain3 (FRAME_pain301, FRAME_pain309, HoverFramesPain3, &CMonster::Run);

CFrame HoverFramesPain2 [] =
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
	CFrame (&CMonster::AI_Move,	0)
};
CAnim HoverMovePain2 (FRAME_pain201, FRAME_pain212, HoverFramesPain2, &CMonster::Run);

CFrame HoverFramesPain1 [] =
{
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	2),
	CFrame (&CMonster::AI_Move,	-8),
	CFrame (&CMonster::AI_Move,	-4),
	CFrame (&CMonster::AI_Move,	-6),
	CFrame (&CMonster::AI_Move,	-4),
	CFrame (&CMonster::AI_Move,	-3),
	CFrame (&CMonster::AI_Move,	1),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	3),
	CFrame (&CMonster::AI_Move,	1),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	2),
	CFrame (&CMonster::AI_Move,	3),
	CFrame (&CMonster::AI_Move,	2),
	CFrame (&CMonster::AI_Move,	7),
	CFrame (&CMonster::AI_Move,	1),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	2),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	5),
	CFrame (&CMonster::AI_Move,	3),
	CFrame (&CMonster::AI_Move,	4)
};
CAnim HoverMovePain1 (FRAME_pain101, FRAME_pain128, HoverFramesPain1, &CMonster::Run);

/*mframe_t hover_frames_land [] =
{
	CFrame (&CMonster::AI_Move,	0,	NULL
};
mmove_t hover_move_land = {FRAME_land01, FRAME_land01, hover_frames_land, NULL};

mframe_t hover_frames_forward [] =
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
mmove_t hover_move_forward = {FRAME_forwrd01, FRAME_forwrd35, hover_frames_forward, NULL};*/

CFrame HoverFramesWalk [] =
{
	CFrame (&CMonster::AI_Walk,	4),
	CFrame (&CMonster::AI_Walk,	4),
	CFrame (&CMonster::AI_Walk,	4),
	CFrame (&CMonster::AI_Walk,	4),
	CFrame (&CMonster::AI_Walk,	4),
	CFrame (&CMonster::AI_Walk,	4),
	CFrame (&CMonster::AI_Walk,	4),
	CFrame (&CMonster::AI_Walk,	4),
	CFrame (&CMonster::AI_Walk,	4),
	CFrame (&CMonster::AI_Walk,	4),
	CFrame (&CMonster::AI_Walk,	4),
	CFrame (&CMonster::AI_Walk,	4),
	CFrame (&CMonster::AI_Walk,	4),
	CFrame (&CMonster::AI_Walk,	4),
	CFrame (&CMonster::AI_Walk,	4),
	CFrame (&CMonster::AI_Walk,	4),
	CFrame (&CMonster::AI_Walk,	4),
	CFrame (&CMonster::AI_Walk,	4),
	CFrame (&CMonster::AI_Walk,	4),
	CFrame (&CMonster::AI_Walk,	4),
	CFrame (&CMonster::AI_Walk,	4),
	CFrame (&CMonster::AI_Walk,	4),
	CFrame (&CMonster::AI_Walk,	4),
	CFrame (&CMonster::AI_Walk,	4),
	CFrame (&CMonster::AI_Walk,	4),
	CFrame (&CMonster::AI_Walk,	4),
	CFrame (&CMonster::AI_Walk,	4),
	CFrame (&CMonster::AI_Walk,	4),
	CFrame (&CMonster::AI_Walk,	4),
	CFrame (&CMonster::AI_Walk,	4),
	CFrame (&CMonster::AI_Walk,	4),
	CFrame (&CMonster::AI_Walk,	4),
	CFrame (&CMonster::AI_Walk,	4),
	CFrame (&CMonster::AI_Walk,	4),
	CFrame (&CMonster::AI_Walk,	4)
};
CAnim HoverMoveWalk (FRAME_forwrd01, FRAME_forwrd35, HoverFramesWalk);

CFrame HoverFramesRun [] =
{
	CFrame (&CMonster::AI_Run,	10),
	CFrame (&CMonster::AI_Run,	10),
	CFrame (&CMonster::AI_Run,	10),
	CFrame (&CMonster::AI_Run,	10),
	CFrame (&CMonster::AI_Run,	10),
	CFrame (&CMonster::AI_Run,	10),
	CFrame (&CMonster::AI_Run,	10),
	CFrame (&CMonster::AI_Run,	10),
	CFrame (&CMonster::AI_Run,	10),
	CFrame (&CMonster::AI_Run,	10),
	CFrame (&CMonster::AI_Run,	10),
	CFrame (&CMonster::AI_Run,	10),
	CFrame (&CMonster::AI_Run,	10),
	CFrame (&CMonster::AI_Run,	10),
	CFrame (&CMonster::AI_Run,	10),
	CFrame (&CMonster::AI_Run,	10),
	CFrame (&CMonster::AI_Run,	10),
	CFrame (&CMonster::AI_Run,	10),
	CFrame (&CMonster::AI_Run,	10),
	CFrame (&CMonster::AI_Run,	10),
	CFrame (&CMonster::AI_Run,	10),
	CFrame (&CMonster::AI_Run,	10),
	CFrame (&CMonster::AI_Run,	10),
	CFrame (&CMonster::AI_Run,	10),
	CFrame (&CMonster::AI_Run,	10),
	CFrame (&CMonster::AI_Run,	10),
	CFrame (&CMonster::AI_Run,	10),
	CFrame (&CMonster::AI_Run,	10),
	CFrame (&CMonster::AI_Run,	10),
	CFrame (&CMonster::AI_Run,	10),
	CFrame (&CMonster::AI_Run,	10),
	CFrame (&CMonster::AI_Run,	10),
	CFrame (&CMonster::AI_Run,	10),
	CFrame (&CMonster::AI_Run,	10),
	CFrame (&CMonster::AI_Run,	10)
};
CAnim HoverMoveRun (FRAME_forwrd01, FRAME_forwrd35, HoverFramesRun);

CFrame HoverFramesDeath1 [] =
{
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	-10),
	CFrame (&CMonster::AI_Move,	3),
	CFrame (&CMonster::AI_Move,	5),
	CFrame (&CMonster::AI_Move,	4),
	CFrame (&CMonster::AI_Move,	7)
};
CAnim HoverMoveDeath1 (FRAME_death101, FRAME_death111, HoverFramesDeath1, ConvertDerivedFunction(&CIcarus::Dead));

/*mframe_t hover_frames_backward [] =
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
	CFrame (&CMonster::AI_Move,	0,	NULL
};
mmove_t hover_move_backward = {FRAME_backwd01, FRAME_backwd24, hover_frames_backward, NULL};*/

CFrame HoverFramesStartAttack [] =
{
	CFrame (&CMonster::AI_Charge,	1),
	CFrame (&CMonster::AI_Charge,	1),
	CFrame (&CMonster::AI_Charge,	1)
};
CAnim HoverMoveStartAttack (FRAME_attak101, FRAME_attak103, HoverFramesStartAttack, ConvertDerivedFunction(&CIcarus::StartAttack));

CFrame HoverFramesAttack1 [] =
{
	CFrame (&CMonster::AI_Charge,	-10,	ConvertDerivedFunction(&CIcarus::FireBlaster)),
	CFrame (&CMonster::AI_Charge,	-10,	ConvertDerivedFunction(&CIcarus::FireBlaster)),
	CFrame (&CMonster::AI_Charge,	0,		ConvertDerivedFunction(&CIcarus::ReAttack))
};
CAnim HoverMoveAttack1 (FRAME_attak104, FRAME_attak106, HoverFramesAttack1);

CFrame HoverFramesEndAttack [] =
{
	CFrame (&CMonster::AI_Charge,	1),
	CFrame (&CMonster::AI_Charge,	1)
};
CAnim HoverMoveEndAttack (FRAME_attak107, FRAME_attak108, HoverFramesEndAttack, &CMonster::Run);

#ifdef MONSTER_USE_ROGUE_AI

/* PMM - circle strafing code */
CFrame HoverFramesStartAttack2 [] =
{
	CFrame (&CMonster::AI_Charge,	15),
	CFrame (&CMonster::AI_Charge,	15),
	CFrame (&CMonster::AI_Charge,	15)
};
CAnim HoverMoveStartAttack2 (FRAME_attak101, FRAME_attak103, HoverFramesStartAttack2, ConvertDerivedFunction(&CIcarus::StartAttack));

CFrame HoverFramesAttack2 [] =
{
	CFrame (&CMonster::AI_Charge,	10,	ConvertDerivedFunction(&CIcarus::FireBlaster)),
	CFrame (&CMonster::AI_Charge,	10,	ConvertDerivedFunction(&CIcarus::FireBlaster)),
	CFrame (&CMonster::AI_Charge,	10,	ConvertDerivedFunction(&CIcarus::ReAttack))
};
CAnim HoverMoveAttack2 (FRAME_attak104, FRAME_attak106, HoverFramesAttack2);

CFrame HoverFramesEndAttack2 [] =
{
	CFrame (&CMonster::AI_Charge,	15),
	CFrame (&CMonster::AI_Charge,	15)
};
CAnim HoverMoveEndAttack2 (FRAME_attak107, FRAME_attak108, HoverFramesEndAttack2, &CMonster::Run);
// end of circle strafe
#endif

void CIcarus::ReAttack ()
{
	if (dynamic_cast<CHurtableEntity*>(Entity->gameEntity->enemy->Entity)->Health > 0 && IsVisible (Entity, Entity->gameEntity->enemy->Entity) && random() <= 0.6)
	{
#ifdef MONSTER_USE_ROGUE_AI
		CurrentMove = (AttackState == AS_SLIDING) ? &HoverMoveAttack2 : &HoverMoveAttack1;
#else
		CurrentMove = &HoverMoveAttack1;
#endif
		return;
	}

	CurrentMove = &HoverMoveEndAttack;
}


void CIcarus::FireBlaster ()
{
	vec3f	start, forward, right, end, dir;

	Entity->State.GetAngles().ToVectors (&forward, &right, NULL);
	G_ProjectSource (Entity->State.GetOrigin(), dumb_and_hacky_monster_MuzzFlashOffset[MZ2_HOVER_BLASTER_1], forward, right, start);

	end = vec3f(Entity->gameEntity->enemy->state.origin);
	end.Z += Entity->gameEntity->enemy->viewheight;
	dir = end - start;

	MonsterFireBlaster (start, dir, 1, 1000, MZ2_HOVER_BLASTER_1, (Entity->State.GetFrame() == FRAME_attak104) ? EF_HYPERBLASTER : 0);
}


void CIcarus::Stand ()
{
	CurrentMove = &HoverMoveStand;
}

void CIcarus::Run ()
{
	CurrentMove = (AIFlags & AI_STAND_GROUND) ? &HoverMoveStand : &HoverMoveRun;
}

void CIcarus::Walk ()
{
	CurrentMove = &HoverMoveWalk;
}

void CIcarus::Attack ()
{
#ifdef MONSTER_USE_ROGUE_AI
	float chance;

	// 0% chance of circle in easy
	// 50% chance in normal
	// 75% chance in hard
	// 86.67% chance in nightmare
	if (!skill->Integer())
		chance = 0;
	else
		chance = 1.0f - (0.5f/skill->Float());

	if (random() > chance)
	{
		CurrentMove = &HoverMoveStartAttack;
		AttackState = AS_STRAIGHT;
	}
	else // circle strafe
	{
		if (random () <= 0.5) // switch directions
			Lefty = !Lefty;
		CurrentMove = &HoverMoveStartAttack2;
		AttackState = AS_SLIDING;
	}
#else
	CurrentMove = &HoverMoveStartAttack;
#endif
}

void CIcarus::StartAttack()
{
	CurrentMove = &HoverMoveAttack1;
}

void CIcarus::Pain (CBaseEntity *other, float kick, int damage)
{
	if (Entity->Health < (Entity->MaxHealth / 2))
		Entity->State.SetSkinNum(1);

	if (level.framenum < Entity->gameEntity->pain_debounce_time)
		return;

	Entity->gameEntity->pain_debounce_time = level.framenum + 30;

	if (skill->Integer() == 3)
		return;		// no pain anims in nightmare

	if (damage <= 25)
	{
		if (random() < 0.5)
		{
			Entity->PlaySound (CHAN_VOICE, SoundPain1);
			CurrentMove = &HoverMovePain3;
		}
		else
		{
			Entity->PlaySound (CHAN_VOICE, SoundPain2);
			CurrentMove = &HoverMovePain2;
		}
	}
	else
	{
#ifndef MONSTER_USE_ROGUE_AI
		Entity->PlaySound (CHAN_VOICE, SoundPain1);
		CurrentMove = &HoverMovePain1;
#else
		//PGM pain sequence is WAY too long
		if (random() < (0.45 - (0.1 * skill->Float())))
		{
			Entity->PlaySound (CHAN_VOICE, SoundPain1);
			CurrentMove = &HoverMovePain1;
		}
		else
		{
			Entity->PlaySound (CHAN_VOICE, SoundPain2);
			CurrentMove = &HoverMovePain2;
		}
#endif
	}
}

void CIcarus::DeadThink ()
{
	if (!Entity->GroundEntity && level.framenum < TimeStamp)
	{
		Entity->NextThink = level.framenum + FRAMETIME;
		return;
	}
	Entity->BecomeExplosion(false);
}

void CIcarus::Dead ()
{
	Entity->SetMins (vec3f(-16, -16, -24));
	Entity->SetMaxs (vec3f(16, 16, -8));
	Think = ConvertDerivedFunction(&CIcarus::DeadThink);
	Entity->NextThink = level.framenum + FRAMETIME;
	TimeStamp = level.framenum + 150;
	Entity->Link ();
}

void CIcarus::Die (CBaseEntity *inflictor, CBaseEntity *attacker, int damage, vec3f &point)
{
// check for gib
	if (Entity->Health <= Entity->GibHealth)
	{
		Entity->PlaySound (CHAN_VOICE, SoundIndex ("misc/udeath.wav"));
		for (int n= 0; n < 2; n++)
			CGibEntity::Spawn (Entity, gMedia.Gib_Bone[0], damage, GIB_ORGANIC);
		for (int n= 0; n < 2; n++)
			CGibEntity::Spawn (Entity, gMedia.Gib_SmallMeat, damage, GIB_ORGANIC);
		Entity->ThrowHead (gMedia.Gib_SmallMeat, damage, GIB_ORGANIC);
		Entity->DeadFlag = true;
		return;
	}

	if (Entity->DeadFlag == true)
		return;

	Entity->PhysicsType = PHYSICS_TOSS;
// regular death
	Entity->PlaySound (CHAN_VOICE, (random() < 0.5) ? SoundDeath1 : SoundDeath2);
	Entity->DeadFlag = true;
	Entity->CanTakeDamage = true;
	CurrentMove = &HoverMoveDeath1;
}

/*QUAKED monster_hover (1 .5 0) (-16 -16 -24) (16 16 32) Ambush Trigger_Spawn Sight
*/
void CIcarus::Spawn ()
{
	SoundPain1 = SoundIndex ("hover/hovpain1.wav");	
	SoundPain2 = SoundIndex ("hover/hovpain2.wav");	
	SoundDeath1 = SoundIndex ("hover/hovdeth1.wav");	
	SoundDeath2 = SoundIndex ("hover/hovdeth2.wav");	
	SoundSight = SoundIndex ("hover/hovsght1.wav");	
	SoundSearch1 = SoundIndex ("hover/hovsrch1.wav");	
	SoundSearch2 = SoundIndex ("hover/hovsrch2.wav");	

	SoundIndex ("hover/hovatck1.wav");	

	Entity->State.SetSound (SoundIndex ("hover/hovidle1.wav"));

	Entity->SetSolid (SOLID_BBOX);
	Entity->State.SetModelIndex ( ModelIndex("models/monsters/hover/tris.md2"));
	Entity->SetMins (vec3f(-24, -24, -24));
	Entity->SetMaxs (vec3f(24, 24, 32));

	Entity->Health = 240;
	Entity->GibHealth = -100;
	Entity->gameEntity->mass = 150;

	MonsterFlags |= (MF_HAS_ATTACK | MF_HAS_SIGHT | MF_HAS_SEARCH);

	Entity->Link ();

	CurrentMove = &HoverMoveStand;	

	FlyMonsterStart ();
}

LINK_MONSTER_CLASSNAME_TO_CLASS ("monster_hover", CIcarus);