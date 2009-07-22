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
// cc_parasite.cpp
// Parasite
//

#include "cc_local.h"
#include "cc_parasite.h"
#include "m_parasite.h"

CParasite::CParasite ()
{
	Scale = MODEL_SCALE;
}

void CParasite::Launch ()
{
	Entity->PlaySound (CHAN_WEAPON, SoundLaunch, 1, ATTN_NORM, 0);
}

void CParasite::ReelIn ()
{
	Entity->PlaySound (CHAN_WEAPON, SoundReelIn, 1, ATTN_NORM, 0);
}

void CParasite::Sight ()
{
	Entity->PlaySound (CHAN_WEAPON, SoundSight, 1, ATTN_NORM, 0);
}

void CParasite::Tap ()
{
	Entity->PlaySound (CHAN_WEAPON, SoundTap, 1, ATTN_IDLE, 0);
}

void CParasite::Scratch ()
{
	Entity->PlaySound (CHAN_WEAPON, SoundScratch, 1, ATTN_IDLE, 0);
}

CFrame ParasiteFramesStartFidget [] =
{
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0)
};
CAnim ParasiteMoveStartFidget (FRAME_stand18, FRAME_stand21, ParasiteFramesStartFidget, ConvertDerivedFunction(&CParasite::DoFidget));

CFrame ParasiteFramesFidget [] =
{
	CFrame (&CMonster::AI_Stand, 0, ConvertDerivedFunction(&CParasite::Scratch)),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0, ConvertDerivedFunction(&CParasite::Scratch)),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0)
};
CAnim ParasiteMoveFidget (FRAME_stand22, FRAME_stand27, ParasiteFramesFidget, ConvertDerivedFunction(&CParasite::ReFidget));

CFrame ParasiteFramesEndFidget [] =
{
	CFrame (&CMonster::AI_Stand, 0, ConvertDerivedFunction(&CParasite::Scratch)),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0)
};
CAnim ParasiteMoveEndFidget (FRAME_stand28, FRAME_stand35, ParasiteFramesEndFidget, &CMonster::Stand);

void CParasite::EndFidget ()
{
	CurrentMove = &ParasiteMoveEndFidget;
}

void CParasite::DoFidget ()
{
	CurrentMove = &ParasiteMoveFidget;
}

void CParasite::ReFidget ()
{
	CurrentMove = (random() <= 0.8) ? &ParasiteMoveFidget : &ParasiteMoveEndFidget;
}

void CParasite::Idle ()
{
	CurrentMove = &ParasiteMoveStartFidget;
}


CFrame ParasiteFramesStand [] =
{
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0, ConvertDerivedFunction(&CParasite::Tap)),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0, ConvertDerivedFunction(&CParasite::Tap)),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0, ConvertDerivedFunction(&CParasite::Tap)),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0, ConvertDerivedFunction(&CParasite::Tap)),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0, ConvertDerivedFunction(&CParasite::Tap)),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0, ConvertDerivedFunction(&CParasite::Tap))
};
CAnim ParasiteMoveStand (FRAME_stand01, FRAME_stand17, ParasiteFramesStand, &CMonster::Stand);

void CParasite::Stand ()
{
	CurrentMove = &ParasiteMoveStand;
}

CFrame ParasiteFramesRun [] =
{
	CFrame (&CMonster::AI_Run, 30),
	CFrame (&CMonster::AI_Run, 30),
	CFrame (&CMonster::AI_Run, 22),
	CFrame (&CMonster::AI_Run, 19),
	CFrame (&CMonster::AI_Run, 24),
	CFrame (&CMonster::AI_Run, 28),
	CFrame (&CMonster::AI_Run, 25)
};
CAnim ParasiteMoveRun (FRAME_run03, FRAME_run09, ParasiteFramesRun);

CFrame ParasiteFramesStartRun [] =
{
	CFrame (&CMonster::AI_Run, 0),
	CFrame (&CMonster::AI_Run, 30),
};
CAnim ParasiteMoveStartRun (FRAME_run01, FRAME_run02, ParasiteFramesStartRun, ConvertDerivedFunction(&CParasite::StartRun));

CFrame ParasiteFramesStopRun [] =
{	
	CFrame (&CMonster::AI_Run, 20),
	CFrame (&CMonster::AI_Run, 20),
	CFrame (&CMonster::AI_Run, 12),
	CFrame (&CMonster::AI_Run, 10),
	CFrame (&CMonster::AI_Run, 0),
	CFrame (&CMonster::AI_Run, 0)
};
CAnim ParasiteMoveStopRun (FRAME_run10, FRAME_run15, ParasiteFramesStopRun);

