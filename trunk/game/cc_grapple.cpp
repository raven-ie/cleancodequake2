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

bool CGrapple::CanFire (edict_t *ent)
{
	switch (ent->client->playerState.gunFrame)
	{
	case 6:
		return true;
	}
	return false;
}

bool CGrapple::CanStopFidgetting (edict_t *ent)
{
	switch (ent->client->playerState.gunFrame)
	{
	case 10:
	case 18:
	case 27:
		return true;
	}
	return false;
}

bool CGrapple::AttemptToFire (edict_t *ent)
{
	return true;
}

// ent is player
void CGrapple::PlayerResetGrapple(edict_t *ent)
{
	if (ent->client && ent->client->ctf_grapple)
		ResetGrapple(ent->client->ctf_grapple);
}

// self is grapple, not player
void CGrapple::ResetGrapple(edict_t *self)
{
	if (self->owner->client->ctf_grapple)
	{
		float volume = 1.0;
		gclient_t *cl;

		if (self->owner->client->silencer_shots)
			volume = 0.2f;

		PlaySoundFrom (self->owner, CHAN_WEAPON, SoundIndex("weapons/grapple/grreset.wav"), volume, ATTN_NORM, 0);
		cl = self->owner->client;
		cl->ctf_grapple = NULL;
		cl->ctf_grapplereleasetime = level.time;
		cl->ctf_grapplestate = CTF_GRAPPLE_STATE_HANG+1; // we're firing, not on hook
		cl->playerState.pMove.pmFlags &= ~PMF_NO_PREDICTION;
		G_FreeEdict(self);
	}
}

void CGrapple::GrappleTouch (edict_t *self, edict_t *other, plane_t *plane, cmBspSurface_t *surf)
{
	float volume = 1.0;

	if (other == self->owner)
		return;

	if (self->owner->client->ctf_grapplestate != CTF_GRAPPLE_STATE_FLY)
		return;

	if (surf && (surf->flags & SURF_TEXINFO_SKY))
	{
		ResetGrapple(self);
		return;
	}

	Vec3Copy(vec3Origin, self->velocity);

	PlayerNoise(self->owner, self->state.origin, PNOISE_IMPACT);

	if (other->takedamage) {
		T_Damage (other, self, self->owner, self->velocity, self->state.origin, plane->normal, self->dmg, 1, 0, MOD_GRAPPLE);
		ResetGrapple(self);
		return;
	}

	self->owner->client->ctf_grapplestate = CTF_GRAPPLE_STATE_PULL; // we're on hook
	self->enemy = other;

	self->solid = SOLID_NOT;

	if (self->owner->client->silencer_shots)
		volume = 0.2f;

	PlaySoundFrom (self->owner, CHAN_WEAPON, SoundIndex("weapons/grapple/grpull.wav"), volume, ATTN_NORM, 0);
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
	vec3_t	offset, start, end, f, r;
	vec3_t	dir;
	float	distance;

	Angles_Vectors (self->owner->client->v_angle, f, r, NULL);
	Vec3Set(offset, 16, 16, self->owner->viewheight-8);
	P_ProjectSource (self->owner->client, self->owner->state.origin, offset, f, r, start);

	Vec3Subtract(start, self->owner->state.origin, offset);

	Vec3Subtract (start, self->state.origin, dir);
	distance = Vec3Length(dir);
	// don't draw cable if close
	if (distance < 64)
		return;

#if 0
	if (distance > 256)
		return;

	// check for min/max pitch
	vectoangles (dir, angles);
	if (angles[0] < -180)
		angles[0] += 360;
	if (fabs(angles[0]) > 45)
		return;

	CTrace	tr; //!!

	tr = CTrace (start, self->state.origin, self, CONTENTS_MASK_SHOT);
	if (tr.ent != self) {
		CTFResetGrapple(self);
		return;
	}
#endif

	// adjust start for beam origin being in middle of a segment
//	Vec3MA (start, 8, f, start);

	Vec3Copy (self->state.origin, end);
	// adjust end z for end spot since the monster is currently dead
//	end[2] = self->absmin[2] + self->size[2] / 2;

	WriteByte (SVC_TEMP_ENTITY);
#if 1 //def USE_GRAPPLE_CABLE
	WriteByte (TE_GRAPPLE_CABLE);
	WriteShort (self->owner - g_edicts);
	WritePosition (self->owner->state.origin);
	WritePosition (end);
	WritePosition (offset);
#else
	WriteByte (TE_MEDIC_CABLE_ATTACK);
	WriteShort (self - g_edicts);
	WritePosition (end);
	WritePosition (start);
#endif
	Cast (CASTFLAG_PVS, self->state.origin);
}

