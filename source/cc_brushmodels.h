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
// cc_brushmodels.h
// 
//

enum
{
	BRUSHTHINK_NONE,

	BRUSHTHINK_MOVEBEGIN,
	BRUSHTHINK_MOVEFINAL,
	BRUSHTHINK_MOVEDONE,

	BRUSHTHINK_MOVEACCEL,

	BRUSHTHINK_AMOVEBEGIN,
	BRUSHTHINK_AMOVEFINAL,
	BRUSHTHINK_AMOVEDONE,

	BRUSHTHINK_CUSTOM_START,
};

// Contains code common to brush models
class CBrushModel : public virtual CBaseEntity, public CThinkableEntity, public CPushPhysics, public CStopPhysics
{
public:
	// fixed data
	vec3_t		StartOrigin;
	vec3_t		StartAngles;
	vec3_t		EndOrigin;
	vec3_t		EndAngles;

	MediaIndex	SoundStart;
	MediaIndex	SoundMiddle;
	MediaIndex	SoundEnd;

	float		Accel;
	float		Speed;
	float		Decel;
	float		Distance;

	float		Wait;

	// state data
	int			MoveState;
	vec3_t		Dir;
	float		CurrentSpeed;
	float		MoveSpeed;
	float		NextSpeed;
	float		RemainingDistance;
	float		DecelDistance;
	uint32		EndFunc;
	virtual void	DoEndFunc () {};

	uint32		ThinkType;

	CBrushModel ();
	CBrushModel (int Index);

	// Origin/velocity
	void MoveDone ();
	void MoveFinal ();
	void MoveBegin ();
	void MoveCalc (vec3_t dest, uint32 EndFunc);

	// Angle/avelocity
	void AngleMoveDone ();
	void AngleMoveFinal ();
	void AngleMoveBegin ();
	void AngleMoveCalc (uint32 EndFunc);

	// Accel
	inline float AccelerationDistance (const float target, const float rate)
	{
		return (target * ((target / rate) + 1) / 2);
	}

	void CalcAcceleratedMove();
	void Accelerate ();
	void ThinkAccelMove ();

	virtual void Think ();
	bool Run ();
};

class CPlatForm : public CMapEntity, public CBrushModel, public CBlockableEntity, public CUsableEntity
{
public:
	enum
	{
		PLATTHINK_GO_DOWN = BRUSHTHINK_CUSTOM_START
	};
	CPlatForm();
	CPlatForm(int Index);

	bool Run ();
	void Blocked (CBaseEntity *other);
	void Use (CBaseEntity *other, CBaseEntity *activator);
	void HitTop ();
	void HitBottom ();

	enum
	{
		PLATENDFUNC_HITBOTTOM,
		PLATENDFUNC_HITTOP
	};

	void DoEndFunc ();
	void GoDown ();
	void GoUp ();
	void Think ();

	class CPlatFormInsideTrigger : public CTouchableEntity
	{
	public:
		CPlatFormInsideTrigger::CPlatFormInsideTrigger ();
		CPlatFormInsideTrigger::CPlatFormInsideTrigger (int Index);
		void Touch (CBaseEntity *other, plane_t *plane, cmBspSurface_t *surf);
	};

	void SpawnInsideTrigger ();
	void Spawn ();
};

// Base door class
class CDoor : public CMapEntity, public CBrushModel, public CHurtableEntity, public CBlockableEntity, public CTouchableEntity, public CUsableEntity
{
public:
	enum
	{
		DOORTHINK_SPAWNDOORTRIGGER = BRUSHTHINK_CUSTOM_START,
		DOORTHINK_CALCMOVESPEED,

		DOORTHINK_GODOWN,
	};
	CDoor();
	CDoor(int Index);

	void UseAreaPortals (bool isOpen);
	bool Run ();

	enum
	{
		DOORENDFUNC_HITBOTTOM,
		DOORENDFUNC_HITTOP
	};

	virtual void HitTop ();
	virtual void HitBottom ();
	virtual void GoDown ();
	virtual void GoUp (CBaseEntity *activator);
	virtual void DoEndFunc ();
	virtual void Think ();

	class CDoorTrigger : public CTouchableEntity
	{
	public:
		CDoorTrigger::CDoorTrigger ();
		CDoorTrigger::CDoorTrigger (int Index);

		void Touch (CBaseEntity *other, plane_t *plane, cmBspSurface_t *surf);
	};

	void Blocked (CBaseEntity *other);
	void Use (CBaseEntity *other, CBaseEntity *activator);
	void Die (CBaseEntity *inflictor, CBaseEntity *attacker, int damage, vec3_t point);
	void Pain (CBaseEntity *other, float kick, int damage);
	void Touch (CBaseEntity *other, plane_t *plane, cmBspSurface_t *surf);

	// Thinks
	void SpawnDoorTrigger ();
	void CalcMoveSpeed ();

	virtual void Spawn ();
};

class CRotatingDoor : public CDoor
{
public:
	CRotatingDoor ();
	CRotatingDoor (int Index);

	void GoDown();
	void GoUp (CBaseEntity *activator);

	void Spawn ();
};