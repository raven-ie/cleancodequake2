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
// cc_arg.cpp
// Wrapper for the argv/s/c system
//

#include "cc_local.h"

#define MAX_ARG_CONCAT	256
#define MAX_ARG		64

char	*nullArg = "";

int		numArgv;
char	*argvConcatString;
char	*argvStringArray[MAX_ARG];
int		argvIntegerArray[MAX_ARG];
float	argvFloatArray[MAX_ARG];

void SetupArg ()
{
	numArgv = 0;
	memset (argvStringArray, 0, sizeof(argvStringArray));
	memset (argvIntegerArray, 0, sizeof(argvIntegerArray));
	memset (argvFloatArray, 0, sizeof(argvFloatArray));
	argvConcatString = NULL;
}

// Called at the beginning of an arg session
void InitArg ()
{
_CC_DISABLE_DEPRECATION
(
	numArgv = gi.argc();

	if (numArgv == 0)
		return;

	argvConcatString = gi.args();

	for (int i = 0; i < numArgv; i++)
	{
		size_t size = strlen(gi.argv(i))+1;
		argvStringArray[i] = QNew (com_genericPool, 0) char[size];
		Q_snprintfz (argvStringArray[i], size, "%s", gi.argv(i));

		argvFloatArray[i] = atof (argvStringArray[i]);
		argvIntegerArray[i] = (int)argvFloatArray[i];
	}
);
}

// Called at the end of an arg session
void EndArg ()
{
	for (int i = 0; i < numArgv; i++)
		delete[] argvStringArray[i];

	SetupArg();
}

// Actual functions
int ArgCount ()
{
	return numArgv;
}

char *ArgGetConcatenatedString ()
{
	return argvConcatString;
}

char *ArgGets (int Index)
{
	return (Index >= numArgv) ? nullArg : argvStringArray[Index];
}

int ArgGeti (int Index)
{
	return (Index >= numArgv) ? 0 : argvIntegerArray[Index];
}

float ArgGetf (int Index)
{
	return (Index >= numArgv) ? 0 : argvFloatArray[Index];
}