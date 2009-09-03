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
// cc_servercommands.cpp
// ServerCommand & related functions
//

#include "cc_local.h"

void CServerCmd::Run ()
{
	RunFunction ();
};

CServerCmd::CServerCmd (char *name, void (*Func)())
{
	cmdName = QNew (com_gamePool, 0) char[strlen(name)+1];//(char*)gi.TagMalloc(strlen(name)+1, TAG_GAME);
	Q_snprintfz (cmdName, strlen(name)+1, "%s", name);
	hashValue = Com_HashGeneric(name, MAX_CMD_HASH);
	RunFunction = Func;
};

CServerCmd::~CServerCmd ()
{
	QDelete cmdName;//gi.TagFree(cmdName);
};

CServerCmd *ServerCommandList[MAX_COMMANDS];
CServerCmd *ServerCommandHashList[MAX_CMD_HASH];
int numServerCommands = 0;

CServerCmd *SvCmd_FindCommand (char *commandName)
{
	CServerCmd *Command;
	uint32 hash = Com_HashGeneric(commandName, MAX_CMD_HASH);

	for (Command = ServerCommandHashList[hash]; Command; Command=Command->hashNext)
	{
		if (Q_stricmp(Command->cmdName, commandName) == 0)
			return Command;
	}
	return NULL;
}

void SvCmd_AddCommand (char *commandName, void (*Func) ())
{
	// Make sure the function doesn't already exist
	if (SvCmd_FindCommand(commandName))
	{
		Com_Printf (0, "%s already exists as a command!\n", commandName);
		return;
	}

	// We can add it!
	ServerCommandList[numServerCommands] = QNew (com_gamePool, 0) CServerCmd (commandName, Func);
	// Link it in the hash tree
	ServerCommandList[numServerCommands]->hashNext = ServerCommandHashList[ServerCommandList[numServerCommands]->hashValue];
	ServerCommandHashList[ServerCommandList[numServerCommands]->hashValue] = ServerCommandList[numServerCommands];
	numServerCommands++;
}

void SvCmd_RemoveCommands ()
{
	// Remove all commands
	for (int i = 0; i < numServerCommands; i++)
	{
		QDelete ServerCommandList[numServerCommands];
		numServerCommands--;
	}
}

void SvCmd_RunCommand (char *commandName)
{
	CServerCmd *Command = SvCmd_FindCommand(commandName);

	if (Command)
		Command->Run();
	else
		Com_Printf (0, "Unknown server command \"%s\"\n", commandName);
}

typedef struct SServerEntityListIndex_s
{
	const char	*className;
	uint32		Num;
	bool		Old;

	struct SServerEntityListIndex_s	*hashNext;
	uint32							hashValue;

	SServerEntityListIndex_s (const char *name) :
		className(name),
		hashValue (Com_HashGeneric(className, MAX_CS_EDICTS)),
		Num (0),
		Old (true)
	{
	};
} SServerEntityListIndex;

class CServerEntityList
{
public:
	SServerEntityListIndex		*List[MAX_CS_EDICTS];
	SServerEntityListIndex		*HashedList[MAX_CS_EDICTS];
	uint32						NumInList;

	CServerEntityList () :
		NumInList (0)
	{
		memset (List, 0, sizeof(List));
		memset (HashedList, 0, sizeof(HashedList));
	};

	~CServerEntityList ()
	{
		for (uint32 i = 0; i < NumInList; i++)
			QDelete List[i];
	};

	SServerEntityListIndex *Exists (const char *className)
	{
		uint32 Hash = Com_HashGeneric (className, MAX_CS_EDICTS);

		for (SServerEntityListIndex *Index = HashedList[Hash]; Index; Index = Index->hashNext)
		{
			if (Q_stricmp(Index->className, className) == 0)
				return Index;
		}
		return NULL;
	};

	SServerEntityListIndex *AddToList (const char *className)
	{
		SServerEntityListIndex *Ind = new SServerEntityListIndex(className);
		List[NumInList] = Ind;
		List[NumInList]->hashNext = HashedList[List[NumInList]->hashValue];
		HashedList[List[NumInList]->hashValue] = List[NumInList];
		NumInList++;

		return Ind;
	};

	void Search ()
	{
		for (int i = 0; i < globals.numEdicts; i++)
		{
			edict_t *e = (&g_edicts[i]);
			if (!e->inUse)
				continue;
			
			SServerEntityListIndex *Index = Exists(e->classname);
			if (!Index)
			{
				Index = AddToList (e->classname);
				if (e->Entity)
					Index->Old = false;
			}

			Index->Num++;
		}
	};
};
void SvCmd_EntList_f ()
{
	/*
	int numOld = 0, numNew = 0;
	for (int i = 0; i < globals.numEdicts; i++)
	{
		edict_t *e = (&g_edicts[i]);
		if (!e->inUse)
			continue;

		if (!e->Entity)
			numOld++;
		else
			numNew++;
	}
	int newPerc = (int)((float)(numNew) / ((float)numOld + (float)numNew) * 100);
	int oldPerc = 100 - newPerc;
	DebugPrintf ("Clean Entities: %i (%i%%)\nOld Entities: %i (%i%%)\nTotal Entities: %i\n", numNew, newPerc, numOld, oldPerc, numOld+numNew);
	*/
	CServerEntityList tmp;
	tmp.Search ();

	DebugPrintf ("Entity Stats\n"
			"      old        amount       classname\n"
			"----  ---        ------       --------------------------\n");

	uint32 oldCount = 0, newCount = 0;
	for (uint32 i = 0; i < tmp.NumInList; i++)
	{
		DebugPrintf (
			"#%3u  %s         %5u       %s\n",
			i, (tmp.List[i]->Old) ? "Yes" : "No ", tmp.List[i]->Num, tmp.List[i]->className);

		if (tmp.List[i]->Old)
			oldCount += tmp.List[i]->Num;
		else
			newCount += tmp.List[i]->Num;
	}

	DebugPrintf ("----  ---        ------       --------------------------\n");
	DebugPrintf ("Tally Old:       %5u          %5.0f%%\n", oldCount, (!oldCount) ? 0 : (float)oldCount / ((float)(oldCount + newCount)) * 100);
	DebugPrintf ("Tally New:       %5u          %5.0f%%\n", newCount, (!newCount) ? 0 : (float)newCount / ((float)(oldCount + newCount)) * 100);
}

extern char *gEntString;
void SvCmd_Dump_f ()
{
	std::string FileName = "/maps/ents/" + std::string(level.mapname) + ".ccent";
	fileHandle_t f;

	FS_OpenFile (FileName.c_str(), &f, FS_MODE_WRITE_BINARY);

	if (!f)
		return;

	FS_Write (gEntString, strlen(gEntString), f);

	FS_CloseFile (f);
}

void SvCmd_Register ()
{
	SvCmd_AddCommand ("entlist", SvCmd_EntList_f);
	SvCmd_AddCommand ("dump", SvCmd_Dump_f);
}

/*
=================
ServerCommand

ServerCommand will be called when an "sv" command is issued.
The game can issue gi.argc() / gi.argv() commands to get the rest
of the parameters
=================
*/
void ServerCommand (void)
{
	InitArg ();
	SvCmd_RunCommand (ArgGets(1));
	EndArg ();
}

