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
// cc_gamecommands.cpp
// ClientCommand & related functions
//

#include "cc_local.h"
#include "m_player.h"

char *ClientTeam (edict_t *ent)
{
	char		*p;
	// Paril todo fixme, this sucks.
	static char	value[MAX_INFO_STRING];

	value[0] = 0;

	if (!ent->client)
		return value;

	Q_strncpyz(value, Info_ValueForKey (ent->client->pers.userinfo, "skin"), MAX_INFO_STRING);
	p = strchr(value, '/');
	if (!p)
		return value;

	if (dmFlags.dfModelTeams)
	{
		*p = 0;
		return value;
	}

	// if ((int)(dmflags->floatVal) & DF_SKINTEAMS)
	return ++p;
}

bool OnSameTeam (edict_t *ent1, edict_t *ent2)
{
	if (!(dmFlags.dfSkinTeams || dmFlags.dfModelTeams))
		return false;

	if (strcmp(ClientTeam (ent1), ClientTeam (ent2)) == 0)
		return true;
	return false;
}

/*
==================
Cmd_God_f

Sets client to godmode

argv(0) god
==================
*/
void Cmd_God_f (edict_t *ent)
{
	ent->flags ^= FL_GODMODE;
	ClientPrintf (ent, PRINT_HIGH, "God mode %s\n", (!(ent->flags & FL_GODMODE)) ? "off" : "on");
}


/*
==================
Cmd_Notarget_f

Sets client to notarget

argv(0) notarget
==================
*/
void Cmd_Notarget_f (edict_t *ent)
{
	ent->flags ^= FL_NOTARGET;
	ClientPrintf (ent, PRINT_HIGH, "Notarget %s\n", (!(ent->flags & FL_NOTARGET)) ? "off" : "on");
}


/*
==================
Cmd_Noclip_f

argv(0) noclip
==================
*/
void Cmd_Noclip_f (edict_t *ent)
{
	ent->movetype = (ent->movetype == MOVETYPE_NOCLIP) ? MOVETYPE_WALK : MOVETYPE_NOCLIP;
	ClientPrintf (ent, PRINT_HIGH, "Noclip %s\n", (ent->movetype == MOVETYPE_NOCLIP) ? "on" : "off");
}

/*
=================
Cmd_Kill_f
=================
*/
void Cmd_Kill_f (edict_t *ent)
{
	if((level.time - ent->client->respawn_time) < 5)
		return;
	ent->flags &= ~FL_GODMODE;
	ent->health = 0;
	meansOfDeath = MOD_SUICIDE;
	player_die (ent, ent, ent, 100000, vec3Origin);
}

/*
=================
Cmd_PutAway_f
=================
*/
void Cmd_PutAway_f (edict_t *ent)
{
	ent->client->showscores = false;
	ent->client->showhelp = false;
	ent->client->showinventory = false;
}


int PlayerSort (void const *a, void const *b)
{
	int		anum, bnum;

	anum = *(int *)a;
	bnum = *(int *)b;

	anum = game.clients[anum].ps.stats[STAT_FRAGS];
	bnum = game.clients[bnum].ps.stats[STAT_FRAGS];

	if (anum < bnum)
		return -1;
	if (anum > bnum)
		return 1;
	return 0;
}

/*
=================
Cmd_Players_f
=================
*/
void Cmd_Players_f (edict_t *ent)
{
	int		i;
	int		count;
	char	small[MAX_INFO_KEY];
	char	large[MAX_INFO_STRING];
	int		*index = new int[maxclients->Integer()];

	count = 0;
	for (i = 0 ; i < maxclients->Integer() ; i++)
	{
		if (game.clients[i].pers.state >= SVCS_CONNECTED)
		{
			index[count] = i;
			count++;
		}
	}

	// sort by frags
	qsort (index, count, sizeof(index[0]), PlayerSort);

	// print information
	large[0] = 0;

	for (i = 0 ; i < count ; i++)
	{
		Q_snprintfz (small, sizeof(small), "%3i %s\n",
			game.clients[index[i]].ps.stats[STAT_FRAGS],
			game.clients[index[i]].pers.netname);
		if (strlen (small) + strlen(large) > sizeof(large) - 100 )
		{	// can't print all of them in one packet
			Q_strcatz (large, "...\n", MAX_INFO_STRING);
			break;
		}
		Q_strcatz (large, small, MAX_INFO_STRING);
	}

	ClientPrintf (ent, PRINT_HIGH, "%s\n%i players\n", large, count);

	delete[] index;
}

