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

#include "cc_local.h"
#ifdef CLEANCTF_ENABLED
#include "cc_weaponmain.h"
#include "m_player.h"
#include <sstream>

void EndDMLevel ();

CCTFGameLocals ctfgame;

CCvar *ctf;
CCvar *ctf_forcejoin;

CCvar *competition;
CCvar *matchlock;
CCvar *electpercentage;
CCvar *matchtime;
CCvar *matchsetuptime;
CCvar *matchstarttime;
CCvar *admin_password;
CCvar *warp_list;

void CreateCTFStatusbar ()
{
	CStatusBar CTFBar;

	CTFBar.AddPoint_Y (-24, true);

	CTFBar.AddVirtualPoint_X (0);
	CTFBar.AddHealthNum ();
	CTFBar.AddVirtualPoint_X (50);
	CTFBar.AddPicStat (STAT_HEALTH_ICON);

	CTFBar.AddIf (STAT_AMMO_ICON);
	CTFBar.AddVirtualPoint_X (100);
	CTFBar.AddAmmoNum ();
	CTFBar.AddVirtualPoint_X (150);
	CTFBar.AddPicStat (STAT_AMMO_ICON);
	CTFBar.AddEndIf ();

	CTFBar.AddIf (STAT_ARMOR);
	CTFBar.AddVirtualPoint_X (200);
	CTFBar.AddArmorNum ();
	CTFBar.AddVirtualPoint_X (250);
	CTFBar.AddPicStat (STAT_ARMOR_ICON);
	CTFBar.AddEndIf ();

	CTFBar.AddIf (STAT_SELECTED_ICON);
	CTFBar.AddVirtualPoint_X (296);
	CTFBar.AddPicStat (STAT_SELECTED_ICON);
	CTFBar.AddEndIf ();

	CTFBar.AddPoint_Y (-50, true);

	CTFBar.AddIf (STAT_PICKUP_ICON);
	CTFBar.AddVirtualPoint_X (0);
	CTFBar.AddPicStat (STAT_PICKUP_ICON);
	CTFBar.AddVirtualPoint_X (26);
	CTFBar.AddPoint_Y (-42, true);
	CTFBar.AddStatString (STAT_PICKUP_STRING);
	CTFBar.AddPoint_Y (-50, true);
	CTFBar.AddEndIf ();

	CTFBar.AddIf (STAT_TIMER_ICON);
	CTFBar.AddVirtualPoint_X (246);
	CTFBar.AddNumStat (STAT_TIMER, 2);
	CTFBar.AddVirtualPoint_X (296);
	CTFBar.AddPicStat (STAT_TIMER_ICON);
	CTFBar.AddEndIf ();

	CTFBar.AddIf (STAT_HELPICON);
	CTFBar.AddVirtualPoint_X (148);
	CTFBar.AddPicStat (STAT_HELPICON);
	CTFBar.AddEndIf ();

	CTFBar.AddPoint_X (-50, true);
	CTFBar.AddPoint_Y (2, false);
	CTFBar.AddNumStat (STAT_FRAGS, 2);

	CTFBar.AddPoint_Y (-129, true);

	CTFBar.AddIf (STAT_TECH);
		CTFBar.AddPoint_X (-26, true);
		CTFBar.AddPicStat (STAT_TECH);
	CTFBar.AddEndIf();

	CTFBar.AddPoint_Y (-102, true);

	CTFBar.AddIf (STAT_CTF_TEAM1_PIC);
		CTFBar.AddPoint_X (-26, true);
		CTFBar.AddPicStat (STAT_CTF_TEAM1_PIC);
	CTFBar.AddEndIf ();
	CTFBar.AddPoint_X (-62, true);
	CTFBar.AddNumStat (STAT_CTF_TEAM1_CAPS, 2);

	CTFBar.AddIf (STAT_CTF_JOINED_TEAM1_PIC);
		CTFBar.AddPoint_Y (-104, true);
		CTFBar.AddPoint_X (-28, true);
		CTFBar.AddPicStat (STAT_CTF_JOINED_TEAM1_PIC);
	CTFBar.AddEndIf ();

	CTFBar.AddPoint_Y (-75, true);

	CTFBar.AddIf (STAT_CTF_TEAM2_PIC);
		CTFBar.AddPoint_X (-26, true);
		CTFBar.AddPicStat (STAT_CTF_TEAM2_PIC);
	CTFBar.AddEndIf ();
	CTFBar.AddPoint_X (-62, true);
	CTFBar.AddNumStat (STAT_CTF_TEAM2_CAPS, 2);

	CTFBar.AddIf (STAT_CTF_JOINED_TEAM2_PIC);
		CTFBar.AddPoint_Y (-77, true);
		CTFBar.AddPoint_X (-28, true);
		CTFBar.AddPicStat (STAT_CTF_JOINED_TEAM2_PIC);
	CTFBar.AddEndIf ();

	CTFBar.AddIf (STAT_CTF_FLAG_PIC);
		CTFBar.AddPoint_Y (26, false);
		CTFBar.AddPoint_X (-24, true);
		CTFBar.AddPicStat (STAT_CTF_FLAG_PIC);
	CTFBar.AddEndIf();

	CTFBar.AddIf (STAT_CTF_ID_VIEW);
		CTFBar.AddVirtualPoint_X (0);
		CTFBar.AddPoint_Y (-58, true);
		CTFBar.AddString ("Viewing", false, false);
		CTFBar.AddVirtualPoint_X (64);
		CTFBar.AddStatString (STAT_CTF_ID_VIEW);
	CTFBar.AddEndIf ();

	CTFBar.AddIf (STAT_CTF_MATCH);
	CTFBar.AddPoint_X (0, false);
	CTFBar.AddPoint_Y (-78, true);
	CTFBar.AddStatString (STAT_CTF_MATCH);
	CTFBar.AddEndIf();

	CTFBar.Send ();
}

/*--------------------------------------------------------------------------*/

/*
=================
findradius

Returns entities that have origins within a spherical area

findradius (origin, radius)
=================
*/
static inline void BuildBoxPoints(vec3f p[8], vec3f &org, vec3f &mins, vec3f &maxs)
{
	p[0] = org + mins;
	p[1] = p[0] - vec3f(mins.X, 0, 0);
	p[2] = p[0] - vec3f(0, mins.Y, 0);
	p[3] = p[0] - vec3f(mins.X, mins.Y, 0);

	p[4] = org + maxs;
	p[5] = p[4] - vec3f(maxs.X, 0, 0);
	p[6] = p[0] - vec3f(0, maxs.Y, 0);
	p[7] = p[0] - vec3f(maxs.X, maxs.Y, 0);
}

bool loc_CanSee (CBaseEntity *targ, CBaseEntity *inflictor)
{
	// bmodels need special checking because their origin is 0,0,0
	if ((targ->EntityFlags & ENT_PHYSICS) && (entity_cast<CPhysicsEntity>(targ))->PhysicsType == PHYSICS_PUSH)
		return false; // bmodels not supported

	vec3f	targpoints[8];
	BuildBoxPoints(targpoints, targ->State.GetOrigin(), targ->GetMins(), targ->GetMaxs());
	
	vec3f viewpoint = inflictor->State.GetOrigin() + vec3f(0,0,inflictor->ViewHeight);
	for (uint8 i = 0; i < 8; i++)
	{
		CTrace trace (viewpoint, targpoints[i], inflictor, CONTENTS_MASK_SOLID);
		if (trace.fraction == 1.0)
			return true;
	}

	return false;
}

