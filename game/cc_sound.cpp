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
// cc_sound.cpp
// Sound replacements
//

#include "cc_local.h"

/*
==================
SV_StartSound

Each entity can have eight independant sound sources, like voice,
weapon, feet, etc.

If channel & 8, the sound will be sent to everyone, not just
things in the PHS.

FIXME: if entity isn't in PHS, they must be forced to be sent or
have the origin explicitly sent.

Channel 0 is an auto-allocate channel, the others override anything
already running on that entity/channel pair.

An attenuation of 0 will play full volume everywhere in the level.
Larger attenuations will drop off.  (max 4 attenuation)

timeOffset can range from 0.0 to 0.1 to cause sounds to be started
later in the frame than they normally would.

If origin is NULL, the origin is determined from the entity origin
or the midpoint of the entity box for bmodels.
==================
*/
#define 	SOUND_FULLVOLUME	80

// sound communication
// a sound without an ent or pos will be a local only sound
enum {
	SND_VOLUME			= BIT(0),	// a byte
	SND_ATTENUATION		= BIT(1),	// a byte
	SND_POS				= BIT(2),	// three coordinates
	SND_ENT				= BIT(3),	// a short 0-2: channel, 3-12: entity
	SND_OFFSET			= BIT(4)	// a byte, msec offset from frame start
};

#define DEFAULT_SOUND_PACKET_VOLUME			1.0
#define DEFAULT_SOUND_PACKET_ATTENUATION	1.0

/*
static BOOL GI_IsInPVS (vec3_t p1, vec3_t p2)
{
	int		leafnum;
	int		cluster;
	int		area1, area2;
	byte	*mask;

	leafnum = CM_PointLeafnum (p1);
	cluster = CM_LeafCluster (leafnum);
	area1 = CM_LeafArea (leafnum);
	mask = CM_ClusterPVS (cluster);

	leafnum = CM_PointLeafnum (p2);
	cluster = CM_LeafCluster (leafnum);
	area2 = CM_LeafArea (leafnum);

	if (mask && (!(mask[cluster>>3] & BIT(cluster&7))))
		return false;

	if (!CM_AreasConnected (area1, area2))
		return false;		// A door blocks sight

	return true;
}

static BOOL GI_IsInPHS (vec3_t p1, vec3_t p2)
{
	int		leafnum;
	int		cluster;
	int		area1, area2;
	byte	*mask;

	leafnum = CM_PointLeafnum (p1);
	cluster = CM_LeafCluster (leafnum);
	area1 = CM_LeafArea (leafnum);
	mask = CM_ClusterPHS (cluster);

	leafnum = CM_PointLeafnum (p2);
	cluster = CM_LeafCluster (leafnum);
	area2 = CM_LeafArea (leafnum);

	if (mask && (!(mask[cluster>>3] & BIT(cluster&7))))
		return false;		// More than one bounce away

	if (!CM_AreasConnected (area1, area2))
		return false;		// A door blocks hearing

	return true;
}
*/

