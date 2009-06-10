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
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

// game.h
// - game dll information visible to server

#define GAME_APIVERSION		3

// edict->svFlags

#define SVF_NOCLIENT			0x00000001	// don't send entity to clients, even if it has effects
#define SVF_DEADMONSTER			0x00000002	// treat as CONTENTS_DEADMONSTER for collision
#define SVF_MONSTER				0x00000004	// treat as CONTENTS_MONSTER for collision
//ZOID
#define SVF_PROJECTILE			0x00000008  // entity is simple projectile, used for network optimization
// if an entity is projectile, the model index/x/y/z/pitch/yaw are sent, encoded into
// seven (or eight) bytes.  This is to speed up projectiles.  Currently, only the
// hyperblaster makes use of this.  use for items that are moving with a constant
// velocity that don't change direction or model
//ZOID

// edict->solid values
typedef enum {
	SOLID_NOT,			// no interaction with other objects
	SOLID_TRIGGER,		// only touch when inside, after moving
	SOLID_BBOX,			// touch on edge
	SOLID_BSP			// bsp clip, touch on edge
} solid_t;

// ==========================================================================

// link_t is only used for entity area links now
typedef struct link_s {
	struct link_s	*prev, *next;
} link_t;

#define MAX_ENT_CLUSTERS	16

#ifndef GAME_INCLUDE

typedef struct gclient_s {
	playerState_t		playerState;		// communicated by server to clients
	int					ping;
	// the game dll can add anything it wants after
	// this point in the structure
} gclient_t;

typedef struct edict_s {
	entityStateOld_t	s;
	struct gclient_s	*client;	// NULL if not a player
									// the server expects the first part
									// of gclient_s to be a player_state_t
									// but the rest of it is opaque
	BOOL				inUse;
	int					linkCount;

	// FIXME: move these fields to a server private sv_entity_t
	link_t				area;				// linked to a division node or leaf
	
	int					numClusters;		// if -1, use headnode instead
	int					clusterNums[MAX_ENT_CLUSTERS];
	int					headNode;			// unused if numClusters != -1
	int					areaNum, areaNum2;

	//================================

	int					svFlags;			// SVF_NOCLIENT, SVF_DEADMONSTER, SVF_MONSTER, etc
	vec3_t				mins, maxs;
	vec3_t				absMin, absMax, size;
	solid_t				solid;
	int					clipMask;
	struct edict_s		*owner;

	// the game dll can add anything it wants after
	// this point in the structure
} edict_t;

#else		// GAME_INCLUDE

typedef struct edict_s edict_t;
typedef struct gclient_s gclient_t;

#endif		// GAME_INCLUDE

// ==========================================================================

