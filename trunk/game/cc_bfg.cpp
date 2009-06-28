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
// cc_bfg.cpp
// BFG10k
//

#include "cc_local.h"
#include "m_player.h"

CBFG WeaponBFG;

CBFG::CBFG() :
CWeapon("models/weapons/v_bfg/tris.md2", 0, 8, 9, 32,
		33, 55, 56, 58, "weapons/bfg_hum.wav")
{
}

bool CBFG::CanFire (CPlayerEntity *ent)
{
	switch (ent->Client.PlayerState.GetGunFrame())
	{
	case 9:
	case 17:
		return true;
	}
	return false;
}

bool CBFG::CanStopFidgetting (CPlayerEntity *ent)
{
	switch (ent->Client.PlayerState.GetGunFrame())
	{
	case 39:
	case 45:
	case 50:
	case 55:
		return true;
	}
	return false;
}

void CBFG::Fire (CPlayerEntity *ent)
{
	switch (ent->Client.PlayerState.GetGunFrame())
	{
	case 9:
		MuzzleEffect (ent);
		break;
	case 17:
		FireBFG (ent);
		break;
	}
}

void CBFG::MuzzleEffect (CPlayerEntity *ent)
{
	vec3_t origin;
	ent->State.GetOrigin (origin);
	// send muzzle flash
	Muzzle (ent, MZ_BFG);

	ent->Client.PlayerState.SetGunFrame (ent->Client.PlayerState.GetGunFrame()+1);
}

void CBFG::FireBFG (CPlayerEntity *ent)
{
	vec3_t	offset, start;
	vec3_t	forward, right;
	int		damage = (game.mode & GAME_DEATHMATCH) ? 200 : 500;
	float	damage_radius = 1000;

	// cells can go down during windup (from power armor hits), so
	// check again and abort firing if we don't have enough now
	if (ent->Client.pers.Inventory.Has(ent->Client.pers.Weapon->WeaponItem->Ammo) < 50)
	{
		ent->Client.PlayerState.SetGunFrame (ent->Client.PlayerState.GetGunFrame()+1);
		return;
	}

	FireAnimation (ent);

	if (isQuad)
		damage *= 4;

	Angles_Vectors (ent->Client.v_angle, forward, right, NULL);

	Vec3Scale (forward, -2, ent->Client.kick_origin);

	// make a big pitch kick with an inverse fall
	ent->Client.v_dmg_pitch = -40;
	ent->Client.v_dmg_roll = crandom()*8;
	ent->Client.v_dmg_time = level.framenum + DAMAGE_TIME;

	Vec3Set (offset, 8, 8, ent->gameEntity->viewheight-8);
	P_ProjectSource (ent, offset, forward, right, start);
	fire_bfg (ent->gameEntity, start, forward, damage, 400, damage_radius);
	AttackSound (ent);

	ent->Client.PlayerState.SetGunFrame (ent->Client.PlayerState.GetGunFrame()+1);

	PlayerNoise(ent, start, PNOISE_WEAPON);

	if (!dmFlags.dfInfiniteAmmo)
		DepleteAmmo (ent, 50);
}
