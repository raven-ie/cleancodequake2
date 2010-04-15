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
// cc_rogue_stalker.cpp
// 
//

#include "cc_local.h"

#if ROGUE_FEATURES
#include "cc_rogue_stalker.h"
#include "m_stalker.h"

CStalker::CStalker (uint32 ID) :
CMonster(ID)
{
	Scale = MODEL_SCALE;
	MonsterName = "Stalker";
}

//extern qboolean SV_PointCloseEnough (edict_t *ent, vec3_t goal, float dist);

//=========================
//=========================
bool CStalker::OKToTransition ()
{
	float MaxDist = -384;
	float Margin = Entity->GetMins().Z - 8;

	if (!OnCeiling())
	{
		// her stalkers are just better
		if (AIFlags & AI_SPAWNED_WIDOW)
			MaxDist = 256;
		else
			MaxDist = 180;
		Margin = Entity->GetMaxs().Z + 8;
	}

	CTrace trace (Entity->State.GetOrigin(), Entity->GetMins(), Entity->GetMaxs(), Entity->State.GetOrigin() + vec3f(0, 0, MaxDist), Entity, CONTENTS_MASK_MONSTERSOLID);

	if (trace.fraction == 1.0 || 
		!(trace.contents & CONTENTS_SOLID) ||
		(trace.Ent != World))
	{
		if (OnCeiling())
		{
			if (trace.plane.normal.Z < 0.9)
				return false;
		}
		else
		{
			if (trace.plane.normal.Z > -0.9)
				return false;
		}
	}

	float end_height = trace.EndPos.Z;

	// check the four corners, tracing only to the endpoint of the center trace (vertically).
	vec3f pt (Entity->GetAbsMin().X, Entity->GetAbsMin().Y, trace.EndPos.Z + Margin);
	vec3f start (pt);
	start.Z = Entity->State.GetOrigin().Z;
	
	trace (start, pt, Entity, CONTENTS_MASK_MONSTERSOLID);
	if (trace.fraction == 1.0 || !(trace.contents & CONTENTS_SOLID) || (trace.Ent != World))
		return false;

	if (abs(end_height + Margin - trace.EndPos.Z) > 8)
		return false;

	pt.Set (Entity->GetAbsMax().X, Entity->GetAbsMin().Y, pt.Z);
	start = pt;
	start.Z = Entity->State.GetOrigin().Z;

	trace (start, pt, Entity, CONTENTS_MASK_MONSTERSOLID);
	if (trace.fraction == 1.0 || !(trace.contents & CONTENTS_SOLID) || (trace.Ent != World))
		return false;
	if (abs(end_height + Margin - trace.EndPos.Z) > 8)
		return false;

	pt.Set (Entity->GetAbsMax().X, Entity->GetAbsMax().Y, pt.Z);
	start = pt;
	start.Z = Entity->State.GetOrigin().Z;

	trace (start, pt, Entity, CONTENTS_MASK_MONSTERSOLID);
	if (trace.fraction == 1.0 || !(trace.contents & CONTENTS_SOLID) || (trace.Ent != World))
		return false;
	if (abs(end_height + Margin - trace.EndPos.Z) > 8)
		return false;

	pt.Set (Entity->GetAbsMin().X, Entity->GetAbsMax().Y, pt.Z);
	start = pt;
	start.Z = Entity->State.GetOrigin().Z;
	trace (start, pt, Entity, CONTENTS_MASK_MONSTERSOLID);
	if (trace.fraction == 1.0 || !(trace.contents & CONTENTS_SOLID) || (trace.Ent != World))
		return false;
	if (abs(end_height + Margin - trace.EndPos.Z) > 8)
		return false;

	return true;
}

//=========================
//=========================
void CStalker::Sight ()
{
	Entity->PlaySound (CHAN_WEAPON, Sounds[SOUND_SIGHT]);
}

// ******************
// IDLE
// ******************

void CStalker::IdleNoise ()
{
	Entity->PlaySound (CHAN_WEAPON, Sounds[SOUND_IDLE]);
}

CFrame StalkerFramesIdle [] =
{
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),

	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0, ConvertDerivedFunction(&CStalker::IdleNoise)),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),

	CFrame (&CMonster::AI_Stand, 0),
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
CAnim StalkerMoveIdle (FRAME_idle01, FRAME_idle21, StalkerFramesIdle, &CMonster::Stand);

