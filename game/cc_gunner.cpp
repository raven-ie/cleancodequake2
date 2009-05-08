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
// cc_infantry.cpp
// Infantry Monster
//

#include "cc_local.h"
#include "m_gunner.h"

CGunner Monster_Gunner;

CGunner::CGunner ()
{
	Classname = "monster_gunner";
	Scale = MODEL_SCALE;
}

void CGunner::Allocate (edict_t *ent)
{
	ent->Monster = new CGunner(Monster_Gunner);
}

void CGunner::Idle ()
{
	Sound (Entity, CHAN_VOICE, SoundIdle, 1, ATTN_IDLE, 0);
}

void CGunner::Sight ()
{
	Sound (Entity, CHAN_VOICE, SoundSight);
}

void CGunner::Search ()
{
	Sound (Entity, CHAN_VOICE, SoundSearch);
}

CFrame GunnerFramesFidget [] =
{
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0, ConvertDerivedFunction(&CGunner::Idle)),
	CFrame (&CMonster::AI_Stand, 0),

	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),

	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),

	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),

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
CAnim GunnerMoveFidget (FRAME_stand31, FRAME_stand70, GunnerFramesFidget, ConvertDerivedFunction(&CGunner::Stand));

void CGunner::Fidget ()
{
	if (AIFlags & AI_STAND_GROUND)
		return;
	if (random() <= 0.05)
		CurrentMove = &GunnerMoveFidget;
}

CFrame GunnerFramesStand [] =
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
	CFrame (&CMonster::AI_Stand, 0, ConvertDerivedFunction(&CGunner::Fidget)),

	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0, ConvertDerivedFunction(&CGunner::Fidget)),

	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0, ConvertDerivedFunction(&CGunner::Fidget))
};
CAnim GunnerMoveStand (FRAME_stand01, FRAME_stand30, GunnerFramesStand);

void CGunner::Stand ()
{
	CurrentMove = &GunnerMoveStand;
}

CFrame GunnerFramesWalk [] =
{
	CFrame (&CMonster::AI_Walk, 0),
	CFrame (&CMonster::AI_Walk, 3),
	CFrame (&CMonster::AI_Walk, 4),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 7),
	CFrame (&CMonster::AI_Walk, 2),
	CFrame (&CMonster::AI_Walk, 6),
	CFrame (&CMonster::AI_Walk, 4),
	CFrame (&CMonster::AI_Walk, 2),
	CFrame (&CMonster::AI_Walk, 7),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 7),
	CFrame (&CMonster::AI_Walk, 4)
};
CAnim GunnerMoveWalk (FRAME_walk07, FRAME_walk19, GunnerFramesWalk);

void CGunner::Walk ()
{
	CurrentMove = &GunnerMoveWalk;
}

CFrame GunnerFramesRun [] =
{
	CFrame (&CMonster::AI_Run, 26),
	CFrame (&CMonster::AI_Run, 9),
	CFrame (&CMonster::AI_Run, 9),
	CFrame (&CMonster::AI_Run, 9),
	CFrame (&CMonster::AI_Run, 15),
	CFrame (&CMonster::AI_Run, 10),
	CFrame (&CMonster::AI_Run, 13),
	CFrame (&CMonster::AI_Run, 6)
};
CAnim GunnerMoveRun(FRAME_run01, FRAME_run08, GunnerFramesRun);

void CGunner::Run ()
{
	CurrentMove = (AIFlags & AI_STAND_GROUND) ? &GunnerMoveStand : &GunnerMoveRun;
}

CFrame GunnerFramesRunAndShoot [] =
{
	CFrame (&CMonster::AI_Run, 32),
	CFrame (&CMonster::AI_Run, 15),
	CFrame (&CMonster::AI_Run, 10),
	CFrame (&CMonster::AI_Run, 18),
	CFrame (&CMonster::AI_Run, 8),
	CFrame (&CMonster::AI_Run, 20)
};
CAnim GunnerMoveRunAndShoot (FRAME_runs01, FRAME_runs06, GunnerFramesRunAndShoot);

void CGunner::RunAndShoot ()
{
	CurrentMove = &GunnerMoveRunAndShoot;
}

