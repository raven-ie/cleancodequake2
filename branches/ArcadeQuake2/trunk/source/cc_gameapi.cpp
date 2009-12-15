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
#include "cc_exceptionhandler.h"
#include "cc_gamecommands.h"
#include "cc_servercommands.h"

gameImport_t	gi;

EBrushContents PointContents (vec3f &start)
{
_CC_DISABLE_DEPRECATION
	return gi.pointcontents(start);
_CC_ENABLE_DEPRECATION};

sint32 BoxEdicts (vec3f &mins, vec3f &maxs, edict_t **list, sint32 maxCount, bool triggers)
{
_CC_DISABLE_DEPRECATION
	return gi.BoxEdicts (mins, maxs, list, maxCount, (triggers) ? AREA_TRIGGERS : AREA_SOLID);
_CC_ENABLE_DEPRECATION
}

void ConfigString (sint32 configStringIndex, const char *configStringValue, CPlayerEntity *Audience)
{
	if (Audience)
	{
		WriteChar (SVC_CONFIGSTRING);
		WriteShort (configStringIndex);
		WriteString (const_cast<char*>(configStringValue));
		Audience->CastTo (CASTFLAG_UNRELIABLE);
	}
	else
	{
_CC_DISABLE_DEPRECATION
		gi.configstring (configStringIndex, const_cast<char*>(configStringValue));
_CC_ENABLE_DEPRECATION
	}
}

void GameError (char *fmt, ...)
{
	va_list		argptr;
	static char	text[MAX_COMPRINT];

	va_start (argptr, fmt);
	vsnprintf_s (text, sizeof(text), MAX_COMPRINT, fmt, argptr);
	va_end (argptr);

	CC_ReportGameError (text);

_CC_DISABLE_DEPRECATION
	gi.error (text);
_CC_ENABLE_DEPRECATION
}

//
//
// Main Game API
//
//

bool CGameAPI::ClientConnect (CPlayerEntity *Player, char *userinfo)
{
	return Player->Connect (userinfo);
}

void CGameAPI::ClientBegin (CPlayerEntity *Player)
{
	Player->Begin ();
}

void CGameAPI::ClientUserinfoChanged (CPlayerEntity *Player, char *userInfo)
{
	Player->UserinfoChanged (userInfo);
}

void CGameAPI::ClientDisconnect (CPlayerEntity *Player)
{
	Player->Disconnect ();
}

void CGameAPI::ClientThink (CPlayerEntity *Player, userCmd_t *cmd)
{
	Player->ClientThink (cmd);
}

GAME_CLASS	Game;

/*
===========
ShutdownGame

called when the game is being unloaded.
============
*/
void ShutdownGame ()
{
#if CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_BEGIN
#endif

	Game.Shutdown ();

#if CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_END
#endif
}

/*
===========
ClientBegin

called when a client has finished connecting, and is ready
to be placed into the game.  This will happen every level load.
============
*/
void ClientBegin (edict_t *ent)
{
#if CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_BEGIN
#endif

	Game.ClientBegin (entity_cast<CPlayerEntity>(ent->Entity));

#if CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_END
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
#if CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_BEGIN
#endif

	Game.ClientUserinfoChanged (entity_cast<CPlayerEntity>(ent->Entity), userinfo);

#if CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_END
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
#if CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_BEGIN
#endif

	return (Game.ClientConnect (entity_cast<CPlayerEntity>(ent->Entity), userinfo) ? 1 : 0);

#if CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_END_CUSTOM
(
	return 0;
)
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
#if CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_BEGIN
#endif

	Game.ClientDisconnect (entity_cast<CPlayerEntity>(ent->Entity));

#if CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_END
#endif
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
#if CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_BEGIN
#endif

	Game.ClientThink (entity_cast<CPlayerEntity>(ent->Entity), ucmd);

#if CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_END
#endif
}

/*
================
RunFrame

Advances the world by 0.1 seconds
================
*/
void RunFrame ()
{
#if CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_BEGIN
#endif

	Game.RunFrame ();

#if CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_END
#endif
}

/*
============
InitGame

This will be called when the dll is first loaded, which
only happens when a new game is started or a save game
is loaded.
============
*/
void InitGame ()
{
#if CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_BEGIN
#endif

	Game.Init ();

#if CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_END
#endif
}

/*
==============
SpawnEntities

Creates a server's entity / program execution context by
parsing textual entity definitions out of an ent file.
==============
*/
void SpawnEntities (char *ServerLevelName, char *entities, char *spawnpoint)
{
#if CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_BEGIN
#endif

	Game.SpawnEntities (ServerLevelName, entities, spawnpoint);

#if CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_END
#endif
}

/*
=================
ServerCommand

ServerCommand will be called when an "sv" command is issued.
The game can issue gi.argc() / gi.argv() commands to get the rest
of the parameters
=================
*/
void ServerCommand ()
{
#if CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_BEGIN
#endif

	Game.ServerCommand ();

#if CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_END
#endif
}