CFrame StalkerFramesIdle2 [] =
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
	CFrame (&CMonster::AI_Stand, 0)
};
CAnim StalkerMoveIdle2 (FRAME_idle201, FRAME_idle213, StalkerFramesIdle2, &CMonster::Stand);

void CStalker::Idle ()
{ 
	CurrentMove = frand() < 0.35 ? &StalkerMoveIdle : &StalkerMoveIdle2;
}

// ******************
// STAND
// ******************

CFrame StalkerFramesStand [] =
{
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),

	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0, ConvertDerivedFunction(&CStalker::IdleNoise)),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),

	CFrame (&CMonster::AI_Stand, 0),
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
CAnim	StalkerMoveStand (FRAME_idle01, FRAME_idle21, StalkerFramesStand, &CMonster::Stand);

void CStalker::Stand ()
{
	CurrentMove = (frand() < 0.25) ? &StalkerMoveStand : &StalkerMoveIdle2;
}

// ******************
// RUN
// ******************

CFrame StalkerFramesRun [] =
{
	CFrame (&CMonster::AI_Run, 13),
	CFrame (&CMonster::AI_Run, 17),
	CFrame (&CMonster::AI_Run, 21),
	CFrame (&CMonster::AI_Run, 18)
};
CAnim StalkerMoveRun (FRAME_run01, FRAME_run04, StalkerFramesRun);

void CStalker::Run ()
{
	if (AIFlags & AI_STAND_GROUND)
		CurrentMove = &StalkerMoveStand;
	else
		CurrentMove = &StalkerMoveRun;
}

// ******************
// WALK
// ******************

CFrame StalkerFramesWalk [] =
{
	CFrame (&CMonster::AI_Walk, 4),
	CFrame (&CMonster::AI_Walk, 6),
	CFrame (&CMonster::AI_Walk, 8),
	CFrame (&CMonster::AI_Walk, 5),

	CFrame (&CMonster::AI_Walk, 4),
	CFrame (&CMonster::AI_Walk, 6),
	CFrame (&CMonster::AI_Walk, 8),
	CFrame (&CMonster::AI_Walk, 4)
};
CAnim StalkerMoveWalk (FRAME_walk01, FRAME_walk08, StalkerFramesWalk, &CMonster::Walk);

void CStalker::Walk ()
{
	CurrentMove = &StalkerMoveWalk;
}

// ******************
// false death
// ******************
CFrame StalkerFramesReactivate [] = 
{
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0)
};
CAnim StalkerMoveFalseDeathEnd (FRAME_reactive01, FRAME_reactive04, StalkerFramesReactivate, &CMonster::Run);

void CStalker::Reactivate ()
{
	//AIFlags &= ~AI_STAND_GROUND;
	CurrentMove = &StalkerMoveFalseDeathEnd;
}

void CStalker::Heal ()
{
	switch (CvarList[CV_SKILL].Integer())
	{
	case 2:
		Entity->Health += 2;
		break;
	case 3:
		Entity->Health += 3;
		break;
	default:
		Entity->Health++;
		break;
	};

	if (Entity->Health > (Entity->MaxHealth / 2))
		Entity->State.GetSkinNum() = 0;

	if (Entity->Health >= Entity->MaxHealth)
	{
		Entity->Health = Entity->MaxHealth;
		Reactivate ();
	}
}

CFrame StalkerFramesFalseDeath [] =
{
	CFrame (&CMonster::AI_Move, 0, ConvertDerivedFunction(&CStalker::Heal)),
	CFrame (&CMonster::AI_Move, 0, ConvertDerivedFunction(&CStalker::Heal)),
	CFrame (&CMonster::AI_Move, 0, ConvertDerivedFunction(&CStalker::Heal)),
	CFrame (&CMonster::AI_Move, 0, ConvertDerivedFunction(&CStalker::Heal)),
	CFrame (&CMonster::AI_Move, 0, ConvertDerivedFunction(&CStalker::Heal)),

	CFrame (&CMonster::AI_Move, 0, ConvertDerivedFunction(&CStalker::Heal)),
	CFrame (&CMonster::AI_Move, 0, ConvertDerivedFunction(&CStalker::Heal)),
	CFrame (&CMonster::AI_Move, 0, ConvertDerivedFunction(&CStalker::Heal)),
	CFrame (&CMonster::AI_Move, 0, ConvertDerivedFunction(&CStalker::Heal)),
	CFrame (&CMonster::AI_Move, 0, ConvertDerivedFunction(&CStalker::Heal))
};
CAnim StalkerMoveFalseDeath (FRAME_twitch01, FRAME_twitch10, StalkerFramesFalseDeath, ConvertDerivedFunction(&CStalker::FalseDeath));

