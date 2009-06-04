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

bool CMachinegun::CanFire (edict_t *ent)
{
	switch (ent->client->playerState.gunFrame)
	{
	case 4:
	case 5:
		return true;
	}
	return false;
}

bool CMachinegun::CanStopFidgetting (edict_t *ent)
{
	switch (ent->client->playerState.gunFrame)
	{
	case 23:
	case 45:
		return true;
	}
	return false;
}

void CMachinegun::FireAnimation (edict_t *ent)
{
	ent->client->anim_priority = ANIM_ATTACK;
	if (ent->client->playerState.pMove.pmFlags & PMF_DUCKED)
	{
		ent->state.frame = FRAME_crattak1 - (int) (random()+0.25);
		ent->client->anim_end = FRAME_crattak9;
	}
	else
	{
		ent->state.frame = FRAME_attack1 - (int) (random()+0.25);
		ent->client->anim_end = FRAME_attack8;
	}
}

void CMachinegun::Fire (edict_t *ent)
{
	int	i;
	vec3_t		start;
	vec3_t		forward, right;
	vec3_t		angles;
	int			damage = 8;
	int			kick = 2;
	vec3_t		offset;

	if (!(ent->client->buttons & BUTTON_ATTACK))
	{
		ent->client->machinegun_shots = 0;
		ent->client->playerState.gunFrame++;
		return;
	}

	if (ent->client->playerState.gunFrame == 5)
		ent->client->playerState.gunFrame = 4;
	else
		ent->client->playerState.gunFrame = 5;

	if (!AttemptToFire(ent))
	{
		ent->client->playerState.gunFrame = 6;
		OutOfAmmo(ent);
		NoAmmoWeaponChange (ent);
		return;
	}

	if (isQuad)
	{
		damage *= 4;
		kick *= 4;
		PlaySoundFrom(ent, CHAN_ITEM, SoundIndex("items/damage3.wav"));
	}

	for (i=1 ; i<3 ; i++)
	{
		ent->client->kick_origin[i] = crandom() * 0.35;
		ent->client->kick_angles[i] = crandom() * 0.7;
	}
	ent->client->kick_origin[0] = crandom() * 0.35;
	ent->client->kick_angles[0] = ent->client->machinegun_shots * -1.5;

	// raise the gun as it is firing
	if (!deathmatch->Integer())
	{
		ent->client->machinegun_shots++;
		if (ent->client->machinegun_shots > 9)
			ent->client->machinegun_shots = 9;
	}

	// get start / end positions
	Vec3Add (ent->client->v_angle, ent->client->kick_angles, angles);
	Angles_Vectors (angles, forward, right, NULL);
	Vec3Set (offset, 0, 8, ent->viewheight-8);
	P_ProjectSource (ent->client, ent->state.origin, offset, forward, right, start);
	fire_bullet (ent, start, forward, damage, kick, DEFAULT_BULLET_HSPREAD, DEFAULT_BULLET_VSPREAD, MOD_MACHINEGUN);

	Muzzle (ent, MZ_MACHINEGUN);

	PlayerNoise(ent, start, PNOISE_WEAPON);

	if (!dmFlags.dfInfiniteAmmo)
		DepleteAmmo(ent, 1);

	FireAnimation (ent);
}