/*--------------------------------------------------------------------------*/

void CTFSpawn()
{
	ctfgame.Clear ();

	if (competition->Integer() > 1)
	{
		ctfgame.match = MATCH_SETUP;
		ctfgame.matchtime = level.Frame + matchsetuptime->Integer() * 60;
	}
}

void CTFInit()
{
	ctf = QNew (com_genericPool, 0) CCvar("ctf", "0", CVAR_LATCH_SERVER|CVAR_SERVERINFO);
	ctf_forcejoin = QNew (com_genericPool, 0) CCvar("ctf_forcejoin", "", 0);
	competition = QNew (com_genericPool, 0) CCvar("competition", "0", CVAR_SERVERINFO);
	matchlock = QNew (com_genericPool, 0) CCvar("matchlock", "1", CVAR_SERVERINFO);
	electpercentage = QNew (com_genericPool, 0) CCvar("electpercentage", "66", 0);
	matchtime = QNew (com_genericPool, 0) CCvar("matchtime", "20", CVAR_SERVERINFO);
	matchsetuptime = QNew (com_genericPool, 0) CCvar("matchsetuptime", "10", 0);
	matchstarttime = QNew (com_genericPool, 0) CCvar("matchstarttime", "20", 0);
	admin_password = QNew (com_genericPool, 0) CCvar("admin_password", "", 0);
	warp_list = QNew (com_genericPool, 0) CCvar("warp_list", "q2ctf1 q2ctf2 q2ctf3 q2ctf4 q2ctf5", 0);
}

/*------------------------------------------------------------------------*/
/*
CTFFragBonuses

Calculate the bonuses for flag defense, flag carrier defense, etc.
Note that bonuses are not cumaltive.  You get one, they are in importance
order.
*/
void CTFFragBonuses(CPlayerEntity *targ, CPlayerEntity *attacker)
{
	if (attacker->Client.Respawn.CTF.Ghost && (attacker != targ))
		attacker->Client.Respawn.CTF.Ghost->kills++;
	if (targ->Client.Respawn.CTF.Ghost)
		targ->Client.Respawn.CTF.Ghost->deaths++;

	// no bonus for fragging yourself
	if (targ == attacker)
		return;

	ETeamIndex otherteam = CTFOtherTeam(targ->Client.Respawn.CTF.Team);
	if (otherteam < 0)
		return; // whoever died isn't on a team

	// same team, if the flag at base, check if he has the enemy flag
	CFlag *flag_item = (targ->Client.Respawn.CTF.Team == CTF_TEAM1) ? NItems::RedFlag : NItems::BlueFlag;
	CFlag *enemy_flag_item = (flag_item == NItems::RedFlag) ? NItems::BlueFlag : NItems::RedFlag;

	// did the attacker frag the flag carrier?
	if (targ->Client.Persistent.Inventory.Has(enemy_flag_item))
	{
		attacker->Client.Respawn.CTF.LastFraggedCarrier = level.Frame;
		attacker->Client.Respawn.Score += CTF_FRAG_CARRIER_BONUS;
		attacker->PrintToClient (PRINT_MEDIUM, "BONUS: %d points for fragging enemy flag carrier.\n",
			CTF_FRAG_CARRIER_BONUS);

		// the target had the flag, clear the hurt carrier
		// field on the other team
		for (uint8 i = 1; i <= game.maxclients; i++)
		{
			CPlayerEntity *ent = entity_cast<CPlayerEntity>((g_edicts + i)->Entity);
			if (ent->GetInUse() && ent->Client.Respawn.CTF.Team == otherteam)
				ent->Client.Respawn.CTF.LastHurtCarrier = 0;
		}
		return;
	}

	if (targ->Client.Respawn.CTF.LastHurtCarrier &&
		(level.Frame - targ->Client.Respawn.CTF.LastHurtCarrier < CTF_CARRIER_DANGER_PROTECT_TIMEOUT) &&
		!attacker->Client.Persistent.Inventory.Has(flag_item))
	{
		// attacker is on the same team as the flag carrier and
		// fragged a guy who hurt our flag carrier
		attacker->Client.Respawn.Score += CTF_CARRIER_DANGER_PROTECT_BONUS;
		BroadcastPrintf(PRINT_MEDIUM, "%s defends %s's flag carrier against an agressive enemy\n",
			attacker->Client.Persistent.Name.c_str(), 
			CTFTeamName(attacker->Client.Respawn.CTF.Team));
		if (attacker->Client.Respawn.CTF.Ghost)
			attacker->Client.Respawn.CTF.Ghost->carrierdef++;
		return;
	}

	// flag and flag carrier area defense bonuses
	// we have to find the flag and carrier entities

	// find the flag
	char *c;
	switch (attacker->Client.Respawn.CTF.Team)
	{
	case CTF_TEAM1:
		c = "item_flag_team1";
		break;
	case CTF_TEAM2:
		c = "item_flag_team2";
		break;
	default:
		return;
	}

	CFlagEntity *flag = NULL;
	while ((flag = CC_Find<CFlagEntity, ENT_BASE, EntityMemberOffset(CBaseEntity,ClassName)> (flag, c)) != NULL)
	{
		if (!(flag->SpawnFlags & DROPPED_ITEM))
			break;
	}

	if (!flag)
		return; // can't find attacker's flag

	// find attacker's team's flag carrier
	CPlayerEntity *carrier = NULL;
	for (uint8 i = 1; i <= game.maxclients; i++)
	{
		carrier = entity_cast<CPlayerEntity>((g_edicts + i)->Entity);
		if (carrier->GetInUse() && 
			carrier->Client.Persistent.Inventory.Has(flag_item))
			break;
		carrier = NULL;
	}

	// ok we have the attackers flag and a pointer to the carrier

	// check to see if we are defending the base's flag
	if (((targ->State.GetOrigin() - flag->State.GetOrigin()).Length() < CTF_TARGET_PROTECT_RADIUS ||
		(attacker->State.GetOrigin() - flag->State.GetOrigin()).Length() < CTF_TARGET_PROTECT_RADIUS ||
		loc_CanSee(flag, targ) || loc_CanSee(flag, attacker)) &&
		attacker->Client.Respawn.CTF.Team != targ->Client.Respawn.CTF.Team)
	{
		// we defended the base flag
		attacker->Client.Respawn.Score += CTF_FLAG_DEFENSE_BONUS;
		if (flag->GetSolid() == SOLID_NOT)
			BroadcastPrintf(PRINT_MEDIUM, "%s defends the %s base.\n",
				attacker->Client.Persistent.Name.c_str(), 
				CTFTeamName(attacker->Client.Respawn.CTF.Team));
		else
			BroadcastPrintf(PRINT_MEDIUM, "%s defends the %s flag.\n",
				attacker->Client.Persistent.Name.c_str(), 
				CTFTeamName(attacker->Client.Respawn.CTF.Team));
		if (attacker->Client.Respawn.CTF.Ghost)
			attacker->Client.Respawn.CTF.Ghost->basedef++;
		return;
	}

	if (carrier && carrier != attacker)
	{
		if ((targ->State.GetOrigin() - carrier->State.GetOrigin()).Length() < CTF_ATTACKER_PROTECT_RADIUS ||
			(attacker->State.GetOrigin() - carrier->State.GetOrigin()).Length() < CTF_ATTACKER_PROTECT_RADIUS ||
			loc_CanSee(carrier, targ) || loc_CanSee(carrier, attacker))
		{
			attacker->Client.Respawn.Score += CTF_CARRIER_PROTECT_BONUS;
			BroadcastPrintf(PRINT_MEDIUM, "%s defends the %s's flag carrier.\n",
				attacker->Client.Persistent.Name.c_str(), 
				CTFTeamName(attacker->Client.Respawn.CTF.Team));
			if (attacker->Client.Respawn.CTF.Ghost)
				attacker->Client.Respawn.CTF.Ghost->carrierdef++;
			return;
		}
	}
}

