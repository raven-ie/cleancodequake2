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

typedef struct
{
	char	*name;
	void	(*spawn)(edict_t *ent);
} spawn_t;


/*void SP_item_health (edict_t *self);
void SP_item_health_small (edict_t *self);
void SP_item_health_large (edict_t *self);
void SP_item_health_mega (edict_t *self);*/

void SP_info_player_start (edict_t *ent);
void SP_info_player_deathmatch (edict_t *ent);
void SP_info_player_coop (edict_t *ent);
void SP_info_player_intermission (edict_t *ent);

void SP_func_plat (edict_t *ent);
void SP_func_rotating (edict_t *ent);
void SP_func_button (edict_t *ent);
void SP_func_door (edict_t *ent);
void SP_func_door_secret (edict_t *ent);
void SP_func_door_rotating (edict_t *ent);
void SP_func_water (edict_t *ent);
void SP_func_train (edict_t *ent);
void SP_func_conveyor (edict_t *self);
void SP_func_wall (edict_t *self);
void SP_func_object (edict_t *self);
void SP_func_explosive (edict_t *self);
void SP_func_timer (edict_t *self);
void SP_func_areaportal (edict_t *ent);
void SP_func_clock (edict_t *ent);
void SP_func_killbox (edict_t *ent);

void SP_trigger_always (edict_t *ent);
void SP_trigger_once (edict_t *ent);
void SP_trigger_multiple (edict_t *ent);
void SP_trigger_relay (edict_t *ent);
void SP_trigger_push (edict_t *ent);
void SP_trigger_hurt (edict_t *ent);
void SP_trigger_key (edict_t *ent);
void SP_trigger_counter (edict_t *ent);
void SP_trigger_elevator (edict_t *ent);
void SP_trigger_gravity (edict_t *ent);
void SP_trigger_teleport (edict_t *ent);
void SP_trigger_monsterjump (edict_t *ent);

void SP_target_temp_entity (edict_t *ent);
void SP_target_speaker (edict_t *ent);
void SP_target_explosion (edict_t *ent);
void SP_target_changelevel (edict_t *ent);
void SP_target_secret (edict_t *ent);
void SP_target_goal (edict_t *ent);
void SP_target_splash (edict_t *ent);
void SP_target_spawner (edict_t *ent);
void SP_target_blaster (edict_t *ent);
void SP_target_crosslevel_trigger (edict_t *ent);
void SP_target_crosslevel_target (edict_t *ent);
void SP_target_laser (edict_t *self);
void SP_target_help (edict_t *self);
void SP_target_actor (edict_t *self);
void SP_target_location (edict_t *self);
void SP_target_position (edict_t *self);
void SP_target_lightramp (edict_t *self);
void SP_target_earthquake (edict_t *ent);
void SP_target_character (edict_t *ent);
void SP_target_string (edict_t *ent);

void SP_worldspawn (edict_t *ent);
void SP_viewthing (edict_t *ent);

void SP_light (edict_t *self);
void SP_light_mine1 (edict_t *ent);
void SP_light_mine2 (edict_t *ent);
void SP_info_null (edict_t *self);
void SP_info_notnull (edict_t *self);
void SP_path_corner (edict_t *self);
void SP_point_combat (edict_t *self);

void SP_misc_banner (edict_t *self);
void SP_misc_satellite_dish (edict_t *self);
void SP_misc_actor (edict_t *self);
void SP_misc_gib_arm (edict_t *self);
void SP_misc_gib_leg (edict_t *self);
void SP_misc_gib_head (edict_t *self);
void SP_misc_insane (edict_t *self);
void SP_misc_deadsoldier (edict_t *self);
void SP_misc_viper (edict_t *self);
void SP_misc_viper_bomb (edict_t *self);
void SP_misc_bigviper (edict_t *self);
void SP_misc_strogg_ship (edict_t *self);
void SP_misc_teleporter (edict_t *self);
void SP_misc_teleporter_dest (edict_t *self);
void SP_misc_blackhole (edict_t *self);
void SP_misc_eastertank (edict_t *self);
void SP_misc_easterchick (edict_t *self);
void SP_misc_easterchick2 (edict_t *self);
void SP_misc_model (edict_t *self);

