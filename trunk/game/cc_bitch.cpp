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

CMaiden Monster_Bitch;

CMaiden::CMaiden ()
{
	Classname = "monster_bitch";
	Scale = MODEL_SCALE;
}

void CMaiden::Allocate (edict_t *ent)
{
	ent->Monster = new CMaiden(Monster_Bitch);
}

void CMaiden::Moan ()
{
	Sound (Entity, CHAN_VOICE, (random() < 0.5) ? SoundIdle1 : SoundIdle2, 1, ATTN_IDLE, 0);
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
	CFrame (&CMonster::AI_Run, 5),
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

void CMaiden::Pain (edict_t *other, float kick, int damage)
{
	if (Entity->health < (Entity->max_health / 2))
		Entity->s.skinNum = 1;

	if (level.time < Entity->pain_debounce_time)
		return;

	Entity->pain_debounce_time = level.time + 3;

	int r = rand()%3;
	switch (r)
	{
	case 0:
	default:
		Sound (Entity, CHAN_VOICE, SoundPain1);
		break;
	case 1:
		Sound (Entity, CHAN_VOICE, SoundPain2);
		break;
	case 2:
		Sound (Entity, CHAN_VOICE, SoundPain3);
		break;
	}

	if (skill->Integer() == 3)
		return;		// no pain anims in nightmare

	if (damage <= 10)
		CurrentMove = &ChickMovePain1;
	else if (damage <= 25)
		CurrentMove = &ChickMovePain2;
	else
		CurrentMove = &ChickMovePain3;
}

void CMaiden::Dead ()
{
	Vec3Set (Entity->mins, -16, -16, 0);
	Vec3Set (Entity->maxs, 16, 16, 16);
	Entity->movetype = MOVETYPE_TOSS;
	Entity->svFlags |= SVF_DEADMONSTER;
	NextThink = 0;
	gi.linkentity (Entity);
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

void CMaiden::Die (edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
{
	int		n;

// check for gib
	if (Entity->health <= Entity->gib_health)
	{
		Sound (Entity, CHAN_VOICE, SoundIndex ("misc/udeath.wav"));
		for (n= 0; n < 2; n++)
			ThrowGib (Entity, "models/objects/gibs/bone/tris.md2", damage, GIB_ORGANIC);
		for (n= 0; n < 4; n++)
			ThrowGib (Entity, "models/objects/gibs/sm_meat/tris.md2", damage, GIB_ORGANIC);
		ThrowHead (Entity, "models/objects/gibs/head2/tris.md2", damage, GIB_ORGANIC);
		Entity->deadflag = DEAD_DEAD;
		return;
	}

	if (Entity->deadflag == DEAD_DEAD)
		return;

// regular death
	Entity->deadflag = DEAD_DEAD;
	Entity->takedamage = DAMAGE_YES;

	n = rand() % 2;
	CurrentMove = (n == 0) ? &ChickMoveDeath1 : &ChickMoveDeath2;
	Sound (Entity, CHAN_VOICE, (n == 0) ? SoundDeath1 : SoundDeath2);
}

void CMaiden::DuckDown ()
{
	if (AIFlags & AI_DUCKED)
		return;
	AIFlags |= AI_DUCKED;
	Entity->maxs[2] -= 32;
	Entity->takedamage = DAMAGE_YES;
	PauseTime = level.time + 1;
	gi.linkentity (Entity);
}

void CMaiden::DuckHold ()
{
	if (level.time >= PauseTime)
		AIFlags &= ~AI_HOLD_FRAME;
	else
		AIFlags |= AI_HOLD_FRAME;
}

void CMaiden::DuckUp ()
{
	AIFlags &= ~AI_DUCKED;
	Entity->maxs[2] += 32;
	Entity->takedamage = DAMAGE_AIM;
	gi.linkentity (Entity);
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
CAnim ChickMoveDuck (FRAME_duck01, FRAME_duck07, ChickFramesDuck, ConvertDerivedFunction(&CMaiden::Run));

void CMaiden::Dodge (edict_t *attacker, float eta)
{
	if (random() > 0.25)
		return;

	if (!Entity->enemy)
		Entity->enemy = attacker;

	CurrentMove = &ChickMoveDuck;
}

void CMaiden::Slash ()
{
	vec3_t	aim = {MELEE_DISTANCE, Entity->mins[0], 10};
	Sound (Entity, CHAN_WEAPON, SoundMeleeSwing);
	fire_hit (Entity, aim, (10 + (rand() %6)), 100);
}


void CMaiden::Rocket ()
{
	vec3_t	forward, right;
	vec3_t	start;
	vec3_t	dir;
	vec3_t	vec;

	Angles_Vectors (Entity->s.angles, forward, right, NULL);
	G_ProjectSource (Entity->s.origin, dumb_and_hacky_monster_MuzzFlashOffset[MZ2_CHICK_ROCKET_1], forward, right, start);

	Vec3Copy (Entity->enemy->s.origin, vec);
	vec[2] += Entity->enemy->viewheight;
	Vec3Subtract (vec, start, dir);
	VectorNormalizef (dir, dir);

	MonsterFireRocket (start, dir, 50, 500, MZ2_CHICK_ROCKET_1);
}	

void CMaiden::PreAttack ()
{
	Sound (Entity, CHAN_VOICE, SoundMissilePrelaunch);
}

void CMaiden::Reload ()
{
	Sound (Entity, CHAN_VOICE, SoundMissileReload);
}

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

void CMaiden::ReRocket()
{
	if (Entity->enemy->health > 0)
	{
		if (range (Entity, Entity->enemy) > RANGE_MELEE && visible (Entity, Entity->enemy) && random() < 0.6)
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
	if (Entity->enemy->health > 0)
	{
		if (range (Entity, Entity->enemy) == RANGE_MELEE)
		{
			if (random() <= 0.9)
			{				
				CurrentMove = &ChickMoveSlash;
				return;
			}
			else
			{
				CurrentMove = &ChickMoveEndSlash;
				return;
			}
		}
	}
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
	CurrentMove = &ChickMoveStartAttack1;
}

void CMaiden::Sight()
{
	Sound (Entity, CHAN_VOICE, SoundSight);
}

void CMaiden::Spawn ()
{
	Entity->movetype = MOVETYPE_STEP;
	Entity->solid = SOLID_BBOX;
	Entity->s.modelIndex = ModelIndex("models/monsters/bitch/tris.md2");
	Vec3Set (Entity->mins, -16, -16, 0);
	Vec3Set (Entity->maxs, 16, 16, 56);

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

	Entity->health = 175;
	Entity->gib_health = -70;
	Entity->mass = 200;

	MonsterFlags = (MF_HAS_MELEE | MF_HAS_ATTACK | MF_HAS_IDLE | MF_HAS_SIGHT);

	gi.linkentity (Entity);

	CurrentMove = &ChickMoveStand;
	WalkMonsterStart ();
}
