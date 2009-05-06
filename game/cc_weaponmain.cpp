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
// cc_weaponmain.cpp
// Main system/class for the weapons
//

#include "cc_local.h"
#include "m_player.h"

CWeapon::CWeapon(char *model, int ActivationStart, int ActivationNumFrames, int FireStart, int FireNumFrames,
				 int IdleStart, int IdleNumFrames, int DeactStart, int DeactNumFrames, char *WeaponSound) :
ActivationStart(ActivationStart),
ActivationNumFrames(ActivationNumFrames),
FireStart(FireStart),
FireNumFrames(FireNumFrames),
IdleStart(IdleStart),
IdleNumFrames(IdleNumFrames),
DeactStart(DeactStart),
DeactNumFrames(DeactNumFrames),
WeaponSound(WeaponSound)
{
	WeaponModelString = model;

	ActivationEnd = (ActivationStart + ActivationNumFrames);
	FireEnd = (FireStart + FireNumFrames);
	IdleEnd = (IdleStart + IdleNumFrames);
	DeactEnd = (DeactStart + DeactNumFrames);
};

void CWeapon::InitWeapon (edict_t *ent)
{
	ent->client->ps.gunFrame = ActivationStart;
	ent->client->ps.gunIndex = ModelIndex(WeaponModelString);
	ent->client->weaponstate = WS_ACTIVATING;
}

void CWeapon::WeaponGeneric (edict_t *ent)
{
	// Idea from Brazen source
	int newFrame = -1, newState = -1;

	switch (ent->client->weaponstate)
	{
	case WS_ACTIVATING:
		if (ent->client->ps.gunFrame == ActivationEnd)
		{
			newFrame = IdleStart;
			newState = WS_IDLE;
		}
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
			// Quad damage sound if we have it...
			if (isQuad)
				Sound(ent, CHAN_ITEM, SoundIndex("items/damage3.wav"));

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
			newFrame = IdleStart;
			newState = WS_IDLE;
		}
		break;
	case WS_DEACTIVATING:
		if (ent->client->ps.gunFrame == DeactEnd)
		{
			// Change weapon
			this->ChangeWeapon (ent);
			return;
		}
		break;
	}

	if (newFrame != -1)
		ent->client->ps.gunFrame = newFrame;
	if (newState != -1)
		ent->client->weaponstate = newState;

	if (newFrame == -1 && newState == -1)
		ent->client->ps.gunFrame++;
}

void CWeapon::ChangeWeapon (edict_t *ent)
{
	ent->client->pers.Weapon = ent->client->NewWeapon;
	ent->client->NewWeapon = NULL;
	ent->client->machinegun_shots = 0;

	// set visible model
	if (ent->s.modelIndex == 255) {
		int i = 0;
		//if (ent->client->pers.Weapon)
		//	i = ((ent->client->pers.Weapon & 0xff) << 8);
		//else
		ent->s.skinNum = (ent - g_edicts - 1) | i;
	}

	if (!ent->client->pers.Weapon)
	{	// dead
		ent->client->ps.gunIndex = 0;

		if (!ent->client->grenade_thrown && !ent->client->grenade_blew_up && ent->client->grenade_time >= level.time) // We had a grenade cocked
		{
			WeaponGrenades.FireGrenade(ent, false);
			ent->client->grenade_time = 0;
		}
		return;
	}

	ent->client->pers.Weapon->InitWeapon(ent);

	ent->client->anim_priority = ANIM_PAIN;
	if (ent->client->ps.pMove.pmFlags & PMF_DUCKED)
	{
		ent->s.frame = FRAME_crpain1;
		ent->client->anim_end = FRAME_crpain4;
	}
	else
	{
		ent->s.frame = FRAME_pain301;
		ent->client->anim_end = FRAME_pain304;
	}
}

void CWeapon::DepleteAmmo (edict_t *ent, int Amount = 1)
{
	if (this->WeaponItem)
	{
		CAmmo *Ammo = this->WeaponItem->Ammo;

		if (Ammo)
			ent->client->pers.Inventory.Remove (Ammo, Amount);
	}
	else if (this->Item && (this->Item->Flags & ITEMFLAG_AMMO))
		ent->client->pers.Inventory.Remove (this->Item, Amount);
}

bool CWeapon::AttemptToFire (edict_t *ent)
{
	int numAmmo = 0;
	CAmmo *Ammo;
	int quantity = 0;

	if (this->Item && (this->Item->Flags & ITEMFLAG_AMMO))
	{
		numAmmo = ent->client->pers.Inventory.Has(this->Item);
		Ammo = dynamic_cast<CAmmo*>(this->Item);
		quantity = Ammo->Amount;
	}
	// Revision: Always going to be true here.
	else
	{
		if (this->WeaponItem->Ammo)
		{
			Ammo = this->WeaponItem->Ammo;
			quantity = this->WeaponItem->Quantity;
			if (Ammo)
				numAmmo = ent->client->pers.Inventory.Has(Ammo);
		}
		else
			return true;
	}

	if (numAmmo < quantity)
		return false;
	else
		return true;
}

