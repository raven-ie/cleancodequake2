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
// cc_makron.cpp
// 
//

#include "cc_local.h"
#include "cc_jorg.h" // Req.
#include "cc_makron.h"
#include "m_boss32.h"

CMakron::CMakron ()
{
	Scale = MODEL_SCALE;
}

void CMakron::Taunt ()
{
	MediaIndex Sound = SoundTaunt1;
	switch (rand() % 3)
	{
	case 0:
		break;
	case 1:
		Sound = SoundTaunt2;
		break;
	case 2:
	default:
		Sound = SoundTaunt3;
		break;
	};

	Entity->PlaySound (CHAN_AUTO, Sound, 1, ATTN_NONE, 0);
}

//
// stand
//

CFrame MakronFramesStand []=
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
	CFrame (&CMonster::AI_Stand, 0),		// 10
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),		// 20
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),		// 30
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),		// 40
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),		// 50
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),		// 60
};
CAnim	MakronMoveStand (FRAME_stand201, FRAME_stand260, MakronFramesStand);
	
void CMakron::Stand ()
{
	CurrentMove = &MakronMoveStand;
}

void CMakron::Hit ()
{
	Entity->PlaySound (CHAN_AUTO, SoundHit, 1, ATTN_NONE,0);
}

void CMakron::PopUp ()
{
	Entity->PlaySound (CHAN_BODY, SoundPopUp, 1, ATTN_NONE,0);
}

void CMakron::StepLeft ()
{
	Entity->PlaySound (CHAN_BODY, SoundStepLeft, 1, ATTN_NORM,0);
}

void CMakron::StepRight ()
{
	Entity->PlaySound (CHAN_BODY, SoundStepRight, 1, ATTN_NORM,0);
}

void CMakron::BrainSplorch ()
{
	Entity->PlaySound (CHAN_VOICE, SoundBrainSplorch, 1, ATTN_NORM,0);
}

void CMakron::PreRailgun ()
{
	Entity->PlaySound (CHAN_WEAPON, SoundPreRailgun, 1, ATTN_NORM,0);
}

CFrame MakronFramesRun [] =
{
	CFrame (&CMonster::AI_Run, 3,	ConvertDerivedFunction(&CMakron::StepLeft)),
	CFrame (&CMonster::AI_Run, 12),
	CFrame (&CMonster::AI_Run, 8),
	CFrame (&CMonster::AI_Run, 8),
	CFrame (&CMonster::AI_Run, 8,	ConvertDerivedFunction(&CMakron::StepRight)),
	CFrame (&CMonster::AI_Run, 6),
	CFrame (&CMonster::AI_Run, 12),
	CFrame (&CMonster::AI_Run, 9),
	CFrame (&CMonster::AI_Run, 6),
	CFrame (&CMonster::AI_Run, 12)
};
CAnim	MakronMoveRun (FRAME_walk204, FRAME_walk213, MakronFramesRun);

CFrame MakronFramesWalk [] =
{
	CFrame (&CMonster::AI_Walk, 3,	ConvertDerivedFunction(&CMakron::StepLeft)),
	CFrame (&CMonster::AI_Walk, 12),
	CFrame (&CMonster::AI_Walk, 8),
	CFrame (&CMonster::AI_Walk, 8),
	CFrame (&CMonster::AI_Walk, 8,	ConvertDerivedFunction(&CMakron::StepRight)),
	CFrame (&CMonster::AI_Walk, 6),
	CFrame (&CMonster::AI_Walk, 12),
	CFrame (&CMonster::AI_Walk, 9),
	CFrame (&CMonster::AI_Walk, 6),
	CFrame (&CMonster::AI_Walk, 12)
};
CAnim	MakronMoveWalk (FRAME_walk204, FRAME_walk213, MakronFramesRun);

void CMakron::Walk ()
{
	CurrentMove = &MakronMoveWalk;
}

