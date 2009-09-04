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
#include "g_local.h"
#ifdef CLEANCTF_ENABLED
#include "m_player.h"

void EndDMLevel (void);

ctfgame_t ctfgame;

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

	CTFBar.AddIf (STAT_CTF_TECH);
		CTFBar.AddPoint_X (-26, true);
		CTFBar.AddPicStat (STAT_CTF_TECH);
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

void stuffcmd(edict_t *ent, char *s) 	
{
   	WriteByte (SVC_STUFFTEXT);	        
	WriteString (s);
    Cast (CASTFLAG_RELIABLE, ent);	
}

/*--------------------------------------------------------------------------*/

/*
=================
findradius

Returns entities that have origins within a spherical area

findradius (origin, radius)
=================
*/
static CBaseEntity *loc_findradius (CBaseEntity *from, vec3f org, float rad)
{
	edict_t *fromEnt;
	if (!from)
		fromEnt = g_edicts;
	else
	{
		fromEnt = from->gameEntity;
		fromEnt++;
	}
	for ( ; fromEnt < &g_edicts[globals.numEdicts]; fromEnt++)
	{
		if (!fromEnt->inUse)
			continue;
		if (!fromEnt->Entity)
			continue;
#if 0
		if (from->solid == SOLID_NOT)
			continue;
#endif

		from = fromEnt->Entity;
		if ((org - (from->State.GetOrigin() + (from->GetMins() + from->GetMaxs()) * 0.5)).Length() > rad)
			continue;
		return from;
	}

	return NULL;
}

static void loc_buildboxpoints(vec3f p[8], vec3f org, vec3f mins, vec3f maxs)
{
	p[0] = org + mins;
	p[1] = p[0];
	p[1].X -= mins.X;
	p[2] = p[0];
	p[2].Y -= mins.Y;
	p[3] = p[0];
	p[3].X -= mins.X;
	p[3].Y -= mins.Y;

	p[4] = org + maxs;
	p[5] = p[4];
	p[5].X -= maxs.X;
	p[6] = p[0];
	p[6].Y -= maxs.Y;
	p[7] = p[0];
	p[7].X -= mins.X;
	p[7].Y -= mins.Y;
}

bool loc_CanSee (CBaseEntity *targ, CBaseEntity *inflictor)
{
	// bmodels need special checking because their origin is 0,0,0
	if ((targ->EntityFlags & ENT_PHYSICS) && (dynamic_cast<CPhysicsEntity*>(targ))->PhysicsType == PHYSICS_PUSH)
		return false; // bmodels not supported

	vec3f	targpoints[8];
	loc_buildboxpoints(targpoints, targ->State.GetOrigin(), targ->GetMins(), targ->GetMaxs());
	
	vec3f viewpoint = inflictor->State.GetOrigin() + vec3f(0,0,inflictor->gameEntity->viewheight);
	for (int i = 0; i < 8; i++) {
		CTrace trace (viewpoint, targpoints[i], inflictor->gameEntity, CONTENTS_MASK_SOLID);
		if (trace.fraction == 1.0)
			return true;
	}

	return false;
}

/*--------------------------------------------------------------------------*/

void CTFSpawn(void)
{
	memset(&ctfgame, 0, sizeof(ctfgame));
	CTFSetupTechSpawn();

	if (competition->Integer() > 1) {
		ctfgame.match = MATCH_SETUP;
		ctfgame.matchtime = level.framenum + matchsetuptime->Integer() * 600;
	}
}

void CTFInit(void)
{
	ctf = QNew (com_gamePool, 0) CCvar("ctf", "0", CVAR_LATCH_SERVER|CVAR_SERVERINFO);
	ctf_forcejoin = QNew (com_gamePool, 0) CCvar("ctf_forcejoin", "", 0);
	competition = QNew (com_gamePool, 0) CCvar("competition", "0", CVAR_SERVERINFO);
	matchlock = QNew (com_gamePool, 0) CCvar("matchlock", "1", CVAR_SERVERINFO);
	electpercentage = QNew (com_gamePool, 0) CCvar("electpercentage", "66", 0);
	matchtime = QNew (com_gamePool, 0) CCvar("matchtime", "20", CVAR_SERVERINFO);
	matchsetuptime = QNew (com_gamePool, 0) CCvar("matchsetuptime", "10", 0);
	matchstarttime = QNew (com_gamePool, 0) CCvar("matchstarttime", "20", 0);
	admin_password = QNew (com_gamePool, 0) CCvar("admin_password", "", 0);
	warp_list = QNew (com_gamePool, 0) CCvar("warp_list", "q2ctf1 q2ctf2 q2ctf3 q2ctf4 q2ctf5", 0);
}

/*--------------------------------------------------------------------------*/

char *CTFTeamName(int team)
{
	switch (team) {
	case CTF_TEAM1:
		return "RED";
	case CTF_TEAM2:
		return "BLUE";
	}
	return "UKNOWN";
}

char *CTFOtherTeamName(int team)
{
	switch (team) {
	case CTF_TEAM1:
		return "BLUE";
	case CTF_TEAM2:
		return "RED";
	}
	return "UKNOWN";
}

