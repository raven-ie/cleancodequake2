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
// cc_ctfmenu.cpp
// Self-explanatory
//

#include "g_local.h"
#ifdef CLEANCTF_ENABLED

#ifdef TEMP
void CTFJoinTeam1(edict_t *ent, pmenuhnd_t *p);
void CTFJoinTeam2(edict_t *ent, pmenuhnd_t *p);
void CTFCredits(edict_t *ent, pmenuhnd_t *p);
void CTFReturnToMain(edict_t *ent, pmenuhnd_t *p);
void CTFChaseCam(edict_t *ent, pmenuhnd_t *p);

static void SetLevelName(pmenu_t *p)
{
	static char levelname[33];

	levelname[0] = '*';
	if (g_edicts[0].message)
		strncpy(levelname+1, g_edicts[0].message, sizeof(levelname) - 2);
	else
		strncpy(levelname+1, level.mapname, sizeof(levelname) - 2);
	levelname[sizeof(levelname) - 1] = 0;
	p->text = levelname;
}

pmenu_t creditsmenu[] = {
	{ "*Quake II",						PMENU_ALIGN_CENTER, NULL },
	{ "*ThreeWave Capture the Flag",	PMENU_ALIGN_CENTER, NULL },
	{ NULL,								PMENU_ALIGN_CENTER, NULL },
	{ "*Programming",					PMENU_ALIGN_CENTER, NULL }, 
	{ "Dave 'Zoid' Kirsch",				PMENU_ALIGN_CENTER, NULL },
	{ "*Level Design", 					PMENU_ALIGN_CENTER, NULL },
	{ "Christian Antkow",				PMENU_ALIGN_CENTER, NULL },
	{ "Tim Willits",					PMENU_ALIGN_CENTER, NULL },
	{ "Dave 'Zoid' Kirsch",				PMENU_ALIGN_CENTER, NULL },
	{ "*Art",							PMENU_ALIGN_CENTER, NULL },
	{ "Adrian Carmack Paul Steed",		PMENU_ALIGN_CENTER, NULL },
	{ "Kevin Cloud",					PMENU_ALIGN_CENTER, NULL },
	{ "*Sound",							PMENU_ALIGN_CENTER, NULL },
	{ "Tom 'Bjorn' Klok",				PMENU_ALIGN_CENTER, NULL },
	{ "*Original CTF Art Design",		PMENU_ALIGN_CENTER, NULL },
	{ "Brian 'Whaleboy' Cozzens",		PMENU_ALIGN_CENTER, NULL },
	{ NULL,								PMENU_ALIGN_CENTER, NULL },
	{ "Return to Main Menu",			PMENU_ALIGN_LEFT, CTFReturnToMain }
};

static const int jmenu_level = 2;
static const int jmenu_match = 3;
static const int jmenu_red = 5;
static const int jmenu_blue = 7;
static const int jmenu_chase = 9;
static const int jmenu_reqmatch = 11;

pmenu_t joinmenu[] = {
	{ "*Quake II",			PMENU_ALIGN_CENTER, NULL },
	{ "*ThreeWave Capture the Flag",	PMENU_ALIGN_CENTER, NULL },
	{ NULL,					PMENU_ALIGN_CENTER, NULL },
	{ NULL,					PMENU_ALIGN_CENTER, NULL },
	{ NULL,					PMENU_ALIGN_CENTER, NULL },
	{ "Join Red Team",		PMENU_ALIGN_LEFT, CTFJoinTeam1 },
	{ NULL,					PMENU_ALIGN_LEFT, NULL },
	{ "Join Blue Team",		PMENU_ALIGN_LEFT, CTFJoinTeam2 },
	{ NULL,					PMENU_ALIGN_LEFT, NULL },
	{ "Chase Camera",		PMENU_ALIGN_LEFT, CTFChaseCam },
	{ "Credits",			PMENU_ALIGN_LEFT, CTFCredits },
	{ NULL,					PMENU_ALIGN_LEFT, NULL },
	{ NULL,					PMENU_ALIGN_LEFT, NULL },
	{ "Use [ and ] to move cursor",	PMENU_ALIGN_LEFT, NULL },
	{ "ENTER to select",	PMENU_ALIGN_LEFT, NULL },
	{ "ESC to Exit Menu",	PMENU_ALIGN_LEFT, NULL },
	{ "(TAB to Return)",	PMENU_ALIGN_LEFT, NULL },
	{ "v" CTF_STRING_VERSION,	PMENU_ALIGN_RIGHT, NULL },
};

