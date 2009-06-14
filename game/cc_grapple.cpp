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
// cc_grapple.cpp
// CTF Grappling hook
//

#include "cc_local.h"
#ifdef CLEANCTF_ENABLED
#include "m_player.h"

CGrapple WeaponGrapple;

CGrapple::CGrapple() :
CWeapon("models/weapons/grapple/tris.md2", 0, 5, 6, 9,
		10, 31, 32, 36)
{
}

bool CGrapple::CanFire (CPlayerEntity *ent)
{
	switch (ent->Client.PlayerState.GetGunFrame())
	{
	case 6:
		return true;
	}
	return false;
}

bool CGrapple::CanStopFidgetting (CPlayerEntity *ent)
{
	switch (ent->Client.PlayerState.GetGunFrame())
	{
	case 10:
	case 18:
	case 27:
		return true;
	}
	return false;
}

bool CGrapple::AttemptToFire (CPlayerEntity *ent)
{
	return true;
}

// ent is player
void CGrapple::PlayerResetGrapple(CPlayerEntity *ent)
{
	if (ent->Client.ctf_grapple)
		ResetGrapple(ent->Client.ctf_grapple);
}

// self is grapple, not player
void CGrapple::ResetGrapple(edict_t *self)
{
	CPlayerEntity *Player = dynamic_cast<CPlayerEntity*>(self->owner->Entity);
	if (Player->Client.ctf_grapple)
	{
		float volume = 1.0;

		if (Player->Client.silencer_shots)
			volume = 0.2f;

		//PlaySoundFrom (Player->gameEntity, CHAN_WEAPON, SoundIndex("weapons/grapple/grreset.wav"), volume, ATTN_NORM, 0);
		Player->Client.ctf_grapple = NULL;
		Player->Client.ctf_grapplereleasetime = level.time;
		Player->Client.ctf_grapplestate = CTF_GRAPPLE_STATE_HANG+1; // we're firing, not on hook
		Player->Client.PlayerState.GetPMove()->pmFlags &= ~PMF_NO_PREDICTION;
		G_FreeEdict(self);
	}
}

void CGrapple::GrappleTouch (edict_t *self, edict_t *other, plane_t *plane, cmBspSurface_t *surf)
{
	float volume = 1.0;

	if (other == self->owner)
		return;

	CPlayerEntity *Player = dynamic_cast<CPlayerEntity*>(self->owner->Entity);
	if (Player->Client.ctf_grapplestate != CTF_GRAPPLE_STATE_FLY)
		return;

	if (surf && (surf->flags & SURF_TEXINFO_SKY))
	{
		ResetGrapple(self);
		return;
	}

	Vec3Copy(vec3Origin, self->velocity);

	PlayerNoise(Player, self->state.origin, PNOISE_IMPACT);

	if (other->takedamage)
	{
		T_Damage (other, self, Player->gameEntity, self->velocity, self->state.origin, plane->normal, self->dmg, 1, 0, MOD_GRAPPLE);
		ResetGrapple(self);
		return;
	}

	Player->Client.ctf_grapplestate = CTF_GRAPPLE_STATE_PULL; // we're on hook
	self->enemy = other;

	self->solid = SOLID_NOT;

	if (Player->Client.silencer_shots)
		volume = 0.2f;

	PlaySoundFrom (Player->gameEntity, CHAN_WEAPON, SoundIndex("weapons/grapple/grpull.wav"), volume, ATTN_NORM, 0);
	PlaySoundFrom (self, CHAN_WEAPON, SoundIndex("weapons/grapple/grhit.wav"), volume, ATTN_NORM, 0);

	WriteByte (SVC_TEMP_ENTITY);
	WriteByte (TE_SPARKS);
	WritePosition (self->state.origin);
	if (!plane)
		WriteDirection (vec3Origin);
	else
		WriteDirection (plane->normal);
	Cast (CASTFLAG_PVS, self->state.origin);
}