void CMakron::Run ()
{
	CurrentMove = (AIFlags & AI_STAND_GROUND) ? &MakronMoveStand : &MakronMoveRun;
}

CFrame MakronFramesPain6 [] =
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
	CFrame (&CMonster::AI_Move,	0),		// 10
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0,	ConvertDerivedFunction(&CMakron::PopUp)),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),		// 20
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0,	ConvertDerivedFunction(&CMakron::Taunt)),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0)
};
CAnim MakronMovePain6 (FRAME_pain601, FRAME_pain627, MakronFramesPain6, &CMonster::Run);

CFrame MakronFramesPain5 [] =
{
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0)
};
CAnim MakronMovePain5 (FRAME_pain501, FRAME_pain504, MakronFramesPain5, &CMonster::Run);

CFrame MakronFramesPain4 [] =
{
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0)
};
CAnim MakronMovePain4 (FRAME_pain401, FRAME_pain404, MakronFramesPain4, &CMonster::Run);

void CMakron::Pain (CBaseEntity *other, float kick, int damage)
{
	if (Entity->gameEntity->health < (Entity->gameEntity->max_health / 2))
			Entity->State.SetSkinNum (1);

	if (level.framenum < Entity->gameEntity->pain_debounce_time)
			return;

	// Lessen the chance of him going into his pain frames
	if ((damage <= 25) && (random() < 0.2f))
		return;

	Entity->gameEntity->pain_debounce_time = level.framenum + 30;
	if (skill->Integer() == 3)
		return;		// no pain anims in nightmare

	if (damage <= 40)
	{
		Entity->PlaySound (CHAN_VOICE, SoundPain4, 1, ATTN_NONE,0);
		CurrentMove = &MakronMovePain4;
	}
	else if (damage <= 110)
	{
		Entity->PlaySound (CHAN_VOICE, SoundPain5, 1, ATTN_NONE,0);
		CurrentMove = &MakronMovePain5;
	}
	else
	{
		if ((damage <= 150) && (random() <= 0.45f))
		{
			Entity->PlaySound (CHAN_VOICE, SoundPain6, 1, ATTN_NONE,0);
			CurrentMove = &MakronMovePain6;
		}
		else if (random() <= 0.35f)
		{
			Entity->PlaySound (CHAN_VOICE, SoundPain6, 1, ATTN_NONE,0);
			CurrentMove = &MakronMovePain6;
		}
	}
};

CFrame MakronFramesDeath2 [] =
{
	CFrame (&CMonster::AI_Move,	-15),
	CFrame (&CMonster::AI_Move,	3),
	CFrame (&CMonster::AI_Move,	-12),
	CFrame (&CMonster::AI_Move,	0,	ConvertDerivedFunction(&CMakron::StepLeft)),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),			// 10
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	11),
	CFrame (&CMonster::AI_Move,	12),
	CFrame (&CMonster::AI_Move,	11,	ConvertDerivedFunction(&CMakron::StepRight)),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),			// 20
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),			
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),			// 30
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	5),
	CFrame (&CMonster::AI_Move,	7),
	CFrame (&CMonster::AI_Move,	6,	ConvertDerivedFunction(&CMakron::StepLeft)),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	-1),
	CFrame (&CMonster::AI_Move,	2),			// 40
	CFrame (&CMonster::AI_Move,	0),			
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),			
	CFrame (&CMonster::AI_Move,	0),			// 50
	CFrame (&CMonster::AI_Move,	0),			
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	-6),
	CFrame (&CMonster::AI_Move,	-4),
	CFrame (&CMonster::AI_Move,	-6,	ConvertDerivedFunction(&CMakron::StepRight)),
	CFrame (&CMonster::AI_Move,	-4),
	CFrame (&CMonster::AI_Move,	-4,	ConvertDerivedFunction(&CMakron::StepLeft)),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),			// 60
	CFrame (&CMonster::AI_Move,	0),			
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	-2),
	CFrame (&CMonster::AI_Move,	-5),
	CFrame (&CMonster::AI_Move,	-3,	ConvertDerivedFunction(&CMakron::StepRight)),
	CFrame (&CMonster::AI_Move,	-8),
	CFrame (&CMonster::AI_Move,	-3,	ConvertDerivedFunction(&CMakron::StepLeft)),
	CFrame (&CMonster::AI_Move,	-7),
	CFrame (&CMonster::AI_Move,	-4),
	CFrame (&CMonster::AI_Move,	-4,	ConvertDerivedFunction(&CMakron::StepRight)),			// 70
	CFrame (&CMonster::AI_Move,	-6),			
	CFrame (&CMonster::AI_Move,	-7),
	CFrame (&CMonster::AI_Move,	0,	ConvertDerivedFunction(&CMakron::StepLeft)),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),			// 80
	CFrame (&CMonster::AI_Move,	0),			
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	-2),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	2),
	CFrame (&CMonster::AI_Move,	0),			// 90
	CFrame (&CMonster::AI_Move,	27,	ConvertDerivedFunction(&CMakron::Hit)),			
	CFrame (&CMonster::AI_Move,	26),
	CFrame (&CMonster::AI_Move,	0,	ConvertDerivedFunction(&CMakron::BrainSplorch)),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0)			// 95
};
CAnim MakronMoveDeath2 (FRAME_death201, FRAME_death295, MakronFramesDeath2, ConvertDerivedFunction(&CMakron::Dead));