pmenu_t nochasemenu[] = {
	{ "*Quake II",			PMENU_ALIGN_CENTER, NULL },
	{ "*ThreeWave Capture the Flag",	PMENU_ALIGN_CENTER, NULL },
	{ NULL,					PMENU_ALIGN_CENTER, NULL },
	{ NULL,					PMENU_ALIGN_CENTER, NULL },
	{ "No one to chase",	PMENU_ALIGN_LEFT, NULL },
	{ NULL,					PMENU_ALIGN_CENTER, NULL },
	{ "Return to Main Menu", PMENU_ALIGN_LEFT, CTFReturnToMain }
};

void CTFJoinTeam(edict_t *ent, int desired_team)
{
	char *s;

	PMenu_Close(ent);

	ent->svflags &= ~SVF_NOCLIENT;
	ent->client->resp.ctf_team = desired_team;
	ent->client->resp.ctf_state = 0;
	s = Info_ValueForKey (ent->client->pers.userinfo, "skin");
	CTFAssignSkin(ent, s);

	// assign a ghost if we are in match mode
	if (ctfgame.match == MATCH_GAME) {
		if (ent->client->resp.ghost)
			ent->client->resp.ghost->code = 0;
		ent->client->resp.ghost = NULL;
		CTFAssignGhost(ent);
	}

	PutClientInServer (ent);
	// add a teleportation effect
	ent->state.event = EV_PLAYER_TELEPORT;
	// hold in place briefly
	ent->client->playerState.pmove.pm_flags = PMF_TIME_TELEPORT;
	ent->client->playerState.pmove.pm_time = 14;
	gi.bprintf(PRINT_HIGH, "%s joined the %s team.\n",
		ent->client->pers.netname, CTFTeamName(desired_team));

	if (ctfgame.match == MATCH_SETUP) {
		gi.centerprintf(ent,	"***********************\n"
								"Type \"ready\" in console\n"
								"to ready up.\n"
								"***********************");
	}
}

void CTFJoinTeam1(edict_t *ent, pmenuhnd_t *p)
{
	CTFJoinTeam(ent, CTF_TEAM1);
}

void CTFJoinTeam2(edict_t *ent, pmenuhnd_t *p)
{
	CTFJoinTeam(ent, CTF_TEAM2);
}

void CTFChaseCam(edict_t *ent, pmenuhnd_t *p)
{
	int i;
	edict_t *e;

	if (ent->client->chase_target) {
		ent->client->chase_target = NULL;
		PMenu_Close(ent);
		return;
	}

	for (i = 1; i <= maxclients->value; i++) {
		e = g_edicts + i;
		if (e->inuse && e->solid != SOLID_NOT) {
			ent->client->chase_target = e;
			PMenu_Close(ent);
			ent->client->update_chase = true;
			return;
		}
	}

	SetLevelName(nochasemenu + jmenu_level);

	PMenu_Close(ent);
	PMenu_Open(ent, nochasemenu, -1, sizeof(nochasemenu) / sizeof(pmenu_t), NULL);
}

void CTFReturnToMain(edict_t *ent, pmenuhnd_t *p)
{
	PMenu_Close(ent);
	CTFOpenJoinMenu(ent);
}

void CTFRequestMatch(edict_t *ent, pmenuhnd_t *p)
{
	char text[1024];

	PMenu_Close(ent);

	sprintf(text, "%s has requested to switch to competition mode.",
		ent->client->pers.netname);
	CTFBeginElection(ent, ELECT_MATCH, text);
}

void DeathmatchScoreboard (edict_t *ent);

void CTFShowScores(edict_t *ent, pmenu_t *p)
{
	PMenu_Close(ent);

	ent->client->showscores = true;
	ent->client->showinventory = false;
	DeathmatchScoreboard (ent);
}

