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
// cc_floater.cpp
// Floater Monster
//

#include "cc_local.h"
#include "m_float.h"

CFloater Monster_Floater;

CFloater::CFloater ()
{
	Classname = "monster_floater";
	Scale = MODEL_SCALE;
}

void CFloater::Allocate (edict_t *ent)
{
	ent->Monster = QNew (com_levelPool, 0) CFloater(Monster_Floater);
}

void CFloater::Sight ()
{
	PlaySoundFrom (Entity, CHAN_VOICE, SoundSight);
}

void CFloater::Idle ()
{
	PlaySoundFrom (Entity, CHAN_VOICE, SoundIdle, 1, ATTN_IDLE, 0);
}

void CFloater::FireBlaster ()
{
	vec3_t	start;
	vec3_t	forward, right;
	vec3_t	end;
	vec3_t	dir;
	int		effect = 0;

	switch (Entity->state.frame)
	{
	case FRAME_attak104:
	case FRAME_attak107:
		effect = EF_HYPERBLASTER;
		break;
	}

	Angles_Vectors (Entity->state.angles, forward, right, NULL);
	G_ProjectSource (Entity->state.origin, dumb_and_hacky_monster_MuzzFlashOffset[MZ2_FLOAT_BLASTER_1], forward, right, start);

	Vec3Copy (Entity->enemy->state.origin, end);
	end[2] += Entity->enemy->viewheight;
	Vec3Subtract (end, start, dir);

	MonsterFireBlaster (start, dir, 1, 1000, MZ2_FLOAT_BLASTER_1, effect);
}

CFrame FloaterFramesStand1 [] =
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
	CFrame (&CMonster::AI_Stand, 0)
};
CAnim FloaterMoveStand1 (FRAME_stand101, FRAME_stand152, FloaterFramesStand1);

CFrame FloaterFramesStand2 [] =
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
	CFrame (&CMonster::AI_Stand, 0)
};
CAnim FloaterMoveStand2 (FRAME_stand201, FRAME_stand252, FloaterFramesStand2);

void CFloater::Stand ()
{
	CurrentMove = (random() <= 0.5) ? &FloaterMoveStand1 : &FloaterMoveStand2;
}

CFrame FloaterFramesAttack1 [] =
{
	CFrame (&CMonster::AI_Charge,	0),			// Blaster attack
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0,	ConvertDerivedFunction(&CFloater::FireBlaster)),			// BOOM (0, -25.8, 32.5)	-- LOOP Starts
	CFrame (&CMonster::AI_Charge,	0,	ConvertDerivedFunction(&CFloater::FireBlaster)),
	CFrame (&CMonster::AI_Charge,	0,	ConvertDerivedFunction(&CFloater::FireBlaster)),
	CFrame (&CMonster::AI_Charge,	0,	ConvertDerivedFunction(&CFloater::FireBlaster)),
	CFrame (&CMonster::AI_Charge,	0,	ConvertDerivedFunction(&CFloater::FireBlaster)),
	CFrame (&CMonster::AI_Charge,	0,	ConvertDerivedFunction(&CFloater::FireBlaster)),
	CFrame (&CMonster::AI_Charge,	0,	ConvertDerivedFunction(&CFloater::FireBlaster)),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0)			//							-- LOOP Ends
};
CAnim FloaterMoveAttack1 (FRAME_attak101, FRAME_attak114, FloaterFramesAttack1, ConvertDerivedFunction(&CFloater::Run));

#ifdef MONSTER_USE_ROGUE_AI
CFrame FloaterFramesAttack1a [] =
{
	CFrame (&CMonster::AI_Charge,	10),			// Blaster attack
	CFrame (&CMonster::AI_Charge,	10),
	CFrame (&CMonster::AI_Charge,	10),
	CFrame (&CMonster::AI_Charge,	10,	ConvertDerivedFunction(&CFloater::FireBlaster)),			// BOOM (0, -25.8, 32.5)	-- LOOP Starts
	CFrame (&CMonster::AI_Charge,	10,	ConvertDerivedFunction(&CFloater::FireBlaster)),
	CFrame (&CMonster::AI_Charge,	10,	ConvertDerivedFunction(&CFloater::FireBlaster)),
	CFrame (&CMonster::AI_Charge,	10,	ConvertDerivedFunction(&CFloater::FireBlaster)),
	CFrame (&CMonster::AI_Charge,	10,	ConvertDerivedFunction(&CFloater::FireBlaster)),
	CFrame (&CMonster::AI_Charge,	10,	ConvertDerivedFunction(&CFloater::FireBlaster)),
	CFrame (&CMonster::AI_Charge,	10,	ConvertDerivedFunction(&CFloater::FireBlaster)),
	CFrame (&CMonster::AI_Charge,	10),
	CFrame (&CMonster::AI_Charge,	10),
	CFrame (&CMonster::AI_Charge,	10),
	CFrame (&CMonster::AI_Charge,	10)			//							-- LOOP Ends
};
CAnim FloaterMoveAttack1a (FRAME_attak101, FRAME_attak114, FloaterFramesAttack1a, ConvertDerivedFunction(&CFloater::Run));
#endif

