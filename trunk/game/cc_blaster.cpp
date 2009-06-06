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
// cc_blaster.cpp
// Blaster!
//

#include "cc_local.h"
#include "m_player.h"

CBlaster WeaponBlaster;

CBlaster::CBlaster() :
CWeapon("models/weapons/v_blast/tris.md2", 0, 4, 5, 8,
		9, 52, 53, 55)
{
}

bool CBlaster::CanFire (edict_t *ent)
{
	switch (ent->client->playerState.gunFrame)
	{
	case 5:
		return true;
	}
	return false;
}

bool CBlaster::CanStopFidgetting (edict_t *ent)
{
	switch (ent->client->playerState.gunFrame)
	{
	case 19:
	case 32:
		return true;
	}
	return false;
}

bool CBlaster::AttemptToFire (edict_t *ent)
{
	return true;
}

void CBlaster::Fire (edict_t *ent)
{
	int damage = deathmatch->Integer() ? 15 : 10;
	vec3_t	forward, right;
	vec3_t	start;
	vec3_t	offset;

	if (isQuad)
		damage *= 4;

	Angles_Vectors (ent->client->v_angle, forward, right, NULL);
	Vec3Set (offset, 24, 8, ent->viewheight-8);
	P_ProjectSource (ent->client, ent->state.origin, offset, forward, right, start);

	Vec3Scale (forward, -2, ent->client->kick_origin);
	ent->client->kick_angles[0] = -1;

	fire_blaster (ent, start, forward, damage, 1000, EF_BLASTER, false);

	// send muzzle flash
	Muzzle (ent, MZ_BLASTER);
	AttackSound (ent);

	PlayerNoise(ent, start, PNOISE_WEAPON);
	FireAnimation(ent);

	ent->client->playerState.gunFrame++;
}
