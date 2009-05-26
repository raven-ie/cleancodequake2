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

#include "g_local.h"

game_locals_t	game;
level_locals_t	level;
gameImport_t	gi;
gameExport_t	globals;
spawn_temp_t	st;

MediaIndex	snd_fry;
int meansOfDeath;

edict_t		*g_edicts;

CCvar	*deathmatch;
CCvar	*coop;
CCvar	*dmflags;
CCvar	*skill;
CCvar	*fraglimit;
CCvar	*timelimit;
CCvar	*password;
CCvar	*spectator_password;
CCvar	*needpass;
CCvar	*maxclients;
CCvar	*maxspectators;
CCvar	*maxentities;
CCvar	*g_select_empty;
CCvar	*dedicated;

CCvar	*filterban;

CCvar	*sv_maxvelocity;
CCvar	*sv_gravity;

CCvar	*sv_rollspeed;
CCvar	*sv_rollangle;
CCvar	*gun_x;
CCvar	*gun_y;
CCvar	*gun_z;

CCvar	*run_pitch;
CCvar	*run_roll;
CCvar	*bob_up;
CCvar	*bob_pitch;
CCvar	*bob_roll;

CCvar	*sv_cheats;

CCvar	*flood_msgs;
CCvar	*flood_persecond;
CCvar	*flood_waitdelay;

CCvar	*sv_maplist;

void SpawnEntities (char *mapname, char *entities, char *spawnpoint);
void ClientThink (edict_t *ent, userCmd_t *cmd);
BOOL ClientConnect (edict_t *ent, char *userinfo);
void ClientUserinfoChanged (edict_t *ent, char *userinfo);
void ClientDisconnect (edict_t *ent);
void ClientBegin (edict_t *ent);
void RunEntity (edict_t *ent);
void WriteGame (char *filename, BOOL autosave);
void ReadGame (char *filename);
void WriteLevel (char *filename);
void ReadLevel (char *filename);
void InitGame (void);
void G_RunFrame (void);


//===================================================================


void ShutdownGame (void)
{
	DebugPrintf ("==== ShutdownGame ====\n");

	Cmd_RemoveCommands ();
	SvCmd_RemoveCommands ();

	gi.FreeTags (TAG_LEVEL);
	gi.FreeTags (TAG_GAME);
	gi.FreeTags (TAG_CLEAN_GAME);
	gi.FreeTags (TAG_CLEAN_LEVEL);
}


#ifndef _FRONTEND
/*
=================
GetGameAPI

Returns a pointer to the structure with all entry points
and global variables
=================
*/
gameExport_t *GetGameAPI (gameImport_t *import)
{
	gi = *import;

	Swap_Init ();

	globals.apiVersion = GAME_APIVERSION;
	globals.Init = InitGame;
	globals.Shutdown = ShutdownGame;
	globals.SpawnEntities = SpawnEntities;

	globals.WriteGame = WriteGame;
	globals.ReadGame = ReadGame;
	globals.WriteLevel = WriteLevel;
	globals.ReadLevel = ReadLevel;

	globals.ClientThink = ClientThink;
	globals.ClientConnect = ClientConnect;
	globals.ClientUserinfoChanged = ClientUserinfoChanged;
	globals.ClientDisconnect = ClientDisconnect;
	globals.ClientBegin = ClientBegin;
	globals.ClientCommand = ClientCommand;

	globals.RunFrame = G_RunFrame;

	globals.ServerCommand = ServerCommand;

	globals.edictSize = sizeof(edict_t);

	return &globals;
}
#endif

#ifndef GAME_HARD_LINKED
// this is only here so the functions in q_shared.c and q_shwin.c can link

void Com_Printf (comPrint_t flags, char *fmt, ...)
{
	va_list		argptr;
	char		text[MAX_COMPRINT];

	va_start (argptr, fmt);
	vsnprintf_s (text, sizeof(text), MAX_COMPRINT, fmt, argptr);
	va_end (argptr);

	DebugPrintf ("%s", text);
}

