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

#define Function(f) {#f, f}

field_t fields[] = {
	{"classname", FOFS(classname), F_LSTRING},
	{"model", FOFS(model), F_LSTRING},
	{"spawnflags", FOFS(spawnflags), F_INT},
	{"speed", FOFS(speed), F_FLOAT},
	{"accel", FOFS(accel), F_FLOAT},
	{"decel", FOFS(decel), F_FLOAT},
	{"target", FOFS(target), F_LSTRING},
	{"targetname", FOFS(targetname), F_LSTRING},
	{"pathtarget", FOFS(pathtarget), F_LSTRING},
	{"deathtarget", FOFS(deathtarget), F_LSTRING},
	{"killtarget", FOFS(killtarget), F_LSTRING},
	{"combattarget", FOFS(combattarget), F_LSTRING},
	{"message", FOFS(message), F_LSTRING},
	{"team", FOFS(team), F_LSTRING},
	{"wait", FOFS(wait), F_FLOAT},
	{"delay", FOFS(delay), F_FLOAT},
	{"random", FOFS(random), F_FLOAT},
	{"move_origin", FOFS(move_origin), F_VECTOR},
	{"move_angles", FOFS(move_angles), F_VECTOR},
	{"style", FOFS(style), F_INT},
	{"count", FOFS(count), F_INT},
	{"health", FOFS(health), F_INT},
	{"sounds", FOFS(sounds), F_INT},
	{"light", 0, F_IGNORE},
	{"dmg", FOFS(dmg), F_INT},
	{"mass", FOFS(mass), F_INT},
	{"volume", FOFS(volume), F_FLOAT},
	{"attenuation", FOFS(attenuation), F_FLOAT},
	{"map", FOFS(map), F_LSTRING},
	{"origin", FOFS(state.origin), F_VECTOR},
	{"angles", FOFS(state.angles), F_VECTOR},
	{"angle", FOFS(state.angles), F_ANGLEHACK},

	{"goalentity", FOFS(goalentity), F_EDICT, FFL_NOSPAWN},
	{"movetarget", FOFS(movetarget), F_EDICT, FFL_NOSPAWN},
	{"enemy", FOFS(enemy), F_EDICT, FFL_NOSPAWN},
	{"oldenemy", FOFS(oldenemy), F_EDICT, FFL_NOSPAWN},
	{"activator", FOFS(activator), F_EDICT, FFL_NOSPAWN},
	{"groundentity", FOFS(groundentity), F_EDICT, FFL_NOSPAWN},
	{"teamchain", FOFS(teamchain), F_EDICT, FFL_NOSPAWN},
	{"teammaster", FOFS(teammaster), F_EDICT, FFL_NOSPAWN},
	{"owner", FOFS(owner), F_EDICT, FFL_NOSPAWN},
	{"mynoise", FOFS(mynoise), F_EDICT, FFL_NOSPAWN},
	{"mynoise2", FOFS(mynoise2), F_EDICT, FFL_NOSPAWN},
	{"target_ent", FOFS(target_ent), F_EDICT, FFL_NOSPAWN},
	{"chain", FOFS(chain), F_EDICT, FFL_NOSPAWN},

	{"prethink", FOFS(prethink), F_FUNCTION, FFL_NOSPAWN},
	{"think", FOFS(think), F_FUNCTION, FFL_NOSPAWN},
	{"blocked", FOFS(blocked), F_FUNCTION, FFL_NOSPAWN},
	{"touch", FOFS(touch), F_FUNCTION, FFL_NOSPAWN},
	{"use", FOFS(use), F_FUNCTION, FFL_NOSPAWN},
	{"pain", FOFS(pain), F_FUNCTION, FFL_NOSPAWN},
	{"die", FOFS(die), F_FUNCTION, FFL_NOSPAWN},

	{"endfunc", FOFS(moveinfo.endfunc), F_FUNCTION, FFL_NOSPAWN},

	// temp spawn vars -- only valid when the spawn function is called
	{"lip", STOFS(lip), F_INT, FFL_SPAWNTEMP},
	{"distance", STOFS(distance), F_INT, FFL_SPAWNTEMP},
	{"height", STOFS(height), F_INT, FFL_SPAWNTEMP},
	{"noise", STOFS(noise), F_LSTRING, FFL_SPAWNTEMP},
	{"pausetime", STOFS(pausetime), F_FLOAT, FFL_SPAWNTEMP},
	{"item", STOFS(item), F_LSTRING, FFL_SPAWNTEMP},

//need for item field in edict struct, FFL_SPAWNTEMP item will be skipped on saves
	{"item", FOFS(item), F_ITEM},

	{"gravity", STOFS(gravity), F_LSTRING, FFL_SPAWNTEMP},
	{"sky", STOFS(sky), F_LSTRING, FFL_SPAWNTEMP},
	{"skyrotate", STOFS(skyrotate), F_FLOAT, FFL_SPAWNTEMP},
	{"skyaxis", STOFS(skyaxis), F_VECTOR, FFL_SPAWNTEMP},
	{"minyaw", STOFS(minyaw), F_FLOAT, FFL_SPAWNTEMP},
	{"maxyaw", STOFS(maxyaw), F_FLOAT, FFL_SPAWNTEMP},
	{"minpitch", STOFS(minpitch), F_FLOAT, FFL_SPAWNTEMP},
	{"maxpitch", STOFS(maxpitch), F_FLOAT, FFL_SPAWNTEMP},
	{"nextmap", STOFS(nextmap), F_LSTRING, FFL_SPAWNTEMP},

	{0, 0, F_IGNORE, 0}

};