int CTFOtherTeam(int team)
{
	switch (team) {
	case CTF_TEAM1:
		return CTF_TEAM2;
	case CTF_TEAM2:
		return CTF_TEAM1;
	}
	return -1; // invalid value
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
	int i;
	CBaseItem *flag_item, *enemy_flag_item;
	int otherteam;
	char *c;
	vec3f v1, v2;

	if (attacker->Client.resp.ghost && (attacker != targ))
		attacker->Client.resp.ghost->kills++;
	if (targ->Client.resp.ghost)
		targ->Client.resp.ghost->deaths++;

	// no bonus for fragging yourself
	if (targ == attacker)
		return;

	otherteam = CTFOtherTeam(targ->Client.resp.ctf_team);
	if (otherteam < 0)
		return; // whoever died isn't on a team

	// same team, if the flag at base, check if he has the enemy flag
	flag_item = (targ->Client.resp.ctf_team == CTF_TEAM1) ? NItems::RedFlag : NItems::BlueFlag;
	enemy_flag_item = (flag_item == NItems::RedFlag) ? NItems::BlueFlag : NItems::RedFlag;

	// did the attacker frag the flag carrier?
	if (targ->Client.pers.Inventory.Has(enemy_flag_item))
	{
		attacker->Client.resp.ctf_lastfraggedcarrier = level.framenum;
		attacker->Client.resp.score += CTF_FRAG_CARRIER_BONUS;
		attacker->PrintToClient (PRINT_MEDIUM, "BONUS: %d points for fragging enemy flag carrier.\n",
			CTF_FRAG_CARRIER_BONUS);

		// the target had the flag, clear the hurt carrier
		// field on the other team
		for (i = 1; i <= game.maxclients; i++)
		{
			CPlayerEntity *ent = dynamic_cast<CPlayerEntity*>((g_edicts + i)->Entity);
			if (ent->IsInUse() && ent->Client.resp.ctf_team == otherteam)
				ent->Client.resp.ctf_lasthurtcarrier = 0;
		}
		return;
	}

	if (targ->Client.resp.ctf_lasthurtcarrier &&
		(level.framenum - targ->Client.resp.ctf_lasthurtcarrier < CTF_CARRIER_DANGER_PROTECT_TIMEOUT) &&
		!attacker->Client.pers.Inventory.Has(flag_item)) {
		// attacker is on the same team as the flag carrier and
		// fragged a guy who hurt our flag carrier
		attacker->Client.resp.score += CTF_CARRIER_DANGER_PROTECT_BONUS;
		BroadcastPrintf(PRINT_MEDIUM, "%s defends %s's flag carrier against an agressive enemy\n",
			attacker->Client.pers.netname, 
			CTFTeamName(attacker->Client.resp.ctf_team));
		if (attacker->Client.resp.ghost)
			attacker->Client.resp.ghost->carrierdef++;
		return;
	}

	// flag and flag carrier area defense bonuses
	// we have to find the flag and carrier entities

	// find the flag
	switch (attacker->Client.resp.ctf_team) {
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
	while ((flag = dynamic_cast<CFlagEntity*>(CC_Find (flag, FOFS(classname), c))) != NULL)
	{
		if (!(flag->gameEntity->spawnflags & DROPPED_ITEM))
			break;
	}

	if (!flag)
		return; // can't find attacker's flag

	// find attacker's team's flag carrier
	CPlayerEntity *carrier = NULL;
	for (i = 1; i <= game.maxclients; i++)
	{
		carrier = dynamic_cast<CPlayerEntity*>((g_edicts + i)->Entity);
		if (carrier->IsInUse() && 
			carrier->Client.pers.Inventory.Has(flag_item))
			break;
		carrier = NULL;
	}

	// ok we have the attackers flag and a pointer to the carrier

	// check to see if we are defending the base's flag
	v1 = targ->State.GetOrigin() - flag->State.GetOrigin();
	v2 = attacker->State.GetOrigin() - flag->State.GetOrigin();

	if ((v1.Length() < CTF_TARGET_PROTECT_RADIUS ||
		v2.Length() < CTF_TARGET_PROTECT_RADIUS ||
		loc_CanSee(flag, targ) || loc_CanSee(flag, attacker)) &&
		attacker->Client.resp.ctf_team != targ->Client.resp.ctf_team)
	{
		// we defended the base flag
		attacker->Client.resp.score += CTF_FLAG_DEFENSE_BONUS;
		if (flag->GetSolid() == SOLID_NOT)
			BroadcastPrintf(PRINT_MEDIUM, "%s defends the %s base.\n",
				attacker->Client.pers.netname, 
				CTFTeamName(attacker->Client.resp.ctf_team));
		else
			BroadcastPrintf(PRINT_MEDIUM, "%s defends the %s flag.\n",
				attacker->Client.pers.netname, 
				CTFTeamName(attacker->Client.resp.ctf_team));
		if (attacker->Client.resp.ghost)
			attacker->Client.resp.ghost->basedef++;
		return;
	}

	if (carrier && carrier != attacker)
	{
		v1 = (targ->State.GetOrigin() - carrier->State.GetOrigin());
		v2 = (attacker->State.GetOrigin() - carrier->State.GetOrigin());

		if (Vec3Length(v1) < CTF_ATTACKER_PROTECT_RADIUS ||
			Vec3Length(v2) < CTF_ATTACKER_PROTECT_RADIUS ||
			loc_CanSee(carrier, targ) || loc_CanSee(carrier, attacker))
		{
			attacker->Client.resp.score += CTF_CARRIER_PROTECT_BONUS;
			BroadcastPrintf(PRINT_MEDIUM, "%s defends the %s's flag carrier.\n",
				attacker->Client.pers.netname, 
				CTFTeamName(attacker->Client.resp.ctf_team));
			if (attacker->Client.resp.ghost)
				attacker->Client.resp.ghost->carrierdef++;
			return;
		}
	}
}

void CTFCheckHurtCarrier(CPlayerEntity *targ, CPlayerEntity *attacker)
{
	CBaseItem *flag_item;

	if (targ->Client.resp.ctf_team == CTF_TEAM1)
		flag_item = NItems::BlueFlag;
	else
		flag_item = NItems::RedFlag;

	if (targ->Client.pers.Inventory.Has(flag_item) &&
		targ->Client.resp.ctf_team != attacker->Client.resp.ctf_team)
		attacker->Client.resp.ctf_lasthurtcarrier = level.framenum;
}


/*------------------------------------------------------------------------*/

void CTFResetFlag(int ctf_team)
{
	char *c;

	switch (ctf_team) {
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
	while ((ent = dynamic_cast<CFlagEntity*>(CC_Find (ent, FOFS(classname), c))) != NULL)
	{
		if (ent->gameEntity->spawnflags & DROPPED_ITEM)
			ent->Free ();
		else
		{
			ent->SetSvFlags(ent->GetSvFlags() & ~SVF_NOCLIENT);
			ent->SetSolid (SOLID_TRIGGER);
			ent->Link ();
			ent->State.SetEvent (EV_ITEM_RESPAWN);
		}
	}
}

void CTFResetFlags(void)
{
	CTFResetFlag(CTF_TEAM1);
	CTFResetFlag(CTF_TEAM2);
}

// Called from PlayerDie, to drop the flag from a dying player
void CTFDeadDropFlag(CPlayerEntity *self)
{
	if (self->Client.pers.Flag)
	{
		self->Client.pers.Flag->DropItem (self);
		self->Client.pers.Inventory.Set (self->Client.pers.Flag, 0);
		BroadcastPrintf (PRINT_HIGH, "%s lost the %s flag!\n", self->Client.pers.netname, CTFTeamName(self->Client.pers.Flag->team));
		self->Client.pers.Flag = NULL;
	}
}

// called when we enter the intermission
void CTFCalcScores(void)
{
	ctfgame.total1 = ctfgame.total2 = 0;
	for (int i = 0; i < game.maxclients; i++)
	{
		CPlayerEntity *Player = dynamic_cast<CPlayerEntity*>(g_edicts[i+1].Entity);

		if (!Player->IsInUse())
			continue;
		if (Player->Client.resp.ctf_team == CTF_TEAM1)
			ctfgame.total1 += Player->Client.resp.score;
		else if (Player->Client.resp.ctf_team == CTF_TEAM2)
			ctfgame.total2 += Player->Client.resp.score;
	}
}

void CTFID_f (CPlayerEntity *ent)
{
	ent->Client.resp.id_state = !ent->Client.resp.id_state;
	ent->PrintToClient (PRINT_HIGH, "%s player identification display\n", (ent->Client.resp.id_state) ? "Activating" : "Deactivating");
}

/*------------------------------------------------------------------------*/

void CTFTeam_f (CPlayerEntity *ent)
{
	char *t, *s;
	int desired_team;

	t = ArgGetConcatenatedString();
	if (!*t)
	{
		ent->PrintToClient (PRINT_HIGH, "You are on the %s team.\n",
			CTFTeamName(ent->Client.resp.ctf_team));
		return;
	}

	if (ctfgame.match > MATCH_SETUP)
	{
		ent->PrintToClient (PRINT_HIGH, "Can't change teams in a match.\n");
		return;
	}

	if (Q_stricmp(t, "red") == 0)
		desired_team = CTF_TEAM1;
	else if (Q_stricmp(t, "blue") == 0)
		desired_team = CTF_TEAM2;
	else
	{
		ent->PrintToClient (PRINT_HIGH, "Unknown team %s.\n", t);
		return;
	}

	if (ent->Client.resp.ctf_team == desired_team)
	{
		ent->PrintToClient (PRINT_HIGH, "You are already on the %s team.\n",
			CTFTeamName(ent->Client.resp.ctf_team));
		return;
	}

////
	ent->SetSvFlags (0);
	ent->gameEntity->flags &= ~FL_GODMODE;
	ent->Client.resp.ctf_team = desired_team;
	ent->Client.resp.ctf_state = 0;
	s = Info_ValueForKey (ent->Client.pers.userinfo, "skin");
	ent->CTFAssignSkin(s);

	if (ent->GetSolid() == SOLID_NOT)
	{ // spectator
		ent->PutInServer();
		// add a teleportation effect
		ent->State.SetEvent (EV_PLAYER_TELEPORT);
		// hold in place briefly
		ent->Client.PlayerState.GetPMove()->pmFlags = PMF_TIME_TELEPORT;
		ent->Client.PlayerState.GetPMove()->pmTime = 14;
		BroadcastPrintf(PRINT_HIGH, "%s joined the %s team.\n",
			ent->Client.pers.netname, CTFTeamName(desired_team));
		return;
	}

	ent->gameEntity->health = 0;
	ent->Die (ent, ent, 100000, vec3fOrigin);
	// don't even bother waiting for death frames
	ent->gameEntity->deadflag = DEAD_DEAD;
	ent->Respawn ();

	ent->Client.resp.score = 0;

	BroadcastPrintf(PRINT_HIGH, "%s changed to the %s team.\n",
		ent->Client.pers.netname, CTFTeamName(desired_team));
}

/*
======================================================================

SAY_TEAM

======================================================================
*/

// This array is in 'importance order', it indicates what items are
// more important when reporting their names.
struct {
	char *classname;
	int priority;
} loc_names[] = 
{
	{	"item_flag_team1",			1 },
	{	"item_flag_team2",			1 },
	{	"item_quad",				2 }, 
	{	"item_invulnerability",		2 },
	{	"weapon_bfg",				3 },
	{	"weapon_railgun",			4 },
	{	"weapon_rocketlauncher",	4 },
	{	"weapon_hyperblaster",		4 },
	{	"weapon_chaingun",			4 },
	{	"weapon_grenadelauncher",	4 },
	{	"weapon_machinegun",		4 },
	{	"weapon_supershotgun",		4 },
	{	"weapon_shotgun",			4 },
	{	"item_power_screen",		5 },
	{	"item_power_shield",		5 },
	{	"item_armor_body",			6 },
	{	"item_armor_combat",		6 },
	{	"item_armor_jacket",		6 },
	{	"item_silencer",			7 },
	{	"item_breather",			7 },
	{	"item_enviro",				7 },
	{	"item_adrenaline",			7 },
	{	"item_bandolier",			8 },
	{	"item_pack",				8 },
	{ NULL, 0 }
};


static inline void CTFSay_Team_Location(CPlayerEntity *who, char *buf, size_t bufSize)
{
	CBaseEntity *hot = NULL;
	float hotdist = 999999, newdist;
	int hotindex = 999;
	int i;
	CBaseItem *item;
	int nearteam = -1;
	CFlagEntity *flag1, *flag2;
	bool hotsee = false;
	bool cansee;
	CBaseEntity *what = NULL;

	vec3f origin = who->State.GetOrigin();
	while ((what = loc_findradius(what, origin, 1024)) != NULL)
	{
		// find what in loc_classnames
		for (i = 0; loc_names[i].classname; i++)
			if (strcmp(what->gameEntity->classname, loc_names[i].classname) == 0)
				break;
		if (!loc_names[i].classname)
			continue;
		// something we can see get priority over something we can't
		cansee = loc_CanSee(what, who);
		if (cansee && !hotsee)
		{
			hotsee = true;
			hotindex = loc_names[i].priority;
			hot = what;

			hotdist = (what->State.GetOrigin() - origin).Length();
			continue;
		}
		// if we can't see this, but we have something we can see, skip it
		if (hotsee && !cansee)
			continue;
		if (hotsee && hotindex < loc_names[i].priority)
			continue;

		newdist = (what->State.GetOrigin() - origin).Length();
		if (newdist < hotdist || 
			(cansee && loc_names[i].priority < hotindex))
		{
			hot = what;
			hotdist = newdist;
			hotindex = i;
			hotsee = loc_CanSee(hot, who);
		}
	}

	if (!hot)
	{
		Q_strncpyz(buf, "nowhere", bufSize);
		return;
	}

	// we now have the closest item
	// see if there's more than one in the map, if so
	// we need to determine what team is closest
	what = NULL;
	while ((what = CC_Find(what, FOFS(classname), hot->gameEntity->classname)) != NULL)
	{
		if (what == hot)
			continue;
		// if we are here, there is more than one, find out if hot
		// is closer to red flag or blue flag
		if ((flag1 = dynamic_cast<CFlagEntity*>(CC_Find(NULL, FOFS(classname), "item_flag_team1"))) != NULL &&
			(flag2 = dynamic_cast<CFlagEntity*>(CC_Find(NULL, FOFS(classname), "item_flag_team2"))) != NULL)
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

	if ((item = FindItemByClassname(hot->gameEntity->classname)) == NULL)
	{
		Q_strncpyz(buf, "nowhere", bufSize);
		return;
	}

	// in water?
	if (who->gameEntity->waterlevel)
		Q_strncpyz(buf, "in the water ", bufSize);
	else
		*buf = 0;

	// near or above
	vec3f v = origin - hot->State.GetOrigin();
	if (Q_fabs(v.Z) > Q_fabs(v.X) && Q_fabs(v.Z) > Q_fabs(v.Y))
	{
		if (v.Z > 0)
			Q_strcatz(buf, "above ", bufSize);
		else
			Q_strcatz(buf, "below ", bufSize);
	}
	else
		Q_strcatz(buf, "near ", bufSize);

	if (nearteam == CTF_TEAM1)
		Q_strcatz(buf, "the red ", bufSize);
	else if (nearteam == CTF_TEAM2)
		Q_strcatz(buf, "the blue ", bufSize);
	else
		Q_strcatz(buf, "the ", bufSize);

	Q_strcatz(buf, item->Name, bufSize);
}

static inline void CTFSay_Team_Armor(CPlayerEntity *who, char *buf, size_t bufSize)
{
	CBaseItem		*item;
	int			power_armor_type;

	*buf = 0;

	power_armor_type = who->PowerArmorType ();
	if (power_armor_type)
	{
		int cells = who->Client.pers.Inventory.Has(NItems::Cells);
		if (cells)
			Q_snprintfz(buf+strlen(buf), bufSize, "%s with %i cells ",
				(power_armor_type == POWER_ARMOR_SCREEN) ?
				"Power Screen" : "Power Shield", cells);
	}

	item = who->Client.pers.Armor;
	if (item)
	{
		if (*buf)
			Q_strcatz(buf, "and ", bufSize);
			Q_snprintfz(buf+strlen(buf), bufSize, "%i units of %s",
				who->Client.pers.Inventory.Has(item), item->Name, bufSize);
	}

	if (!*buf)
		Q_strncpyz(buf, "no armor", bufSize);
}

static inline void CTFSay_Team_Health(CPlayerEntity *who, char *buf, size_t bufSize)
{
	if (who->gameEntity->health <= 0)
		Q_strncpyz(buf, "dead", bufSize);
	else
		Q_snprintfz(buf, bufSize, "%i health", who->gameEntity->health);
}

static inline void CTFSay_Team_Tech(CPlayerEntity *who, char *buf, size_t bufSize)
{
	// see if the player has a tech powerup
	if (who->Client.pers.Tech)
		Q_snprintfz(buf, bufSize, "the %s", who->Client.pers.Tech->Name);
	else
		Q_strncpyz(buf, "no powerup", bufSize);
}

static inline void CTFSay_Team_Weapon(CPlayerEntity *who, char *buf, size_t bufSize)
{
	if (who->Client.pers.Weapon)
		Q_strncpyz(buf, who->Client.pers.Weapon->Item->Name, bufSize);
	else
		Q_strncpyz(buf, "none", bufSize);
}

static inline void CTFSay_Team_Sight(CPlayerEntity *who, char *buf, size_t bufSize)
{
	int i;
	int n = 0;
	char s[1024];
	char s2[1024];

	*s = *s2 = 0;
	for (i = 1; i <= game.maxclients; i++)
	{
		CPlayerEntity *targ = dynamic_cast<CPlayerEntity*>(g_edicts[i].Entity);
		if (!targ->IsInUse() || 
			targ == who ||
			!loc_CanSee(targ, who))
			continue;
		if (*s2)
		{
			if (strlen(s) + strlen(s2) + 3 < sizeof(s))
			{
				if (n)
					Q_strcatz(s, ", ", sizeof(s));
				Q_strcatz(s, s2, sizeof(s));
				*s2 = 0;
			}
			n++;
		}
		Q_strncpyz(s2, targ->Client.pers.netname, sizeof(s2));
	}
	if (*s2)
	{
		if (strlen(s) + strlen(s2) + 6 < sizeof(s))
		{
			if (n)
				Q_strcatz(s, " and ", sizeof(s));
			Q_strcatz(s, s2, sizeof(s));
		}
		Q_strncpyz(buf, s, bufSize);
	}
	else
		Q_strncpyz(buf, "no one", bufSize);
}

bool CheckFlood(CPlayerEntity *ent);
void CTFSay_Team(CPlayerEntity *who, char *msg)
{
	char outmsg[1024];
	char buf[1024];
	int i;
	char *p;

	if (CheckFlood(who))
		return;

	outmsg[0] = 0;

	if (*msg == '\"') {
		msg[strlen(msg) - 1] = 0;
		msg++;
	}

	for (p = outmsg; *msg && (p - outmsg) < sizeof(outmsg) - 1; msg++) {
		if (*msg == '%') {
			switch (*++msg) {
				case 'l' :
				case 'L' :
					CTFSay_Team_Location(who, buf, sizeof(buf));
					Q_strncpyz(p, buf, sizeof(buf));
					p += strlen(buf);
					break;
				case 'a' :
				case 'A' :
					CTFSay_Team_Armor(who, buf, sizeof(buf));
					Q_strncpyz(p, buf, sizeof(buf));
					p += strlen(buf);
					break;
				case 'h' :
				case 'H' :
					CTFSay_Team_Health(who, buf, sizeof(buf));
					Q_strncpyz(p, buf, sizeof(buf));
					p += strlen(buf);
					break;
				case 't' :
				case 'T' :
					CTFSay_Team_Tech(who, buf, sizeof(buf));
					Q_strncpyz(p, buf, sizeof(buf));
					p += strlen(buf);
					break;
				case 'w' :
				case 'W' :
					CTFSay_Team_Weapon(who, buf, sizeof(buf));
					Q_strncpyz(p, buf, sizeof(buf));
					p += strlen(buf);
					break;

				case 'n' :
				case 'N' :
					CTFSay_Team_Sight(who, buf, sizeof(buf));
					Q_strncpyz(p, buf, sizeof(buf));
					p += strlen(buf);
					break;

				default :
					*p++ = *msg;
			}
		} else
			*p++ = *msg;
	}
	*p = 0;

	for (i = 0; i < game.maxclients; i++) {
		CPlayerEntity *cl_ent = dynamic_cast<CPlayerEntity*>((g_edicts + 1 + i)->Entity);
		if (!cl_ent->IsInUse())
			continue;
		if (cl_ent->Client.pers.state != SVCS_SPAWNED)
			continue;
		if (cl_ent->Client.resp.ctf_team == who->Client.resp.ctf_team)
			cl_ent->PrintToClient (PRINT_CHAT, "(%s): %s\n", 
				who->Client.pers.netname, outmsg);
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

	CMiscCTFBanner (int Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CThinkableEntity (Index)
	{
	};

	bool Run ()
	{
		return CBaseEntity::Run();
	};

	void Think ()
	{
		State.SetFrame ((State.GetFrame() + 1) % 16);
		NextThink = level.framenum + FRAMETIME;
	};

	virtual void Spawn ()
	{
		SetSolid (SOLID_NOT);
		State.SetModelIndex (ModelIndex ("models/ctf/banner/tris.md2"));
		if (gameEntity->spawnflags & 1) // team2
			State.SetSkinNum (1);

		State.SetFrame (rand() % 16);
		Link ();

		NextThink = level.framenum + FRAMETIME;
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

	CMiscCTFBannerSmall (int Index) :
	  CBaseEntity (Index),
	  CMiscCTFBanner (Index)
	{
	};

	void Spawn ()
	{
		SetSolid (SOLID_NOT);
		State.SetModelIndex (ModelIndex ("models/ctf/banner/small.md2"));
		if (gameEntity->spawnflags & 1) // team2
			State.SetSkinNum (1);

		State.SetFrame (rand() % 16);
		Link ();

		NextThink = level.framenum + FRAMETIME;
	};
};

LINK_CLASSNAME_TO_CLASS ("misc_ctf_banner_small", CMiscCTFBannerSmall);

/* ELECTIONS */

bool CTFBeginElection(CPlayerEntity *ent, elect_t type, char *msg)
{
	int i;
	int count;

	if (electpercentage->Integer() == 0) {
		ent->PrintToClient (PRINT_HIGH, "Elections are disabled, only an admin can process this action.\n");
		return false;
	}


	if (ctfgame.election != ELECT_NONE) {
		ent->PrintToClient (PRINT_HIGH, "Election already in progress.\n");
		return false;
	}

	// clear votes
	count = 0;
	for (i = 1; i <= game.maxclients; i++)
	{
		CPlayerEntity *e = dynamic_cast<CPlayerEntity*>((g_edicts + i)->Entity);
		e->Client.resp.voted = false;
		if (e->IsInUse())
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
	ctfgame.electtime = level.framenum + 200; // twenty seconds for election
	Q_strncpyz(ctfgame.emsg, msg, sizeof(ctfgame.emsg) - 1);

	// tell everyone
	BroadcastPrintf(PRINT_CHAT, "%s\n", ctfgame.emsg);
	BroadcastPrintf(PRINT_HIGH, "Type YES or NO to vote on this request.\n");
	BroadcastPrintf(PRINT_HIGH, "Votes: %d  Needed: %d  Time left: %ds\n", ctfgame.evotes, ctfgame.needvotes,
		(int)((ctfgame.electtime - level.framenum) / 10));

	return true;
}

void CTFResetAllPlayers(void)
{
	for (int i = 1; i <= game.maxclients; i++)
	{
		CPlayerEntity *ent = dynamic_cast<CPlayerEntity*>(g_edicts[i].Entity);
		if (!ent->IsInUse())
			continue;

		//if (ent->client->menu)
		//	PMenu_Close(ent);
		if (ent->Client.resp.MenuState.InMenu)
			ent->Client.resp.MenuState.CloseMenu();

		CGrapple::PlayerResetGrapple(ent);
		CTFDeadDropFlag(ent);
		CTFDeadDropTech(ent);

		ent->Client.resp.ctf_team = CTF_NOTEAM;
		ent->Client.resp.ready = false;

		ent->gameEntity->svFlags = 0;
		ent->gameEntity->flags &= ~FL_GODMODE;
		ent->PutInServer();
	}

	// reset the level
	CTFResetTech();
	CTFResetFlags();

	for (int i = game.maxclients; i < globals.numEdicts; i++)
	{
		CBaseEntity *ent = g_edicts[i].Entity;

		if (ent && ent->IsInUse() && (ent->EntityFlags & ENT_ITEM))
		{
			CItemEntity *Item = dynamic_cast<CItemEntity*>(ent);
			if (Item->GetSolid() == SOLID_NOT && Item->ThinkState == ITS_RESPAWN &&
				Item->NextThink >= level.framenum)
			{
				Item->NextThink = 0;
				Item->Think ();
			}
		}
	}
	if (ctfgame.match == MATCH_SETUP)
		ctfgame.matchtime = level.framenum + matchsetuptime->Integer() * 600;
}

// start a match
void CTFStartMatch(void)
{
	ctfgame.match = MATCH_GAME;
	ctfgame.matchtime = level.framenum + matchtime->Integer() * 600;

	ctfgame.team1 = ctfgame.team2 = 0;

	memset(ctfgame.ghosts, 0, sizeof(ctfgame.ghosts));

	for (int i = 1; i <= game.maxclients; i++)
	{
		CPlayerEntity *ent = dynamic_cast<CPlayerEntity*>((g_edicts + i)->Entity);
		if (!ent->IsInUse())
			continue;

		ent->Client.resp.score = 0;
		ent->Client.resp.ctf_state = 0;
		ent->Client.resp.ghost = NULL;

		ent->PrintToClient (PRINT_CENTER, "******************\n\nMATCH HAS STARTED!\n\n******************");

		if (ent->Client.resp.ctf_team != CTF_NOTEAM)
		{
			// make up a ghost code
			ent->CTFAssignGhost();
			CGrapple::PlayerResetGrapple(ent);
			ent->SetSvFlags (SVF_NOCLIENT);
			ent->gameEntity->flags &= ~FL_GODMODE;

			ent->Client.respawn_time = level.framenum + 10 + ((rand()%300)/100);
			ent->Client.PlayerState.GetPMove()->pmType = PMT_DEAD;
			ent->Client.anim_priority = ANIM_DEATH;
			ent->State.SetFrame (FRAME_death308-1);
			ent->Client.anim_end = FRAME_death308;
			ent->gameEntity->deadflag = DEAD_DEAD;
			ent->NoClip = true;
			ent->Client.PlayerState.SetGunIndex(0);
			ent->Link ();
		}
	}
}

void CTFEndMatch(void)
{
	ctfgame.match = MATCH_POST;
	BroadcastPrintf(PRINT_CHAT, "MATCH COMPLETED!\n");

	CTFCalcScores();

	BroadcastPrintf(PRINT_HIGH, "RED TEAM:  %d captures, %d points\n",
		ctfgame.team1, ctfgame.total1);
	BroadcastPrintf(PRINT_HIGH, "BLUE TEAM:  %d captures, %d points\n",
		ctfgame.team2, ctfgame.total2);

	if (ctfgame.team1 > ctfgame.team2)
		BroadcastPrintf(PRINT_CHAT, "RED team won over the BLUE team by %d CAPTURES!\n",
			ctfgame.team1 - ctfgame.team2);
	else if (ctfgame.team2 > ctfgame.team1)
		BroadcastPrintf(PRINT_CHAT, "BLUE team won over the RED team by %d CAPTURES!\n",
			ctfgame.team2 - ctfgame.team1);
	else if (ctfgame.total1 > ctfgame.total2) // frag tie breaker
		BroadcastPrintf(PRINT_CHAT, "RED team won over the BLUE team by %d POINTS!\n",
			ctfgame.total1 - ctfgame.total2);
	else if (ctfgame.total2 > ctfgame.total1) 
		BroadcastPrintf(PRINT_CHAT, "BLUE team won over the RED team by %d POINTS!\n",
			ctfgame.total2 - ctfgame.total1);
	else
		BroadcastPrintf(PRINT_CHAT, "TIE GAME!\n");

	EndDMLevel();
}

bool CTFNextMap(void)
{
	if (ctfgame.match == MATCH_POST) {
		ctfgame.match = MATCH_SETUP;
		CTFResetAllPlayers();
		return true;
	}
	return false;
}

void CTFWinElection(void)
{
	switch (ctfgame.election) {
	case ELECT_MATCH :
		// reset into match mode
		if (competition->Integer() < 3)
			competition->Set ("2");
		ctfgame.match = MATCH_SETUP;
		CTFResetAllPlayers();
		break;

	case ELECT_ADMIN :
		ctfgame.etarget->Client.resp.admin = true;
		BroadcastPrintf(PRINT_HIGH, "%s has become an admin.\n", ctfgame.etarget->Client.pers.netname);
		ctfgame.etarget->PrintToClient (PRINT_HIGH, "Type 'admin' to access the adminstration menu.\n");
		break;

	case ELECT_MAP :
		BroadcastPrintf(PRINT_HIGH, "%s is warping to level %s.\n", 
			ctfgame.etarget->Client.pers.netname, ctfgame.elevel);
		Q_strncpyz(level.forcemap, ctfgame.elevel, sizeof(level.forcemap) - 1);
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
	if (ent->Client.resp.voted)
	{
		ent->PrintToClient (PRINT_HIGH, "You already voted.\n");
		return;
	}
	if (ctfgame.etarget == ent)
	{
		ent->PrintToClient (PRINT_HIGH, "You can't vote for yourself.\n");
		return;
	}

	ent->Client.resp.voted = true;

	ctfgame.evotes++;
	if (ctfgame.evotes == ctfgame.needvotes)
	{
		// the election has been won
		CTFWinElection();
		return;
	}
	BroadcastPrintf(PRINT_HIGH, "%s\n", ctfgame.emsg);
	BroadcastPrintf(PRINT_CHAT, "Votes: %d  Needed: %d  Time left: %ds\n", ctfgame.evotes, ctfgame.needvotes,
		(int)((ctfgame.electtime - level.framenum)/10));
}

void CTFVoteNo(CPlayerEntity *ent)
{
	if (ctfgame.election == ELECT_NONE) {
		ent->PrintToClient (PRINT_HIGH, "No election is in progress.\n");
		return;
	}
	if (ent->Client.resp.voted) {
		ent->PrintToClient (PRINT_HIGH, "You already voted.\n");
		return;
	}
	if (ctfgame.etarget == ent) {
		ent->PrintToClient (PRINT_HIGH, "You can't vote for yourself.\n");
		return;
	}

	ent->Client.resp.voted = true;

	BroadcastPrintf(PRINT_HIGH, "%s\n", ctfgame.emsg);
	BroadcastPrintf(PRINT_CHAT, "Votes: %d  Needed: %d  Time left: %ds\n", ctfgame.evotes, ctfgame.needvotes,
		(int)((ctfgame.electtime - level.framenum)/10));
}

void CTFReady(CPlayerEntity *ent)
{
	int i, j;
	int t1, t2;

	if (ent->Client.resp.ctf_team == CTF_NOTEAM)
	{
		ent->PrintToClient (PRINT_HIGH, "Pick a team first (hit <TAB> for menu)\n");
		return;
	}

	if (ctfgame.match != MATCH_SETUP)
	{
		ent->PrintToClient (PRINT_HIGH, "A match is not being setup.\n");
		return;
	}

	if (ent->Client.resp.ready)
	{
		ent->PrintToClient (PRINT_HIGH, "You have already commited.\n");
		return;
	}

	ent->Client.resp.ready = true;
	BroadcastPrintf(PRINT_HIGH, "%s is ready.\n", ent->Client.pers.netname);

	t1 = t2 = 0;
	for (j = 0, i = 1; i <= game.maxclients; i++)
	{
		CPlayerEntity *e = dynamic_cast<CPlayerEntity*>(g_edicts[i].Entity);
		if (!e->IsInUse())
			continue;
		if (e->Client.resp.ctf_team != CTF_NOTEAM && !e->Client.resp.ready)
			j++;
		if (e->Client.resp.ctf_team == CTF_TEAM1)
			t1++;
		else if (e->Client.resp.ctf_team == CTF_TEAM2)
			t2++;
	}
	if (!j && t1 && t2)
	{
		// everyone has commited
		BroadcastPrintf(PRINT_CHAT, "All players have commited.  Match starting\n");
		ctfgame.match = MATCH_PREGAME;
		ctfgame.matchtime = level.framenum + (matchstarttime->Float() * 10);
	}
}

void CTFNotReady(CPlayerEntity *ent)
{
	if (ent->Client.resp.ctf_team == CTF_NOTEAM) {
		ent->PrintToClient (PRINT_HIGH, "Pick a team first (hit <TAB> for menu)\n");
		return;
	}

	if (ctfgame.match != MATCH_SETUP && ctfgame.match != MATCH_PREGAME) {
		ent->PrintToClient (PRINT_HIGH, "A match is not being setup.\n");
		return;
	}

	if (!ent->Client.resp.ready) {
		ent->PrintToClient (PRINT_HIGH, "You haven't commited.\n");
		return;
	}

	ent->Client.resp.ready = false;
	BroadcastPrintf(PRINT_HIGH, "%s is no longer ready.\n", ent->Client.pers.netname);

	if (ctfgame.match == MATCH_PREGAME)
	{
		BroadcastPrintf(PRINT_CHAT, "Match halted.\n");
		ctfgame.match = MATCH_SETUP;
		ctfgame.matchtime = level.framenum + matchsetuptime->Integer() * 600;
	}
}

void CTFGhost(CPlayerEntity *ent)
{
	int i;
	int n;

	if (ArgCount() < 2)
	{
		ent->PrintToClient (PRINT_HIGH, "Usage:  ghost <code>\n");
		return;
	}

	if (ent->Client.resp.ctf_team != CTF_NOTEAM)
	{
		ent->PrintToClient (PRINT_HIGH, "You are already in the game.\n");
		return;
	}
	if (ctfgame.match != MATCH_GAME)
	{
		ent->PrintToClient (PRINT_HIGH, "No match is in progress.\n");
		return;
	}

	n = ArgGeti(1);

	for (i = 0; i < MAX_CS_CLIENTS; i++)
	{
		if (ctfgame.ghosts[i].code && ctfgame.ghosts[i].code == n)
		{
			ent->PrintToClient (PRINT_HIGH, "Ghost code accepted, your position has been reinstated.\n");
			ctfgame.ghosts[i].ent->Client.resp.ghost = NULL;
			ent->Client.resp.ctf_team = ctfgame.ghosts[i].team;
			ent->Client.resp.ghost = ctfgame.ghosts + i;
			ent->Client.resp.score = ctfgame.ghosts[i].score;
			ent->Client.resp.ctf_state = 0;
			ctfgame.ghosts[i].ent = ent;
			ent->SetSvFlags(0);
			ent->gameEntity->flags &= ~FL_GODMODE;
			ent->PutInServer();
			BroadcastPrintf(PRINT_HIGH, "%s has been reinstated to %s team.\n",
				ent->Client.pers.netname, CTFTeamName(ent->Client.resp.ctf_team));
			return;
		}
	}
	ent->PrintToClient (PRINT_HIGH, "Invalid ghost code.\n");
}

bool CTFMatchSetup(void)
{
	if (ctfgame.match == MATCH_SETUP || ctfgame.match == MATCH_PREGAME)
		return true;
	return false;
}

bool CTFMatchOn(void)
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
	CTFDeadDropFlag(ent);
	CTFDeadDropTech(ent);

	ent->NoClip = true;
	ent->SetSolid(SOLID_NOT);
	ent->SetSvFlags (ent->GetSvFlags() | SVF_NOCLIENT);
	ent->Client.resp.ctf_team = CTF_NOTEAM;
	ent->Client.PlayerState.SetGunIndex(0);
	ent->Client.resp.score = 0;
	ent->Link();
	CTFOpenJoinMenu(ent);
}

bool CTFInMatch(void)
{
	if (ctfgame.match > MATCH_NONE)
		return true;
	return false;
}

bool CTFCheckRules(void)
{
	char text[64];

	if (ctfgame.election != ELECT_NONE && ctfgame.electtime <= level.framenum) {
		BroadcastPrintf(PRINT_CHAT, "Election timed out and has been cancelled.\n");
		ctfgame.election = ELECT_NONE;
	}

	if (ctfgame.match != MATCH_NONE)
	{
		int t = ctfgame.matchtime - level.framenum;

		if (t <= 0) { // time ended on something
			switch (ctfgame.match) {
			case MATCH_SETUP :
				// go back to normal mode
				if (competition->Integer() < 3) {
					ctfgame.match = MATCH_NONE;
					competition->Set("1");
					CTFResetAllPlayers();
				} else {
					// reset the time
					ctfgame.matchtime = level.framenum + matchsetuptime->Integer() * 600;
				}
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

		switch (ctfgame.match)
		{
		case MATCH_SETUP :
			int j, i;
			for (j = 0, i = 1; i <= game.maxclients; i++)
			{
				CPlayerEntity *ent = dynamic_cast<CPlayerEntity*>(g_edicts[i].Entity);
				if (!ent->IsInUse())
					continue;
				if (ent->Client.resp.ctf_team != CTF_NOTEAM &&
					!ent->Client.resp.ready)
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
	int i, e;
	ghost_t *g;
	char st[80];
	char text[1400];

	*text = 0;
	if (ctfgame.match == MATCH_SETUP)
	{
		for (i = 1; i <= game.maxclients; i++)
		{
			CPlayerEntity *e2 = dynamic_cast<CPlayerEntity*>(g_edicts[i].Entity);
			if (!e2->IsInUse())
				continue;
			if (!e2->Client.resp.ready && e2->Client.resp.ctf_team != CTF_NOTEAM)
			{
				Q_snprintfz(st, sizeof(st), "%s is not ready.\n", e2->Client.pers.netname);
				if (strlen(text) + strlen(st) < sizeof(text) - 50)
					Q_strcatz(text, st, sizeof(text));
			}
		}
	}

	for (i = 0, g = ctfgame.ghosts; i < MAX_CS_CLIENTS; i++, g++)
	{
		if (g->ent)
			break;
	}

	if (i == MAX_CS_CLIENTS)
	{
		if (*text)
			ent->PrintToClient (PRINT_HIGH, "%s", text);
		ent->PrintToClient (PRINT_HIGH, "No statistics available.\n");
		return;
	}

	Q_strcatz(text, "  #|Name            |Score|Kills|Death|BasDf|CarDf|Effcy|\n", sizeof(text));

	for (i = 0, g = ctfgame.ghosts; i < MAX_CS_CLIENTS; i++, g++)
	{
		if (!*g->netname)
			continue;

		if (g->deaths + g->kills == 0)
			e = 50;
		else
			e = g->kills * 100 / (g->kills + g->deaths);
		Q_snprintfz(st, sizeof(st), "%3d|%-16.16s|%5d|%5d|%5d|%5d|%5d|%4d%%|\n",
			g->number, 
			g->netname, 
			g->score, 
			g->kills, 
			g->deaths, 
			g->basedef,
			g->carrierdef, 
			e);
		if (strlen(text) + strlen(st) > sizeof(text) - 50) {
			Q_snprintfz(text+strlen(text), sizeof(text), "And more...\n");
			ent->PrintToClient (PRINT_HIGH, "%s", text);
			return;
		}
		Q_strcatz(text, st, sizeof(text));
	}
	ent->PrintToClient (PRINT_HIGH, "%s", text);
}

void CTFPlayerList(CPlayerEntity *ent)
{
	int i;
	char st[80];
	char text[1400];

	*text = 0;
	if (ctfgame.match == MATCH_SETUP)
	{
		for (i = 1; i <= game.maxclients; i++)
		{
			CPlayerEntity *e2 = dynamic_cast<CPlayerEntity*>(g_edicts[i].Entity);
			if (!e2->IsInUse())
				continue;
			if (!e2->Client.resp.ready && e2->Client.resp.ctf_team != CTF_NOTEAM) {
				Q_snprintfz(st, sizeof(st), "%s is not ready.\n", e2->Client.pers.netname);
				if (strlen(text) + strlen(st) < sizeof(text) - 50)
					Q_strcatz(text, st, sizeof(text));
			}
		}
	}

	// number, name, connect time, ping, score, admin

	*text = 0;
	for (i = 1; i <= game.maxclients; i++)
	{
		CPlayerEntity *e2 = dynamic_cast<CPlayerEntity*>(g_edicts[i].Entity);
		if (!e2->IsInUse())
			continue;

		Q_snprintfz(st, sizeof(st), "%3d %-16.16s %02d:%02d %4d %3d%s%s\n",
			i + 1,
			e2->Client.pers.netname,
			(level.framenum - e2->Client.resp.enterframe) / 600,
			((level.framenum - e2->Client.resp.enterframe) % 600)/10,
			e2->Client.GetPing(),
			e2->Client.resp.score,
			(ctfgame.match == MATCH_SETUP || ctfgame.match == MATCH_PREGAME) ?
			(e2->Client.resp.ready ? " (ready)" : " (notready)") : "",
			e2->Client.resp.admin ? " (admin)" : "");
		if (strlen(text) + strlen(st) > sizeof(text) - 50)
		{
			Q_snprintfz(text+strlen(text), sizeof(text), "And more...\n");
			ent->PrintToClient (PRINT_HIGH, "%s", text);
			return;
		}
		Q_strcatz(text, st, sizeof(text));
	}
	ent->PrintToClient (PRINT_HIGH, "%s", text);
}


void CTFWarp(CPlayerEntity *ent)
{
	char text[1024];
	char *mlist, *token;
	static const char *seps = " \t\n\r";
	char *nextToken;

	if (ArgCount() < 2)
	{
		ent->PrintToClient (PRINT_HIGH, "Where do you want to warp to?\n");
		ent->PrintToClient (PRINT_HIGH, "Available levels are: %s\n", warp_list->String());
		return;
	}

	mlist = strdup(warp_list->String());

	token = strtok_s(mlist, seps, &nextToken);
	while (token != NULL)
	{
		if (Q_stricmp(token, ArgGets(1)) == 0)
			break;
		token = strtok_s(NULL, seps, &nextToken);
	}

	if (token == NULL)
	{
		ent->PrintToClient (PRINT_HIGH, "Unknown CTF level.\n");
		ent->PrintToClient (PRINT_HIGH, "Available levels are: %s\n", warp_list->String());
		free(mlist);
		return;
	}

	free(mlist);


	if (ent->Client.resp.admin)
	{
		BroadcastPrintf(PRINT_HIGH, "%s is warping to level %s.\n", 
			ent->Client.pers.netname, ArgGets(1));
		Q_strncpyz(level.forcemap, ArgGets(1), sizeof(level.forcemap) - 1);
		EndDMLevel();
		return;
	}

	Q_snprintfz(text, sizeof(text), "%s has requested warping to level %s.", 
			ent->Client.pers.netname, ArgGets(1));
	if (CTFBeginElection(ent, ELECT_MAP, text))
		Q_strncpyz(ctfgame.elevel, ArgGets(1), sizeof(ctfgame.elevel) - 1);
}

void CTFBoot(CPlayerEntity *ent)
{
	int i;
	char text[80];

	if (!ent->Client.resp.admin) {
		ent->PrintToClient (PRINT_HIGH, "You are not an admin.\n");
		return;
	}

	if (ArgCount() < 2) {
		ent->PrintToClient (PRINT_HIGH, "Who do you want to kick?\n");
		return;
	}

	if (*ArgGets(1) < '0' && *ArgGets(1) > '9') {
		ent->PrintToClient (PRINT_HIGH, "Specify the player number to kick.\n");
		return;
	}

	i = ArgGeti(1);
	if (i < 1 || i > game.maxclients) {
		ent->PrintToClient (PRINT_HIGH, "Invalid player number.\n");
		return;
	}

	CPlayerEntity *targ = dynamic_cast<CPlayerEntity*>(g_edicts[i].Entity);
	if (!targ->IsInUse())
	{
		ent->PrintToClient (PRINT_HIGH, "That player number is not connected.\n");
		return;
	}

	Q_snprintfz(text, sizeof(text), "kick %d\n", i - 1);
	gi.AddCommandString(text);
}
#endif