CFrame MakronFramesDeath3 [] =
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
	CFrame (&CMonster::AI_Move,	0)
};
CAnim MakronMoveDeath3 (FRAME_death301, FRAME_death320, MakronFramesDeath3);

CFrame MakronFramesSight [] =
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
	CFrame (&CMonster::AI_Move,	0)
};
CAnim MakronMoveSight (FRAME_active01, FRAME_active13, MakronFramesSight, &CMonster::Run);

void CMakron::FireBFG ()
{
	vec3f	forward, right;
	vec3f	start;
	vec3f	dir;
	vec3f	vec;

	Entity->State.GetAngles().ToVectors (&forward, &right, NULL);
	G_ProjectSource (Entity->State.GetOrigin(), dumb_and_hacky_monster_MuzzFlashOffset[MZ2_MAKRON_BFG], forward, right, start);

	vec = vec3f(Entity->gameEntity->enemy->state.origin);
	vec.Z += Entity->gameEntity->enemy->viewheight;
	dir = vec - start;
	dir.Normalize();

	Entity->PlaySound (CHAN_VOICE, SoundAttackBfg, 1, ATTN_NORM, 0);
	MonsterFireBfg (start, dir, 50, 300, 100, 300, MZ2_MAKRON_BFG);
}	

CFrame MakronFramesAttack3 []=
{
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0,	ConvertDerivedFunction(&CMakron::FireBFG)),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0)
};
CAnim MakronMoveAttack3 (FRAME_attak301, FRAME_attak308, MakronFramesAttack3, &CMonster::Run);

