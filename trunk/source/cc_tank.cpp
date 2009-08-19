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
// cc_tank.cpp
// Tank
//

#include "cc_local.h"
#include "m_tank.h"
#include "cc_tank.h"

CTank::CTank ()
{
	Scale = MODEL_SCALE;
}

CTankCommander::CTankCommander ()
{
	Scale = MODEL_SCALE;
}

//
// misc
//

void CTank::Sight ()
{
	Entity->PlaySound (CHAN_VOICE, SoundSight);
}

void CTank::Footstep ()
{
	Entity->PlaySound (CHAN_BODY, SoundStep);
}

void CTank::Thud ()
{
	Entity->PlaySound (CHAN_BODY, SoundThud);
}

void CTank::Windup ()
{
	Entity->PlaySound (CHAN_WEAPON, SoundWindup);
}

void CTank::Idle ()
{
	Entity->PlaySound (CHAN_VOICE, SoundIdle, 1, ATTN_IDLE, 0);
}

//
// stand
//

CFrame TankFramesStand []=
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
CAnim	TankMoveStand (FRAME_stand01, FRAME_stand30, TankFramesStand);
	
void CTank::Stand ()
{
	CurrentMove = &TankMoveStand;
}

//
// walk
//

CFrame TankFramesWalk [] =
{
	CFrame (&CMonster::AI_Walk, 4),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 3),
	CFrame (&CMonster::AI_Walk, 2),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 4),
	CFrame (&CMonster::AI_Walk, 4, ConvertDerivedFunction(&CTank::Footstep)),
	CFrame (&CMonster::AI_Walk, 3),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 4),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 7),
	CFrame (&CMonster::AI_Walk, 7),
	CFrame (&CMonster::AI_Walk, 6),
	CFrame (&CMonster::AI_Walk, 6, ConvertDerivedFunction(&CTank::Footstep))
};
CAnim TankMoveWalk (FRAME_walk05, FRAME_walk20, TankFramesWalk);

void CTank::Walk ()
{
	CurrentMove = &TankMoveWalk;
}

//
// run
//

CFrame TankFramesStartRun [] =
{
	CFrame (&CMonster::AI_Run,  0),
	CFrame (&CMonster::AI_Run,  6),
	CFrame (&CMonster::AI_Run,  6),
	CFrame (&CMonster::AI_Run, 11, ConvertDerivedFunction(&CTank::Footstep))
};
CAnim TankMoveStartRun (FRAME_walk01, FRAME_walk04, TankFramesStartRun, ConvertDerivedFunction(&CTank::Run));

CFrame TankFramesRun [] =
{
	CFrame (&CMonster::AI_Run, 4),
	CFrame (&CMonster::AI_Run, 5),
	CFrame (&CMonster::AI_Run, 3),
	CFrame (&CMonster::AI_Run, 2),
	CFrame (&CMonster::AI_Run, 5),
	CFrame (&CMonster::AI_Run, 5),
	CFrame (&CMonster::AI_Run, 4),
	CFrame (&CMonster::AI_Run, 4, ConvertDerivedFunction(&CTank::Footstep)),
	CFrame (&CMonster::AI_Run, 3),
	CFrame (&CMonster::AI_Run, 5),
	CFrame (&CMonster::AI_Run, 4),
	CFrame (&CMonster::AI_Run, 5),
	CFrame (&CMonster::AI_Run, 7),
	CFrame (&CMonster::AI_Run, 7),
	CFrame (&CMonster::AI_Run, 6),
	CFrame (&CMonster::AI_Run, 6, ConvertDerivedFunction(&CTank::Footstep))
};
CAnim TankMoveRun (FRAME_walk05, FRAME_walk20, TankFramesRun);