int CTFUpdateJoinMenu(edict_t *ent)
{
	static char team1players[32];
	static char team2players[32];
	int num1, num2, i;

	if (ctfgame.match >= MATCH_PREGAME && matchlock->value) {
		joinmenu[jmenu_red].text = "MATCH IS LOCKED";
		joinmenu[jmenu_red].SelectFunc = NULL;
		joinmenu[jmenu_blue].text = "  (entry is not permitted)";
		joinmenu[jmenu_blue].SelectFunc = NULL;
	} else {
		if (ctfgame.match >= MATCH_PREGAME) {
			joinmenu[jmenu_red].text = "Join Red MATCH Team";
			joinmenu[jmenu_blue].text = "Join Blue MATCH Team";
		} else {
			joinmenu[jmenu_red].text = "Join Red Team";
			joinmenu[jmenu_blue].text = "Join Blue Team";
		}
		joinmenu[jmenu_red].SelectFunc = CTFJoinTeam1;
		joinmenu[jmenu_blue].SelectFunc = CTFJoinTeam2;
	}

	if (ctf_forcejoin->string && *ctf_forcejoin->string) {
		if (stricmp(ctf_forcejoin->string, "red") == 0) {
			joinmenu[jmenu_blue].text = NULL;
			joinmenu[jmenu_blue].SelectFunc = NULL;
		} else if (stricmp(ctf_forcejoin->string, "blue") == 0) {
			joinmenu[jmenu_red].text = NULL;
			joinmenu[jmenu_red].SelectFunc = NULL;
		}
	}

	if (ent->client->chase_target)
		joinmenu[jmenu_chase].text = "Leave Chase Camera";
	else
		joinmenu[jmenu_chase].text = "Chase Camera";

	SetLevelName(joinmenu + jmenu_level);

	num1 = num2 = 0;
	for (i = 0; i < maxclients->value; i++) {
		if (!g_edicts[i+1].inuse)
			continue;
		if (game.clients[i].resp.ctf_team == CTF_TEAM1)
			num1++;
		else if (game.clients[i].resp.ctf_team == CTF_TEAM2)
			num2++;
	}

	sprintf(team1players, "  (%d players)", num1);
	sprintf(team2players, "  (%d players)", num2);

	switch (ctfgame.match) {
	case MATCH_NONE :
		joinmenu[jmenu_match].text = NULL;
		break;

	case MATCH_SETUP :
		joinmenu[jmenu_match].text = "*MATCH SETUP IN PROGRESS";
		break;

	case MATCH_PREGAME :
		joinmenu[jmenu_match].text = "*MATCH STARTING";
		break;

	case MATCH_GAME :
		joinmenu[jmenu_match].text = "*MATCH IN PROGRESS";
		break;
	}

	if (joinmenu[jmenu_red].text)
		joinmenu[jmenu_red+1].text = team1players;
	else
		joinmenu[jmenu_red+1].text = NULL;
	if (joinmenu[jmenu_blue].text)
		joinmenu[jmenu_blue+1].text = team2players;
	else
		joinmenu[jmenu_blue+1].text = NULL;

	joinmenu[jmenu_reqmatch].text = NULL;
	joinmenu[jmenu_reqmatch].SelectFunc = NULL;
	if (competition->value && ctfgame.match < MATCH_SETUP) {
		joinmenu[jmenu_reqmatch].text = "Request Match";
		joinmenu[jmenu_reqmatch].SelectFunc = CTFRequestMatch;
	}
	
	if (num1 > num2)
		return CTF_TEAM1;
	else if (num2 > num1)
		return CTF_TEAM2;
	return (irandom(2)) ? CTF_TEAM1 : CTF_TEAM2;
}

void CTFOpenJoinMenu(edict_t *ent)
{
	int team;

	team = CTFUpdateJoinMenu(ent);
	if (ent->client->chase_target)
		team = 8;
	else if (team == CTF_TEAM1)
		team = 4;
	else
		team = 6;
	PMenu_Open(ent, joinmenu, team, sizeof(joinmenu) / sizeof(pmenu_t), NULL);
}

void CTFCredits(edict_t *ent, pmenuhnd_t *p)
{
	PMenu_Close(ent);
	PMenu_Open(ent, creditsmenu, -1, sizeof(creditsmenu) / sizeof(pmenu_t), NULL);
}

