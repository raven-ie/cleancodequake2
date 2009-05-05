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
// cc_sbar.h
// Contains a class to create statusbars easily, without the need to directly edit a string.
// This can also be used for scoreboards (+ SendMsg)
//

#include "cc_local.h"

void CStatusBar::AddToBarBuffer (char *string)
{
	Q_snprintfz (bar, sizeof(bar), "%s%s", bar, temp);
}

void CStatusBar::AddVirtualPoint_Y (int y)
{
	Q_snprintfz (temp, sizeof(temp), "yv %i ", y);
	AddToBarBuffer (temp);
}

void CStatusBar::AddVirtualPoint_X (int x)
{
	Q_snprintfz (temp, sizeof(temp), "xv %i ", x);
	AddToBarBuffer (temp);
}

void CStatusBar::AddPoint_X (int x, bool inverted = false)
{
	Q_snprintfz (temp, sizeof(temp), "x%c %i ", inverted ? 'r' : 'l', x);
	AddToBarBuffer (temp);
}

void CStatusBar::AddPoint_Y (int y, bool inverted = false)
{
	Q_snprintfz (temp, sizeof(temp), "y%c %i ", inverted ? 'b' : 't', y);
	AddToBarBuffer (temp);
}

void CStatusBar::AddString (char *string, bool highBit = false)
{
	Q_snprintfz (temp, sizeof(temp), "string%s %s ", highBit ? "2" : "", string);
	AddToBarBuffer(temp);
}

void CStatusBar::AddStatString (int stat)
{
	Q_snprintfz (temp, sizeof(temp), "stat_string %i ", stat);
	AddToBarBuffer (temp);
}

void CStatusBar::AddPic (char *pic)
{
	Q_snprintfz (temp, sizeof(temp), "picn %s ", pic);
	AddToBarBuffer(temp);
}

void CStatusBar::AddPicStat (int stat)
{
	Q_snprintfz (temp, sizeof(temp), "pic %i ", stat);
	AddToBarBuffer(temp);
}

void CStatusBar::AddNumStat (int stat, int width = 3)
{
	Q_snprintfz (temp, sizeof(temp), "num %i %i ", width, stat);
	AddToBarBuffer(temp);
}

void CStatusBar::AddStatement (int stat, bool end = false)
{
	if (!end)
		Q_snprintfz (temp, sizeof(temp), "if %i ", stat);
	else
		Q_snprintfz (temp, sizeof(temp), "endif ");
	AddToBarBuffer(temp);
}

void CStatusBar::AddAmmoNum ()
{
	Q_snprintfz (temp, sizeof(temp), "anum ");
	AddToBarBuffer(temp);
}

void CStatusBar::AddHealthNum ()
{
	Q_snprintfz (temp, sizeof(temp), "hnum ");
	AddToBarBuffer(temp);
}

void CStatusBar::AddArmorNum ()
{
	Q_snprintfz (temp, sizeof(temp), "rnum ");
	AddToBarBuffer(temp);
}

void CStatusBar::AddClientBlock (int x, int y, int cNum, int score, int ping, int time)
{
	Q_snprintfz (temp, sizeof(temp), "client %i %i %i %i %i %i ", x, y, cNum, score, ping, time);
	AddToBarBuffer(temp);
}

void CStatusBar::AddClientBlock (int x, int y, int cNum, int score, int ping)
{
	Q_snprintfz (temp, sizeof(temp), "ctf %i %i %i %i %i %i ", x, y, cNum, score, ping);
	AddToBarBuffer(temp);
}

void CreateDMStatusbar ()
{
	CStatusBar *DMBar = new CStatusBar;

	DMBar->AddPoint_Y (-24, true);

	DMBar->AddVirtualPoint_X (0);
	DMBar->AddHealthNum ();
	DMBar->AddVirtualPoint_X (50);
	DMBar->AddPicStat (STAT_HEALTH_ICON);

	DMBar->AddStatement (STAT_AMMO_ICON);
	DMBar->AddVirtualPoint_X (100);
	DMBar->AddAmmoNum ();
	DMBar->AddVirtualPoint_X (150);
	DMBar->AddPicStat (STAT_AMMO_ICON);
	DMBar->AddStatement (STAT_AMMO, true);

	DMBar->AddStatement (STAT_ARMOR);
	DMBar->AddVirtualPoint_X (200);
	DMBar->AddArmorNum ();
	DMBar->AddVirtualPoint_X (250);
	DMBar->AddPicStat (STAT_ARMOR_ICON);
	DMBar->AddStatement (STAT_ARMOR, true);

	DMBar->AddStatement (STAT_SELECTED_ICON);
	DMBar->AddVirtualPoint_X (296);
	DMBar->AddPicStat (STAT_SELECTED_ICON);
	DMBar->AddStatement (STAT_SELECTED_ICON, true);

	DMBar->AddPoint_Y (-50, true);

	DMBar->AddStatement (STAT_PICKUP_ICON);
	DMBar->AddVirtualPoint_X (0);
	DMBar->AddPicStat (STAT_PICKUP_ICON);
	DMBar->AddVirtualPoint_X (26);
	DMBar->AddPoint_Y (-42, true);
	DMBar->AddStatString (STAT_PICKUP_STRING);
	DMBar->AddPoint_Y (-50, true);
	DMBar->AddStatement (STAT_PICKUP_ICON, true);

	DMBar->AddStatement (STAT_TIMER_ICON);
	DMBar->AddVirtualPoint_X (246);
	DMBar->AddNumStat (STAT_TIMER, 2);
	DMBar->AddVirtualPoint_X (296);
	DMBar->AddPicStat (STAT_TIMER_ICON);
	DMBar->AddStatement (STAT_TIMER_ICON, true);

	DMBar->AddStatement (STAT_HELPICON);
	DMBar->AddVirtualPoint_X (148);
	DMBar->AddPicStat (STAT_HELPICON);
	DMBar->AddStatement (STAT_HELPICON, true);

	DMBar->AddPoint_X (-50, true);
	DMBar->AddPoint_Y (2);
	DMBar->AddNumStat (STAT_FRAGS);

	DMBar->AddStatement (STAT_SPECTATOR);
	DMBar->AddVirtualPoint_X (0);
	DMBar->AddPoint_Y (-58, true);
	DMBar->AddString ("SPECTATOR MODE", true);
	DMBar->AddStatement (STAT_SPECTATOR, true);

	DMBar->AddStatement (STAT_CHASE);
	DMBar->AddVirtualPoint_X (0);
	DMBar->AddPoint_Y (-68, true);
	DMBar->AddString ("Chasing");
	DMBar->AddVirtualPoint_X (64);
	DMBar->AddStatString (STAT_CHASE);
	DMBar->AddStatement (STAT_CHASE, true);

	DMBar->Send();
}

