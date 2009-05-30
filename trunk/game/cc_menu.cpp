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
void TouchItem (edict_t *ent, edict_t *other, plane_t *plane, cmBspSurface_t *surf);

SSpinControlIndex TestIndexes[] =
{
	{"Yes", "1"},
	{"No", "0"},
	{"Maybe", "?"}
};

class CTestMenu : public CMenu
{
public:
	class Label1 : public CMenu_Label
	{
	public:
		Label1 (CMenu *Menu, int x, int y) :
		CMenu_Label(Menu,x,y)
		{
		};

		bool	Select (edict_t *ent)
		{
			Cmd_Kill_f (ent);
			return true;
		}
	};
	class Label2 : public CMenu_Label
	{
	public:
		Label2 (CMenu *Menu, int x, int y) :
		CMenu_Label(Menu,x,y)
		{
		};

		bool	Select (edict_t *ent)
		{
			edict_t *it_ent = G_Spawn();
			CBaseItem *it = FindItem("Shotgun");
			it_ent->classname = it->Classname;
			SpawnItem (it_ent, it);
			TouchItem (it_ent, ent, NULL, NULL);
			if (it_ent->inUse)
				G_FreeEdict(it_ent);
			return true;
		}
	};
	class Label3 : public CMenu_Label
	{
	public:
		Label3 (CMenu *Menu, int x, int y) :
		CMenu_Label(Menu,x,y)
		{
		};

		bool	Select (edict_t *ent)
		{
			return false;
		}
	};
	class Image1 : public CMenu_Image
	{
	public:
		Image1 (CMenu *Menu, int x, int y) :
		CMenu_Image(Menu,x,y)
		{
		};

		bool	CanSelect (edict_t *ent)
		{
			return false;
		}
	};

	CTestMenu			(edict_t *ent) :
	CMenu(ent)
	{
	};

	bool				Open ()
	{
		CMenu_Label *Label = new Label1(this, 0, 0);
		Label->Align = LA_CENTER;
		Q_snprintfz (Label->LabelString, sizeof(Label->LabelString), "Kill self");
		AddItem(Label);

		Label = new Label2(this, 0, 8);
		Label->Align = LA_LEFT;
		Q_snprintfz (Label->LabelString, sizeof(Label->LabelString), "Gimme a shotgun");
		AddItem(Label);

		Label = new Label3(this, 0, 16);
		Label->Align = LA_RIGHT;
		Q_snprintfz (Label->LabelString, sizeof(Label->LabelString), "Don't do anything");
		AddItem(Label);

		Label = new Label3(this, 0, 32);
		Label->Align = LA_CENTER;
		Label->Enabled = false;
		Q_snprintfz (Label->LabelString, sizeof(Label->LabelString), "This multiline\nstring cannot be\nselected.");
		AddItem(Label);

		CMenu_Image *Image = new Image1 (this, -12, -24);
		Q_snprintfz (Image->ImageString, sizeof(Image->ImageString), "k_security");
		AddItem(Image);

		CMenu_Spin *SpinTest = new CMenu_Spin (this, 0, 64, TestIndexes);
		AddItem(SpinTest);
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