void CTFCheckHurtCarrier(CPlayerEntity *targ, CPlayerEntity *attacker)
{
	CBaseItem *flag_item;

	switch (targ->Client.Respawn.CTF.Team)
	{
	case CTF_TEAM1:
		flag_item = NItems::BlueFlag;
		break;
	case CTF_TEAM2:
		flag_item = NItems::RedFlag;
		break;
	default:
		flag_item = NULL;
		return;
	}

	if (targ->Client.Persistent.Inventory.Has(flag_item) &&
		targ->Client.Respawn.CTF.Team != attacker->Client.Respawn.CTF.Team)
		attacker->Client.Respawn.CTF.LastHurtCarrier = level.Frame;
}


/*------------------------------------------------------------------------*/

void CTFResetFlag(ETeamIndex Team)
{
	char *c;

	switch (Team)
	{
	case CTF_TEAM1:
		c = "item_flag_team1";
		break;
	case CTF_TEAM2:
		c = "item_flag_team2";
		break;
	default:
		return;
	}

	CFlagEntity *ent = NULL;
	while ((ent = CC_Find<CFlagEntity, ENT_BASE, EntityMemberOffset(CBaseEntity,ClassName)> (ent, c)) != NULL)
	{
		if (ent->SpawnFlags & DROPPED_ITEM)
			ent->Free ();
		else
		{
			ent->GetSvFlags() &= ~SVF_NOCLIENT;
			ent->GetSolid() = SOLID_TRIGGER;
			ent->Link ();
			ent->State.GetEvent() = EV_ITEM_RESPAWN;
		}
	}
}

void CTFResetFlags()
{
	CTFResetFlag(CTF_TEAM1);
	CTFResetFlag(CTF_TEAM2);
}

// called when we enter the intermission
void CTFCalcScores()
{
	ctfgame.total1 = ctfgame.total2 = 0;

	for (uint8 i = 0; i < game.maxclients; i++)
	{
		CPlayerEntity *Player = entity_cast<CPlayerEntity>(g_edicts[i+1].Entity);

		if (!Player->GetInUse())
			continue;

		switch (Player->Client.Respawn.CTF.Team)
		{
		case CTF_TEAM1:
			ctfgame.total1 += Player->Client.Respawn.Score;
			break;
		case CTF_TEAM2:
			ctfgame.total2 += Player->Client.Respawn.Score;
			break;
		}
	}
}

void CTFID_f (CPlayerEntity *ent)
{
	ent->Client.Respawn.CTF.IDState = !ent->Client.Respawn.CTF.IDState;
	ent->PrintToClient (PRINT_HIGH, "%s player identification display\n", (ent->Client.Respawn.CTF.IDState) ? "Activating" : "Deactivating");
}

/*------------------------------------------------------------------------*/

void CTFTeam_f (CPlayerEntity *ent)
{
	sint32 desired_team;

	std::cc_string t = ArgGetConcatenatedString();
	if (!t[0])
	{
		ent->PrintToClient (PRINT_HIGH, "You are on the %s team.\n",
			CTFTeamName(ent->Client.Respawn.CTF.Team));
		return;
	}

	if (ctfgame.match > MATCH_SETUP)
	{
		ent->PrintToClient (PRINT_HIGH, "Can't change teams in a match.\n");
		return;
	}

	if (Q_stricmp (t.c_str(), "red") == 0)
		desired_team = CTF_TEAM1;
	else if (Q_stricmp (t.c_str(), "blue") == 0)
		desired_team = CTF_TEAM2;
	else
	{
		ent->PrintToClient (PRINT_HIGH, "Unknown team %s.\n", t);
		return;
	}

	if (ent->Client.Respawn.CTF.Team == desired_team)
	{
		ent->PrintToClient (PRINT_HIGH, "You are already on the %s team.\n",
			CTFTeamName(ent->Client.Respawn.CTF.Team));
		return;
	}

////
	ent->GetSvFlags() = 0;
	ent->Flags &= ~FL_GODMODE;
	ent->Client.Respawn.CTF.Team = desired_team;
	ent->Client.Respawn.CTF.State = 0;
	ent->CTFAssignSkin(Info_ValueForKey (ent->Client.Persistent.UserInfo, "skin"));

	if (ent->GetSolid() == SOLID_NOT)
	{ // Spectator
		ent->PutInServer();
		// add a teleportation effect
		ent->State.GetEvent() = EV_PLAYER_TELEPORT;
		// hold in place briefly
		ent->Client.PlayerState.GetPMove()->pmFlags = PMF_TIME_TELEPORT;
		ent->Client.PlayerState.GetPMove()->pmTime = 14;
		BroadcastPrintf(PRINT_HIGH, "%s joined the %s team.\n",
			ent->Client.Persistent.Name.c_str(), CTFTeamName(desired_team));
		return;
	}

	ent->Health = 0;
	ent->Die (ent, ent, 100000, vec3fOrigin);
	// don't even bother waiting for death frames
	ent->DeadFlag = true;
	ent->Respawn ();

	ent->Client.Respawn.Score = 0;

	BroadcastPrintf(PRINT_HIGH, "%s changed to the %s team.\n",
		ent->Client.Persistent.Name.c_str(), CTFTeamName(desired_team));
}

/*
======================================================================

SAY_TEAM

======================================================================
*/

#define HASHSIZE_CLASSNAMES 256

// This array is in 'importance order', it indicates what items are
// more important when reporting their names.
class CLocName
{
public:
	std::cc_string		classname;
	uint32				hash;

	uint8				priority;

	CLocName (const char *name, uint8 priority) :
		classname(name),
		priority(priority),
		hash(Com_HashGeneric (name, HASHSIZE_CLASSNAMES))
	{
	};
};

CLocName LocNames[] = 
{
	CLocName(	"item_flag_team1",			1 ),
	CLocName(	"item_flag_team2",			1 ),
	CLocName(	"item_quad",				2 ), 
	CLocName(	"item_invulnerability",		2 ),
	CLocName(	"weapon_bfg",				3 ),
	CLocName(	"weapon_railgun",			4 ),
	CLocName(	"weapon_rocketlauncher",	4 ),
	CLocName(	"weapon_hyperblaster",		4 ),
	CLocName(	"weapon_chaingun",			4 ),
	CLocName(	"weapon_grenadelauncher",	4 ),
	CLocName(	"weapon_machinegun",		4 ),
	CLocName(	"weapon_supershotgun",		4 ),
	CLocName(	"weapon_shotgun",			4 ),
	CLocName(	"item_power_screen",		5 ),
	CLocName(	"item_power_shield",		5 ),
	CLocName(	"item_armor_body",			6 ),
	CLocName(	"item_armor_combat",		6 ),
	CLocName(	"item_armor_jacket",		6 ),
	CLocName(	"item_silencer",			7 ),
	CLocName(	"item_breather",			7 ),
	CLocName(	"item_enviro",				7 ),
	CLocName(	"item_adrenaline",			7 ),
	CLocName(	"item_bandolier",			8 ),
	CLocName(	"item_pack",				8 ),
	CLocName(	"",							0 )
};

