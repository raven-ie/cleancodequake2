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
// cc_railgun.cpp
// Railgun
//

#include "cc_local.h"
#include "m_player.h"

CRailgun WeaponRailgun;

CRailgun::CRailgun() :
CWeapon("models/weapons/v_rail/tris.md2", 0, 3, 4, 14,
		19, 37, 57, 4)
{
}

bool CRailgun::CanFire (edict_t *ent)
{
	switch (ent->client->ps.gunFrame)
	{
	case 4:
		return true;
	}
	return false;
}

bool CRailgun::CanStopFidgetting (edict_t *ent)
{
	switch (ent->client->ps.gunFrame)
	{
	case 56:
		return true;
	}
	return false;
}

void CRailgun::Fire (edict_t *ent)
{
	vec3_t		start;
	vec3_t		forward, right;
	vec3_t		offset;
	int			damage;
	int			kick;

	if (deathmatch->Integer())
	{	// normal damage is too extreme in dm
		damage = 100;
		kick = 200;
	}
	else
	{
		damage = 150;
		kick = 250;
	}

	if (isQuad)
	{
		damage *= 4;
		kick *= 4;
	}

	Angles_Vectors (ent->client->v_angle, forward, right, NULL);

	Vec3Scale (forward, -3, ent->client->kick_origin);
	ent->client->kick_angles[0] = -3;

	Vec3Set (offset, 0, 7,  ent->viewheight-8);
	P_ProjectSource (ent->client, ent->s.origin, offset, forward, right, start);
	fire_rail (ent, start, forward, damage, kick);

	// send muzzle flash
	Muzzle (ent, MZ_RAILGUN);

	ent->client->ps.gunFrame++;
	PlayerNoise(ent, start, PNOISE_WEAPON);

	if (!dmFlags.dfInfiniteAmmo)
		DepleteAmmo(ent, 1);
}
