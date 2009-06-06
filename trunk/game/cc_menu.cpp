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
// cc_menu.cpp
// Menu system
//

#include "cc_local.h"

CMenuItem::CMenuItem (CMenu *Menu, int x, int y) :
Menu(Menu),
x(x),
y(y),
Selected(Selected),
Enabled(true)
{
};

bool CMenuItem::CanSelect (edict_t *ent)
{
	return Enabled;
}

void CMenuItem::Update (edict_t *ent)
{
}

bool CMenuItem::Select (edict_t *ent)
{
	return false;
}

CMenu::CMenu (edict_t *ent) :
Cursor(0),
ent(ent)
{
};

CMenu::~CMenu ()
{
	for (uint32 i = 0; i < Items.size(); i++)
	{
		CMenuItem *Item = Items[i];
		delete Item;
	}
};

void CMenu::AddItem (CMenuItem *Item)
{
	// Insert in the list
	Items.push_back (Item);
}

void CMenu::DrawItems (CStatusBar *Bar)
{
	for (std::vector<CMenuItem*>::iterator it = Items.begin(); it < Items.end(); it++)
		(*it)->Draw (ent, Bar);
}

CMenuState::CMenuState ()
{
};

CMenuState::CMenuState (edict_t *ent) :
ent(ent),
Cursor(-1),
CurrentMenu(NULL),
InMenu(false)
{
};

void CMenuState::OpenMenu ()
{
	if (!CurrentMenu)
	{
		DebugPrintf ("CMenuState::OpenMenu called with no menu!\n");
		return;
	}

	if (!CurrentMenu->Open ())
	{
		// No dice
		delete CurrentMenu;
		return;
	}

	// Menu successfully opened!
	Cursor = CurrentMenu->Cursor;
	InMenu = true;

	CurrentMenu->Items.at(Cursor)->Selected = true;
};

void CMenuState::CloseMenu ()
{
	if (!CurrentMenu)
	{
		DebugPrintf ("CMenuState::CloseMenu called with no menu!\n");
		return;
	}

	CurrentMenu->Close (); // Shut all of it down
	delete CurrentMenu; // Delete it

	Cursor = -1;
	InMenu = false;
}

void CMenuState::SelectNext ()
{
	int i = 0;

	if (!CurrentMenu)
	{
		DebugPrintf ("CMenuState::SelectNext called with no menu!\n");
		return;
	}

	// Unselect what is selected currently
	if (Cursor != -1)
		CurrentMenu->Items.at(Cursor)->Selected = false;

	// Find the next selectable item
	if (Cursor == -1)
		Cursor = 0; // Temporary

	for (i = Cursor; i < (signed)CurrentMenu->Items.size(); i++)
	{
		if (i == Cursor)
			continue;

		CMenuItem *Item = CurrentMenu->Items.at(i);
		if (Item && Item->CanSelect(ent))
		{
			Cursor = i;
			CurrentMenu->Items.at(Cursor)->Selected = true;
			return;
		}
	}
	
	// Couldn't find an item after cursor
	// Try from 0
	for (i = 0; i < (signed)CurrentMenu->Items.size(); i++)
	{
		if (i == Cursor)
			continue;

		CMenuItem *Item = CurrentMenu->Items.at(i);
		if (Item && Item->CanSelect(ent))
		{
			Cursor = i;
			CurrentMenu->Items.at(Cursor)->Selected = true;
			return;
		}
	}

	// Couldn't find any item
	Cursor = -1;
	return;
}

void CMenuState::SelectPrev ()
{
	int i = 0;

	if (!CurrentMenu)
	{
		DebugPrintf ("CMenuState::SelectPrev called with no menu!\n");
		return;
	}

	// Unselect what is selected currently
	if (Cursor != -1)
		CurrentMenu->Items.at(Cursor)->Selected = false;

	// Find the previous selectable item
	if (Cursor == -1)
		Cursor = 0; // Temporary

	for (i = Cursor; i >= 0; i--)
	{
		if (i == Cursor)
			continue;

		CMenuItem *Item = CurrentMenu->Items.at(i);
		if (Item && Item->CanSelect(ent))
		{
			Cursor = i;
			CurrentMenu->Items.at(Cursor)->Selected = true;
			return;
		}
	}

	// Couldn't find an item before cursor
	// Try from end
	for (i = (signed)CurrentMenu->Items.size()-1; i >= 0; i--)
	{
		if (i == Cursor)
			continue;

		CMenuItem *Item = CurrentMenu->Items.at(i);
		if (Item && Item->CanSelect(ent))
		{
			Cursor = i;
			CurrentMenu->Items.at(Cursor)->Selected = true;
			return;
		}
	}

	// Couldn't find any item
	Cursor = -1;
	return;
}