void CWeapon::OutOfAmmo (edict_t *ent)
{
	// Doesn't affect pain anymore!
	if (level.time >= ent->damage_debounce_time)
	{
		Sound(ent, CHAN_AUTO, SoundIndex("weapons/noammo.wav"));
		ent->damage_debounce_time = level.time + 1;
	}
}

// Routines
inline void P_ProjectSource (gclient_t *client, vec3_t point, vec3_t distance, vec3_t forward, vec3_t right, vec3_t result)
{
	vec3_t	_distance;

	Vec3Copy (distance, _distance);
	if (client->pers.hand == LEFT_HANDED)
		_distance[1] *= -1;
	else if (client->pers.hand == CENTER_HANDED)
		_distance[1] = 0;
	G_ProjectSource (point, _distance, forward, right, result);
}


/*
===============
PlayerNoise

Each player can have two noise objects associated with it:
a personal noise (jumping, pain, weapon firing), and a weapon
target noise (bullet wall impacts)

Monsters that don't directly see the player can move
to a noise in hopes of seeing the player from there.
===============
*/
void PlayerNoise(edict_t *who, vec3_t where, int type)
{
	edict_t		*noise;

	if (type == PNOISE_WEAPON)
	{
		if (who->client->silencer_shots)
		{
			who->client->silencer_shots--;
			return;
		}
	}

	if (deathmatch->Integer())
		return;

	if (who->flags & FL_NOTARGET)
		return;


	if (!who->mynoise)
	{
		noise = G_Spawn();
		noise->classname = "player_noise";
		Vec3Set (noise->mins, -8, -8, -8);
		Vec3Set (noise->maxs, 8, 8, 8);
		noise->owner = who;
		noise->svFlags = SVF_NOCLIENT;
		who->mynoise = noise;

		noise = G_Spawn();
		noise->classname = "player_noise";
		Vec3Set (noise->mins, -8, -8, -8);
		Vec3Set (noise->maxs, 8, 8, 8);
		noise->owner = who;
		noise->svFlags = SVF_NOCLIENT;
		who->mynoise2 = noise;
	}

	if (type == PNOISE_SELF || type == PNOISE_WEAPON)
	{
		noise = who->mynoise;
		level.sound_entity = noise;
		level.sound_entity_framenum = level.framenum;
	}
	else // type == PNOISE_IMPACT
	{
		noise = who->mynoise2;
		level.sound2_entity = noise;
		level.sound2_entity_framenum = level.framenum;
	}

	Vec3Copy (where, noise->s.origin);
	Vec3Subtract (where, noise->maxs, noise->absMin);
	Vec3Add (where, noise->maxs, noise->absMax);
	noise->teleport_time = level.time;
	gi.linkentity (noise);
}

void CWeapon::Muzzle (edict_t *ent, int muzzleNum)
{
	if (isSilenced)
		muzzleNum |= MZ_SILENCED;
	TempEnts.MuzzleFlash(ent->s.origin, ent-g_edicts, muzzleNum);
}

/*
=================
Think_Weapon

Called by ClientBeginServerFrame and ClientThink
=================
*/
void CWeapon::Think (edict_t *ent)
{
	// if just died, put the weapon away
	if (ent->health < 1)
	{
		ent->client->NewWeapon = NULL;
		ChangeWeapon (ent);
		return;
	}

	// call active weapon think routine
	isQuad = (ent->client->quad_framenum > level.framenum);
	isSilenced = (ent->client->silencer_shots) ? true : false;
	WeaponGeneric (ent);
}

