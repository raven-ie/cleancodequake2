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
// cc_shotgun.cpp
// Shotgun
//

#include "cc_local.h"
#include "m_player.h"

CShotgun WeaponShotgun;

CShotgun::CShotgun() :
CWeapon("models/weapons/v_shotg/tris.md2", 0, 7, 8, 11,
		20, 16, 36, 3)
{
}

bool CShotgun::CanFire (edict_t *ent)
{
	switch (ent->client->ps.gunFrame)
	{
	case 8:
		return true;
	}
	return false;
}

bool CShotgun::CanStopFidgetting (edict_t *ent)
{
	switch (ent->client->ps.gunFrame)
	{
	case 22:
	case 28:
	case 34:
		return true;
	}
	return false;
}

void CShotgun::Fire (edict_t *ent)
{
	vec3_t		start;
	vec3_t		forward, right;
	vec3_t		offset;
	int			damage = 4;
	int			kick = 8;

	Angles_Vectors (ent->client->v_angle, forward, right, NULL);

	Vec3Scale (forward, -2, ent->client->kick_origin);
	ent->client->kick_angles[0] = -2;

	Vec3Set (offset, 0, 8,  ent->viewheight-8);
	P_ProjectSource (ent->client, ent->s.origin, offset, forward, right, start);

	if (isQuad)
	{
		damage *= 4;
		kick *= 4;
		PlaySoundFrom(ent, CHAN_ITEM, SoundIndex("items/damage3.wav"));
	}

	if (deathmatch->Integer())
		fire_shotgun (ent, start, forward, damage, kick, 500, 500, DEFAULT_DEATHMATCH_SHOTGUN_COUNT, MOD_SHOTGUN);
	else
		fire_shotgun (ent, start, forward, damage, kick, 500, 500, DEFAULT_SHOTGUN_COUNT, MOD_SHOTGUN);

	// send muzzle flash
	Muzzle (ent, MZ_SHOTGUN);

	ent->client->ps.gunFrame++;
	PlayerNoise(ent, start, PNOISE_WEAPON);
	FireAnimation (ent);

	if (!dmFlags.dfInfiniteAmmo)
		DepleteAmmo(ent, 1);
}
