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
// cc_cvar.cpp
// A small wrapper for cvars that make managing and using cvars a bit easier.
//

#include "cc_local.h"

_CC_DISABLE_DEPRECATION

CCvar::CCvar ()
{
	cVar = NULL;
	mainValue = NULL;
	floatVal = intVal = 0;
}
CCvar::CCvar (char *cvarName, char *defaultValue, int flags)
{
	CCvar();
	Register(cvarName, defaultValue, flags);
}
CCvar::CCvar (char *cvarName, int defaultValue, int flags)
{
	CCvar();
	Register(cvarName, defaultValue, flags);
}
CCvar::CCvar (char *cvarName, float defaultValue, int flags)
{
	CCvar();
	Register(cvarName, defaultValue, flags);
}

void CCvar::Update()
{
	mainValue = cVar->string;
	floatVal = cVar->floatVal;
	intVal = floatVal;
}

void CCvar::Register(char *cvarName, char *defaultValue, int flags)
{
	cVar = gi.cvar (cvarName, defaultValue, flags);

	mainValue = cVar->string;
	floatVal = cVar->floatVal;
	intVal = floatVal;
}

void CCvar::Register(char *cvarName, float defaultValue, int flags)
{
	// I know these temporary alloc buffers seem a bit wonky. Let me know if anyone has a cleaner solution.
	char tempAlloc[100];
	Q_snprintfz(tempAlloc, defaultValue, "%f", defaultValue);

	cVar = gi.cvar (cvarName, tempAlloc, flags);

	mainValue = cVar->string;
	floatVal = cVar->floatVal;
	intVal = floatVal;
}

void CCvar::Register(char *cvarName, int defaultValue, int flags)
{
	char tempAlloc[15];
	Q_snprintfz(tempAlloc, sizeof(tempAlloc), "%i", defaultValue);

	cVar = gi.cvar (cvarName, tempAlloc, flags);

	mainValue = cVar->string;
	floatVal = cVar->floatVal;
	intVal = floatVal;
}

void CCvar::Set (char *value, bool Force)
{
	if (!Force)
		cVar = gi.cvar_set (cVar->name, value);
	else
		cVar = gi.cvar_forceset (cVar->name, value);
}

void CCvar::Set (float value, bool Force)
{
	char tempAlloc[100];
	Q_snprintfz(tempAlloc, value, "%f", value);

	if (!Force)
		cVar = gi.cvar_set (cVar->name, tempAlloc);
	else
		cVar = gi.cvar_forceset (cVar->name, tempAlloc);
}

void CCvar::Set (int value, bool Force)
{
	char tempAlloc[15];
	Q_snprintfz(tempAlloc, value, "%i", value);

	if (!Force)
		cVar = gi.cvar_set (cVar->name, tempAlloc);
	else
		cVar = gi.cvar_forceset (cVar->name, tempAlloc);
}

float CCvar::Float ()
{
	Update ();
	return floatVal;
}

int CCvar::Integer ()
{
	Update ();
	return intVal;
}

char *CCvar::String ()
{
	Update ();
	return mainValue;
}

bool CCvar::Boolean (bool MustBeOne)
{
	Update ();
	return (MustBeOne ? (Integer() == 1) : (Float() != 0));
}

bool CCvar::Modified ()
{
	bool modified = (cVar->modified == 1);

	cVar->modified = 0;
	return modified;
}

_CC_ENABLE_DEPRECATION