CFrame TankFramesStopRun [] =
{
	CFrame (&CMonster::AI_Run,  3),
	CFrame (&CMonster::AI_Run,  3),
	CFrame (&CMonster::AI_Run,  2),
	CFrame (&CMonster::AI_Run,  2),
	CFrame (&CMonster::AI_Run,  4, ConvertDerivedFunction(&CTank::Footstep))
};
CAnim TankMoveStopRun (FRAME_walk21, FRAME_walk25, TankFramesStopRun, ConvertDerivedFunction(&CTank::Walk));

void CTank::Run ()
{
	if (Entity->gameEntity->enemy && Entity->gameEntity->enemy->client)
		AIFlags |= AI_BRUTAL;
	else
		AIFlags &= ~AI_BRUTAL;

	if (AIFlags & AI_STAND_GROUND)
	{
		CurrentMove = &TankMoveStand;
		return;
	}

	if (CurrentMove == &TankMoveWalk ||
		CurrentMove == &TankMoveStartRun)
	{
		CurrentMove = &TankMoveRun;
	}
	else
	{
		CurrentMove = &TankMoveStartRun;
	}
}

//
// pain
//

CFrame TankFramesPain1 [] =
{
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0)
};
CAnim TankMovePain1 (FRAME_pain101, FRAME_pain104, TankFramesPain1, ConvertDerivedFunction(&CTank::Run));

CFrame TankFramesPain2 [] =
{
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0)
};
CAnim TankMovePain2 (FRAME_pain201, FRAME_pain205, TankFramesPain2, ConvertDerivedFunction(&CTank::Run));

CFrame TankFramesPain3 [] =
{
	CFrame (&CMonster::AI_Move, -7),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 2),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 3),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 2),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0, ConvertDerivedFunction(&CTank::Footstep))
};
CAnim TankMovePain3 (FRAME_pain301, FRAME_pain316, TankFramesPain3, ConvertDerivedFunction(&CTank::Run));

void CTank::Pain (CBaseEntity *other, float kick, int damage)
{
	if (Entity->gameEntity->health < (Entity->gameEntity->max_health / 2))
		Entity->State.SetSkinNum (Entity->State.GetSkinNum() | 1);

	if (damage <= 10)
		return;

	if (level.framenum < Entity->gameEntity->pain_debounce_time)
			return;

	if (damage <= 30 && random() > 0.2)
		return;
	
	// If hard or nightmare, don't go into pain while attacking
	if ( skill->Integer() >= 2)
	{
		if ( (Entity->State.GetFrame() >= FRAME_attak301) && (Entity->State.GetFrame() <= FRAME_attak330) )
			return;
		if ( (Entity->State.GetFrame() >= FRAME_attak101) && (Entity->State.GetFrame() <= FRAME_attak116) )
			return;
	}

	Entity->gameEntity->pain_debounce_time = level.framenum + 30;
	Entity->PlaySound (CHAN_VOICE, SoundPain);

	if (skill->Integer() == 3)
		return;		// no pain anims in nightmare

#ifdef MONSTER_USE_ROGUE_AI
	// PMM - blindfire cleanup
	AIFlags &= ~AI_MANUAL_STEERING;
	// pmm
#endif

	CurrentMove = ((damage <= 60) ? ((damage <= 30) ? &TankMovePain1 : &TankMovePain2) : &TankMovePain3);
};


//
// attacks
//

void CTank::Blaster ()
{
	vec3_t	forward, right;
	vec3_t	start;
	vec3_t	end;
	vec3_t	dir;
	int		flash_number;

	switch (Entity->State.GetFrame())
	{
	case FRAME_attak110:
		flash_number = MZ2_TANK_BLASTER_1;
		break;
	case FRAME_attak113:
		flash_number = MZ2_TANK_BLASTER_2;
		break;
	default:
		flash_number = MZ2_TANK_BLASTER_3;
		break;
	}

	vec3_t angles, origin;
	Entity->State.GetAngles(angles);
	Entity->State.GetOrigin(origin);
	Angles_Vectors (angles, forward, right, NULL);
	G_ProjectSource (origin, dumb_and_hacky_monster_MuzzFlashOffset[flash_number], forward, right, start);

	Vec3Copy (Entity->gameEntity->enemy->state.origin, end);
	end[2] += Entity->gameEntity->enemy->viewheight;
	Vec3Subtract (end, start, dir);

	MonsterFireBlaster (start, dir, 30, 800, flash_number, EF_BLASTER);
}	

