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

/*
===============
ED_CallSpawn

Finds the spawn function for the entity and calls it
===============
*/
void ED_CallSpawn (edict_t *ent)
{
	if (!ent->classname)
	{
		//gi.dprintf ("ED_CallSpawn: NULL classname\n");
		MapPrint (MAPPRINT_ERROR, ent, ent->state.origin, "NULL classname!\n");
		return;
	}

	// Check CleanCode stuff
	CBaseEntity *MapEntity = ResolveMapEntity(ent);

	if (!MapEntity)
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
	int		c = 0, c2 = 0;

	CBaseEntity *e, *e2;
	int i, j;
	for (i = 1, e = g_edicts[i].Entity; i < globals.numEdicts; i++, e = g_edicts[i].Entity)
	{
		if (!e)
			continue;
		if (!e->IsInUse())
			continue;
		if (!e->gameEntity->team)
			continue;
		if (e->Flags & FL_TEAMSLAVE)
			continue;

		CBaseEntity *chain = e;
		e->TeamMaster = e;

		c++;
		c2++;
		for (j = i + 1, e2 = g_edicts[j].Entity; j < globals.numEdicts; j++, e2 = g_edicts[j].Entity)
		{
			if (!e2)
				continue;
			if (!e2->IsInUse())
				continue;
			if (!e2->gameEntity->team)
				continue;
			if (e2->Flags & FL_TEAMSLAVE)
				continue;

			if (!strcmp(e->gameEntity->team, e2->gameEntity->team))
			{
				c2++;
				chain->TeamChain = e2;
				e2->TeamMaster = e;

				chain = e2;
				e2->Flags |= FL_TEAMSLAVE;
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

void InitPlayers ()
{
	// Set up the client entities
	for (int i = 1; i <= game.maxclients; i++)
	{
		edict_t *ent = &g_edicts[i];

		if (!ent->Entity)
			ent->Entity = QNew (com_levelPool, 0) CPlayerEntity(i);
	}
}

void InitEntities ()
{
	// Set up the world
	edict_t *theWorld = &g_edicts[0];
	if (!theWorld->Entity)
		theWorld->Entity = QNew (com_levelPool, 0) CWorldEntity(0);

	InitPlayers();
}

extern clientPersistent_t *SavedClients;

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

	InitEntities ();

	// set client fields on player ents
	for (i=0 ; i<game.maxclients ; i++)
	{
		// Reset the entity states
		//g_edicts[i+1].Entity = SavedClients[i];
		CPlayerEntity *Player = dynamic_cast<CPlayerEntity*>(g_edicts[i+1].Entity);
		memcpy (&Player->Client.pers, &SavedClients[i], sizeof(clientPersistent_t));
		g_edicts[i+1].client = game.clients + i;
	}

	QDelete[] SavedClients;
	SavedClients = NULL;

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

		if (!ent->inUse)
			inhibit++;
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

#ifdef MONSTERS_USE_PATHFINDING
	LoadNodes();
	LoadPathTable ();
#endif

	SetupTechSpawn();

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
