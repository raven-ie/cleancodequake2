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
		17, 48, 0, 0)
{
}

bool CHandGrenade::CanStopFidgetting (CPlayerEntity *ent)
{
	switch (ent->Client.PlayerState.GetGunFrame())
	{
	case 29:
	case 34:
	case 39:
	case 48:
		return true;
	}
	return false;
}

#define GRENADE_TIMER		30
#define GRENADE_MINSPEED	400
#define GRENADE_MAXSPEED	800

void CHandGrenade::Hold (CPlayerEntity *ent)
{
	if (!ent->Client.grenade_time)
	{
		ent->Client.grenade_time = level.framenum + GRENADE_TIMER + 2;
		ent->Client.weapon_sound = SoundIndex("weapons/hgrenc1b.wav");
	}

	// they waited too long, detonate it in their hand
	if (!ent->Client.grenade_blew_up && (level.framenum >= ent->Client.grenade_time))
	{
		ent->Client.weapon_sound = 0;
		FireGrenade (ent, true);
		ent->Client.grenade_blew_up = true;

		ent->Client.PlayerState.SetGunFrame(15);
		return;
	}

	if (ent->Client.buttons & BUTTON_ATTACK)
		return;

	ent->Client.PlayerState.SetGunFrame(ent->Client.PlayerState.GetGunFrame()+1);
}

void CHandGrenade::FireGrenade (CPlayerEntity *ent, bool inHand)
{
	vec3_t	offset;
	vec3_t	forward, right;
	vec3_t	start;
	int		damage = 125;

	float radius = damage+40;
	if (isQuad)
		damage *= 4;

	ent->Client.grenade_thrown = true;

	Vec3Set (offset, 8, 8, ent->gameEntity->viewheight-8);
	Angles_Vectors (ent->Client.v_angle, forward, right, NULL);
	P_ProjectSource (ent, offset, forward, right, start);

	float timer = (float)(ent->Client.grenade_time - level.framenum) / 10;
	int speed = (GRENADE_MINSPEED + ((GRENADE_TIMER/10) - timer) * ((GRENADE_MAXSPEED - GRENADE_MINSPEED) / (GRENADE_TIMER/10)));
	//fire_grenade2 (ent->gameEntity, start, forward, damage, speed, timer, radius, inHand);
	CGrenade::Spawn (ent, start, forward, damage, speed, timer, radius, true, inHand);

#ifdef CLEANCTF_ENABLED
	ent->Client.grenade_time = level.framenum + (((game.mode & GAME_CTF) && ent->CTFApplyHaste()) ? 5 : 10);
#else
	ent->Client.grenade_time = level.framenum + 10;
#endif
	if (!dmFlags.dfInfiniteAmmo)
		DepleteAmmo(ent, 1);

	if (ent->gameEntity->health <= 0 || ent->gameEntity->deadflag || ent->State.GetModelIndex() != 255) // VWep animations screw up corpses
		return;

	AttackSound (ent);

	if (ent->Client.PlayerState.GetPMove()->pmFlags & PMF_DUCKED)
	{
		ent->Client.anim_priority = ANIM_ATTACK;
		ent->State.SetFrame (FRAME_crattak1-1);
		ent->Client.anim_end = FRAME_crattak3;
	}
	else
	{
		ent->Client.anim_priority = ANIM_REVERSE;
		ent->State.SetFrame (FRAME_wave08);
		ent->Client.anim_end = FRAME_wave01;
	}
	ent->Client.PlayerState.SetGunFrame (ent->Client.PlayerState.GetGunFrame()+1);
}

void CHandGrenade::Wait (CPlayerEntity *ent)
{
	ent->Client.grenade_blew_up = false;
	// if we aren't dead...
	if (!ent->gameEntity->deadflag)
		ent->Client.grenade_thrown = false;
	if (level.framenum < ent->Client.grenade_time)
		return;
	ent->Client.PlayerState.SetGunFrame (ent->Client.PlayerState.GetGunFrame()+1);
}

void CHandGrenade::Fire (CPlayerEntity *ent)
{
	switch (ent->Client.PlayerState.GetGunFrame())
	{
	case 11:
		Hold (ent);
		break;
	case 12:
		ent->Client.weapon_sound = 0;
		FireGrenade(ent, false);
		break;
	case 15:
		Wait(ent);
		break;
	}
}

bool CHandGrenade::CanFire (CPlayerEntity *ent)
{
	switch (ent->Client.PlayerState.GetGunFrame())
	{
	case 5:
		PlaySoundFrom(ent->gameEntity, CHAN_WEAPON, SoundIndex("weapons/hgrena1b.wav"));
		return false;
	case 11:
	case 12:
	case 15:
		return true;
	}
	return false;
}

void CHandGrenade::WeaponGeneric (CPlayerEntity *ent)
{
	// Idea from Brazen source
	int newFrame = -1, newState = -1;

	switch (ent->Client.weaponstate)
	{
	case WS_ACTIVATING:
		newFrame = IdleStart;
		newState = WS_IDLE;
		break;
	case WS_IDLE:
		if (ent->Client.NewWeapon && ent->Client.NewWeapon != this)
		{
			// We want to go away!
			newState = WS_DEACTIVATING;
			newFrame = DeactStart;
		}
		else if ((ent->Client.buttons|ent->Client.latched_buttons) & BUTTON_ATTACK)
		{
			ent->Client.latched_buttons &= ~BUTTON_ATTACK;

			// We want to attack!
			// First call, check AttemptToFire
			if (AttemptToFire(ent))
			{
				// Got here, we can fire!
				ent->Client.PlayerState.SetGunFrame (FireStart);
				ent->Client.weaponstate = WS_FIRING;
				ent->Client.grenade_time = 0;

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
			if (ent->Client.PlayerState.GetGunFrame() == IdleEnd)
				newFrame = IdleStart;
			else
			{
				if (CanStopFidgetting(ent) && (rand()&15))
					newFrame = ent->Client.PlayerState.GetGunFrame();
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
			newState = ent->Client.weaponstate;
			newFrame = ent->Client.PlayerState.GetGunFrame();
		}

		// Only do this if we haven't been explicitely set a newFrame
		// because we might want to keep firing beyond this point
		if (newFrame == -1 && ent->Client.PlayerState.GetGunFrame() == FireEnd)
		{
			if (!ent->Client.pers.Inventory.Has(Item))
			{
				NoAmmoWeaponChange (ent);
				newState = WS_DEACTIVATING;
				newFrame = DeactStart;
				ent->Client.grenade_time = 0;
				ent->Client.grenade_thrown = false;
			}
			else
			{
				ent->Client.grenade_time = 0;
				ent->Client.grenade_thrown = false;
				newFrame = IdleStart;
				newState = WS_IDLE;
			}
		}
		break;
	case WS_DEACTIVATING:
		// Change weapon
		ChangeWeapon (ent);
		return;
		break;
	}

	if (newFrame != -1)
		ent->Client.PlayerState.SetGunFrame (newFrame);
	if (newState != -1)
		ent->Client.weaponstate = newState;

	if (newFrame == -1 && newState == -1)
		ent->Client.PlayerState.SetGunFrame (ent->Client.PlayerState.GetGunFrame()+1);
}