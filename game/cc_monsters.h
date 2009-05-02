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
// cc_monsters.h
// Monsters
//

class CMonster;

class CFrame
{
public:

	void (CMonster::*Run) (float Dist);
	float	Dist;
	void (CMonster::*Function) ();

	CFrame ();

	CFrame(void (CMonster::*Run) (float Dist), float Dist, void (CMonster::*Function) () = NULL) :
	Run(Run),
	Dist(Dist),
	Function(Function)
	{
	};
};
class CAnim
{
public:

	int			FirstFrame, LastFrame;
	void		(CMonster::*EndFunc) ();

	CFrame	*Frames;

	CAnim (int FirstFrame, int LastFrame, CFrame *Frames, void (CMonster::*EndFunc) () = NULL) :
	FirstFrame(FirstFrame),
	LastFrame(LastFrame),
	EndFunc(EndFunc),
	Frames(Frames)
	{
	};
};

//monster ai flags
#define AI_STAND_GROUND			0x00000001
#define AI_TEMP_STAND_GROUND	0x00000002
#define AI_SOUND_TARGET			0x00000004
#define AI_LOST_SIGHT			0x00000008
#define AI_PURSUIT_LAST_SEEN	0x00000010
#define AI_PURSUE_NEXT			0x00000020
#define AI_PURSUE_TEMP			0x00000040
#define AI_HOLD_FRAME			0x00000080
#define AI_GOOD_GUY				0x00000100
#define AI_BRUTAL				0x00000200
#define AI_NOSTEP				0x00000400
#define AI_DUCKED				0x00000800
#define AI_COMBAT_POINT			0x00001000
#define AI_MEDIC				0x00002000
#define AI_RESURRECTING			0x00004000
#define AI_HAS_ATTACK			0x00008000
#define AI_HAS_MELEE			0x00010000

//monster attack state
#define AS_STRAIGHT				1
#define AS_SLIDING				2
#define AS_MELEE				3
#define AS_MISSILE				4

class CMonster
{
protected:
public:
	edict_t				*Entity; // Entity linked to the monster

	float				IdealYaw;
	edict_t				*GoalEntity;
	edict_t				*MoveTarget;
	float				YawSpeed;
	int					AIFlags;

	void				RunThink ();
	void				(CMonster::*Think) ();
	float				NextThink;
	int					NextFrame;
	float				Scale;
	float				PauseTime;
	float				AttackFinished;
	
	vec3_t				SavedGoal;
	float				SearchTime;
	float				TrailTime;
	vec3_t				LastSighting;
	int					AttackState;
	int					Lefty;
	float				IdleTime;
	int					LinkCount;

	int					PowerArmorType;
	int					PowerArmorPower;

	bool				EnemyInfront, EnemyVis;
	int					EnemyRange;
	float				EnemyYaw;

	CAnim				*CurrentMove;
	
	// Virtual functions
	virtual void		Stand			();
	virtual void		Idle			();
	virtual void		Search			();
	virtual void		Walk			();
	virtual void		Run				();
	virtual void		Dodge			(edict_t *other, float eta);
	virtual void		Attack			();
	virtual void		Melee			();
	virtual void		Sight			(edict_t *other);
	virtual bool		CheckAttack		();

	void				MonsterThink	();

	void AI_Charge (float Dist);
	void AI_Move (float Dist);
	void AI_Stand (float Dist);
	void AI_Run (float Dist);
	void AI_Run_Melee ();
	void AI_Run_Missile ();
	void AI_Run_Slide (float Dist);
	void AI_Turn (float Dist);
	void AI_Walk (float Dist);
	bool FacingIdeal ();
	bool FindTarget ();
	void FoundTarget ();
	void HuntTarget ();

	void MoveFrame ();

	bool AI_CheckAttack ();

	void CatagorizePosition ();
	void CheckGround ();

	void DropToFloor ();

	void FliesOff ();
	void FliesOn ();
	void CheckFlies ();

	void SetEffects ();
	void WorldEffects ();

	void MonsterDeathUse ();

	void MonsterFireBfg (vec3_t start, vec3_t aimdir, int damage, int speed, int kick, float damage_radius, int flashtype);
	void MonsterFireBlaster (vec3_t start, vec3_t dir, int damage, int speed, int flashtype, int effect);
	void MonsterFireGrenade (vec3_t start, vec3_t aimdir, int damage, int speed, int flashtype);
	void MonsterFireRailgun (vec3_t start, vec3_t aimdir, int damage, int kick, int flashtype);
	void MonsterFireShotgun (vec3_t start, vec3_t aimdir, int damage, int kick, int hspread, int vspread, int count, int flashtype);
	void MonsterFireBullet (vec3_t start, vec3_t dir, int damage, int kick, int hspread, int vspread, int flashtype);
	void MonsterFireRocket (vec3_t start, vec3_t dir, int damage, int speed, int flashtype);

	void MonsterTriggeredSpawn ();
	static void _cdecl MonsterTriggeredSpawnUse (edict_t *self, edict_t *other, edict_t *activator);
	static void _cdecl MonsterUse (edict_t *self, edict_t *other, edict_t *activator);
	void MonsterTriggeredStart ();

	void MonsterStart();
	void MonsterStartGo();

	void FlyMonsterStart ();
	void FlyMonsterStartGo ();
	void SwimMonsterStart ();
	void SwimMonsterStartGo ();
	void WalkMonsterStart ();
	void WalkMonsterStartGo ();

	void ChangeYaw ();
	bool CheckBottom ();
	void MoveToGoal (float Dist);
	bool WalkMove (float Yaw, float Dist);
	bool CloseEnough (edict_t *Goal, float Dist);
	void NewChaseDir (edict_t *Enemy, float Dist);
	bool StepDirection (float Yaw, float Dist);
	bool MoveStep (vec3_t move, bool ReLink);
};

#define DI_NODIR	-1

void RunMonsterThink (edict_t *ent);
