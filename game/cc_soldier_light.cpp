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
// cc_soldier_light.cpp
// Bew pew gew.
//

#include "cc_local.h"

CSoldierLight Monster_Soldier_Light;

CSoldierLight::CSoldierLight ()
{
	Classname = "monster_soldier_light";
}

void CSoldierLight::Allocate (edict_t *ent)
{
	ent->Monster = new CSoldierLight(Monster_Soldier_Light);
}

extern CAnim SoldierMoveAttack1;
extern CAnim SoldierMoveAttack2;

void CSoldierLight::Attack ()
{
	if (random() < 0.5)
		CurrentMove = &SoldierMoveAttack1;
	else
		CurrentMove = &SoldierMoveAttack2;
}

static int BlasterFlash [] = {MZ2_SOLDIER_BLASTER_1, MZ2_SOLDIER_BLASTER_2, MZ2_SOLDIER_BLASTER_3, MZ2_SOLDIER_BLASTER_4, MZ2_SOLDIER_BLASTER_5, MZ2_SOLDIER_BLASTER_6, MZ2_SOLDIER_BLASTER_7, MZ2_SOLDIER_BLASTER_8};
void CSoldierLight::FireGun (int FlashNumber)
{
	vec3_t	start;
	vec3_t	forward, right, up;
	vec3_t	aim;
	vec3_t	dir;
	vec3_t	end;
	float	r, u;
	int		flashIndex = BlasterFlash[FlashNumber];

	Angles_Vectors (Entity->s.angles, forward, right, NULL);
	G_ProjectSource (Entity->s.origin, dumb_and_hacky_monster_MuzzFlashOffset[flashIndex], forward, right, start);

	if (FlashNumber == 5 || FlashNumber == 6)
		Vec3Copy (forward, aim);
	else
	{
		Vec3Copy (Entity->enemy->s.origin, end);
		end[2] += Entity->enemy->viewheight;
		Vec3Subtract (end, start, aim);
		VecToAngles (aim, dir);
		Angles_Vectors (dir, forward, right, up);

		r = crandom()*1000;
		u = crandom()*500;
		Vec3MA (start, 8192, forward, end);
		Vec3MA (end, r, right, end);
		Vec3MA (end, u, up, end);

		Vec3Subtract (end, start, aim);
		VectorNormalizef (aim, aim);
	}

	MonsterFireBlaster (start, aim, 5, 600, flashIndex, EF_BLASTER);
}

void CSoldierLight::SpawnSoldier ()
{
	SoundPain = SoundIndex ("soldier/solpain2.wav");
	SoundDeath = SoundIndex ("soldier/soldeth2.wav");
	ModelIndex ("models/objects/laser/tris.md2");
	SoundIndex ("misc/lasfly.wav");
	SoundIndex ("soldier/solatck2.wav");

	Entity->s.skinNum = 0;
	Entity->health = 20;
	Entity->gib_health = -30;
}