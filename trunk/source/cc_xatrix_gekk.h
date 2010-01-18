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
// cc_xatrix_gekk.h
// 
//

#if !defined(CC_GUARD_CC_XATRIX_GEKK_H) || !INCLUDE_GUARDS
#define CC_GUARD_CC_XATRIX_GEKK_H

class CGekk : public CMonster
{
public:
	bool Jumping;

	MONSTER_SOUND_ENUM
	(
		SOUND_SWING,
		SOUND_HIT,
		SOUND_HIT2,
		SOUND_DEATH,
		SOUND_PAIN1,
		SOUND_SIGHT,
		SOUND_SEARCH,
		SOUND_STEP1,
		SOUND_STEP2,
		SOUND_STEP3,
		SOUND_THUD,
		SOUND_CHANT_LOW,
		SOUND_CHANT_MID,
		SOUND_CHANT_HIGH,

		SOUND_MAX
	);

	void SaveMonsterFields (CFile &File)
	{
		SAVE_MONSTER_SOUNDS
		File.Write<bool> (Jumping);
	}
	void LoadMonsterFields (CFile &File)
	{
		LOAD_MONSTER_SOUNDS
		Jumping = File.Read <bool> ();
	}

	CGekk (uint32 ID);

	bool CheckMelee ();
	bool CheckJump ();
	bool CheckJumpClose ();

	void Gekk_AI_Stand (float Dist);

	void LandToWater ();
	void WaterToLand ();
	void GibFest ();
	void IsGibFest ();
	void Jump ();
	void CheckLanding ();
	void StopSkid ();
	void JumpTakeoff2 ();
	void JumpTakeoff ();
	void Bite ();
	void CheckUnderwater ();
	void ReFireLoogie ();
	void Loogie ();
	void CheckMeleeRefire ();
	void HitLeft ();
	void HitRight ();
	void DoRun ();
	void IdleLoop ();
	void Chant ();
	void SwimLoop ();
	void Face ();
	void Swing ();
	void Step ();
	void Swim ();

	void Attack ();
	void Run (); // run start
	void Stand ();
	void Walk ();
	void Search ();
	void Idle ();
	void Sight ();
	void Melee ();
#if !MONSTER_USE_ROGUE_AI
	void Dodge (CBaseEntity *Attacker, float eta);
	void Duck_Down ();
	void Duck_Hold ();
	void Duck_Up ();
#else
	void Duck (float eta);
	void SideStep ();
#endif

	bool CheckAttack ();

	void Touch (CBaseEntity *Other, plane_t *plane, cmBspSurface_t *surf);

	void Dead ();
	void Die (CBaseEntity *Inflictor, CBaseEntity *Attacker, sint32 Damage, vec3f &point);
	void Pain (CBaseEntity *Other, sint32 Damage);
	void DamageEffect (vec3f &dir, vec3f &point, vec3f &normal, sint32 &damage, sint32 &dflags);

	void Spawn ();
};

class CLoogie : public CFlyMissileProjectile, public CThinkableEntity, public CTouchableEntity
{
public:
	sint32		Damage;

	CLoogie ();
	CLoogie (sint32 Index);

	IMPLEMENT_SAVE_HEADER (CLoogie)

	void SaveFields (CFile &File)
	{
		File.Write<sint32> (Damage);

		CThinkableEntity::SaveFields (File);
		CTouchableEntity::SaveFields (File);
		CFlyMissileProjectile::SaveFields (File);
	}

	void LoadFields (CFile &File)
	{
		Damage = File.Read <sint32>();

		CThinkableEntity::LoadFields (File);
		CTouchableEntity::LoadFields (File);
		CFlyMissileProjectile::LoadFields (File);
	}

	void Think ();
	void Touch (CBaseEntity *Other, plane_t *plane, cmBspSurface_t *surf);

	static void Spawn	(CBaseEntity *Spawner, vec3f start, vec3f dir,
						sint32 Damage, sint32 speed);

	bool Run ();
};