/*
=================
Cmd_Wave_f
=================
*/
void Cmd_Wave_f (edict_t *ent)
{
	// can't wave when ducked
	if (ent->client->ps.pMove.pmFlags & PMF_DUCKED)
		return;

	if (ent->client->anim_priority > ANIM_WAVE)
		return;

	ent->client->anim_priority = ANIM_WAVE;

	switch (ArgGeti(1))
	{
	case 0:
		ClientPrintf (ent, PRINT_HIGH, "flipoff\n");
		ent->s.frame = FRAME_flip01-1;
		ent->client->anim_end = FRAME_flip12;
		break;
	case 1:
		ClientPrintf (ent, PRINT_HIGH, "salute\n");
		ent->s.frame = FRAME_salute01-1;
		ent->client->anim_end = FRAME_salute11;
		break;
	case 2:
		ClientPrintf (ent, PRINT_HIGH, "taunt\n");
		ent->s.frame = FRAME_taunt01-1;
		ent->client->anim_end = FRAME_taunt17;
		break;
	case 3:
		ClientPrintf (ent, PRINT_HIGH, "wave\n");
		ent->s.frame = FRAME_wave01-1;
		ent->client->anim_end = FRAME_wave11;
		break;
	case 4:
	default:
		ClientPrintf (ent, PRINT_HIGH, "point\n");
		ent->s.frame = FRAME_point01-1;
		ent->client->anim_end = FRAME_point12;
		break;
	}
}

/*
==================
Cmd_Say_f
==================
*/
#define MAX_TALK_STRING 100

void Cmd_Say_f (edict_t *ent, bool team, bool arg0)
{
	int		i, j;
	edict_t	*other;
	char	*p;
	char	text[MAX_TALK_STRING];
	gclient_t *cl;

	if (ArgCount () < 2 && !arg0)
		return;

	if (Bans.IsSquelched(ent->client->pers.IP) || Bans.IsSquelched(ent->client->pers.netname))
	{
		ClientPrintf (ent, PRINT_HIGH, "You are squelched and may not talk.\n");
		return;
	}

	if (!(dmFlags.dfSkinTeams || dmFlags.dfModelTeams))
		team = false;

	if (team)
		Q_snprintfz (text, sizeof(text), "(%s): ", ent->client->pers.netname);
	else
		Q_snprintfz (text, sizeof(text), "%s: ", ent->client->pers.netname);

	if (arg0)
	{
		Q_strcatz (text, ArgGets(0), sizeof(text));
		Q_strcatz (text, " ", sizeof(text));
		Q_strcatz (text, ArgGetConcatenatedString(), sizeof(text));
	}
	else
	{
		p = ArgGetConcatenatedString();

		if (*p == '"')
		{
			p++;
			p[strlen(p)-1] = 0;
		}
		Q_strcatz(text, p, sizeof(text));
	}

	// don't let text be too long for malicious reasons
	if (strlen(text) > sizeof(text))
		text[sizeof(text)] = 0;

	Q_strcatz(text, "\n", sizeof(text));

	if (flood_msgs->Integer())
	{
		cl = ent->client;

        if (level.time < cl->flood_locktill)
		{
			ClientPrintf(ent, PRINT_HIGH, "You can't talk for %d more seconds\n",
				(int)(cl->flood_locktill - level.time));
            return;
        }
        i = cl->flood_whenhead - flood_msgs->Integer() + 1;
        if (i < 0)
            i = (sizeof(cl->flood_when)/sizeof(cl->flood_when[0])) + i;
		if (cl->flood_when[i] && 
			level.time - cl->flood_when[i] < flood_persecond->Float())
		{
			cl->flood_locktill = level.time + flood_waitdelay->Float();
			ClientPrintf(ent, PRINT_CHAT, "Flood protection:  You can't talk for %d seconds.\n",
				flood_waitdelay->Integer());
            return;
        }
		cl->flood_whenhead = (cl->flood_whenhead + 1) %
			(sizeof(cl->flood_when)/sizeof(cl->flood_when[0]));
		cl->flood_when[cl->flood_whenhead] = level.time;
	}

	if (dedicated->Integer())
		ClientPrintf(NULL, PRINT_CHAT, "%s", text);

	for (j = 1; j <= game.maxclients; j++)
	{
		other = &g_edicts[j];
		if (!other->inUse)
			continue;
		if (!other->client)
			continue;
		if (team)
		{
			if (!OnSameTeam(ent, other))
				continue;
		}
		ClientPrintf(other, PRINT_CHAT, "%s", text);
	}
}