void CTank::Strike ()
{
	Entity->PlaySound (CHAN_WEAPON, SoundStrike);
}	

void CTank::Rocket ()
{
	vec3_t	forward, right;
	vec3_t	start;
	vec3_t	dir;
	vec3_t	vec;
	int		flash_number;
	CTrace	trace;				// PGM
	int		rocketSpeed;		// PGM
	// pmm - blindfire support
	vec3_t	target;
#ifdef MONSTER_USE_ROGUE_AI
	bool blindfire = false;
#endif

	if(!Entity->gameEntity->enemy || !Entity->gameEntity->enemy->inUse)		//PGM
		return;									//PGM

	// pmm - blindfire check
#ifdef MONSTER_USE_ROGUE_AI
	if (AIFlags & AI_MANUAL_STEERING)
		blindfire = true;
#endif

	switch (Entity->State.GetFrame())
	{
	case FRAME_attak324:
		flash_number = MZ2_TANK_ROCKET_1;
		break;
	case FRAME_attak327:
		flash_number = MZ2_TANK_ROCKET_2;
		break;
	default:
		flash_number = MZ2_TANK_ROCKET_3;
		break;
	}

	vec3_t angles, origin;
	Entity->State.GetAngles(angles);
	Entity->State.GetOrigin(origin);
	Angles_Vectors (angles, forward, right, NULL);
	G_ProjectSource (origin, dumb_and_hacky_monster_MuzzFlashOffset[flash_number], forward, right, start);

	rocketSpeed = 500 + (100 * skill->Integer());	// PGM rock & roll.... :)

		// PMM
#ifdef MONSTER_USE_ROGUE_AI
	if (blindfire)
		Vec3Copy (BlindFireTarget, target);
	else
#endif
		Vec3Copy (Entity->gameEntity->enemy->state.origin, target);
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
	if(random() < 0.66 || (start[2] < Entity->gameEntity->enemy->absMin[2]))
	{
//		gi.dprintf("normal shot\n");
		Vec3Copy (Entity->gameEntity->enemy->state.origin, vec);
		vec[2] += Entity->gameEntity->enemy->viewheight;
		Vec3Subtract (vec, start, dir);
	}
	else
	{
//		gi.dprintf("shooting at feet!\n");
		Vec3Copy (Entity->gameEntity->enemy->state.origin, vec);
		vec[2] = Entity->gameEntity->enemy->absMin[2];
		Vec3Subtract (vec, start, dir);
	}
//PGM
	
//======
//PMM - lead target  (not when blindfiring)
	// 20, 35, 50, 65 chance of leading
	if(
#ifdef MONSTER_USE_ROGUE_AI
		(!blindfire) && 
#endif
		((random() < (0.2 + ((3 - skill->Integer()) * 0.15)))))
	{
		float	dist;
		float	time;

//		gi.dprintf ("leading target\n");
		dist = Vec3Length (dir);
		time = dist/rocketSpeed;
		Vec3MA(vec, time, Entity->gameEntity->enemy->velocity, vec);
		Vec3Subtract(vec, start, dir);
	}
//PMM - lead target
//======

	VectorNormalizeFastf (dir);

	// pmm blindfire doesn't check target (done in checkattack)
	// paranoia, make sure we're not shooting a target right next to us
	trace = CTrace(start, vec, Entity->gameEntity, CONTENTS_MASK_SHOT);
	#ifdef MONSTER_USE_ROGUE_AI
	if (blindfire)
	{
		// blindfire has different fail criteria for the trace
		if (!(trace.startSolid || trace.allSolid || (trace.fraction < 0.5)))
			MonsterFireRocket (start, dir, 50, rocketSpeed, flash_number);
		else 
		{
			// try shifting the target to the left a little (to help counter large offset)
			Vec3Copy (target, vec);
			Vec3MA (vec, -20, right, vec);
			Vec3Subtract(vec, start, dir);
			VectorNormalizeFastf (dir);
			trace = CTrace(start, vec, Entity->gameEntity, CONTENTS_MASK_SHOT);
			if (!(trace.startSolid || trace.allSolid || (trace.fraction < 0.5)))
				MonsterFireRocket (start, dir, 50, rocketSpeed, flash_number);
			else 
			{
				// ok, that failed.  try to the right
				Vec3Copy (target, vec);
				Vec3MA (vec, 20, right, vec);
				Vec3Subtract(vec, start, dir);
				VectorNormalizeFastf (dir);
				trace = CTrace(start, vec, Entity->gameEntity, CONTENTS_MASK_SHOT);
				if (!(trace.startSolid || trace.allSolid || (trace.fraction < 0.5)))
					MonsterFireRocket (start, dir, 50, rocketSpeed, flash_number);
			}
		}
	}
	else
#endif
	{
		trace = CTrace(start, vec, Entity->gameEntity, CONTENTS_MASK_SHOT);
		if(trace.ent == Entity->gameEntity->enemy || trace.ent == world)
		{
			if(trace.fraction > 0.5 || (trace.ent && trace.ent->client))
				MonsterFireRocket (start, dir, 50, rocketSpeed, flash_number);
	//		else
	//			gi.dprintf("didn't make it halfway to target...aborting\n");
		}
	}
}	

