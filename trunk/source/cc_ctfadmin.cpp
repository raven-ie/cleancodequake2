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
// cc_ctfadmin.cpp
// 
//

#include "cc_local.h"
#include "cc_menu.h"

#if CLEANCTF_ENABLED
bool CTFBeginElection(CPlayerEntity *ent, EElectState type, char *msg);
void CTFResetAllPlayers();

class CCTFSettingsMenu : public CMenu
{
public:
	CMenu_Slider<sint32>	*MatchStartLength;
	CMenu_Slider<float>	*MatchLength,
						*MatchSetupLength;

	CMenu_Spin			*WeaponsStaySpin,
						*InstantItemsSpin,
						*QuadDropSpin,
						*InstantWeaponsSpin,
						*MatchLockSpin;

	class CCloseLabel : public CMenu_Label
	{
	public:
		CCloseLabel(CCTFSettingsMenu *Menu, sint32 x, sint32 y) :
		CMenu_Label(Menu, x, y)
		{
		};

		bool Select (CPlayerEntity *ent)
		{
			return true;
		};
	};
	
	class CApplyLabel : public CMenu_Label
	{
	public:
		CCTFSettingsMenu *MyMenu;

		CApplyLabel(CCTFSettingsMenu *Menu, sint32 x, sint32 y) :
		CMenu_Label(Menu, x, y),
		MyMenu(Menu)
		{
		};

		bool Select (CPlayerEntity *ent)
		{
			if (MyMenu->MatchLength->Value != matchtime->Float())
			{
				BroadcastPrintf (PRINT_HIGH, "%s changed the match length to %d minutes.\n",
					ent->Client.Persistent.Name.c_str(), MyMenu->MatchLength->Value);

				if (ctfgame.match == MATCH_GAME)
					// in the middle of a match, change it on the fly
					ctfgame.matchtime = (ctfgame.matchtime - matchtime->Float()*600) + MyMenu->MatchLength->Value*600;

				matchtime->Set (MyMenu->MatchLength->Value);
			}

			if (MyMenu->MatchSetupLength->Value != matchsetuptime->Float())
			{
				BroadcastPrintf (PRINT_HIGH, "%s changed the match setup time to %d minutes.\n",
					ent->Client.Persistent.Name.c_str(), MyMenu->MatchSetupLength->Value);

				if (ctfgame.match == MATCH_SETUP)
					// in the middle of a match, change it on the fly
					ctfgame.matchtime = (ctfgame.matchtime - matchsetuptime->Float()*60) + MyMenu->MatchSetupLength->Value*60;


				matchsetuptime->Set (MyMenu->MatchSetupLength->Value);
			}

			if (MyMenu->MatchStartLength->Value != matchstarttime->Integer())
			{
				BroadcastPrintf(PRINT_HIGH, "%s changed the match start time to %d seconds.\n",
					ent->Client.Persistent.Name.c_str(), MyMenu->MatchStartLength->Value);

				if (ctfgame.match == MATCH_PREGAME)
					// in the middle of a match, change it on the fly
					ctfgame.matchtime = (ctfgame.matchtime - (matchstarttime->Integer()*10)) + (MyMenu->MatchStartLength->Value*10);

				matchstarttime->Set (MyMenu->MatchStartLength->Value);
			}

			sint32 i = dmflags->Integer();
			if (!!MyMenu->WeaponsStaySpin->Index != dmFlags.dfWeaponsStay)
			{
				BroadcastPrintf(PRINT_HIGH, "%s turned %s weapons stay.\n",
					ent->Client.Persistent.Name.c_str(), MyMenu->WeaponsStaySpin->Index ? "on" : "off");

				if (MyMenu->WeaponsStaySpin->Index)
					i |= DF_WEAPONS_STAY;
				else
					i &= ~DF_WEAPONS_STAY;
			}

			if (!!MyMenu->InstantItemsSpin->Index != dmFlags.dfInstantItems)
			{
				BroadcastPrintf(PRINT_HIGH, "%s turned %s instant items.\n",
					ent->Client.Persistent.Name.c_str(), MyMenu->InstantItemsSpin->Index ? "on" : "off");

				if (MyMenu->InstantItemsSpin->Index)
					i |= DF_INSTANT_ITEMS;
				else
					i &= ~DF_INSTANT_ITEMS;
			}

			if (!!MyMenu->QuadDropSpin->Index != dmFlags.dfQuadDrop)
			{
				BroadcastPrintf(PRINT_HIGH, "%s turned %s quad drop.\n",
					ent->Client.Persistent.Name.c_str(), MyMenu->QuadDropSpin->Index ? "on" : "off");

				if (MyMenu->QuadDropSpin->Index)
					i |= DF_QUAD_DROP;
				else
					i &= ~DF_QUAD_DROP;
			}

			dmflags->Set (i);

			if (!!MyMenu->InstantWeaponsSpin->Index != instantweap->Boolean())
			{
				BroadcastPrintf(PRINT_HIGH, "%s turned %s instant weapons.\n",
					ent->Client.Persistent.Name.c_str(), MyMenu->InstantWeaponsSpin->Index ? "on" : "off");

				instantweap->Set (MyMenu->InstantWeaponsSpin->Index);
			}

			if (!!MyMenu->MatchLockSpin->Index != matchlock->Boolean())
			{
				BroadcastPrintf(PRINT_HIGH, "%s turned %s match lock.\n",
					ent->Client.Persistent.Name.c_str(), MyMenu->MatchLockSpin->Index ? "on" : "off");

				matchlock->Set (MyMenu->MatchLockSpin->Index);
			}

			return true;
		};
	};

