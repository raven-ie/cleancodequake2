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
#include "cc_exceptionhandler.h"

game_locals_t	game;
level_locals_t	level;
gameImport_t	gi;
gameExport_t	globals;
spawn_temp_t	st;

MediaIndex	snd_fry;
EMeansOfDeath meansOfDeath;

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
CCvar	*developer;

CCvar	*filterban;

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
CCvar	*map_debug;
CCvar	*cc_techflags;

#ifdef CLEANCTF_ENABLED
//ZOID
CCvar	*capturelimit;
CCvar	*instantweap;
//ZOID
#endif

void SpawnEntities (char *mapname, char *entities, char *spawnpoint);
void RunEntity (edict_t *ent);
void WriteGame (char *filename, BOOL autosave);
void ReadGame (char *filename);
void WriteLevel (char *filename);
void ReadLevel (char *filename);
void InitGame (void);
void G_RunFrame (void);


//===================================================================

void ShutdownBodyQueue ();
void Shutdown_Junk ();
void Mem_Stats_f(CPlayerEntity *ent);

void ShutdownGame (void)
{
#ifdef CC_USE_EXCEPTION_HANDLER
__try
{
#endif
	DebugPrintf ("==== ShutdownGame ====\n");

	Cmd_RemoveCommands ();
	SvCmd_RemoveCommands ();

	ShutdownBodyQueue ();
	Shutdown_Junk ();

	size_t freedGame = Mem_FreePool (com_gamePool);
	size_t freedLevel = Mem_FreePool (com_levelPool);
	size_t freedGeneric = Mem_FreePool (com_genericPool);

	DebugPrintf ("Freed %u bytes of game memory, %u bytes of level memory and %u bytes of generic memory.\n", freedGame, freedLevel, freedGeneric);
#ifdef CC_USE_EXCEPTION_HANDLER
}
__except (EGLExceptionHandler(GetExceptionCode(), GetExceptionInformation()))
{
	return;
}
#endif
}


#ifndef _FRONTEND

/*
===========
ClientBegin

called when a client has finished connecting, and is ready
to be placed into the game.  This will happen every level load.
============
*/
void ClientBegin (edict_t *ent)
{
#ifdef CC_USE_EXCEPTION_HANDLER
__try
{
#endif
	dynamic_cast<CPlayerEntity*>(ent->Entity)->Begin ();
#ifdef CC_USE_EXCEPTION_HANDLER
}
__except (EGLExceptionHandler(GetExceptionCode(), GetExceptionInformation()))
{
	return;
}
#endif
}

/*
===========
ClientUserInfoChanged

called whenever the player updates a userinfo variable.

The game can override any of the settings in place
(forcing skins or names, etc) before copying it off.
============
*/
void ClientUserinfoChanged (edict_t *ent, char *userinfo)
{
#ifdef CC_USE_EXCEPTION_HANDLER
__try
{
#endif
	(dynamic_cast<CPlayerEntity*>(ent->Entity))->UserinfoChanged (userinfo);
#ifdef CC_USE_EXCEPTION_HANDLER
}
__except (EGLExceptionHandler(GetExceptionCode(), GetExceptionInformation()))
{
	return;
}
#endif
}


/*
===========
ClientConnect

Called when a player begins connecting to the server.
The game can refuse entrance to a client by returning false.
If the client is allowed, the connection process will continue
and eventually get to ClientBegin()
Changing levels will NOT cause this to be called again, but
loadgames will.
============
*/
BOOL ClientConnect (edict_t *ent, char *userinfo)
{
#ifdef CC_USE_EXCEPTION_HANDLER
__try
{
#endif
	return (dynamic_cast<CPlayerEntity*>(ent->Entity)->Connect(userinfo)) ? 1 : 0;
#ifdef CC_USE_EXCEPTION_HANDLER
}
__except (EGLExceptionHandler(GetExceptionCode(), GetExceptionInformation()))
{
	return true;
}
#endif
}

/*
===========
ClientDisconnect

Called when a player drops from the server.
Will not be called between levels.
============
*/
void ClientDisconnect (edict_t *ent)
{
	dynamic_cast<CPlayerEntity*>(ent->Entity)->Disconnect();
}


//==============================================================

/*
==============
ClientThink

This will be called once for each client frame, which will
usually be a couple times for each server frame.
==============
*/
void ClientThink (edict_t *ent, userCmd_t *ucmd)
{
	(dynamic_cast<CPlayerEntity*>(ent->Entity))->ClientThink (ucmd);
}

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

_CC_DISABLE_DEPRECATION
	gi.error ("%s", text);