void Cmd_PlayerList_f(edict_t *ent)
{
	int i;
	char st[80];
	char text[MAX_COMPRINT/4];
	edict_t *e2;

	// connect time, ping, score, name
	*text = 0;

	Q_snprintfz (text, sizeof(text), "Spawned:\n");
	for (i = 0, e2 = g_edicts + 1; i < maxclients->Integer(); i++, e2++)
	{
		if (!e2->inUse)
			continue;
		if (e2->client->pers.state != SVCS_SPAWNED)
			continue;

		Q_snprintfz(st, sizeof(st), " - %02d:%02d %4d %3d %s%s\n",
			(level.framenum - e2->client->resp.enterframe) / 600,
			((level.framenum - e2->client->resp.enterframe) % 600)/10,
			e2->client->ping,
			e2->client->resp.score,
			e2->client->pers.netname,
			e2->client->resp.spectator ? " (spectator)" : "");
		if (strlen(text) + strlen(st) > sizeof(text) - 50) {
			Q_snprintfz (text+strlen(text), sizeof(text), "And more...\n");
			ClientPrintf(ent, PRINT_HIGH, "%s", text);
			return;
		}
		Q_strcatz(text, st, sizeof(text));
	}

	Q_strcatz (text, "Connecting:\n", sizeof(text));
	for (i = 0, e2 = g_edicts + 1; i < maxclients->Integer(); i++, e2++)
	{
		if (!e2->inUse)
			continue;
		if (e2->client->pers.state == SVCS_SPAWNED)
			continue;

		Q_snprintfz(st, sizeof(st), " - %s%s\n",
			e2->client->pers.netname,
			e2->client->resp.spectator ? " (spectator)" : "");
		if (strlen(text) + strlen(st) > sizeof(text) - 50) {
			Q_snprintfz (text+strlen(text), sizeof(text), "And more...\n");
			ClientPrintf(ent, PRINT_HIGH, "%s", text);
			return;
		}
		Q_strcatz(text, st, sizeof(text));
	}
	ClientPrintf(ent, PRINT_HIGH, "%s", text);
}

void GCmd_Say_f (edict_t *ent)
{
	Cmd_Say_f (ent, false, false);
}

void GCmd_SayTeam_f (edict_t *ent)
{
	Cmd_Say_f (ent, true, false);
}

void Cmd_Test_f (edict_t *ent)
{
	char *sound = ArgGets(1);

	if (!sound || !sound[0])
		return;

	gi.configstring (CS_SOUNDS+70, sound);
	PlaySoundFrom (ent, CHAN_AUTO, 70);
}

void Cmd_Register ()
{
	// These commands are generic, and can be executed any time
	// during play, even during intermission and by spectators.
	Cmd_AddCommand ("players",				Cmd_Players_f,			CMD_SPECTATOR);
	Cmd_AddCommand ("say",					GCmd_Say_f,				CMD_SPECTATOR);
	Cmd_AddCommand ("score",				Cmd_Score_f,			CMD_SPECTATOR);
	Cmd_AddCommand ("help",					Cmd_Help_f,				CMD_SPECTATOR);
	Cmd_AddCommand ("putaway",				Cmd_PutAway_f,			CMD_SPECTATOR);
	Cmd_AddCommand ("playerlist",			Cmd_PlayerList_f,		CMD_SPECTATOR);

	// These commands are also generic, but can only be executed
	// by in-game players during the game
	Cmd_AddCommand ("say_team",				GCmd_SayTeam_f);
	Cmd_AddCommand ("kill",					Cmd_Kill_f);
	Cmd_AddCommand ("wave",					Cmd_Wave_f);

	Cmd_AddCommand ("use",					Cmd_Use_f);
	Cmd_AddCommand ("drop",					Cmd_Drop_f);
	Cmd_AddCommand ("inven",				Cmd_Inven_f);
	Cmd_AddCommand ("invuse",				Cmd_InvUse_f);
	Cmd_AddCommand ("invdrop",				Cmd_InvDrop_f);
	Cmd_AddCommand ("weapprev",				Cmd_WeapPrev_f);
	Cmd_AddCommand ("weapnext",				Cmd_WeapNext_f);
	Cmd_AddCommand ("weaplast",				Cmd_WeapLast_f);
	Cmd_AddCommand ("invnext",				Cmd_SelectNextItem_f);
	Cmd_AddCommand ("invprev",				Cmd_SelectPrevItem_f);
	Cmd_AddCommand ("invnextw",				Cmd_SelectNextWeapon_f);
	Cmd_AddCommand ("invprevw",				Cmd_SelectPrevWeapon_f);
	Cmd_AddCommand ("invnextp",				Cmd_SelectNextPowerup_f);
	Cmd_AddCommand ("invprevp",				Cmd_SelectPrevPowerup_f);
	Cmd_AddCommand ("test",					Cmd_Test_f);

	// And last but certainly not least...
	Cmd_AddCommand ("god",					Cmd_God_f,				CMD_CHEAT);
	Cmd_AddCommand ("notarget",				Cmd_Notarget_f,			CMD_CHEAT);
	Cmd_AddCommand ("noclip",				Cmd_Noclip_f,			CMD_CHEAT);
	Cmd_AddCommand ("give",					Cmd_Give_f,				CMD_CHEAT);
	Cmd_AddCommand ("spawn",				Cmd_Give,				CMD_CHEAT);
}

/*
=================
ClientCommand
=================
*/
void ClientCommand (edict_t *ent)
{
	if (!ent->client)
		return;		// not fully in game yet

	InitArg ();
	Cmd_RunCommand (ArgGets(0), ent);
	EndArg ();
}