void SV_AddGravity (edict_t *ent);

// pull the player toward the grapple
void CGrapple::GrapplePull(edict_t *self)
{
	vec3_t hookdir, v;
	float vlen;

	if (self->owner->client->pers.Weapon->Item == FindItem("Grapple") &&
		!self->owner->client->NewWeapon &&
		self->owner->client->weaponstate != WS_FIRING &&
		self->owner->client->weaponstate != WS_ACTIVATING) {
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
		} else
			Vec3Copy(self->enemy->velocity, self->velocity);
		if (self->enemy->takedamage &&
			!CheckTeamDamage (self->enemy, self->owner))
		{
			float volume = 1.0;

			if (self->owner->client->silencer_shots)
				volume = 0.2f;

			T_Damage (self->enemy, self, self->owner, self->velocity, self->state.origin, vec3Origin, 1, 1, 0, MOD_GRAPPLE);
			PlaySoundFrom (self, CHAN_WEAPON, SoundIndex("weapons/grapple/grhurt.wav"), volume, ATTN_NORM, 0);
		}
		if (self->enemy->deadflag) { // he died
			ResetGrapple(self);
			return;
		}
	}

	GrappleDrawCable(self);

	if (self->owner->client->ctf_grapplestate > CTF_GRAPPLE_STATE_FLY) {
		// pull player toward grapple
		// this causes icky stuff with prediction, we need to extend
		// the prediction layer to include two new fields in the player
		// move stuff: a point and a velocity.  The client should add
		// that velociy in the direction of the point
		vec3_t forward, up;

		Angles_Vectors (self->owner->client->v_angle, forward, NULL, up);
		Vec3Copy(self->owner->state.origin, v);
		v[2] += self->owner->viewheight;
		Vec3Subtract (self->state.origin, v, hookdir);

		vlen = Vec3Length(hookdir);

		if (self->owner->client->ctf_grapplestate == CTF_GRAPPLE_STATE_PULL &&
			vlen < 64)
		{
			float volume = 1.0;

			if (self->owner->client->silencer_shots)
				volume = 0.2f;

			self->owner->client->playerState.pMove.pmFlags |= PMF_NO_PREDICTION;
			PlaySoundFrom (self->owner, CHAN_WEAPON, SoundIndex("weapons/grapple/grhang.wav"), volume, ATTN_NORM, 0);
			self->owner->client->ctf_grapplestate = CTF_GRAPPLE_STATE_HANG;
		}

		VectorNormalizeFastf (hookdir);
		Vec3Scale(hookdir, CTF_GRAPPLE_PULL_SPEED, hookdir);
		Vec3Copy(hookdir, self->owner->velocity);
		SV_AddGravity(self->owner);
	}
}

void CGrapple::FireGrapple (edict_t *self, vec3_t start, vec3_t dir, int damage, int speed, int effect)
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
//	grapple->state.sound = SoundIndex ("misc/lasfly.wav");
	grapple->owner = self;
	grapple->touch = GrappleTouch;
//	grapple->nextthink = level.time + FRAMETIME;
//	grapple->think = CTFGrappleThink;
	grapple->dmg = damage;
	self->client->ctf_grapple = grapple;
	self->client->ctf_grapplestate = CTF_GRAPPLE_STATE_FLY; // we're firing, not on hook
	gi.linkentity (grapple);

	CTrace tr = CTrace (self->state.origin, grapple->state.origin, grapple, CONTENTS_MASK_SHOT);
	if (tr.fraction < 1.0)
	{
		Vec3MA (grapple->state.origin, -10, dir, grapple->state.origin);
		grapple->touch (grapple, tr.ent, NULL, NULL);
	}
}	

