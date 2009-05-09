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

CGrenadeLauncher WeaponGrenadeLauncher;

CGrenadeLauncher::CGrenadeLauncher() :
CWeapon("models/weapons/v_launch/tris.md2", 0, 5, 6, 10,
		17, 42, 60, 4)
{
}

bool CGrenadeLauncher::CanFire (edict_t *ent)
{
	switch (ent->client->ps.gunFrame)
	{
	case 6:
		return true;
	}
	return false;
}

bool CGrenadeLauncher::CanStopFidgetting (edict_t *ent)
{
	switch (ent->client->ps.gunFrame)
	{
	case 34:
	case 51:
	case 59:
		return true;
	}
	return false;
}

void CGrenadeLauncher::Fire (edict_t *ent)
{
	vec3_t	offset;
	vec3_t	forward, right;
	vec3_t	start;
	int		damage = 120;
	float	radius;

	radius = damage+40;
	if (isQuad)
	{
		Sound(ent, CHAN_ITEM, SoundIndex("items/damage3.wav"));
		damage *= 4;
	}

	FireAnimation (ent);

	Vec3Set (offset, 8, 8, ent->viewheight-8);
	Angles_Vectors (ent->client->v_angle, forward, right, NULL);
	P_ProjectSource (ent->client, ent->s.origin, offset, forward, right, start);

	Vec3Scale (forward, -2, ent->client->kick_origin);
	ent->client->kick_angles[0] = -1;

	fire_grenade (ent, start, forward, damage, 600, 2.5, radius);

	Muzzle (ent, MZ_GRENADE);

	PlayerNoise(ent, start, PNOISE_WEAPON);

	if (!dmFlags.dfInfiniteAmmo)
		DepleteAmmo(ent, 1);

	ent->client->ps.gunFrame++;
}