_CC_ENABLE_DEPRECATION
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
	// calc the player views now that all pushing
	// and damage has been added
	for (int i = 1; i <= game.maxclients ; i++)
	{
		CPlayerEntity *Player = dynamic_cast<CPlayerEntity*>(g_edicts[i].Entity);
		if (!Player->IsInUse())
			continue;
		Player->EndServerFrame ();
	}

}
/*
=================
CreateTargetChangeLevel

Returns the created target changelevel
=================
*/
#include "cc_target_entities.h"
// FIXME: Remove. This sucks.
CTargetChangeLevel *CreateTargetChangeLevel(char *map)
{
	CTargetChangeLevel *Temp = QNew (com_levelPool, 0) CTargetChangeLevel;
	Temp->gameEntity->classname = "target_changelevel";

	Q_snprintfz (level.nextmap, sizeof(level.nextmap), "%s", map);
	Temp->gameEntity->map = level.nextmap;

	return Temp;
}
/*
=================
EndDMLevel

The timelimit or fraglimit has been exceeded
=================
*/
void EndDMLevel (void)
{
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

	if (*level.forcemap) {
		BeginIntermission (CreateTargetChangeLevel (level.forcemap) );
		return;
	}

	// see if it's in the map list
	if (*sv_maplist->String())
	{
		s = Mem_StrDup(sv_maplist->String());
		f = NULL;
#ifndef CRT_USE_UNDEPRECATED_FUNCTIONS
		t = strtok(s, seps);
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
				QDelete s;
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
	else
	{	// search for a changelevel
		CTargetChangeLevel *ent = dynamic_cast<CTargetChangeLevel*>(CC_Find (NULL, FOFS(classname), "target_changelevel"));
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
inline void CheckNeedPass ()
{
	// if password or spectator_password has changed, update needpass
	// as needed
	if (password->Modified() || spectator_password->Modified()) 
	{
		int need = 0;

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
	if (level.intermissiontime)
		return;

	if (!(game.mode & GAME_DEATHMATCH))
		return;

#ifdef CLEANCTF_ENABLED
//ZOID
	if ((game.mode & GAME_CTF) && CTFCheckRules())
	{
		EndDMLevel ();
		return;
	}
	if (CTFInMatch())
		return; // no checking in match mode
//ZOID
#endif

	if (timelimit->Float())
	{
		if (level.framenum >= ((timelimit->Float()*60)*10))
		{
			BroadcastPrintf (PRINT_HIGH, "Timelimit hit.\n");
			EndDMLevel ();
			return;
		}
	}

	if (fraglimit->Integer())
	{
		for (int i=0 ; i<game.maxclients ; i++)
		{
			CPlayerEntity *cl = dynamic_cast<CPlayerEntity*>(g_edicts[i+1].Entity);
			if (!cl->IsInUse())
				continue;

			if (cl->Client.resp.score >= fraglimit->Integer())
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
	char	command [256];

	Q_snprintfz (command, sizeof(command), "gamemap \"%s\"\n", level.changemap);
	gi.AddCommandString (command);
	level.changemap = NULL;
	level.exitintermission = 0;
	level.intermissiontime = 0;
	ClientEndServerFrames ();

	// clear some things before going to next level
	for (i=0 ; i<game.maxclients ; i++)
	{
		CPlayerEntity *ent = dynamic_cast<CPlayerEntity*>(g_edicts[1 + i].Entity);
		if (!ent->IsInUse())
			continue;
		if (ent->Health > ent->Client.pers.max_health)
			ent->Health = ent->Client.pers.max_health;
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

#if defined (_M_IX86)
#define _DbgBreak() __asm { int 3 }
#elif defined (_M_IA64)
void __break(int);
#pragma intrinsic (__break)
#define _DbgBreak() __break(0x80016)
#else  /* defined (_M_IA64) */
#define _DbgBreak() DebugBreak()
#endif  /* defined (_M_IA64) */

extern bool requestedBreak;
void RunFrame ()
{
	if (requestedBreak)
	{
		requestedBreak = false;
		_DbgBreak();
	}

	if (level.framenum >= 3 && map_debug->Boolean())
	{
		level.framenum ++;
		// Run the players only
		// build the playerstate_t structures for all players
		ClientEndServerFrames ();

#ifdef MONSTERS_USE_PATHFINDING
		RunNodes();
#endif
		return;
	}

	int		i;
	edict_t	*ent;

	level.framenum++;

	if (level.framenum == 2)
		EndMapCounter();

	// choose a client for monsters to target this frame
	// Only do it when we have spawned everything
	if (!(game.mode & GAME_DEATHMATCH) && level.framenum > 20) // Paril, lol
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

		if (ent->Entity)
		{
			CBaseEntity *Entity = ent->Entity;
			level.CurrentEntity = Entity;

			Entity->State.SetOldOrigin (Entity->State.GetOrigin());

			// if the ground entity moved, make sure we are still on it
			if ((Entity->GroundEntity) && ((!Entity->GroundEntity->gameEntity) || (Entity->GroundEntity->GetLinkCount() != Entity->GroundEntityLinkCount)))
			{
				Entity->GroundEntity = NULL;
				if ( !(Entity->Flags & (FL_SWIM|FL_FLY)) && (Entity->EntityFlags & ENT_MONSTER))
					(dynamic_cast<CMonsterEntity*>(Entity))->Monster->CheckGround ();
			}

			if (!Entity->Freed && (Entity->EntityFlags & ENT_THINKABLE)) 
				dynamic_cast<CThinkableEntity*>(Entity)->PreThink ();

			Entity->Run ();

			if (!Entity->Freed && (Entity->EntityFlags & ENT_THINKABLE))
				dynamic_cast<CThinkableEntity*>(Entity)->RunThink ();

			// Were we freed?
			// This has to be processed after thinking and running, because
			// the entity still has to be intact after that
			if (Entity->Freed)
			{
				QDelete Entity;
				ent->Entity = NULL;
			}
			continue;
		}
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

void G_RunFrame (void)
{
#ifdef CC_USE_EXCEPTION_HANDLER
	__try
	{
#endif
	RunFrame ();
#ifdef CC_USE_EXCEPTION_HANDLER
	}
	__except (EGLExceptionHandler(GetExceptionCode(), GetExceptionInformation()))
	{
		return;
	}
#endif
}

