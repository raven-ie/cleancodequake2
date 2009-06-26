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

bool CHyperBlaster::CanFire (CPlayerEntity *ent)
{
	if (ent->Client.PlayerState.GetGunFrame() >= 6 && ent->Client.PlayerState.GetGunFrame() <= 11)
		return true;
	return false;
}

bool CHyperBlaster::CanStopFidgetting (CPlayerEntity *ent)
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

void CHyperBlaster::Fire (CPlayerEntity *ent)
{
	float	rotation;
	vec3_t	offset;
	int		effect;
	int		damage;

	ent->Client.weapon_sound = SoundIndex("weapons/hyprbl1a.wav");

	if (!(ent->Client.buttons & BUTTON_ATTACK))
		ent->Client.PlayerState.SetGunFrame(ent->Client.PlayerState.GetGunFrame() + 1);
	else
	{
		if (!ent->Client.pers.Inventory.Has(ent->Client.pers.Weapon->WeaponItem->Ammo) )
		{
			OutOfAmmo(ent);
			NoAmmoWeaponChange (ent);
		}
		else
		{
			// I replaced this part with a table because they are constant.
			rotation = hyperblasterRotationTable[ent->Client.PlayerState.GetGunFrame() - 6];
			Vec3Copy (hyperblasterOffsetTable[ent->Client.PlayerState.GetGunFrame() - 6], offset);

			if ((ent->Client.PlayerState.GetGunFrame() == 6) || (ent->Client.PlayerState.GetGunFrame() == 9))
				effect = EF_HYPERBLASTER;
			else
				effect = 0;

			damage = (game.mode & GAME_DEATHMATCH) ? 15 : 20;

			vec3_t	forward, right;
			vec3_t	start;
			vec3_t	noffset;

			if (isQuad)
				damage *= 4;

			Angles_Vectors (ent->Client.v_angle, forward, right, NULL);
			Vec3Set (noffset, 24, 8, ent->gameEntity->viewheight-8);
			Vec3Add (noffset, offset, noffset);
			P_ProjectSource (ent, noffset, forward, right, start);

			Vec3Scale (forward, -2, ent->Client.kick_origin);
			ent->Client.kick_angles[0] = -1;

			//fire_blaster (ent->gameEntity, start, forward, damage, 1000, effect, true);
			CBlasterProjectile::Spawn (ent, start, forward, damage, 1000, effect, true);

			// send muzzle flash
			Muzzle (ent, MZ_HYPERBLASTER);
			AttackSound (ent);

			PlayerNoise(ent, start, PNOISE_WEAPON);

			if (!dmFlags.dfInfiniteAmmo)
				DepleteAmmo (ent, 1);

			FireAnimation (ent);
		}

		ent->Client.PlayerState.SetGunFrame (ent->Client.PlayerState.GetGunFrame() + 1);
		if (ent->Client.PlayerState.GetGunFrame() == 12 && ent->Client.pers.Inventory.Has(ent->Client.pers.Weapon->WeaponItem->Ammo))
			ent->Client.PlayerState.SetGunFrame (6);
	}

	if (ent->Client.PlayerState.GetGunFrame() == 12)
	{
		PlaySoundFrom(ent->gameEntity, CHAN_AUTO, SoundIndex("weapons/hyprbd1a.wav"));
		ent->Client.weapon_sound = 0;
	}
}