void CGrapple::Fire (edict_t *ent)
{
	vec3_t	forward, right;
	vec3_t	start;
	vec3_t	offset;
	float volume = 1.0;

	if (ent->client->ctf_grapplestate > CTF_GRAPPLE_STATE_FLY)
		return; // it's already out

	Angles_Vectors (ent->client->v_angle, forward, right, NULL);
	Vec3Set(offset, 24, 8, ent->viewheight-8+2);
	P_ProjectSource (ent->client, ent->state.origin, offset, forward, right, start);

	Vec3Scale (forward, -2, ent->client->kick_origin);
	ent->client->kick_angles[0] = -1;

	if (ent->client->silencer_shots)
		volume = 0.2f;

	PlaySoundFrom (ent, CHAN_WEAPON, SoundIndex("weapons/grapple/grfire.wav"), volume, ATTN_NORM, 0);
	FireGrapple (ent, start, forward, 10, CTF_GRAPPLE_SPEED, 0);

	PlayerNoise(ent, start, PNOISE_WEAPON);
	FireAnimation(ent);

	ent->client->playerState.gunFrame++;
}

void CGrapple::WeaponGeneric (edict_t *ent)
{
	// Idea from Brazen source
	int newFrame = -1, newState = -1;

	switch (ent->client->weaponstate)
	{
	case WS_ACTIVATING:
		if (ent->client->playerState.gunFrame == ActivationEnd)
		{
			newFrame = IdleStart;
			newState = WS_IDLE;
		
			// if we just switched back to grapple, immediately go to fire frame
			if (ent->client->ctf_grapplestate > CTF_GRAPPLE_STATE_FLY)
			{
				if (!(ent->client->buttons & BUTTON_ATTACK))
					newFrame = 9;
				else
					newFrame = 5;
				newState = WS_FIRING;
			}
		}
		break;
	case WS_IDLE:
		if (ent->client->ctf_grapplestate == CTF_GRAPPLE_STATE_HANG+1)
		{
			ent->client->ctf_grapplestate = CTF_GRAPPLE_STATE_FLY;
			PlaySoundFrom (ent, CHAN_WEAPON, SoundIndex("weapons/grapple/grreset.wav"), (ent->client->silencer_shots) ? 0.2f : 1.0, ATTN_NORM, 0);
		}
		if (ent->client->NewWeapon && ent->client->NewWeapon != this)
		{
			// We want to go away!
			newState = WS_DEACTIVATING;
			newFrame = DeactStart;
		}
		else if ((ent->client->buttons|ent->client->latched_buttons) & BUTTON_ATTACK)
		{
			ent->client->latched_buttons &= ~BUTTON_ATTACK;

			// This here is ugly, but necessary so that machinegun/chaingun/hyperblaster
			// get the right acceptance on first-frame-firing
			ent->client->buttons |= BUTTON_ATTACK;

			// We want to attack!
			// First call, check AttemptToFire
			if (AttemptToFire(ent))
			{
				// Got here, we can fire!
				ent->client->playerState.gunFrame = FireStart;
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
			if (ent->client->playerState.gunFrame == IdleEnd)
				newFrame = IdleStart;
			else
			{
				if (CanStopFidgetting(ent) && (rand()&15))
					newFrame = ent->client->playerState.gunFrame;
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
			newFrame = ent->client->playerState.gunFrame;
		}

		// Only do this if we haven't been explicitely set a newFrame
		// because we might want to keep firing beyond this point
		// Go right away if we aren't holding attack
		else if (!(ent->client->buttons & BUTTON_ATTACK))
		{
			ResetGrapple(ent->client->ctf_grapple);
			newFrame = IdleStart+1;
			newState = WS_IDLE;
		}
		else if (ent->client->NewWeapon && 
			ent->client->ctf_grapplestate > CTF_GRAPPLE_STATE_FLY)
		{
			// he wants to change weapons while grappled
			newState = WS_DEACTIVATING;
			newFrame = 32;
		}
		else if (newFrame == -1 && ent->client->playerState.gunFrame > FireEnd)
		{
			// Grapple shouldn't change unless we want it to
			if ((ent->client->buttons & BUTTON_ATTACK) && 
				ent->client->ctf_grapple)
				newFrame = ent->client->playerState.gunFrame;
			else
			{
				newFrame = IdleStart+1;
				newState = WS_IDLE;
			}
		}
		break;
	case WS_DEACTIVATING:
		if (ent->client->playerState.gunFrame == DeactEnd)
		{
			// Change weapon
			this->ChangeWeapon (ent);
			return;
		}
		break;
	}

	if (newFrame != -1)
		ent->client->playerState.gunFrame = newFrame;
	if (newState != -1)
		ent->client->weaponstate = newState;

	if (newFrame == -1 && newState == -1)
		ent->client->playerState.gunFrame++;
}
#endif