void CStalker::FalseDeath ()
{
	CurrentMove = &StalkerMoveFalseDeath;
}

CFrame StalkerFramesFalseDeathStart [] =
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
};
CAnim StalkerMoveFalseDeathStart (FRAME_death01, FRAME_death09, StalkerFramesFalseDeathStart, ConvertDerivedFunction(&CStalker::FalseDeath));

void CStalker::FalseDeathStart ()
{
	Entity->State.GetAngles().Z = 0;
	Entity->GravityVector.Set (0, 0, -1);

	//AIFlags |= AI_STAND_GROUND;
	CurrentMove = &StalkerMoveFalseDeathStart;
}


// ******************
// PAIN
// ******************

CFrame StalkerFramesPain [] =
{
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0)
};
CAnim StalkerMovePain (FRAME_pain01, FRAME_pain04, StalkerFramesPain, &CMonster::Run);

void CStalker::Pain (IBaseEntity *Other, sint32 Damage)
{
	if (Entity->DeadFlag)
		return;

	if (Entity->Health < (Entity->MaxHealth / 2)) 
		Entity->State.GetSkinNum() = 1;

	if (CvarList[CV_SKILL].Integer() == 3)
		return;		// no pain anims in nightmare

	if (Entity->GroundEntity == NULL)
		return;

	// if we're reactivating or false dying, ignore the pain.
	if (CurrentMove == &StalkerMoveFalseDeathEnd ||
		CurrentMove == &StalkerMoveFalseDeathStart)
		return;

	if (CurrentMove == &StalkerMoveFalseDeath)
	{
		Reactivate ();
		return;
	}

	if ((Entity->Health > 0) && (Entity->Health < (Entity->MaxHealth / 4)))
	{
		if(frand() < (0.2 * CvarList[CV_SKILL].Integer()))
		{
			if(!OnCeiling() || OKToTransition() )
			{
				FalseDeathStart ();
				return;
			}
		}	
	}

	if (Level.Frame < PainDebounceTime)
		return;

	PainDebounceTime = Level.Frame + 30;

	if (Damage > 10)		// don't react unless the damage was significant
	{
		// stalker should dodge jump periodically to help avoid damage.
		if (Entity->GroundEntity && (frand() < 0.5f))
			DodgeJump ();
		else
			CurrentMove = &StalkerMovePain;

		Entity->PlaySound (CHAN_WEAPON, Sounds[SOUND_PAIN]);
	}
}


// ******************
// STALKER ATTACK
// ******************

void CStalker::ShootAttack ()
{
	if (!HasValidEnemy ())
		return;

	if (Entity->GroundEntity && frand() < 0.33f)
	{
		float dist = (Entity->Enemy->State.GetOrigin() - Entity->State.GetOrigin()).Length();

		if((dist > 256) || (frand() < 0.5))
			DoPounce (Entity->Enemy->State.GetOrigin());
		else
			JumpStraightUp ();
	}

	vec3f f, r, start, end;
	Entity->State.GetAngles().ToVectors (&f, &r, NULL);
	G_ProjectSource (Entity->State.GetOrigin(), vec3f(24, 0, 6), f, r, start);

	vec3f dir = (Entity->Enemy->State.GetOrigin() - start);

	if ((Entity->Enemy->EntityFlags & ENT_HURTABLE) && frand() < (0.20f + 0.1f * CvarList[CV_SKILL].Float()))
	{
		end = Entity->Enemy->State.GetOldOrigin().MultiplyAngles (dir.Length() / 1000, entity_cast<IPhysicsEntity>(Entity->Enemy)->Velocity);
		dir = end - start;
	}	
	else
		end = Entity->Enemy->State.GetOrigin();

	CTrace trace (start, end, Entity, CONTENTS_MASK_SHOT);

	if (trace.Ent == Entity->Enemy || trace.Ent == World)
		MonsterFireBlaster2 (start, dir, 15, 800, MZ2_STALKER_BLASTER, EF_BLASTER);
}