CFrame FloaterFramesAttack2 [] =
{
	CFrame (&CMonster::AI_Charge,	0),			// Claws
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),			// WHAM (0, -45, 29.6)		-- LOOP Starts
	CFrame (&CMonster::AI_Charge,	0, ConvertDerivedFunction(&CFloater::Wham)),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),			//							-- LOOP Ends
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0)
};
CAnim FloaterMoveAttack2 (FRAME_attak201, FRAME_attak225, FloaterFramesAttack2, ConvertDerivedFunction(&CFloater::Run));

CFrame FloaterFramesAttack3 [] =
{
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0, ConvertDerivedFunction(&CFloater::Zap)),		//								-- LOOP Starts
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),		//								-- LOOP Ends
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0)
};
CAnim FloaterMoveAttack3 (FRAME_attak301, FRAME_attak334, FloaterFramesAttack3, ConvertDerivedFunction(&CFloater::Run));

CFrame FloaterFramesPain1 [] =
{
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0)
};
CAnim FloaterMovePain1 (FRAME_pain101, FRAME_pain107, FloaterFramesPain1, ConvertDerivedFunction(&CFloater::Run));

CFrame FloaterFramesPain2 [] =
{
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0)
};
CAnim FloaterMovePain2 (FRAME_pain201, FRAME_pain208, FloaterFramesPain2, ConvertDerivedFunction(&CFloater::Run));

CFrame FloaterFramesPain3 [] =
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
CAnim FloaterMovePain3 (FRAME_pain301, FRAME_pain312, FloaterFramesPain3, ConvertDerivedFunction(&CFloater::Run));

CFrame FloaterFramesWalk [] =
{
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 5)
};
CAnim FloaterMoveWalk (FRAME_stand101, FRAME_stand152, FloaterFramesWalk);

CFrame FloaterFramesRun [] =
{
	CFrame (&CMonster::AI_Run, 13),
	CFrame (&CMonster::AI_Run, 13),
	CFrame (&CMonster::AI_Run, 13),
	CFrame (&CMonster::AI_Run, 13),
	CFrame (&CMonster::AI_Run, 13),
	CFrame (&CMonster::AI_Run, 13),
	CFrame (&CMonster::AI_Run, 13),
	CFrame (&CMonster::AI_Run, 13),
	CFrame (&CMonster::AI_Run, 13),
	CFrame (&CMonster::AI_Run, 13),
	CFrame (&CMonster::AI_Run, 13),
	CFrame (&CMonster::AI_Run, 13),
	CFrame (&CMonster::AI_Run, 13),
	CFrame (&CMonster::AI_Run, 13),
	CFrame (&CMonster::AI_Run, 13),
	CFrame (&CMonster::AI_Run, 13),
	CFrame (&CMonster::AI_Run, 13),
	CFrame (&CMonster::AI_Run, 13),
	CFrame (&CMonster::AI_Run, 13),
	CFrame (&CMonster::AI_Run, 13),
	CFrame (&CMonster::AI_Run, 13),
	CFrame (&CMonster::AI_Run, 13),
	CFrame (&CMonster::AI_Run, 13),
	CFrame (&CMonster::AI_Run, 13),
	CFrame (&CMonster::AI_Run, 13),
	CFrame (&CMonster::AI_Run, 13),
	CFrame (&CMonster::AI_Run, 13),
	CFrame (&CMonster::AI_Run, 13),
	CFrame (&CMonster::AI_Run, 13),
	CFrame (&CMonster::AI_Run, 13),
	CFrame (&CMonster::AI_Run, 13),
	CFrame (&CMonster::AI_Run, 13),
	CFrame (&CMonster::AI_Run, 13),
	CFrame (&CMonster::AI_Run, 13),
	CFrame (&CMonster::AI_Run, 13),
	CFrame (&CMonster::AI_Run, 13),
	CFrame (&CMonster::AI_Run, 13),
	CFrame (&CMonster::AI_Run, 13),
	CFrame (&CMonster::AI_Run, 13),
	CFrame (&CMonster::AI_Run, 13),
	CFrame (&CMonster::AI_Run, 13),
	CFrame (&CMonster::AI_Run, 13),
	CFrame (&CMonster::AI_Run, 13),
	CFrame (&CMonster::AI_Run, 13),
	CFrame (&CMonster::AI_Run, 13),
	CFrame (&CMonster::AI_Run, 13),
	CFrame (&CMonster::AI_Run, 13),
	CFrame (&CMonster::AI_Run, 13),
	CFrame (&CMonster::AI_Run, 13),
	CFrame (&CMonster::AI_Run, 13),
	CFrame (&CMonster::AI_Run, 13),
	CFrame (&CMonster::AI_Run, 13)
};
CAnim FloaterMoveRun (FRAME_stand101, FRAME_stand152, FloaterFramesRun);