void CParasite::Run ()
{	
	CurrentMove = (AIFlags & AI_STAND_GROUND) ? &ParasiteMoveStand : &ParasiteMoveStartRun;
}

void CParasite::StartRun ()
{
	CurrentMove = (AIFlags & AI_STAND_GROUND) ? &ParasiteMoveStand : &ParasiteMoveRun;
}

CFrame ParasiteFramesWalk [] =
{
	CFrame (&CMonster::AI_Walk, 30),
	CFrame (&CMonster::AI_Walk, 30),
	CFrame (&CMonster::AI_Walk, 22),
	CFrame (&CMonster::AI_Walk, 19),
	CFrame (&CMonster::AI_Walk, 24),
	CFrame (&CMonster::AI_Walk, 28),
	CFrame (&CMonster::AI_Walk, 25)
};
CAnim ParasiteMoveWalk (FRAME_run03, FRAME_run09, ParasiteFramesWalk, &CMonster::Walk);

CFrame ParasiteFramesStartWalk [] =
{
	CFrame (&CMonster::AI_Walk, 0),
	CFrame (&CMonster::AI_Walk, 30, ConvertDerivedFunction(&CParasite::DoWalk))
};
CAnim ParasiteMoveStartWalk (FRAME_run01, FRAME_run02, ParasiteFramesStartWalk);

CFrame ParasiteFramesStopWalk [] =
{	
	CFrame (&CMonster::AI_Walk, 20),
	CFrame (&CMonster::AI_Walk, 20),
	CFrame (&CMonster::AI_Walk, 12),
	CFrame (&CMonster::AI_Walk, 10),
	CFrame (&CMonster::AI_Walk, 0),
	CFrame (&CMonster::AI_Walk, 0)
};
CAnim ParasiteMoveStopWalk (FRAME_run10, FRAME_run15, ParasiteFramesStopWalk);

void CParasite::Walk ()
{	
	CurrentMove = &ParasiteMoveStartWalk;
}

void CParasite::DoWalk ()
{
	CurrentMove = &ParasiteMoveWalk;
}

CFrame ParasiteFramesPain1 [] =
{
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 6),
	CFrame (&CMonster::AI_Move, 16),
	CFrame (&CMonster::AI_Move, -6),
	CFrame (&CMonster::AI_Move, -7),
	CFrame (&CMonster::AI_Move, 0)
};
CAnim ParasiteMovePain1 (FRAME_pain101, FRAME_pain111, ParasiteFramesPain1, &CMonster::Run);

void CParasite::Pain (CBaseEntity *other, float kick, int damage)
{
	if (Entity->gameEntity->health < (Entity->gameEntity->max_health / 2))
		Entity->State.SetSkinNum(1);

	if (level.framenum < Entity->gameEntity->pain_debounce_time)
		return;

	Entity->gameEntity->pain_debounce_time = level.framenum + 30;

	if (skill->Integer() == 3)
		return;		// no pain anims in nightmare

	Entity->PlaySound (CHAN_VOICE, (random() < 0.5) ? SoundPain1 : SoundPain2, 1, ATTN_NORM, 0);

	CurrentMove = &ParasiteMovePain1;
}

bool CParasite::DrainAttackOK (vec3f &start, vec3f &end)
{
	vec3f dir = start - end;

	// check for max distance
	if (dir.Length() > 256)
		return false;

	// check for min/max pitch
	dir.Z = 0;
	vec3f angles = dir.ToAngles();
	if (angles.X < -180)
		angles.X += 360;
	if (Q_fabs(angles.X) > 30)
		return false;

	return true;
}