void CStalker::ShootAttack2 ()
{
	if (frand() < (0.4f + (0.1f * CvarList[CV_SKILL].Float())))
		ShootAttack ();
}

CFrame StalkerFramesShoot [] =
{
	CFrame (&CMonster::AI_Charge, 13),
	CFrame (&CMonster::AI_Charge, 17, ConvertDerivedFunction(&CStalker::ShootAttack)),
	CFrame (&CMonster::AI_Charge, 21),
	CFrame (&CMonster::AI_Charge, 18, ConvertDerivedFunction(&CStalker::ShootAttack2))
};
CAnim StalkerMoveShoot (FRAME_run01, FRAME_run04, StalkerFramesShoot, &CMonster::Run);

void CStalker::Attack ()
{
	if (!HasValidEnemy())
		return;

	// PMM - circle strafe stuff
	if (frand() > (1.0f - (0.5f/CvarList[CV_SKILL].Float())))
		AttackState = AS_STRAIGHT;
	else
	{
		if (frand() <= 0.5) // switch directions
			Lefty = !Lefty;

		AttackState = AS_SLIDING;
	}

	CurrentMove = &StalkerMoveShoot;
}

// ******************
// close combat
// ******************

void CStalker::SwingAttack ()
{
	static vec3f aim (MELEE_DISTANCE, 0, 0);

	if (CMeleeWeapon::Fire (Entity, aim, (5 + (rand() % 5)), 50))
		Entity->PlaySound (CHAN_WEAPON, (Entity->State.GetFrame() < FRAME_attack08) ? Sounds[SOUND_PUNCH_HIT1] : Sounds[SOUND_PUNCH_HIT2]);
}

CFrame StalkerFramesSwingL [] =
{
	CFrame (&CMonster::AI_Charge, 2),
	CFrame (&CMonster::AI_Charge, 4),
	CFrame (&CMonster::AI_Charge, 6),
	CFrame (&CMonster::AI_Charge, 10),

	CFrame (&CMonster::AI_Charge, 5, ConvertDerivedFunction(&CStalker::SwingAttack)),
	CFrame (&CMonster::AI_Charge, 5),
	CFrame (&CMonster::AI_Charge, 5),
	CFrame (&CMonster::AI_Charge, 5)
};
CAnim StalkerMoveSwingL (FRAME_attack01, FRAME_attack08, StalkerFramesSwingL, &CMonster::Run);

CFrame StalkerFramesSwingR [] =
{
	CFrame (&CMonster::AI_Charge, 4),
	CFrame (&CMonster::AI_Charge, 6),
	CFrame (&CMonster::AI_Charge, 6, ConvertDerivedFunction(&CStalker::SwingAttack)),
	CFrame (&CMonster::AI_Charge, 10),
	CFrame (&CMonster::AI_Charge, 5)
};
CAnim StalkerMoveSwingR (FRAME_attack11, FRAME_attack15, StalkerFramesSwingR, &CMonster::Run);

void CStalker::Melee ()
{
	if (!HasValidEnemy())
		return;
	
	CurrentMove = (frand() < 0.5f) ? &StalkerMoveSwingL : &StalkerMoveSwingR;
}


// ******************
// POUNCE
// ******************

#define FAUX_GRAVITY	800.0

