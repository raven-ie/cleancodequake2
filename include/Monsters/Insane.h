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
// cc_insane.h
// Insane marine
//

#if !defined(CC_GUARD_INSANE_H) || !INCLUDE_GUARDS
#define CC_GUARD_INSANE_H

class CInsane : public CMonster
{
public:
	enum
	{
		FRAME_stand1,
		FRAME_stand2,
		FRAME_stand3,
		FRAME_stand4,
		FRAME_stand5,
		FRAME_stand6,
		FRAME_stand7,
		FRAME_stand8,
		FRAME_stand9,
		FRAME_stand10,
		FRAME_stand11,
		FRAME_stand12,
		FRAME_stand13,
		FRAME_stand14,
		FRAME_stand15,
		FRAME_stand16,
		FRAME_stand17,
		FRAME_stand18,
		FRAME_stand19,
		FRAME_stand20,
		FRAME_stand21,
		FRAME_stand22,
		FRAME_stand23,
		FRAME_stand24,
		FRAME_stand25,
		FRAME_stand26,
		FRAME_stand27,
		FRAME_stand28,
		FRAME_stand29,
		FRAME_stand30,
		FRAME_stand31,
		FRAME_stand32,
		FRAME_stand33,
		FRAME_stand34,
		FRAME_stand35,
		FRAME_stand36,
		FRAME_stand37,
		FRAME_stand38,
		FRAME_stand39,
		FRAME_stand40,
		FRAME_stand41,
		FRAME_stand42,
		FRAME_stand43,
		FRAME_stand44,
		FRAME_stand45,
		FRAME_stand46,
		FRAME_stand47,
		FRAME_stand48,
		FRAME_stand49,
		FRAME_stand50,
		FRAME_stand51,
		FRAME_stand52,
		FRAME_stand53,
		FRAME_stand54,
		FRAME_stand55,
		FRAME_stand56,
		FRAME_stand57,
		FRAME_stand58,
		FRAME_stand59,
		FRAME_stand60,
		FRAME_stand61,
		FRAME_stand62,
		FRAME_stand63,
		FRAME_stand64,
		FRAME_stand65,
		FRAME_stand66,
		FRAME_stand67,
		FRAME_stand68,
		FRAME_stand69,
		FRAME_stand70,
		FRAME_stand71,
		FRAME_stand72,
		FRAME_stand73,
		FRAME_stand74,
		FRAME_stand75,
		FRAME_stand76,
		FRAME_stand77,
		FRAME_stand78,
		FRAME_stand79,
		FRAME_stand80,
		FRAME_stand81,
		FRAME_stand82,
		FRAME_stand83,
		FRAME_stand84,
		FRAME_stand85,
		FRAME_stand86,
		FRAME_stand87,
		FRAME_stand88,
		FRAME_stand89,
		FRAME_stand90,
		FRAME_stand91,
		FRAME_stand92,
		FRAME_stand93,
		FRAME_stand94,
		FRAME_stand95,
		FRAME_stand96,
		FRAME_stand97,
		FRAME_stand98,
		FRAME_stand99,
		FRAME_stand100,
		FRAME_stand101,
		FRAME_stand102,
		FRAME_stand103,
		FRAME_stand104,
		FRAME_stand105,
		FRAME_stand106,
		FRAME_stand107,
		FRAME_stand108,
		FRAME_stand109,
		FRAME_stand110,
		FRAME_stand111,
		FRAME_stand112,
		FRAME_stand113,
		FRAME_stand114,
		FRAME_stand115,
		FRAME_stand116,
		FRAME_stand117,
		FRAME_stand118,
		FRAME_stand119,
		FRAME_stand120,
		FRAME_stand121,
		FRAME_stand122,
		FRAME_stand123,
		FRAME_stand124,
		FRAME_stand125,
		FRAME_stand126,
		FRAME_stand127,
		FRAME_stand128,
		FRAME_stand129,
		FRAME_stand130,
		FRAME_stand131,
		FRAME_stand132,
		FRAME_stand133,
		FRAME_stand134,
		FRAME_stand135,
		FRAME_stand136,
		FRAME_stand137,
		FRAME_stand138,
		FRAME_stand139,
		FRAME_stand140,
		FRAME_stand141,
		FRAME_stand142,
		FRAME_stand143,
		FRAME_stand144,
		FRAME_stand145,
		FRAME_stand146,
		FRAME_stand147,
		FRAME_stand148,
		FRAME_stand149,
		FRAME_stand150,
		FRAME_stand151,
		FRAME_stand152,
		FRAME_stand153,
		FRAME_stand154,
		FRAME_stand155,
		FRAME_stand156,
		FRAME_stand157,
		FRAME_stand158,
		FRAME_stand159,
		FRAME_stand160,
		FRAME_walk27,
		FRAME_walk28,
		FRAME_walk29,
		FRAME_walk30,
		FRAME_walk31,
		FRAME_walk32,
		FRAME_walk33,
		FRAME_walk34,
		FRAME_walk35,
		FRAME_walk36,
		FRAME_walk37,
		FRAME_walk38,
		FRAME_walk39,
		FRAME_walk1,
		FRAME_walk2,
		FRAME_walk3,
		FRAME_walk4,
		FRAME_walk5,
		FRAME_walk6,
		FRAME_walk7,
		FRAME_walk8,
		FRAME_walk9,
		FRAME_walk10,
		FRAME_walk11,
		FRAME_walk12,
		FRAME_walk13,
		FRAME_walk14,
		FRAME_walk15,
		FRAME_walk16,
		FRAME_walk17,
		FRAME_walk18,
		FRAME_walk19,
		FRAME_walk20,
		FRAME_walk21,
		FRAME_walk22,
		FRAME_walk23,
		FRAME_walk24,
		FRAME_walk25,
		FRAME_walk26,
		FRAME_st_pain2,
		FRAME_st_pain3,
		FRAME_st_pain4,
		FRAME_st_pain5,
		FRAME_st_pain6,
		FRAME_st_pain7,
		FRAME_st_pain8,
		FRAME_st_pain9,
		FRAME_st_pain10,
		FRAME_st_pain11,
		FRAME_st_pain12,
		FRAME_st_death2,
		FRAME_st_death3,
		FRAME_st_death4,
		FRAME_st_death5,
		FRAME_st_death6,
		FRAME_st_death7,
		FRAME_st_death8,
		FRAME_st_death9,
		FRAME_st_death10,
		FRAME_st_death11,
		FRAME_st_death12,
		FRAME_st_death13,
		FRAME_st_death14,
		FRAME_st_death15,
		FRAME_st_death16,
		FRAME_st_death17,
		FRAME_st_death18,
		FRAME_crawl1,
		FRAME_crawl2,
		FRAME_crawl3,
		FRAME_crawl4,
		FRAME_crawl5,
		FRAME_crawl6,
		FRAME_crawl7,
		FRAME_crawl8,
		FRAME_crawl9,
		FRAME_cr_pain2,
		FRAME_cr_pain3,
		FRAME_cr_pain4,
		FRAME_cr_pain5,
		FRAME_cr_pain6,
		FRAME_cr_pain7,
		FRAME_cr_pain8,
		FRAME_cr_pain9,
		FRAME_cr_pain10,
		FRAME_cr_death10,
		FRAME_cr_death11,
		FRAME_cr_death12,
		FRAME_cr_death13,
		FRAME_cr_death14,
		FRAME_cr_death15,
		FRAME_cr_death16,
		FRAME_cross1,
		FRAME_cross2,
		FRAME_cross3,
		FRAME_cross4,
		FRAME_cross5,
		FRAME_cross6,
		FRAME_cross7,
		FRAME_cross8,
		FRAME_cross9,
		FRAME_cross10,
		FRAME_cross11,
		FRAME_cross12,
		FRAME_cross13,
		FRAME_cross14,
		FRAME_cross15,
		FRAME_cross16,
		FRAME_cross17,
		FRAME_cross18,
		FRAME_cross19,
		FRAME_cross20,
		FRAME_cross21,
		FRAME_cross22,
		FRAME_cross23,
		FRAME_cross24,
		FRAME_cross25,
		FRAME_cross26,
		FRAME_cross27,
		FRAME_cross28,
		FRAME_cross29,
		FRAME_cross30,
	};

	MediaIndex	SoundScream[8];
	inline const char	*GetMonsterName() { return "Insane Marine"; }

	MONSTER_SOUND_ENUM
	(
		SOUND_FIST,
		SOUND_SHAKE,
		SOUND_MOAN,

		SOUND_MAX
	)

	CInsane (uint32 ID);

	void SaveMonsterFields (CFile &File)
	{
		SAVE_MONSTER_SOUNDS
		for (uint8 i = 0; i < 8; i++)
			WriteIndex (File, SoundScream[i], INDEX_SOUND);
	}
	void LoadMonsterFields (CFile &File)
	{
		LOAD_MONSTER_SOUNDS
		for (uint8 i = 0; i < 8; i++)
			ReadIndex (File, SoundScream[i], INDEX_SOUND);
	}

	void Fist ();
	void Shake ();
	void Moan ();
	void Scream ();
	void CheckDown ();
	void OnGround ();
	void CheckUp ();
	void Cross ();

	void Run ();
	void Stand ();
	void Walk ();

	void Dead ();
	void Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point);
	void Pain (IBaseEntity *Other, sint32 Damage);

	void Spawn ();
	
	MONSTER_ID_HEADER
};

#else
FILE_WARNING
#endif
