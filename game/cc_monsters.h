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

	void (CMonster::*AIFunc) (float Dist);
	void (CMonster::*Function) ();
	float	Dist;

	CFrame ();

	CFrame(void (CMonster::*AIFunc) (float Dist), float Dist, void (CMonster::*Function) () = NULL) :
	AIFunc(AIFunc),
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

#ifndef MONSTER_USE_ROGUE_AI
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
	#define	AI_SLIDE				0x00008000
#else
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

	//ROGUE
	#define AI_WALK_WALLS			0x00008000
	#define AI_MANUAL_STEERING		0x00010000
	#define AI_TARGET_ANGER			0x00020000
	#define AI_DODGING				0x00040000
	#define AI_CHARGING				0x00080000
	#define AI_HINT_PATH			0x00100000
	#define	AI_IGNORE_SHOTS			0x00200000
	// PMM - FIXME - last second added for E3 .. there's probably a better way to do this, but
	// this works
	#define	AI_DO_NOT_COUNT			0x00400000	// set for healed monsters
	#define	AI_SPAWNED_CARRIER		0x00800000	// both do_not_count and spawned are set for spawned monsters
	#define	AI_SPAWNED_MEDIC_C		0x01000000	// both do_not_count and spawned are set for spawned monsters
	#define	AI_SPAWNED_WIDOW		0x02000000	// both do_not_count and spawned are set for spawned monsters
	#define AI_SPAWNED_MASK			0x03800000	// mask to catch all three flavors of spawned
	#define	AI_BLOCKED				0x04000000	// used by blocked_checkattack: set to say I'm attacking while blocked 
												// (prevents run-attacks)
#endif

#define MF_HAS_MELEE			0x00000001
#define MF_HAS_IDLE				0x00000002
#define MF_HAS_SEARCH			0x00000004
#define	MF_HAS_SIGHT			0x00000008
#define MF_HAS_ATTACK			0x00000010
#ifdef MONSTER_USE_ROGUE_AI
#define MF_HAS_DODGE			0x00000020
#define MF_HAS_DUCK				0x00000040
#define MF_HAS_UNDUCK			0x00000080
#define MF_HAS_SIDESTEP			0x00000100
#endif

//monster attack state
enum EAttackState
{
	AS_STRAIGHT,
	AS_SLIDING,
	AS_MELEE,
	AS_MISSILE,
#ifdef MONSTER_USE_ROGUE_AI
	AS_BLIND
#endif
};

class CMonsterEntity : public CStepPhysics, public CTossProjectile, public CHurtableEntity, public CThinkableEntity, public CTouchableEntity
{
public:
	bool			TossPhysics;
	bool			BouncePhysics;
	bool			IsHead;

	class CMonster	*Monster;

	CMonsterEntity	();
	CMonsterEntity	(int Index);

	void			Think ();

	void Pain (CBaseEntity *other, float kick, int damage);
	void Die (CBaseEntity *inflictor, CBaseEntity *attacker, int damage, vec3_t point);

	virtual void	Touch (CBaseEntity *other, plane_t *plane, cmBspSurface_t *surf); // Empty

	bool			Run ();

	void			ThrowHead (MediaIndex gibIndex, int damage, int type);
};

class CMonster
{
protected:
public:
	void				(CMonster::*Think) ();

	uint32				MonsterID;

	// Hash
	uint32				HashValue;
	CMonster			*HashNext;

	char				*Classname;
	CMonsterEntity		*Entity; // Entity linked to the monster

	float				IdealYaw;
	edict_t				*GoalEntity;
	edict_t				*MoveTarget;
	float				YawSpeed;
	uint32				AIFlags;

#ifdef MONSTER_USE_ROGUE_AI
//ROGUE
	bool		BlindFire;		// will the monster blindfire?
	int			MedicTries;