// YUCK
// Better way?
void CWeapon::NoAmmoWeaponChange (edict_t *ent)
{
	// Dead?
	if (!ent->client->pers.Weapon || ent->health <= 0 || ent->deadflag)
		return;

	// Collect info on our current state
	bool HasShotgun = (ent->client->pers.Inventory.Has(FindItem("Shotgun")) != 0);
	bool HasSuperShotgun = (ent->client->pers.Inventory.Has(FindItem("Super Shotgun")) != 0);
	bool HasMachinegun = (ent->client->pers.Inventory.Has(FindItem("Machinegun")) != 0);
	bool HasChaingun = (ent->client->pers.Inventory.Has(FindItem("Chaingun")) != 0);
	bool HasGrenadeLauncher = (ent->client->pers.Inventory.Has(FindItem("Grenade Launcher")) != 0);
	bool HasRocketLauncher = (ent->client->pers.Inventory.Has(FindItem("Rocket Launcher")) != 0);
	bool HasHyperblaster = (ent->client->pers.Inventory.Has(FindItem("Hyperblaster")) != 0);
	bool HasRailgun = (ent->client->pers.Inventory.Has(FindItem("Railgun")) != 0);
	bool HasBFG = (ent->client->pers.Inventory.Has(FindItem("BFG10k")) != 0);

	bool HasShells = (ent->client->pers.Inventory.Has(FindItem("Shells")) != 0);
	bool HasShells_ForSuperShotty = (ent->client->pers.Inventory.Has(FindItem("Shells")) > 5);
	bool HasBullets = (ent->client->pers.Inventory.Has(FindItem("Bullets")) != 0);
	bool HasBullets_ForChaingun = (ent->client->pers.Inventory.Has(FindItem("Bullets")) >= 50);
	bool HasGrenades = (ent->client->pers.Inventory.Has(FindItem("Grenades")) != 0);
	bool HasRockets = (ent->client->pers.Inventory.Has (FindItem("Rockets")) != 0);
	bool HasCells = (ent->client->pers.Inventory.Has (FindItem("Cells")) != 0);
	bool HasCells_ForBFG = (ent->client->pers.Inventory.Has (FindItem("Cells")) >= 50);
	bool HasSlugs = (ent->client->pers.Inventory.Has(FindItem("Slugs")) != 0);

	bool AlmostDead = (ent->health <= 20);

	CWeaponItem	*Chosen_Weapon = NULL;
	CAmmo		*Chosen_Ammo = NULL;

	// Try not to choose explosive weapons
	if (AlmostDead)
	{
		if (HasCells && HasHyperblaster)
			Chosen_Weapon = dynamic_cast<CWeaponItem*>(FindItem("Hyperblaster"));
		else if (HasSlugs && HasRailgun)
			Chosen_Weapon = dynamic_cast<CWeaponItem*>(FindItem("Railgun"));
		else if (HasBullets_ForChaingun && HasChaingun)
			Chosen_Weapon = dynamic_cast<CWeaponItem*>(FindItem("Chaingun"));
		else if (HasBullets_ForChaingun && HasMachinegun)
			Chosen_Weapon = dynamic_cast<CWeaponItem*>(FindItem("Machinegun"));
		else if (HasShells_ForSuperShotty && HasSuperShotgun)
			Chosen_Weapon = dynamic_cast<CWeaponItem*>(FindItem("Super Shotgun"));
		else if (HasShells && HasSuperShotgun)
			Chosen_Weapon = dynamic_cast<CWeaponItem*>(FindItem("Super Shotgun"));
		else if (HasShells_ForSuperShotty && HasShotgun)
			Chosen_Weapon = dynamic_cast<CWeaponItem*>(FindItem("Shotgun"));
		else if (HasShells && HasShotgun)
			Chosen_Weapon = dynamic_cast<CWeaponItem*>(FindItem("Shotgun"));
	}

	// Still nothing
	if (!Chosen_Weapon || !Chosen_Ammo)
	{
		if (HasCells_ForBFG && HasBFG)
			Chosen_Weapon = dynamic_cast<CWeaponItem*>(FindItem("BFG10k"));
		else if (HasCells && HasHyperblaster)
			Chosen_Weapon = dynamic_cast<CWeaponItem*>(FindItem("Hyperblaster"));
		else if (HasSlugs && HasRailgun)
			Chosen_Weapon = dynamic_cast<CWeaponItem*>(FindItem("Railgun"));
		else if (HasRockets && HasRocketLauncher)
			Chosen_Weapon = dynamic_cast<CWeaponItem*>(FindItem("Rocket Launcher"));
		else if (HasGrenades && HasGrenadeLauncher)
			Chosen_Weapon = dynamic_cast<CWeaponItem*>(FindItem("Grenade Launcher"));
		else if (HasBullets_ForChaingun && HasChaingun)
			Chosen_Weapon = dynamic_cast<CWeaponItem*>(FindItem("Chaingun"));
		else if (HasBullets && HasMachinegun)
			Chosen_Weapon = dynamic_cast<CWeaponItem*>(FindItem("Machinegun"));
		else if (HasBullets && HasChaingun)
			Chosen_Weapon = dynamic_cast<CWeaponItem*>(FindItem("Chaingun"));
		else if (HasShells_ForSuperShotty && HasSuperShotgun)
			Chosen_Weapon = dynamic_cast<CWeaponItem*>(FindItem("Super Shotgun"));
		else if (HasShells && HasSuperShotgun)
			Chosen_Weapon = dynamic_cast<CWeaponItem*>(FindItem("Super Shotgun"));
		else if (HasShells_ForSuperShotty && HasShotgun)
			Chosen_Weapon = dynamic_cast<CWeaponItem*>(FindItem("Shotgun"));
		else if (HasShells && HasShotgun)
			Chosen_Weapon = dynamic_cast<CWeaponItem*>(FindItem("Shotgun"));
		else if (HasGrenades)
			Chosen_Ammo = dynamic_cast<CAmmo*>(FindItem("Grenades"));
	}

	if (!Chosen_Weapon && !Chosen_Ammo)
		Chosen_Weapon = dynamic_cast<CWeaponItem*>(FindItem("Blaster"));

	bool HasCurrentWeapon = true;
	// Do a quick check to see if we still even have the weapon we're holding.
	if (ent->client->pers.Weapon->WeaponItem && !ent->client->pers.Inventory.Has(ent->client->pers.Weapon->WeaponItem))
		HasCurrentWeapon = false;

	ent->client->NewWeapon = (Chosen_Weapon == NULL) ? Chosen_Ammo->Weapon : Chosen_Weapon->Weapon;
	if (!HasCurrentWeapon)
		ent->client->pers.Weapon->ChangeWeapon(ent);
}