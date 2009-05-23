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
// cc_indexing.cpp
// New indexing system
//

#include "cc_local.h"

CIndexList	ModelList, SoundList, ImageList;

CIndexList::CIndexList () :
numIndexes(0)
{
	memset (List, 0, sizeof(CIndex*) * MAX_INDEXES);
	memset (HashList, 0, sizeof(CIndex*) * MAX_INDEXES);
}

int CIndexList::GetIndex (char *String)
{
	// Check through the itemlist
	uint32 hash = Com_HashGeneric(String, MAX_INDEXES);
	CIndex *Index;

	for (Index = HashList[hash]; Index; Index=Index->HashNext)
	{
		if (Q_stricmp(Index->Name, String) == 0)
			return Index->Index;
	}
	return -1;
}

int CIndexList::AddToList (char *String, int Index)
{
	CIndex *NewIndex = new CIndex;
	NewIndex->Index = Index;
	NewIndex->Name = new char[strlen(String)+1];
	NewIndex->HashValue = Com_HashGeneric (String, MAX_INDEXES);

	Q_snprintfz(NewIndex->Name, strlen(String)+1, "%s", String);

	List[numIndexes++] = NewIndex;

	NewIndex->HashNext = HashList[NewIndex->HashValue];
	HashList[NewIndex->HashValue] = NewIndex;

	return Index;
}

_CC_DISABLE_DEPRECATION
(
int ModelIndex (char *String)
{
	// Do we exist?
	int Index = ModelList.GetIndex(String);
	if (Index == -1)
		return ModelList.AddToList (String, gi.modelindex(String));
	return Index;
}

int SoundIndex (char *String)
{
	// Do we exist?
	int Index = SoundList.GetIndex(String);
	if (Index == -1)
		return SoundList.AddToList (String, gi.soundindex(String));
	return Index;
}

int ImageIndex (char *String)
{
	// Do we exist?
	int Index = ImageList.GetIndex(String);
	if (Index == -1)
		return ImageList.AddToList (String, gi.imageindex(String));
	return Index;
}
)

void CIndexList::Clear ()
{
	numIndexes = 0;
	memset (List, 0, sizeof(CIndex*) * MAX_INDEXES);
	memset (HashList, 0, sizeof(CIndex*) * MAX_INDEXES);
}

void ClearList ()
{
	ModelList.Clear();
	ImageList.Clear();
	SoundList.Clear();
}