CFrame GunnerFramesPain3 [] =
{
	CFrame (&CMonster::AI_Move, -3),
	CFrame (&CMonster::AI_Move, 1),
	CFrame (&CMonster::AI_Move, 1),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 1)
};
CAnim GunnerMovePain3 (FRAME_pain301, FRAME_pain305, GunnerFramesPain3, ConvertDerivedFunction(&CGunner::Run));

CFrame GunnerFramesPain2 [] =
{
	CFrame (&CMonster::AI_Move, -2),
	CFrame (&CMonster::AI_Move, 11),
	CFrame (&CMonster::AI_Move, 6),
	CFrame (&CMonster::AI_Move, 2),
	CFrame (&CMonster::AI_Move, -1),
	CFrame (&CMonster::AI_Move, -7),
	CFrame (&CMonster::AI_Move, -2),
	CFrame (&CMonster::AI_Move, -7)
};
CAnim GunnerMovePain2 (FRAME_pain201, FRAME_pain208, GunnerFramesPain2, ConvertDerivedFunction(&CGunner::Run));

CFrame GunnerFramesPain1 [] =
{
	CFrame (&CMonster::AI_Move, 2),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, -5),
	CFrame (&CMonster::AI_Move, 3),
	CFrame (&CMonster::AI_Move, -1),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 1),
	CFrame (&CMonster::AI_Move, 1),
	CFrame (&CMonster::AI_Move, 2),
	CFrame (&CMonster::AI_Move, 1),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, -2),
	CFrame (&CMonster::AI_Move, -2),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0)
};
CAnim GunnerMovePain1 (FRAME_pain101, FRAME_pain118, GunnerFramesPain1, ConvertDerivedFunction(&CGunner::Run));

void CGunner::Pain (edict_t *other, float kick, int damage)
{
	if (Entity->health < (Entity->max_health / 2))
		Entity->s.skinNum = 1;

	if (level.time < Entity->pain_debounce_time)
		return;

	Entity->pain_debounce_time = level.time + 3;
	Sound (Entity, CHAN_VOICE, (rand()&1) ? SoundPain : SoundPain2);

	if (skill->Integer() == 3)
		return;		// no pain anims in nightmare

	CurrentMove = ((damage <= 10) ? &GunnerMovePain3 : ((damage <= 25) ? &GunnerMovePain2 : &GunnerMovePain1));
}

void CGunner::Dead ()
{
	Vec3Set (Entity->mins, -16, -16, -24);
	Vec3Set (Entity->maxs, 16, 16, -8);
	Entity->movetype = MOVETYPE_TOSS;
	Entity->svFlags |= SVF_DEADMONSTER;
	Entity->nextthink = 0;
	gi.linkentity (Entity);
}

CFrame GunnerFramesDeath [] =
{
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, -7),
	CFrame (&CMonster::AI_Move, -3),
	CFrame (&CMonster::AI_Move, -5),
	CFrame (&CMonster::AI_Move, 8),
	CFrame (&CMonster::AI_Move, 6),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0)
};
CAnim GunnerMoveDeath (FRAME_death01, FRAME_death11, GunnerFramesDeath, ConvertDerivedFunction(&CGunner::Dead));