// draw beam between grapple and self
void CGrapple::GrappleDrawCable(edict_t *self)
{
	vec3_t	offset, start, end, f, r, origin;
	vec3_t	dir;
	float	distance;
	CPlayerEntity *Player = dynamic_cast<CPlayerEntity*>(self->owner->Entity);

	Player->State.GetOrigin (origin);

	Angles_Vectors (Player->Client.v_angle, f, r, NULL);
	Vec3Set(offset, 16, 16, Player->gameEntity->viewheight-8);
	P_ProjectSource (Player, offset, f, r, start);

	Vec3Subtract(start, origin, offset);

	Vec3Subtract (start, self->state.origin, dir);
	distance = Vec3Length(dir);
	// don't draw cable if close
	if (distance < 64)
		return;

	// adjust start for beam origin being in middle of a segment
	Vec3Copy (self->state.origin, end);

	CTempEnt_Trails::GrappleCable (origin, end, Player->gameEntity - g_edicts, offset);
}

void SV_AddGravity (edict_t *ent);

// pull the player toward the grapple
void CGrapple::GrapplePull(edict_t *self)
{
	vec3_t hookdir, v;
	float vlen;
	CPlayerEntity *Player = dynamic_cast<CPlayerEntity*>(self->owner->Entity);

	if (Player->Client.pers.Weapon->Item == FindItem("Grapple") &&
		!Player->Client.NewWeapon &&
		Player->Client.weaponstate != WS_FIRING &&
		Player->Client.weaponstate != WS_ACTIVATING)
	{
		ResetGrapple(self);
		return;
	}

	if (self->enemy)
	{
		if (self->enemy->solid == SOLID_NOT)
		{
			ResetGrapple(self);
			return;
		}
		if (self->enemy->solid == SOLID_BBOX)
		{
			Vec3Scale(self->enemy->size, 0.5, v);
			Vec3Add(v, self->enemy->state.origin, v);
			Vec3Add(v, self->enemy->mins, self->state.origin);
			gi.linkentity (self);
		}
		else
			Vec3Copy(self->enemy->velocity, self->velocity);
		if (self->enemy->takedamage &&
			!CheckTeamDamage (self->enemy, Player->gameEntity))
		{
			float volume = 1.0;

			if (Player->Client.silencer_shots)
				volume = 0.2f;

			T_Damage (self->enemy, self, Player->gameEntity, self->velocity, self->state.origin, vec3Origin, 1, 1, 0, MOD_GRAPPLE);
			PlaySoundFrom (self, CHAN_WEAPON, SoundIndex("weapons/grapple/grhurt.wav"), volume, ATTN_NORM, 0);
		}
		if (self->enemy->deadflag)
		{ // he died
			ResetGrapple(self);
			return;
		}
	}

	GrappleDrawCable(self);

	if (Player->Client.ctf_grapplestate > CTF_GRAPPLE_STATE_FLY)
	{
		// pull player toward grapple
		// this causes icky stuff with prediction, we need to extend
		// the prediction layer to include two new fields in the player
		// move stuff: a point and a velocity.  The client should add
		// that velociy in the direction of the point
		vec3_t forward, up;

		Angles_Vectors (Player->Client.v_angle, forward, NULL, up);
		Player->State.GetOrigin (v);
		v[2] += Player->gameEntity->viewheight;
		Vec3Subtract (self->state.origin, v, hookdir);

		vlen = Vec3Length(hookdir);

		if (Player->Client.ctf_grapplestate == CTF_GRAPPLE_STATE_PULL &&
			vlen < 64)
		{
			float volume = 1.0;

			if (Player->Client.silencer_shots)
				volume = 0.2f;

			Player->Client.PlayerState.GetPMove()->pmFlags |= PMF_NO_PREDICTION;
			PlaySoundFrom (Player->gameEntity, CHAN_WEAPON, SoundIndex("weapons/grapple/grhang.wav"), volume, ATTN_NORM, 0);
			Player->Client.ctf_grapplestate = CTF_GRAPPLE_STATE_HANG;
		}

		VectorNormalizeFastf (hookdir);
		Vec3Scale(hookdir, CTF_GRAPPLE_PULL_SPEED, hookdir);
		Vec3Copy(hookdir, Player->gameEntity->velocity);
		SV_AddGravity(Player->gameEntity);
	}
}

