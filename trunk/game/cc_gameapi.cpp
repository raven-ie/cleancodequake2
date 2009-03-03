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
// cc_gameapi.cpp
// Contains overloads and other neat things
//

#include "cc_local.h"

// Sounds
static void Sound_Base (vec3_t pos, edict_t *ent, EEntSndChannel channel, int soundindex, float volume, int attenuation, float timeOfs, char *soundString)
{
	int sIndex;

	if ((!pos && !ent))
	{
		Com_Printf (0, "Attempted to play a sound on no entity!\n");
		return;
	}

	if (!soundString)
		sIndex = soundindex;
	else if (soundindex != -1)
		sIndex = gi.soundindex(soundString);
	else
	{
		Com_Printf (0, "No sound index!\n");
		return;
	}

	if (pos)
		gi.positioned_sound (pos, ent ? ent : &g_edicts[0], channel, soundindex, volume, attenuation, timeOfs);
	else
		gi.sound (ent, channel, soundindex, volume, attenuation, timeOfs);
}

void Sound (edict_t *ent, EEntSndChannel channel, int soundindex, float volume, int attenuation, float timeOfs)
{
	Sound_Base (NULL, ent, channel, soundindex, volume, attenuation, timeOfs, NULL);
}
void Sound (edict_t *ent, EEntSndChannel channel, char *soundString, float volume, int attenuation, float timeOfs)
{
	Sound_Base (NULL, ent, channel, -1, volume, attenuation, timeOfs, soundString);
}

void Sound (edict_t *ent, EEntSndChannel channel, int soundindex, float volume, int attenuation)
{
	Sound_Base (NULL, ent, channel, soundindex, volume, attenuation, 0, NULL);
}
void Sound (edict_t *ent, EEntSndChannel channel, char *soundString, float volume, int attenuation)
{
	Sound_Base (NULL, ent, channel, -1, volume, attenuation, 0, soundString);
}

void Sound (edict_t *ent, EEntSndChannel channel, int soundindex, float volume)
{
	Sound_Base (NULL, ent, channel, soundindex, volume, ATTN_NORM, 0, NULL);
}
void Sound (edict_t *ent, EEntSndChannel channel, char *soundString, float volume)
{
	Sound_Base (NULL, ent, channel, -1, volume, ATTN_NORM, 0, soundString);
}

void Sound (edict_t *ent, EEntSndChannel channel, int soundindex)
{
	Sound_Base (NULL, ent, channel, soundindex, 1.0f, ATTN_NORM, 0, NULL);
}
void Sound (edict_t *ent, EEntSndChannel channel, char *soundString)
{
	Sound_Base (NULL, ent, channel, -1, 1.0f, ATTN_NORM, 0, soundString);
}

void Sound (edict_t *ent, int soundindex)
{
	Sound_Base (NULL, ent, CHAN_AUTO, soundindex, 1.0f, ATTN_NORM, 0, NULL);
}
void Sound (edict_t *ent, char *soundString)
{
	Sound_Base (NULL, ent, CHAN_AUTO, -1, 1.0f, ATTN_NORM, 0, soundString);
}

void Sound (int soundindex)
{
	Sound_Base (NULL, &g_edicts[0], CHAN_AUTO, soundindex, 1.0f, ATTN_NONE, 0, NULL);
}
void Sound (char *soundString)
{
	Sound_Base (NULL, &g_edicts[0], CHAN_AUTO, -1, 1.0f, ATTN_NONE, 0, soundString);
}

void Sound (vec3_t pos, edict_t *ent, EEntSndChannel channel, int soundindex, float volume, int attenuation, float timeOfs)
{
	Sound_Base (pos, ent, channel, soundindex, volume, attenuation, timeOfs, NULL);
}
void Sound (vec3_t pos, edict_t *ent, EEntSndChannel channel, char *soundString, float volume, int attenuation, float timeOfs)
{
	Sound_Base (pos, ent, channel, -1, volume, attenuation, timeOfs, soundString);
}

void Sound (vec3_t pos, edict_t *ent, EEntSndChannel channel, int soundindex, float volume, int attenuation)
{
	Sound_Base (pos, ent, channel, soundindex, volume, attenuation, 0, NULL);
}
void Sound (vec3_t pos, edict_t *ent, EEntSndChannel channel, char *soundString, float volume, int attenuation)
{
	Sound_Base (pos, ent, channel, -1, volume, attenuation, 0, soundString);
}

void Sound (vec3_t pos, edict_t *ent, EEntSndChannel channel, int soundindex, float volume)
{
	Sound_Base (pos, ent, channel, soundindex, volume, ATTN_NORM, 0, NULL);
}
void Sound (vec3_t pos, edict_t *ent, EEntSndChannel channel, char *soundString, float volume)
{
	Sound_Base (pos, ent, channel, -1, volume, ATTN_NORM, 0, soundString);
}

void Sound (vec3_t pos, edict_t *ent, EEntSndChannel channel, int soundindex)
{
	Sound_Base (pos, ent, channel, soundindex, 1.0f, ATTN_NORM, 0, NULL);
}
void Sound (vec3_t pos, edict_t *ent, EEntSndChannel channel, char *soundString)
{
	Sound_Base (pos, ent, channel, -1, 1.0f, ATTN_NORM, 0, soundString);
}

void Sound (vec3_t pos, edict_t *ent, int soundindex)
{
	Sound_Base (pos, ent, CHAN_AUTO, soundindex, 1.0f, ATTN_NORM, 0, NULL);
}
void Sound (vec3_t pos, edict_t *ent, char *soundString)
{
	Sound_Base (pos, ent, CHAN_AUTO, -1, 1.0f, ATTN_NORM, 0, soundString);
}