// ====================
// ====================
void CalcJumpAngle(vec3f start, vec3f end, float velocity, vec3f &angles)
{
	vec3f	dist = (end - start);

	float distH = sqrtf(dist[0]*dist[0] + dist[1]*dist[1]);
	float distV = dist[2];

	if (distV < 0)
		distV = 0 - distV;

	if (distV)
	{
		float l = sqrtf(distH*distH + distV*distV);
		float U = atanf(distV / distH);
		if (dist[2] > 0)
			U = -U;

		angles[2] = 0.0;

		float cosU = cosf(U);
		float one = l * FAUX_GRAVITY * (cosU * cosU);
		one = one / (velocity * velocity);
		one = one - sinf(U);
		angles[0] = asinf(one);
		if(_isnan(angles[0]))
			angles[2] = 1.0;
		angles[1] = M_PI - angles[0];
		if(_isnan(angles[1]))
			angles[2] = 1.0;

		angles[0] = RAD2DEG ( (angles[0] - U) / 2.0 );
		angles[1] = RAD2DEG ( (angles[1] - U) / 2.0 );
	}
	else
	{
		float l = sqrtf(distH*distH + distV*distV);

		angles[2] = 0.0;

		float one = l * FAUX_GRAVITY;
		one = one / (velocity * velocity);
		angles[0] = asinf(one);
		if(_isnan(angles[0]))
			angles[2] = 1.0;
		angles[1] = M_PI - angles[0];
		if(_isnan(angles[1]))
			angles[2] = 1.0;

		angles[0] = RAD2DEG ( (angles[0]) / 2.0 );
		angles[1] = RAD2DEG ( (angles[1]) / 2.0 );
	}
}

// ====================
// ====================
bool CStalker::CheckLZ (IBaseEntity *target, vec3f dest)
{
	if (!(target->EntityFlags & ENT_HURTABLE) || !(target->EntityFlags & ENT_PHYSICS))
		return false;

	if ((PointContents (dest) & CONTENTS_MASK_WATER) || entity_cast<IPhysicsEntity>(target)->WaterInfo.Level)
		return false;

	if (!target->GroundEntity)
		return false;

	// check under the player's four corners
	// if they're not solid, bail.
	vec3f jumpLZ = Entity->Enemy->GetMins();
	jumpLZ.Z -= 0.25f;
	if (!(PointContents (jumpLZ) & CONTENTS_MASK_SOLID))
		return false;

	jumpLZ.X = Entity->Enemy->GetMaxs().X;
	jumpLZ.Y = Entity->Enemy->GetMins().Y;
	if (!(PointContents (jumpLZ) & CONTENTS_MASK_SOLID))
		return false;

	jumpLZ.X = Entity->Enemy->GetMaxs().X;
	jumpLZ.Y = Entity->Enemy->GetMaxs().Y;
	if (!(PointContents (jumpLZ) & CONTENTS_MASK_SOLID))
		return false;

	jumpLZ[0] = Entity->Enemy->GetMins().X;
	jumpLZ[1] = Entity->Enemy->GetMaxs().Y;
	if (!(PointContents (jumpLZ) & CONTENTS_MASK_SOLID))
		return false;

	return true;
}

// ====================
// ====================
bool CStalker::DoPounce (vec3f dest)
{
	// don't pounce when we're on the ceiling
	if (OnCeiling())
		return false;

	if (!CheckLZ (Entity->Enemy, dest))
		return false;

	vec3f dist = dest - Entity->State.GetOrigin();
	vec3f jumpAngles = dist.ToAngles();
	
	// make sure we're pointing in that direction 15deg margin of error.
	if (fabs(jumpAngles.Y - Entity->State.GetAngles().Y) > 45)
		return false;			// not facing the player...

	IdealYaw = jumpAngles.Y;
	ChangeYaw();

	if(dist.Length() > 450)
		return false;			// can't jump that far...

	vec3f jumpLZ = dest;
	int preferHighJump = 0;

	// if we're having to jump up a distance, jump a little too high to compensate.
	if (dist.Z >= 32.0)
	{
		preferHighJump = 1;
		jumpLZ.Z += 32;
	}

	CTrace trace (Entity->State.GetOrigin(), dest, Entity, CONTENTS_MASK_MONSTERSOLID);

	if ((trace.fraction < 1) && (trace.Ent != Entity->Enemy))
		preferHighJump = 1; 

	// find a valid angle/velocity combination
	float velocity = 400.1f;
	while (velocity <= 800)
	{
		CalcJumpAngle(Entity->State.GetOrigin(), jumpLZ, velocity, jumpAngles);
		if ((!isnan(jumpAngles.X)) || (!isnan(jumpAngles.Y)))
			break;
		
		velocity += 200;
	};

	vec3f forward;
	Entity->State.GetAngles().ToVectors (&forward, NULL, NULL);
	forward.Normalize();

	if (!preferHighJump && (!isnan(jumpAngles.X)) )
	{
		Entity->Velocity = (forward * (velocity * cosf(DEG2RAD(jumpAngles.X))));
		Entity->Velocity.Z = velocity * sinf(DEG2RAD(jumpAngles.Y)) + (0.5f * CvarList[CV_GRAVITY].Float() * FRAMETIME);
		
		return true;
	}

	if (!isnan(jumpAngles.Y))
	{
		Entity->Velocity = (forward * (velocity * cosf(DEG2RAD(jumpAngles.Y))));
		Entity->Velocity.Z = velocity * sinf(DEG2RAD(jumpAngles.Y)) + (0.5f * CvarList[CV_GRAVITY].Float() * FRAMETIME);
		return true;
	}

	return false;
}

