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
CWeapon("models/weapons/v_rocket/tris.md2", 0, 4, 5, 8,
		14, 36, 51, 3)
{
}

bool CRocketLauncher::CanFire (edict_t *ent)
{
	switch (ent->client->ps.gunFrame)
	{
	case 5:
		return true;
	}
	return false;
}

bool CRocketLauncher::CanStopFidgetting (edict_t *ent)
{
	switch (ent->client->ps.gunFrame)
	{
	case 25:
	case 33:
	case 42:
	case 50:
		return true;
	}
	return false;
}

void CRocketLauncher::Fire (edict_t *ent)
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
		PlaySoundFrom(ent, CHAN_ITEM, SoundIndex("items/damage3.wav"));
	}

	Angles_Vectors (ent->client->v_angle, forward, right, NULL);

	Vec3Scale (forward, -2, ent->client->kick_origin);
	ent->client->kick_angles[0] = -1;

	Vec3Set (offset, 8, 8, ent->viewheight-8);
	P_ProjectSource (ent->client, ent->s.origin, offset, forward, right, start);
	fire_rocket (ent, start, forward, damage, 650, damage_radius, radius_damage);

	// send muzzle flash
	Muzzle (ent, MZ_ROCKET);

	PlayerNoise(ent, start, PNOISE_WEAPON);
	FireAnimation (ent);

	if (!dmFlags.dfInfiniteAmmo)
		DepleteAmmo(ent, 1);

	ent->client->ps.gunFrame++;
}