void CParasite::DrainAttack ()
{

	vec3f f, r;
	Entity->State.GetAngles().ToVectors (&f, &r, NULL);
	vec3f offset(24, 0, 6);

	vec3f start;
	G_ProjectSource (Entity->State.GetOrigin(), offset, f, r, start);

	vec3f end = vec3f(Entity->gameEntity->enemy->state.origin);
	if (!DrainAttackOK(start, end))
	{
		end.Z = Entity->gameEntity->enemy->state.origin[2] + Entity->gameEntity->enemy->maxs[2] - 8;
		if (!DrainAttackOK(start, end))
		{
			end.Z = Entity->gameEntity->enemy->state.origin[2] + Entity->gameEntity->enemy->maxs[2] + 8;
			if (!DrainAttackOK(start, end))
				return;
		}
	}
	end = vec3f(Entity->gameEntity->enemy->state.origin);

	CTrace tr = CTrace(start, end, Entity->gameEntity, CONTENTS_MASK_SHOT);
	if (tr.ent != Entity->gameEntity->enemy)
		return;

	int damage = (Entity->State.GetFrame() == FRAME_drain03) ? 5 : 2;
	if (Entity->State.GetFrame() == FRAME_drain03)
		PlaySoundFrom (Entity->gameEntity->enemy, CHAN_AUTO, SoundImpact, 1, ATTN_NORM, 0);
	else if (Entity->State.GetFrame() == FRAME_drain04)
		Entity->PlaySound (CHAN_WEAPON, SoundSuck, 1, ATTN_NORM, 0);

	vec3_t ostart = {start.X, start.Y, start.Z};
	vec3_t oend = {end.X, end.Y, end.Z};
	CTempEnt_Trails::FleshCable (ostart, oend, Entity->State.GetNumber());

	vec3_t dir;
	dir[0] = start.X - end.X;
	dir[1] = start.Y - end.Y;
	dir[2] = start.Z - end.Z;
	T_Damage (Entity->gameEntity->enemy, Entity->gameEntity, Entity->gameEntity, dir, Entity->gameEntity->enemy->state.origin, vec3Origin, damage, 0, DAMAGE_NO_KNOCKBACK, MOD_UNKNOWN);
}

CFrame ParasiteFramesDrain [] =
{
	CFrame (&CMonster::AI_Charge, 0,	ConvertDerivedFunction(&CParasite::Launch)),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 15,	ConvertDerivedFunction(&CParasite::DrainAttack)),			// Target hits
	CFrame (&CMonster::AI_Charge, 0,	ConvertDerivedFunction(&CParasite::DrainAttack)),			// drain
	CFrame (&CMonster::AI_Charge, 0,	ConvertDerivedFunction(&CParasite::DrainAttack)),			// drain
	CFrame (&CMonster::AI_Charge, 0,	ConvertDerivedFunction(&CParasite::DrainAttack)),			// drain
	CFrame (&CMonster::AI_Charge, 0,	ConvertDerivedFunction(&CParasite::DrainAttack)),			// drain
	CFrame (&CMonster::AI_Charge, -2,  ConvertDerivedFunction(&CParasite::DrainAttack)),			// drain
	CFrame (&CMonster::AI_Charge, -2,	ConvertDerivedFunction(&CParasite::DrainAttack)),			// drain
	CFrame (&CMonster::AI_Charge, -3,	ConvertDerivedFunction(&CParasite::DrainAttack)),			// drain
	CFrame (&CMonster::AI_Charge, -2,	ConvertDerivedFunction(&CParasite::DrainAttack)),			// drain
	CFrame (&CMonster::AI_Charge, 0,	ConvertDerivedFunction(&CParasite::DrainAttack)),			// drain
	CFrame (&CMonster::AI_Charge, -1,  ConvertDerivedFunction(&CParasite::DrainAttack)),			// drain
	CFrame (&CMonster::AI_Charge, 0,	ConvertDerivedFunction(&CParasite::ReelIn)),				// let go
	CFrame (&CMonster::AI_Charge, -2),
	CFrame (&CMonster::AI_Charge, -2),
	CFrame (&CMonster::AI_Charge, -3),
	CFrame (&CMonster::AI_Charge, 0)
};
CAnim ParasiteMoveDrain (FRAME_drain01, FRAME_drain18, ParasiteFramesDrain, &CMonster::Run);

#if 0
CFrame ParasiteFramesBreak [] =
{
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, -3),
	CFrame (&CMonster::AI_Charge, 1),
	CFrame (&CMonster::AI_Charge, 2),
	CFrame (&CMonster::AI_Charge, -3),
	CFrame (&CMonster::AI_Charge, 1),
	CFrame (&CMonster::AI_Charge, 1),
	CFrame (&CMonster::AI_Charge, 3),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, -18),
	CFrame (&CMonster::AI_Charge, 3),
	CFrame (&CMonster::AI_Charge, 9),
	CFrame (&CMonster::AI_Charge, 6),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, -18),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 8),
	CFrame (&CMonster::AI_Charge, 9),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, -18),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),		// airborne
	CFrame (&CMonster::AI_Charge, 0),		// airborne
	CFrame (&CMonster::AI_Charge, 0),		// slides
	CFrame (&CMonster::AI_Charge, 0),		// slides
	CFrame (&CMonster::AI_Charge, 0),		// slides
	CFrame (&CMonster::AI_Charge, 0),		// slides
	CFrame (&CMonster::AI_Charge, 4),
	CFrame (&CMonster::AI_Charge, 11),		
	CFrame (&CMonster::AI_Charge, -2),
	CFrame (&CMonster::AI_Charge, -5),
	CFrame (&CMonster::AI_Charge, 1,	NULL
};
mmove_t parasite_move_break = {FRAME_break01, FRAME_break32, parasite_frames_break, parasite_start_run};
#endif

