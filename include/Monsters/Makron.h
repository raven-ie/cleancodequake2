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
// cc_makron.h
// 
//

#if !defined(CC_GUARD_MAKRON_H) || !INCLUDE_GUARDS
#define CC_GUARD_MAKRON_H

class CMakron : public IMonster
{
public:
	enum
	{
		FRAME_attak101,
		FRAME_attak102,
		FRAME_attak103,
		FRAME_attak104,
		FRAME_attak105,
		FRAME_attak106,
		FRAME_attak107,
		FRAME_attak108,
		FRAME_attak109,
		FRAME_attak110,
		FRAME_attak111,
		FRAME_attak112,
		FRAME_attak113,
		FRAME_attak114,
		FRAME_attak115,
		FRAME_attak116,
		FRAME_attak117,
		FRAME_attak118,
		FRAME_attak201,
		FRAME_attak202,
		FRAME_attak203,
		FRAME_attak204,
		FRAME_attak205,
		FRAME_attak206,
		FRAME_attak207,
		FRAME_attak208,
		FRAME_attak209,
		FRAME_attak210,
		FRAME_attak211,
		FRAME_attak212,
		FRAME_attak213,
		FRAME_death01,
		FRAME_death02,
		FRAME_death03,
		FRAME_death04,
		FRAME_death05,
		FRAME_death06,
		FRAME_death07,
		FRAME_death08,
		FRAME_death09,
		FRAME_death10,
		FRAME_death11,
		FRAME_death12,
		FRAME_death13,
		FRAME_death14,
		FRAME_death15,
		FRAME_death16,
		FRAME_death17,
		FRAME_death18,
		FRAME_death19,
		FRAME_death20,
		FRAME_death21,
		FRAME_death22,
		FRAME_death33,
		FRAME_death24,
		FRAME_death25,
		FRAME_death26,
		FRAME_death27,
		FRAME_death28,
		FRAME_death29,
		FRAME_death30,
		FRAME_death31,
		FRAME_death32,
		FRAME_death33_,
		FRAME_death34,
		FRAME_death35,
		FRAME_death36,
		FRAME_death37,
		FRAME_death38,
		FRAME_death39,
		FRAME_death40,
		FRAME_death41,
		FRAME_death42,
		FRAME_death43,
		FRAME_death44,
		FRAME_death45,
		FRAME_death46,
		FRAME_death47,
		FRAME_death48,
		FRAME_death49,
		FRAME_death50,
		FRAME_pain101,
		FRAME_pain102,
		FRAME_pain103,
		FRAME_pain201,
		FRAME_pain202,
		FRAME_pain203,
		FRAME_pain301,
		FRAME_pain302,
		FRAME_pain303,
		FRAME_pain304,
		FRAME_pain305,
		FRAME_pain306,
		FRAME_pain307,
		FRAME_pain308,
		FRAME_pain309,
		FRAME_pain310,
		FRAME_pain311,
		FRAME_pain312,
		FRAME_pain313,
		FRAME_pain314,
		FRAME_pain315,
		FRAME_pain316,
		FRAME_pain317,
		FRAME_pain318,
		FRAME_pain319,
		FRAME_pain320,
		FRAME_pain321,
		FRAME_pain322,
		FRAME_pain323,
		FRAME_pain324,
		FRAME_pain325,
		FRAME_stand01,
		FRAME_stand02,
		FRAME_stand03,
		FRAME_stand04,
		FRAME_stand05,
		FRAME_stand06,
		FRAME_stand07,
		FRAME_stand08,
		FRAME_stand09,
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
		FRAME_walk01,
		FRAME_walk02,
		FRAME_walk03,
		FRAME_walk04,
		FRAME_walk05,
		FRAME_walk06,
		FRAME_walk07,
		FRAME_walk08,
		FRAME_walk09,
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
		FRAME_active01,
		FRAME_active02,
		FRAME_active03,
		FRAME_active04,
		FRAME_active05,
		FRAME_active06,
		FRAME_active07,
		FRAME_active08,
		FRAME_active09,
		FRAME_active10,
		FRAME_active11,
		FRAME_active12,
		FRAME_active13,
		FRAME_attak301,
		FRAME_attak302,
		FRAME_attak303,
		FRAME_attak304,
		FRAME_attak305,
		FRAME_attak306,
		FRAME_attak307,
		FRAME_attak308,
		FRAME_attak401,
		FRAME_attak402,
		FRAME_attak403,
		FRAME_attak404,
		FRAME_attak405,
		FRAME_attak406,
		FRAME_attak407,
		FRAME_attak408,
		FRAME_attak409,
		FRAME_attak410,
		FRAME_attak411,
		FRAME_attak412,
		FRAME_attak413,
		FRAME_attak414,
		FRAME_attak415,
		FRAME_attak416,
		FRAME_attak417,
		FRAME_attak418,
		FRAME_attak419,
		FRAME_attak420,
		FRAME_attak421,
		FRAME_attak422,
		FRAME_attak423,
		FRAME_attak424,
		FRAME_attak425,
		FRAME_attak426,
		FRAME_attak501,
		FRAME_attak502,
		FRAME_attak503,
		FRAME_attak504,
		FRAME_attak505,
		FRAME_attak506,
		FRAME_attak507,
		FRAME_attak508,
		FRAME_attak509,
		FRAME_attak510,
		FRAME_attak511,
		FRAME_attak512,
		FRAME_attak513,
		FRAME_attak514,
		FRAME_attak515,
		FRAME_attak516,
		FRAME_death201,
		FRAME_death202,
		FRAME_death203,
		FRAME_death204,
		FRAME_death205,
		FRAME_death206,
		FRAME_death207,
		FRAME_death208,
		FRAME_death209,
		FRAME_death210,
		FRAME_death211,
		FRAME_death212,
		FRAME_death213,
		FRAME_death214,
		FRAME_death215,
		FRAME_death216,
		FRAME_death217,
		FRAME_death218,
		FRAME_death219,
		FRAME_death220,
		FRAME_death221,
		FRAME_death222,
		FRAME_death223,
		FRAME_death224,
		FRAME_death225,
		FRAME_death226,
		FRAME_death227,
		FRAME_death228,
		FRAME_death229,
		FRAME_death230,
		FRAME_death231,
		FRAME_death232,
		FRAME_death233,
		FRAME_death234,
		FRAME_death235,
		FRAME_death236,
		FRAME_death237,
		FRAME_death238,
		FRAME_death239,
		FRAME_death240,
		FRAME_death241,
		FRAME_death242,
		FRAME_death243,
		FRAME_death244,
		FRAME_death245,
		FRAME_death246,
		FRAME_death247,
		FRAME_death248,
		FRAME_death249,
		FRAME_death250,
		FRAME_death251,
		FRAME_death252,
		FRAME_death253,
		FRAME_death254,
		FRAME_death255,
		FRAME_death256,
		FRAME_death257,
		FRAME_death258,
		FRAME_death259,
		FRAME_death260,
		FRAME_death261,
		FRAME_death262,
		FRAME_death263,
		FRAME_death264,
		FRAME_death265,
		FRAME_death266,
		FRAME_death267,
		FRAME_death268,
		FRAME_death269,
		FRAME_death270,
		FRAME_death271,
		FRAME_death272,
		FRAME_death273,
		FRAME_death274,
		FRAME_death275,
		FRAME_death276,
		FRAME_death277,
		FRAME_death278,
		FRAME_death279,
		FRAME_death280,
		FRAME_death281,
		FRAME_death282,
		FRAME_death283,
		FRAME_death284,
		FRAME_death285,
		FRAME_death286,
		FRAME_death287,
		FRAME_death288,
		FRAME_death289,
		FRAME_death290,
		FRAME_death291,
		FRAME_death292,
		FRAME_death293,
		FRAME_death294,
		FRAME_death295,
		FRAME_death301,
		FRAME_death302,
		FRAME_death303,
		FRAME_death304,
		FRAME_death305,
		FRAME_death306,
		FRAME_death307,
		FRAME_death308,
		FRAME_death309,
		FRAME_death310,
		FRAME_death311,
		FRAME_death312,
		FRAME_death313,
		FRAME_death314,
		FRAME_death315,
		FRAME_death316,
		FRAME_death317,
		FRAME_death318,
		FRAME_death319,
		FRAME_death320,
		FRAME_jump01,
		FRAME_jump02,
		FRAME_jump03,
		FRAME_jump04,
		FRAME_jump05,
		FRAME_jump06,
		FRAME_jump07,
		FRAME_jump08,
		FRAME_jump09,
		FRAME_jump10,
		FRAME_jump11,
		FRAME_jump12,
		FRAME_jump13,
		FRAME_pain401,
		FRAME_pain402,
		FRAME_pain403,
		FRAME_pain404,
		FRAME_pain501,
		FRAME_pain502,
		FRAME_pain503,
		FRAME_pain504,
		FRAME_pain601,
		FRAME_pain602,
		FRAME_pain603,
		FRAME_pain604,
		FRAME_pain605,
		FRAME_pain606,
		FRAME_pain607,
		FRAME_pain608,
		FRAME_pain609,
		FRAME_pain610,
		FRAME_pain611,
		FRAME_pain612,
		FRAME_pain613,
		FRAME_pain614,
		FRAME_pain615,
		FRAME_pain616,
		FRAME_pain617,
		FRAME_pain618,
		FRAME_pain619,
		FRAME_pain620,
		FRAME_pain621,
		FRAME_pain622,
		FRAME_pain623,
		FRAME_pain624,
		FRAME_pain625,
		FRAME_pain626,
		FRAME_pain627,
		FRAME_stand201,
		FRAME_stand202,
		FRAME_stand203,
		FRAME_stand204,
		FRAME_stand205,
		FRAME_stand206,
		FRAME_stand207,
		FRAME_stand208,
		FRAME_stand209,
		FRAME_stand210,
		FRAME_stand211,
		FRAME_stand212,
		FRAME_stand213,
		FRAME_stand214,
		FRAME_stand215,
		FRAME_stand216,
		FRAME_stand217,
		FRAME_stand218,
		FRAME_stand219,
		FRAME_stand220,
		FRAME_stand221,
		FRAME_stand222,
		FRAME_stand223,
		FRAME_stand224,
		FRAME_stand225,
		FRAME_stand226,
		FRAME_stand227,
		FRAME_stand228,
		FRAME_stand229,
		FRAME_stand230,
		FRAME_stand231,
		FRAME_stand232,
		FRAME_stand233,
		FRAME_stand234,
		FRAME_stand235,
		FRAME_stand236,
		FRAME_stand237,
		FRAME_stand238,
		FRAME_stand239,
		FRAME_stand240,
		FRAME_stand241,
		FRAME_stand242,
		FRAME_stand243,
		FRAME_stand244,
		FRAME_stand245,
		FRAME_stand246,
		FRAME_stand247,
		FRAME_stand248,
		FRAME_stand249,
		FRAME_stand250,
		FRAME_stand251,
		FRAME_stand252,
		FRAME_stand253,
		FRAME_stand254,
		FRAME_stand255,
		FRAME_stand256,
		FRAME_stand257,
		FRAME_stand258,
		FRAME_stand259,
		FRAME_stand260,
		FRAME_walk201,
		FRAME_walk202,
		FRAME_walk203,
		FRAME_walk204,
		FRAME_walk205,
		FRAME_walk206,
		FRAME_walk207,
		FRAME_walk208,
		FRAME_walk209,
		FRAME_walk210,
		FRAME_walk211,
		FRAME_walk212,
		FRAME_walk213,
		FRAME_walk214,
		FRAME_walk215,
		FRAME_walk216,
		FRAME_walk217,
	};