void CMenuState::Select ()
{
	if (!CurrentMenu)
	{
		DebugPrintf ("CMenuState::Select called with no menu!\n");
		return;
	}

	if (Cursor == -1)
		return;

	if (Cursor > (signed)(CurrentMenu->Items.size()-1))
	{
		DebugPrintf ("Funny cursor size\n");
		return;
	}

	if (CurrentMenu->Items.at(Cursor)->Select(ent))
		CloseMenu ();
}

void Cmd_Kill_f (edict_t *ent);

static SSpinControlIndex AmmoIndexes[] =
{
	{"Shells", "Shells"},
	{"Bullets", "Bullets"},
	{"Cells", "Cells"},
	{"Rockets", "Rockets"},
	{"Slugs", "Slugs"},
	{"Grenades", "Grenades"},
	NULL
};

#if 0
class CTestMenu : public CMenu
{
public:
	CMenu_Label		*Label1, *Label2;
	CMenu_Spin		*Spin1;
	CMenu_Slider	*Slider1;

	class CLabel1 : public CMenu_Label
	{
	public:
		CTestMenu	*Menu;
		CLabel1 (CTestMenu *Menu, int x, int y) :
		CMenu_Label(Menu,x,y),
		Menu(Menu)
		{
		};

		bool	Select (edict_t *ent)
		{
			CAmmo *Ammo = dynamic_cast<CAmmo*>(FindItem(Menu->Spin1->Indices[Menu->Spin1->Index].Text));

			if (Ammo)
			{
				edict_t *it_ent = G_Spawn();
				it_ent->classname = Ammo->Classname;
				SpawnItem (it_ent, Ammo);
				it_ent->count = Menu->Slider1->Value;
				TouchItem (it_ent, ent, NULL, NULL);
				if (it_ent->inUse)
					G_FreeEdict(it_ent);
			}
			return true;
		}
	};

	CTestMenu			(edict_t *ent) :
	CMenu(ent)
	{
		Cursor = 3;
	};

	bool				Open ()
	{
		Label1 = new CMenu_Label (this, 0, -48);
		Label1->LabelString = "Pick an ammo and amount...\n\"menu_left\" and \"menu_right\" to choose";
		Label1->Align = LA_CENTER;
		Label1->Enabled = false;
		AddItem(Label1);

		CMenu_Box *Box1 = new CMenu_Box (this, 0, -40);
		Box1->Width = 11;
		Box1->Type = 2;
		Box1->Align = LA_CENTER;
		AddItem (Box1);

		Box1 = new CMenu_Box (this, 0, -12);
		Box1->Width = 11;
		Box1->Height = 4;
		Box1->Type = 1;
		Box1->Align = LA_CENTER;
		AddItem (Box1);

		Spin1 = new CMenu_Spin (this, 0, -8, AmmoIndexes);
		Spin1->Align = LA_CENTER;
		AddItem (Spin1);

		Slider1 = new CMenu_Slider (this, -8, 8);
		Slider1->Align = LA_CENTER;
		Slider1->Min = 0;
		Slider1->Max = 200;
		Slider1->Width = 8;
		Slider1->Step = 5;
		Slider1->Value = 0;
		AddItem(Slider1);

		Label2 = new CLabel1 (this, 0, 24);
		Label2->LabelString = "Give!";
		Label2->Align = LA_CENTER;
		AddItem(Label2);

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

void OpenTestMenu (edict_t *ent)
{
	if (ent->client->resp.MenuState.InMenu)
		return;

	ent->client->resp.MenuState.CurrentMenu = new CTestMenu(ent);
	ent->client->resp.MenuState.OpenMenu ();

	ent->client->resp.MenuState.CurrentMenu->Draw (true);
}
#endif

void Cmd_MenuLeft_t (edict_t *ent)
{
	if (!ent->client->resp.MenuState.InMenu)
		return;

	ent->client->resp.MenuState.Key = KEY_LEFT;

	// Update the currently selected control
	if (ent->client->resp.MenuState.Cursor != -1)
		ent->client->resp.MenuState.CurrentMenu->Items.at(ent->client->resp.MenuState.Cursor)->Update(ent);

	ent->client->resp.MenuState.Key = KEY_NONE;
}

void Cmd_MenuRight_t (edict_t *ent)
{
	if (!ent->client->resp.MenuState.InMenu)
		return;

	ent->client->resp.MenuState.Key = KEY_RIGHT;

	// Update the currently selected control
	if (ent->client->resp.MenuState.Cursor != -1)
		ent->client->resp.MenuState.CurrentMenu->Items.at(ent->client->resp.MenuState.Cursor)->Update(ent);

	ent->client->resp.MenuState.Key = KEY_NONE;
}
