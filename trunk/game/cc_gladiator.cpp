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
// cc_gladiator.cpp
// WACHOOEYCHOY
//

#include "cc_local.h"
#include "m_gladiator.h"

CGladiator Monster_Gladiator;

CGladiator::CGladiator ()
{
	Classname = "monster_gladiator";
	Scale = MODEL_SCALE;
}

void CGladiator::Allocate (edict_t *ent)
{
	ent->Monster = new CGladiator(Monster_Gladiator);
}

void CGladiator::Idle ()
{
	PlaySoundFrom (Entity, CHAN_VOICE, SoundIdle);
}

void CGladiator::Sight ()
{
	PlaySoundFrom (Entity, CHAN_VOICE, SoundSight);
}

void CGladiator::Search ()
{
	PlaySoundFrom (Entity, CHAN_VOICE, SoundSearch);
}

void CGladiator::SwingCleaver ()
{
	PlaySoundFrom (Entity, CHAN_WEAPON, SoundCleaverSwing);
}

CFrame GladiatorFramesStand [] =
{
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0)
};
CAnim GladiatorMoveStand (FRAME_stand1, FRAME_stand7, GladiatorFramesStand);

void CGladiator::Stand ()
{
	CurrentMove = &GladiatorMoveStand;
}

CFrame GladiatorFramesWalk [] =
{
	CFrame (&CMonster::AI_Walk, 15),
	CFrame (&CMonster::AI_Walk, 7),
	CFrame (&CMonster::AI_Walk, 6),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 2),
	CFrame (&CMonster::AI_Walk, 0),
	CFrame (&CMonster::AI_Walk, 2),
	CFrame (&CMonster::AI_Walk, 8),
	CFrame (&CMonster::AI_Walk, 12),
	CFrame (&CMonster::AI_Walk, 8),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 2),
	CFrame (&CMonster::AI_Walk, 2),
	CFrame (&CMonster::AI_Walk, 1),
	CFrame (&CMonster::AI_Walk, 8)
};
CAnim GladiatorMoveWalk (FRAME_walk1, FRAME_walk16, GladiatorFramesWalk);

void CGladiator::Walk ()
{
	CurrentMove = &GladiatorMoveWalk;
}

CFrame GladiatorFramesRun [] =
{
	CFrame (&CMonster::AI_Run, 23),
	CFrame (&CMonster::AI_Run, 14),
	CFrame (&CMonster::AI_Run, 14),
	CFrame (&CMonster::AI_Run, 21),
	CFrame (&CMonster::AI_Run, 12),
	CFrame (&CMonster::AI_Run, 13)
};
CAnim GladiatorMoveRun (FRAME_run1, FRAME_run6, GladiatorFramesRun);

void CGladiator::Run ()
{
	CurrentMove = (AIFlags & AI_STAND_GROUND) ? &GladiatorMoveStand : &GladiatorMoveRun;
}

void CGladiator::MeleeAttack ()
{
	static vec3_t	aim = {MELEE_DISTANCE, Entity->mins[0], -4};

	if (fire_hit (Entity, aim, (20 + (rand() %5)), 300))
		PlaySoundFrom (Entity, CHAN_AUTO, SoundCleaverHit);
	else
		PlaySoundFrom (Entity, CHAN_AUTO, SoundCleaverMiss);
}

CFrame GladiatorFramesAttackMelee [] =
{
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0, ConvertDerivedFunction(&CGladiator::SwingCleaver)),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0, ConvertDerivedFunction(&CGladiator::MeleeAttack)),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0, ConvertDerivedFunction(&CGladiator::SwingCleaver)),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0, ConvertDerivedFunction(&CGladiator::MeleeAttack)),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0)
};
CAnim GladiatorMoveAttackMelee (FRAME_melee1, FRAME_melee17, GladiatorFramesAttackMelee, &CMonster::Run);

void CGladiator::Melee ()
{
	CurrentMove = &GladiatorMoveAttackMelee;
}

void CGladiator::FireRail ()
{
	vec3_t	start;
	vec3_t	dir;
	vec3_t	forward, right;

	Angles_Vectors (Entity->s.angles, forward, right, NULL);
	G_ProjectSource (Entity->s.origin, dumb_and_hacky_monster_MuzzFlashOffset[MZ2_GLADIATOR_RAILGUN_1], forward, right, start);

	// calc direction to where we targted
	Vec3Subtract (Entity->pos1, start, dir);
	VectorNormalizef (dir, dir);

	MonsterFireRailgun (start, dir, 50, 100, MZ2_GLADIATOR_RAILGUN_1);
}

CFrame GladiatorFramesAttackGun [] =
{
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0, ConvertDerivedFunction(&CGladiator::FireRail)),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0)
};
CAnim GladiatorMoveAttackGun (FRAME_attack1, FRAME_attack9, GladiatorFramesAttackGun, &CMonster::Run);