CFrame MakronFramesAttack4[]=
{
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Move,	0,	ConvertDerivedFunction(&CMakron::FireHyperblaster)),		// fire
	CFrame (&CMonster::AI_Move,	0,	ConvertDerivedFunction(&CMakron::FireHyperblaster)),		// fire
	CFrame (&CMonster::AI_Move,	0,	ConvertDerivedFunction(&CMakron::FireHyperblaster)),		// fire
	CFrame (&CMonster::AI_Move,	0,	ConvertDerivedFunction(&CMakron::FireHyperblaster)),		// fire
	CFrame (&CMonster::AI_Move,	0,	ConvertDerivedFunction(&CMakron::FireHyperblaster)),		// fire
	CFrame (&CMonster::AI_Move,	0,	ConvertDerivedFunction(&CMakron::FireHyperblaster)),		// fire
	CFrame (&CMonster::AI_Move,	0,	ConvertDerivedFunction(&CMakron::FireHyperblaster)),		// fire
	CFrame (&CMonster::AI_Move,	0,	ConvertDerivedFunction(&CMakron::FireHyperblaster)),		// fire
	CFrame (&CMonster::AI_Move,	0,	ConvertDerivedFunction(&CMakron::FireHyperblaster)),		// fire
	CFrame (&CMonster::AI_Move,	0,	ConvertDerivedFunction(&CMakron::FireHyperblaster)),		// fire
	CFrame (&CMonster::AI_Move,	0,	ConvertDerivedFunction(&CMakron::FireHyperblaster)),		// fire
	CFrame (&CMonster::AI_Move,	0,	ConvertDerivedFunction(&CMakron::FireHyperblaster)),		// fire
	CFrame (&CMonster::AI_Move,	0,	ConvertDerivedFunction(&CMakron::FireHyperblaster)),		// fire
	CFrame (&CMonster::AI_Move,	0,	ConvertDerivedFunction(&CMakron::FireHyperblaster)),		// fire
	CFrame (&CMonster::AI_Move,	0,	ConvertDerivedFunction(&CMakron::FireHyperblaster)),		// fire
	CFrame (&CMonster::AI_Move,	0,	ConvertDerivedFunction(&CMakron::FireHyperblaster)),		// fire
	CFrame (&CMonster::AI_Move,	0,	ConvertDerivedFunction(&CMakron::FireHyperblaster)),		// fire
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0)
};
CAnim MakronMoveAttack4 (FRAME_attak401, FRAME_attak426, MakronFramesAttack4, &CMonster::Run);

// FIXME: This is all wrong. He's not firing at the proper angles.
void CMakron::FireHyperblaster ()
{
	vec3f	dir;
	vec3f	start;
	vec3f	forward, right;
	int		flash_number = MZ2_MAKRON_BLASTER_1 + (Entity->State.GetFrame() - FRAME_attak405);

	Entity->State.GetAngles().ToVectors(&forward, &right, NULL);
	G_ProjectSource (Entity->State.GetOrigin(), dumb_and_hacky_monster_MuzzFlashOffset[flash_number], forward, right, start);

	if (Entity->gameEntity->enemy)
	{
		vec3f vec = vec3f (Entity->gameEntity->enemy->state.origin);
		vec.Z += Entity->gameEntity->enemy->viewheight;
		vec = vec - start;
		vec = vec.ToAngles();
		dir.X = vec.X;
	}
	else
		dir.X = 0;

	if (Entity->State.GetFrame() <= FRAME_attak413)
		dir.Y = Entity->State.GetAngles().Y - 10 * (Entity->State.GetFrame() - FRAME_attak413);
	else
		dir.Y = Entity->State.GetAngles().Y + 10 * (Entity->State.GetFrame() - FRAME_attak421);
	dir.Z = 0;

	dir.ToVectors (&forward, NULL, NULL);
	MonsterFireBlaster (start, forward, 15, 1000, MZ2_MAKRON_BLASTER_1, EF_BLASTER);
}	

CFrame MakronFramesAttack5[]=
{
	CFrame (&CMonster::AI_Charge,	0,	ConvertDerivedFunction(&CMakron::PreRailgun)),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0,	ConvertDerivedFunction(&CMakron::SavePosition)),
	CFrame (&CMonster::AI_Move,	0,	ConvertDerivedFunction(&CMakron::FireRailgun)),		// Fire railgun
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0)
};
CAnim MakronMoveAttack5 (FRAME_attak501, FRAME_attak516, MakronFramesAttack5, &CMonster::Run);

void CMakron::SavePosition ()
{
	SavedLoc = vec3f(Entity->gameEntity->enemy->state.origin);
	SavedLoc.Z += Entity->gameEntity->enemy->viewheight;
};

