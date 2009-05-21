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
// cc_ban.cpp
// Simple ban system
//

#include "cc_local.h"

CBanList	Bans;

IPAddress IPStringToArrays (const char *IP)
{
	IPAddress Address = {0,0,0,0};
	int i = 0, z = 0, c = 0;
	char tempAddress[3];

	while (i != 4)
	{
		if (IP[c] == '.' || IP[c] == 0)
		{
			if (z == 1)
			{
				tempAddress[2] = tempAddress[0];
				tempAddress[0] = tempAddress[1] = '0';
			}
			else if (z == 2)
			{
				tempAddress[2] = tempAddress[1];
				tempAddress[1] = tempAddress[0];
				tempAddress[0] = '0';
			}

			Address.adr[i++] = atoi(tempAddress);
			tempAddress[0] = tempAddress[1] = tempAddress[2] = 0;

			c++;
			z = 0;
			continue;
		}

		tempAddress[z++] = IP[c++];
	}

	return Address;
}

IPAddress IPStringToArrays (char *IP)
{
	IPAddress Address = {0,0,0,0};
	int i = 0, z = 0, c = 0;
	char tempAddress[3];

	while (i != 4)
	{
		if (IP[c] == '.' || IP[c] == 0)
		{
			if (z == 1)
			{
				tempAddress[2] = tempAddress[0];
				tempAddress[0] = tempAddress[1] = '0';
			}
			else if (z == 2)
			{
				tempAddress[2] = tempAddress[1];
				tempAddress[1] = tempAddress[0];
				tempAddress[0] = '0';
			}

			Address.adr[i++] = atoi(tempAddress);
			tempAddress[0] = tempAddress[1] = tempAddress[2] = 0;

			c++;
			z = 0;
			continue;
		}

		tempAddress[z++] = IP[c++];
	}

	return Address;
}

int ipcmp (IPAddress left, IPAddress right)
{
	for (int i = 0; i < 4; i++)
	{
		if (left.adr[i] != right.adr[i])
			return 1;
	}
	return 0;
}

// Ban list looks as follows:
/*
// Comments are ignored.
// First token needs to be either ip or name, followed by the IP address
// or name of the offender, respectively.
// Then flags! See cc_ban.h for flag numbers.
// Unknown flags are stored, but ignored by older versions.
name Paril 1
ip 252.6.10.6 5
*/

void CBanList::LoadFromFile ()
{
	FILE *fp;
	std::string fileName;

	fileName += CCvar("gamename", "").String();
	fileName += "/bans.lst"; // FIXME: customizable

#ifndef CRT_USE_UNDEPRECATED_FUNCTIONS
	fp = fopen (fileName.c_str(), "rb");
#else
	int errorVal = fopen_s (&fp, fileName.c_str(), "rb");
#endif

	if (!fp || errorVal)
		return;

	fseek (fp, 0, SEEK_END);
	size_t length = ftell(fp);
	fseek (fp, 0, SEEK_SET);

	char *temp = new char[length+1];
	fread (temp, length, sizeof(char), fp);
	temp[length] = 0;

	fclose (fp);

	std::string mainString = temp;
	std::string line, token;

	// Get the entire line
	int z = 0, c = 0, oc = 0;
	z = mainString.find_first_of("\n\0");
	line = mainString.substr(0, (z == -1) ? length : z);

	while (line[0])
	{
		// Parse the line.
		// Get the distance to the first character.
		oc = line.find_first_of(" \n\0", c);
		token = line.substr (c, oc-c);
		c = oc;
		while (token[0])
		{
			if (token[0] == '/' && token[1] == '/')
				break;

			if (token == "name")
			{
				// Get the distance to the next token.
				// find_first_of operates on length so we need to calc the length
				// of the next token as well.
				oc = line.find_first_of(" \n\0", ++c);
				token = line.substr (c, oc-c);
				c = oc;

				BanIndex *NewIndex = new BanIndex;
				NewIndex->IP = false;
				NewIndex->Name = new char[token.length()];
				Q_strncpyz (NewIndex->Name, token.c_str(), token.length());
				NewIndex->Name[token.length()] = 0;

				oc = line.find_first_of(" \n\0", ++c);
				token = line.substr (c, oc-c);
				c = oc;

				EBanTypeFlags Flags = atoi(token.c_str());

				NewIndex->Flags = Flags;

				BanList.push_back (NewIndex);
			}
			else if (token== "ip")
			{
				oc = line.find_first_of(" \n\0", ++c);
				token = line.substr (c, oc-c);
				c = oc;

				BanIndex *NewIndex = new BanIndex;
				NewIndex->IP = true;
				NewIndex->IPAddress = new IPAddress(IPStringToArrays(token.c_str()));

				oc = line.find_first_of(" \n\0", ++c);
				token = line.substr (c, oc-c);
				c = oc;

				EBanTypeFlags Flags = atoi(token.c_str());

				NewIndex->Flags = Flags;

				BanList.push_back (NewIndex);
			}

			if (c == -1)
				break;

			oc = line.find_first_of(" \n\0", ++c);
			token = line.substr (c, oc-c);
			c = oc;
		}

		c = 0;

		if (z == -1)
			break;

		oc = mainString.find_first_of("\n\0", ++z);
		line = mainString.substr (z, oc-z);
		z = oc;
		oc = 0;
	}

	// Free everything
	delete[] temp;
}

