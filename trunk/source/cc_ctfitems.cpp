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
// cc_ctfitems.cpp
// Flags and Techs
//


#include "cc_local.h"

#ifdef CLEANCTF_ENABLED

void CTFResetFlags(void);

CFlag::CFlag (char *Classname, char *WorldModel, int EffectFlags,
			   char *PickupSound, char *Icon, char *Name, EItemFlags Flags,
			   char *Precache, int team) :
CBaseItem (Classname, WorldModel, EffectFlags, PickupSound, Icon, Name, Flags,
		   Precache),
team(team)
{
};

void	CFlag::Drop (CPlayerEntity *ent)
{
}

void	CFlag::Use (CPlayerEntity *ent)
{
}

bool CFlag::Pickup(CItemEntity *ent, CPlayerEntity *other)
{
	if (team == other->Client.resp.ctf_team)
	{
		if (!(ent->gameEntity->spawnflags & DROPPED_ITEM))
		{
			// If we have the flag, but the flag isn't this, then we have another flag.
			// FIXME this code here will break with > 2 teams (when we get there)!!
			if (other->Client.pers.Flag && (other->Client.pers.Flag != this))
			{
				BroadcastPrintf(PRINT_HIGH, "%s captured the %s flag!\n",
						other->Client.pers.netname, CTFOtherTeamName(team));
				other->Client.pers.Inventory.Set(other->Client.pers.Flag, 0);
				other->Client.pers.Flag = NULL;

				ctfgame.last_flag_capture = level.framenum;
				ctfgame.last_capture_team = team;

				if (team == CTF_TEAM1)
					ctfgame.team1++;
				else
					ctfgame.team2++;

				ent->PlaySound(CHAN_RELIABLE+CHAN_NO_PHS_ADD+CHAN_VOICE, SoundIndex("ctf/flagcap.wav"), 1, ATTN_NONE, 0);

				// other gets another 10 frag bonus
				other->Client.resp.score += CTF_CAPTURE_BONUS;
				if (other->Client.resp.ghost)
					other->Client.resp.ghost->caps++;

				// Ok, let's do the player loop, hand out the bonuses
				for (int i = 1; i <= game.maxclients; i++)
				{
					CPlayerEntity *player = dynamic_cast<CPlayerEntity*>(g_edicts[i].Entity);
					if (!player->IsInUse())
						continue;

					if (player->Client.resp.ctf_team != other->Client.resp.ctf_team)
						player->Client.resp.ctf_lasthurtcarrier = -5;
					else if (player->Client.resp.ctf_team == other->Client.resp.ctf_team)
					{
						if (player != other)
							player->Client.resp.score += CTF_TEAM_BONUS;
						// award extra points for capture assists
						if (player->Client.resp.ctf_lastreturnedflag + CTF_RETURN_FLAG_ASSIST_TIMEOUT > level.framenum)
						{
							BroadcastPrintf(PRINT_HIGH, "%s gets an assist for returning the flag!\n", player->Client.pers.netname);
							player->Client.resp.score += CTF_RETURN_FLAG_ASSIST_BONUS;
						}
						if (player->Client.resp.ctf_lastfraggedcarrier + CTF_FRAG_CARRIER_ASSIST_TIMEOUT > level.framenum)
						{
							BroadcastPrintf(PRINT_HIGH, "%s gets an assist for fragging the flag carrier!\n", player->Client.pers.netname);
							player->Client.resp.score += CTF_FRAG_CARRIER_ASSIST_BONUS;
						}
					}
				}

				CTFResetFlags();
				return false;
			}
			return false; // its at home base already
		}	
		// hey, its not home.  return it by teleporting it back
		BroadcastPrintf(PRINT_HIGH, "%s returned the %s flag!\n", 
			other->Client.pers.netname, CTFTeamName(team));
		other->Client.resp.score += CTF_RECOVERY_BONUS;
		other->Client.resp.ctf_lastreturnedflag = level.framenum;
		ent->PlaySound (CHAN_RELIABLE+CHAN_NO_PHS_ADD+CHAN_VOICE, SoundIndex("ctf/flagret.wav"), 1, ATTN_NONE, 0);
		//CTFResetFlag will remove this entity!  We must return false
		CTFResetFlag(team);
		return false;
	}

	// hey, its not our flag, pick it up
	BroadcastPrintf(PRINT_HIGH, "%s got the %s flag!\n",
		other->Client.pers.netname, CTFTeamName(team));
	other->Client.resp.score += CTF_FLAG_BONUS;

	other->Client.pers.Inventory.Set(this, 1);
	other->Client.pers.Flag = this;
	other->Client.resp.ctf_flagsince = level.framenum;

	// pick up the flag
	// if it's not a dropped flag, we just make is disappear
	// if it's dropped, it will be removed by the pickup caller
	if (!(ent->gameEntity->spawnflags & DROPPED_ITEM))
	{
		ent->Flags |= FL_RESPAWN;
		ent->SetSvFlags (ent->GetSvFlags() | SVF_NOCLIENT);
		ent->SetSolid (SOLID_NOT);
	}
	return true;
}

void AddFlagsToList ()
{
	NItems::RedFlag = QNew (com_gamePool, 0) CFlag ("item_flag_team1", "players/male/flag1.md2", EF_FLAG1, "ctf/flagtk.wav", "i_ctf1", "Red Flag", ITEMFLAG_GRABBABLE, NULL, CTF_TEAM1);
	NItems::BlueFlag = QNew (com_gamePool, 0) CFlag ("item_flag_team2", "players/male/flag2.md2", EF_FLAG2, "ctf/flagtk.wav", "i_ctf2", "Blue Flag", ITEMFLAG_GRABBABLE, NULL, CTF_TEAM2);

	ItemList->AddItemToList (NItems::RedFlag);
	ItemList->AddItemToList (NItems::BlueFlag);
}

#endif