// FIXME: He's not firing from the proper Z
void CMakron::FireRailgun ()
{
	vec3f	start;
	vec3f	dir;
	vec3f	forward, right;

	Entity->State.GetAngles().ToVectors (&forward, &right, NULL);
	G_ProjectSource (Entity->State.GetOrigin(), dumb_and_hacky_monster_MuzzFlashOffset[MZ2_MAKRON_RAILGUN_1], forward, right, start);
	
	// calc direction to where we targted
	dir = SavedLoc - start;
	dir.Normalize();

	MonsterFireRailgun (start, dir, 50, 100, MZ2_MAKRON_RAILGUN_1);
}

void CMakron::Sight()
{
	CurrentMove = &MakronMoveSight;
};

void CMakron::Attack()
{
	switch (rand()%3)
	{
	case 0:
		CurrentMove = &MakronMoveAttack3;
		break;
	case 1:
		CurrentMove = &MakronMoveAttack4;
		break;
	case 2:
	default:
		CurrentMove = &MakronMoveAttack5;
		break;
	};
}

/*
---
Makron Torso. This needs to be spawned in
---
*/

class CMakronTorso : public virtual CBaseEntity, public CThinkableEntity
{
public:
	CMakronTorso () :
	  CBaseEntity(),
	  CThinkableEntity()
	  {
	  };

	CMakronTorso (int Index) :
	  CBaseEntity(Index),
	  CThinkableEntity (Index)
	  {
	  };

	void Think ()
	{
		State.SetFrame (State.GetFrame() + 1);
		if (State.GetFrame() < 365)
			NextThink = level.framenum + FRAMETIME;
		else
		{		
			State.SetFrame (346);
			NextThink = level.framenum + FRAMETIME;
		}
	};

	static void Spawn (CMonsterEntity *Owner)
	{
		CMakronTorso *NewClass = QNew (com_levelPool, 0) CMakronTorso;

		NewClass->State.SetOrigin (Owner->State.GetOrigin());
		NewClass->State.SetAngles (Owner->State.GetAngles());
		vec3f origin = Owner->State.GetOrigin();
		origin.Y -= 84;

		NewClass->SetMins (vec3f(-8, -8, 0));
		NewClass->SetMaxs (vec3f(8, 8, 8));
		NewClass->State.SetModelIndex (Owner->State.GetModelIndex());
		NewClass->NextThink = level.framenum + 2;
		NewClass->State.SetSound (SoundIndex("makron/spine.wav"));
		NewClass->State.SetFrame (346);
		NewClass->SetSolid (SOLID_NOT);
		NewClass->Link ();
	};
};

//
// death
//

void CMakron::Dead ()
{
	Entity->SetMins (vec3f(-60, -60, 0));
	Entity->SetMaxs (vec3f(60, 60, 72));
	Entity->PhysicsType = PHYSICS_TOSS;
	Entity->SetSvFlags (Entity->GetSvFlags() | SVF_DEADMONSTER);
	Entity->NextThink = 0;
	Entity->Link ();
}

void CMakron::Die(CBaseEntity *inflictor, CBaseEntity *attacker, int damage, vec3_t point)
{
	Entity->State.SetSound (0);
	// check for gib
	if (Entity->gameEntity->health <= Entity->gameEntity->gib_health)
	{
		Entity->PlaySound (CHAN_VOICE, SoundIndex ("misc/udeath.wav"), 1, ATTN_NORM, 0);
		for (int n= 0; n < 1 /*4*/; n++)
			CGibEntity::Spawn (Entity, gMedia.Gib_SmallMeat, damage, GIB_ORGANIC);
		for (int n= 0; n < 4; n++)
			CGibEntity::Spawn (Entity, gMedia.Gib_SmallMetal, damage, GIB_METALLIC);
		Entity->ThrowHead (gMedia.Gib_Gear, damage, GIB_METALLIC);
		Entity->gameEntity->deadflag = DEAD_DEAD;
		return;
	}

	if (Entity->gameEntity->deadflag == DEAD_DEAD)
		return;

// regular death
	Entity->PlaySound (CHAN_VOICE, SoundDeath, 1, ATTN_NONE, 0);
	Entity->gameEntity->deadflag = DEAD_DEAD;
	Entity->gameEntity->takedamage = true;

	// Spawn torso
	CMakronTorso::Spawn (Entity);

	CurrentMove = &MakronMoveDeath2;
}