static inline void CTFSay_Team_Location(CPlayerEntity *who, std::cc_stringstream &OutMessage)
{
	CBaseEntity *hot = NULL;
	float hotdist = 999999, newdist;
	sint32 hotindex = 999;
	CBaseItem *item;
	sint32 nearteam = -1;
	CFlagEntity *flag1, *flag2;
	bool hotsee = false;
	bool cansee;
	CBaseEntity *what = NULL;

	while ((what = FindRadius<ENT_BASE>(what, who->State.GetOrigin(), 1024, false)) != NULL)
	{
		// find what in loc_classnames
		uint32 hash = Com_HashGeneric (what->ClassName, HASHSIZE_CLASSNAMES);

		uint8 i;
		for (i = 0; !LocNames[i].classname.empty(); i++)
		{
			if (hash == LocNames[i].hash && strcmp(what->ClassName, LocNames[i].classname.c_str()) == 0)
				break;
		}

		if (LocNames[i].classname.empty())
			continue;

		// something we can see get priority over something we can't
		cansee = loc_CanSee(what, who);
		if (cansee && !hotsee)
		{
			hotsee = true;
			hotindex = LocNames[i].priority;
			hot = what;

			hotdist = (what->State.GetOrigin() - who->State.GetOrigin()).Length();
			continue;
		}
		// if we can't see this, but we have something we can see, skip it
		if (hotsee && !cansee)
			continue;
		if (hotsee && hotindex < LocNames[i].priority)
			continue;

		newdist = (what->State.GetOrigin() - who->State.GetOrigin()).Length();
		if (newdist < hotdist || 
			(cansee && LocNames[i].priority < hotindex))
		{
			hot = what;
			hotdist = newdist;
			hotindex = i;
			hotsee = loc_CanSee(hot, who);
		}
	}

	if (!hot)
	{
		OutMessage << "nowhere";
		return;
	}

	// we now have the closest item
	// see if there's more than one in the map, if so
	// we need to determine what team is closest
	what = NULL;
	while ((what = CC_Find<CBaseEntity, ENT_BASE, EntityMemberOffset(CBaseEntity,ClassName)> (what, hot->ClassName)) != NULL)
	{
		if (what == hot)
			continue;
		// if we are here, there is more than one, find out if hot
		// is closer to red flag or blue flag
		if ((flag1 = CC_Find<CFlagEntity, ENT_ITEM, EntityMemberOffset(CBaseEntity,ClassName)> (NULL, "item_flag_team1")) != NULL &&
			(flag2 = CC_Find<CFlagEntity, ENT_ITEM, EntityMemberOffset(CBaseEntity,ClassName)> (NULL, "item_flag_team2")) != NULL)
		{
			hotdist = (hot->State.GetOrigin() - flag1->State.GetOrigin()).Length();
			newdist = (hot->State.GetOrigin() - flag2->State.GetOrigin()).Length();
			if (hotdist < newdist)
				nearteam = CTF_TEAM1;
			else if (hotdist > newdist)
				nearteam = CTF_TEAM2;
		}
		break;
	}

	if ((item = FindItemByClassname(hot->ClassName)) == NULL)
	{
		OutMessage << "nowhere";
		return;
	}

	// in water?
	if (who->WaterInfo.Level)
		OutMessage << "in the water ";

	// near or above
	vec3f v = who->State.GetOrigin() - hot->State.GetOrigin();
	if (Q_fabs(v.Z) > Q_fabs(v.X) && Q_fabs(v.Z) > Q_fabs(v.Y))
		OutMessage << (v.Z > 0) ? "above " : "below ";
	else
		OutMessage << "near ";

	switch (nearteam)
	{
	case CTF_TEAM1:
		OutMessage << "the red ";
		break;
	case CTF_TEAM2:
		OutMessage << "the blue ";
		break;
	default:
		OutMessage << "the ";
		break;
	}

	OutMessage << item->Name;
}

static inline void CTFSay_Team_Armor(CPlayerEntity *who, std::cc_stringstream &OutMessage)
{
	CBaseItem		*item = who->Client.Persistent.Armor;
	EPowerArmorType power_armor_type = who->PowerArmorType ();

	if (power_armor_type)
	{
		sint32 cells = who->Client.Persistent.Inventory.Has(NItems::Cells);
		if (cells)
		{
			OutMessage << ((power_armor_type == POWER_ARMOR_SCREEN) ? "Power Screen" : "Power Shield") << " with " << cells << " cells ";
			if (item)
				OutMessage << "and ";
		}
	}

	if (item)
		OutMessage << who->Client.Persistent.Inventory.Has(item) << " units of " << item->Name;
	else if (!power_armor_type)
		OutMessage << "no armor";
}

static inline void CTFSay_Team_Health(CPlayerEntity *who, std::cc_stringstream &OutMessage)
{
	if (who->Health <= 0)
		OutMessage << "dead";
	else
		OutMessage << who->Health << " health";
}

static inline void CTFSay_Team_Tech(CPlayerEntity *who, std::cc_stringstream &OutMessage)
{
	// see if the player has a tech powerup
	if (who->Client.Persistent.Tech)
		OutMessage << "the " << who->Client.Persistent.Tech->Name;
	else
		OutMessage << "no powerup";
}

static inline void CTFSay_Team_Weapon(CPlayerEntity *who, std::cc_stringstream &OutMessage)
{
	if (who->Client.Persistent.Weapon)
		OutMessage << who->Client.Persistent.Weapon->Item->Name;
	else
		OutMessage << "none";
}

static inline void CTFSay_Team_Sight(CPlayerEntity *who, std::cc_stringstream &OutMessage)
{
	sint32 n = 0;
	static std::cc_string nameStore;

	for (uint8 i = 1; i <= game.maxclients; i++)
	{
		CPlayerEntity *targ = entity_cast<CPlayerEntity>(g_edicts[i].Entity);
		if (!targ->GetInUse() || 
			targ == who ||
			!loc_CanSee(targ, who))
			continue;

		if (nameStore.empty())
			nameStore = targ->Client.Persistent.Name;
		else
		{
			if (n)
				OutMessage << ", ";

			OutMessage << nameStore;
			nameStore = targ->Client.Persistent.Name;

			n++;
		}
	}

	if (n)
	{
		if (!nameStore.empty())
		{
			OutMessage << " and ";
			OutMessage << nameStore;
		}
	}
	else
		OutMessage << "no one";
}