pmenu_t joinmenu[] = {
	{ "*Quake II",			PMENU_ALIGN_CENTER, NULL },
	{ "*ThreeWave Capture the Flag",	PMENU_ALIGN_CENTER, NULL },
	{ NULL,					PMENU_ALIGN_CENTER, NULL },
	{ NULL,					PMENU_ALIGN_CENTER, NULL },
	{ NULL,					PMENU_ALIGN_CENTER, NULL },
	{ "Join Red Team",		PMENU_ALIGN_LEFT, CTFJoinTeam1 },
	{ NULL,					PMENU_ALIGN_LEFT, NULL },
	{ "Join Blue Team",		PMENU_ALIGN_LEFT, CTFJoinTeam2 },
	{ NULL,					PMENU_ALIGN_LEFT, NULL },
	{ "Chase Camera",		PMENU_ALIGN_LEFT, CTFChaseCam },
	{ "Credits",			PMENU_ALIGN_LEFT, CTFCredits },
	{ NULL,					PMENU_ALIGN_LEFT, NULL },
	{ NULL,					PMENU_ALIGN_LEFT, NULL },
	{ "Use [ and ] to move cursor",	PMENU_ALIGN_LEFT, NULL },
	{ "ENTER to select",	PMENU_ALIGN_LEFT, NULL },
	{ "ESC to Exit Menu",	PMENU_ALIGN_LEFT, NULL },
	{ "(TAB to Return)",	PMENU_ALIGN_LEFT, NULL },
	{ "v" CTF_STRING_VERSION,	PMENU_ALIGN_RIGHT, NULL },
};

	{ "*Quake II",						PMENU_ALIGN_CENTER, NULL },
	{ "*ThreeWave Capture the Flag",	PMENU_ALIGN_CENTER, NULL },
	{ NULL,								PMENU_ALIGN_CENTER, NULL },
	{ "*Programming",					PMENU_ALIGN_CENTER, NULL }, 
	{ "Dave 'Zoid' Kirsch",				PMENU_ALIGN_CENTER, NULL },
	{ "*Level Design", 					PMENU_ALIGN_CENTER, NULL },
	{ "Christian Antkow",				PMENU_ALIGN_CENTER, NULL },
	{ "Tim Willits",					PMENU_ALIGN_CENTER, NULL },
	{ "Dave 'Zoid' Kirsch",				PMENU_ALIGN_CENTER, NULL },
	{ "*Art",							PMENU_ALIGN_CENTER, NULL },
	{ "Adrian Carmack Paul Steed",		PMENU_ALIGN_CENTER, NULL },
	{ "Kevin Cloud",					PMENU_ALIGN_CENTER, NULL },
	{ "*Sound",							PMENU_ALIGN_CENTER, NULL },
	{ "Tom 'Bjorn' Klok",				PMENU_ALIGN_CENTER, NULL },
	{ "*Original CTF Art Design",		PMENU_ALIGN_CENTER, NULL },
	{ "Brian 'Whaleboy' Cozzens",		PMENU_ALIGN_CENTER, NULL },
	{ NULL,								PMENU_ALIGN_CENTER, NULL },
	{ "Return to Main Menu",			PMENU_ALIGN_LEFT, CTFReturnToMain }
#endif

void CTFOpenJoinMenu(edict_t *ent);

class CCTFCreditsMenu : public CMenu
{
public:
	CCTFCreditsMenu			(CPlayerEntity *ent) :
	CMenu(ent)
	{
		Cursor = 2;
	};

	class CCloseLabel : public CMenu_Label
	{
	public:
		CCloseLabel(CCTFCreditsMenu *Menu, int x, int y) :
		CMenu_Label(Menu, x, y)
		{
		};

		bool Select (CPlayerEntity *ent)
		{
			ent->Client.resp.MenuState.CloseMenu();
			CTFOpenJoinMenu (ent);

			// Has to be false so we can tell it to switch manually
			return false;
		};
	};


	bool				Open ()
	{
		int x = 0, y = -76; // Top

		CMenu_Label *TopGreen = QNew (com_levelPool, 0) CMenu_Label(this, x, y);
		TopGreen->Enabled = false;
		TopGreen->Align = LA_CENTER;
		TopGreen->Flags = LF_GREEN;
		Q_snprintfz (TopGreen->LabelString, sizeof(TopGreen->LabelString), "Quake II\nCleanCode Capture the Flag\n\nProgramming\n\nOriginal Idea\n\nArt\n\n\n\nSounds\n\nTesting");
		AddItem(TopGreen);

		y += 32;

		CMenu_Label *BottomWhite = QNew (com_levelPool, 0) CMenu_Label(this, x, y);
		BottomWhite->Enabled = false;
		BottomWhite->Align = LA_CENTER;
		Q_snprintfz (BottomWhite->LabelString, sizeof(BottomWhite->LabelString), "Paril\n\nDavid 'Zoid' Kirsch\n\nAdrian Carmack\nPaul Steed\nBrian Cozzens\n\nTom Klok\n\nxRDVx");
		AddItem(BottomWhite);

		x = -93;
		y += (8 * 12);

		CCloseLabel *Back = QNew (com_levelPool, 0) CCloseLabel (this, x, y);
		Back->Enabled = true;
		Q_snprintfz (Back->LabelString, sizeof(Back->LabelString), "Back");
		AddItem(Back);

		return true;
	};

