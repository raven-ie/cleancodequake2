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
// cc_write.cpp
// "Write" functions to write and check for malformed writes.
//

#include "cc_local.h"

#include <limits.h>
#include <float.h>

_CC_DISABLE_DEPRECATION

// Origin or Ent has to exist.
void Cast (ECastType castType, ECastFlags castFlags, vec3_t Origin = NULL, edict_t *Ent = NULL)
{
	// Sanity checks
	if (castType == CAST_MULTI && Ent)
		Com_Printf (0, "Multicast with an associated Ent\n");
	else if (castType == CAST_MULTI && !Origin)
	{
		Com_Printf (0, "Multicast with no assicated Origin! Can't do!\n");
		return;
	}
	else if (castType == CAST_UNI && !Ent)
	{
		Com_Printf (0, "Unicast with no assicated Ent! Can't do!\n");
		return;
	}
	else if (castType == CAST_UNI && Origin)
		Com_Printf (0, "Multicast with an associated Origin\n");

	// Sends to all entities
	// FIXME: The data still gets written and the tempents happen only once these are hit..
	// Bad, might need to have writes queued :S
	/*if (castType == CAST_MULTI)
	{
		edict_t *e = &g_edicts[1];
		for (int i = 0; i < game.maxclients; i++, e++)
		{
			if (!e || !e->inUse || !e->client || !e->client->pers.connected)
				continue;

			if ((castFlags & CASTFLAG_PVS) && !gi.inPVS(Origin, e->state.origin))
				continue;
			if ((castFlags & CASTFLAG_PHS) && !gi.inPHS(Origin, e->state.origin))
				continue;

			gi.unicast (e, (castFlags & CASTFLAG_RELIABLE) ? true : false);
		}
	}
	// Send to one entity
	else if (castType == CAST_UNI)
	{
		if ((castFlags & CASTFLAG_PVS) && !gi.inPVS(Origin, Ent->state.origin))
			return;
		if ((castFlags & CASTFLAG_PHS) && !gi.inPHS(Origin, Ent->state.origin))
			return;

		gi.unicast (Ent, (castFlags & CASTFLAG_RELIABLE) ? true : false);
	}*/
	if (castType == CAST_MULTI)
		gi.multicast (Origin, (castFlags & CASTFLAG_PVS) ? MULTICAST_PVS : MULTICAST_PHS);
	else if (castType == CAST_UNI)
		gi.unicast (Ent, (castFlags & CASTFLAG_RELIABLE) ? true : false);

}
void Cast (ECastFlags castFlags, vec3_t Origin)
{
	Cast (CAST_MULTI, castFlags, Origin, NULL);
}
void Cast (ECastFlags castFlags, edict_t *Ent)
{
	Cast (CAST_UNI, castFlags, NULL, Ent);
}

void WriteChar (char val)
{
	if (val < CHAR_MIN || val > CHAR_MAX)
	{
		Com_Printf (0, "Malformed char written!\n");
		val = Clamp<char> (val, CHAR_MIN, CHAR_MAX);
	}

	gi.WriteChar (val);
}

void WriteByte (byte val)
{
	if (val < 0 || val > UCHAR_MAX)
	{
		Com_Printf (0, "Malformed byte written!\n");
		val = Clamp<byte> (val, 0, UCHAR_MAX);
	}

	gi.WriteByte (val);
}

void WriteShort (short val)
{
	if (val < SHRT_MIN || val > SHRT_MAX)
	{
		Com_Printf (0, "Malformed short written!\n");
		val = Clamp<short> (val, SHRT_MIN, SHRT_MAX);
	}

	gi.WriteShort (val);
}

void WriteLong (long val)
{
	if (val < LONG_MIN || val > LONG_MAX)
	{
		Com_Printf (0, "Malformed long written!\n");
		val = Clamp<long> (val, LONG_MIN, LONG_MAX);
	}

	gi.WriteLong (val);
}

void WriteFloat (float val)
{
	if (val < FLT_MIN || val > FLT_MAX)
	{
		Com_Printf (0, "Malformed float written!\n");
		val = Clamp<float> (val, FLT_MIN, FLT_MAX);
	}

	gi.WriteFloat (val);
}

void WriteAngle (float val)
{
	if (val < 0 || val > 360)
		Com_Printf (0, "Malformed angle may have been written!\n");

	WriteByte (ANGLE2BYTE (val));
}

void WriteAngle16 (float val)
{
	if (val < 0 || val > 360)
		Com_Printf (0, "Malformed angle may have been written!\n");

	WriteShort (ANGLE2SHORT (val));
}

void WriteString (char *val)
{
	if (!val || val == NULL || !val[0] || strlen(val) > 1400)
	{
		Com_Printf (0, "Malformed string written!\n");
		// FIXME: Clamp the string??
	}

	gi.WriteString (val);
}

void WriteCoord (float f)
{
	WriteShort ((int)(f * 8));
}

void WritePosition (vec3_t val)
{
	if (!val)
	{
		for (int i = 0; i < 3; i++)
			WriteCoord(vec3Origin[i]);
	}
	else
	{
		bool Printed = false;
		for (int i = 0; i < 3; i++)
		{
			if (!Printed && (val[i] > 4096 || val[i] < -4096))
			{			
				Com_Printf (0, "Malformed position may have been written!\n");
				Printed = true;
			}

			WriteCoord(val[i]);
		}
	}
}

void WritePosition (vec3f val)
{
	if (!val)
	{
		for (int i = 0; i < 3; i++)
			WriteCoord(vec3Origin[i]);
	}
	else
	{
		bool Printed = false;
		for (int i = 0; i < 3; i++)
		{
			if (!Printed && (val[i] > 4096 || val[i] < -4096))
			{			
				Com_Printf (0, "Malformed position may have been written!\n");
				Printed = true;
			}

			WriteCoord(val[i]);
		}
	}
}

void WriteDirection (vec3_t val)
{
	WriteByte (DirToByte (val));
}

void WriteDirection (vec3f val)
{
	WriteByte (DirToByte (val));
}

_CC_ENABLE_DEPRECATION