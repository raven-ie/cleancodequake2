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
// cc_handgrenade.cpp
// Handgrenades
//

#include "cc_local.h"
#include "m_player.h"

CHandGrenade WeaponGrenades;

CHandGrenade::CHandGrenade() :
CWeapon("models/weapons/v_handgr/tris.md2", 0, 0, 0, 16,
		17, 31, 0, 0)
{
}

bool CHandGrenade::CanStopFidgetting (edict_t *ent)
{
	switch (ent->client->ps.gunFrame)
	{
	case 29:
	case 34:
	case 39:
	case 48:
		return true;
	}
	return false;
}

#define GRENADE_TIMER		3.0
#define GRENADE_MINSPEED	400
#define GRENADE_MAXSPEED	800

void CHandGrenade::Hold (edict_t *ent)
{
	if (!ent->client->grenade_time)
	{
		ent->client->grenade_time = level.time + GRENADE_TIMER + 0.2;
		ent->client->weapon_sound = SoundIndex("weapons/hgrenc1b.wav");
	}

	// they waited too long, detonate it in their hand
	if (!ent->client->grenade_blew_up && level.time >= ent->client->grenade_time)
	{
		ent->client->weapon_sound = 0;
		FireGrenade (ent, true);
		ent->client->grenade_blew_up = true;

		ent->client->ps.gunFrame = 15;
		return;
	}

	if (ent->client->buttons & BUTTON_ATTACK)
		return;

	ent->client->ps.gunFrame++;
}

void CHandGrenade::FireGrenade (edict_t *ent, bool inHand)
{
	vec3_t	offset;
	vec3_t	forward, right;
	vec3_t	start;
	int		damage = 125;

	float radius = damage+40;
	if (isQuad)
	{
		damage *= 4;
		PlaySoundFrom(ent, CHAN_ITEM, SoundIndex("items/damage3.wav")); // Make sure people know. REOOOOOOOO
	}

	ent->client->grenade_thrown = true;

	Vec3Set (offset, 8, 8, ent->viewheight-8);
	Angles_Vectors (ent->client->v_angle, forward, right, NULL);
	P_ProjectSource (ent->client, ent->s.origin, offset, forward, right, start);

	float timer = ent->client->grenade_time - level.time;
	int speed = GRENADE_MINSPEED + (GRENADE_TIMER - timer) * ((GRENADE_MAXSPEED - GRENADE_MINSPEED) / GRENADE_TIMER);
	fire_grenade2 (ent, start, forward, damage, speed, timer, radius, inHand);

	ent->client->grenade_time = level.time + 1.0;
	if (!dmFlags.dfInfiniteAmmo)
		DepleteAmmo(ent, 1);

	if(ent->health <= 0 || ent->deadflag || ent->s.modelIndex != 255) // VWep animations screw up corpses
		return;

	if (ent->client->ps.pMove.pmFlags & PMF_DUCKED)
	{
		ent->client->anim_priority = ANIM_ATTACK;
		ent->s.frame = FRAME_crattak1-1;
		ent->client->anim_end = FRAME_crattak3;
	}
	else
	{
		ent->client->anim_priority = ANIM_REVERSE;
		ent->s.frame = FRAME_wave08;
		ent->client->anim_end = FRAME_wave01;
	}
	ent->client->ps.gunFrame++;
}

void CHandGrenade::Wait (edict_t *ent)
{
	ent->client->grenade_blew_up = false;
	if (level.time < ent->client->grenade_time)
		return;
	ent->client->ps.gunFrame++;
}

void CHandGrenade::Fire (edict_t *ent)
{
	switch (ent->client->ps.gunFrame)
	{
	case 11:
		Hold (ent);
		break;
	case 12:
		ent->client->weapon_sound = 0;
		FireGrenade(ent, false);
		break;
	case 15:
		Wait(ent);
		break;
	}
}

bool CHandGrenade::CanFire (edict_t *ent)
{
	switch (ent->client->ps.gunFrame)
	{
	case 5:
		PlaySoundFrom(ent, CHAN_WEAPON, SoundIndex("weapons/hgrena1b.wav"));
		return false;
	case 11:
	case 12:
	case 15:
		return true;
	}
	return false;
}

void CHandGrenade::WeaponGeneric (edict_t *ent)
{
	// Idea from Brazen source
	int newFrame = -1, newState = -1;

	switch (ent->client->weaponstate)
	{
	case WS_ACTIVATING:
		newFrame = IdleStart;
		newState = WS_IDLE;
		break;
	case WS_IDLE:
		if (ent->client->NewWeapon && ent->client->NewWeapon != this)
		{
			// We want to go away!
			newState = WS_DEACTIVATING;
			newFrame = DeactStart;
		}
		else if ((ent->client->buttons|ent->client->latched_buttons) & BUTTON_ATTACK)
		{
			ent->client->latched_buttons &= ~BUTTON_ATTACK;

			// We want to attack!
			// First call, check AttemptToFire
			if (AttemptToFire(ent))
			{
				// Got here, we can fire!
				ent->client->ps.gunFrame = FireStart;
				ent->client->weaponstate = WS_FIRING;
				ent->client->grenade_time = 0;

				// We need to check against us right away for first-frame firing
				WeaponGeneric(ent);
				return;
			}
			else
			{
				OutOfAmmo(ent);
				NoAmmoWeaponChange (ent);
			}
		}

		// Either we are still idle or a failed fire.
		if (newState == -1)
		{
			if (ent->client->ps.gunFrame == IdleEnd)
				newFrame = IdleStart;
			else
			{
				if (CanStopFidgetting(ent) && (rand()&15))
					newFrame = ent->client->ps.gunFrame;
			}
		}
		break;
	case WS_FIRING:
		// Check if this is a firing frame.
		if (CanFire(ent))
		{
			Fire(ent);

			// Now, this call above CAN change the underlying frame and state.
			// We need this block to make sure we are still doing what we are supposed to.
			newState = ent->client->weaponstate;
			newFrame = ent->client->ps.gunFrame;
		}

		// Only do this if we haven't been explicitely set a newFrame
		// because we might want to keep firing beyond this point
		if (newFrame == -1 && ent->client->ps.gunFrame == FireEnd)
		{
			if (!ent->client->pers.Inventory.Has(this->Item))
			{
				NoAmmoWeaponChange (ent);
				newState = WS_DEACTIVATING;
				newFrame = DeactStart;
				ent->client->grenade_time = 0;
				ent->client->grenade_thrown = false;
			}
			else
			{
				ent->client->grenade_time = 0;
				ent->client->grenade_thrown = false;
				newFrame = IdleStart;
				newState = WS_IDLE;
			}
		}
		break;
	case WS_DEACTIVATING:
		// Change weapon
		this->ChangeWeapon (ent);
		return;
		break;
	}

	if (newFrame != -1)
		ent->client->ps.gunFrame = newFrame;
	if (newState != -1)
		ent->client->weaponstate = newState;

	if (newFrame == -1 && newState == -1)
		ent->client->ps.gunFrame++;
}