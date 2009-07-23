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

void BeginIntermission (edict_t *targ)
{
	int		i, n;
	edict_t	*ent;

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
		CPlayerEntity *client = dynamic_cast<CPlayerEntity*>((g_edicts + 1 + i)->Entity);
		if (!client->IsInUse())
			continue;
		if (client->gameEntity->health <= 0)
			client->Respawn();
	}

	level.intermissiontime = level.framenum;
	level.changemap = targ->map;

	if (strstr(level.changemap, "*"))
	{
		if (game.mode == GAME_COOPERATIVE)
		{
			for (i=0 ; i<game.maxclients ; i++)
			{
				CPlayerEntity *client = dynamic_cast<CPlayerEntity*>((g_edicts + 1 + i)->Entity);
				if (!client->IsInUse())
					continue;
				// strip players of all keys between units
				for (n = 0; n < MAX_CS_ITEMS; n++)
				{
					if (n >= GetNumItems())
						break;
					if (GetItemByIndex(n)->Flags & ITEMFLAG_KEY)
						client->Client.pers.Inventory.Set(n, 0);
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
		CPlayerEntity *client = dynamic_cast<CPlayerEntity*>((g_edicts + 1 + i)->Entity);
		if (!client->IsInUse())
			continue;
		client->MoveToIntermission();
	}
}


/*
==================
DeathmatchScoreboard

Draw instead of help message.
Note that it isn't that hard to overflow the 1400 byte message limit!
==================
*/
void DeathmatchScoreboard (CPlayerEntity *ent)
{
	ent->DeathmatchScoreboardMessage (true);
}


/*
==================
Cmd_Score_f

Display the scoreboard
==================
*/
void Cmd_Score_f (CPlayerEntity *ent)
{
	ent->Client.showinventory = false;
	ent->Client.showhelp = false;

	if (ent->Client.resp.MenuState.InMenu)
	{
		ent->Client.resp.MenuState.CloseMenu();
		return;
	}

	if (game.mode == GAME_SINGLEPLAYER)
		return;

	if (ent->Client.showscores)
	{
		ent->Client.showscores = false;
		ent->Client.update_chase = true;
		return;
	}

	ent->Client.showscores = true;
	DeathmatchScoreboard (ent);
}

/*
==================
Cmd_Help_f

Display the current help message
==================
*/
void Cmd_Help_f (CPlayerEntity *ent)
{
	// this is for backwards compatability
	if (game.mode & GAME_DEATHMATCH)
	{
		Cmd_Score_f (ent);
		return;
	}

	ent->Client.showinventory = false;
	ent->Client.showscores = false;

	if (ent->Client.resp.MenuState.InMenu)
	{
		ent->Client.resp.MenuState.CloseMenu();
		return;
	}

	if (ent->Client.showhelp && (ent->Client.pers.game_helpchanged == game.helpchanged))
	{
		ent->Client.showhelp = false;
		return;
	}

	ent->Client.showhelp = true;
	ent->Client.pers.helpchanged = 0;
	HelpComputer (ent);
}