void Com_DevPrintf (comPrint_t flags, char *fmt, ...)
{
	va_list		argptr;
	char		text[MAX_COMPRINT];

	va_start (argptr, fmt);
	vsnprintf_s (text, sizeof(text), MAX_COMPRINT, fmt, argptr);
	va_end (argptr);

	DebugPrintf ("%s", text);
}

// FIXME code is ignored
void Com_Error (EComErrorType code, char *fmt, ...)
{
	va_list		argptr;
	char		text[MAX_COMPRINT];

	va_start (argptr, fmt);
	vsnprintf_s (text, sizeof(text), MAX_COMPRINT, fmt, argptr);
	va_end (argptr);

	gi.error ("%s", text);
}

#endif

//======================================================================


/*
=================
ClientEndServerFrames
=================
*/
void ClientEndServerFrames (void)
{
	int		i;
	edict_t	*ent;

	// calc the player views now that all pushing
	// and damage has been added
	for (i=0 ; i<maxclients->Float() ; i++)
	{
		ent = g_edicts + 1 + i;
		if (!ent->inUse || !ent->client)
			continue;
		ClientEndServerFrame (ent);
	}

}

/*
=================
CreateTargetChangeLevel

Returns the created target changelevel
=================
*/
edict_t *CreateTargetChangeLevel(char *map)
{
	edict_t *ent;

	ent = G_Spawn ();
	ent->classname = "target_changelevel";
	Q_snprintfz(level.nextmap, sizeof(level.nextmap), "%s", map);
	ent->map = level.nextmap;
	return ent;
}

/*
=================
EndDMLevel

The timelimit or fraglimit has been exceeded
=================
*/
void EndDMLevel (void)
{
	edict_t		*ent;
	char *s, *t, *f;
	static const char *seps = " ,\n\r";
#ifdef CRT_USE_UNDEPRECATED_FUNCTIONS
	char *nextToken;
#endif

	// stay on same level flag
	if (dmFlags.dfSameLevel)
	{
		BeginIntermission (CreateTargetChangeLevel (level.mapname) );
		return;
	}

	// see if it's in the map list
	if (*sv_maplist->String()) {
		s = G_CopyString(sv_maplist->String());
		f = NULL;
#ifndef CRT_USE_UNDEPRECATED_FUNCTIONS
		t = strtok_s(s, seps);
#else
		t = strtok_s (s, seps, &nextToken);
#endif
		while (t != NULL) {
			if (Q_stricmp(t, level.mapname) == 0) {
				// it's in the list, go to the next one
#ifndef CRT_USE_UNDEPRECATED_FUNCTIONS
				t = strtok(NULL, seps);
#else
				t = strtok_s (NULL, seps, &nextToken);
#endif
				if (t == NULL) { // end of list, go to first one
					if (f == NULL) // there isn't a first one, same level
						BeginIntermission (CreateTargetChangeLevel (level.mapname) );
					else
						BeginIntermission (CreateTargetChangeLevel (f) );
				} else
					BeginIntermission (CreateTargetChangeLevel (t) );
				gi.TagFree(s);
				return;
			}
			if (!f)
				f = t;
#ifndef CRT_USE_UNDEPRECATED_FUNCTIONS
			t = strtok(NULL, seps);
#else
			t = strtok_s (NULL, seps, &nextToken);
#endif
		}
		free(s);
	}

	if (level.nextmap[0]) // go to a specific map
		BeginIntermission (CreateTargetChangeLevel (level.nextmap) );
	else {	// search for a changelevel
		ent = G_Find (NULL, FOFS(classname), "target_changelevel");
		if (!ent)
		{	// the map designer didn't include a changelevel,
			// so create a fake ent that goes back to the same level
			BeginIntermission (CreateTargetChangeLevel (level.mapname) );
			return;
		}
		BeginIntermission (ent);
	}
}


/*
=================
CheckNeedPass
=================
*/
void CheckNeedPass (void)
{
	int need;

	// if password or spectator_password has changed, update needpass
	// as needed
	if (password->Modified() || spectator_password->Modified()) 
	{
		need = 0;

		if (*password->String() && Q_stricmp(password->String(), "none"))
			need |= 1;
		if (*spectator_password->String() && Q_stricmp(spectator_password->String(), "none"))
			need |= 2;

		needpass->Set(Q_VarArgs ("%d", need));
	}
}