	void				Close ()
	{
	};

	void				Draw (bool reliable)
	{
		CStatusBar Bar;

		DrawItems(&Bar);
		Bar.SendMsg(ent->gameEntity, reliable);
	};
};

void CTFOpenCreditsMenu(CPlayerEntity *ent)
{
	if (ent->Client.resp.MenuState.InMenu)
		return;

	ent->Client.resp.MenuState.CurrentMenu = QNew (com_levelPool, 0) CCTFCreditsMenu(ent);
	ent->Client.resp.MenuState.OpenMenu ();

	ent->Client.resp.MenuState.CurrentMenu->Draw (true);
}

class CCTFMainMenu : public CMenu
{
public:
	CCTFMainMenu			(CPlayerEntity *ent) :
	CMenu(ent)
	{
		Cursor = 3;
	};

	class CJoinGameLabel : public CMenu_Label
	{
	public:
		int team;

		CJoinGameLabel(CCTFMainMenu *Menu, int x, int y, int team) :
		CMenu_Label(Menu, x, y),
		team(team)
		{
		};

		bool Select (CPlayerEntity *ent)
		{
			ent->SetSvFlags(ent->GetSvFlags() & ~SVF_NOCLIENT);
			ent->Client.resp.ctf_team = team;
			ent->Client.resp.ctf_state = 0;
			char *s = Info_ValueForKey (ent->Client.pers.userinfo, "skin");
			ent->CTFAssignSkin(s);

			// assign a ghost if we are in match mode
			if (ctfgame.match == MATCH_GAME)
			{
				if (ent->Client.resp.ghost)
					ent->Client.resp.ghost->code = 0;
				ent->Client.resp.ghost = NULL;
				ent->CTFAssignGhost();
			}

			ent->PutInServer ();
			// add a teleportation effect
			ent->State.SetEvent (EV_PLAYER_TELEPORT);
			// hold in place briefly
			ent->Client.PlayerState.GetPMove()->pmFlags = PMF_TIME_TELEPORT;
			ent->Client.PlayerState.GetPMove()->pmTime = 14;
			BroadcastPrintf(PRINT_HIGH, "%s joined the %s team.\n",
				ent->Client.pers.netname, CTFTeamName(team));

			if (ctfgame.match == MATCH_SETUP)
			{
				ClientPrintf(ent->gameEntity,	PRINT_CENTER, 
												"***********************\n"
												"Type \"ready\" in console\n"
												"to ready up.\n"
												"***********************");
			}
			return true;
		};
	};

	class CObserverLabel : public CMenu_Label
	{
	public:
		CObserverLabel(CCTFMainMenu *Menu, int x, int y) :
		CMenu_Label(Menu, x, y)
		{
		};

		bool Select (CPlayerEntity *ent)
		{
			if (ent->Client.chase_target)
			{
				ent->Client.chase_target = NULL;
				return true;
			}

			for (int i = 1; i <= game.maxclients; i++)
			{
				CPlayerEntity *e = dynamic_cast<CPlayerEntity*>((g_edicts + i)->Entity);
				if (e->IsInUse() && e->GetSolid() != SOLID_NOT)
				{
					ent->Client.chase_target = e;
					ent->Client.update_chase = true;
					return true;
				}
			}

			ent->PrintToClient(PRINT_CENTER, "No one to chase\n");
			return true;
		};
	};
	class CCreditsLabel : public CMenu_Label
	{
	public:
		CCreditsLabel(CCTFMainMenu *Menu, int x, int y) :
		CMenu_Label(Menu, x, y)
		{
		};

		bool Select (CPlayerEntity *ent)
		{
			ent->Client.resp.MenuState.CloseMenu();
			CTFOpenCreditsMenu (ent);

			// Has to be false so we can tell it to switch manually
			return false;
		};
	};


