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
// cc_hyperblaster.cpp
// Hyperblaster
//

#include "cc_local.h"
#include "m_player.h"

CHyperBlaster WeaponHyperBlaster;

CHyperBlaster::CHyperBlaster() :
CWeapon("models/weapons/v_hyperb/tris.md2", 0, 5, 6, 20,
		21, 49, 50, 53)
{
}

bool CHyperBlaster::CanFire (edict_t *ent)
{
	if (ent->client->playerState.gunFrame >= 6 && ent->client->playerState.gunFrame <= 11)
		return true;
	return false;
}

bool CHyperBlaster::CanStopFidgetting (edict_t *ent)
{
	return false;
}

float hyperblasterRotationTable[] =
{
	1.04f,
	2.09f,
	3.14f,
	4.18f,
	5.23f,
	6.28f
};

vec3_t hyperblasterOffsetTable[] =
{
	{ -3.46f, 0, 2.0f	},
	{ -3.46f, 0, -2.0f	},
	{ 0, 0, -4.0f		},
	{ 3.46f, 0, -2.0f	},
	{ 3.46f, 0, 2.0f	},
	{ 0, 0, 4.0f		}
};

void CHyperBlaster::Fire (edict_t *ent)
{
	float	rotation;
	vec3_t	offset;
	int		effect;
	int		damage;

	ent->client->weapon_sound = SoundIndex("weapons/hyprbl1a.wav");

	if (!(ent->client->buttons & BUTTON_ATTACK))
		ent->client->playerState.gunFrame++;
	else
	{
		if (! ent->client->pers.Inventory.Has(ent->client->pers.Weapon->WeaponItem->Ammo) )
		{
			OutOfAmmo(ent);
			NoAmmoWeaponChange (ent);
		}
		else
		{
			// I replaced these with a table because they are constant.
			// It always bugged me that they did operations like this.
			/*rotation = (ent->client->playerState.gunFrame - 5) * 2*M_PI/6;
			offset[0] = -4 * sinf(rotation);
			offset[1] = 0;
			offset[2] = 4 * cosf(rotation);*/
			rotation = hyperblasterRotationTable[ent->client->playerState.gunFrame - 6];
			Vec3Copy (hyperblasterOffsetTable[ent->client->playerState.gunFrame - 6], offset);

			if ((ent->client->playerState.gunFrame == 6) || (ent->client->playerState.gunFrame == 9))
				effect = EF_HYPERBLASTER;
			else
				effect = 0;
			if (deathmatch->Integer())
				damage = 15;
			else
				damage = 20;

			vec3_t	forward, right;
			vec3_t	start;
			vec3_t	noffset;

			if (isQuad)
			{
				damage *= 4;
				PlaySoundFrom(ent, CHAN_ITEM, SoundIndex("items/damage3.wav"));
			}

			Angles_Vectors (ent->client->v_angle, forward, right, NULL);
			Vec3Set (noffset, 24, 8, ent->viewheight-8);
			Vec3Add (noffset, offset, noffset);
			P_ProjectSource (ent->client, ent->state.origin, noffset, forward, right, start);

			Vec3Scale (forward, -2, ent->client->kick_origin);
			ent->client->kick_angles[0] = -1;

			fire_blaster (ent, start, forward, damage, 1000, effect, true);

			// send muzzle flash
			Muzzle (ent, MZ_HYPERBLASTER);

			PlayerNoise(ent, start, PNOISE_WEAPON);

			if (!dmFlags.dfInfiniteAmmo)
				DepleteAmmo (ent, 1);

			FireAnimation (ent);
		}

		ent->client->playerState.gunFrame++;
		if (ent->client->playerState.gunFrame == 12 && ent->client->pers.Inventory.Has(ent->client->pers.Weapon->WeaponItem->Ammo))
			ent->client->playerState.gunFrame = 6;
	}

	if (ent->client->playerState.gunFrame == 12)
	{
		PlaySoundFrom(ent, CHAN_AUTO, SoundIndex("weapons/hyprbd1a.wav"));
		ent->client->weapon_sound = 0;
	}
}