/*
=================
CheckDMRules
=================
*/
void CheckDMRules (void)
{
	int			i;
	gclient_t	*cl;

	if (level.intermissiontime)
		return;

	if (!deathmatch->Integer())
		return;

	if (timelimit->Float())
	{
		if (level.time >= timelimit->Float()*60)
		{
			BroadcastPrintf (PRINT_HIGH, "Timelimit hit.\n");
			EndDMLevel ();
			return;
		}
	}

	if (fraglimit->Integer())
	{
		for (i=0 ; i<maxclients->Integer() ; i++)
		{
			cl = game.clients + i;
			if (!g_edicts[i+1].inUse)
				continue;

			if (cl->resp.score >= fraglimit->Integer())
			{
				BroadcastPrintf (PRINT_HIGH, "Fraglimit hit.\n");
				EndDMLevel ();
				return;
			}
		}
	}
}


/*
=============
ExitLevel
=============
*/
void ExitLevel (void)
{
	int		i;
	edict_t	*ent;
	char	command [256];

	Q_snprintfz (command, sizeof(command), "gamemap \"%s\"\n", level.changemap);
	gi.AddCommandString (command);
	level.changemap = NULL;
	level.exitintermission = 0;
	level.intermissiontime = 0;
	ClientEndServerFrames ();

	// clear some things before going to next level
	for (i=0 ; i<maxclients->Integer() ; i++)
	{
		ent = g_edicts + 1 + i;
		if (!ent->inUse)
			continue;
		if (ent->health > ent->client->pers.max_health)
			ent->health = ent->client->pers.max_health;
	}

#ifdef MONSTERS_USE_PATHFINDING
	SavePathTable ();
#endif
}

/*
================
G_RunFrame

Advances the world by 0.1 seconds
================
*/
void G_RunFrame (void)
{
	int		i;
	edict_t	*ent;

	if (level.paused)
	{
		ent = &g_edicts[1];
		for (i=1 ; i <= maxclients->Integer() ; i++, ent++)
		{
			if (!ent->inUse)
				continue;
			ClientBeginServerFrame (ent);
		}

		// build the playerstate_t structures for all players
		ClientEndServerFrames ();

		if (dmflags->Modified())
			dmFlags.UpdateFlags(dmflags->Integer());
		return;
	}

	level.framenum++;
	level.time = level.framenum*FRAMETIME;

	if (level.framenum == 2)
		EndMapCounter();

	// choose a client for monsters to target this frame
	AI_SetSightClient ();

	// exit intermissions

	if (level.exitintermission)
	{
		ExitLevel ();
		return;
	}

	//
	// treat each object in turn
	// even the world gets a chance to think
	//
	ent = &g_edicts[0];
	for (i=0 ; i<globals.numEdicts ; i++, ent++)
	{
		if (!ent->inUse)
			continue;

		level.current_entity = ent;

		Vec3Copy (ent->s.origin, ent->s.oldOrigin);

		// if the ground entity moved, make sure we are still on it
		if ((ent->groundentity) && (ent->groundentity->linkCount != ent->groundentity_linkcount))
		{
			ent->groundentity = NULL;
			if ( !(ent->flags & (FL_SWIM|FL_FLY)) && (ent->svFlags & SVF_MONSTER) && ent->Monster )
				ent->Monster->CheckGround ();
		}

		if (i > 0 && i <= maxclients->Integer())
		{
			ClientBeginServerFrame (ent);
			continue;
		}

		G_RunEntity (ent);
	}

	// see if it is time to end a deathmatch
	CheckDMRules ();

	// see if needpass needs updated
	CheckNeedPass ();

	// build the playerstate_t structures for all players
	ClientEndServerFrames ();

	if (dmflags->Modified())
		dmFlags.UpdateFlags(dmflags->Integer());

#if 0
	DrawNewton();
#endif

#ifdef MONSTERS_USE_PATHFINDING
	RunNodes();
#endif
}