	bool				Open ()
	{
		int num1 = 0, num2 = 0;
		for (int i = 0; i < game.maxclients; i++)
		{
			CPlayerEntity *Player = dynamic_cast<CPlayerEntity*>(g_edicts[i+1].Entity);
			if (!Player->IsInUse())
				continue;
			if (Player->Client.resp.ctf_team == CTF_TEAM1)
				num1++;
			else if (Player->Client.resp.ctf_team == CTF_TEAM2)
				num2++;
		}

		int x = 0, y = 0;

		CMenu_Image *Background = QNew (com_levelPool, 0) CMenu_Image (this, x, y);
		Q_snprintfz (Background->ImageString, sizeof(Background->ImageString), "inventory");
		Background->Width = 256;
		Background->Height = 192;
		Background->Enabled = false;
		AddItem (Background);

		y = -76; // Top
		CMenu_Label *TopName = QNew (com_levelPool, 0) CMenu_Label(this, x, y);
		TopName->Enabled = false;
		TopName->Align = LA_CENTER;
		TopName->Flags = LF_GREEN;
		Q_snprintfz (TopName->LabelString, sizeof(TopName->LabelString), "Quake II\nCleanCode Capture the Flag");
		AddItem(TopName);

		y += (8 * 2);
		CMenu_Label *LevelName = QNew (com_levelPool, 0) CMenu_Label(this, x, y);
		LevelName->Enabled = false;
		LevelName->Align = LA_CENTER;
		LevelName->Flags = LF_GREEN;
		Q_snprintfz (LevelName->LabelString, sizeof(LevelName->LabelString), "%s", level.level_name);
		AddItem(LevelName);

		y += 8;
		CMenu_Label *MatchProgress;
		switch (ctfgame.match)
		{
		case MATCH_NONE :
			break;

		case MATCH_SETUP :
			MatchProgress = QNew (com_levelPool, 0) CMenu_Label(this, x, y);
			MatchProgress->Enabled = false;
			MatchProgress->Align = LA_CENTER;
			MatchProgress->Flags = LF_GREEN;
			Q_snprintfz (MatchProgress->LabelString, sizeof(MatchProgress->LabelString), "MATCH SETUP IN PROGRESS");
			AddItem(MatchProgress);
			break;

		case MATCH_PREGAME :
			MatchProgress = QNew (com_levelPool, 0) CMenu_Label(this, x, y);
			MatchProgress->Enabled = false;
			MatchProgress->Align = LA_CENTER;
			MatchProgress->Flags = LF_GREEN;
			Q_snprintfz (MatchProgress->LabelString, sizeof(MatchProgress->LabelString), "MATCH STARTING");
			AddItem(MatchProgress);
			break;

		case MATCH_GAME :
			MatchProgress = QNew (com_levelPool, 0) CMenu_Label(this, x, y);
			MatchProgress->Enabled = false;
			MatchProgress->Align = LA_CENTER;
			MatchProgress->Flags = LF_GREEN;
			Q_snprintfz (MatchProgress->LabelString, sizeof(MatchProgress->LabelString), "MATCH IN PROGRESS");
			AddItem(MatchProgress);
			break;
		}

		y += 8 * 3;
		x = -98;

		if (ctfgame.match >= MATCH_PREGAME && matchlock->Integer())
		{
			CMenu_Label *LockedMsg = QNew (com_levelPool, 0) CMenu_Label(this, x, y);
			LockedMsg->Enabled = false;
			LockedMsg->Align = LA_LEFT;
			Q_snprintfz (LockedMsg->LabelString, sizeof(LockedMsg->LabelString), "MATCH IS LOCKED\n(entry is not permitted)");
			AddItem(LockedMsg);
		}
		else if (ctf_forcejoin->String() && *ctf_forcejoin->String())
		{
			if (Q_stricmp(ctf_forcejoin->String(), "red") == 0)
			{
				// Only add red
				CJoinGameLabel *JoinRed = QNew (com_levelPool, 0) CJoinGameLabel(this, x, y, CTF_TEAM1);
				JoinRed->Enabled = true;
				JoinRed->Align = LA_LEFT;
				Q_snprintfz (JoinRed->LabelString, sizeof(JoinRed->LabelString), "Join %s Team    (%d players)", (ctfgame.match >= MATCH_PREGAME) ? "Red MATCH" : "Red", num1);
				AddItem(JoinRed);
			}
			else if (Q_stricmp(ctf_forcejoin->String(), "blue") == 0)
			{
				// Only add blue
				CJoinGameLabel *JoinBlue = QNew (com_levelPool, 0) CJoinGameLabel(this, x, y + 8, CTF_TEAM2);
				JoinBlue->Enabled = true;
				JoinBlue->Align = LA_LEFT;
				Q_snprintfz (JoinBlue->LabelString, sizeof(JoinBlue->LabelString), "Join %s Team    (%d players)", (ctfgame.match >= MATCH_PREGAME) ? "Blue MATCH" : "Blue", num2);
				AddItem(JoinBlue);
			}
			else
			{
				CJoinGameLabel *JoinRed = QNew (com_levelPool, 0) CJoinGameLabel(this, x, y, CTF_TEAM1);
				JoinRed->Enabled = true;
				JoinRed->Align = LA_LEFT;
				Q_snprintfz (JoinRed->LabelString, sizeof(JoinRed->LabelString), "Join %s Team     (%d players)", (ctfgame.match >= MATCH_PREGAME) ? "Red MATCH" : "Red", num1);
				AddItem(JoinRed);

				CJoinGameLabel *JoinBlue = QNew (com_levelPool, 0) CJoinGameLabel(this, x, y + 8, CTF_TEAM2);
				JoinBlue->Enabled = true;
				JoinBlue->Align = LA_LEFT;
				Q_snprintfz (JoinBlue->LabelString, sizeof(JoinBlue->LabelString), "Join %s Team   (%d players)", (ctfgame.match >= MATCH_PREGAME) ? "Blue MATCH" : "Blue", num2);
				AddItem(JoinBlue);
			}
		}
		else
		{
			CJoinGameLabel *JoinRed = QNew (com_levelPool, 0) CJoinGameLabel(this, x, y, CTF_TEAM1);
			JoinRed->Enabled = true;
			JoinRed->Align = LA_LEFT;
			Q_snprintfz (JoinRed->LabelString, sizeof(JoinRed->LabelString), "Join %s Team    (%d players)", (ctfgame.match >= MATCH_PREGAME) ? "Red MATCH" : "Red", num1);
			AddItem(JoinRed);

			CJoinGameLabel *JoinBlue = QNew (com_levelPool, 0) CJoinGameLabel(this, x, y + 8, CTF_TEAM2);
			JoinBlue->Enabled = true;
			JoinBlue->Align = LA_LEFT;
			Q_snprintfz (JoinBlue->LabelString, sizeof(JoinBlue->LabelString), "Join %s Team   (%d players)", (ctfgame.match >= MATCH_PREGAME) ? "Blue MATCH" : "Blue", num2);
			AddItem(JoinBlue);
		}

		y += 24;
		CObserverLabel *ChaseCam = QNew (com_levelPool, 0) CObserverLabel(this, x, y);
		ChaseCam->Enabled = true;
		ChaseCam->Align = LA_LEFT;
		if (ent->Client.chase_target)
			Q_snprintfz (ChaseCam->LabelString, sizeof(ChaseCam->LabelString), "Leave Chase Camera");
		else
			Q_snprintfz (ChaseCam->LabelString, sizeof(ChaseCam->LabelString), "Chase Camera");
		AddItem(ChaseCam);

		y += 8;
		CCreditsLabel *Credits = QNew (com_levelPool, 0) CCreditsLabel(this, x, y);
		Credits->Enabled = true;
		Credits->Align = LA_LEFT;
		Q_snprintfz (Credits->LabelString, sizeof(Credits->LabelString), "Credits");
		AddItem(Credits);

		return true;
	};

	void				Close ()
	{
	};

	void				Draw (bool reliable)
	{
		CStatusBar Bar;

		DrawItems(&Bar);
		Bar.SendMsg(ent->gameEntity, reliable);
	};
};

void CTFOpenJoinMenu(CPlayerEntity *ent)
{
	if (ent->Client.resp.MenuState.InMenu)
		return;

	ent->Client.resp.MenuState.CurrentMenu = QNew (com_levelPool, 0) CCTFMainMenu(ent);
	ent->Client.resp.MenuState.OpenMenu ();

	ent->Client.resp.MenuState.CurrentMenu->Draw (true);
}
#endif