// ******************
// DODGE
// ******************

//===================
// stalker_jump_straightup
//===================
void CStalker::JumpStraightUp ()
{
	if (Entity->DeadFlag)
		return;

	if (OnCeiling())
	{
		if (OKToTransition())
		{
			Entity->GravityVector.Z = -1;
			
			Entity->State.GetAngles().Z += 180.0;
			if (Entity->State.GetAngles().Z > 360.0)
				Entity->State.GetAngles().Z -= 360.0;
			
			Entity->GroundEntity = NULL;
		}
	}
	else if (Entity->GroundEntity)	// make sure we're standing on SOMETHING...
	{
		Entity->Velocity.X += ((frand() * 10) - 5);
		Entity->Velocity.Y += ((frand() * 10) - 5);
		Entity->Velocity.Z += -400 * Entity->GravityVector.Z;

		if (OKToTransition())
		{
			Entity->GravityVector.Z = 1;
			Entity->State.GetAngles().Z = 180.0;
			Entity->GroundEntity = NULL;
		}
	}
	TimeStamp = Level.Frame;
}

CFrame StalkerFramesJumpStraightUp [] =
{
	CFrame (&CMonster::AI_Move, 1, ConvertDerivedFunction(&CStalker::JumpStraightUp)),
	CFrame (&CMonster::AI_Move, 1, ConvertDerivedFunction(&CStalker::JumpWaitLand)),
	CFrame (&CMonster::AI_Move, -1),
	CFrame (&CMonster::AI_Move, -1)
};
CAnim StalkerMoveJumpStraightUp (FRAME_jump04, FRAME_jump07, StalkerFramesJumpStraightUp, &CMonster::Run);

//===================
// stalker_dodge_jump - abstraction so pain function can trigger a dodge jump too without
//		faking the inputs to stalker_dodge
//===================

void CStalker::JumpWaitLand ()
{
	if ((frand() < (0.3f + (0.1f*CvarList[CV_SKILL].Float()))) && (Level.Frame >= AttackFinished))
	{
		AttackFinished = Level.Frame + 3;
		ShootAttack ();
	}
	
	if (!Entity->GroundEntity)
	{
		Entity->GravityMultiplier = 1.3f;
		NextFrame = Entity->State.GetFrame();

		if ((Level.Frame - TimeStamp) > 30)
		{
			Entity->GravityMultiplier = 1;
			NextFrame = Entity->State.GetFrame() + 1;
		}
	}
	else 
	{
		Entity->GravityMultiplier = 1;
		NextFrame = Entity->State.GetFrame() + 1;
	}
}

void CStalker::DodgeJump ()
{
	CurrentMove = &StalkerMoveJumpStraightUp;
}

CFrame StalkerFramesDodgeRun [] =
{
	CFrame (&CMonster::AI_Run, 13),
	CFrame (&CMonster::AI_Run, 17),
	CFrame (&CMonster::AI_Run, 21),
	CFrame (&CMonster::AI_Run, 18
#if ROGUE_FEATURES
	, &CMonster::DoneDodge
#endif
	)
};
CAnim StalkerMoveDodgeRun (FRAME_run01, FRAME_run04, StalkerFramesDodgeRun);

void CStalker::Dodge (IBaseEntity *Attacker, float eta
#if ROGUE_FEATURES
		, CTrace *tr
#endif
		)
{
	if (!Entity->GroundEntity || Entity->Health <= 0)
		return;

	if (!Entity->Enemy)
	{
		Entity->Enemy = Attacker;
		FoundTarget ();
		return;
	}
	
	// PMM - don't bother if it's going to hit anyway; fix for weird in-your-face etas (I was
	// seeing numbers like 13 and 14)
	if ((eta < 0.1) || (eta > 5))
		return;

	// this will override the foundtarget call of stalker_run
	DodgeJump ();
}

