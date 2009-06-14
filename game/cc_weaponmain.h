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
// cc_weapons.h
// New, improved, better, stable item system!
//

typedef int EWeaponState;
enum //EWeaponState
{
	WS_ACTIVATING,
	WS_IDLE,
	WS_FIRING,
	WS_DEACTIVATING
};

class CWeapon
{
protected:
	// Frames
	int				ActivationStart, ActivationNumFrames,
					FireStart,		FireNumFrames,
					IdleStart,		IdleNumFrames,
					DeactStart,		DeactNumFrames;

	int				ActivationEnd, FireEnd, IdleEnd, DeactEnd; // To save calls.

	CWeapon ();

public:
	bool			isQuad, isSilenced;
	CBaseItem		*Item;
	CWeaponItem		*WeaponItem; // The weapon that is linked to this weapon.
	char			*WeaponSound;
	MediaIndex		WeaponSoundIndex;
	char			*WeaponModelString; // Temporary
	MediaIndex		WeaponModelIndex;

	MediaIndex		vwepIndex;

	CWeapon(char *model, int ActivationStart, int ActivationEnd, int FireStart, int FireEnd,
				 int IdleStart, int IdleEnd, int DeactStart, int DeactEnd, char *WeaponSound = NULL);

	// InitWeapon "clears" the previous weapon by introducing the current weapon.
	virtual void	InitWeapon (CPlayerEntity *ent);

	// Muzzle flash
	virtual void	Muzzle (CPlayerEntity *ent, int muzzleNum);
	virtual inline void	FireAnimation (CPlayerEntity *ent);
	void		AttackSound (CPlayerEntity *ent); // Sound for quad and CTF techs

	// General animating function.
	// Doesn't need to be changed.
	virtual void	WeaponGeneric (CPlayerEntity *ent);

	// These two functions replace the need for an array for
	// pause and fire frames.
	virtual inline bool	CanFire	(CPlayerEntity *ent) = 0;
	virtual inline bool	CanStopFidgetting (CPlayerEntity *ent) = 0;

	// Ammo usage
	virtual void	DepleteAmmo(CPlayerEntity *ent, int Amount);
	
	// This function is called when the player hits the attack button.
	// Returns "true" if the animation can go ahead (check for ammo, etc here)
	virtual void	OutOfAmmo (CPlayerEntity *ent);
	virtual bool	AttemptToFire (CPlayerEntity *ent); 

	// The function called to "fire"
	virtual void	Fire (CPlayerEntity *ent) = 0;

	void ChangeWeapon (CPlayerEntity *ent);

	virtual void	Think (CPlayerEntity *ent);

	void	NoAmmoWeaponChange (CPlayerEntity *ent);
};

extern void P_ProjectSource (CPlayerEntity *Player, vec3_t distance, vec3_t forward, vec3_t right, vec3_t result);

#include "cc_blaster.h"
#include "cc_shotgun.h"
#include "cc_supershotgun.h"
#include "cc_machinegun.h"
#include "cc_chaingun.h"
#include "cc_handgrenade.h"
#include "cc_grenadelauncher.h"
#include "cc_rocketlauncher.h"
#include "cc_hyperblaster.h"
#include "cc_railgun.h"
#include "cc_bfg.h"
#ifdef CLEANCTF_ENABLED
#include "cc_grapple.h"
#endif