/*
=================
ClientCommand

game commands issued by clients
=================
*/
void ClientCommand (edict_t *ent)
{
#if CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_BEGIN
#endif
	if (!ent->client || !ent->Entity || (entity_cast<CPlayerEntity>(ent->Entity)->Client.Persistent.State != SVCS_SPAWNED))
		return;		// not fully in game yet

	Game.ClientCommand (entity_cast<CPlayerEntity>(ent->Entity));

#if CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_END
#endif
}

/*
============
WriteGame

This will be called whenever the game goes to a new level,
and when the user explicitly saves the game.

Game information include cross level data, like multi level
triggers, help computer info, and all client states.

A single player death will automatically restore from the
last save position.
============
*/
void WriteGame (char *filename, BOOL autosave)
{
#if CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_BEGIN
#endif

	Game.WriteGame (filename, !!autosave);

#if CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_END
#endif
}

void ReadGame (char *filename)
{
#if CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_BEGIN
#endif

	Game.ReadGame (filename);

#if CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_END
#endif
}

//==========================================================

/*
=================
WriteLevel

=================
*/
void WriteLevel (char *filename)
{
#if CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_BEGIN

#endif
	Game.WriteLevel (filename);

#if CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_END
#endif
}


/*
=================
ReadLevel

SpawnEntities will allready have been called on the
level the same way it was when the level was saved.

That is necessary to get the baselines
set up identically.

The server will have cleared all of the world links before
calling ReadLevel.

No clients are connected yet.
=================
*/
void ReadLevel (char *filename)
{
#if CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_BEGIN
#endif

	Game.ReadLevel (filename);

#if CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_END
#endif
}

/*
=================
GetGameAPI

Returns a pointer to the structure with all entry points
and global variables
=================
*/

//
// functions exported by the game subsystem
//
struct gameExport_t
{
	sint32			apiVersion;

	// the init function will only be called when a game starts,
	// not each time a level is loaded.  Persistant data for clients
	// and the server can be allocated in init
	void		(*Init) ();
	void		(*Shutdown) ();

	// each new level entered will cause a call to SpawnEntities
	void		(*SpawnEntities) (char *mapName, char *entString, char *spawnPoint);

	// Read/Write Game is for storing persistant cross level information
	// about the world state and the clients.
	// WriteGame is called every time a level is exited.
	// ReadGame is called on a loadgame.
	void		(*WriteGame) (char *fileName, BOOL autoSave);
	void		(*ReadGame) (char *fileName);

	// ReadLevel is called after the default map information has been
	// loaded with SpawnEntities
	void		(*WriteLevel) (char *filename);
	void		(*ReadLevel) (char *filename);

	BOOL		(*ClientConnect) (edict_t *ent, char *userInfo);
	void		(*ClientBegin) (edict_t *ent);
	void		(*ClientUserinfoChanged) (edict_t *ent, char *userInfo);
	void		(*ClientDisconnect) (edict_t *ent);
	void		(*ClientCommand) (edict_t *ent);
	void		(*ClientThink) (edict_t *ent, userCmd_t *cmd);

	void		(*RunFrame) ();

	// ServerCommand will be called when an "sv <command>" command is issued on the
	// server console.
	// The game can issue gi.argc() / gi.argv() commands to get the rest
	// of the parameters
	void		(*ServerCommand) ();

	//
	// global variables shared between game and server
	//

	// The edict array is allocated in the game dll so it
	// can vary in size from one game to another.
	// 
	// The size will be fixed when ge->Init() is called
	edict_t		*edicts;
	sint32		edictSize;
	sint32		numEdicts;		// current number, <= MAX_CS_EDICTS
	sint32		maxEdicts;
} globals =
{
	GAME_APIVERSION,
	InitGame,
	ShutdownGame,
	SpawnEntities,
	WriteGame,
	ReadGame,
	WriteLevel,
	ReadLevel,
	ClientConnect,
	ClientBegin,
	ClientUserinfoChanged,
	ClientDisconnect,
	ClientCommand,
	ClientThink,
	RunFrame,
	ServerCommand,
	NULL,
	sizeof(edict_t),
	0,
	0
};

edict_t *CGameAPI::GetEntities ()
{
	return globals.edicts;
};

void CGameAPI::SetEntities (edict_t *entities)
{
	globals.edicts = entities;
};

sint32 &CGameAPI::GetEdictSize ()
{
	return globals.edictSize;
};

sint32 &CGameAPI::GetNumEdicts()
{
	return globals.numEdicts;
};

sint32 &CGameAPI::GetMaxEdicts()
{
	return globals.maxEdicts;
};

gameExport_t *GetGameAPI (gameImport_t *import)
{
	gi = *import;

	Swap_Init ();
	return &globals;
}