void CTank::MachineGun ()
{
	vec3_t	dir;
	vec3_t	start;
	vec3_t	forward, right;
	int		flash_number = MZ2_TANK_MACHINEGUN_1 + (Entity->State.GetFrame() - FRAME_attak406);

	vec3_t angles, origin;
	Entity->State.GetAngles(angles);
	Entity->State.GetOrigin(origin);
	Angles_Vectors (angles, forward, right, NULL);
	G_ProjectSource (origin, dumb_and_hacky_monster_MuzzFlashOffset[flash_number], forward, right, start);

	if (Entity->gameEntity->enemy)
	{
		vec3_t vec;
		Vec3Copy (Entity->gameEntity->enemy->state.origin, vec);
		vec[2] += Entity->gameEntity->enemy->viewheight;
		Vec3Subtract (vec, start, vec);
		VecToAngles (vec, vec);
		dir[0] = vec[0];
	}
	else
		dir[0] = 0;

	if (Entity->State.GetFrame() <= FRAME_attak415)
		dir[1] = angles[1] - 8 * (Entity->State.GetFrame() - FRAME_attak411);
	else
		dir[1] = angles[1] + 8 * (Entity->State.GetFrame() - FRAME_attak419);
	dir[2] = 0;

	Angles_Vectors (dir, forward, NULL, NULL);

	MonsterFireBullet (start, forward, 20, 4, DEFAULT_BULLET_HSPREAD, DEFAULT_BULLET_VSPREAD, flash_number);
}	


CFrame TankFramesAttackBlast [] =
{
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, -1),
	CFrame (&CMonster::AI_Charge, -2),
	CFrame (&CMonster::AI_Charge, -1),
	CFrame (&CMonster::AI_Charge, -1),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0,	ConvertDerivedFunction(&CTank::Blaster)),		// 10
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0,	ConvertDerivedFunction(&CTank::Blaster)),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0,	ConvertDerivedFunction(&CTank::Blaster)) // 16
};
CAnim TankMoveAttackBlast (FRAME_attak101, FRAME_attak116, TankFramesAttackBlast, ConvertDerivedFunction(&CTank::ReAttackBlaster));