static void SV_StartSound (vec3_t origin, edict_t *entity, EEntSndChannel channel, MediaIndex soundIndex, float vol, float attenuation, float timeOffset)
{
	int			sendChan, flags, i, ent;
	float		leftVol, rightVol, distanceMult;
	edict_t		*client;
	vec3_t		sourceVec, listenerRight;
	vec3_t		originVec;
	float		dot, dist;
	float		rightScale, leftScale;
	bool		usePHS;

	if (!origin && !entity)
	{
		DebugPrintf ("CleanCode SV_StartSound: No entity or origin!\n");
		return;
	}

	if (vol < 0 || vol > 1.0f)
	{
		DebugPrintf ("CleanCode SV_StartSound: volume = %f", vol);
		return;
	}

	if (attenuation < 0 || attenuation > 4)
	{
		DebugPrintf ("CleanCode SV_StartSound: attenuation = %f", attenuation);
		return;
	}

	if (timeOffset < 0 || timeOffset > 0.255f)
	{
		DebugPrintf ("CleanCode SV_StartSound: timeOffset = %f", timeOffset);
		return;
	}

	ent = entity - g_edicts;

	// No PHS flag
	if (channel & 8 || attenuation == ATTN_NONE)
	{
		// If the sound doesn't attenuate, send it to everyone (global radio chatter, voiceovers, etc)
		usePHS = false;
		channel &= 7;
	}
	else
		usePHS = true;

	sendChan = (ent<<3) | (channel&7);

	flags = 0;
	if (vol != DEFAULT_SOUND_PACKET_VOLUME)
		flags |= SND_VOLUME;
	if (attenuation != DEFAULT_SOUND_PACKET_ATTENUATION)
		flags |= SND_ATTENUATION;

	/*
	** the client doesn't know that bmodels have weird origins
	** the origin can also be explicitly set
	*/
	if ((entity->svFlags & SVF_NOCLIENT) || (entity->solid == SOLID_BSP) || origin)
		flags |= SND_POS;

	// always send the entity number for channel overrides
	flags |= SND_ENT;

	if (timeOffset)
		flags |= SND_OFFSET;

	// use the entity origin/bmodel origin if the origin is specified
	if (!origin)
	{
		if (entity->solid == SOLID_BSP)
		{
			originVec[0] = entity->state.origin[0] + 0.5f * (entity->mins[0] + entity->maxs[0]);
			originVec[1] = entity->state.origin[1] + 0.5f * (entity->mins[1] + entity->maxs[1]);
			originVec[2] = entity->state.origin[2] + 0.5f * (entity->mins[2] + entity->maxs[2]);
		}
		else
			Vec3Copy (entity->state.origin, originVec);

		origin = originVec;
	}

	// Cycle through the different targets and do attenuation calculations
	for (i=1, client=&g_edicts[1] ; i<=game.maxclients ; i++, client++)
	{
		CPlayerEntity *Player = dynamic_cast<CPlayerEntity*>(client->Entity);
		if (Player->Client.pers.state == SVCS_FREE)
			continue;

		if (Player->Client.pers.state != SVCS_SPAWNED && !(channel & CHAN_RELIABLE))
			continue;

		if (usePHS)
		{
			// Not hearable from here
			if (!gi.inPHS (client->state.origin, origin))
				continue;
		}

		Vec3Subtract (origin, client->state.origin, sourceVec);
		distanceMult = attenuation * ((attenuation == ATTN_STATIC) ? 0.001f : 0.0005f);

		dist = VectorNormalizef (sourceVec, sourceVec) - SOUND_FULLVOLUME;
		if (dist < 0)
			dist = 0;			// close enough to be at full volume
		dist *= distanceMult;	// different attenuation levels

		Angles_Vectors (client->state.angles, NULL, listenerRight, NULL);
		dot = DotProduct (listenerRight, sourceVec);

		if (!distanceMult)
		{
			// no attenuation = no spatialization
			rightScale = 1.0f;
			leftScale = 1.0f;
		}
		else
		{
			rightScale = 0.5f * (1.0f + dot);
			leftScale = 0.5f * (1.0f - dot);
		}

		// add in distance effect
		rightVol = (vol * ((1.0f - dist) * rightScale));
		leftVol = (vol * ((1.0f - dist) * leftScale));

		if (rightVol <= 0 && leftVol <= 0)
			continue; // silent

		WriteByte (SVC_SOUND);
		WriteByte (flags);
		WriteByte (soundIndex);

		if (flags & SND_VOLUME)
			WriteByte (vol*255);

		if (flags & SND_ATTENUATION)
			WriteByte (attenuation*64);

		if (flags & SND_OFFSET)
			WriteByte (timeOffset*1000);

		if (flags & SND_ENT)
			WriteShort (sendChan);

		if (flags & SND_POS)
			WritePosition (origin);

		Cast ((channel & CHAN_RELIABLE) ? CASTFLAG_RELIABLE : CASTFLAG_UNRELIABLE, client);
	}
}

void PlaySoundFrom (edict_t *ent, EEntSndChannel channel, MediaIndex soundIndex, float volume, int attenuation, float timeOfs)
{
	SV_StartSound (NULL, ent, channel, soundIndex, volume, attenuation, timeOfs);
}

void PlaySoundAt (vec3_t origin, edict_t *ent, EEntSndChannel channel, MediaIndex soundIndex, float volume, int attenuation, float timeOfs)
{
	SV_StartSound (origin, ent, channel, soundIndex, volume, attenuation, timeOfs);
}