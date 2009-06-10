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
======================================================================

INTERMISSION

======================================================================
*/

void MoveClientToIntermission (edict_t *ent)
{
	if (game.mode != GAME_SINGLEPLAYER)
		ent->client->showscores = true;
	Vec3Copy (level.intermission_origin, ent->state.origin);
	ent->client->playerState.pMove.origin[0] = level.intermission_origin[0]*8;
	ent->client->playerState.pMove.origin[1] = level.intermission_origin[1]*8;
	ent->client->playerState.pMove.origin[2] = level.intermission_origin[2]*8;
	Vec3Copy (level.intermission_angle, ent->client->playerState.viewAngles);
	ent->client->playerState.pMove.pmType = PMT_FREEZE;
	ent->client->playerState.gunIndex = 0;
	ent->client->playerState.viewBlend[3] = 0;
	ent->client->playerState.rdFlags &= ~RDF_UNDERWATER;

	// clean up powerup info
	ent->client->quad_framenum = 0;
	ent->client->invincible_framenum = 0;
	ent->client->breather_framenum = 0;
	ent->client->enviro_framenum = 0;
	ent->client->grenade_blew_up = ent->client->grenade_thrown = false;
	ent->client->grenade_time = 0;

	ent->viewheight = 0;
	ent->state.modelIndex = 0;
	ent->state.modelIndex2 = 0;
	ent->state.modelIndex3 = 0;
	ent->state.effects = 0;
	ent->state.sound = 0;
	ent->solid = SOLID_NOT;

	// add the layout

	if (game.mode != GAME_SINGLEPLAYER)
		DeathmatchScoreboardMessage (ent, NULL, true);
}

void BeginIntermission (edict_t *targ)
{
	int		i, n;
	edict_t	*ent, *client;

	if (level.intermissiontime)
		return;		// already activated

#ifdef CLEANCTF_ENABLED
//ZOID
	if (game.mode & GAME_CTF)
		CTFCalcScores();
//ZOID
#endif

	game.autosaved = false;

	// respawn any dead clients
	for (i=0 ; i<game.maxclients ; i++)
	{
		client = g_edicts + 1 + i;
		if (!client->inUse)
			continue;
		if (client->health <= 0)
			respawn(client);
	}

	level.intermissiontime = level.time;
	level.changemap = targ->map;

	if (strstr(level.changemap, "*"))
	{
		if (game.mode == GAME_COOPERATIVE)
		{
			for (i=0 ; i<game.maxclients ; i++)
			{
				client = g_edicts + 1 + i;
				if (!client->inUse)
					continue;
				// strip players of all keys between units
				for (n = 0; n < MAX_CS_ITEMS; n++)
				{
					if (n >= GetNumItems())
						break;
					if (GetItemByIndex(n)->Flags & ITEMFLAG_KEY)
						client->client->pers.Inventory.Set(n, 0);
				}
			}
		}
	}
	else
	{
		if (!(game.mode & GAME_DEATHMATCH))
		{
			level.exitintermission = 1;		// go immediately to the next level
			return;
		}
	}

	level.exitintermission = 0;

	// find an intermission spot
	ent = G_Find (NULL, FOFS(classname), "info_player_intermission");
	if (!ent)
	{	// the map creator forgot to put in an intermission point...
		ent = G_Find (NULL, FOFS(classname), "info_player_start");
		if (!ent)
			ent = G_Find (NULL, FOFS(classname), "info_player_deathmatch");
	}
	else
	{	// chose one of four spots
		i = rand() & 3;
		while (i--)
		{
			ent = G_Find (ent, FOFS(classname), "info_player_intermission");
			if (!ent)	// wrap around the list
				ent = G_Find (ent, FOFS(classname), "info_player_intermission");
		}
	}

	Vec3Copy (ent->state.origin, level.intermission_origin);
	Vec3Copy (ent->state.angles, level.intermission_angle);

	// move all clients to the intermission point
	for (i=0 ; i<game.maxclients ; i++)
	{
		client = g_edicts + 1 + i;
		if (!client->inUse)
			continue;
		MoveClientToIntermission (client);
	}
}


/*
==================
DeathmatchScoreboard

Draw instead of help message.
Note that it isn't that hard to overflow the 1400 byte message limit!
==================
*/
void DeathmatchScoreboard (edict_t *ent)
{
	DeathmatchScoreboardMessage (ent, ent->enemy, true);
}


/*
==================
Cmd_Score_f

Display the scoreboard
==================
*/
void Cmd_Score_f (edict_t *ent)
{
	ent->client->showinventory = false;
	ent->client->showhelp = false;

	if (ent->client->resp.MenuState.InMenu)
	{
		ent->client->resp.MenuState.CloseMenu();
		return;
	}

	if (game.mode == GAME_SINGLEPLAYER)
		return;

	if (ent->client->showscores)
	{
		ent->client->showscores = false;
		ent->client->update_chase = true;
		return;
	}

	ent->client->showscores = true;
	DeathmatchScoreboard (ent);
}

/*
==================
Cmd_Help_f

Display the current help message
==================
*/
void Cmd_Help_f (edict_t *ent)
{
	// this is for backwards compatability
	if (game.mode & GAME_DEATHMATCH)
	{
		Cmd_Score_f (ent);
		return;
	}

	ent->client->showinventory = false;
	ent->client->showscores = false;

	if (ent->client->resp.MenuState.InMenu)
	{
		ent->client->resp.MenuState.CloseMenu();
		return;
	}

	if (ent->client->showhelp && (ent->client->pers.game_helpchanged == game.helpchanged))
	{
		ent->client->showhelp = false;
		return;
	}

	ent->client->showhelp = true;
	ent->client->pers.helpchanged = 0;
	HelpComputer (ent);
}