CFrame TankFramesReAttackBlast [] =
{
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0,	ConvertDerivedFunction(&CTank::Blaster)),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0,	ConvertDerivedFunction(&CTank::Blaster))			// 16
};
CAnim TankMoveReAttackBlast (FRAME_attak111, FRAME_attak116, TankFramesReAttackBlast, ConvertDerivedFunction(&CTank::ReAttackBlaster));

CFrame TankFramesAttackPostBlast [] =	
{
	CFrame (&CMonster::AI_Move, 0),				// 17
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 2),
	CFrame (&CMonster::AI_Move, 3),
	CFrame (&CMonster::AI_Move, 2),
	CFrame (&CMonster::AI_Move, -2,	ConvertDerivedFunction(&CTank::Footstep))		// 22
};
CAnim TankMoveAttackPostBlast (FRAME_attak117, FRAME_attak122, TankFramesAttackPostBlast, ConvertDerivedFunction(&CTank::Run));

void CTank::ReAttackBlaster ()
{
	if (skill->Integer() >= 2 && visible (Entity->gameEntity, Entity->gameEntity->enemy) && Entity->gameEntity->enemy->health > 0 && random() <= 0.6)
	{
		CurrentMove = &TankMoveReAttackBlast;
		return;
	}
	CurrentMove = &TankMoveAttackPostBlast;
}

void CTank::PostStrike ()
{
	Entity->gameEntity->enemy = NULL;
	Run ();
}

CFrame TankFramesAttackStrike [] =
{
	CFrame (&CMonster::AI_Move, 3),
	CFrame (&CMonster::AI_Move, 2),
	CFrame (&CMonster::AI_Move, 2),
	CFrame (&CMonster::AI_Move, 1),
	CFrame (&CMonster::AI_Move, 6),
	CFrame (&CMonster::AI_Move, 7),
	CFrame (&CMonster::AI_Move, 9, ConvertDerivedFunction(&CTank::Footstep)),
	CFrame (&CMonster::AI_Move, 2),
	CFrame (&CMonster::AI_Move, 1),
	CFrame (&CMonster::AI_Move, 2),
	CFrame (&CMonster::AI_Move, 2, ConvertDerivedFunction(&CTank::Footstep)),
	CFrame (&CMonster::AI_Move, 2),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, -2),
	CFrame (&CMonster::AI_Move, -2),
	CFrame (&CMonster::AI_Move, 0, ConvertDerivedFunction(&CTank::Windup)),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0, ConvertDerivedFunction(&CTank::Strike)),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, -1),
	CFrame (&CMonster::AI_Move, -1),
	CFrame (&CMonster::AI_Move, -1),
	CFrame (&CMonster::AI_Move, -1),
	CFrame (&CMonster::AI_Move, -1),
	CFrame (&CMonster::AI_Move, -3),
	CFrame (&CMonster::AI_Move, -10),
	CFrame (&CMonster::AI_Move, -10),
	CFrame (&CMonster::AI_Move, -2),
	CFrame (&CMonster::AI_Move, -3),
	CFrame (&CMonster::AI_Move, -2, ConvertDerivedFunction(&CTank::Footstep))
};
CAnim TankMoveAttackStrike (FRAME_attak201, FRAME_attak238, TankFramesAttackStrike, ConvertDerivedFunction(&CTank::PostStrike));

CFrame TankFramesAttackPreRocket [] =
{
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),			// 10

	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 1),
	CFrame (&CMonster::AI_Charge, 2),
	CFrame (&CMonster::AI_Charge, 7),
	CFrame (&CMonster::AI_Charge, 7),
	CFrame (&CMonster::AI_Charge, 7,  ConvertDerivedFunction(&CTank::Footstep)),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),			// 20

	CFrame (&CMonster::AI_Charge, -3)
};
CAnim TankMoveAttackPreRocket (FRAME_attak301, FRAME_attak321, TankFramesAttackPreRocket, ConvertDerivedFunction(&CTank::DoAttackRocket));