void CGunner::Die (edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
{
// check for gib
	if (Entity->health <= Entity->gib_health)
	{
		Sound (Entity, CHAN_VOICE, gi.soundindex ("misc/udeath.wav"));
		for (int n= 0; n < 2; n++)
			ThrowGib (Entity, "models/objects/gibs/bone/tris.md2", damage, GIB_ORGANIC);
		for (int n= 0; n < 4; n++)
			ThrowGib (Entity, "models/objects/gibs/sm_meat/tris.md2", damage, GIB_ORGANIC);
		ThrowHead (Entity, "models/objects/gibs/head2/tris.md2", damage, GIB_ORGANIC);
		Entity->deadflag = DEAD_DEAD;
		return;
	}

	if (Entity->deadflag == DEAD_DEAD)
		return;

// regular death
	Sound (Entity, CHAN_VOICE, SoundDeath);
	Entity->deadflag = DEAD_DEAD;
	Entity->takedamage = DAMAGE_YES;
	CurrentMove = &GunnerMoveDeath;
}

void CGunner::DuckDown ()
{
	if (AIFlags & AI_DUCKED)
		return;
	AIFlags |= AI_DUCKED;
	if (skill->Integer() >= 2)
	{
		if (random() > 0.5)
			Grenade ();
	}

	Entity->maxs[2] -= 32;
	Entity->takedamage = DAMAGE_YES;
	PauseTime = level.time + 1;
	gi.linkentity (Entity);
}

void CGunner::DuckHold ()
{
	if (level.time >= PauseTime)
		AIFlags &= ~AI_HOLD_FRAME;
	else
		AIFlags |= AI_HOLD_FRAME;
}

void CGunner::DuckUp ()
{
	AIFlags &= ~AI_DUCKED;
	Entity->maxs[2] += 32;
	Entity->takedamage = DAMAGE_AIM;
	gi.linkentity (Entity);
}

CFrame GunnerFramesDuck [] =
{
	CFrame (&CMonster::AI_Move, 1, ConvertDerivedFunction(&CGunner::DuckDown)),
	CFrame (&CMonster::AI_Move, 1),
	CFrame (&CMonster::AI_Move, 1, ConvertDerivedFunction(&CGunner::DuckHold)),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, -1),
	CFrame (&CMonster::AI_Move, -1),
	CFrame (&CMonster::AI_Move, 0, ConvertDerivedFunction(&CGunner::DuckUp)),
	CFrame (&CMonster::AI_Move, -1)
};
CAnim GunnerMoveDuck (FRAME_duck01, FRAME_duck08, GunnerFramesDuck, ConvertDerivedFunction(&CGunner::Run));

void CGunner::Dodge (edict_t *attacker, float eta)
{
	if (random() > 0.25)
		return;

	if (!Entity->enemy)
		Entity->enemy = attacker;

	CurrentMove = &GunnerMoveDuck;
}

void CGunner::OpenGun ()
{
	Sound (Entity, CHAN_VOICE, SoundOpen, 1, ATTN_IDLE, 0);
}

void CGunner::Fire ()
{
	vec3_t	start;
	vec3_t	forward, right;
	vec3_t	target;
	vec3_t	aim;
	int		flash_number = MZ2_GUNNER_MACHINEGUN_1 + (Entity->s.frame - FRAME_attak216);

	Angles_Vectors (Entity->s.angles, forward, right, NULL);
	G_ProjectSource (Entity->s.origin, dumb_and_hacky_monster_MuzzFlashOffset[flash_number], forward, right, start);

	// project enemy back a bit and target there
	Vec3Copy (Entity->enemy->s.origin, target);
	Vec3MA (target, -0.2, Entity->enemy->velocity, target);
	target[2] += Entity->enemy->viewheight;

	Vec3Subtract (target, start, aim);
	VectorNormalizef (aim, aim);
	MonsterFireBullet (start, aim, 3, 4, DEFAULT_BULLET_HSPREAD, DEFAULT_BULLET_VSPREAD, flash_number);
}

void CGunner::Grenade ()
{
	vec3_t	start;
	vec3_t	forward, right;
	vec3_t	aim;
	int		flash_number;

	switch (Entity->s.frame)
	{
	case FRAME_attak105:
		flash_number = MZ2_GUNNER_GRENADE_1;
		break;
	case FRAME_attak108:
		flash_number = MZ2_GUNNER_GRENADE_2;
		break;
	case FRAME_attak111:
		flash_number = MZ2_GUNNER_GRENADE_3;
		break;
	default:
		flash_number = MZ2_GUNNER_GRENADE_4;
		break;
	}
	Angles_Vectors (Entity->s.angles, forward, right, NULL);
	G_ProjectSource (Entity->s.origin, dumb_and_hacky_monster_MuzzFlashOffset[flash_number], forward, right, start);

	//FIXME : do a spread -225 -75 75 225 degrees around forward
	Vec3Copy (forward, aim);

	MonsterFireGrenade (start, aim, 50, 600, flash_number);
}

CFrame GunnerFramesAttackChain [] =
{
	/*
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	*/
	CFrame (&CMonster::AI_Charge, 0, ConvertDerivedFunction(&CGunner::OpenGun)),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0)
};
CAnim GunnerMoveAttackChain (FRAME_attak209, FRAME_attak215, GunnerFramesAttackChain, ConvertDerivedFunction(&CGunner::FireChain));