	CCTFSettingsMenu	(CPlayerEntity *Player) :
	  CMenu (Player)
	  {
	  };

	bool Open ()
	{
		sint32 x = 0, y = 0;
		static SSpinControlIndex YesNoValues[] =
		{
			{ "Off", "0" },
			{ "On", "1" },
			{ NULL, NULL},
		};

		CMenu_Image *Background = QNew (com_levelPool, 0) CMenu_Image (this, x, y);
		Background->ImageString = "inventory";
		Background->Width = 256;
		Background->Height = 192;
		Background->Enabled = false;

		y = -76; // Top
		CMenu_Label *TopName = QNew (com_levelPool, 0) CMenu_Label(this, x, y);
		TopName->Enabled = false;
		TopName->Align = LA_CENTER;
		TopName->Flags = LF_GREEN;
		TopName->LabelString = "Settings Menu";

		x = -98;
		y += 8 * 3;

		x += (8 * 16);
		MatchLength = QNew (com_levelPool, 0) CMenu_Slider<float> (this, x, y);
		MatchLength->Align = LA_LEFT;
		MatchLength->AppendText = " min";
		MatchLength->Min = 0.5f;
		MatchLength->Max = 60.0f;
		MatchLength->Step = 0.5f;
		MatchLength->Width = 6;
		MatchLength->Value = matchtime->Float();
		MatchLength->Enabled = true;
		MatchLength->x += 8 * 2;

		y += 8;
		MatchSetupLength = QNew (com_levelPool, 0) CMenu_Slider<float> (this, x, y);
		MatchSetupLength->Align = LA_LEFT;
		MatchSetupLength->AppendText = " min";
		MatchSetupLength->Min = 0.5f;
		MatchSetupLength->Max = 60.0f;
		MatchSetupLength->Step = 0.5f;
		MatchSetupLength->Width = 6;
		MatchSetupLength->Value = matchsetuptime->Float();
		MatchSetupLength->Enabled = true;
		MatchSetupLength->x += 8 * 2;

		y += 8;
		MatchStartLength = QNew (com_levelPool, 0) CMenu_Slider<sint32> (this, x, y);
		MatchStartLength->Align = LA_LEFT;
		MatchStartLength->AppendText = " sec";
		MatchStartLength->Min = 0;
		MatchStartLength->Max = 60.0f;
		MatchStartLength->Step = 1;
		MatchStartLength->Width = 6;
		MatchStartLength->Value = matchstarttime->Integer();
		MatchStartLength->Enabled = true;
		MatchStartLength->x += 8 * 2;

		y += 8;
		WeaponsStaySpin = QNew (com_levelPool, 0) CMenu_Spin (this, x, y, &YesNoValues[0]);
		WeaponsStaySpin->Index = dmFlags.dfWeaponsStay;
		WeaponsStaySpin->x += (8 * 4);
		WeaponsStaySpin->Align = LA_LEFT;

		y += 8;
		InstantItemsSpin = QNew (com_levelPool, 0) CMenu_Spin (this, x, y, &YesNoValues[0]);
		InstantItemsSpin->Index = dmFlags.dfInstantItems;
		InstantItemsSpin->x += (8 * 4);
		InstantItemsSpin->Align = LA_LEFT;

		y += 8;
		QuadDropSpin = QNew (com_levelPool, 0) CMenu_Spin (this, x, y, &YesNoValues[0]);
		QuadDropSpin->Index = dmFlags.dfQuadDrop;
		QuadDropSpin->x += (8 * 4);
		QuadDropSpin->Align = LA_LEFT;

		y += 8;
		InstantWeaponsSpin = QNew (com_levelPool, 0) CMenu_Spin (this, x, y, &YesNoValues[0]);
		InstantWeaponsSpin->Index = instantweap->Boolean();
		InstantWeaponsSpin->x += (8 * 4);
		InstantWeaponsSpin->Align = LA_LEFT;

		y += 8;
		MatchLockSpin = QNew (com_levelPool, 0) CMenu_Spin (this, x, y, &YesNoValues[0]);
		MatchLockSpin->Index = matchlock->Boolean();
		MatchLockSpin->x += (8 * 4);
		MatchLockSpin->Align = LA_LEFT;

		x = -98;
		y = -76 + 8 * 3; // Top

		CMenu_Label *MatchLengthLabel = QNew (com_levelPool, 0) CMenu_Label (this, x, y);
		MatchLengthLabel->Enabled = false;
		MatchLengthLabel->LabelString = "Match Len";
		MatchLengthLabel->Flags = LF_GREEN;

		y += 8;
		CMenu_Label *MatchSetupLengthLabel = QNew (com_levelPool, 0) CMenu_Label (this, x, y);
		MatchSetupLengthLabel->Enabled = false;
		MatchSetupLengthLabel->LabelString = "Match Setup Len";
		MatchSetupLengthLabel->Flags = LF_GREEN;

		y += 8;
		CMenu_Label *MatchStartLengthLabel = QNew (com_levelPool, 0) CMenu_Label (this, x, y);
		MatchStartLengthLabel->Enabled = false;
		MatchStartLengthLabel->LabelString = "Match Start Len";
		MatchStartLengthLabel->Flags = LF_GREEN;

		y += 8;
		CMenu_Label *WeaponsStayLabel = QNew (com_levelPool, 0) CMenu_Label (this, x, y);
		WeaponsStayLabel->Enabled = false;
		WeaponsStayLabel->LabelString = "Weapons Stay";
		WeaponsStayLabel->Flags = LF_GREEN;

		y += 8;
		CMenu_Label *InstantItemsLabel = QNew (com_levelPool, 0) CMenu_Label (this, x, y);
		InstantItemsLabel->Enabled = false;
		InstantItemsLabel->LabelString = "Instant Items";
		InstantItemsLabel->Flags = LF_GREEN;

		y += 8;
		CMenu_Label *QuadDropLabel = QNew (com_levelPool, 0) CMenu_Label (this, x, y);
		QuadDropLabel->Enabled = false;
		QuadDropLabel->LabelString = "Quad Drop";
		QuadDropLabel->Flags = LF_GREEN;

		y += 8;
		CMenu_Label *InstantWeaponsLabel = QNew (com_levelPool, 0) CMenu_Label (this, x, y);
		InstantWeaponsLabel->Enabled = false;
		InstantWeaponsLabel->LabelString = "Instant Weapons";
		InstantWeaponsLabel->Flags = LF_GREEN;

		y += 8;
		CMenu_Label *MatchLockLabel = QNew (com_levelPool, 0) CMenu_Label (this, x, y);
		MatchLockLabel->Enabled = false;
		MatchLockLabel->LabelString = "Match Lock";
		MatchLockLabel->Flags = LF_GREEN;

		y += 16;
		CApplyLabel *ApplyLabel = QNew (com_levelPool, 0) CApplyLabel (this, x, y);
		ApplyLabel->Enabled = true;
		ApplyLabel->LabelString = "Apply";
		ApplyLabel->Flags = LF_GREEN;

		y += 8;
		CCloseLabel *CancelLabel = QNew (com_levelPool, 0) CCloseLabel (this, x, y);
		CancelLabel->Enabled = true;
		CancelLabel->LabelString = "Cancel";
		CancelLabel->Flags = LF_GREEN;

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

void CTFOpenAdminSettings (CPlayerEntity *ent)
{
	if (ent->Client.Respawn.MenuState.InMenu)
		return;

	ent->Client.Respawn.MenuState.CurrentMenu = QNew (com_levelPool, 0) CCTFSettingsMenu(ent);
	ent->Client.Respawn.MenuState.OpenMenu ();

	ent->Client.Respawn.MenuState.CurrentMenu->Draw (true);
}

class CCTFAdminMenu : public CMenu
{
public:
	CCTFAdminMenu		(CPlayerEntity *Player) :
	  CMenu (Player)
	  {
	  };
	  
	class CMatchSetLabel : public CMenu_Label
	{
	public:
		CMatchSetLabel(CCTFAdminMenu *Menu, sint32 x, sint32 y) :
		CMenu_Label(Menu, x, y)
		{
		};

		bool Select (CPlayerEntity *ent)
		{
			switch (ctfgame.match)
			{
			case MATCH_SETUP:
				BroadcastPrintf (PRINT_CHAT, "Match has been forced to start.\n");
				ctfgame.match = MATCH_PREGAME;
				ctfgame.matchtime = level.Frame + (matchstarttime->Float() * 10);
				break;
			case MATCH_GAME:
				BroadcastPrintf(PRINT_CHAT, "Match has been forced to terminate.\n");
				ctfgame.match = MATCH_SETUP;
				ctfgame.matchtime = level.Frame + (matchsetuptime->Float() * 60);
				CTFResetAllPlayers();
				break;
			}
			return true;
		};
	};
	  
	class CMatchModeLabel : public CMenu_Label
	{
	public:
		CMatchModeLabel(CCTFAdminMenu *Menu, sint32 x, sint32 y) :
		CMenu_Label(Menu, x, y)
		{
		};

		bool Select (CPlayerEntity *ent)
		{
			if (ctfgame.match != MATCH_SETUP)
			{
				if (competition->Integer() < 3)
					competition->Set (2);
				ctfgame.match = MATCH_SETUP;
				CTFResetAllPlayers();
			}

			return true;
		};
	};

	class CCloseLabel : public CMenu_Label
	{
	public:
		CCloseLabel(CCTFAdminMenu *Menu, sint32 x, sint32 y) :
		CMenu_Label(Menu, x, y)
		{
		};

		bool Select (CPlayerEntity *ent)
		{
			return true;
		};
	};

	class COpenSettingsLabel : public CMenu_Label
	{
	public:
		COpenSettingsLabel(CCTFAdminMenu *Menu, sint32 x, sint32 y) :
		CMenu_Label(Menu, x, y)
		{
		};

		bool Select (CPlayerEntity *ent)
		{
			ent->Client.Respawn.MenuState.CloseMenu();
			CTFOpenAdminSettings (ent);

			return false;
		};
	};

	bool Open ()
	{
		sint32 x = 0, y = 0;

		CMenu_Image *Background = QNew (com_levelPool, 0) CMenu_Image (this, x, y);
		Background->ImageString = "inventory";
		Background->Width = 256;
		Background->Height = 192;
		Background->Enabled = false;

		y = -76; // Top
		CMenu_Label *TopName = QNew (com_levelPool, 0) CMenu_Label(this, x, y);
		TopName->Enabled = false;
		TopName->Align = LA_CENTER;
		TopName->Flags = LF_GREEN;
		TopName->LabelString = "Quake II\nCleanCode Capture the Flag\nAdministration Menu";

		x = -98;
		y += 8 * 5;
		COpenSettingsLabel *SettingsLabel = QNew (com_levelPool, 0) COpenSettingsLabel(this, x, y);
		SettingsLabel->Enabled = true;
		SettingsLabel->Align = LA_LEFT;
		SettingsLabel->LabelString = "Settings";

		y += 8;

		if (competition->Integer())
		{
			CMenu_Label *MatchLabel;

			switch (ctfgame.match)
			{
			case MATCH_SETUP:
				MatchLabel = QNew (com_levelPool, 0) CMatchSetLabel(this, x, y);
				MatchLabel->LabelString = "Force match start";
				break;
			case MATCH_GAME:
				MatchLabel = QNew (com_levelPool, 0) CMatchSetLabel(this, x, y);
				MatchLabel->LabelString = "Cancel match";
				break;
			case MATCH_NONE:
			default:
				MatchLabel = QNew (com_levelPool, 0) CMatchModeLabel(this, x, y);
				MatchLabel->LabelString = "Switch to match mode";
				break;
			};

			MatchLabel->Enabled = true;
			MatchLabel->Align = LA_LEFT;
		}

		y += 8 * 2;
		CCloseLabel *CloseLabel = QNew (com_levelPool, 0) CCloseLabel(this, x, y);
		CloseLabel->Enabled = true;
		CloseLabel->Align = LA_LEFT;
		CloseLabel->LabelString = "Close";

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

void CTFAdmin(CPlayerEntity *ent)
{
	char text[1024];

	if (ArgCount() > 1 && admin_password->String() && *admin_password->String() &&
		!ent->Client.Respawn.CTF.Admin && strcmp(admin_password->String(), ArgGets(1).c_str()) == 0)
	{
		ent->Client.Respawn.CTF.Admin = true;
		BroadcastPrintf(PRINT_HIGH, "%s has become an admin.\n", ent->Client.Persistent.Name.c_str());
		ent->PrintToClient (PRINT_HIGH, "Type 'admin' to access the adminstration menu.\n");

		return;
	}

	if (!ent->Client.Respawn.CTF.Admin)
	{
		Q_snprintfz(text, sizeof(text), "%s has requested admin rights.",
			ent->Client.Persistent.Name.c_str());
		CTFBeginElection(ent, ELECT_ADMIN, text);
		return;
	}

	if (ent->Client.Respawn.MenuState.InMenu)
		return;

	ent->Client.Respawn.MenuState.CurrentMenu = QNew (com_levelPool, 0) CCTFAdminMenu(ent);
	ent->Client.Respawn.MenuState.OpenMenu ();

	ent->Client.Respawn.MenuState.CurrentMenu->Draw (true);
}
#endif