// ******************
// Death
// ******************

void CStalker::Dead ()
{
	Entity->GetMins().Set (-28, -28, -18);
	Entity->GetMaxs().Set (28, 28, -4);
	Entity->PhysicsType = PHYSICS_TOSS;
	Entity->GetSvFlags() |= SVF_DEADMONSTER;
	Entity->NextThink = 0;
	Entity->Link ();
}

CFrame StalkerFramesDeath [] =
{
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, -5),
	CFrame (&CMonster::AI_Move, -10),
	CFrame (&CMonster::AI_Move, -20),
	
	CFrame (&CMonster::AI_Move, -10),
	CFrame (&CMonster::AI_Move, -10),
	CFrame (&CMonster::AI_Move, -5),
	CFrame (&CMonster::AI_Move, -5),

	CFrame (&CMonster::AI_Move, 0)
};
CAnim StalkerMoveDeath (FRAME_death01, FRAME_death09, StalkerFramesDeath, ConvertDerivedFunction(&CStalker::Dead));

void CStalker::Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &point)
{
	Entity->PhysicsType = PHYSICS_TOSS;
	Entity->State.GetAngles().Z = 0;
	Entity->GravityVector.Set (0, 0, -1);

// check for gib
	if (Entity->Health <= Entity->GibHealth)
	{
		Entity->PlaySound (CHAN_VOICE, SoundIndex ("misc/udeath.wav"));
		for (int n = 0; n < 2; n++)
			CGibEntity::Spawn (Entity, GameMedia.Gib_Bone[0], Damage, GIB_ORGANIC);
		for (int n = 0; n < 4; n++)
			CGibEntity::Spawn (Entity, GameMedia.Gib_SmallMeat, Damage, GIB_ORGANIC);
		Entity->ThrowHead (GameMedia.Gib_Head[1], Damage, GIB_ORGANIC);
		Entity->DeadFlag = true;
		return;
	}

	if (Entity->DeadFlag)
		return;

// regular death
	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_DIE]);
	Entity->DeadFlag = true;
	CurrentMove = &StalkerMoveDeath;
}


// ******************
// SPAWN
// ******************

/*QUAKED monster_stalker (1 .5 0) (-28 -28 -18) (28 28 18) Ambush Trigger_Spawn Sight OnRoof
Spider Monster

  ONROOF - Monster starts sticking to the roof.
*/
#define STALKER_ON_ROOF 8

void CStalker::Spawn ()
{
	Sounds[SOUND_PAIN] = SoundIndex ("stalker/pain.wav");	
	Sounds[SOUND_DIE] = SoundIndex ("stalker/death.wav");	
	Sounds[SOUND_SIGHT] = SoundIndex("stalker/sight.wav");
	Sounds[SOUND_PUNCH_HIT1] = SoundIndex ("stalker/melee1.wav");
	Sounds[SOUND_PUNCH_HIT2] = SoundIndex ("stalker/melee2.wav");
	Sounds[SOUND_IDLE] = SoundIndex ("stalker/idle.wav");

	// PMM - precache bolt2
	ModelIndex ("models/proj/laser2/tris.md2");

	Entity->State.GetModelIndex() = ModelIndex ("models/monsters/stalker/tris.md2");
	Entity->GetMins().Set (-28, -28, -18);
	Entity->GetMaxs().Set (28, 28, 18);
	Entity->PhysicsType = PHYSICS_STEP;
	Entity->GetSolid() = SOLID_BBOX;

	Entity->Health = 250;
	Entity->GibHealth = -50; 
	Entity->Mass = 250;

	MonsterFlags = (MF_HAS_ATTACK | MF_HAS_MELEE | MF_HAS_SIGHT | MF_HAS_IDLE);

	Entity->Link ();

	CurrentMove = &StalkerMoveStand;	

	AIFlags |= AI_WALK_WALLS;

	if (Entity->SpawnFlags & STALKER_ON_ROOF)
	{
		Entity->State.GetAngles().Z = 180;
		Entity->GravityVector.Z = 1;
	}

	WalkMonsterStart ();
}

LINK_MONSTER_CLASSNAME_TO_CLASS ("monster_stalker", CStalker);
#endif
