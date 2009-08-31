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
#include "cc_floater.h"

CFloater::CFloater ()
{
	Scale = MODEL_SCALE;
}

void CFloater::Sight ()
{
	Entity->PlaySound (CHAN_VOICE, SoundSight);
}

void CFloater::Idle ()
{
	Entity->PlaySound (CHAN_VOICE, SoundIdle, 1, ATTN_IDLE, 0);
}

void CFloater::FireBlaster ()
{
	vec3_t	start;
	vec3_t	forward, right;
	vec3_t	end;
	vec3_t	dir;
	int		effect = 0;

	switch (Entity->State.GetFrame())
	{
	case FRAME_attak104:
	case FRAME_attak107:
		effect = EF_HYPERBLASTER;
		break;
	}

	vec3_t angles, origin;
	Entity->State.GetAngles(angles);
	Entity->State.GetOrigin(origin);
	Angles_Vectors (angles, forward, right, NULL);
	G_ProjectSource (origin, dumb_and_hacky_monster_MuzzFlashOffset[MZ2_FLOAT_BLASTER_1], forward, right, start);

	Vec3Copy (Entity->gameEntity->enemy->state.origin, end);
	end[2] += Entity->gameEntity->enemy->viewheight;
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
	Entity->PlaySound (CHAN_WEAPON, SoundAttack3);
	CMeleeWeapon::Fire (Entity, aim, 5 + rand() % 6, -50);
}

void CFloater::Zap ()
{
	vec3_t	forward, right;
	vec3_t	origin;
	vec3_t	dir;
	vec3_t	offset;

	vec3_t eOrigin;
	Entity->State.GetOrigin(eOrigin);
	Vec3Subtract (Entity->gameEntity->enemy->state.origin, eOrigin, dir);

	vec3_t angles;
	Entity->State.GetAngles(angles);
	Angles_Vectors (angles, forward, right, NULL);
	//FIXME use a flash and replace these two lines with the commented one
	Vec3Set (offset, 18.5f, -0.9f, 10);

	G_ProjectSource (eOrigin, offset, forward, right, origin);
//	G_ProjectSource (self->state.origin, dumb_and_hacky_monster_MuzzFlashOffset[flash_number], forward, right, origin);

	Entity->PlaySound (CHAN_WEAPON, SoundAttack2);

	//FIXME use the flash, Luke
	CTempEnt_Splashes::Splash (origin, vec3Origin, CTempEnt_Splashes::SPTSparks, 32);

	T_Damage (Entity->gameEntity->enemy, Entity->gameEntity, Entity->gameEntity, vec3Origin, Entity->gameEntity->enemy->state.origin, vec3Origin, 5 + rand() % 6, -10, DAMAGE_ENERGY, MOD_UNKNOWN);
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

void CFloater::Pain (CBaseEntity *other, float kick, int damage)
{
	if (Entity->gameEntity->health < (Entity->gameEntity->max_health / 2))
		Entity->State.SetSkinNum(1);

	if (level.framenum < Entity->gameEntity->pain_debounce_time)
		return;

	Entity->gameEntity->pain_debounce_time = level.framenum + 30;
	if (skill->Integer() == 3)
		return;		// no pain anims in nightmare

	bool n = (random() < 0.5);
	Entity->PlaySound (CHAN_VOICE, n ? SoundPain1 : SoundPain2);
	CurrentMove = n ? &FloaterMovePain1 : &FloaterMovePain2;
}

void CFloater::Die (CBaseEntity *inflictor, CBaseEntity *attacker, int damage, vec3_t point)
{
	Entity->PlaySound (CHAN_VOICE, SoundDeath1);
	Entity->BecomeExplosion (false);
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

	Entity->State.SetSound (SoundIndex ("floater/fltsrch1.wav"));

	Entity->SetSolid (SOLID_BBOX);
	Entity->State.SetModelIndex (ModelIndex ("models/monsters/float/tris.md2"));
	Entity->SetMins (vec3f(-24, -24, -24));
	Entity->SetMaxs (vec3f(24, 24, 32));

	Entity->gameEntity->health = 200;
	Entity->gameEntity->gib_health = -80;
	Entity->gameEntity->mass = 300;

	MonsterFlags |= (MF_HAS_ATTACK | MF_HAS_MELEE | MF_HAS_SIGHT | MF_HAS_IDLE);

	Entity->Link ();

	CurrentMove = (random() <= 0.5) ? &FloaterMoveStand1 : &FloaterMoveStand2;	

	FlyMonsterStart ();
}

LINK_MONSTER_CLASSNAME_TO_CLASS ("monster_floater", CFloater);