void CGladiator::Attack ()
{
	float	range;
	vec3_t	v;

	// a small safe zone
	Vec3Subtract (Entity->s.origin, Entity->enemy->s.origin, v);
	range = Vec3Length(v);
	if (range <= (MELEE_DISTANCE + 32))
		return;

	// charge up the railgun
	PlaySoundFrom (Entity, CHAN_WEAPON, SoundGun);
	Vec3Copy (Entity->enemy->s.origin, Entity->pos1);	//save for aiming the shot
	Entity->pos1[2] += Entity->enemy->viewheight;
	CurrentMove = &GladiatorMoveAttackGun;
}

CFrame GladiatorFramesPain [] =
{
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0)
};
CAnim GladiatorMovePain (FRAME_pain1, FRAME_pain6, GladiatorFramesPain, &CMonster::Run);

CFrame GladiatorFramesPainAir [] =
{
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0)
};
CAnim GladiatorMovePainAir (FRAME_painup1, FRAME_painup7, GladiatorFramesPainAir, &CMonster::Run);

void CGladiator::Pain (edict_t *other, float kick, int damage)
{
	if (Entity->health < (Entity->max_health / 2))
		Entity->s.skinNum = 1;

	if (level.time < Entity->pain_debounce_time)
	{
		if ((Entity->velocity[2] > 100) && (CurrentMove == &GladiatorMovePain))
			CurrentMove = &GladiatorMovePainAir;
		return;
	}

	Entity->pain_debounce_time = level.time + 3;

	PlaySoundFrom (Entity, CHAN_VOICE, (random() < 0.5) ? SoundPain1 : SoundPain2);
	if (skill->Integer() == 3)
		return;		// no pain anims in nightmare

	CurrentMove = (Entity->velocity[2] > 100) ? &GladiatorMovePainAir : &GladiatorMovePain;
}

void CGladiator::Dead ()
{
	Vec3Set (Entity->mins, -16, -16, -24);
	Vec3Set (Entity->maxs, 16, 16, -8);
	Entity->movetype = MOVETYPE_TOSS;
	Entity->svFlags |= SVF_DEADMONSTER;
	NextThink = 0;
	gi.linkentity (Entity);
}

CFrame GladiatorFramesDeath [] =
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
	CFrame (&CMonster::AI_Move, 0)
};
CAnim GladiatorMoveDeath (FRAME_death1, FRAME_death22, GladiatorFramesDeath, ConvertDerivedFunction(&CGladiator::Dead));

void CGladiator::Die (edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
{
	int		n;

// check for gib
	if (Entity->health <= Entity->gib_health)
	{
		PlaySoundFrom (Entity, CHAN_VOICE, SoundIndex ("misc/udeath.wav"));
		for (n= 0; n < 2; n++)
			ThrowGib (Entity, gMedia.Gib_Bone[0], damage, GIB_ORGANIC);
		for (n= 0; n < 4; n++)
			ThrowGib (Entity, gMedia.Gib_SmallMeat, damage, GIB_ORGANIC);
		ThrowHead (Entity, gMedia.Gib_Head[1], damage, GIB_ORGANIC);
		Entity->deadflag = DEAD_DEAD;
		return;
	}

	if (Entity->deadflag == DEAD_DEAD)
		return;

// regular death
	PlaySoundFrom (Entity, CHAN_VOICE, SoundDie, 1, ATTN_NORM, 0);
	Entity->deadflag = DEAD_DEAD;
	Entity->takedamage = DAMAGE_YES;

	CurrentMove = &GladiatorMoveDeath;
}


/*QUAKED monster_gladiator (1 .5 0) (-32 -32 -24) (32 32 64) Ambush Trigger_Spawn Sight
*/
void CGladiator::Spawn ()
{
	SoundPain1 = SoundIndex ("gladiator/pain.wav");	
	SoundPain2 = SoundIndex ("gladiator/gldpain2.wav");	
	SoundDie = SoundIndex ("gladiator/glddeth2.wav");	
	SoundGun = SoundIndex ("gladiator/railgun.wav");
	SoundCleaverSwing = SoundIndex ("gladiator/melee1.wav");
	SoundCleaverHit = SoundIndex ("gladiator/melee2.wav");
	SoundCleaverMiss = SoundIndex ("gladiator/melee3.wav");
	SoundIdle = SoundIndex ("gladiator/gldidle1.wav");
	SoundSearch = SoundIndex ("gladiator/gldsrch1.wav");
	SoundSight = SoundIndex ("gladiator/sight.wav");

	Entity->movetype = MOVETYPE_STEP;
	Entity->solid = SOLID_BBOX;
	Entity->s.modelIndex = ModelIndex ("models/monsters/gladiatr/tris.md2");
	Vec3Set (Entity->mins, -32, -32, -24);
	Vec3Set (Entity->maxs, 32, 32, 64);

	Entity->health = 400;
	Entity->gib_health = -175;
	Entity->mass = 400;

	MonsterFlags = (MF_HAS_SEARCH | MF_HAS_IDLE | MF_HAS_SIGHT | MF_HAS_MELEE | MF_HAS_ATTACK);

	gi.linkentity (Entity);

	CurrentMove = &GladiatorMoveStand;
	WalkMonsterStart ();
}
