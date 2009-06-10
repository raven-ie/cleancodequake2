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
// cc_machinegun.cpp
// Machinegun
//

#include "cc_local.h"
#include "m_player.h"

CMachinegun WeaponMachinegun;

CMachinegun::CMachinegun() :
CWeapon("models/weapons/v_machn/tris.md2", 0, 3, 4, 5,
		6, 45, 46, 49)
{
}

bool CMachinegun::CanFire (CPlayerEntity *ent)
{
	switch (ent->Client.PlayerState.GetGunFrame())
	{
	case 4:
	case 5:
		return true;
	}
	return false;
}

bool CMachinegun::CanStopFidgetting (CPlayerEntity *ent)
{
	switch (ent->Client.PlayerState.GetGunFrame())
	{
	case 23:
	case 45:
		return true;
	}
	return false;
}

void CMachinegun::FireAnimation (CPlayerEntity *ent)
{
	ent->Client.anim_priority = ANIM_ATTACK;
	if (ent->Client.PlayerState.GetPMove()->pmFlags & PMF_DUCKED)
	{
		ent->gameEntity->state.frame = FRAME_crattak1 - (int) (random()+0.25);
		ent->Client.anim_end = FRAME_crattak9;
	}
	else
	{
		ent->gameEntity->state.frame = FRAME_attack1 - (int) (random()+0.25);
		ent->Client.anim_end = FRAME_attack8;
	}
}

void CMachinegun::Fire (CPlayerEntity *ent)
{
	int	i;
	vec3_t		start;
	vec3_t		forward, right;
	vec3_t		angles;
	int			damage = 8;
	int			kick = 2;
	vec3_t		offset;

	if (!(ent->Client.buttons & BUTTON_ATTACK))
	{
		ent->Client.machinegun_shots = 0;
		ent->Client.PlayerState.SetGunFrame(ent->Client.PlayerState.GetGunFrame() + 1);
		return;
	}

	if (ent->Client.PlayerState.GetGunFrame() == 5)
		ent->Client.PlayerState.SetGunFrame(4);
	else
		ent->Client.PlayerState.SetGunFrame(5);

	if (!AttemptToFire(ent))
	{
		ent->Client.PlayerState.SetGunFrame (6);
		OutOfAmmo(ent);
		NoAmmoWeaponChange (ent);
		return;
	}

	if (isQuad)
	{
		damage *= 4;
		kick *= 4;
	}

	for (i=1 ; i<3 ; i++)
	{
		ent->Client.kick_origin[i] = crandom() * 0.35;
		ent->Client.kick_angles[i] = crandom() * 0.7;
	}
	ent->Client.kick_origin[0] = crandom() * 0.35;
	ent->Client.kick_angles[0] = ent->Client.machinegun_shots * -1.5;

	// raise the gun as it is firing
	if (!(game.mode & GAME_DEATHMATCH))
	{
		ent->Client.machinegun_shots++;
		if (ent->Client.machinegun_shots > 9)
			ent->Client.machinegun_shots = 9;
	}

	// get start / end positions
	Vec3Add (ent->Client.v_angle, ent->Client.kick_angles, angles);
	Angles_Vectors (angles, forward, right, NULL);
	Vec3Set (offset, 0, 8, ent->gameEntity->viewheight-8);
	P_ProjectSource (ent, ent->gameEntity->state.origin, offset, forward, right, start);
	fire_bullet (ent->gameEntity, start, forward, damage, kick, DEFAULT_BULLET_HSPREAD, DEFAULT_BULLET_VSPREAD, MOD_MACHINEGUN);

	Muzzle (ent, MZ_MACHINEGUN);
	AttackSound (ent);

	PlayerNoise(ent, start, PNOISE_WEAPON);

	if (!dmFlags.dfInfiniteAmmo)
		DepleteAmmo(ent, 1);

	FireAnimation (ent);
}