bool CheckFlood(CPlayerEntity *ent);
void CTFSay_Team(CPlayerEntity *who, std::cc_string msg)
{
	static std::cc_stringstream OutMessage;
	
	if (CheckFlood(who))
		return;

	OutMessage.str("");

	if (msg[0] == '\"')
	{
		msg.erase (0, 1);
		msg.erase (msg.end() - 1);
	}

	//for (p = outmsg; msg[pm] && (p - outmsg) < sizeof(outmsg) - 1; pm++)
	for (size_t pm = 0; pm < msg.size(); pm++)
	{
		if (msg[pm] == '%')
		{
			switch (Q_tolower(msg[++pm]))
			{
			case 'l' :
				CTFSay_Team_Location(who, OutMessage);
				break;
			case 'a' :
				CTFSay_Team_Armor(who, OutMessage);
				break;
			case 'h' :
				CTFSay_Team_Health(who, OutMessage);
				break;
			case 't' :
				CTFSay_Team_Tech(who, OutMessage);
				break;
			case 'w' :
				CTFSay_Team_Weapon(who, OutMessage);
				break;
			case 'n' :
				CTFSay_Team_Sight(who, OutMessage);
				break;
			default :
				OutMessage << msg[pm];
			}
		}
		else
			OutMessage << msg[pm];
	}

	for (uint8 i = 0; i < game.maxclients; i++)
	{
		CPlayerEntity *cl_ent = entity_cast<CPlayerEntity>((g_edicts + 1 + i)->Entity);
		if (!cl_ent->GetInUse())
			continue;
		if (cl_ent->Client.Persistent.state != SVCS_SPAWNED)
			continue;
		if (cl_ent->Client.Respawn.CTF.Team == who->Client.Respawn.CTF.Team)
			cl_ent->PrintToClient (PRINT_CHAT, "(%s): %s\n", 
				who->Client.Persistent.Name.c_str(), OutMessage.str().c_str());
	}
}

void GCTFSay_Team (CPlayerEntity *ent)
{
	CTFSay_Team (ent, ArgGetConcatenatedString());
}

/*-----------------------------------------------------------------------*/
/*QUAKED misc_ctf_banner (1 .5 0) (-4 -64 0) (4 64 248) TEAM2
The origin is the bottom of the banner.
The banner is 248 tall.
*/
class CMiscCTFBanner : public CMapEntity, public CThinkableEntity
{
public:
	CMiscCTFBanner () :
	  CBaseEntity (),
	  CMapEntity (),
	  CThinkableEntity ()
	{
	};

	CMiscCTFBanner (sint32 Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CThinkableEntity (Index)
	{
	};

	IMPLEMENT_SAVE_HEADER(CMiscCTFBanner)

	bool Run ()
	{
		return CBaseEntity::Run();
	};

	void SaveFields (CFile &File)
	{
		CMapEntity::SaveFields (File);
		CThinkableEntity::SaveFields (File);
	};

	void LoadFields (CFile &File)
	{
		CMapEntity::LoadFields (File);
		CThinkableEntity::LoadFields (File);
	};

	void Think ()
	{
		State.GetFrame() = (State.GetFrame() + 1) % 16;
		NextThink = level.Frame + FRAMETIME;
	};

	virtual void Spawn ()
	{
		GetSolid() = SOLID_NOT;
		State.GetModelIndex() = ModelIndex ("models/ctf/banner/tris.md2");
		if (SpawnFlags & 1) // team2
			State.GetSkinNum() = 1;

		State.GetFrame() = irandom(16);
		Link ();

		NextThink = level.Frame + FRAMETIME;
	};
};

LINK_CLASSNAME_TO_CLASS ("misc_ctf_banner", CMiscCTFBanner);

/*QUAKED misc_ctf_small_banner (1 .5 0) (-4 -32 0) (4 32 124) TEAM2
The origin is the bottom of the banner.
The banner is 124 tall.
*/
class CMiscCTFBannerSmall : public CMiscCTFBanner
{
public:
	CMiscCTFBannerSmall () :
	  CBaseEntity (),
	  CMiscCTFBanner ()
	{
	};

	CMiscCTFBannerSmall (sint32 Index) :
	  CBaseEntity (Index),
	  CMiscCTFBanner (Index)
	{
	};

	IMPLEMENT_SAVE_HEADER(CMiscCTFBannerSmall)

