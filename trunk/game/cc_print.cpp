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
// cc_print.cpp
// Print replacements
//

#include "cc_local.h"

#ifndef USE_EXTENDED_GAME_IMPORTS

/*
=================
SV_ClientPrintf

Sends text across to be displayed if the level passes
=================
*/
static void SV_ClientPrintf (edict_t *ent, EGamePrintLevel printLevel, char *fmt, ...)
{
	va_list		argptr;
	char		string[MAX_COMPRINT];

	if (printLevel < ent->client->resp.messageLevel)
		return;

	va_start (argptr, fmt);
	vsnprintf_s (string, sizeof(string), sizeof(string), fmt, argptr);
	va_end (argptr);

	WriteByte (SVC_PRINT);
	WriteByte (printLevel);
	WriteString (string);
	Cast (CASTFLAG_UNRELIABLE, ent);
}

void ClientPrintf (edict_t *ent, EGamePrintLevel printLevel, char *fmt, ...)
{
	char		msg[MAX_COMPRINT];
	va_list		argptr;
	int			n = 0;

	if (ent)
	{
		n = ent - g_edicts;
		if (n < 1 || n > game.maxclients)
		{
			Com_Printf (0, "CleanCode Warning: ClientPrintf to a non-client\n");
			return;
		}
	}

	// Evaluate args
	va_start (argptr, fmt);
	vsnprintf_s (msg, sizeof(msg), sizeof(msg), fmt, argptr);
	va_end (argptr);

	// Print
	if (ent)
		SV_ClientPrintf (ent, printLevel, "%s", msg);
	else
		Com_Printf (0, "%s", msg);
}

#if defined(WIN32) && defined(_DEBUG)
#include <windows.h>
#endif

// Dprintf is the only command that has to be the same, because of Com_ConPrintf (we don't have it)
void DebugPrintf (char *fmt, ...)
{
	va_list		argptr;
	char		text[MAX_COMPRINT];

	va_start (argptr, fmt);
	vsnprintf_s (text, sizeof(text), MAX_COMPRINT, fmt, argptr);
	va_end (argptr);

_CC_DISABLE_DEPRECATION
(
	gi.dprintf ("%s", text);
)

#if defined(WIN32) && defined(_DEBUG)
	// Pipe to visual studio
	OutputDebugString(text);
#endif
}

void CenterPrintf (edict_t *ent, char *fmt, ...)
{
	char		msg[MAX_COMPRINT];
	va_list		argptr;
	int			n;

	if (!ent)
	{
		DebugPrintf ("CleanCode Warning: CenterPrintf to a non-entity\n");
		return;
	}

	n = ent - g_edicts;
	if ((n < 1) || (n > game.maxclients))
		return;

	va_start (argptr,fmt);
	vsnprintf_s (msg, sizeof(msg), sizeof(msg), fmt, argptr);
	va_end (argptr);

	WriteByte (SVC_CENTERPRINT);
	WriteString (msg);
	Cast (CASTFLAG_RELIABLE, ent);
}

void BroadcastPrintf (EGamePrintLevel printLevel, char *fmt, ...)
{
	va_list		argptr;
	char		string[MAX_COMPRINT];

	va_start (argptr, fmt);
	vsnprintf_s (string, sizeof(string), sizeof(string), fmt, argptr);
	va_end (argptr);
	
	// Echo to console
	if (dedicated->Integer())
	{
		char	copy[1024];
		int		i;
		
		// Mask off high bits
		for (i=0 ; i<((MAX_COMPRINT/2) - 1) && string[i] ; i++)
			copy[i] = string[i]&127;
		copy[i] = 0;
		Com_Printf (0, "%s", copy);
	}

	edict_t *cl;
	int i;
	for (i=1, cl=&g_edicts[1]; i<=game.maxclients ; i++, cl++)
	{
		if (printLevel < cl->client->resp.messageLevel)
			continue;
		if (cl->client->pers.state != SVCS_SPAWNED)
			continue;

		WriteByte (SVC_PRINT);
		WriteByte (printLevel);
		WriteString (string);
		Cast (CASTFLAG_UNRELIABLE, cl);
	}
}
#endif