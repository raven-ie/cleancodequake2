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
// cc_controls.cpp
// Common controls
//

#include "cc_local.h"

CMenu_Label::CMenu_Label (CMenu *Menu, int x, int y) :
CMenuItem(Menu, x, y)
{
};

void CMenu_Label::Draw (edict_t *ent, CStatusBar *DrawState)
{
	bool high = (Flags & LF_GREEN);
	DrawState->AddVirtualPoint_Y (y + 120);

	if (Selected)
	{
		char arrowThing[2] = {CCHAR_ARROW,'\0'};
		int arrowX = x;

		switch (Align)
		{
		case LA_LEFT:
			arrowX += (160 - 12);
			break;
		case LA_CENTER:
			arrowX -= 8 + ((strlen(LabelString)*8)/2);
			break;
		case LA_RIGHT:
			arrowX += (160 - (strlen(LabelString)*8)) - 12;
			break;
		}
		DrawState->AddVirtualPoint_X (arrowX);

		DrawState->AddString (arrowThing, true, (Align == LA_CENTER));
		high = true;
	}
	int drawX = x;

	switch (Align)
	{
	case LA_LEFT:
		drawX += 160;
		break;
	case LA_CENTER:
		break;
	case LA_RIGHT:
		drawX += 160 - (strlen(LabelString)*8);
		break;
	}

	DrawState->AddVirtualPoint_X (drawX);
	DrawState->AddString (LabelString, high, (Align == LA_CENTER));
};

CMenu_Image::CMenu_Image (CMenu *Menu, int x, int y) :
CMenuItem(Menu, x, y)
{
};

void CMenu_Image::Draw (edict_t *ent, CStatusBar *DrawState)
{
	DrawState->AddVirtualPoint_X ((x + 160) - Width/2);
	DrawState->AddVirtualPoint_Y ((y + 120) - Height/2);
	DrawState->AddPic (ImageString);

	if (Selected)
	{
		DrawState->AddVirtualPoint_X ((x + 160) - Width);		
		char arrowThing[2] = {CCHAR_ARROW,'\0'};
		DrawState->AddString (arrowThing, true, false);
	}
};

CMenu_Spin::CMenu_Spin (CMenu *Menu, int x, int y, SSpinControlIndex *Indices) :
CMenuItem(Menu, x, y),
Indices(Indices)
{
	Index = 0;
	NumIndices = (sizeof(Indices) / sizeof(SSpinControlIndex));
};

void CMenu_Spin::Draw (edict_t *ent, CStatusBar *DrawState)
{
	DrawState->AddVirtualPoint_X (x + 160);
	DrawState->AddVirtualPoint_Y (y + 120);

	DrawState->AddString (Indices[Index].Text, Selected, false);

	if (Selected)
	{
		// Is there any more indices to the left?
		if (Index > 0)
		{
			DrawState->AddVirtualPoint_X (x + 160 - (((strlen(Indices[Index].Text)*8)/2)-8));
			DrawState->AddString ("<", false, false);
		}

		// To the right?
		if (Index < (NumIndices-1))
		{
			DrawState->AddVirtualPoint_X (x + 160 + (((strlen(Indices[Index].Text)*8)/2)+8));
			DrawState->AddString (">", false, false);
		}
	}
};

void CMenu_Spin::Update (edict_t *ent)
{
	switch (ent->client->resp.MenuState.Key)
	{
	case KEY_RIGHT:
		if (Index == (NumIndices-1))
			return; // Can't do that, Dave

		Index++;
		break;
	case KEY_LEFT:
		if (Index == 0)
			return;

		Index--;
		break;
	}
};