// Model data
#define FRAME_stand_01		0
#define FRAME_stand_02		1
#define FRAME_stand_03		2
#define FRAME_stand_04		3
#define FRAME_stand_05		4
#define FRAME_stand_06		5
#define FRAME_stand_07		6
#define FRAME_stand_08		7
#define FRAME_stand_09		8
#define FRAME_stand_10		9
#define FRAME_stand_11		10
#define FRAME_stand_12		11
#define FRAME_stand_13		12
#define FRAME_stand_14		13
#define FRAME_stand_15		14
#define FRAME_stand_16		15
#define FRAME_stand_17		16
#define FRAME_stand_18		17
#define FRAME_stand_19		18
#define FRAME_stand_20		19
#define FRAME_stand_21		20
#define FRAME_stand_22		21
#define FRAME_stand_23		22
#define FRAME_stand_24		23
#define FRAME_stand_25		24
#define FRAME_stand_26		25
#define FRAME_stand_27		26
#define FRAME_stand_28		27
#define FRAME_stand_29		28
#define FRAME_stand_30		29
#define FRAME_stand_31		30
#define FRAME_stand_32		31
#define FRAME_stand_33		32
#define FRAME_stand_34		33
#define FRAME_stand_35		34
#define FRAME_stand_36		35
#define FRAME_stand_37		36
#define FRAME_stand_38		37
#define FRAME_stand_39		38
#define FRAME_run_01		39
#define FRAME_run_02		40
#define FRAME_run_03		41
#define FRAME_run_04		42
#define FRAME_run_05		43
#define FRAME_run_06		44
#define FRAME_clawatk3_01		45
#define FRAME_clawatk3_02		46
#define FRAME_clawatk3_03		47
#define FRAME_clawatk3_04		48
#define FRAME_clawatk3_05		49
#define FRAME_clawatk3_06		50
#define FRAME_clawatk3_07		51
#define FRAME_clawatk3_08		52
#define FRAME_clawatk3_09		53
#define FRAME_clawatk4_01		54
#define FRAME_clawatk4_02		55
#define FRAME_clawatk4_03		56
#define FRAME_clawatk4_04		57
#define FRAME_clawatk4_05		58
#define FRAME_clawatk4_06		59
#define FRAME_clawatk4_07		60
#define FRAME_clawatk4_08		61
#define FRAME_clawatk5_01		62
#define FRAME_clawatk5_02		63
#define FRAME_clawatk5_03		64
#define FRAME_clawatk5_04		65
#define FRAME_clawatk5_05		66
#define FRAME_clawatk5_06		67
#define FRAME_clawatk5_07		68
#define FRAME_clawatk5_08		69
#define FRAME_clawatk5_09		70
#define FRAME_leapatk_01		71
#define FRAME_leapatk_02		72
#define FRAME_leapatk_03		73
#define FRAME_leapatk_04		74
#define FRAME_leapatk_05		75
#define FRAME_leapatk_06		76
#define FRAME_leapatk_07		77
#define FRAME_leapatk_08		78
#define FRAME_leapatk_09		79
#define FRAME_leapatk_10		80
#define FRAME_leapatk_11		81
#define FRAME_leapatk_12		82
#define FRAME_leapatk_13		83
#define FRAME_leapatk_14		84
#define FRAME_leapatk_15		85
#define FRAME_leapatk_16		86
#define FRAME_leapatk_17		87
#define FRAME_leapatk_18		88
#define FRAME_leapatk_19		89
#define FRAME_pain3_01		90
#define FRAME_pain3_02		91
#define FRAME_pain3_03		92
#define FRAME_pain3_04		93
#define FRAME_pain3_05		94
#define FRAME_pain3_06		95
#define FRAME_pain3_07		96
#define FRAME_pain3_08		97
#define FRAME_pain3_09		98
#define FRAME_pain3_10		99
#define FRAME_pain3_11		100
#define FRAME_pain4_01		101
#define FRAME_pain4_02		102
#define FRAME_pain4_03		103
#define FRAME_pain4_04		104
#define FRAME_pain4_05		105
#define FRAME_pain4_06		106
#define FRAME_pain4_07		107
#define FRAME_pain4_08		108
#define FRAME_pain4_09		109
#define FRAME_pain4_10		110
#define FRAME_pain4_11		111
#define FRAME_pain4_12		112
#define FRAME_pain4_13		113
#define FRAME_death1_01		114
#define FRAME_death1_02		115
#define FRAME_death1_03		116
#define FRAME_death1_04		117
#define FRAME_death1_05		118
#define FRAME_death1_06		119
#define FRAME_death1_07		120
#define FRAME_death1_08		121
#define FRAME_death1_09		122
#define FRAME_death1_10		123
#define FRAME_death2_01		124
#define FRAME_death2_02		125
#define FRAME_death2_03		126
#define FRAME_death2_04		127
#define FRAME_death2_05		128
#define FRAME_death2_06		129
#define FRAME_death2_07		130
#define FRAME_death2_08		131
#define FRAME_death2_09		132
#define FRAME_death2_10		133
#define FRAME_death2_11		134
#define FRAME_death3_01		135
#define FRAME_death3_02		136
#define FRAME_death3_03		137
#define FRAME_death3_04		138
#define FRAME_death3_05		139
#define FRAME_death3_06		140
#define FRAME_death3_07		141
#define FRAME_death4_01		142
#define FRAME_death4_02		143
#define FRAME_death4_03		144
#define FRAME_death4_04		145
#define FRAME_death4_05		146
#define FRAME_death4_06		147
#define FRAME_death4_07		148
#define FRAME_death4_08		149
#define FRAME_death4_09		150
#define FRAME_death4_10		151
#define FRAME_death4_11		152
#define FRAME_death4_12		153
#define FRAME_death4_13		154
#define FRAME_death4_14		155
#define FRAME_death4_15		156
#define FRAME_death4_16		157
#define FRAME_death4_17		158
#define FRAME_death4_18		159
#define FRAME_death4_19		160
#define FRAME_death4_20		161
#define FRAME_death4_21		162
#define FRAME_death4_22		163
#define FRAME_death4_23		164
#define FRAME_death4_24		165
#define FRAME_death4_25		166
#define FRAME_death4_26		167
#define FRAME_death4_27		168
#define FRAME_death4_28		169
#define FRAME_death4_29		170
#define FRAME_death4_30		171
#define FRAME_death4_31		172
#define FRAME_death4_32		173
#define FRAME_death4_33		174
#define FRAME_death4_34		175
#define FRAME_death4_35		176
#define FRAME_rduck_01		177
#define FRAME_rduck_02		178
#define FRAME_rduck_03		179
#define FRAME_rduck_04		180
#define FRAME_rduck_05		181
#define FRAME_rduck_06		182
#define FRAME_rduck_07		183
#define FRAME_rduck_08		184
#define FRAME_rduck_09		185
#define FRAME_rduck_10		186
#define FRAME_rduck_11		187
#define FRAME_rduck_12		188
#define FRAME_rduck_13		189
#define FRAME_lduck_01		190
#define FRAME_lduck_02		191
#define FRAME_lduck_03		192
#define FRAME_lduck_04		193
#define FRAME_lduck_05		194
#define FRAME_lduck_06		195
#define FRAME_lduck_07		196
#define FRAME_lduck_08		197
#define FRAME_lduck_09		198
#define FRAME_lduck_10		199
#define FRAME_lduck_11		200
#define FRAME_lduck_12		201
#define FRAME_lduck_13		202
#define FRAME_idle_01		203
#define FRAME_idle_02		204
#define FRAME_idle_03		205
#define FRAME_idle_04		206
#define FRAME_idle_05		207
#define FRAME_idle_06		208
#define FRAME_idle_07		209
#define FRAME_idle_08		210
#define FRAME_idle_09		211
#define FRAME_idle_10		212
#define FRAME_idle_11		213
#define FRAME_idle_12		214
#define FRAME_idle_13		215
#define FRAME_idle_14		216
#define FRAME_idle_15		217
#define FRAME_idle_16		218
#define FRAME_idle_17		219
#define FRAME_idle_18		220
#define FRAME_idle_19		221
#define FRAME_idle_20		222
#define FRAME_idle_21		223
#define FRAME_idle_22		224
#define FRAME_idle_23		225
#define FRAME_idle_24		226
#define FRAME_idle_25		227
#define FRAME_idle_26		228
#define FRAME_idle_27		229
#define FRAME_idle_28		230
#define FRAME_idle_29		231
#define FRAME_idle_30		232
#define FRAME_idle_31		233
#define FRAME_idle_32		234
#define FRAME_spit_01		235
#define FRAME_spit_02		236
#define FRAME_spit_03		237
#define FRAME_spit_04		238
#define FRAME_spit_05		239
#define FRAME_spit_06		240
#define FRAME_spit_07		241
#define FRAME_amb_01		242
#define FRAME_amb_02		243
#define FRAME_amb_03		244
#define FRAME_amb_04		245
#define FRAME_wdeath_01		246
#define FRAME_wdeath_02		247
#define FRAME_wdeath_03		248
#define FRAME_wdeath_04		249
#define FRAME_wdeath_05		250
#define FRAME_wdeath_06		251
#define FRAME_wdeath_07		252
#define FRAME_wdeath_08		253
#define FRAME_wdeath_09		254
#define FRAME_wdeath_10		255
#define FRAME_wdeath_11		256
#define FRAME_wdeath_12		257
#define FRAME_wdeath_13		258
#define FRAME_wdeath_14		259
#define FRAME_wdeath_15		260
#define FRAME_wdeath_16		261
#define FRAME_wdeath_17		262
#define FRAME_wdeath_18		263
#define FRAME_wdeath_19		264
#define FRAME_wdeath_20		265
#define FRAME_wdeath_21		266
#define FRAME_wdeath_22		267
#define FRAME_wdeath_23		268
#define FRAME_wdeath_24		269
#define FRAME_wdeath_25		270
#define FRAME_wdeath_26		271
#define FRAME_wdeath_27		272
#define FRAME_wdeath_28		273
#define FRAME_wdeath_29		274
#define FRAME_wdeath_30		275
#define FRAME_wdeath_31		276
#define FRAME_wdeath_32		277
#define FRAME_wdeath_33		278
#define FRAME_wdeath_34		279
#define FRAME_wdeath_35		280
#define FRAME_wdeath_36		281
#define FRAME_wdeath_37		282
#define FRAME_wdeath_38		283
#define FRAME_wdeath_39		284
#define FRAME_wdeath_40		285
#define FRAME_wdeath_41		286
#define FRAME_wdeath_42		287
#define FRAME_wdeath_43		288
#define FRAME_wdeath_44		289
#define FRAME_wdeath_45		290
#define FRAME_swim_01		291
#define FRAME_swim_02		292
#define FRAME_swim_03		293
#define FRAME_swim_04		294
#define FRAME_swim_05		295
#define FRAME_swim_06		296
#define FRAME_swim_07		297
#define FRAME_swim_08		298
#define FRAME_swim_09		299
#define FRAME_swim_10		300
#define FRAME_swim_11		301
#define FRAME_swim_12		302
#define FRAME_swim_13		303
#define FRAME_swim_14		304
#define FRAME_swim_15		305
#define FRAME_swim_16		306
#define FRAME_swim_17		307
#define FRAME_swim_18		308
#define FRAME_swim_19		309
#define FRAME_swim_20		310
#define FRAME_swim_21		311
#define FRAME_swim_22		312
#define FRAME_swim_23		313
#define FRAME_swim_24		314
#define FRAME_swim_25		315
#define FRAME_swim_26		316
#define FRAME_swim_27		317
#define FRAME_swim_28		318
#define FRAME_swim_29		319
#define FRAME_swim_30		320
#define FRAME_swim_31		321
#define FRAME_swim_32		322
#define FRAME_attack_01		323
#define FRAME_attack_02		324
#define FRAME_attack_03		325
#define FRAME_attack_04		326
#define FRAME_attack_05		327
#define FRAME_attack_06		328
#define FRAME_attack_07		329
#define FRAME_attack_08		330
#define FRAME_attack_09		331
#define FRAME_attack_10		332
#define FRAME_attack_11		333
#define FRAME_attack_12		334
#define FRAME_attack_13		335
#define FRAME_attack_14		336
#define FRAME_attack_15		337
#define FRAME_attack_16		338
#define FRAME_attack_17		339
#define FRAME_attack_18		340
#define FRAME_attack_19		341
#define FRAME_attack_20		342
#define FRAME_attack_21		343
#define FRAME_pain_01		344
#define FRAME_pain_02		345
#define FRAME_pain_03		346
#define FRAME_pain_04		347
#define FRAME_pain_05		348
#define FRAME_pain_06		349

#define MODEL_SCALE		1.000000

#else
FILE_WARNING
#endif