void CGrapple::FireGrapple (CPlayerEntity *Player, vec3_t start, vec3_t dir, int damage, int speed, int effect)
{
	edict_t	*grapple;

	VectorNormalizeFastf (dir);

	grapple = G_Spawn();
	Vec3Copy (start, grapple->state.origin);
	Vec3Copy (start, grapple->state.oldOrigin);
	VecToAngles (dir, grapple->state.angles);
	Vec3Scale (dir, speed, grapple->velocity);
	grapple->movetype = MOVETYPE_FLYMISSILE;
	grapple->clipMask = CONTENTS_MASK_SHOT;
	grapple->solid = SOLID_BBOX;
	grapple->state.effects |= effect;
	Vec3Clear (grapple->mins);
	Vec3Clear (grapple->maxs);
	grapple->state.modelIndex = ModelIndex ("models/weapons/grapple/hook/tris.md2");
	grapple->owner = Player->gameEntity;
	grapple->touch = GrappleTouch;
	grapple->dmg = damage;
	Player->Client.ctf_grapple = grapple;
	Player->Client.ctf_grapplestate = CTF_GRAPPLE_STATE_FLY; // we're firing, not on hook
	gi.linkentity (grapple);

	vec3_t origin;
	Player->State.GetOrigin (origin);
	CTrace tr = CTrace (origin, grapple->state.origin, grapple, CONTENTS_MASK_SHOT);
	if (tr.fraction < 1.0)
	{
		Vec3MA (grapple->state.origin, -10, dir, grapple->state.origin);
		grapple->touch (grapple, tr.ent, NULL, NULL);
	}
}	

void CGrapple::Fire (CPlayerEntity *Player)
{
	vec3_t	forward, right;
	vec3_t	start;
	vec3_t	offset;
	float volume = 1.0;

	if (Player->Client.ctf_grapplestate > CTF_GRAPPLE_STATE_FLY)
		return; // it's already out

	Angles_Vectors (Player->Client.v_angle, forward, right, NULL);
	Vec3Set(offset, 24, 8, Player->gameEntity->viewheight-8+2);
	P_ProjectSource (Player, offset, forward, right, start);

	Vec3Scale (forward, -2, Player->Client.kick_origin);
	Player->Client.kick_angles[0] = -1;

	if (Player->Client.silencer_shots)
		volume = 0.2f;

	PlaySoundFrom (Player->gameEntity, CHAN_WEAPON, SoundIndex("weapons/grapple/grfire.wav"), volume, ATTN_NORM, 0);
	FireGrapple (Player, start, forward, 10, CTF_GRAPPLE_SPEED, 0);

	PlayerNoise(Player, start, PNOISE_WEAPON);
	FireAnimation(Player);

	Player->Client.PlayerState.SetGunFrame (Player->Client.PlayerState.GetGunFrame() + 1);
}