void SP_turret_breach (edict_t *self);
void SP_turret_base (edict_t *self);
//void SP_turret_driver (edict_t *self);
void SP_monster_boss3_stand (edict_t *self);

spawn_t	spawns[] = {
	//{"target_actor", SP_target_actor},

//	{"viewthing", SP_viewthing},

	//{"misc_actor", SP_misc_actor},

	{NULL, NULL}
};

/*
===============
ED_CallSpawn

Finds the spawn function for the entity and calls it
===============
*/
void ED_CallSpawn (edict_t *ent)
{
	spawn_t	*s;
	if (!ent->classname)
	{
		//gi.dprintf ("ED_CallSpawn: NULL classname\n");
		MapPrint (MAPPRINT_ERROR, ent, ent->state.origin, "NULL classname!\n");
		return;
	}

	// check item spawn functions
//	if (ItemExists(ent))
//		return;

/*
	if ((ent->Monster = FindMonster(ent->classname)) != NULL)
	{
		ent->Monster->Allocate(ent);
		ent->Monster->Init (ent);
		return;
	}
	else
		ent->Monster = NULL;
*/

	// Check CleanCode stuff
	CBaseEntity *MapEntity = ResolveMapEntity(ent);
	if (MapEntity != NULL)
		return;

	// check normal spawn functions
	for (s=spawns ; s->name ; s++)
	{
		if (!strcmp(s->name, ent->classname))
		{	// found it
			s->spawn (ent);
			return;
		}
	}
	//gi.dprintf ("%s doesn't have a spawn function\n", ent->classname);
	MapPrint (MAPPRINT_ERROR, ent, ent->state.origin, "Invalid entity (no spawn function)\n");
}

/*
=============
ED_NewString
=============
*/
char *ED_NewString (char *string)
{
	size_t l = strlen(string) + 1;
	char *newb = QNew (com_levelPool, 0) char[l];
	char *new_p = newb;

	for (size_t i=0 ; i< l ; i++)
	{
		if (string[i] == '\\' && i < l-1)
		{
			i++;
			if (string[i] == 'n')
				*new_p++ = '\n';
			else
				*new_p++ = '\\';
		}
		else
			*new_p++ = string[i];
	}
	
	return newb;
}


/*
===============
ED_ParseField

Takes a key/value pair and sets the binary values
in an edict
===============
*/
static void ED_ParseField (char *key, char *value, edict_t *ent)
{
	field_t	*f;
	byte	*b;
	float	v;
	vec3_t	vec;

	for (f=fields ; f->name ; f++)
	{
		if (!(f->flags & FFL_NOSPAWN) && !Q_stricmp(f->name, key))
		{	// found it
			if (f->flags & FFL_SPAWNTEMP)
				b = (byte *)&st;
			else
				b = (byte *)ent;

			switch (f->type)
			{
			case F_LSTRING:
				*(char **)(b+f->ofs) = ED_NewString (value);
				break;
			case F_VECTOR:
				sscanf_s (value, "%f %f %f", &vec[0], &vec[1], &vec[2]);
				((float *)(b+f->ofs))[0] = vec[0];
				((float *)(b+f->ofs))[1] = vec[1];
				((float *)(b+f->ofs))[2] = vec[2];
				break;
			case F_INT:
				*(int *)(b+f->ofs) = atoi(value);
				break;
			case F_FLOAT:
				*(float *)(b+f->ofs) = atof(value);
				break;
			case F_ANGLEHACK:
				v = atof(value);
				((float *)(b+f->ofs))[0] = 0;
				((float *)(b+f->ofs))[1] = v;
				((float *)(b+f->ofs))[2] = 0;
				break;
			case F_IGNORE:
				break;
			}
			return;
		}
	}
	//gi.dprintf ("%s is not a field\n", key);
	MapPrint (MAPPRINT_ERROR, ent, ent->state.origin, "\"%s\" is not a field\n", key);
}

