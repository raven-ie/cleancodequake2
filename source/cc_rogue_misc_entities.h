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
// cc_rogue_misc_entities.h
// 
//

#if !defined(CC_GUARD_CC_ROGUE_MISC_ENTITIES_H) || !INCLUDE_GUARDS
#define CC_GUARD_CC_ROGUE_MISC_ENTITIES_H

#if ROGUE_FEATURES

class CPlatForm2 : public CPlatForm
{
public:
	/**
	\enum	

	\brief	Values that represent spawnflags pertaining to CPlatForm2. 
	**/
	enum
	{
		PLAT2_CALLED	= BIT(0),
		PLAT2_MOVING	= BIT(1),
		PLAT2_WAITING	= BIT(2)
	};

	bool			RequiresActivation;
	FrameNumber	LastMoveTime;
	EPlat2Flags		PlatFlags;
	class CBadArea	*BadArea;

	CPlatForm2();
	CPlatForm2(sint32 Index);

	void Blocked (IBaseEntity *Other);
	void Use (IBaseEntity *Other, IBaseEntity *Activator);
	void HitTop ();
	void HitBottom ();

	ENTITYFIELDS_SAVABLE(CPlatForm2)

	void DoEndFunc ();
	void GoDown ();
	void GoUp ();
	void Operate (IBaseEntity *Other);
	void Think ();
	void SpawnDangerArea ();
	void KillDangerArea ();

	CPlatFormInsideTrigger *SpawnInsideTrigger ();
	void Spawn ();
};

#endif

#else
FILE_WARNING
#endif