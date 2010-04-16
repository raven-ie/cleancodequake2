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
// cc_flyer.h
// Flyer Monster
//

#if !defined(CC_GUARD_FLYER_H) || !INCLUDE_GUARDS
#define CC_GUARD_FLYER_H

class CFlyer : public CMonster
{
public:
	MONSTER_SOUND_ENUM
	(
		SOUND_SIGHT,
		SOUND_IDLE,
		SOUND_PAIN1,
		SOUND_PAIN2,
		SOUND_SLASH,
		SOUND_SPROING,
		SOUND_DIE,

		SOUND_MAX
	);

	CFlyer (uint32 ID);

	MONSTER_SAVE_LOAD_NO_FIELDS

	ROGUE_VIRTUAL void Attack ();
#if (MONSTER_SPECIFIC_FLAGS & FLYER_KNOWS_HOW_TO_DODGE)
#if ROGUE_FEATURES
	void Duck (float eta);
	void SideStep ();
#else
	void Dodge (IBaseEntity *Attacker, float eta);
#endif

	void ChooseAfterDodge ();
	void AI_Roll (float Dist);
#endif
	void Idle ();
	ROGUE_VIRTUAL void Run ();
	void Sight ();
	ROGUE_VIRTUAL void Stand ();
	ROGUE_VIRTUAL void Walk ();
	ROGUE_VIRTUAL void Melee ();

	void CheckMelee ();
	void LoopMelee ();
	void SlashLeft ();
	void SlashRight ();
	void PopBlades ();
	void FireLeft ();
	void FireRight ();
	void Fire (sint32 FlashNumber);

	void Dead ();
	ROGUE_VIRTUAL void Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &point);
	ROGUE_VIRTUAL void Pain (IBaseEntity *Other, sint32 Damage);

	void Spawn ();
};

#else
FILE_WARNING
#endif