void CBanList::SaveList ()
{
	FILE *fp;
	std::string fileName;

	fileName += CCvar("gamename", "").String();
	fileName += "/bans.lst"; // FIXME: customizable

#ifndef CRT_USE_UNDEPRECATED_FUNCTIONS
	fp = fopen (fileName.c_str(), "w+");
#else
	int errorVal = fopen_s (&fp, fileName.c_str(), "w+");
#endif

	if (!fp || errorVal)
		return;

	for (size_t i = 0; i < BanList.size(); i++)
	{
		BanIndex *Index = BanList[i];

		fprintf (fp, "%s ", (Index->IP) ? "ip" : "name");
		if (Index->IP)
			fprintf (fp, "%c.%c.%c.%c ", Index->IPAddress->adr[0], Index->IPAddress->adr[1], Index->IPAddress->adr[2], Index->IPAddress->adr[3]);
		else
			fprintf (fp, "%s ", Index->Name);
		fprintf (fp, "%i\n", Index->Flags);
	}

	fclose(fp);
}

void CBanList::AddToList (IPAddress Adr, EBanTypeFlags Flags)
{
	BanIndex *NewIndex = new BanIndex;
	NewIndex->IP = true;
	NewIndex->IPAddress = new IPAddress(Adr);
	NewIndex->Flags = Flags;

	BanList.push_back (NewIndex);
}

void CBanList::AddToList (char *Name, EBanTypeFlags Flags)
{
	BanIndex *NewIndex = new BanIndex;
	NewIndex->IP = false;
	NewIndex->Name = new char[strlen(Name)];
	Q_strncpyz (NewIndex->Name, Name, sizeof(NewIndex->Name));
	NewIndex->Name[strlen(Name)] = 0;
	NewIndex->Flags = Flags;

	BanList.push_back (NewIndex);
}

void CBanList::RemoveFromList (IPAddress Adr)
{
	std::vector<BanIndex*>::iterator it;

	for ( it=BanList.begin() ; it < BanList.end(); it++ )
	{
		BanIndex *Index = *it;

		if (!Index->IP)
			continue;

		if (!ipcmp(Adr, *Index->IPAddress))
		{
			BanList.erase(it);
			break;
		}
	}
}

void CBanList::RemoveFromList (char *Name)
{
	std::vector<BanIndex*>::iterator it;

	for ( it=BanList.begin() ; it < BanList.end(); it++ )
	{
		BanIndex *Index = *it;

		if (Index->IP)
			continue;

		if (!strcmp(Name, Index->Name))
		{
			BanList.erase(it);
			break;
		}
	}
}

void CBanList::ChangeBan (IPAddress Adr, EBanTypeFlags Flags)
{
	std::vector<BanIndex*>::iterator it;

	for ( it=BanList.begin() ; it < BanList.end(); it++ )
	{
		BanIndex *Index = *it;

		if (!Index->IP)
			continue;

		if (!ipcmp(Adr, *Index->IPAddress))
		{
			Index->Flags = Flags;
			break;
		}
	}
}

void CBanList::ChangeBan (char *Name, EBanTypeFlags Flags)
{
	std::vector<BanIndex*>::iterator it;

	for ( it=BanList.begin() ; it < BanList.end(); it++ )
	{
		BanIndex *Index = *it;

		if (Index->IP)
			continue;

		if (!strcmp(Name, Index->Name))
		{
			Index->Flags = Flags;
			break;
		}
	}
}

bool CBanList::IsSquelched (IPAddress Adr)
{
	std::vector<BanIndex*>::iterator it;

	for ( it=BanList.begin() ; it < BanList.end(); it++ )
	{
		BanIndex *Index = *it;

		if (!Index->IP)
			continue;

		if (!ipcmp(Adr, *Index->IPAddress))
			return !!(Index->Flags & BAN_SQUELCH);
	}
	return false;
}

bool CBanList::IsBannedFromSpectator (IPAddress Adr)
{
	std::vector<BanIndex*>::iterator it;

	for ( it=BanList.begin() ; it < BanList.end(); it++ )
	{
		BanIndex *Index = *it;

		if (!Index->IP)
			continue;

		if (!ipcmp(Adr, *Index->IPAddress))
			return !!(Index->Flags & BAN_SPECTATOR);
	}
	return false;
}

bool CBanList::IsBanned (IPAddress Adr)
{
	std::vector<BanIndex*>::iterator it;

	for ( it=BanList.begin() ; it < BanList.end(); it++ )
	{
		BanIndex *Index = *it;

		if (!Index->IP)
			continue;

		if (!ipcmp(Adr, *Index->IPAddress))
			return !!(Index->Flags & BAN_ENTER);
	}
	return false;
}

bool CBanList::IsSquelched (char *Name)
{
	std::vector<BanIndex*>::iterator it;

	for ( it=BanList.begin() ; it < BanList.end(); it++ )
	{
		BanIndex *Index = *it;

		if (Index->IP)
			continue;

		if (!strcmp(Name, Index->Name))
			return !!(Index->Flags & BAN_SQUELCH);
	}
	return false;
}

bool CBanList::IsBannedFromSpectator (char *Name)
{
	std::vector<BanIndex*>::iterator it;

	for ( it=BanList.begin() ; it < BanList.end(); it++ )
	{
		BanIndex *Index = *it;

		if (Index->IP)
			continue;

		if (!strcmp(Name, Index->Name))
			return !!(Index->Flags & BAN_SPECTATOR);
	}
	return false;
}

bool CBanList::IsBanned (char *Name)
{
	std::vector<BanIndex*>::iterator it;

	for ( it=BanList.begin() ; it < BanList.end(); it++ )
	{
		BanIndex *Index = *it;

		if (Index->IP)
			continue;

		if (!strcmp(Name, Index->Name))
			return !!(Index->Flags & BAN_ENTER);
	}
	return false;
}
