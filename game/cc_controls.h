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
// cc_controls.h
// Common controls
//

enum ELabelFlags
{
	LF_GREEN			=	1,
};

enum ELabelAlign
{
	LA_LEFT,
	LA_CENTER,
	LA_RIGHT
};

class CMenu_Label : public CMenuItem
{
public:
	char					LabelString[MAX_COMPRINT/4];
	ELabelFlags				Flags;
	ELabelAlign				Align;

	CMenu_Label				(CMenu *Menu, int x, int y);
	virtual void Draw		(CPlayerEntity *ent, CStatusBar *DrawState);

	virtual bool	CanSelect (CPlayerEntity *ent)
	{
		return Enabled;
	}
	virtual bool	Select (CPlayerEntity *ent)
	{
		return false;
	}
	virtual void	Update (CPlayerEntity *ent)
	{
	}
};

class CMenu_Image : public CMenuItem
{
public:
	char					ImageString[MAX_INFO_KEY*2];
	int						Width, Height;

	CMenu_Image				(CMenu *Menu, int x, int y);
	virtual void Draw		(CPlayerEntity *ent, CStatusBar *DrawState);

	virtual bool	CanSelect (CPlayerEntity *ent)
	{
		return Enabled;
	}
	virtual bool	Select (CPlayerEntity *ent)
	{
		return false;
	}
	virtual void	Update (CPlayerEntity *ent)
	{
	}
};

typedef struct
{
	char		*Text;
	char		*Value;
} SSpinControlIndex;

class CMenu_Spin : public CMenuItem
{
public:
	ELabelFlags				Flags;
	ELabelAlign				Align;
	int						Index; // Where we are in the spin control
	int						NumIndices;
	SSpinControlIndex		*Indices;

	CMenu_Spin				(CMenu *Menu, int x, int y, SSpinControlIndex *Indices);
	virtual void Draw		(CPlayerEntity *ent, CStatusBar *DrawState);

	virtual bool	CanSelect (CPlayerEntity *ent)
	{
		return Enabled;
	}
	virtual bool	Select (CPlayerEntity *ent)
	{
		return false;
	}
	virtual void	Update (CPlayerEntity *ent);
};

typedef int ESliderTextPosition;
enum
{
	STP_RIGHT,
	STP_TOP,
	STP_LEFT,
	STP_BOTTOM,

	STP_CUSTOM // Allows programmer to shove the text where ever he wants
};

class CMenu_Slider : public CMenuItem
{
public:
	ELabelAlign				Align;
	ESliderTextPosition		TextAlign;
	int						TextX, TextY;

	int						Min;
	int						Max;
	int						Step;
	int						Value;
	int						Width;

	CMenu_Slider			(CMenu *Menu, int x, int y);
	virtual void Draw		(CPlayerEntity *ent, CStatusBar *DrawState);

	virtual bool	CanSelect (CPlayerEntity *ent)
	{
		return Enabled;
	}
	virtual bool	Select (CPlayerEntity *ent)
	{
		return false;
	}
	virtual void	Update (CPlayerEntity *ent);
};

class CMenu_Box : public CMenuItem
{
private:
	bool					Enabled;

public:
	ELabelAlign				Align;

	int						Width;
	int						Height;
	int						Type;

	CMenu_Box			(CMenu *Menu, int x, int y);
	virtual void Draw		(CPlayerEntity *ent, CStatusBar *DrawState);

	// Can't select
	bool	CanSelect (CPlayerEntity *ent)
	{
		return false;
	}
	bool	Select (CPlayerEntity *ent)
	{
		return false;
	}
	virtual void	Update (CPlayerEntity *ent)
	{
	};
};