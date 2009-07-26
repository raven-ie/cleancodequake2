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

#define ConvertDerivedBrushModelEndFunc(x) static_cast<void (__thiscall CBrushModel::*) (void)>(x)
