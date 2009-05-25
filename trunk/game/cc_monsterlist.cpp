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
// cc_monsterlist.cpp
// Monster list (for maps)
//

#include "cc_local.h"

CMonsterList MonsterList;

CMonsterList::CMonsterList () :
numMonsters(0)
{
	memset (List, 0, sizeof(CMonster*) * MAX_MONSTERS);
	memset (HashedList, 0, sizeof(CMonster*) * MAX_MONSTERS_HASHED);
};

void CMonsterList::AddMonsterToList (CMonster *Monster)
{
	if (!Monster || !Monster->Classname)
		return;

	List[numMonsters++] = Monster;

	// Fill the hash table
	Monster->HashValue = Com_HashGeneric(Monster->Classname, MAX_MONSTERS_HASHED);
	Monster->HashNext = HashedList[Monster->HashValue];
	HashedList[Monster->HashValue] = Monster;
}

CMonster *CMonsterList::MonsterExists (char *Classname)
{
	// Check through the itemlist
	uint32 hash = Com_HashGeneric(Classname, MAX_MONSTERS_HASHED);
	CMonster *Monster;

	for (Monster = HashedList[hash]; Monster; Monster=Monster->HashNext)
	{
		if (Q_stricmp(Classname, Monster->Classname) == 0)
			return Monster;
	}
	return NULL;
}

CMonster *FindMonster (char *Classname)
{
	return MonsterList.MonsterExists(Classname);
}

void InitMonsterList ()
{
	MonsterList.AddMonsterToList (&Monster_Infantry);
	MonsterList.AddMonsterToList (&Monster_Bitch);
	MonsterList.AddMonsterToList (&Monster_Soldier_Light);
	MonsterList.AddMonsterToList (&Monster_Soldier_Shotgun);
	MonsterList.AddMonsterToList (&Monster_Soldier_Machinegun);
	MonsterList.AddMonsterToList (&Monster_Flyer);
	MonsterList.AddMonsterToList (&Monster_Tank);
	MonsterList.AddMonsterToList (&Monster_Tank_Commander);
	MonsterList.AddMonsterToList (&Monster_Gunner);
	MonsterList.AddMonsterToList (&Monster_Floater);
	MonsterList.AddMonsterToList (&Monster_SuperTank);
	MonsterList.AddMonsterToList (&Monster_Shark);
	MonsterList.AddMonsterToList (&Monster_Icarus);
}