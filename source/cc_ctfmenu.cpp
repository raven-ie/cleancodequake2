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
#include "cc_menu.h"

#ifdef CLEANCTF_ENABLED

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
			ent->Client.Respawn.MenuState.CloseMenu();
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
		Bar.SendMsg(ent, reliable);
	};
};

void CTFOpenCreditsMenu(CPlayerEntity *ent)
{
	if (ent->Client.Respawn.MenuState.InMenu)
		return;

	ent->Client.Respawn.MenuState.CurrentMenu = QNew (com_levelPool, 0) CCTFCreditsMenu(ent);
	ent->Client.Respawn.MenuState.OpenMenu ();

	ent->Client.Respawn.MenuState.CurrentMenu->Draw (true);
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
			ent->Client.Respawn.ctf_team = team;
			ent->Client.Respawn.ctf_state = 0;
			char *s = Info_ValueForKey (ent->Client.Persistent.userinfo, "skin");
			ent->CTFAssignSkin(s);

			// assign a ghost if we are in match mode
			if (ctfgame.match == MATCH_GAME)
			{
				if (ent->Client.Respawn.ghost)
					ent->Client.Respawn.ghost->code = 0;
				ent->Client.Respawn.ghost = NULL;
				ent->CTFAssignGhost();
			}

			ent->PutInServer ();
			// add a teleportation effect
			ent->State.GetEvent() = EV_PLAYER_TELEPORT;
			// hold in place briefly
			ent->Client.PlayerState.GetPMove()->pmFlags = PMF_TIME_TELEPORT;
			ent->Client.PlayerState.GetPMove()->pmTime = 14;
			BroadcastPrintf(PRINT_HIGH, "%s joined the %s team.\n",
				ent->Client.Persistent.netname, CTFTeamName(team));

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
				CPlayerEntity *e = entity_cast<CPlayerEntity>((g_edicts + i)->Entity);
				if (e->IsInUse() && e->GetSolid() != SOLID_NOT)
				{
					ent->Client.chase_target = e;
					ent->Client.LayoutFlags |= LF_UPDATECHASE;
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
			ent->Client.Respawn.MenuState.CloseMenu();
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
			CPlayerEntity *Player = entity_cast<CPlayerEntity>(g_edicts[i+1].Entity);
			if (!Player->IsInUse())
				continue;
			if (Player->Client.Respawn.ctf_team == CTF_TEAM1)
				num1++;
			else if (Player->Client.Respawn.ctf_team == CTF_TEAM2)
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
		Bar.SendMsg(ent, reliable);
	};
};

void CTFOpenJoinMenu(CPlayerEntity *ent)
{
	if (ent->Client.Respawn.MenuState.InMenu)
		return;

	ent->Client.Respawn.MenuState.CurrentMenu = QNew (com_levelPool, 0) CCTFMainMenu(ent);
	ent->Client.Respawn.MenuState.OpenMenu ();

	ent->Client.Respawn.MenuState.CurrentMenu->Draw (true);
}
#endif