bool CMakron::CheckAttack ()
{
#ifndef MONSTER_USE_ROGUE_AI
	vec3_t	spot1, spot2;
	float	chance;
	CTrace	tr;

	if (Entity->gameEntity->enemy->health > 0)
	{
	// see if any entities are in the way of the shot
		Entity->State.GetOrigin(spot1);
		spot1[2] += Entity->gameEntity->viewheight;
		Vec3Copy (Entity->gameEntity->enemy->state.origin, spot2);
		spot2[2] += Entity->gameEntity->enemy->viewheight;

		tr = CTrace(spot1, spot2, Entity->gameEntity, CONTENTS_SOLID|CONTENTS_MONSTER|CONTENTS_SLIME|CONTENTS_LAVA|CONTENTS_WINDOW);

		// do we have a clear shot?
		if (tr.ent != Entity->gameEntity->enemy)
			return false;
	}
	
	// melee attack
	if (EnemyRange == RANGE_MELEE)
	{
		// don't always melee in easy mode
		if (skill->Integer() == 0 && (rand()&3) )
			return false;
		if (MonsterFlags & MF_HAS_MELEE)
			AttackState = AS_MELEE;
		else
			AttackState = AS_MISSILE;
		return true;
	}
	
// missile attack
	if (!(MonsterFlags & MF_HAS_ATTACK))
		return false;
		
	if (level.framenum < AttackFinished)
		return false;
		
	if (EnemyRange == RANGE_FAR)
		return false;

	if (AIFlags & AI_STAND_GROUND)
	{
		chance = 0.4f;
	}
	else if (EnemyRange == RANGE_MELEE)
	{
		chance = 0.8f;
	}
	else if (EnemyRange == RANGE_NEAR)
	{
		chance = 0.4f;
	}
	else if (EnemyRange == RANGE_MID)
	{
		chance = 0.2f;
	}
	else
	{
		return false;
	}

	if (skill->Integer() == 0)
		chance *= 0.5;
	else if (skill->Integer() >= 2)
		chance *= 2;

	if (random () < chance)
	{
		AttackState = AS_MISSILE;
		AttackFinished = level.framenum + ((2*random())*10);
		return true;
	}

	if (Entity->gameEntity->flags & FL_FLY)
	{
		if (random() < 0.3)
			AttackState = AS_SLIDING;
		else
			AttackState = AS_STRAIGHT;
	}

	return false;
#else
	float	chance;

	if (Entity->gameEntity->enemy->health > 0)
	{
	// see if any entities are in the way of the shot
		vec3_t	spot1, spot2;
		Entity->State.GetOrigin (spot1);
		spot1[2] += Entity->gameEntity->viewheight;
		Vec3Copy (Entity->gameEntity->enemy->state.origin, spot2);
		spot2[2] += Entity->gameEntity->enemy->viewheight;

		CTrace tr (spot1, spot2, Entity->gameEntity, CONTENTS_SOLID|CONTENTS_MONSTER|CONTENTS_SLIME|CONTENTS_LAVA|CONTENTS_WINDOW);

		// do we have a clear shot?
		if (tr.ent != Entity->gameEntity->enemy)
		{	
			// PGM - we want them to go ahead and shoot at info_notnulls if they can.
			if(Entity->gameEntity->enemy->solid != SOLID_NOT || tr.fraction < 1.0)		//PGM
			{
				// PMM - if we can't see our target, and we're not blocked by a monster, go into blind fire if available
				if ((!(tr.ent->svFlags & SVF_MONSTER)) && (!visible(Entity->gameEntity, Entity->gameEntity->enemy)))
				{
					if ((BlindFire) && (BlindFireDelay <= 20.0))
					{
						if (level.framenum < AttackFinished)
							return false;
						if (level.framenum < (TrailTime + BlindFireDelay))
							// wait for our time
							return false;
						else
						{
							// make sure we're not going to shoot a monster
							tr = CTrace (spot1, BlindFireTarget, Entity->gameEntity, CONTENTS_MONSTER);
							if (tr.allSolid || tr.startSolid || ((tr.fraction < 1.0) && (tr.ent != Entity->gameEntity->enemy)))
								return false;

							AttackState = AS_BLIND;
							return true;
						}
					}
				}
				// pmm
				return false;
			}
		}
	}
	
	// melee attack
	if (EnemyRange == RANGE_MELEE)
	{
		// don't always melee in easy mode
		if (skill->Integer() == 0 && (rand()&3) )
		{
			// PMM - fix for melee only monsters & strafing
			AttackState = AS_STRAIGHT;
			return false;
		}
		if (MonsterFlags & MF_HAS_MELEE)
			AttackState = AS_MELEE;
		else
			AttackState = AS_MISSILE;
		return true;
	}
	
// missile attack
	if (!(MonsterFlags & MF_HAS_ATTACK))
	{
		// PMM - fix for melee only monsters & strafing
		AttackState = AS_STRAIGHT;
		return false;
	}
	
	if (level.framenum < AttackFinished)
		return false;
		
	if (EnemyRange == RANGE_FAR)
		return false;

	if (AIFlags & AI_STAND_GROUND)
		chance = 0.4f;
	else if (EnemyRange == RANGE_MELEE)
		chance = 0.8f;
	else if (EnemyRange == RANGE_NEAR)
		chance = 0.4f;
	else if (EnemyRange == RANGE_MID)
		chance = 0.2f;
	else
		return false;

	if (skill->Integer() == 0)
		chance *= 0.5;
	else if (skill->Integer() >= 2)
		chance *= 2;

	// PGM - go ahead and shoot every time if it's a info_notnull
	if ((random () < chance) || (Entity->gameEntity->enemy->solid == SOLID_NOT))
	{
		AttackState = AS_MISSILE;
		AttackFinished = level.framenum + ((2*random())*10);
		return true;
	}

	// PMM -daedalus should strafe more .. this can be done here or in a customized
	// check_attack code for the hover.
	if (Entity->gameEntity->flags & FL_FLY)
	{
		// originally, just 0.3
		float strafe_chance;
		if (!(strcmp(Entity->gameEntity->classname, "monster_daedalus")))
			strafe_chance = 0.8f;
		else
			strafe_chance = 0.6f;

		// if enemy is tesla, never strafe
		if ((Entity->gameEntity->enemy) && (Entity->gameEntity->enemy->classname) && (!strcmp(Entity->gameEntity->enemy->classname, "tesla")))
			strafe_chance = 0;

		if (random() < strafe_chance)
			AttackState = AS_SLIDING;
		else
			AttackState = AS_STRAIGHT;
	}
// do we want the monsters strafing?
	else
	{
		if (random() < 0.4)
			AttackState = AS_SLIDING;
		else
			AttackState = AS_STRAIGHT;
	}

//-PMM

	return false;
#endif
}

