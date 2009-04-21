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
// cc_chaingun.cpp
// Chaingun
//

#include "cc_local.h"
#include "m_player.h"

CChaingun WeaponChaingun;

CChaingun::CChaingun() :
CWeapon("models/weapons/v_chain/tris.md2", 0, 4, 5, 26,
		32, 29, 62, 2)
{
}

bool CChaingun::CanFire (edict_t *ent)
{
	if (ent->client->ps.gunFrame >= 5 && ent->client->ps.gunFrame <= 21)
		return true;
	return false;
}

bool CChaingun::CanStopFidgetting (edict_t *ent)
{
	switch (ent->client->ps.gunFrame)
	{
	case 38:
	case 43:
	case 51:
	case 61:
		return true;
	}
	return false;
}

void CChaingun::Fire (edict_t *ent)
{
	int			i;
	int			shots;
	vec3_t		start;
	vec3_t		forward, right, up;
	float		r, u;
	vec3_t		offset;
	int			damage;
	int			kick = 2;

	if (deathmatch->Integer())
		damage = 6;
	else
		damage = 8;

	if (ent->client->ps.gunFrame == 5)
		Sound(ent, CHAN_AUTO, gi.soundindex("weapons/chngnu1a.wav"));

	if ((ent->client->ps.gunFrame == 14) && !(ent->client->buttons & BUTTON_ATTACK))
	{
		ent->client->ps.gunFrame = 32;
		ent->client->weapon_sound = 0;
		return;
	}
	else if ((ent->client->ps.gunFrame == 21) && (ent->client->buttons & BUTTON_ATTACK)
		&& ent->client->pers.Inventory.Has(ent->client->pers.Weapon->WeaponItem->Ammo))
	{
		ent->client->ps.gunFrame = 15;
	}
	else
	{
		ent->client->ps.gunFrame++;
	}

	if (ent->client->ps.gunFrame == 22)
	{
		ent->client->weapon_sound = 0;
		Sound(ent, CHAN_AUTO, gi.soundindex("weapons/chngnd1a.wav"));
	}
	else
	{
		ent->client->weapon_sound = gi.soundindex("weapons/chngnl1a.wav");
	}

	ent->client->anim_priority = ANIM_ATTACK;
	if (ent->client->ps.pMove.pmFlags & PMF_DUCKED)
	{
		ent->s.frame = FRAME_crattak1 - (ent->client->ps.gunFrame & 1);
		ent->client->anim_end = FRAME_crattak9;
	}
	else
	{
		ent->s.frame = FRAME_attack1 - (ent->client->ps.gunFrame & 1);
		ent->client->anim_end = FRAME_attack8;
	}

	if (ent->client->ps.gunFrame <= 9)
		shots = 1;
	else if (ent->client->ps.gunFrame <= 14)
	{
		if (ent->client->buttons & BUTTON_ATTACK)
			shots = 2;
		else
			shots = 1;
	}
	else
		shots = 3;

	if (ent->client->pers.Inventory.Has(ent->client->pers.Weapon->WeaponItem->Ammo) < shots)
		shots = ent->client->pers.Inventory.Has(ent->client->pers.Weapon->WeaponItem->Ammo);

	if (!shots)
	{
		OutOfAmmo(ent);
		//NoAmmoWeaponChange (ent);
		return;
	}

	/*if (is_quad)
	{
		damage *= 4;
		kick *= 4;
	}*/

	for (i=0 ; i<3 ; i++)
	{
		ent->client->kick_origin[i] = crandom() * 0.35;
		ent->client->kick_angles[i] = crandom() * 0.7;
	}

	for (i=0 ; i<shots ; i++)
	{
		// get start / end positions
		Angles_Vectors (ent->client->v_angle, forward, right, up);
		r = 7 + crandom()*4;
		u = crandom()*4;
		Vec3Set (offset, 0, r, u + ent->viewheight-8);
		P_ProjectSource (ent->client, ent->s.origin, offset, forward, right, start);

		fire_bullet (ent, start, forward, damage, kick, DEFAULT_BULLET_HSPREAD, DEFAULT_BULLET_VSPREAD, MOD_CHAINGUN);
	}

	// send muzzle flash
	gi.WriteByte (SVC_MUZZLEFLASH);
	gi.WriteShort (ent-g_edicts);
	gi.WriteByte ((MZ_CHAINGUN1 + shots - 1)/* | is_silenced*/);
	gi.multicast (ent->s.origin, MULTICAST_PVS);

	PlayerNoise(ent, start, PNOISE_WEAPON);

	if (!dmFlags.dfInfiniteAmmo)
		DepleteAmmo (ent, shots);
}