void CGrapple::WeaponGeneric (CPlayerEntity *Player)
{
	// Idea from Brazen source
	int newFrame = -1, newState = -1;

	switch (Player->Client.weaponstate)
	{
	case WS_ACTIVATING:
		if (Player->Client.PlayerState.GetGunFrame() == ActivationEnd)
		{
			newFrame = IdleStart;
			newState = WS_IDLE;
		
			// if we just switched back to grapple, immediately go to fire frame
			if (Player->Client.ctf_grapplestate > CTF_GRAPPLE_STATE_FLY)
			{
				newFrame = 9;
				newState = WS_FIRING;
			}
		}
		break;
	case WS_IDLE:
		if (Player->Client.ctf_grapplestate == CTF_GRAPPLE_STATE_HANG+1)
		{
			Player->Client.ctf_grapplestate = CTF_GRAPPLE_STATE_FLY;
			PlaySoundFrom (Player->gameEntity, CHAN_WEAPON, SoundIndex("weapons/grapple/grreset.wav"), (Player->Client.silencer_shots) ? 0.2f : 1.0, ATTN_NORM, 0);
		}
		if (Player->Client.NewWeapon && Player->Client.NewWeapon != this)
		{
			// We want to go away!
			newState = WS_DEACTIVATING;
			newFrame = DeactStart;
		}
		else if ((Player->Client.buttons|Player->Client.latched_buttons) & BUTTON_ATTACK)
		{
			Player->Client.latched_buttons &= ~BUTTON_ATTACK;

			// This here is ugly, but necessary so that machinegun/chaingun/hyperblaster
			// get the right acceptance on first-frame-firing
			Player->Client.buttons |= BUTTON_ATTACK;

			// We want to attack!
			// First call, check AttemptToFire
			if (AttemptToFire(Player))
			{
				// Got here, we can fire!
				Player->Client.PlayerState.SetGunFrame(FireStart);
				Player->Client.weaponstate = WS_FIRING;

				// We need to check against us right away for first-frame firing
				WeaponGeneric(Player);
				return;
			}
			else
			{
				OutOfAmmo(Player);
				NoAmmoWeaponChange (Player);
			}
		}

		// Either we are still idle or a failed fire.
		if (newState == -1)
		{
			if (Player->Client.PlayerState.GetGunFrame() == IdleEnd)
				newFrame = IdleStart;
			else
			{
				if (CanStopFidgetting(Player) && (rand()&15))
					newFrame = Player->Client.PlayerState.GetGunFrame();
			}
		}
		break;
	case WS_FIRING:
		// Check if this is a firing frame.
		if (CanFire(Player))
		{
			Fire(Player);

			// Now, this call above CAN change the underlying frame and state.
			// We need this block to make sure we are still doing what we are supposed to.
			newState = Player->Client.weaponstate;
			newFrame = Player->Client.PlayerState.GetGunFrame();
		}

		// Only do this if we haven't been explicitely set a newFrame
		// because we might want to keep firing beyond this point
		// Go right away if we aren't holding attack
		else if (!(Player->Client.buttons & BUTTON_ATTACK))
		{
			if (Player->Client.ctf_grapple)
				ResetGrapple(Player->Client.ctf_grapple);
			newFrame = IdleStart+1;
			newState = WS_IDLE;
		}
		else if (Player->Client.NewWeapon && 
			Player->Client.ctf_grapplestate > CTF_GRAPPLE_STATE_FLY)
		{
			// he wants to change weapons while grappled
			newState = WS_DEACTIVATING;
			newFrame = 32;
		}
		else if (newFrame == -1 && Player->Client.PlayerState.GetGunFrame() > FireEnd)
		{
			// Grapple shouldn't change unless we want it to
			if ((Player->Client.buttons & BUTTON_ATTACK) && 
				Player->Client.ctf_grapple)
				newFrame = Player->Client.PlayerState.GetGunFrame();
			else
			{
				newFrame = IdleStart+1;
				newState = WS_IDLE;
			}
		}
		break;
	case WS_DEACTIVATING:
		if (Player->Client.PlayerState.GetGunFrame() == DeactEnd)
		{
			// Change weapon
			ChangeWeapon (Player);
			return;
		}
		break;
	}

	if (newFrame != -1)
		Player->Client.PlayerState.SetGunFrame (newFrame);
	if (newState != -1)
		Player->Client.weaponstate = newState;

	if (newFrame == -1 && newState == -1)
		Player->Client.PlayerState.SetGunFrame (Player->Client.PlayerState.GetGunFrame() + 1);
}
#endif