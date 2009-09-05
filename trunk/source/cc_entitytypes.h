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
// cc_entitytypes.h
// Different base entity classes
//

#if !defined(__CC_ENTITYTYPES_H__) || !defined(INCLUDE_GUARDS)
#define __CC_ENTITYTYPES_H__

// "Hurtable" entity
enum EDeadFlags
{
	DF_ALIVE,
	DF_DYING,
	DF_DEAD
};

class CHurtableEntity abstract : public virtual CBaseEntity
{
public:
	int			Health;
	int			MaxHealth;
	int			GibHealth;
	EDeadFlags	DeadFlag;
	bool		CanTakeDamage;
	CBaseEntity	*Enemy;

	CHurtableEntity ();
	CHurtableEntity (int index);

	virtual void Pain (CBaseEntity *other, float kick, int damage) = 0;
	virtual void Die (CBaseEntity *inflictor, CBaseEntity *attacker, int damage, vec3_t point) = 0;

	virtual bool CanDamage (CBaseEntity *inflictor);
	virtual bool CheckTeamDamage (CBaseEntity *attacker);
	virtual int CheckPowerArmor (vec3f &point, vec3f &normal, int damage, int dflags);
	virtual void Killed (CBaseEntity *inflictor, CBaseEntity *attacker, int damage, vec3f &point);

	// Takes damage.
	// For this, "this" is target. Use this if the
	// entity can be casted to CHurtableEntity or is CHurtableEntity
	// without question.
	virtual void TakeDamage (	CBaseEntity *inflictor, CBaseEntity *attacker,
							vec3f dir, vec3f point, vec3f normal, int damage,
							int knockback, int dflags, EMeansOfDeath mod);
	
	// This is a convenient static version.
	// This will cast targ to CHurtableEntity
	// and make the necessary damage adjustments if possible.
	// If "targ" can't take damage, nothing will happen.
	static void TakeDamage (	CBaseEntity *targ, CBaseEntity *inflictor,
							CBaseEntity *attacker, vec3f dir, vec3f point,
							vec3f normal, int damage, int knockback,
							int dflags, EMeansOfDeath mod);
};

class CBlockableEntity abstract : public virtual CBaseEntity
{
public:
	CBlockableEntity ();
	CBlockableEntity (int Index);

	virtual void Blocked (CBaseEntity *other) = 0;
};

class CUsableEntity abstract : public virtual CBaseEntity
{
public:
	CUsableEntity ();
	CUsableEntity (int Index);

	virtual void Use (CBaseEntity *other, CBaseEntity *activator) = 0;
};

// Thinkable entity
// Simple think function
class CThinkableEntity abstract : public virtual CBaseEntity
{
public:
	int32		NextThink;

	CThinkableEntity ();
	CThinkableEntity (int index);

	void			RunThink ();
	virtual void	Think () = 0;
	virtual void	PreThink () {};
};

// Touchable entity
class CTouchableEntity abstract : public virtual CBaseEntity
{
public:
	bool Touchable; // Setting to false is equivilent to putting touch = NULL in original Q2

	CTouchableEntity ();
	CTouchableEntity (int index);

	virtual void	Touch (CBaseEntity *other, plane_t *plane, cmBspSurface_t *surf);
};

typedef int32 EPhysicsType;
enum
{
	PHYSICS_NONE,

	PHYSICS_NOCLIP,
	PHYSICS_PUSH,
	PHYSICS_STOP,
	PHYSICS_WALK,
	PHYSICS_STEP,
	PHYSICS_FLY,
	PHYSICS_TOSS,
	PHYSICS_FLYMISSILE,
	PHYSICS_BOUNCE
};
// Contains common code that projectiles will use
class CPhysicsEntity abstract : public virtual CBaseEntity
{
public:
	EPhysicsType		PhysicsType;

	bool				PhysicsDisabled;
	CPhysicsEntity ();
	CPhysicsEntity (int index);

	class CTrace	PushEntity (vec3_t push);
	inline void		AddGravity ();
	void			Impact (CTrace *trace);
};

// "Bouncy" projectile
class CBounceProjectile abstract : public virtual CPhysicsEntity
{
public:
	float			backOff;

	CBounceProjectile ();
	CBounceProjectile (int index);

	bool			Run ();
};

// Same as bouncy, except doesn't bounce
class CTossProjectile abstract : public virtual CBounceProjectile
{
public:
	CTossProjectile();
	CTossProjectile (int index);
};

// Doesn't add gravity
class CFlyMissileProjectile abstract : public virtual CPhysicsEntity
{
public:
	CFlyMissileProjectile ();
	CFlyMissileProjectile (int index);

	bool			Run ();
};

// Gravity, special edge handling
class CStepPhysics abstract : public virtual CPhysicsEntity
{
public:
	CStepPhysics ();
	CStepPhysics (int index);

	virtual void	CheckGround ();

	int				FlyMove (float time, int mask);
	void			AddRotationalFriction ();
	bool			Run ();
};

class CPushPhysics abstract : public virtual CPhysicsEntity
{
public:
	CPushPhysics ();
	CPushPhysics (int Index);

	//bool			Push (vec3_t move, vec3_t amove);
	bool			Run ();
};

class CStopPhysics abstract : public virtual CPushPhysics
{
public:
	CStopPhysics ();
	CStopPhysics (int Index);

	bool			Run ();
};

#else
FILE_WARNING
#endif