//
// monster_makron
//

void CMakron::Precache ()
{
	SoundIndex ("makron/pain3.wav");
	SoundIndex ("makron/pain2.wav");
	SoundIndex ("makron/pain1.wav");
	SoundIndex ("makron/death.wav");
	SoundIndex ("makron/step1.wav");
	SoundIndex ("makron/step2.wav");
	SoundIndex ("makron/bfg_fire.wav");
	SoundIndex ("makron/brain1.wav");
	SoundIndex ("makron/rail_up.wav");
	SoundIndex ("makron/popup.wav");
	SoundIndex ("makron/voice4.wav");
	SoundIndex ("makron/voice3.wav");
	SoundIndex ("makron/voice.wav");
	SoundIndex ("makron/bhit.wav");

	ModelIndex ("models/monsters/boss3/rider/tris.md2");
}

/*QUAKED monster_makron (1 .5 0) (-30 -30 0) (30 30 90) Ambush Trigger_Spawn Sight
*/
void CMakron::Spawn ()
{
	SoundPain4 = SoundIndex ("makron/pain3.wav");
	SoundPain5 = SoundIndex ("makron/pain2.wav");
	SoundPain6 = SoundIndex ("makron/pain1.wav");
	SoundDeath = SoundIndex ("makron/death.wav");
	SoundStepLeft = SoundIndex ("makron/step1.wav");
	SoundStepRight = SoundIndex ("makron/step2.wav");
	SoundAttackBfg = SoundIndex ("makron/bfg_fire.wav");
	SoundBrainSplorch = SoundIndex ("makron/brain1.wav");
	SoundPreRailgun = SoundIndex ("makron/rail_up.wav");
	SoundPopUp = SoundIndex ("makron/popup.wav");
	SoundTaunt1 = SoundIndex ("makron/voice4.wav");
	SoundTaunt2 = SoundIndex ("makron/voice3.wav");
	SoundTaunt3 = SoundIndex ("makron/voice.wav");
	SoundHit = SoundIndex ("makron/bhit.wav");
	
	Entity->SetSolid (SOLID_BBOX);
	Entity->State.SetModelIndex (ModelIndex ("models/monsters/boss3/rider/tris.md2"));
	Entity->SetMins (vec3f(-30, -30, 0));
	Entity->SetMaxs (vec3f(30, 30, 90));

	Entity->gameEntity->health = 3000;
	Entity->gameEntity->gib_health = -2000;
	Entity->gameEntity->mass = 500;

	MonsterFlags = (MF_HAS_ATTACK | MF_HAS_SIGHT);
	Entity->Link ();
	
	CurrentMove = &MakronMoveSight;
	WalkMonsterStart();
}