CFrame TankFramesAttackFireRocket [] =
{
	CFrame (&CMonster::AI_Charge, -3),			// Loop Start	22 
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0,  ConvertDerivedFunction(&CTank::Rocket)),		// 24
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0,  ConvertDerivedFunction(&CTank::Rocket)),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, -1, ConvertDerivedFunction(&CTank::Rocket))		// 30	Loop End
};
CAnim TankMoveAttackFireRocket (FRAME_attak322, FRAME_attak330, TankFramesAttackFireRocket, ConvertDerivedFunction(&CTank::ReFireRocket));

CFrame TankFramesAttackPostRocket [] =
{	
	CFrame (&CMonster::AI_Charge, 0),			// 31
	CFrame (&CMonster::AI_Charge, -1),
	CFrame (&CMonster::AI_Charge, -1),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 2),
	CFrame (&CMonster::AI_Charge, 3),
	CFrame (&CMonster::AI_Charge, 4),
	CFrame (&CMonster::AI_Charge, 2),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),			// 40

	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, -9),
	CFrame (&CMonster::AI_Charge, -8),
	CFrame (&CMonster::AI_Charge, -7),
	CFrame (&CMonster::AI_Charge, -1),
	CFrame (&CMonster::AI_Charge, -1, ConvertDerivedFunction(&CTank::Footstep)),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),			// 50

	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0)
};
CAnim TankMoveAttackPostRocket (FRAME_attak331, FRAME_attak353, TankFramesAttackPostRocket, ConvertDerivedFunction(&CTank::Run));

CFrame TankFramesAttackChain [] =
{
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (NULL,      0, ConvertDerivedFunction(&CTank::MachineGun)),
	CFrame (NULL,      0, ConvertDerivedFunction(&CTank::MachineGun)),
	CFrame (NULL,      0, ConvertDerivedFunction(&CTank::MachineGun)),
	CFrame (NULL,      0, ConvertDerivedFunction(&CTank::MachineGun)),
	CFrame (NULL,      0, ConvertDerivedFunction(&CTank::MachineGun)),
	CFrame (NULL,      0, ConvertDerivedFunction(&CTank::MachineGun)),
	CFrame (NULL,      0, ConvertDerivedFunction(&CTank::MachineGun)),
	CFrame (NULL,      0, ConvertDerivedFunction(&CTank::MachineGun)),
	CFrame (NULL,      0, ConvertDerivedFunction(&CTank::MachineGun)),
	CFrame (NULL,      0, ConvertDerivedFunction(&CTank::MachineGun)),
	CFrame (NULL,      0, ConvertDerivedFunction(&CTank::MachineGun)),
	CFrame (NULL,      0, ConvertDerivedFunction(&CTank::MachineGun)),
	CFrame (NULL,      0, ConvertDerivedFunction(&CTank::MachineGun)),
	CFrame (NULL,      0, ConvertDerivedFunction(&CTank::MachineGun)),
	CFrame (NULL,      0, ConvertDerivedFunction(&CTank::MachineGun)),
	CFrame (NULL,      0, ConvertDerivedFunction(&CTank::MachineGun)),
	CFrame (NULL,      0, ConvertDerivedFunction(&CTank::MachineGun)),
	CFrame (NULL,      0, ConvertDerivedFunction(&CTank::MachineGun)),
	CFrame (NULL,      0, ConvertDerivedFunction(&CTank::MachineGun)),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0)
};
CAnim TankMoveAttackChain (FRAME_attak401, FRAME_attak429, TankFramesAttackChain, ConvertDerivedFunction(&CTank::Run));

void CTank::ReFireRocket ()
{
#ifdef MONSTER_USE_ROGUE_AI
	// PMM - blindfire cleanup
	if (AIFlags & AI_MANUAL_STEERING)
	{
		AIFlags &= ~AI_MANUAL_STEERING;
		CurrentMove = &TankMoveAttackPostRocket;
		return;
	}
	// pmm
#endif

	// Only on hard or nightmare
	if ( skill->Integer() >= 2 && Entity->gameEntity->enemy->health > 0 && visible(Entity->gameEntity, Entity->gameEntity->enemy) && random() <= 0.4)
	{
		CurrentMove = &TankMoveAttackFireRocket;
		return;
	}
	CurrentMove = &TankMoveAttackPostRocket;
}