field_t		levelfields[] =
{
	{"changemap", LLOFS(changemap), F_LSTRING},
                   
	{"sight_client", LLOFS(sight_client), F_EDICT},
//	{"sight_entity", LLOFS(sight_entity), F_EDICT},
//	{"sound_entity", LLOFS(sound_entity), F_EDICT},
//	{"sound2_entity", LLOFS(sound2_entity), F_EDICT},

	{NULL, 0, F_INT}
};

field_t		clientfields[] =
{
	{"pers.Weapon", CLOFS(pers.Weapon), F_NEWITEM},
	//{"pers.lastweapon", CLOFS(pers.lastweapon), F_ITEM},
	{"NewWeapon", CLOFS(NewWeapon), F_NEWITEM},

	{NULL, 0, F_INT}
};

void SetupGamemode ()
{
	int dmInt = deathmatch->Integer(),
		coopInt = coop->Integer();
#ifdef CLEANCTF_ENABLED
	int ctfInt = ctf->Integer();
#endif

	// Did we request deathmatch?
	if (dmInt)
	{
		// Did we also request coop?
		if (coopInt)
		{
			// Which one takes priority?
			if (dmInt > coopInt)
			{
				// We want deathmatch
				coop->Set (0, false);
				// Let it fall through
			}
			else if (coopInt > dmInt)
			{
				// We want coop
				deathmatch->Set (0, false);
				game.mode = GAME_COOPERATIVE;
				return;
			}
			// We don't know what we want, forcing DM
			else
			{
				coop->Set (0, false);
				DebugPrintf		("CleanCode Warning: Both deathmatch and coop are 1; forcing to deathmatch.\n"
								 "Did you know you can make one take priority if you intend to only set one?\n"
								 "If deathmatch is 1 and you want to switch to coop, just type \"coop 2\" and change maps!\n");
				// Let it fall through
			}
		}
		game.mode = GAME_DEATHMATCH;
	}
	// Did we request cooperative?
	else if (coopInt)
	{
		// All the above code handles the case if deathmatch is true.
		game.mode = GAME_COOPERATIVE;
		return;
	}
	else
	{
		game.mode = GAME_SINGLEPLAYER;
		return;
	}

	// If we reached here, we wanted deathmatch
#ifdef CLEANCTF_ENABLED
	if (ctfInt)
		game.mode |= GAME_CTF;
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
// Registers all cvars and commands
void G_Register ()
{
	gun_x = new CCvar ("gun_x", "0", 0);
	gun_y = new CCvar ("gun_y", "0", 0);
	gun_z = new CCvar ("gun_z", "0", 0);

	//FIXME: sv_ prefix is wrong for these
	sv_rollspeed = new CCvar ("sv_rollspeed", "200", 0);
	sv_rollangle = new CCvar ("sv_rollangle", "2", 0);
	sv_gravity = new CCvar ("sv_gravity", "800", 0);

	// noset vars
	dedicated = new CCvar ("dedicated", "0", CVAR_READONLY);

	// latched vars
	sv_cheats = new CCvar ("cheats", "0", CVAR_SERVERINFO|CVAR_LATCH_SERVER);
	CCvar ("gamename", GAMEVERSION , CVAR_SERVERINFO|CVAR_LATCH_SERVER);
	CCvar ("gamedate", __DATE__ , CVAR_SERVERINFO|CVAR_LATCH_SERVER);

	maxclients = new CCvar ("maxclients", "4", CVAR_SERVERINFO | CVAR_LATCH_SERVER);
	maxspectators = new CCvar ("maxspectators", "4", CVAR_SERVERINFO);
	skill = new CCvar ("skill", "1", CVAR_LATCH_SERVER);
	maxentities = new CCvar ("maxentities", 1024, CVAR_LATCH_SERVER);

	// change anytime vars
	dmflags = new CCvar ("dmflags", "0", CVAR_SERVERINFO);
	fraglimit = new CCvar ("fraglimit", "0", CVAR_SERVERINFO);
	timelimit = new CCvar ("timelimit", "0", CVAR_SERVERINFO);
	password = new CCvar ("password", "", CVAR_USERINFO);
	spectator_password = new CCvar ("spectator_password", "", CVAR_USERINFO);
	needpass = new CCvar ("needpass", "0", CVAR_SERVERINFO);
	filterban = new CCvar ("filterban", "1", 0);

	g_select_empty = new CCvar ("g_select_empty", "0", CVAR_ARCHIVE);

	run_pitch = new CCvar ("run_pitch", "0.002", 0);
	run_roll = new CCvar ("run_roll", "0.005", 0);
	bob_up  = new CCvar ("bob_up", "0.005", 0);
	bob_pitch = new CCvar ("bob_pitch", "0.002", 0);
	bob_roll = new CCvar ("bob_roll", "0.002", 0);

	// flood control
	flood_msgs = new CCvar ("flood_msgs", "4", 0);
	flood_persecond = new CCvar ("flood_persecond", "4", 0);
	flood_waitdelay = new CCvar ("flood_waitdelay", "10", 0);

	// dm map list
	sv_maplist = new CCvar ("sv_maplist", "", 0);

	SetupArg ();
	Cmd_Register ();
	SvCmd_Register ();

#ifdef MONSTERS_USE_PATHFINDING
	InitNodes ();
#endif

	// Gamemodes
	deathmatch = new CCvar ("deathmatch", "0", CVAR_SERVERINFO|CVAR_LATCH_SERVER);
	coop = new CCvar ("coop", "0", CVAR_LATCH_SERVER);

#ifdef CLEANCTF_ENABLED
//ZOID
	capturelimit = new CCvar ("capturelimit", "0", CVAR_SERVERINFO);
	instantweap = new CCvar ("instantweap", "0", CVAR_SERVERINFO);
//ZOID
#endif
}

void InitGame (void)
{
	DebugPrintf ("==== InitGame ====\n");

	// Register cvars/commands
	G_Register();

	// items
	InitItemlist ();

	// monsters
	InitMonsterList();

	Q_snprintfz (game.helpmessage1, sizeof(game.helpmessage1), "");

	Q_snprintfz (game.helpmessage2, sizeof(game.helpmessage2), "");

	// initialize all entities for this game
	game.maxentities = maxentities->Integer();
	g_edicts = (edict_t*)gi.TagMalloc (game.maxentities * sizeof(g_edicts[0]), TAG_GAME);
	globals.edicts = g_edicts;
	globals.maxEdicts = game.maxentities;

	// initialize all clients for this game
	game.maxclients = maxclients->Integer();
	game.clients = (gclient_t*)gi.TagMalloc (game.maxclients * sizeof(game.clients[0]), TAG_GAME);
	globals.numEdicts = game.maxclients+1;

	// Vars
	game.maxspectators = maxspectators->Integer();
	game.cheats = (sv_cheats->Integer()) ? true : false;

	DebugPrintf ("Running CleanCode Quake2, built on %s (%s %s)\n", __DATE__, BUILDSTRING, CPUSTRING);

	Bans.LoadFromFile ();

#ifdef CLEANCTF_ENABLED
	// Setup CTF if we have it
	CTFInit();
#endif

	// Setup the gamemode
	SetupGamemode ();
}

//=========================================================

void WriteField1 (FILE *f, field_t *field, byte *base)
{
	void		*p;
	int			len;
	int			index;

	if (field->flags & FFL_SPAWNTEMP)
		return;

	p = (void *)(base + field->ofs);
	switch (field->type)
	{
	case F_INT:
	case F_FLOAT:
	case F_ANGLEHACK:
	case F_VECTOR:
	case F_IGNORE:
		break;

	case F_LSTRING:
	case F_GSTRING:
		if ( *(char **)p )
			len = strlen(*(char **)p) + 1;
		else
			len = 0;
		*(int *)p = len;
		break;
	case F_EDICT:
		if ( *(edict_t **)p == NULL)
			index = -1;
		else
			index = *(edict_t **)p - g_edicts;
		*(int *)p = index;
		break;
	case F_CLIENT:
		if ( *(gclient_t **)p == NULL)
			index = -1;
		else
			index = *(gclient_t **)p - game.clients;
		*(int *)p = index;
		break;
	case F_ITEM:
/*		if ( *(edict_t **)p == NULL)
			index = -1;
		else
			index = *(gitem_t **)p - itemlist;
		*(int *)p = index;*/
		break;
	case F_NEWITEM:
		if ( *(edict_t **)p == NULL)
			index = -1;
		else
		{
			CWeapon *Weap = static_cast<CWeapon *>(p);
			CBaseItem *Item = Weap->Item;
			index = Item->GetIndex();
		}
		*(int *)p = -1;
		break;

	//relative to code segment
	case F_FUNCTION:
		if (*(byte **)p == NULL)
			index = 0;
		else
			index = *(byte **)p - ((byte *)InitGame);
		*(int *)p = index;
		break;

	default:
		gi.error ("WriteEdict: unknown field type");
	}
}


void WriteField2 (FILE *f, field_t *field, byte *base)
{
	int			len;
	void		*p;

	if (field->flags & FFL_SPAWNTEMP)
		return;

	p = (void *)(base + field->ofs);
	switch (field->type)
	{
	case F_LSTRING:
		if ( *(char **)p )
		{
			len = strlen(*(char **)p) + 1;
			fwrite (*(char **)p, len, 1, f);
		}
		break;
	}
}

void ReadField (FILE *f, field_t *field, byte *base)
{
	void		*p;
	int			len;
	int			index;

	if (field->flags & FFL_SPAWNTEMP)
		return;

	p = (void *)(base + field->ofs);
	switch (field->type)
	{
	case F_INT:
	case F_FLOAT:
	case F_ANGLEHACK:
	case F_VECTOR:
	case F_IGNORE:
		break;

	case F_LSTRING:
		len = *(int *)p;
		if (!len)
			*(char **)p = NULL;
		else
		{
			*(char **)p = (char*)gi.TagMalloc (len, TAG_LEVEL);
			fread (*(char **)p, len, 1, f);
		}
		break;
	case F_EDICT:
		index = *(int *)p;
		if ( index == -1 )
			*(edict_t **)p = NULL;
		else
			*(edict_t **)p = &g_edicts[index];
		break;
	case F_CLIENT:
		index = *(int *)p;
		if ( index == -1 )
			*(gclient_t **)p = NULL;
		else
			*(gclient_t **)p = &game.clients[index];
		break;
	case F_ITEM:
/*		index = *(int *)p;
		if ( index == -1 )
			*(gitem_t **)p = NULL;
		else
			*(gitem_t **)p = &itemlist[index];*/
		break;
	case F_NEWITEM:
		index = *(int *)p;
		if ( index == -1 )
			*(CWeapon **)p = NULL;
		else
		{
		}
		break;
	//relative to code segment
	case F_FUNCTION:
		index = *(int *)p;
		if ( index == 0 )
			*(byte **)p = NULL;
		else
			*(byte **)p = ((byte *)InitGame) + index;
		break;
	default:
		gi.error ("ReadEdict: unknown field type");
	}
}

//=========================================================

/*
==============
WriteClient

All pointer variables (except function pointers) must be handled specially.
==============
*/
void WriteClient (FILE *f, gclient_t *client)
{
//	field_t		*field;
	gclient_t	temp;
	
	// all of the ints, floats, and vectors stay as they are
	temp = *client;

	// change the pointers to lengths or indexes
	//for (field=clientfields ; field->name ; field++)
	//{
	//	WriteField1 (f, field, (byte *)&temp);
	//}
	// Write pers.weapon and pers.newweapon
	int pwIndex = -1, nwIndex = -1;

	if (client->pers.Weapon)
	{
		CBaseItem *Item = client->pers.Weapon->Item;
		pwIndex = Item->GetIndex();
	}
	if (client->NewWeapon)
	{
		CBaseItem *Item = client->NewWeapon->Item;
		nwIndex = Item->GetIndex();
	}

	// write the block
	fwrite (&temp, sizeof(temp), 1, f);

	// now write any allocated data following the edict
	/*for (field=clientfields ; field->name ; field++)
	{
		WriteField2 (f, field, (byte *)client);
	}*/
	fwrite (&pwIndex, sizeof(int), 1, f);
	fwrite (&nwIndex, sizeof(int), 1, f);
}

/*
==============
ReadClient

All pointer variables (except function pointers) must be handled specially.
==============
*/
void ReadClient (FILE *f, gclient_t *client)
{
//	field_t		*field;

	fread (client, sizeof(*client), 1, f);

	/*for (field=clientfields ; field->name ; field++)
	{
		ReadField (f, field, (byte *)client);
	}*/
	int pwIndex, nwIndex;
	fread (&pwIndex, sizeof(int), 1, f);
	fread (&nwIndex, sizeof(int), 1, f);

	if (pwIndex != -1)
	{
		CBaseItem *It = GetItemByIndex (pwIndex);
		if (It->Flags & ITEMFLAG_WEAPON)
		{
			if (It->Flags & ITEMFLAG_AMMO)
			{
				CAmmo *Ammo = static_cast<CAmmo*>(It);
				client->pers.Weapon = Ammo->Weapon;
			}
			else
			{
				CWeaponItem *Weapon = static_cast<CWeaponItem*>(It);
				client->pers.Weapon = Weapon->Weapon;
			}
		}
	}
	else
		client->pers.Weapon = NULL;

	if (nwIndex != -1)
	{
		CBaseItem *It = GetItemByIndex (nwIndex);
		if (It->Flags & ITEMFLAG_WEAPON)
		{
			if (It->Flags & ITEMFLAG_AMMO)
			{
				CAmmo *Ammo = static_cast<CAmmo*>(It);
				client->NewWeapon = Ammo->Weapon;
			}
			else
			{
				CWeaponItem *Weapon = static_cast<CWeaponItem*>(It);
				client->NewWeapon = Weapon->Weapon;
			}
		}
	}
	else
		client->NewWeapon = NULL;
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
	FILE	*f;
	int		i;
	char	str[16];

	if (!autosave)
		SaveClientData ();

#ifndef CRT_USE_UNDEPRECATED_FUNCTIONS
	f = fopen (filename, "wb");
#else
	int errorVal = fopen_s(&f, filename, "wb");
#endif

	if (!f || errorVal)
		gi.error ("Couldn't open %s", filename);

	memset (str, 0, sizeof(str));
	Q_strncpyz (str, __DATE__, sizeof(str));
	fwrite (str, sizeof(str), 1, f);

	game.autosaved = autosave ? true : false;
	fwrite (&game, sizeof(game), 1, f);
	game.autosaved = false;

	for (i=0 ; i<game.maxclients ; i++)
		WriteClient (f, &game.clients[i]);

	fclose (f);
}

void ReadGame (char *filename)
{
	FILE	*f;
	int		i;
	char	str[16];

	gi.FreeTags (TAG_GAME);
	gi.FreeTags (TAG_CLEAN_GAME);

#ifndef CRT_USE_UNDEPRECATED_FUNCTIONS
	f = fopen (filename, "rb");
#else
	int errorVal = fopen_s(&f, filename, "rb");
#endif

	if (!f || errorVal)
		gi.error ("Couldn't open %s", filename);

	fread (str, sizeof(str), 1, f);
	if (strcmp (str, __DATE__))
	{
		fclose (f);
		gi.error ("Savegame from an older version.\n");
	}

	g_edicts = (edict_t*)gi.TagMalloc (game.maxentities * sizeof(g_edicts[0]), TAG_GAME);
	globals.edicts = g_edicts;

	fread (&game, sizeof(game), 1, f);
	game.clients = (gclient_t*)gi.TagMalloc (game.maxclients * sizeof(game.clients[0]), TAG_GAME);
	for (i=0 ; i<game.maxclients ; i++)
		ReadClient (f, &game.clients[i]);

	fclose (f);
}

//==========================================================


/*
==============
WriteEdict

All pointer variables (except function pointers) must be handled specially.
==============
*/
void WriteEdict (FILE *f, edict_t *ent)
{
	field_t		*field;
	edict_t		temp;

	// all of the ints, floats, and vectors stay as they are
	temp = *ent;

	// change the pointers to lengths or indexes
	for (field=fields ; field->name ; field++)
	{
		WriteField1 (f, field, (byte *)&temp);
	}

	// write the block
	fwrite (&temp, sizeof(temp), 1, f);

	// now write any allocated data following the edict
	for (field=fields ; field->name ; field++)
	{
		WriteField2 (f, field, (byte *)ent);
	}

}

/*
==============
WriteLevelLocals

All pointer variables (except function pointers) must be handled specially.
==============
*/
void WriteLevelLocals (FILE *f)
{
	field_t		*field;
	level_locals_t		temp;

	// all of the ints, floats, and vectors stay as they are
	temp = level;

	// change the pointers to lengths or indexes
	for (field=levelfields ; field->name ; field++)
	{
		WriteField1 (f, field, (byte *)&temp);
	}

	// write the block
	fwrite (&temp, sizeof(temp), 1, f);

	// now write any allocated data following the edict
	for (field=levelfields ; field->name ; field++)
	{
		WriteField2 (f, field, (byte *)&level);
	}
}


/*
==============
ReadEdict

All pointer variables (except function pointers) must be handled specially.
==============
*/
void ReadEdict (FILE *f, edict_t *ent)
{
	field_t		*field;

	fread (ent, sizeof(*ent), 1, f);

	for (field=fields ; field->name ; field++)
	{
		ReadField (f, field, (byte *)ent);
	}
}

/*
==============
ReadLevelLocals

All pointer variables (except function pointers) must be handled specially.
==============
*/
void ReadLevelLocals (FILE *f)
{
	field_t		*field;

	fread (&level, sizeof(level), 1, f);

	for (field=levelfields ; field->name ; field++)
	{
		ReadField (f, field, (byte *)&level);
	}
}

/*
=================
WriteLevel

=================
*/
void WriteLevel (char *filename)
{
	int		i;
	edict_t	*ent;
	FILE	*f;
	void	*base;

#ifndef CRT_USE_UNDEPRECATED_FUNCTIONS
	f = fopen (filename, "wb");
#else
	int errorVal = fopen_s(&f, filename, "wb");
#endif

	if (!f || errorVal)
		gi.error ("Couldn't open %s", filename);

	// write out edict size for checking
	i = sizeof(edict_t);
	fwrite (&i, sizeof(i), 1, f);

	// write out a function pointer for checking
	base = (void *)InitGame;
	fwrite (&base, sizeof(base), 1, f);

	// write out level_locals_t
	WriteLevelLocals (f);

	// write out all the entities
	for (i=0 ; i<globals.numEdicts ; i++)
	{
		ent = &g_edicts[i];
		if (!ent->inUse)
			continue;
		fwrite (&i, sizeof(i), 1, f);
		WriteEdict (f, ent);
	}
	i = -1;
	fwrite (&i, sizeof(i), 1, f);

	fclose (f);
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
	int		entNum;
	FILE	*f;
	int		i;
	void	*base;
	edict_t	*ent;

#ifndef CRT_USE_UNDEPRECATED_FUNCTIONS
	f = fopen (filename, "rb");
#else
	int errorVal = fopen_s(&f, filename, "rb");
#endif

	if (!f || errorVal)
		gi.error ("Couldn't open %s", filename);

	// free any dynamic memory allocated by loading the level
	// base state
	gi.FreeTags (TAG_LEVEL);

	// wipe all the entities
	memset (g_edicts, 0, game.maxentities*sizeof(g_edicts[0]));
	globals.numEdicts = game.maxclients+1;

	// check edict size
	fread (&i, sizeof(i), 1, f);
	if (i != sizeof(edict_t))
	{
		fclose (f);
		gi.error ("ReadLevel: mismatched edict size");
	}

	// check function pointer base address
	fread (&base, sizeof(base), 1, f);
#ifdef _WIN32
	if (base != (void *)InitGame)
	{
		fclose (f);
		gi.error ("ReadLevel: function pointers have moved");
	}
#else
	gi.dprintf("Function offsets %d\n", ((byte *)base) - ((byte *)InitGame));
#endif

	// load the level locals
	ReadLevelLocals (f);

	// load all the entities
	while (1)
	{
		if (fread (&entNum, sizeof(entNum), 1, f) != 1)
		{
			fclose (f);
			gi.error ("ReadLevel: failed to read entNum");
		}
		if (entNum == -1)
			break;
		if (entNum >= globals.numEdicts)
			globals.numEdicts = entNum+1;

		ent = &g_edicts[entNum];
		ReadEdict (f, ent);

		// let the server rebuild world links for this ent
		memset (&ent->area, 0, sizeof(ent->area));
		gi.linkentity (ent);
	}

	fclose (f);

	// mark all clients as unconnected
	for (i=0 ; i<game.maxclients ; i++)
	{
		ent = &g_edicts[i+1];
		ent->client = game.clients + i;
		ent->client->pers.state = SVCS_FREE;
	}

	// do any load time things at this point
	for (i=0 ; i<globals.numEdicts ; i++)
	{
		ent = &g_edicts[i];

		if (!ent->inUse)
			continue;

		// fire any cross-level triggers
		if (ent->classname)
			if (strcmp(ent->classname, "target_crosslevel_target") == 0)
				ent->nextthink = level.time + ent->delay;
	}
}
