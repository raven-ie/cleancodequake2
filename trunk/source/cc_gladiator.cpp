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
#include "cc_gladiator.h"

CGladiator::CGladiator ()
{
	Scale = MODEL_SCALE;
}

void CGladiator::Idle ()
{
	Entity->PlaySound (CHAN_VOICE, SoundIdle);
}

void CGladiator::Sight ()
{
	Entity->PlaySound (CHAN_VOICE, SoundSight);
}

void CGladiator::Search ()
{
	Entity->PlaySound (CHAN_VOICE, SoundSearch);
}

void CGladiator::SwingCleaver ()
{
	Entity->PlaySound (CHAN_WEAPON, SoundCleaverSwing);
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
	static vec3_t	aim = {MELEE_DISTANCE, Entity->GetMins().X, -4};

	if (CMeleeWeapon::Fire (Entity, aim, (20 + (irandom(5))), 300))
		Entity->PlaySound (CHAN_AUTO, SoundCleaverHit);
	else
		Entity->PlaySound (CHAN_AUTO, SoundCleaverMiss);
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
	vec3f	start, forward, right;

	Entity->State.GetAngles().ToVectors (&forward, &right, NULL);
	G_ProjectSource (Entity->State.GetOrigin(), dumb_and_hacky_monster_MuzzFlashOffset[MZ2_GLADIATOR_RAILGUN_1], forward, right, start);

	// calc direction to where we targted
	MonsterFireRailgun (start, (SavedFirePosition - start).GetNormalized(), 50, 100, MZ2_GLADIATOR_RAILGUN_1);
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

	vec3_t origin;
	Entity->State.GetOrigin(origin);

	// a small safe zone
	Vec3Subtract (origin, Entity->gameEntity->enemy->state.origin, v);
	range = Vec3Length(v);
	if (range <= (MELEE_DISTANCE + 32))
		return;

	// charge up the railgun
	Entity->PlaySound (CHAN_WEAPON, SoundGun);
	SavedFirePosition = Entity->gameEntity->enemy->Entity->State.GetOrigin() + vec3f(0, 0, Entity->gameEntity->enemy->viewheight);
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

void CGladiator::Pain (CBaseEntity *other, float kick, int damage)
{
	if (Entity->Health < (Entity->MaxHealth / 2))
		Entity->State.SetSkinNum(1);

	if (level.framenum < Entity->gameEntity->pain_debounce_time)
	{
		if ((Entity->Velocity.Z > 100) && (CurrentMove == &GladiatorMovePain))
			CurrentMove = &GladiatorMovePainAir;
		return;
	}

	Entity->gameEntity->pain_debounce_time = level.framenum + 30;

	Entity->PlaySound (CHAN_VOICE, (random() < 0.5) ? SoundPain1 : SoundPain2);
	if (skill->Integer() == 3)
		return;		// no pain anims in nightmare

	CurrentMove = (Entity->Velocity.Z > 100) ? &GladiatorMovePainAir : &GladiatorMovePain;
}

void CGladiator::Dead ()
{
	Entity->SetMins (vec3f(-16, -16, -24));
	Entity->SetMaxs (vec3f(16, 16, -8));
	Entity->PhysicsType = PHYSICS_TOSS;
	Entity->SetSvFlags (Entity->GetSvFlags() | SVF_DEADMONSTER);
	Entity->NextThink = 0;
	Entity->Link ();
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

void CGladiator::Die (CBaseEntity *inflictor, CBaseEntity *attacker, int damage, vec3f &point)
{
// check for gib
	if (Entity->Health <= Entity->GibHealth)
	{
		Entity->PlaySound (CHAN_VOICE, SoundIndex ("misc/udeath.wav"));
		for (int n= 0; n < 2; n++)
			CGibEntity::Spawn (Entity, gMedia.Gib_Bone[0], damage, GIB_ORGANIC);
		for (int n= 0; n < 4; n++)
			CGibEntity::Spawn (Entity, gMedia.Gib_SmallMeat, damage, GIB_ORGANIC);
		Entity->ThrowHead (gMedia.Gib_Head[1], damage, GIB_ORGANIC);
		Entity->DeadFlag = true;
		return;
	}

	if (Entity->DeadFlag == true)
		return;

// regular death
	Entity->PlaySound (CHAN_VOICE, SoundDie, 1, ATTN_NORM, 0);
	Entity->DeadFlag = true;
	Entity->CanTakeDamage = true;

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

	Entity->SetSolid (SOLID_BBOX);
	Entity->State.SetModelIndex (ModelIndex ("models/monsters/gladiatr/tris.md2"));
	Entity->SetMins (vec3f(-32, -32, -24));
	Entity->SetMaxs (vec3f(32, 32, 64));

	Entity->Health = 400;
	Entity->GibHealth = -175;
	Entity->gameEntity->mass = 400;

	MonsterFlags = (MF_HAS_SEARCH | MF_HAS_IDLE | MF_HAS_SIGHT | MF_HAS_MELEE | MF_HAS_ATTACK);

	Entity->Link ();

	CurrentMove = &GladiatorMoveStand;
	WalkMonsterStart ();
}

LINK_MONSTER_CLASSNAME_TO_CLASS ("monster_gladiator", CGladiator);