void CreateSPStatusbar ()
{
	CStatusBar *SPBar = new CStatusBar;

	SPBar->AddPoint_Y (-24, true);

	SPBar->AddVirtualPoint_X (0);
	SPBar->AddHealthNum ();
	SPBar->AddVirtualPoint_X (50);
	SPBar->AddPicStat (STAT_HEALTH_ICON);

	SPBar->AddStatement (STAT_AMMO_ICON);
	SPBar->AddVirtualPoint_X (100);
	SPBar->AddAmmoNum ();
	SPBar->AddVirtualPoint_X (150);
	SPBar->AddPicStat (STAT_AMMO_ICON);
	SPBar->AddStatement (STAT_AMMO, true);

	SPBar->AddStatement (STAT_ARMOR);
	SPBar->AddVirtualPoint_X (200);
	SPBar->AddArmorNum ();
	SPBar->AddVirtualPoint_X (250);
	SPBar->AddPicStat (STAT_ARMOR_ICON);
	SPBar->AddStatement (STAT_ARMOR, true);

	SPBar->AddStatement (STAT_SELECTED_ICON);
	SPBar->AddVirtualPoint_X (296);
	SPBar->AddPicStat (STAT_SELECTED_ICON);
	SPBar->AddStatement (STAT_SELECTED_ICON, true);

	SPBar->AddPoint_Y (-50, true);

	SPBar->AddStatement (STAT_PICKUP_ICON);
	SPBar->AddVirtualPoint_X (0);
	SPBar->AddPicStat (STAT_PICKUP_ICON);
	SPBar->AddVirtualPoint_X (26);
	SPBar->AddPoint_Y (-42, true);
	SPBar->AddStatString (STAT_PICKUP_STRING);
	SPBar->AddPoint_Y (-50, true);
	SPBar->AddStatement (STAT_PICKUP_ICON, true);

	SPBar->AddStatement (STAT_TIMER_ICON);
	SPBar->AddVirtualPoint_X (246);
	SPBar->AddNumStat (STAT_TIMER, 2);
	SPBar->AddVirtualPoint_X (296);
	SPBar->AddPicStat (STAT_TIMER_ICON);
	SPBar->AddStatement (STAT_TIMER_ICON, true);

	SPBar->AddStatement (STAT_HELPICON);
	SPBar->AddVirtualPoint_X (148);
	SPBar->AddPicStat (STAT_HELPICON);
	SPBar->AddStatement (STAT_HELPICON, true);

	SPBar->Send();
}


/*
==================
DeathmatchScoreboardMessage
==================
*/
void DeathmatchScoreboardMessage (edict_t *ent, edict_t *killer, bool reliable = true)
{
	CStatusBar *Scoreboard = new CStatusBar;

	int		sorted[MAX_CS_CLIENTS];
	int		sortedscores[MAX_CS_CLIENTS];
	int		score, total;
	gclient_t	*cl;
	edict_t		*cl_ent;

	// sort the clients by score
	total = 0;
	for (int i = 0; i < game.maxclients ; i++)
	{
		cl_ent = g_edicts + 1 + i;
		if (!cl_ent->inUse || game.clients[i].resp.spectator)
			continue;
		score = game.clients[i].resp.score;
		int j = 0;
		for ( ; j < total ; j++)
		{
			if (score > sortedscores[j])
				break;
		}
		int k = total;
		for ( ; k > j ; k--)
		{
			sorted[k] = sorted[k-1];
			sortedscores[k] = sortedscores[k-1];
		}
		sorted[j] = i;
		sortedscores[j] = score;
		total++;
	}

	// add the clients in sorted order
	if (total > 12)
		total = 12;

	for (int i = 0 ; i < total ; i++)
	{
		int		x, y;
		char	*tag;

		cl = &game.clients[sorted[i]];
		cl_ent = g_edicts + 1 + sorted[i];

		x = (i>=6) ? 160 : 0;
		y = 32 + 32 * (i%6);

		// add a dogtag
		if (cl_ent == ent)
			tag = "tag1";
		else if (cl_ent == killer)
			tag = "tag2";
		else
			tag = NULL;
		if (tag)
		{
			Scoreboard->AddVirtualPoint_X (x+32);
			Scoreboard->AddVirtualPoint_Y (y);
			Scoreboard->AddPic (tag);
		}

		// send the layout
		Scoreboard->AddClientBlock (x, y, sorted[i], cl->resp.score, cl->ping, (level.framenum - cl->resp.enterframe)/600);
	}

	Scoreboard->SendMsg (ent, reliable);
}