LINK_MONSTER_CLASSNAME_TO_CLASS ("monster_makron", CMakron);

/*
=================
CMakronJumpTimer class

Jorg is just about dead, so set up to launch Makron out
=================
*/
CMakronJumpTimer::CMakronJumpTimer () :
CBaseEntity(),
CThinkableEntity ()
{
};
CMakronJumpTimer::CMakronJumpTimer (int Index) :
CBaseEntity(Index),
CThinkableEntity (Index)
{
};

void CMakronJumpTimer::Think ()
{
	CMonsterEntity *newClass = QNew (com_levelPool, 0) CMonsterEntity;
	CMakron *Monster = QNew (com_levelPool, 0) CMakron ();
	newClass->Monster = Monster;
	Monster->Entity = newClass;
	newClass->State.SetOrigin (State.GetOrigin());
	Monster->Spawn ();
	newClass->NextThink = level.framenum + 1;
	newClass->gameEntity->target = Target;

	// jump at player
	CPlayerEntity *Player = level.sight_client;
	if (!Player)
		return;

	vec3f vec = (Player->State.GetOrigin() - newClass->State.GetOrigin());
	newClass->State.SetAngles (vec3f(0, vec.ToYaw(), 0));
	vec.Normalize();
	vec3f vel = vec3fOrigin.MultiplyAngles (400, vec);
	newClass->gameEntity->velocity[0] = vel.X;
	newClass->gameEntity->velocity[1] = vel.Y;
	newClass->gameEntity->velocity[2] = 200;
	newClass->gameEntity->groundentity = NULL;

	Free ();
};

void CMakronJumpTimer::Spawn (CBaseEntity *Jorg)
{
	CMakronJumpTimer *Timer = QNew (com_levelPool, 0) CMakronJumpTimer;
	
	Timer->NextThink = level.framenum + 8;
	Timer->Target = Jorg->gameEntity->target;
	Timer->State.SetOrigin (Jorg->State.GetOrigin());
	Timer->Link();
}