void CTank::DoAttackRocket ()
{
	CurrentMove = &TankMoveAttackFireRocket;
}

void CTank::Attack ()
{
	if (!Entity->gameEntity->enemy || !Entity->gameEntity->enemy->inUse)
		return;

	vec3_t	vec;
	float	range;
	float	r;

	if (Entity->gameEntity->enemy->health < 0)
	{
		CurrentMove = &TankMoveAttackStrike;
		AIFlags &= ~AI_BRUTAL;
		return;
	}

#ifdef MONSTER_USE_ROGUE_AI
	// PMM 
	if (AttackState == AS_BLIND)
	{
		// setup shot probabilities
		r = random();

		BlindFireDelay += 3.2 + 2.0 + random()*3.0;

		// don't shoot at the origin
		if (Vec3Compare (BlindFireTarget, vec3Origin))
			return;

		// don't shoot if the dice say not to
		if (BlindFireDelay < 7.5 && (r > 0.4f))
			return;
		else if (r > 0.1f)
			return;

		// turn on manual steering to signal both manual steering and blindfire
		AIFlags |= AI_MANUAL_STEERING;
		CurrentMove = &TankMoveAttackFireRocket;
		AttackFinished = level.framenum + 30 + ((2*random())*10);
		Entity->gameEntity->pain_debounce_time = level.framenum + 50;	// no pain for a while
		return;
	}
	// pmm
#endif

	vec3_t origin;
	Entity->State.GetOrigin(origin);

	Vec3Subtract (Entity->gameEntity->enemy->state.origin, origin, vec);
	range = Vec3Length (vec);

	r = random();

	if (range <= 125)
	{
		if (r < 0.4)
			CurrentMove = &TankMoveAttackChain;
		else 
			CurrentMove = &TankMoveAttackBlast;
	}
	else if (range <= 250)
	{
		if (r < 0.5)
			CurrentMove = &TankMoveAttackChain;
		else
			CurrentMove = &TankMoveAttackBlast;
	}
	else
	{
		if (r < 0.33)
			CurrentMove = &TankMoveAttackChain;
		else if (r < 0.66)
		{
			CurrentMove = &TankMoveAttackPreRocket;
			Entity->gameEntity->pain_debounce_time = level.framenum + 50;	// no pain for a while
		}
		else
			CurrentMove = &TankMoveAttackBlast;
	}
}


//
// death
//

void CTank::Dead ()
{
	Entity->SetMins (vec3f(-16, -16, -16));
	Entity->SetMaxs (vec3f(16, 16, -0));
	Entity->TossPhysics = true;
	Entity->PhysicsType = PHYSICS_TOSS;
	Entity->SetSvFlags (Entity->GetSvFlags() | SVF_DEADMONSTER);
	Entity->NextThink = 0;
	Entity->Link ();
}

CFrame TankFramesDeath1 [] =
{
	CFrame (&CMonster::AI_Move, -7),
	CFrame (&CMonster::AI_Move, -2),
	CFrame (&CMonster::AI_Move, -2),
	CFrame (&CMonster::AI_Move, 1),
	CFrame (&CMonster::AI_Move, 3),
	CFrame (&CMonster::AI_Move, 6),
	CFrame (&CMonster::AI_Move, 1),
	CFrame (&CMonster::AI_Move, 1),
	CFrame (&CMonster::AI_Move, 2),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, -2),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, -3),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, -4),
	CFrame (&CMonster::AI_Move, -6),
	CFrame (&CMonster::AI_Move, -4),
	CFrame (&CMonster::AI_Move, -5),
	CFrame (&CMonster::AI_Move, -7),
	CFrame (&CMonster::AI_Move, -15, ConvertDerivedFunction(&CTank::Thud)),
	CFrame (&CMonster::AI_Move, -5),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0)
};
CAnim TankMoveDeath (FRAME_death101, FRAME_death132, TankFramesDeath1, ConvertDerivedFunction(&CTank::Dead));