	//  while abort_duck would be nice, only monsters which duck but don't sidestep would use it .. only the brain
	//  not really worth it.  sidestep is an implied abort_duck
//	void		(*abort_duck)(edict_t *self);
	float		BaseHeight;
	int32		NextDuckTime;
	int32		DuckWaitTime;
	int32		BlindFireDelay;
	edict_t		*LastPlayerEnemy;
	edict_t		*BadMedic1, *BadMedic2;	// these medics have declared this monster "unhealable"
	edict_t		*Healer;	// this is who is healing this monster
	vec3_t		BlindFireTarget;
	// blindfire stuff .. the boolean says whether the monster will do it, and blind_fire_time is the timing
	// (set in the monster) of the next shot
#endif

	int					NextFrame;
	float				Scale;
	int32				PauseTime;
	int32				AttackFinished;
	
	vec3_t				SavedGoal;
	int32				SearchTime;
	int32				TrailTime;
	vec3_t				LastSighting;
	int					AttackState;
	bool				Lefty;
	float				IdleTime;
	int					LinkCount;

	int					PowerArmorType;
	int					PowerArmorPower;

	bool				EnemyInfront, EnemyVis;
	int					EnemyRange;
	float				EnemyYaw;

	CAnim				*CurrentMove;

	uint32				MonsterFlags;

#ifdef MONSTERS_USE_PATHFINDING
	// Pathfinding
	CPath				*P_CurrentPath;
	CPathNode			*P_CurrentGoalNode;
	CPathNode			*P_CurrentNode; // Always the current path node
	int32				P_CurrentNodeIndex;
	int32				P_NodePathTimeout;
	int32				P_NodeFollowTimeout;
	bool				FollowingPath;

	// Pathfinding functions
	void	FoundPath		(); // Give it current and goal node and you can do this.
	void	MoveToPath		(float Dist);
#endif

	CMonster();

	virtual void	Touch (CBaseEntity *other, plane_t *plane, cmBspSurface_t *surf) {}; // Empty

#ifdef MONSTER_USE_ROGUE_AI
	void DuckDown ();
	virtual void Duck (float eta);
	virtual void UnDuck ();
	virtual void DuckHold ();
	virtual void SideStep ();
#endif

	virtual void Allocate (edict_t *ent) = 0;
	// Virtual functions
	virtual void		Stand			();
	virtual void		Idle			();
	virtual void		Search			();
	virtual void		Walk			();
	virtual void		Run				();
#ifndef MONSTER_USE_ROGUE_AI
	virtual void		Dodge			(edict_t *other, float eta);
#else
	virtual void		Dodge			(edict_t *attacker, float eta, CTrace *tr);
	void		DoneDodge	();
#endif
	virtual void		Attack			();
	virtual void		Melee			();
	virtual void		Sight			();
	virtual bool		CheckAttack		();

	virtual void		ReactToDamage	(edict_t *attacker);

	void				MonsterThink	();

	void AI_Charge (float Dist);
	void AI_Move (float Dist);
	void AI_Stand (float Dist);
	void AI_Run (float Dist);
	void AI_Run_Melee ();
	void AI_Run_Missile ();
	void AI_Run_Slide (float Dist);
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

#ifdef MONSTERS_ARENT_STUPID
	void AlertNearbyStroggs ();
	bool FriendlyInLine (vec3_t Origin, vec3_t Direction);
#endif

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

	// Called on a spawn
	void Init (edict_t *ent);
	virtual void	Spawn () = 0;
	virtual void	Die(CBaseEntity *inflictor, CBaseEntity *attacker, int damage, vec3_t point) = 0;
	virtual void	Pain(CBaseEntity *other, float kick, int damage) = 0;
};

#define DI_NODIR	-1

void Monster_Think (edict_t *ent);

#define ConvertDerivedFunction(x) static_cast<void (__thiscall CMonster::* )(void)>(x)
#define ConvertDerivedAIMove(x) static_cast<void (__thiscall CMonster::* )(float)>(x)