	vec3f		SavedLoc;
	inline const char	*GetMonsterName() { return "Makron"; }

	MONSTER_SOUND_ENUM
	(
		SOUND_PAIN4,
		SOUND_PAIN5,
		SOUND_PAIN6,
		SOUND_DEATH,
		SOUND_STEPLEFT,
		SOUND_STEPRIGHT,
		SOUND_ATTACK_BFG,
		SOUND_BRAIN_SPLORCH,
		SOUND_PRE_RAILGUN,
		SOUND_POPUP,
		SOUND_TAUNT1,
		SOUND_TAUNT2,
		SOUND_TAUNT3,
		SOUND_HIT,

		SOUND_MAX
	);

	CMakron (uint32 ID);

	void SaveMonsterFields (CFile &File)
	{
		SAVE_MONSTER_SOUNDS
		File.Write<vec3f> (SavedLoc);
	}

	void LoadMonsterFields (CFile &File)
	{
		LOAD_MONSTER_SOUNDS
		SavedLoc = File.Read<vec3f> ();
	}

	void Run ();
	void Stand ();
	void Walk ();
	void Attack();
	bool CheckAttack ();
	void Sight ();

	void Taunt ();
	void StepLeft ();
	void StepRight ();
	void Hit ();
	void PopUp ();
	void BrainSplorch ();
	void PreRailgun ();
	void FireBFG ();
	void FireHyperblaster ();
	void FireRailgun ();
	void SavePosition ();

	void Dead ();
	void Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point);
	void Pain (IBaseEntity *Other, sint32 Damage);

	void Spawn ();
	static void Precache ();
	static void Toss (IBaseEntity *Spawner);
	
	MONSTER_ID_HEADER
};

class CMakronJumpTimer : public virtual IBaseEntity, public IThinkableEntity
{
public:
	CMonsterEntity	*LinkedJorg;

	CMakronJumpTimer ();
	CMakronJumpTimer (sint32 Index);

	IMPLEMENT_SAVE_HEADER(CMakronJumpTimer)

	void SaveFields (CFile &File)
	{
		File.Write<sint32> ((LinkedJorg) ? LinkedJorg->State.GetNumber() : -1);

		IThinkableEntity::SaveFields (File);
	}

	void LoadFields (CFile &File)
	{
		sint32 Index = File.Read<sint32> ();

		if (Index != -1)
			LinkedJorg = entity_cast<CMonsterEntity>(Game.Entities[Index].Entity);

		IThinkableEntity::LoadFields (File);
	}

	void Think ();
	static void Spawn (class CJorg *Jorg);
};

#else
FILE_WARNING
#endif