	void Spawn ()
	{
		GetSolid() = SOLID_NOT;
		State.GetModelIndex() = ModelIndex ("models/ctf/banner/small.md2");
		if (SpawnFlags & 1) // team2
			State.GetSkinNum() = 1;

		State.GetFrame() = irandom(16);
		Link ();

		NextThink = level.Frame + FRAMETIME;
	};
};

LINK_CLASSNAME_TO_CLASS ("misc_ctf_banner_small", CMiscCTFBannerSmall);

/* ELECTIONS */

bool CTFBeginElection(CPlayerEntity *ent, EElectState type, char *msg)
{
	if (electpercentage->Integer() == 0)
	{
		ent->PrintToClient (PRINT_HIGH, "Elections are disabled, only an admin can process this action.\n");
		return false;
	}


	if (ctfgame.election != ELECT_NONE)
	{
		ent->PrintToClient (PRINT_HIGH, "Election already in progress.\n");
		return false;
	}

	// clear votes
	uint8 count = 0;
	for (uint8 i = 1; i <= game.maxclients; i++)
	{
		CPlayerEntity *e = entity_cast<CPlayerEntity>((g_edicts + i)->Entity);
		e->Client.Respawn.CTF.Voted = false;
		if (e->GetInUse())
			count++;
	}

	if (count < 2)
	{
		ent->PrintToClient (PRINT_HIGH, "Not enough players for election.\n");
		return false;
	}

	ctfgame.etarget = ent;
	ctfgame.election = type;
	ctfgame.evotes = 0;
	ctfgame.needvotes = (count * electpercentage->Integer()) / 100;
	ctfgame.electtime = level.Frame + 200; // twenty seconds for election
	Q_strncpyz(ctfgame.emsg, msg, sizeof(ctfgame.emsg) - 1);

	// tell everyone
	BroadcastPrintf(PRINT_CHAT, "%s\n", ctfgame.emsg);
	BroadcastPrintf(PRINT_HIGH, "Type YES or NO to vote on this request\n"
								"Votes: %d  Needed: %d  Time left: %ds\n",
								ctfgame.evotes, ctfgame.needvotes,
								(sint32)((ctfgame.electtime - level.Frame) / 10));

	return true;
}

void CTFResetAllPlayers()
{
	for (uint8 i = 1; i <= game.maxclients; i++)
	{
		CPlayerEntity *ent = entity_cast<CPlayerEntity>(g_edicts[i].Entity);
		if (!ent->GetInUse())
			continue;

		//if (ent->client->menu)
		//	PMenu_Close(ent);
		if (ent->Client.Respawn.MenuState.InMenu)
			ent->Client.Respawn.MenuState.CloseMenu();

		CGrapple::PlayerResetGrapple(ent);
		ent->CTFDeadDropFlag();
		ent->DeadDropTech();

		ent->Client.Respawn.CTF.Team = CTF_NOTEAM;
		ent->Client.Respawn.CTF.Ready = false;

		ent->GetSvFlags() = 0;
		ent->Flags &= ~FL_GODMODE;
		ent->PutInServer();
	}

	// reset the level
	ResetTechs();
	CTFResetFlags();

	for (uint8 i = game.maxclients; i < globals.numEdicts; i++)
	{
		CBaseEntity *ent = g_edicts[i].Entity;

		if (ent && ent->GetInUse() && (ent->EntityFlags & ENT_ITEM))
		{
			CItemEntity *Item = entity_cast<CItemEntity>(ent);
			if (Item->GetSolid() == SOLID_NOT && Item->ThinkState == ITS_RESPAWN &&
				Item->NextThink >= level.Frame)
			{
				Item->NextThink = 0;
				Item->Think ();
			}
		}
	}
	if (ctfgame.match == MATCH_SETUP)
		ctfgame.matchtime = level.Frame + matchsetuptime->Integer() * 60;
}

// start a match
void CTFStartMatch()
{
	ctfgame.match = MATCH_GAME;
	ctfgame.matchtime = level.Frame + matchtime->Integer() * 60;

	ctfgame.team1 = ctfgame.team2 = 0;

	ctfgame.Ghosts.clear();

	for (uint8 i = 1; i <= game.maxclients; i++)
	{
		CPlayerEntity *ent = entity_cast<CPlayerEntity>((g_edicts + i)->Entity);
		if (!ent->GetInUse())
			continue;

		ent->Client.Respawn.Score = 0;
		ent->Client.Respawn.CTF.State = 0;
		ent->Client.Respawn.CTF.Ghost = NULL;

		ent->PrintToClient (PRINT_CENTER, "******************\n\nMATCH HAS STARTED!\n\n******************");

		if (ent->Client.Respawn.CTF.Team != CTF_NOTEAM)
		{
			// make up a CTF.Ghost code
			ent->CTFAssignGhost();
			CGrapple::PlayerResetGrapple(ent);
			ent->GetSvFlags() = SVF_NOCLIENT;
			ent->Flags &= ~FL_GODMODE;

			ent->Client.Timers.RespawnTime = level.Frame + 10 + irandom(3);
			ent->Client.PlayerState.GetPMove()->pmType = PMT_DEAD;
			ent->Client.Anim.Priority = ANIM_DEATH;
			ent->State.GetFrame() = FRAME_death308 - 1;
			ent->Client.Anim.EndFrame = FRAME_death308;
			ent->DeadFlag = true;
			ent->NoClip = true;
			ent->Client.PlayerState.GetGunIndex () = 0;
			ent->Link ();
		}
	}
}

void CTFEndMatch()
{
	static std::cc_stringstream BroadcastString;
	BroadcastString.str("");

	ctfgame.match = MATCH_POST;
	BroadcastString <<	"MATCH COMPLETED!\n"
		"RED TEAM:  " << ctfgame.team1 << " captures, " << ctfgame.total1 << " points\n"
		"BLUE TEAM:  " << ctfgame.team2 << " captures, " << ctfgame.total2 << " points\n";

	CTFCalcScores();

	if (ctfgame.team1 > ctfgame.team2)
		BroadcastString << "RED team won over the BLUE team by " << ctfgame.team1 - ctfgame.team2 << " CAPTURES!\n";
	else if (ctfgame.team2 > ctfgame.team1)
		BroadcastString << "BLUE team won over the RED team by " << ctfgame.team2 - ctfgame.team1 << " CAPTURES!\n";
	else if (ctfgame.total1 > ctfgame.total2) // frag tie breaker
		BroadcastString << "RED team won over the BLUE team by " << ctfgame.total1 - ctfgame.total2 << " POINTS!\n";
	else if (ctfgame.total2 > ctfgame.total1) 
		BroadcastString << "BLUE team won over the RED team by " << ctfgame.total2 - ctfgame.total1 << " POINTS!\n";
	else
		BroadcastString << "TIE GAME!\n";

	BroadcastPrintf (PRINT_CHAT, "%s", BroadcastString.str().c_str());
	EndDMLevel();
}

bool CTFNextMap()
{
	if (ctfgame.match == MATCH_POST)
	{
		ctfgame.match = MATCH_SETUP;
		CTFResetAllPlayers();
		return true;
	}
	return false;
}

void CTFWinElection()
{
	switch (ctfgame.election)
	{
	case ELECT_MATCH :
		// reset into match mode
		if (competition->Integer() < 3)
			competition->Set ("2");
		ctfgame.match = MATCH_SETUP;
		CTFResetAllPlayers();
		break;

	case ELECT_ADMIN :
		ctfgame.etarget->Client.Respawn.CTF.Admin = true;
		BroadcastPrintf(PRINT_HIGH, "%s has become an admin.\n", ctfgame.etarget->Client.Persistent.Name.c_str());
		ctfgame.etarget->PrintToClient (PRINT_HIGH, "Type 'admin' to access the adminstration menu.\n");
		break;

	case ELECT_MAP :
		BroadcastPrintf(PRINT_HIGH, "%s is warping to level %s.\n", 
			ctfgame.etarget->Client.Persistent.Name.c_str(), ctfgame.elevel);
		level.ForceMap = ctfgame.elevel;
		EndDMLevel();
		break;
	}
	ctfgame.election = ELECT_NONE;
}

void CTFVoteYes(CPlayerEntity *ent)
{
	if (ctfgame.election == ELECT_NONE)
	{
		ent->PrintToClient (PRINT_HIGH, "No election is in progress.\n");
		return;
	}
	if (ent->Client.Respawn.CTF.Voted)
	{
		ent->PrintToClient (PRINT_HIGH, "You already CTF.Voted.\n");
		return;
	}
	if (ctfgame.etarget == ent)
	{
		ent->PrintToClient (PRINT_HIGH, "You can't vote for yourself.\n");
		return;
	}

	ent->Client.Respawn.CTF.Voted = true;

	ctfgame.evotes++;
	if (ctfgame.evotes == ctfgame.needvotes)
	{
		// the election has been won
		CTFWinElection();
		return;
	}
	BroadcastPrintf(PRINT_HIGH, "%s\n", ctfgame.emsg);
	BroadcastPrintf(PRINT_CHAT, "Votes: %d  Needed: %d  Time left: %ds\n", ctfgame.evotes, ctfgame.needvotes,
		(sint32)((ctfgame.electtime - level.Frame)/10));
}

void CTFVoteNo(CPlayerEntity *ent)
{
	if (ctfgame.election == ELECT_NONE)
	{
		ent->PrintToClient (PRINT_HIGH, "No election is in progress.\n");
		return;
	}
	if (ent->Client.Respawn.CTF.Voted)
	{
		ent->PrintToClient (PRINT_HIGH, "You already CTF.Voted.\n");
		return;
	}
	if (ctfgame.etarget == ent)
	{
		ent->PrintToClient (PRINT_HIGH, "You can't vote for yourself.\n");
		return;
	}

	ent->Client.Respawn.CTF.Voted = true;

	BroadcastPrintf(PRINT_HIGH, "%s\n", ctfgame.emsg);
	BroadcastPrintf(PRINT_CHAT, "Votes: %d  Needed: %d  Time left: %ds\n", ctfgame.evotes, ctfgame.needvotes,
		(sint32)((ctfgame.electtime - level.Frame)/10));
}

void CTFReady(CPlayerEntity *ent)
{
	if (ent->Client.Respawn.CTF.Team == CTF_NOTEAM)
	{
		ent->PrintToClient (PRINT_HIGH, "Pick a team first (hit <TAB> for menu)\n");
		return;
	}

	if (ctfgame.match != MATCH_SETUP)
	{
		ent->PrintToClient (PRINT_HIGH, "A match is not being setup.\n");
		return;
	}

	if (ent->Client.Respawn.CTF.Ready)
	{
		ent->PrintToClient (PRINT_HIGH, "You have already commited.\n");
		return;
	}

	ent->Client.Respawn.CTF.Ready = true;
	BroadcastPrintf(PRINT_HIGH, "%s is ready.\n", ent->Client.Persistent.Name.c_str());

	uint8 t1 = 0, t2 = 0, j = 0;
	for (uint8 i = 1; i <= game.maxclients; i++)
	{
		CPlayerEntity *e = entity_cast<CPlayerEntity>(g_edicts[i].Entity);
		if (!e->GetInUse())
			continue;
		if (e->Client.Respawn.CTF.Team != CTF_NOTEAM && !e->Client.Respawn.CTF.Ready)
			j++;
		if (e->Client.Respawn.CTF.Team == CTF_TEAM1)
			t1++;
		else if (e->Client.Respawn.CTF.Team == CTF_TEAM2)
			t2++;
	}

	if (!j && t1 && t2)
	{
		// everyone has commited
		BroadcastPrintf(PRINT_CHAT, "All players have commited.  Match starting\n");
		ctfgame.match = MATCH_PREGAME;
		ctfgame.matchtime = level.Frame + (matchstarttime->Float() * 10);
	}
}

void CTFNotReady(CPlayerEntity *ent)
{
	if (ent->Client.Respawn.CTF.Team == CTF_NOTEAM)
	{
		ent->PrintToClient (PRINT_HIGH, "Pick a team first (hit <TAB> for menu)\n");
		return;
	}

	if (ctfgame.match != MATCH_SETUP && ctfgame.match != MATCH_PREGAME)
	{
		ent->PrintToClient (PRINT_HIGH, "A match is not being setup.\n");
		return;
	}

	if (!ent->Client.Respawn.CTF.Ready)
	{
		ent->PrintToClient (PRINT_HIGH, "You haven't commited.\n");
		return;
	}

	ent->Client.Respawn.CTF.Ready = false;
	BroadcastPrintf(PRINT_HIGH, "%s is no longer ready.\n", ent->Client.Persistent.Name.c_str());

	if (ctfgame.match == MATCH_PREGAME)
	{
		BroadcastPrintf(PRINT_CHAT, "Match halted.\n");
		ctfgame.match = MATCH_SETUP;
		ctfgame.matchtime = level.Frame + matchsetuptime->Integer() * 60;
	}
}

void CTFGhost(CPlayerEntity *ent)
{
	if (ArgCount() < 2)
	{
		ent->PrintToClient (PRINT_HIGH, "Usage:  ghost <code>\n");
		return;
	}

	if (ent->Client.Respawn.CTF.Team != CTF_NOTEAM)
	{
		ent->PrintToClient (PRINT_HIGH, "You are already in the game.\n");
		return;
	}
	if (ctfgame.match != MATCH_GAME)
	{
		ent->PrintToClient (PRINT_HIGH, "No match is in progress.\n");
		return;
	}

	sint32 n = ArgGeti(1);
	TGhostMapType::iterator it;
	if ((it = ctfgame.Ghosts.find(n)) == ctfgame.Ghosts.end())
	{
		ent->PrintToClient (PRINT_HIGH, "Invalid ghost code.\n");
		return;
	}

	CCTFGhost *Ghost = (*it).second;
	ent->PrintToClient (PRINT_HIGH, "Ghost code accepted, your position has been reinstated.\n");
	Ghost->ent->Client.Respawn.CTF.Ghost = NULL;
	ent->Client.Respawn.CTF.Team = Ghost->team;
	ent->Client.Respawn.CTF.Ghost = Ghost;
	ent->Client.Respawn.Score = Ghost->Score;
	ent->Client.Respawn.CTF.State = 0;
	Ghost->ent = ent;
	ent->GetSvFlags() = 0;
	ent->Flags &= ~FL_GODMODE;
	ent->PutInServer();
	BroadcastPrintf(PRINT_HIGH, "%s has been reinstated to %s team.\n",
		ent->Client.Persistent.Name.c_str(), CTFTeamName(ent->Client.Respawn.CTF.Team));
	return;
}

bool CTFMatchSetup()
{
	if (ctfgame.match == MATCH_SETUP || ctfgame.match == MATCH_PREGAME)
		return true;
	return false;
}

bool CTFMatchOn()
{
	if (ctfgame.match == MATCH_GAME)
		return true;
	return false;
}


/*-----------------------------------------------------------------------*/

void CTFObserver(CPlayerEntity *ent)
{
	// start as 'observer'
	if (ent->NoClip)
	{
		ent->PrintToClient (PRINT_HIGH, "You are already an observer.\n");
		return;
	}

	CGrapple::PlayerResetGrapple(ent);
	ent->CTFDeadDropFlag();
	ent->DeadDropTech();

	ent->NoClip = true;
	ent->GetSolid() = SOLID_NOT;
	ent->GetSvFlags() |= SVF_NOCLIENT;
	ent->Client.Respawn.CTF.Team = CTF_NOTEAM;
	ent->Client.PlayerState.GetGunIndex () = 0;
	ent->Client.Respawn.Score = 0;
	ent->Link();
	CTFOpenJoinMenu(ent);
}

bool CTFInMatch()
{
	if (ctfgame.match > MATCH_NONE)
		return true;
	return false;
}

bool CTFCheckRules()
{
	char text[64];

	if (ctfgame.election != ELECT_NONE && ctfgame.electtime <= level.Frame)
	{
		BroadcastPrintf(PRINT_CHAT, "Election timed out and has been cancelled.\n");
		ctfgame.election = ELECT_NONE;
	}

	if (ctfgame.match != MATCH_NONE)
	{
		sint32 t = ctfgame.matchtime - level.Frame;

		if (t <= 0)
		{ // time ended on something
			switch (ctfgame.match)
			{
			case MATCH_SETUP :
				// go back to normal mode
				if (competition->Integer() < 3)
				{
					ctfgame.match = MATCH_NONE;
					competition->Set("1");
					CTFResetAllPlayers();
				}
				else
					// reset the time
					ctfgame.matchtime = level.Frame + matchsetuptime->Integer() * 60;
				return false;

			case MATCH_PREGAME :
				// match started!
				CTFStartMatch();
				return false;

			case MATCH_GAME :
				// match ended!
				CTFEndMatch();
				return false;
			}
		}

		if (t == ctfgame.lasttime)
			return false;

		ctfgame.lasttime = t;

		uint8 j = 0;
		switch (ctfgame.match)
		{
		case MATCH_SETUP :
			for (uint8 i = 1; i <= game.maxclients; i++)
			{
				CPlayerEntity *ent = entity_cast<CPlayerEntity>(g_edicts[i].Entity);
				if (!ent->GetInUse())
					continue;
				if (ent->Client.Respawn.CTF.Team != CTF_NOTEAM &&
					!ent->Client.Respawn.CTF.Ready)
					j++;
			}

			if (competition->Integer() < 3)
				Q_snprintfz(text, sizeof(text), "%02d:%02d SETUP: %d not ready",
					t / 60, t % 60, j);
			else
				Q_snprintfz(text, sizeof(text), "SETUP: %d not ready", j);

			ConfigString (CONFIG_CTF_MATCH, text);
			break;


		case MATCH_PREGAME :
			Q_snprintfz(text, sizeof(text), "%02d:%02d UNTIL START",
				t / 60, t % 60);
			ConfigString (CONFIG_CTF_MATCH, text);
			break;

		case MATCH_GAME:
			Q_snprintfz(text, sizeof(text), "%02d:%02d MATCH",
				t / 60, t % 60);
			ConfigString (CONFIG_CTF_MATCH, text);
			break;
		}
		return false;
	}

	if (capturelimit->Integer() && 
		(ctfgame.team1 >= capturelimit->Integer() ||
		ctfgame.team2 >= capturelimit->Integer())) {
		BroadcastPrintf (PRINT_HIGH, "Capturelimit hit.\n");
		return true;
	}
	return false;
}

/*----------------------------------------------------------------*/

void CTFStats(CPlayerEntity *ent)
{
	static char tempStr[80];
	static char text[1400];

	*text = 0;
	if (ctfgame.match == MATCH_SETUP)
	{
		for (uint8 i = 1; i <= game.maxclients; i++)
		{
			CPlayerEntity *e2 = entity_cast<CPlayerEntity>(g_edicts[i].Entity);
			if (!e2->GetInUse())
				continue;
			if (!e2->Client.Respawn.CTF.Ready && e2->Client.Respawn.CTF.Team != CTF_NOTEAM)
			{
				Q_snprintfz(tempStr, sizeof(tempStr), "%s is not ready.\n", e2->Client.Persistent.Name.c_str());
				if (strlen(text) + strlen(tempStr) < sizeof(text) - 50)
					Q_strcatz(text, tempStr, sizeof(text));
			}
		}
	}

	if (!ctfgame.Ghosts.size())
	{
		if (*text)
			ent->PrintToClient (PRINT_HIGH, "%s", text);
		ent->PrintToClient (PRINT_HIGH, "No statistics available.\n");
		return;
	}

	Q_strcatz(text, "  #|Name            |Score|Kills|Death|BasDf|CarDf|Effcy|\n", sizeof(text));

	for (TGhostMapType::iterator it = ctfgame.Ghosts.begin(); it != ctfgame.Ghosts.end(); it++)
	{
		CCTFGhost *Ghost = (*it).second;

		if (!Ghost->name[0])
			continue;

		sint32 e;
		if (Ghost->deaths + Ghost->kills == 0)
			e = 50;
		else
			e = Ghost->kills * 100 / (Ghost->kills + Ghost->deaths);
		Q_snprintfz(tempStr, sizeof(tempStr), "%3d|%-16.16s|%5d|%5d|%5d|%5d|%5d|%4d%%|\n",
			Ghost->number, 
			Ghost->name.c_str(), 
			Ghost->Score, 
			Ghost->kills, 
			Ghost->deaths, 
			Ghost->basedef,
			Ghost->carrierdef, 
			e);

		if (strlen(text) + strlen(tempStr) > sizeof(text) - 50)
		{
			Q_snprintfz(text+strlen(text), sizeof(text), "And more...\n");
			ent->PrintToClient (PRINT_HIGH, "%s", text);
			return;
		}

		Q_strcatz(text, tempStr, sizeof(text));
	}
	ent->PrintToClient (PRINT_HIGH, "%s", text);
}

void Cmd_PlayerList_f (CPlayerEntity *ent);
void CTFPlayerList(CPlayerEntity *ent)
{
	if (!(game.mode & GAME_CTF))
	{
		Cmd_PlayerList_f (ent);
		return;
	}

	static char tempStr[80];
	static char text[1400];

	*text = 0;
	if (ctfgame.match == MATCH_SETUP)
	{
		for (uint8 i = 1; i <= game.maxclients; i++)
		{
			CPlayerEntity *e2 = entity_cast<CPlayerEntity>(g_edicts[i].Entity);
			if (!e2->GetInUse())
				continue;
			if (!e2->Client.Respawn.CTF.Ready && e2->Client.Respawn.CTF.Team != CTF_NOTEAM)
			{
				Q_snprintfz(tempStr, sizeof(tempStr), "%s is not ready.\n", e2->Client.Persistent.Name.c_str());
				if (strlen(text) + strlen(tempStr) < sizeof(text) - 50)
					Q_strcatz(text, tempStr, sizeof(text));
			}
		}
	}

	// number, name, connect time, ping, Score, CTF.Admin

	*text = 0;
	for (uint8 i = 1; i <= game.maxclients; i++)
	{
		CPlayerEntity *e2 = entity_cast<CPlayerEntity>(g_edicts[i].Entity);
		if (!e2->GetInUse())
			continue;

		Q_snprintfz(tempStr, sizeof(tempStr), "%3d %-16.16s %02d:%02d %4d %3d%s%s\n",
			i + 1,
			e2->Client.Persistent.Name.c_str(),
			(level.Frame - e2->Client.Respawn.EnterFrame) / 600,
			((level.Frame - e2->Client.Respawn.EnterFrame) % 600)/10,
			e2->Client.GetPing(),
			e2->Client.Respawn.Score,
			(ctfgame.match == MATCH_SETUP || ctfgame.match == MATCH_PREGAME) ?
			(e2->Client.Respawn.CTF.Ready ? " (ready)" : " (notready)") : "",
			e2->Client.Respawn.CTF.Admin ? " (admin)" : "");
		if (strlen(text) + strlen(tempStr) > sizeof(text) - 50)
		{
			Q_snprintfz(text+strlen(text), sizeof(text), "And more...\n");
			ent->PrintToClient (PRINT_HIGH, "%s", text);
			return;
		}
		Q_strcatz(text, tempStr, sizeof(text));
	}
	ent->PrintToClient (PRINT_HIGH, "%s", text);
}


void CTFWarp(CPlayerEntity *ent)
{
	static char text[1024];
	static char *mlist;
	char *token;
	static const char *seps = " \t\n\r";

	if (ArgCount() < 2)
	{
		ent->PrintToClient (PRINT_HIGH, "Where do you want to warp to?\nAvailable levels are: %s\n", warp_list->String());
		return;
	}

	mlist = Mem_PoolStrDup(warp_list->String(), com_genericPool, 0);

	token = strtok(mlist, seps);
	while (token != NULL)
	{
		if (Q_stricmp (ArgGets(1).c_str(), token) == 0)
			break;
		token = strtok(NULL, seps);
	}

	if (token == NULL)
	{
		ent->PrintToClient (PRINT_HIGH, "Unknown CTF level.\n");
		ent->PrintToClient (PRINT_HIGH, "Available levels are: %s\n", warp_list->String());
		QDelete mlist;
		return;
	}

	QDelete mlist;

	if (ent->Client.Respawn.CTF.Admin)
	{
		BroadcastPrintf(PRINT_HIGH, "%s is warping to level %s.\n", 
			ent->Client.Persistent.Name.c_str(), ArgGets(1).c_str());
		level.ForceMap = ArgGets(1);
		EndDMLevel();
		return;
	}

	Q_snprintfz(text, sizeof(text), "%s has requested warping to level %s.", 
			ent->Client.Persistent.Name.c_str(), ArgGets(1).c_str());
	if (CTFBeginElection(ent, ELECT_MAP, text))
		Q_strncpyz(ctfgame.elevel, ArgGets(1).c_str(), sizeof(ctfgame.elevel) - 1);
}

void CTFBoot(CPlayerEntity *ent)
{
	sint32 i;
	static char text[80];

	if (!ent->Client.Respawn.CTF.Admin)
	{
		ent->PrintToClient (PRINT_HIGH, "You are not an admin.\n");
		return;
	}

	if (ArgCount() < 2)
	{
		ent->PrintToClient (PRINT_HIGH, "Who do you want to kick?\n");
		return;
	}

	if (ArgGets(1)[0] < '0' && ArgGets(1)[0] > '9')
	{
		ent->PrintToClient (PRINT_HIGH, "Specify the player number to kick.\n");
		return;
	}

	i = ArgGeti(1);
	if (i < 1 || i > game.maxclients)
	{
		ent->PrintToClient (PRINT_HIGH, "Invalid player number.\n");
		return;
	}

	CPlayerEntity *targ = entity_cast<CPlayerEntity>(g_edicts[i].Entity);
	if (!targ->GetInUse())
	{
		ent->PrintToClient (PRINT_HIGH, "That player number is not connected.\n");
		return;
	}

	Q_snprintfz(text, sizeof(text), "kick %d\n", i - 1);
	gi.AddCommandString(text);
}
#endif
