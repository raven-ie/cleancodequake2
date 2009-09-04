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
// g_utils.c -- misc utility functions for game module

#include "g_local.h"

void G_ProjectSource (vec3_t point, vec3_t distance, vec3_t forward, vec3_t right, vec3_t result)
{
	result[0] = point[0] + forward[0] * distance[0] + right[0] * distance[1];
	result[1] = point[1] + forward[1] * distance[0] + right[1] * distance[1];
	result[2] = point[2] + forward[2] * distance[0] + right[2] * distance[1] + distance[2];
}

void G_InitEdict (edict_t *e)
{
	e->inUse = true;
	e->classname = "noclass";
	e->gravity = 1.0;
	e->state.number = e - g_edicts;
}

/*
=================
G_Spawn

Either finds a free edict, or allocates a new one.
Try to avoid reusing an entity that was recently freed, because it
can cause the client to think the entity morphed into something else
instead of being removed and recreated, which can cause interpolated
angles and bad trails.
=================
*/
edict_t *G_Spawn (void)
{
	int			i;
	edict_t		*e;

	e = &g_edicts[game.maxclients+1];
	for ( i=game.maxclients+1 ; i<globals.numEdicts ; i++, e++)
	{
		// the first couple seconds of server time can involve a lot of
		// freeing and allocating, so relax the replacement policy
		if (!e->inUse && ( e->freetime < 20 || level.framenum - e->freetime > 5 ) )
		{
			if (e->Entity && e->Entity->Freed)
			{
				QDelete e->Entity;
				e->Entity = NULL;
			}
			G_InitEdict (e);
			//DebugPrintf ("Entity %i reused\n", i);
			return e;
		}
	}
	
	if (i == game.maxentities)
		GameError ("ED_Alloc: no free edicts");
		
	//DebugPrintf ("Entity %i allocated\n", i);
	globals.numEdicts++;
	G_InitEdict (e);
	return e;
}

/*
=================
G_FreeEdict

Marks the edict as free
=================
*/
void G_FreeEdict (edict_t *ed)
{
	gi.unlinkentity (ed);		// unlink from world

	// Paril, hack
	CBaseEntity *Entity = ed->Entity;
	memset (ed, 0, sizeof(*ed));
	ed->Entity = Entity;
	ed->classname = "freed";
	ed->freetime = level.framenum;
	ed->inUse = false;
}
