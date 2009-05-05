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
// cc_shared.h
// Shared header, included by g_local.
//

#include "cc_colors.h"
extern CColors Colors;
#include "cc_cvar.h"
#include "cc_dmflags.h"
#include "cc_trace.h"
#include "cc_tent.h"
#include "cc_sbar.h"
#include "cc_cmds.h"
#include "cc_items.h"
#include "cc_weaponmain.h"
#include "cc_inventory.h"
#include "cc_ban.h"
#include "cc_monsters.h"
#include "cc_monsterlist.h"
#include "cc_indexing.h"

extern dmFlagsConfig dmFlags;

// This is.. unnecessarily long.
void Sound (edict_t *ent, EEntSndChannel channel, int soundindex, float volume, int attenuation, float timeOfs);
void Sound (edict_t *ent, EEntSndChannel channel, char *soundString, float volume, int attenuation, float timeOfs);
void Sound (edict_t *ent, EEntSndChannel channel, int soundindex, float volume, int attenuation);
void Sound (edict_t *ent, EEntSndChannel channel, char *soundString, float volume, int attenuation);
void Sound (edict_t *ent, EEntSndChannel channel, int soundindex, float volume);
void Sound (edict_t *ent, EEntSndChannel channel, char *soundString, float volume);
void Sound (edict_t *ent, EEntSndChannel channel, int soundindex);
void Sound (edict_t *ent, EEntSndChannel channel, char *soundString);
void Sound (edict_t *ent, int soundindex, vec3_t pos);
void Sound (edict_t *ent, char *soundString, vec3_t pos);
void Sound (vec3_t pos, edict_t *ent, char *soundString);
void Sound (vec3_t pos, edict_t *ent, int soundindex);
void Sound (vec3_t pos, edict_t *ent, EEntSndChannel channel, char *soundString);
void Sound (vec3_t pos, edict_t *ent, EEntSndChannel channel, int soundindex);
void Sound (vec3_t pos, edict_t *ent, EEntSndChannel channel, char *soundString, float volume);
void Sound (vec3_t pos, edict_t *ent, EEntSndChannel channel, int soundindex, float volume);
void Sound (vec3_t pos, edict_t *ent, EEntSndChannel channel, char *soundString, float volume, int attenuation);
void Sound (vec3_t pos, edict_t *ent, EEntSndChannel channel, int soundindex, float volume, int attenuation);
void Sound (vec3_t pos, edict_t *ent, EEntSndChannel channel, char *soundString, float volume, int attenuation, float timeOfs);
void Sound (vec3_t pos, edict_t *ent, EEntSndChannel channel, int soundindex, float volume, int attenuation, float timeOfs);
void Sound (int soundindex);
void Sound (char *soundString);
void GI_SetModel (edict_t *ent, char *model);

void Cmd_RunCommand (char *commandName, edict_t *ent);
void Cmd_RemoveCommands ();
void Cmd_AddCommand (char *commandName, void (*Func) (edict_t *ent), ECmdTypeFlags Flags = CMD_NORMAL);

#if 0
void DrawNewton ();
#endif

enum EMapPrintType
{
	MAPPRINT_WARNING,
	MAPPRINT_ERROR,
	MAPPRINT_NORMAL
};

void Map_Print (EMapPrintType printType, edict_t *ent, vec3_t origin);
void MapPrint (EMapPrintType printType, edict_t *ent, vec3_t origin, char *fmt, ...);
void EndMapCounter ();
void InitMapCounter ();

//#define MapPrint(printType, ent, origin, format, ...) Map_Print(printType, ent, origin); gi.dprintf (format, ##__VA_ARGS__)
char *CC_ParseSpawnEntities (char *mapname, char *entities);