void CTank::Die (CBaseEntity *inflictor, CBaseEntity *attacker, int damage, vec3_t point)
{
// check for gib
	if (Entity->gameEntity->health <= Entity->gameEntity->gib_health)
	{
		Entity->PlaySound (CHAN_VOICE, SoundIndex ("misc/udeath.wav"));
		for (int n= 0; n < 1 /*4*/; n++)
			CGibEntity::Spawn (Entity, gMedia.Gib_SmallMeat, damage, GIB_ORGANIC);
		for (int n= 0; n < 4; n++)
			CGibEntity::Spawn (Entity, gMedia.Gib_SmallMetal, damage, GIB_METALLIC);
		CGibEntity::Spawn (Entity, gMedia.Gib_Chest, damage, GIB_ORGANIC);
		Entity->ThrowHead (gMedia.Gib_Gear, damage, GIB_METALLIC);
		Entity->gameEntity->deadflag = DEAD_DEAD;
		return;
	}

	if (Entity->gameEntity->deadflag == DEAD_DEAD)
		return;

// regular death
	Entity->PlaySound (CHAN_VOICE, SoundDie);
	Entity->gameEntity->deadflag = DEAD_DEAD;
	Entity->gameEntity->takedamage = true;

	CurrentMove = &TankMoveDeath;
}


//
// monster_tank
//

void CTank::Spawn ()
{
	Entity->State.SetModelIndex (ModelIndex ("models/monsters/tank/tris.md2"));
	Entity->SetMins (vec3f(-32, -32, -16));
	Entity->SetMaxs (vec3f(32, 32, 72));
	Entity->SetSolid (SOLID_BBOX);

	SoundPain = SoundIndex ("tank/tnkpain2.wav");
	SoundThud = SoundIndex ("tank/tnkdeth2.wav");
	SoundIdle = SoundIndex ("tank/tnkidle1.wav");
	SoundDie = SoundIndex ("tank/death.wav");
	SoundStep = SoundIndex ("tank/step.wav");
	SoundWindup = SoundIndex ("tank/tnkatck4.wav");
	SoundStrike = SoundIndex ("tank/tnkatck5.wav");
	SoundSight = SoundIndex ("tank/sight1.wav");

	SoundIndex ("tank/tnkatck1.wav");
	SoundIndex ("tank/tnkatk2a.wav");
	SoundIndex ("tank/tnkatk2b.wav");
	SoundIndex ("tank/tnkatk2c.wav");
	SoundIndex ("tank/tnkatk2d.wav");
	SoundIndex ("tank/tnkatk2e.wav");
	SoundIndex ("tank/tnkatck3.wav");

	Entity->gameEntity->health = 750;
	Entity->gameEntity->gib_health = -200;
	Entity->gameEntity->mass = 500;

	MonsterFlags |= (MF_HAS_ATTACK | MF_HAS_SIGHT | MF_HAS_IDLE);

#ifdef MONSTER_USE_ROGUE_AI
	// PMM
	AIFlags |= AI_IGNORE_SHOTS;
	BlindFire = true;
	//pmm
#endif

	Entity->Link ();
	WalkMonsterStart();
}

LINK_MONSTER_CLASSNAME_TO_CLASS ("monster_tank", CTank);

void CTankCommander::Spawn ()
{
	CTank::Spawn ();
	Entity->gameEntity->health = 1000;
	Entity->gameEntity->gib_health = -225;
	Entity->State.SetSkinNum (2);
}

LINK_MONSTER_CLASSNAME_TO_CLASS ("monster_tank_commander", CTankCommander);