/*
====================
ED_ParseEdict

Parses an edict out of the given string, returning the new position
ed should be a properly initialized empty edict.
====================
*/
static char *ED_ParseEdict (char *data, edict_t *ent)
{
	bool	init;
	char	keyName[256];
	char	*token;

	init = false;
	memset (&st, 0, sizeof(st));

	// Go through all the dictionary pairs
	for ( ; ; ) {
		// Parse key
		token = Com_Parse (&data);
		if (token[0] == '}')
			break;
		if (!data)
			GameError ("ED_ParseEntity: EOF without closing brace");

		Q_strncpyz (keyName, token, sizeof(keyName));
		
		// Parse value	
		token = Com_Parse (&data);
		if (!data)
			GameError ("ED_ParseEntity: EOF without closing brace");
		if (token[0] == '}')
			GameError ("ED_ParseEntity: closing brace without data");

		init = true;	

		// Keynames with a leading underscore are used for utility comments,
		// and are immediately discarded by quake
		if (keyName[0] == '_')
			continue;

		ED_ParseField (keyName, token, ent);
	}

	if (!init)
		memset (ent, 0, sizeof(*ent));

	return data;
}


/*
================
G_FindTeams

Chain together all entities with a matching team field.

All but the first will have the FL_TEAMSLAVE flag set.
All but the last will have the teamchain field set to the next one
================
*/
void G_FindTeams (void)
{
	edict_t	*e, *e2, *chain;
	int		i, j;
	int		c, c2;

	c = 0;
	c2 = 0;
	for (i=1, e=g_edicts+i ; i < globals.numEdicts ; i++,e++)
	{
		if (!e->inUse)
			continue;
		if (!e->team)
			continue;
		if (e->flags & FL_TEAMSLAVE)
			continue;
		chain = e;
		e->teammaster = e;
		c++;
		c2++;
		for (j=i+1, e2=e+1 ; j < globals.numEdicts ; j++,e2++)
		{
			if (!e2->inUse)
				continue;
			if (!e2->team)
				continue;
			if (e2->flags & FL_TEAMSLAVE)
				continue;
			if (!strcmp(e->team, e2->team))
			{
				c2++;
				chain->teamchain = e2;
				e2->teammaster = e;
				chain = e2;
				e2->flags |= FL_TEAMSLAVE;
			}
		}
	}

	DebugPrintf ("%i teams with %i entities\n", c, c2);
}


/*
==============
SpawnEntities

Creates a server's entity / program execution context by
parsing textual entity definitions out of an ent file.
==============
*/
extern int entityNumber;
#include "cc_exceptionhandler.h"
#include "cc_brushmodels.h"

void InitEntities ()
{
	// Set up the world
	edict_t *theWorld = &g_edicts[0];
	if (!theWorld->Entity)
		theWorld->Entity = QNew (com_levelPool, 0) CWorldEntity(0);

	// Set up the client entities
	for (int i = 1; i <= game.maxclients; i++)
	{
		edict_t *ent = &g_edicts[i];

		if (!ent->Entity)
			ent->Entity = QNew (com_gamePool, 0) CPlayerEntity(i);
	}
}

extern CPlayerEntity **SavedClients;