CFrame GunnerFramesFireChain [] =
{
	CFrame (&CMonster::AI_Charge,   0, ConvertDerivedFunction(&CGunner::Fire)),
	CFrame (&CMonster::AI_Charge,   0, ConvertDerivedFunction(&CGunner::Fire)),
	CFrame (&CMonster::AI_Charge,   0, ConvertDerivedFunction(&CGunner::Fire)),
	CFrame (&CMonster::AI_Charge,   0, ConvertDerivedFunction(&CGunner::Fire)),
	CFrame (&CMonster::AI_Charge,   0, ConvertDerivedFunction(&CGunner::Fire)),
	CFrame (&CMonster::AI_Charge,   0, ConvertDerivedFunction(&CGunner::Fire)),
	CFrame (&CMonster::AI_Charge,   0, ConvertDerivedFunction(&CGunner::Fire)),
	CFrame (&CMonster::AI_Charge,   0, ConvertDerivedFunction(&CGunner::Fire))
};
CAnim GunnerMoveFireChain (FRAME_attak216, FRAME_attak223, GunnerFramesFireChain, ConvertDerivedFunction(&CGunner::ReFireChain));

CFrame GunnerFramesEndFireChain [] =
{
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0)
};
CAnim GunnerMoveEndFireChain (FRAME_attak224, FRAME_attak230, GunnerFramesEndFireChain, ConvertDerivedFunction(&CGunner::Run));

CFrame GunnerFramesAttackGrenade [] =
{
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0, ConvertDerivedFunction(&CGunner::Grenade)),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0, ConvertDerivedFunction(&CGunner::Grenade)),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0, ConvertDerivedFunction(&CGunner::Grenade)),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0, ConvertDerivedFunction(&CGunner::Grenade)),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0)
};
CAnim GunnerMoveAttackGrenade (FRAME_attak101, FRAME_attak121, GunnerFramesAttackGrenade, ConvertDerivedFunction(&CGunner::Run));

void CGunner::Attack()
{
	if (range (Entity, Entity->enemy) == RANGE_MELEE)
		CurrentMove = &GunnerMoveAttackChain;
	else
		CurrentMove = (random() <= 0.5) ? &GunnerMoveAttackGrenade : &GunnerMoveAttackChain;
}

void CGunner::FireChain ()
{
	CurrentMove = &GunnerMoveFireChain;
}

void CGunner::ReFireChain ()
{
	if (Entity->enemy->health > 0 && visible (Entity, Entity->enemy) && random() <= 0.5)
	{
		CurrentMove = &GunnerMoveFireChain;
		return;
	}
	CurrentMove = &GunnerMoveEndFireChain;
}

void CGunner::Spawn ()
{
	SoundDeath = SoundIndex ("gunner/death1.wav");	
	SoundPain = SoundIndex ("gunner/gunpain2.wav");	
	SoundPain2 = SoundIndex ("gunner/gunpain1.wav");	
	SoundIdle = SoundIndex ("gunner/gunidle1.wav");	
	SoundOpen = SoundIndex ("gunner/gunatck1.wav");	
	SoundSearch = SoundIndex ("gunner/gunsrch1.wav");	
	SoundSight = SoundIndex ("gunner/sight1.wav");	

	SoundIndex ("gunner/gunatck2.wav");
	SoundIndex ("gunner/gunatck3.wav");

	Entity->movetype = MOVETYPE_STEP;
	Entity->solid = SOLID_BBOX;
	Entity->s.modelIndex = ModelIndex ("models/monsters/gunner/tris.md2");
	Vec3Set (Entity->mins, -16, -16, -24);
	Vec3Set (Entity->maxs, 16, 16, 32);

	Entity->health = 175;
	Entity->gib_health = -70;
	Entity->mass = 200;

	MonsterFlags |= (MF_HAS_ATTACK | MF_HAS_SIGHT | MF_HAS_SEARCH);
	gi.linkentity (Entity);

	CurrentMove = &GunnerMoveStand;	
	WalkMonsterStart ();
}