//
// functions provided by the main engine
//
typedef struct gameImport_s
{
	// special messages
#ifndef USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (BroadcastPrintf)
#endif
	void	(*bprintf) (EGamePrintLevel printlevel, char *fmt, ...);
#ifndef USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (DebugPrintf)
#endif
	void	(*dprintf) (char *fmt, ...);
#ifndef USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (ClientPrintf)
#endif
	void	(*cprintf) (edict_t *ent, EGamePrintLevel printLevel, char *fmt, ...);
#ifndef USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (CenterPrintf)
#endif
	void	(*centerprintf) (edict_t *ent, char *fmt, ...);

#ifndef USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (PlaySoundFrom)
#endif
	void	(*sound) (edict_t *ent, int channel, int soundIndex, float volume, float attenuation, float timeOffset);
#ifndef USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (PlaySoundAt)
#endif
	void	(*positioned_sound) (vec3_t origin, edict_t *ent, int channel, int soundIndex, float volume, float attenuation, float timeOffset);

	// config strings hold all the index strings, the lightstyles,
	// and misc data like the sky definition and cdtrack.
	// All of the current configstrings are sent to clients when
	// they connect, and changes are sent to all connected clients.
	void	(*configstring) (int num, char *string);

	void	(*error) (char *fmt, ...);

	// the *index functions create configstrings and some internal server state
#ifndef USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (ModelIndex)
#endif
	int		(*modelindex) (char *name);
#ifndef USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (SoundIndex)
#endif
	int		(*soundindex) (char *name);
#ifndef USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (ImageIndex)
#endif
	int		(*imageindex) (char *name);

#ifndef USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (SetModel)
#endif
	void	(*setmodel) (edict_t *ent, char *name);

	// collision detection
#ifndef USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (class CTrace)
#endif
	cmTrace_t	(*trace) (vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end, edict_t *passEnt, int contentMask);
	int			(*pointcontents) (vec3_t point);
	BOOL		(*inPVS) (vec3_t p1, vec3_t p2);
	BOOL		(*inPHS) (vec3_t p1, vec3_t p2);
	void		(*SetAreaPortalState) (int portalNum, BOOL open);
	BOOL		(*AreasConnected) (int area1, int area2);

	// an entity will never be sent to a client or used for collision
	// if it is not passed to linkentity.  If the size, position, or
	// solidity changes, it must be relinked.
	void	(*linkentity) (edict_t *ent);
	void	(*unlinkentity) (edict_t *ent);		// call before removing an interactive edict
	int		(*BoxEdicts) (vec3_t mins, vec3_t maxs, edict_t **list,	int maxCount, int areaType);
#ifndef USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (SV_Pmove)
#endif
	void	(*Pmove) (
#ifdef USE_EXTENDED_GAME_IMPORTS
	pMove_t *pMove
#else
	pMoveNew_t *pMove // Just to supress error
#endif
	);		// player movement code common with client prediction

	// network messaging
#ifndef USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (Cast)
#endif
	void	(*multicast) (vec3_t origin, EMultiCast to);

#ifndef USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (Cast)
#endif
	void	(*unicast) (edict_t *ent, BOOL reliable);

#ifndef USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (WriteChar)
#endif
	void	(*WriteChar) (int c);

#ifndef USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (WriteByte)
#endif
	void	(*WriteByte) (int c);

#ifndef USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (WriteShort)
#endif
	void	(*WriteShort) (int c);

#ifndef USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (WriteLong)
#endif
	void	(*WriteLong) (int c);

#ifndef USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (WriteFloat)
#endif
	void	(*WriteFloat) (float f);

#ifndef USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (WriteString)
#endif
	void	(*WriteString) (char *s);

#ifndef USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (WritePosition)
#endif
	void	(*WritePosition) (vec3_t pos);	// some fractional bits

#ifndef USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (WriteDir)
#endif
	void	(*WriteDir) (vec3_t pos);		// single byte encoded, very coarse

#ifndef USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (WriteAngle)
#endif
	void	(*WriteAngle) (float f);

	// managed memory allocation
	void	*(*TagMalloc) (int size, int tag);
	void	(*TagFree) (void *block);
	void	(*FreeTags) (int tag);

	// console variable interaction
#ifndef USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (class CCvar)
#endif
	cVar_t	*(*cvar) (char *varName, char *value, int flags);

#ifndef USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (class CCvar)
#endif
	cVar_t	*(*cvar_set) (char *varName, char *value);

#ifndef USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (class CCvar)
#endif
	cVar_t	*(*cvar_forceset) (char *varName, char *value);

	// ClientCommand and ServerCommand parameter access
#ifndef USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (ArgCount)
#endif
	int		(*argc) (void);

#ifndef USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (ArgGets/ArgGeti/ArgGetf)
#endif
	char	*(*argv) (int n);

#ifndef USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (ArgGetConcatenatedString)
#endif
	char	*(*args) (void);	// concatenation of all argv >= 1

	// add commands to the server console as if they were typed in
	// for map changing, etc
	void	(*AddCommandString) (char *text);

	void	(*DebugGraph) (float value, int color);
} gameImport_t;

//
// functions exported by the game subsystem
//
typedef struct gameExport_s {
	int			apiVersion;

	// the init function will only be called when a game starts,
	// not each time a level is loaded.  Persistant data for clients
	// and the server can be allocated in init
	void		(*Init) (void);
	void		(*Shutdown) (void);

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

	void		(*RunFrame) (void);

	// ServerCommand will be called when an "sv <command>" command is issued on the
	// server console.
	// The game can issue gi.argc() / gi.argv() commands to get the rest
	// of the parameters
	void		(*ServerCommand) (void);

	//
	// global variables shared between game and server
	//

	// The edict array is allocated in the game dll so it
	// can vary in size from one game to another.
	// 
	// The size will be fixed when ge->Init() is called
	struct edict_s	*edicts;
	int				edictSize;
	int				numEdicts;		// current number, <= MAX_CS_EDICTS
	int				maxEdicts;
} gameExport_t;

gameExport_t *GetGameApi (gameImport_t *import);