char *gEntString;
void SpawnEntities (char *mapname, char *entities, char *spawnpoint)
{
#ifdef CC_USE_EXCEPTION_HANDLER
__try
{
#endif
	edict_t		*ent;
	int			inhibit;
	char		*token;
	int			i;
	int		skill_level;
	uint32 startTime = Sys_Milliseconds();

	entityNumber = 0;
	InitMapCounter();

	skill_level = skill->Integer();
	if (skill_level < 0)
		skill_level = 0;
	if (skill_level > 3)
		skill_level = 3;
	if (skill->Integer() != skill_level)
		skill->Set(Q_VarArgs("%i", skill_level), true);

	CPlayerEntity::SaveClientData ();

	Mem_FreePool (com_levelPool);
	gEntString = Mem_PoolStrDup(entities, com_levelPool, 0);

	entities = CC_ParseSpawnEntities (mapname, entities);

#ifdef MONSTERS_USE_PATHFINDING
	InitNodes ();
#endif

	memset (&level, 0, sizeof(level));
	memset (g_edicts, 0, game.maxentities * sizeof(g_edicts[0]));

	Q_strncpyz (level.mapname, mapname, sizeof(level.mapname)-1);
	Q_strncpyz (game.spawnpoint, spawnpoint, sizeof(game.spawnpoint)-1);

	// set client fields on player ents
	for (i=0 ; i<game.maxclients ; i++)
	{
		// Reset the entity states
		g_edicts[i+1].Entity = SavedClients[i];
		g_edicts[i+1].client = game.clients + i;
	}

	QDelete[] SavedClients;

	InitEntities ();

	ent = NULL;
	inhibit = 0;

	// Parse ents
	for ( ; ; ) {
		// Parse the opening brace
		token = Com_Parse (&entities);
		if (!entities)
			break;
		if (token[0] != '{')
			GameError ("ED_LoadFromFile: found %s when expecting {", token);

		if (!ent)
			ent = g_edicts;
		else
			ent = G_Spawn ();
		entities = ED_ParseEdict (entities, ent);

		// Yet another map hack
		if (!Q_stricmp(level.mapname, "command") && !Q_stricmp(ent->classname, "trigger_once") && !Q_stricmp(ent->model, "*27"))
			ent->spawnflags &= ~SPAWNFLAG_NOT_HARD;

		entityNumber++;
		// Remove things (except the world) from different skill levels or deathmatch
		if (ent != g_edicts)
		{
			if (game.mode & GAME_DEATHMATCH)
			{
				if ( ent->spawnflags & SPAWNFLAG_NOT_DEATHMATCH )
				{
					G_FreeEdict (ent);	
					inhibit++;
					continue;
				}
			}
			else
			{
				if ( /* ((game.mode == GAME_COOPERATIVE) && (ent->spawnflags & SPAWNFLAG_NOT_COOP)) || */
					((skill->Integer() == 0) && (ent->spawnflags & SPAWNFLAG_NOT_EASY)) ||
					((skill->Integer() == 1) && (ent->spawnflags & SPAWNFLAG_NOT_MEDIUM)) ||
					(((skill->Integer() == 2) || (skill->Integer() == 3)) && (ent->spawnflags & SPAWNFLAG_NOT_HARD))
					)
					{
						G_FreeEdict (ent);	
						inhibit++;
						continue;
					}
			}

			ent->spawnflags &= ~(SPAWNFLAG_NOT_EASY|SPAWNFLAG_NOT_MEDIUM|SPAWNFLAG_NOT_HARD|SPAWNFLAG_NOT_COOP|SPAWNFLAG_NOT_DEATHMATCH);
		}

		ED_CallSpawn (ent);
	}

	DebugPrintf ("%i entities removed\n", inhibit);

#ifdef DEBUG
	i = 1;
	ent = EDICT_NUM(i);
	while (i < globals.numEdicts) {
		if (ent->inUse != 0 || ent->inUse != 1)
			Com_DevPrintf ("Invalid entity %d\n", i);
		i++, ent++;
	}
#endif

	G_FindTeams ();

	PlayerTrail_Init ();

#ifdef MONSTERS_USE_PATHFINDING
	LoadNodes();
	LoadPathTable ();
#endif

#ifdef CLEANCTF_ENABLED
//ZOID
	CTFSpawn();
//ZOID
#endif

	DebugPrintf ("Finished server initialization in %d ms\n", Sys_Milliseconds() - startTime);
#ifdef CC_USE_EXCEPTION_HANDLER
	}
	__except (EGLExceptionHandler(GetExceptionCode(), GetExceptionInformation()))
	{
		return;
	}
#endif
}


//===================================================================