/*
=== 
Break Stuff Ends
===
*/

void CParasite::Attack ()
{
//	if (random() <= 0.2)
//		self->monsterinfo.currentmove = &parasite_move_break;
//	else
	CurrentMove = &ParasiteMoveDrain;
}

bool CParasite::CheckAttack ()
{
	if (!CMonster::CheckAttack ())
		return false;

	vec3f start = Entity->State.GetOrigin();
	vec3f end = vec3f(Entity->gameEntity->enemy->state.origin);
	return DrainAttackOK(start, end);
}

/*
===
Death Stuff Starts
===
*/

void CParasite::Dead ()
{
	Entity->SetMins (vec3f(-16, -16, -24));
	Entity->SetMaxs (vec3f(16, 16, -8));
	Entity->TossPhysics = true;
	Entity->SetSvFlags (Entity->GetSvFlags() | SVF_DEADMONSTER);
	Entity->NextThink = 0;
	Entity->Link ();
}

CFrame ParasiteFramesDeath [] =
{
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0)
};
CAnim ParasiteMoveDeath (FRAME_death101, FRAME_death107, ParasiteFramesDeath, ConvertDerivedFunction(&CParasite::Dead));

void CParasite::Die (CBaseEntity *inflictor, CBaseEntity *attacker, int damage, vec3_t point)
{
	int		n;

// check for gib
	if (Entity->gameEntity->health <= Entity->gameEntity->gib_health)
	{
		Entity->PlaySound (CHAN_VOICE, SoundIndex ("misc/udeath.wav"), 1, ATTN_NORM, 0);
		for (n= 0; n < 2; n++)
			CGibEntity::Spawn (Entity, gMedia.Gib_Bone[0], damage, GIB_ORGANIC);
		for (n= 0; n < 4; n++)
			CGibEntity::Spawn (Entity, gMedia.Gib_SmallMeat, damage, GIB_ORGANIC);
		Entity->ThrowHead (gMedia.Gib_Head[1], damage, GIB_ORGANIC);
		Entity->gameEntity->deadflag = DEAD_DEAD;
		return;
	}

	if (Entity->gameEntity->deadflag == DEAD_DEAD)
		return;

// regular death
	Entity->PlaySound (CHAN_VOICE, SoundDie, 1, ATTN_NORM, 0);
	Entity->gameEntity->deadflag = DEAD_DEAD;
	Entity->gameEntity->takedamage = true;
	CurrentMove = &ParasiteMoveDeath;
}

/*
===
End Death Stuff
===
*/

/*QUAKED monster_parasite (1 .5 0) (-16 -16 -24) (16 16 32) Ambush Trigger_Spawn Sight
*/
void CParasite::Spawn ()
{
	SoundPain1 = SoundIndex ("parasite/parpain1.wav");	
	SoundPain2 = SoundIndex ("parasite/parpain2.wav");	
	SoundDie = SoundIndex ("parasite/pardeth1.wav");	
	SoundLaunch = SoundIndex("parasite/paratck1.wav");
	SoundImpact = SoundIndex("parasite/paratck2.wav");
	SoundSuck = SoundIndex("parasite/paratck3.wav");
	SoundReelIn = SoundIndex("parasite/paratck4.wav");
	SoundSight = SoundIndex("parasite/parsght1.wav");
	SoundTap = SoundIndex("parasite/paridle1.wav");
	SoundScratch = SoundIndex("parasite/paridle2.wav");
	SoundSearch = SoundIndex("parasite/parsrch1.wav");

	Entity->State.SetModelIndex(ModelIndex ("models/monsters/parasite/tris.md2"));
	Entity->SetMins (vec3f(-16, -16, -24));
	Entity->SetMaxs (vec3f(16, 16, 24));

	Entity->SetSolid (SOLID_BBOX);

	Entity->gameEntity->health = 175;
	Entity->gameEntity->gib_health = -50;
	Entity->gameEntity->mass = 250;

	MonsterFlags |= (MF_HAS_ATTACK | MF_HAS_IDLE | MF_HAS_SIGHT);

	Entity->Link ();

	CurrentMove = &ParasiteMoveStand;	
	WalkMonsterStart ();
}

LINK_MONSTER_CLASSNAME_TO_CLASS ("monster_parasite", CParasite);