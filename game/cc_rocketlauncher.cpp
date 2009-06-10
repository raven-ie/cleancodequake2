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
// cc_grenadelauncher.cpp
// Grenade Launcher
//

#include "cc_local.h"
#include "m_player.h"

CRocketLauncher WeaponRocketLauncher;

CRocketLauncher::CRocketLauncher() :
CWeapon("models/weapons/v_rocket/tris.md2", 0, 4, 5, 12,
		13, 50, 51, 54)
{
}

bool CRocketLauncher::CanFire (CPlayerEntity *ent)
{
	switch (ent->Client.PlayerState.GetGunFrame())
	{
	case 5:
		return true;
	}
	return false;
}

bool CRocketLauncher::CanStopFidgetting (CPlayerEntity *ent)
{
	switch (ent->Client.PlayerState.GetGunFrame())
	{
	case 25:
	case 33:
	case 42:
	case 50:
		return true;
	}
	return false;
}

void CRocketLauncher::Fire (CPlayerEntity *ent)
{
	vec3_t	offset, start;
	vec3_t	forward, right;
	int		damage;
	float	damage_radius;
	int		radius_damage;

	damage = 100 + (int)(random() * 20.0);
	radius_damage = 120;
	damage_radius = 120;
	if (isQuad)
	{
		damage *= 4;
		radius_damage *= 4;
	}

	Angles_Vectors (ent->Client.v_angle, forward, right, NULL);

	Vec3Scale (forward, -2, ent->Client.kick_origin);
	ent->Client.kick_angles[0] = -1;

	Vec3Set (offset, 8, 8, ent->gameEntity->viewheight-8);
	P_ProjectSource (ent, ent->gameEntity->state.origin, offset, forward, right, start);
	fire_rocket (ent->gameEntity, start, forward, damage, 650, damage_radius, radius_damage);

	// send muzzle flash
	Muzzle (ent, MZ_ROCKET);
	AttackSound (ent);

	PlayerNoise(ent, start, PNOISE_WEAPON);
	FireAnimation (ent);

	if (!dmFlags.dfInfiniteAmmo)
		DepleteAmmo(ent, 1);

	ent->Client.PlayerState.SetGunFrame(ent->Client.PlayerState.GetGunFrame()+1);
}