void CFloater::Run ()
{
	CurrentMove = (AIFlags & AI_STAND_GROUND) ? &FloaterMoveStand1 : &FloaterMoveRun;
}

void CFloater::Walk ()
{
	CurrentMove = &FloaterMoveWalk;
}

void CFloater::Wham ()
{
	static	vec3_t	aim = {MELEE_DISTANCE, 0, 0};
	PlaySoundFrom (Entity, CHAN_WEAPON, SoundAttack3);
	fire_hit (Entity, aim, 5 + rand() % 6, -50);
}

void CFloater::Zap ()
{
	vec3_t	forward, right;
	vec3_t	origin;
	vec3_t	dir;
	vec3_t	offset;

	Vec3Subtract (Entity->enemy->state.origin, Entity->state.origin, dir);

	Angles_Vectors (Entity->state.angles, forward, right, NULL);
	//FIXME use a flash and replace these two lines with the commented one
	Vec3Set (offset, 18.5f, -0.9f, 10);
	G_ProjectSource (Entity->state.origin, offset, forward, right, origin);
//	G_ProjectSource (self->state.origin, dumb_and_hacky_monster_MuzzFlashOffset[flash_number], forward, right, origin);

	PlaySoundFrom (Entity, CHAN_WEAPON, SoundAttack2);

	//FIXME use the flash, Luke
	CTempEnt_Splashes::Splash (origin, vec3Origin, CTempEnt_Splashes::SPTSparks, 32);

	T_Damage (Entity->enemy, Entity, Entity, vec3Origin, Entity->enemy->state.origin, vec3Origin, 5 + rand() % 6, -10, DAMAGE_ENERGY, MOD_UNKNOWN);
}

void CFloater::Attack()
{
#ifndef MONSTER_USE_ROGUE_AI
	CurrentMove = &FloaterMoveAttack1;
#else
	float chance = (!skill->Integer()) ? 0 : 1.0 - (0.5/skill->Float());

	// 0% chance of circle in easy
	// 50% chance in normal
	// 75% chance in hard
	// 86.67% chance in nightmare

	if (random() > chance)
	{
		AttackState = AS_STRAIGHT;
		CurrentMove = &FloaterMoveAttack1;
	}
	else // circle strafe
	{
		if (random () <= 0.5) // switch directions
			Lefty = !Lefty;
		AttackState = AS_SLIDING;
		CurrentMove = &FloaterMoveAttack1a;
	}
#endif
}

void CFloater::Melee ()
{
	CurrentMove = (random() < 0.5) ? &FloaterMoveAttack3 : &FloaterMoveAttack2;
}

void CFloater::Pain (edict_t *other, float kick, int damage)
{
	if (Entity->health < (Entity->max_health / 2))
		Entity->state.skinNum = 1;

	if (level.framenum < Entity->pain_debounce_time)
		return;

	Entity->pain_debounce_time = level.framenum + 30;
	if (skill->Integer() == 3)
		return;		// no pain anims in nightmare

	bool n = (random() < 0.5);
	PlaySoundFrom (Entity, CHAN_VOICE, n ? SoundPain1 : SoundPain2);
	CurrentMove = n ? &FloaterMovePain1 : &FloaterMovePain2;
}

void CFloater::Die (edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
{
	PlaySoundFrom (Entity, CHAN_VOICE, SoundDeath1);
	BecomeExplosion1(Entity);
}

void CFloater::Spawn ()
{
	SoundAttack2 = SoundIndex ("floater/fltatck2.wav");
	SoundAttack3 = SoundIndex ("floater/fltatck3.wav");
	SoundDeath1 = SoundIndex ("floater/fltdeth1.wav");
	SoundIdle = SoundIndex ("floater/fltidle1.wav");
	SoundPain1 = SoundIndex ("floater/fltpain1.wav");
	SoundPain2 = SoundIndex ("floater/fltpain2.wav");
	SoundSight = SoundIndex ("floater/fltsght1.wav");

	SoundIndex ("floater/fltatck1.wav");

	Entity->state.sound = SoundIndex ("floater/fltsrch1.wav");

	Entity->movetype = MOVETYPE_STEP;
	Entity->solid = SOLID_BBOX;
	Entity->state.modelIndex = ModelIndex ("models/monsters/float/tris.md2");
	Vec3Set (Entity->mins, -24, -24, -24);
	Vec3Set (Entity->maxs, 24, 24, 32);

	Entity->health = 200;
	Entity->gib_health = -80;
	Entity->mass = 300;

	MonsterFlags |= (MF_HAS_ATTACK | MF_HAS_MELEE | MF_HAS_SIGHT | MF_HAS_IDLE);

	gi.linkentity (Entity);

	CurrentMove = (random() <= 0.5) ? &FloaterMoveStand1 : &FloaterMoveStand2;	

	FlyMonsterStart ();
}
