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

extern CTech *Regeneration;
extern CTech *Haste;
extern CTech *Strength;
extern CTech *Resistance;

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
static edict_t *loc_findradius (edict_t *from, vec3_t org, float rad)
{
	vec3_t	eorg;
	int		j;

	if (!from)
		from = g_edicts;
	else
		from++;
	for ( ; from < &g_edicts[globals.numEdicts]; from++)
	{
		if (!from->inUse)
			continue;
#if 0
		if (from->solid == SOLID_NOT)
			continue;
#endif
		for (j=0 ; j<3 ; j++)
			eorg[j] = org[j] - (from->state.origin[j] + (from->mins[j] + from->maxs[j])*0.5);
		if (Vec3Length(eorg) > rad)
			continue;
		return from;
	}

	return NULL;
}

static void loc_buildboxpoints(vec3_t p[8], vec3_t org, vec3_t mins, vec3_t maxs)
{
	Vec3Add(org, mins, p[0]);
	Vec3Copy(p[0], p[1]);
	p[1][0] -= mins[0];
	Vec3Copy(p[0], p[2]);
	p[2][1] -= mins[1];
	Vec3Copy(p[0], p[3]);
	p[3][0] -= mins[0];
	p[3][1] -= mins[1];
	Vec3Add(org, maxs, p[4]);
	Vec3Copy(p[4], p[5]);
	p[5][0] -= maxs[0];
	Vec3Copy(p[0], p[6]);
	p[6][1] -= maxs[1];
	Vec3Copy(p[0], p[7]);
	p[7][0] -= maxs[0];
	p[7][1] -= maxs[1];
}

bool loc_CanSee (edict_t *targ, edict_t *inflictor)
{
	CTrace	trace;
	vec3_t	targpoints[8];
	int i;
	vec3_t viewpoint;

// bmodels need special checking because their origin is 0,0,0
	if (targ->movetype == MOVETYPE_PUSH)
		return false; // bmodels not supported

	loc_buildboxpoints(targpoints, targ->state.origin, targ->mins, targ->maxs);
	
	Vec3Copy(inflictor->state.origin, viewpoint);
	viewpoint[2] += inflictor->viewheight;

	for (i = 0; i < 8; i++) {
		trace = CTrace (viewpoint, targpoints[i], inflictor, CONTENTS_MASK_SOLID);
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
		ctfgame.matchtime = level.time + matchsetuptime->Integer() * 60;
	}
}

void CTFInit(void)
{
	ctf = new CCvar("ctf", "0", CVAR_LATCH_SERVER|CVAR_SERVERINFO);
	ctf_forcejoin = new CCvar("ctf_forcejoin", "", 0);
	competition = new CCvar("competition", "0", CVAR_SERVERINFO);
	matchlock = new CCvar("matchlock", "1", CVAR_SERVERINFO);
	electpercentage = new CCvar("electpercentage", "66", 0);
	matchtime = new CCvar("matchtime", "20", CVAR_SERVERINFO);
	matchsetuptime = new CCvar("matchsetuptime", "10", 0);
	matchstarttime = new CCvar("matchstarttime", "20", 0);
	admin_password = new CCvar("admin_password", "", 0);
	warp_list = new CCvar("warp_list", "q2ctf1 q2ctf2 q2ctf3 q2ctf4 q2ctf5", 0);
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

/*--------------------------------------------------------------------------*/

edict_t *SelectRandomDeathmatchSpawnPoint (void);
edict_t *SelectFarthestDeathmatchSpawnPoint (void);
float	PlayersRangeFromSpot (edict_t *spot);

void CTFAssignTeam(gclient_t *who)
{
	edict_t		*player;
	int i;
	int team1count = 0, team2count = 0;

	who->resp.ctf_state = 0;

	if (!dmFlags.dfCtfForceJoin)
	{
		who->resp.ctf_team = CTF_NOTEAM;
		return;
	}

	for (i = 1; i <= game.maxclients; i++) {
		player = &g_edicts[i];

		if (!player->inUse || player->client == who)
			continue;

		switch (player->client->resp.ctf_team) {
		case CTF_TEAM1:
			team1count++;
			break;
		case CTF_TEAM2:
			team2count++;
		}
	}
	if (team1count < team2count)
		who->resp.ctf_team = CTF_TEAM1;
	else if (team2count < team1count)
		who->resp.ctf_team = CTF_TEAM2;
	else if (rand() & 1)
		who->resp.ctf_team = CTF_TEAM1;
	else
		who->resp.ctf_team = CTF_TEAM2;
}

/*
================
SelectCTFSpawnPoint

go to a ctf point, but NOT the two points closest
to other players
================
*/
edict_t *SelectCTFSpawnPoint (edict_t *ent)
{
	edict_t	*spot, *spot1, *spot2;
	int		count = 0;
	int		selection;
	float	range, range1, range2;
	char	*cname;

	if (ent->client->resp.ctf_state)
		if ( dmFlags.dfSpawnFarthest)
			return SelectFarthestDeathmatchSpawnPoint ();
		else
			return SelectRandomDeathmatchSpawnPoint ();

	ent->client->resp.ctf_state++;

	switch (ent->client->resp.ctf_team) {
	case CTF_TEAM1:
		cname = "info_player_team1";
		break;
	case CTF_TEAM2:
		cname = "info_player_team2";
		break;
	default:
		return SelectRandomDeathmatchSpawnPoint();
	}

	spot = NULL;
	range1 = range2 = 99999;
	spot1 = spot2 = NULL;

	while ((spot = G_Find (spot, FOFS(classname), cname)) != NULL)
	{
		count++;
		range = PlayersRangeFromSpot(spot);
		if (range < range1)
		{
			range1 = range;
			spot1 = spot;
		}
		else if (range < range2)
		{
			range2 = range;
			spot2 = spot;
		}
	}

	if (!count)
		return SelectRandomDeathmatchSpawnPoint();

	if (count <= 2)
	{
		spot1 = spot2 = NULL;
	}
	else
		count -= 2;

	selection = rand() % count;

	spot = NULL;
	do
	{
		spot = G_Find (spot, FOFS(classname), cname);
		if (spot == spot1 || spot == spot2)
			selection++;
	} while(selection--);

	return spot;
}

/*------------------------------------------------------------------------*/
/*
CTFFragBonuses

Calculate the bonuses for flag defense, flag carrier defense, etc.
Note that bonuses are not cumaltive.  You get one, they are in importance
order.
*/
void CTFFragBonuses(edict_t *targ, edict_t *inflictor, edict_t *attacker)
{
	int i;
	edict_t *ent;
	CBaseItem *flag_item, *enemy_flag_item;
	int otherteam;
	edict_t *flag, *carrier = NULL;
	char *c;
	vec3_t v1, v2;

	if (targ->client && attacker->client) {
		if (attacker->client->resp.ghost)
			if (attacker != targ)
				attacker->client->resp.ghost->kills++;
		if (targ->client->resp.ghost)
			targ->client->resp.ghost->deaths++;
	}

	// no bonus for fragging yourself
	if (!targ->client || !attacker->client || targ == attacker)
		return;

	otherteam = CTFOtherTeam(targ->client->resp.ctf_team);
	if (otherteam < 0)
		return; // whoever died isn't on a team

	// same team, if the flag at base, check if he has the enemy flag
	if (targ->client->resp.ctf_team == CTF_TEAM1)
	{
		flag_item = RedFlag;
		enemy_flag_item = BlueFlag;
	}
	else
	{
		flag_item = BlueFlag;
		enemy_flag_item = RedFlag;
	}

	// did the attacker frag the flag carrier?
	if (targ->client->pers.Inventory.Has(enemy_flag_item))
	{
		attacker->client->resp.ctf_lastfraggedcarrier = level.time;
		attacker->client->resp.score += CTF_FRAG_CARRIER_BONUS;
		ClientPrintf(attacker, PRINT_MEDIUM, "BONUS: %d points for fragging enemy flag carrier.\n",
			CTF_FRAG_CARRIER_BONUS);

		// the target had the flag, clear the hurt carrier
		// field on the other team
		for (i = 1; i <= game.maxclients; i++) {
			ent = g_edicts + i;
			if (ent->inUse && ent->client->resp.ctf_team == otherteam)
				ent->client->resp.ctf_lasthurtcarrier = 0;
		}
		return;
	}

	if (targ->client->resp.ctf_lasthurtcarrier &&
		level.time - targ->client->resp.ctf_lasthurtcarrier < CTF_CARRIER_DANGER_PROTECT_TIMEOUT &&
		!attacker->client->pers.Inventory.Has(flag_item)) {
		// attacker is on the same team as the flag carrier and
		// fragged a guy who hurt our flag carrier
		attacker->client->resp.score += CTF_CARRIER_DANGER_PROTECT_BONUS;
		BroadcastPrintf(PRINT_MEDIUM, "%s defends %s's flag carrier against an agressive enemy\n",
			attacker->client->pers.netname, 
			CTFTeamName(attacker->client->resp.ctf_team));
		if (attacker->client->resp.ghost)
			attacker->client->resp.ghost->carrierdef++;
		return;
	}

	// flag and flag carrier area defense bonuses

	// we have to find the flag and carrier entities

	// find the flag
	switch (attacker->client->resp.ctf_team) {
	case CTF_TEAM1:
		c = "item_flag_team1";
		break;
	case CTF_TEAM2:
		c = "item_flag_team2";
		break;
	default:
		return;
	}

	flag = NULL;
	while ((flag = G_Find (flag, FOFS(classname), c)) != NULL) {
		if (!(flag->spawnflags & DROPPED_ITEM))
			break;
	}

	if (!flag)
		return; // can't find attacker's flag

	// find attacker's team's flag carrier
	for (i = 1; i <= game.maxclients; i++) {
		carrier = g_edicts + i;
		if (carrier->inUse && 
			carrier->client->pers.Inventory.Has(flag_item))
			break;
		carrier = NULL;
	}

	// ok we have the attackers flag and a pointer to the carrier

	// check to see if we are defending the base's flag
	Vec3Subtract(targ->state.origin, flag->state.origin, v1);
	Vec3Subtract(attacker->state.origin, flag->state.origin, v2);

	if ((Vec3Length(v1) < CTF_TARGET_PROTECT_RADIUS ||
		Vec3Length(v2) < CTF_TARGET_PROTECT_RADIUS ||
		loc_CanSee(flag, targ) || loc_CanSee(flag, attacker)) &&
		attacker->client->resp.ctf_team != targ->client->resp.ctf_team) {
		// we defended the base flag
		attacker->client->resp.score += CTF_FLAG_DEFENSE_BONUS;
		if (flag->solid == SOLID_NOT)
			BroadcastPrintf(PRINT_MEDIUM, "%s defends the %s base.\n",
				attacker->client->pers.netname, 
				CTFTeamName(attacker->client->resp.ctf_team));
		else
			BroadcastPrintf(PRINT_MEDIUM, "%s defends the %s flag.\n",
				attacker->client->pers.netname, 
				CTFTeamName(attacker->client->resp.ctf_team));
		if (attacker->client->resp.ghost)
			attacker->client->resp.ghost->basedef++;
		return;
	}

	if (carrier && carrier != attacker) {
		Vec3Subtract(targ->state.origin, carrier->state.origin, v1);
		Vec3Subtract(attacker->state.origin, carrier->state.origin, v1);

		if (Vec3Length(v1) < CTF_ATTACKER_PROTECT_RADIUS ||
			Vec3Length(v2) < CTF_ATTACKER_PROTECT_RADIUS ||
			loc_CanSee(carrier, targ) || loc_CanSee(carrier, attacker)) {
			attacker->client->resp.score += CTF_CARRIER_PROTECT_BONUS;
			BroadcastPrintf(PRINT_MEDIUM, "%s defends the %s's flag carrier.\n",
				attacker->client->pers.netname, 
				CTFTeamName(attacker->client->resp.ctf_team));
			if (attacker->client->resp.ghost)
				attacker->client->resp.ghost->carrierdef++;
			return;
		}
	}
}

void CTFCheckHurtCarrier(edict_t *targ, edict_t *attacker)
{
	CBaseItem *flag_item;

	if (!targ->client || !attacker->client)
		return;

	if (targ->client->resp.ctf_team == CTF_TEAM1)
		flag_item = BlueFlag;
	else
		flag_item = RedFlag;

	if (targ->client->pers.Inventory.Has(flag_item) &&
		targ->client->resp.ctf_team != attacker->client->resp.ctf_team)
		attacker->client->resp.ctf_lasthurtcarrier = level.time;
}


/*------------------------------------------------------------------------*/

void CTFResetFlag(int ctf_team)
{
	char *c;
	edict_t *ent;

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

	ent = NULL;
	while ((ent = G_Find (ent, FOFS(classname), c)) != NULL) {
		if (ent->spawnflags & DROPPED_ITEM)
			G_FreeEdict(ent);
		else {
			ent->svFlags &= ~SVF_NOCLIENT;
			ent->solid = SOLID_TRIGGER;
			gi.linkentity(ent);
			ent->state.event = EV_ITEM_RESPAWN;
		}
	}
}

void CTFResetFlags(void)
{
	CTFResetFlag(CTF_TEAM1);
	CTFResetFlag(CTF_TEAM2);
}

static void CTFDropFlagTouch(edict_t *ent, edict_t *other, plane_t *plane, cmBspSurface_t *surf)
{
	//owner (who dropped us) can't touch for two secs
	if (other == ent->owner && 
		ent->nextthink - level.time > CTF_AUTO_FLAG_RETURN_TIMEOUT-2)
		return;

	TouchItem (ent, other, plane, surf);
}

static void CTFDropFlagThink(edict_t *ent)
{
	// auto return the flag
	// reset flag will remove ourselves
	if (strcmp(ent->classname, "item_flag_team1") == 0) {
		CTFResetFlag(CTF_TEAM1);
		BroadcastPrintf(PRINT_HIGH, "The %s flag has returned!\n",
			CTFTeamName(CTF_TEAM1));
	} else if (strcmp(ent->classname, "item_flag_team2") == 0) {
		CTFResetFlag(CTF_TEAM2);
		BroadcastPrintf(PRINT_HIGH, "The %s flag has returned!\n",
			CTFTeamName(CTF_TEAM2));
	}
}

// Called from PlayerDie, to drop the flag from a dying player
void CTFDeadDropFlag(edict_t *self)
{
	edict_t *dropped = NULL;

	if (self->client->pers.Flag)
	{
		dropped = self->client->pers.Flag->DropItem (self);
		self->client->pers.Inventory.Set (self->client->pers.Flag, 0);
		BroadcastPrintf (PRINT_HIGH, "%s lost the %s flag!\n", self->client->pers.netname, CTFTeamName(self->client->pers.Flag->team));
		self->client->pers.Flag = NULL;
	}

	if (dropped)
	{
		dropped->think = CTFDropFlagThink;
		dropped->nextthink = level.time + CTF_AUTO_FLAG_RETURN_TIMEOUT;
		dropped->touch = CTFDropFlagTouch;
	}
}

bool CTFDrop_Flag(edict_t *ent, CBaseItem *item)
{
	if (rand() & 1) 
		ClientPrintf(ent, PRINT_HIGH, "Only lusers drop flags.\n");
	else
		ClientPrintf(ent, PRINT_HIGH, "Winners don't drop flags.\n");
	return false;
}

static void CTFFlagThink(edict_t *ent)
{
	if (ent->solid != SOLID_NOT)
		ent->state.frame = 173 + (((ent->state.frame - 173) + 1) % 16);
	ent->nextthink = level.time + FRAMETIME;
}


void CTFFlagSetup (edict_t *ent)
{
	CTrace		tr;
	vec3_t		dest;

	Vec3Set (ent->mins, -15, -15, -15);
	Vec3Set (ent->maxs, 15, 15, 15);

	if (ent->model)
		ent->state.modelIndex = ModelIndex(ent->model);
	else
		ent->state.modelIndex = ModelIndex(ent->item->WorldModel);
	ent->solid = SOLID_TRIGGER;
	ent->movetype = MOVETYPE_TOSS;  
	ent->touch = TouchItem;

	vec3_t v = {0,0,-128};
	Vec3Add (ent->state.origin, v, dest);

	tr = CTrace (ent->state.origin, ent->mins, ent->maxs, dest, ent, CONTENTS_MASK_SOLID);
	if (tr.startSolid)
	{
		DebugPrintf ("CTFFlagSetup: %s startSolid at (%f %f %f)\n", ent->classname, ent->state.origin[0], ent->state.origin[1], ent->state.origin[2]);
		G_FreeEdict (ent);
		return;
	}

	Vec3Copy (tr.endPos, ent->state.origin);

	gi.linkentity (ent);

	ent->nextthink = level.time + FRAMETIME;
	ent->think = CTFFlagThink;
}

// called when we enter the intermission
void CTFCalcScores(void)
{
	int i;

	ctfgame.total1 = ctfgame.total2 = 0;
	for (i = 0; i < game.maxclients; i++) {
		if (!g_edicts[i+1].inUse)
			continue;
		if (game.clients[i].resp.ctf_team == CTF_TEAM1)
			ctfgame.total1 += game.clients[i].resp.score;
		else if (game.clients[i].resp.ctf_team == CTF_TEAM2)
			ctfgame.total2 += game.clients[i].resp.score;
	}
}

void CTFID_f (edict_t *ent)
{
	if (ent->client->resp.id_state) {
		ClientPrintf(ent, PRINT_HIGH, "Disabling player identication display.\n");
		ent->client->resp.id_state = false;
	} else {
		ClientPrintf(ent, PRINT_HIGH, "Activating player identication display.\n");
		ent->client->resp.id_state = true;
	}
}

/*------------------------------------------------------------------------*/

/*QUAKED info_player_team1 (1 0 0) (-16 -16 -24) (16 16 32)
potential team1 spawning position for ctf games
*/
void SP_info_player_team1(edict_t *self)
{
}

/*QUAKED info_player_team2 (0 0 1) (-16 -16 -24) (16 16 32)
potential team2 spawning position for ctf games
*/
void SP_info_player_team2(edict_t *self)
{
}

void CTFTeam_f (edict_t *ent)
{
	char *t, *s;
	int desired_team;

	t = ArgGetConcatenatedString();
	if (!*t) {
		ClientPrintf(ent, PRINT_HIGH, "You are on the %s team.\n",
			CTFTeamName(ent->client->resp.ctf_team));
		return;
	}

	if (ctfgame.match > MATCH_SETUP) {
		ClientPrintf(ent, PRINT_HIGH, "Can't change teams in a match.\n");
		return;
	}

	if (Q_stricmp(t, "red") == 0)
		desired_team = CTF_TEAM1;
	else if (Q_stricmp(t, "blue") == 0)
		desired_team = CTF_TEAM2;
	else {
		ClientPrintf(ent, PRINT_HIGH, "Unknown team %s.\n", t);
		return;
	}

	if (ent->client->resp.ctf_team == desired_team) {
		ClientPrintf(ent, PRINT_HIGH, "You are already on the %s team.\n",
			CTFTeamName(ent->client->resp.ctf_team));
		return;
	}

////
	ent->svFlags = 0;
	ent->flags &= ~FL_GODMODE;
	ent->client->resp.ctf_team = desired_team;
	ent->client->resp.ctf_state = 0;
	s = Info_ValueForKey (ent->client->pers.userinfo, "skin");
	CTFAssignSkin(ent, s);

	if (ent->solid == SOLID_NOT) { // spectator
		PutClientInServer (ent);
		// add a teleportation effect
		ent->state.event = EV_PLAYER_TELEPORT;
		// hold in place briefly
		ent->client->playerState.pMove.pmFlags = PMF_TIME_TELEPORT;
		ent->client->playerState.pMove.pmTime = 14;
		BroadcastPrintf(PRINT_HIGH, "%s joined the %s team.\n",
			ent->client->pers.netname, CTFTeamName(desired_team));
		return;
	}

	ent->health = 0;
	player_die (ent, ent, ent, 100000, vec3Origin);
	// don't even bother waiting for death frames
	ent->deadflag = DEAD_DEAD;
	respawn (ent);

	ent->client->resp.score = 0;

	BroadcastPrintf(PRINT_HIGH, "%s changed to the %s team.\n",
		ent->client->pers.netname, CTFTeamName(desired_team));
}

int CTFApplyResistance(CPlayerEntity *ent, int dmg)
{
	float volume = 1.0;

	if (ent->Client.silencer_shots)
		volume = 0.2f;

	if (dmg && (ent->Client.pers.Tech == Resistance))
	{
		// make noise
	   	PlaySoundFrom(ent->gameEntity, CHAN_ITEM, SoundIndex("ctf/tech1.wav"), volume, ATTN_NORM, 0);
		return dmg / 2;
	}
	return dmg;
}

int CTFApplyStrength(CPlayerEntity *ent, int dmg)
{
	if (dmg && (ent->Client.pers.Tech == Strength))
		return dmg * 2;
	return dmg;
}

bool CTFApplyStrengthSound(CPlayerEntity *ent)
{
	float volume = 1.0;

	if (ent->Client.silencer_shots)
		volume = 0.2f;

	if (ent->Client.pers.Tech == Strength)
	{
		if (ent->Client.ctf_techsndtime < level.time)
		{
			ent->Client.ctf_techsndtime = level.time + 1;
			if (ent->Client.quad_framenum > level.framenum)
				PlaySoundFrom(ent->gameEntity, CHAN_ITEM, SoundIndex("ctf/tech2x.wav"), volume, ATTN_NORM, 0);
			else
				PlaySoundFrom(ent->gameEntity, CHAN_ITEM, SoundIndex("ctf/tech2.wav"), volume, ATTN_NORM, 0);
		}
		return true;
	}
	return false;
}


bool CTFApplyHaste(CPlayerEntity *ent)
{
	if (ent->Client.pers.Tech == Haste)
		return true;
	return false;
}

void CTFApplyHasteSound(CPlayerEntity *ent)
{
	float volume = 1.0;

	if (ent->Client.silencer_shots)
		volume = 0.2f;

	if (ent->Client.pers.Tech == Haste) &&
		ent->Client.ctf_techsndtime < level.time)
	{
		ent->Client.ctf_techsndtime = level.time + 1;
		PlaySoundFrom(ent->gameEntity, CHAN_ITEM, SoundIndex("ctf/tech3.wav"), volume, ATTN_NORM, 0);
	}
}

int ArmorIndex (edict_t *ent);
void CTFApplyRegeneration(CPlayerEntity *ent)
{
	bool noise = false;
	CBaseItem *index;
	float volume = 1.0;

	if (ent->Client.silencer_shots)
		volume = 0.2f;

	if (ent->client->pers.Tech == Regeneration)
	{
		if (ent->Client.ctf_regentime < level.time)
		{
			ent->Client.ctf_regentime = level.time;
			if (ent->gameEntity->health < 150)
			{
				ent->gameEntity->health += 5;
				if (ent->gameEntity->health > 150)
					ent->gameEntity->health = 150;
				ent->Client.ctf_regentime += 0.5;
				noise = true;
			}
			index = ent->Client.pers.Armor;
			if (index && ent->Client.pers.Inventory.Has(index) < 150)
			{
				ent->Client.pers.Inventory.Add (index, 5);
				if (ent->Client.pers.Inventory.Has(index) > 150)
					ent->Client.pers.Inventory.Set(index, 150);
				ent->Client.ctf_regentime += 0.5;
				noise = true;
			}
		}
		if (noise && ent->Client.ctf_techsndtime < level.time)
		{
			ent->Client.ctf_techsndtime = level.time + 1;
			PlaySoundFrom(ent->gameEntity, CHAN_ITEM, SoundIndex("ctf/tech4.wav"), volume, ATTN_NORM, 0);
		}
	}
}

bool CTFHasRegeneration(CPlayerEntity *ent)
{
	if (ent->Client.pers.Tech == Regeneration)
		return true;
	return false;
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


static void CTFSay_Team_Location(edict_t *who, char *buf)
{
	edict_t *what = NULL;
	edict_t *hot = NULL;
	float hotdist = 999999, newdist;
	vec3_t v;
	int hotindex = 999;
	int i;
	CBaseItem *item;
	int nearteam = -1;
	edict_t *flag1, *flag2;
	bool hotsee = false;
	bool cansee;

	while ((what = loc_findradius(what, who->state.origin, 1024)) != NULL) {
		// find what in loc_classnames
		for (i = 0; loc_names[i].classname; i++)
			if (strcmp(what->classname, loc_names[i].classname) == 0)
				break;
		if (!loc_names[i].classname)
			continue;
		// something we can see get priority over something we can't
		cansee = loc_CanSee(what, who);
		if (cansee && !hotsee) {
			hotsee = true;
			hotindex = loc_names[i].priority;
			hot = what;
			Vec3Subtract(what->state.origin, who->state.origin, v);
			hotdist = Vec3Length(v);
			continue;
		}
		// if we can't see this, but we have something we can see, skip it
		if (hotsee && !cansee)
			continue;
		if (hotsee && hotindex < loc_names[i].priority)
			continue;
		Vec3Subtract(what->state.origin, who->state.origin, v);
		newdist = Vec3Length(v);
		if (newdist < hotdist || 
			(cansee && loc_names[i].priority < hotindex)) {
			hot = what;
			hotdist = newdist;
			hotindex = i;
			hotsee = loc_CanSee(hot, who);
		}
	}

	if (!hot) {
		Q_strncpyz(buf, "nowhere", sizeof(buf));
		return;
	}

	// we now have the closest item
	// see if there's more than one in the map, if so
	// we need to determine what team is closest
	what = NULL;
	while ((what = G_Find(what, FOFS(classname), hot->classname)) != NULL) {
		if (what == hot)
			continue;
		// if we are here, there is more than one, find out if hot
		// is closer to red flag or blue flag
		if ((flag1 = G_Find(NULL, FOFS(classname), "item_flag_team1")) != NULL &&
			(flag2 = G_Find(NULL, FOFS(classname), "item_flag_team2")) != NULL) {
			Vec3Subtract(hot->state.origin, flag1->state.origin, v);
			hotdist = Vec3Length(v);
			Vec3Subtract(hot->state.origin, flag2->state.origin, v);
			newdist = Vec3Length(v);
			if (hotdist < newdist)
				nearteam = CTF_TEAM1;
			else if (hotdist > newdist)
				nearteam = CTF_TEAM2;
		}
		break;
	}

	if ((item = FindItemByClassname(hot->classname)) == NULL) {
		Q_strncpyz(buf, "nowhere", sizeof(buf));
		return;
	}

	// in water?
	if (who->waterlevel)
		Q_strncpyz(buf, "in the water ", sizeof(buf));
	else
		*buf = 0;

	// near or above
	Vec3Subtract(who->state.origin, hot->state.origin, v);
	if (fabs(v[2]) > fabs(v[0]) && fabs(v[2]) > fabs(v[1]))
		if (v[2] > 0)
			Q_strcatz(buf, "above ", sizeof(buf));
		else
			Q_strcatz(buf, "below ", sizeof(buf));
	else
		Q_strcatz(buf, "near ", sizeof(buf));

	if (nearteam == CTF_TEAM1)
		Q_strcatz(buf, "the red ", sizeof(buf));
	else if (nearteam == CTF_TEAM2)
		Q_strcatz(buf, "the blue ", sizeof(buf));
	else
		Q_strcatz(buf, "the ", sizeof(buf));

	Q_strcatz(buf, item->Name, sizeof(buf));
}

int PowerArmorType (edict_t *ent);
static void CTFSay_Team_Armor(edict_t *who, char *buf)
{
	CBaseItem		*item;
	int			cells;
	int			power_armor_type;

	*buf = 0;

	power_armor_type = PowerArmorType (who);
	if (power_armor_type)
	{
		cells = who->client->pers.Inventory.Has(FindItem ("cells"));
		if (cells)
			Q_snprintfz(buf+strlen(buf), sizeof(buf), "%s with %i cells ",
				(power_armor_type == POWER_ARMOR_SCREEN) ?
				"Power Screen" : "Power Shield", cells);
	}

	item = who->client->pers.Armor;
	if (item)
	{
		if (*buf)
			Q_strcatz(buf, "and ", sizeof(buf));
			Q_snprintfz(buf+strlen(buf), sizeof(buf), "%i units of %s",
				who->client->pers.Inventory.Has(item), item->Name, sizeof(buf));
	}

	if (!*buf)
		Q_strncpyz(buf, "no armor", sizeof(buf));
}

static void CTFSay_Team_Health(edict_t *who, char *buf)
{
	if (who->health <= 0)
		Q_strncpyz(buf, "dead", sizeof(buf));
	else
		Q_snprintfz(buf, sizeof(buf), "%i health", who->health);
}

static void CTFSay_Team_Tech(edict_t *who, char *buf)
{
	// see if the player has a tech powerup
	if (who->client->pers.Tech)
		Q_snprintfz(buf, sizeof(buf), "the %s", who->client->pers.Tech->Name);
	else
		Q_strncpyz(buf, "no powerup", sizeof(buf));
}

static void CTFSay_Team_Weapon(edict_t *who, char *buf)
{
	if (who->client->pers.Weapon)
		Q_strncpyz(buf, who->client->pers.Weapon->Item->Name, sizeof(buf));
	else
		Q_strncpyz(buf, "none", sizeof(buf));
}

static void CTFSay_Team_Sight(edict_t *who, char *buf)
{
	int i;
	edict_t *targ;
	int n = 0;
	char s[1024];
	char s2[1024];

	*s = *s2 = 0;
	for (i = 1; i <= game.maxclients; i++) {
		targ = g_edicts + i;
		if (!targ->inUse || 
			targ == who ||
			!loc_CanSee(targ, who))
			continue;
		if (*s2) {
			if (strlen(s) + strlen(s2) + 3 < sizeof(s)) {
				if (n)
					Q_strcatz(s, ", ", sizeof(s));
				Q_strcatz(s, s2, sizeof(s));
				*s2 = 0;
			}
			n++;
		}
		Q_strncpyz(s2, targ->client->pers.netname, sizeof(s2));
	}
	if (*s2) {
		if (strlen(s) + strlen(s2) + 6 < sizeof(s)) {
			if (n)
				Q_strcatz(s, " and ", sizeof(s));
			Q_strcatz(s, s2, sizeof(s));
		}
		Q_strncpyz(buf, s, sizeof(buf));
	} else
		Q_strncpyz(buf, "no one", sizeof(buf));
}

bool CheckFlood(edict_t *ent);
void CTFSay_Team(edict_t *who, char *msg)
{
	char outmsg[1024];
	char buf[1024];
	int i;
	char *p;
	edict_t *cl_ent;

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
					CTFSay_Team_Location(who, buf);
					Q_strncpyz(p, buf, sizeof(p));
					p += strlen(buf);
					break;
				case 'a' :
				case 'A' :
					CTFSay_Team_Armor(who, buf);
					Q_strncpyz(p, buf, sizeof(p));
					p += strlen(buf);
					break;
				case 'h' :
				case 'H' :
					CTFSay_Team_Health(who, buf);
					Q_strncpyz(p, buf, sizeof(p));
					p += strlen(buf);
					break;
				case 't' :
				case 'T' :
					CTFSay_Team_Tech(who, buf);
					Q_strncpyz(p, buf, sizeof(p));
					p += strlen(buf);
					break;
				case 'w' :
				case 'W' :
					CTFSay_Team_Weapon(who, buf);
					Q_strncpyz(p, buf, sizeof(p));
					p += strlen(buf);
					break;

				case 'n' :
				case 'N' :
					CTFSay_Team_Sight(who, buf);
					Q_strncpyz(p, buf, sizeof(p));
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
		cl_ent = g_edicts + 1 + i;
		if (!cl_ent->inUse)
			continue;
		if (cl_ent->client->resp.ctf_team == who->client->resp.ctf_team)
			ClientPrintf(cl_ent, PRINT_CHAT, "(%s): %s\n", 
				who->client->pers.netname, outmsg);
	}
}

void GCTFSay_Team (edict_t *ent)
{
	CTFSay_Team (ent, ArgGetConcatenatedString());
}

/*-----------------------------------------------------------------------*/
/*QUAKED misc_ctf_banner (1 .5 0) (-4 -64 0) (4 64 248) TEAM2
The origin is the bottom of the banner.
The banner is 248 tall.
*/
static void misc_ctf_banner_think (edict_t *ent)
{
	ent->state.frame = (ent->state.frame + 1) % 16;
	ent->nextthink = level.time + FRAMETIME;
}

void SP_misc_ctf_banner (edict_t *ent)
{
	ent->movetype = MOVETYPE_NONE;
	ent->solid = SOLID_NOT;
	ent->state.modelIndex = ModelIndex ("models/ctf/banner/tris.md2");
	if (ent->spawnflags & 1) // team2
		ent->state.skinNum = 1;

	ent->state.frame = rand() % 16;
	gi.linkentity (ent);

	ent->think = misc_ctf_banner_think;
	ent->nextthink = level.time + FRAMETIME;
}

/*QUAKED misc_ctf_small_banner (1 .5 0) (-4 -32 0) (4 32 124) TEAM2
The origin is the bottom of the banner.
The banner is 124 tall.
*/
void SP_misc_ctf_small_banner (edict_t *ent)
{
	ent->movetype = MOVETYPE_NONE;
	ent->solid = SOLID_NOT;
	ent->state.modelIndex = ModelIndex ("models/ctf/banner/small.md2");
	if (ent->spawnflags & 1) // team2
		ent->state.skinNum = 1;

	ent->state.frame = rand() % 16;
	gi.linkentity (ent);

	ent->think = misc_ctf_banner_think;
	ent->nextthink = level.time + FRAMETIME;
}

/* ELECTIONS */

bool CTFBeginElection(CPlayerEntity *ent, elect_t type, char *msg)
{
	int i;
	int count;

	if (electpercentage->Integer() == 0) {
		ClientPrintf(ent, PRINT_HIGH, "Elections are disabled, only an admin can process this action.\n");
		return false;
	}


	if (ctfgame.election != ELECT_NONE) {
		ClientPrintf(ent, PRINT_HIGH, "Election already in progress.\n");
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
		ClientPrintf(ent, PRINT_HIGH, "Not enough players for election.\n");
		return false;
	}

	ctfgame.etarget = ent;
	ctfgame.election = type;
	ctfgame.evotes = 0;
	ctfgame.needvotes = (count * electpercentage->Integer()) / 100;
	ctfgame.electtime = level.time + 20; // twenty seconds for election
	Q_strncpyz(ctfgame.emsg, msg, sizeof(ctfgame.emsg) - 1);

	// tell everyone
	BroadcastPrintf(PRINT_CHAT, "%s\n", ctfgame.emsg);
	BroadcastPrintf(PRINT_HIGH, "Type YES or NO to vote on this request.\n");
	BroadcastPrintf(PRINT_HIGH, "Votes: %d  Needed: %d  Time left: %ds\n", ctfgame.evotes, ctfgame.needvotes,
		(int)(ctfgame.electtime - level.time));

	return true;
}

void DoRespawn (edict_t *ent);

void CTFResetAllPlayers(void)
{
	int i;
	edict_t *ent;

	for (i = 1; i <= game.maxclients; i++)
	{
		ent = g_edicts + i;
		if (!ent->inUse)
			continue;

		//if (ent->client->menu)
		//	PMenu_Close(ent);

		CGrapple::PlayerResetGrapple(ent);
		CTFDeadDropFlag(ent);
		CTFDeadDropTech(ent);

		ent->client->resp.ctf_team = CTF_NOTEAM;
		ent->client->resp.ready = false;

		ent->svFlags = 0;
		ent->flags &= ~FL_GODMODE;
		PutClientInServer(ent);
	}

	// reset the level
	CTFResetTech();
	CTFResetFlags();

	for (ent = g_edicts + 1, i = 1; i < globals.numEdicts; i++, ent++) {
		if (ent->inUse && !ent->client) {
			if (ent->solid == SOLID_NOT && ent->think == DoRespawn &&
				ent->nextthink >= level.time) {
				ent->nextthink = 0;
				DoRespawn(ent);
			}
		}
	}
	if (ctfgame.match == MATCH_SETUP)
		ctfgame.matchtime = level.time + matchsetuptime->Integer() * 60;
}

// start a match
void CTFStartMatch(void)
{
	int i;
	edict_t *ent;

	ctfgame.match = MATCH_GAME;
	ctfgame.matchtime = level.time + matchtime->Integer() * 60;

	ctfgame.team1 = ctfgame.team2 = 0;

	memset(ctfgame.ghosts, 0, sizeof(ctfgame.ghosts));

	for (i = 1; i <= game.maxclients; i++) {
		ent = g_edicts + i;
		if (!ent->inUse)
			continue;

		ent->client->resp.score = 0;
		ent->client->resp.ctf_state = 0;
		ent->client->resp.ghost = NULL;

		CenterPrintf(ent, "******************\n\nMATCH HAS STARTED!\n\n******************");

		if (ent->client->resp.ctf_team != CTF_NOTEAM) {
			// make up a ghost code
			CTFAssignGhost(ent);
			CGrapple::PlayerResetGrapple(ent);
			ent->svFlags = SVF_NOCLIENT;
			ent->flags &= ~FL_GODMODE;

			ent->client->respawn_time = level.time + 1.0 + ((rand()%30)/10.0);
			ent->client->playerState.pMove.pmType = PMT_DEAD;
			ent->client->anim_priority = ANIM_DEATH;
			ent->state.frame = FRAME_death308-1;
			ent->client->anim_end = FRAME_death308;
			ent->deadflag = DEAD_DEAD;
			ent->movetype = MOVETYPE_NOCLIP;
			ent->client->playerState.gunIndex = 0;
			gi.linkentity (ent);
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
		ctfgame.etarget->client->resp.admin = true;
		BroadcastPrintf(PRINT_HIGH, "%s has become an admin.\n", ctfgame.etarget->client->pers.netname);
		ClientPrintf(ctfgame.etarget, PRINT_HIGH, "Type 'admin' to access the adminstration menu.\n");
		break;

	case ELECT_MAP :
		BroadcastPrintf(PRINT_HIGH, "%s is warping to level %s.\n", 
			ctfgame.etarget->client->pers.netname, ctfgame.elevel);
		Q_strncpyz(level.forcemap, ctfgame.elevel, sizeof(level.forcemap) - 1);
		EndDMLevel();
		break;
	}
	ctfgame.election = ELECT_NONE;
}

void CTFVoteYes(edict_t *ent)
{
	if (ctfgame.election == ELECT_NONE) {
		ClientPrintf(ent, PRINT_HIGH, "No election is in progress.\n");
		return;
	}
	if (ent->client->resp.voted) {
		ClientPrintf(ent, PRINT_HIGH, "You already voted.\n");
		return;
	}
	if (ctfgame.etarget == ent) {
		ClientPrintf(ent, PRINT_HIGH, "You can't vote for yourself.\n");
		return;
	}

	ent->client->resp.voted = true;

	ctfgame.evotes++;
	if (ctfgame.evotes == ctfgame.needvotes) {
		// the election has been won
		CTFWinElection();
		return;
	}
	BroadcastPrintf(PRINT_HIGH, "%s\n", ctfgame.emsg);
	BroadcastPrintf(PRINT_CHAT, "Votes: %d  Needed: %d  Time left: %ds\n", ctfgame.evotes, ctfgame.needvotes,
		(int)(ctfgame.electtime - level.time));
}

void CTFVoteNo(edict_t *ent)
{
	if (ctfgame.election == ELECT_NONE) {
		ClientPrintf(ent, PRINT_HIGH, "No election is in progress.\n");
		return;
	}
	if (ent->client->resp.voted) {
		ClientPrintf(ent, PRINT_HIGH, "You already voted.\n");
		return;
	}
	if (ctfgame.etarget == ent) {
		ClientPrintf(ent, PRINT_HIGH, "You can't vote for yourself.\n");
		return;
	}

	ent->client->resp.voted = true;

	BroadcastPrintf(PRINT_HIGH, "%s\n", ctfgame.emsg);
	BroadcastPrintf(PRINT_CHAT, "Votes: %d  Needed: %d  Time left: %ds\n", ctfgame.evotes, ctfgame.needvotes,
		(int)(ctfgame.electtime - level.time));
}

void CTFReady(edict_t *ent)
{
	int i, j;
	edict_t *e;
	int t1, t2;

	if (ent->client->resp.ctf_team == CTF_NOTEAM) {
		ClientPrintf(ent, PRINT_HIGH, "Pick a team first (hit <TAB> for menu)\n");
		return;
	}

	if (ctfgame.match != MATCH_SETUP) {
		ClientPrintf(ent, PRINT_HIGH, "A match is not being setup.\n");
		return;
	}

	if (ent->client->resp.ready) {
		ClientPrintf(ent, PRINT_HIGH, "You have already commited.\n");
		return;
	}

	ent->client->resp.ready = true;
	BroadcastPrintf(PRINT_HIGH, "%s is ready.\n", ent->client->pers.netname);

	t1 = t2 = 0;
	for (j = 0, i = 1; i <= game.maxclients; i++) {
		e = g_edicts + i;
		if (!e->inUse)
			continue;
		if (e->client->resp.ctf_team != CTF_NOTEAM && !e->client->resp.ready)
			j++;
		if (e->client->resp.ctf_team == CTF_TEAM1)
			t1++;
		else if (e->client->resp.ctf_team == CTF_TEAM2)
			t2++;
	}
	if (!j && t1 && t2) {
		// everyone has commited
		BroadcastPrintf(PRINT_CHAT, "All players have commited.  Match starting\n");
		ctfgame.match = MATCH_PREGAME;
		ctfgame.matchtime = level.time + matchstarttime->Float();
	}
}

void CTFNotReady(edict_t *ent)
{
	if (ent->client->resp.ctf_team == CTF_NOTEAM) {
		ClientPrintf(ent, PRINT_HIGH, "Pick a team first (hit <TAB> for menu)\n");
		return;
	}

	if (ctfgame.match != MATCH_SETUP && ctfgame.match != MATCH_PREGAME) {
		ClientPrintf(ent, PRINT_HIGH, "A match is not being setup.\n");
		return;
	}

	if (!ent->client->resp.ready) {
		ClientPrintf(ent, PRINT_HIGH, "You haven't commited.\n");
		return;
	}

	ent->client->resp.ready = false;
	BroadcastPrintf(PRINT_HIGH, "%s is no longer ready.\n", ent->client->pers.netname);

	if (ctfgame.match == MATCH_PREGAME) {
		BroadcastPrintf(PRINT_CHAT, "Match halted.\n");
		ctfgame.match = MATCH_SETUP;
		ctfgame.matchtime = level.time + matchsetuptime->Integer() * 60;
	}
}

void CTFGhost(edict_t *ent)
{
	int i;
	int n;

	if (ArgCount() < 2) {
		ClientPrintf(ent, PRINT_HIGH, "Usage:  ghost <code>\n");
		return;
	}

	if (ent->client->resp.ctf_team != CTF_NOTEAM) {
		ClientPrintf(ent, PRINT_HIGH, "You are already in the game.\n");
		return;
	}
	if (ctfgame.match != MATCH_GAME) {
		ClientPrintf(ent, PRINT_HIGH, "No match is in progress.\n");
		return;
	}

	n = ArgGeti(1);

	for (i = 0; i < MAX_CS_CLIENTS; i++) {
		if (ctfgame.ghosts[i].code && ctfgame.ghosts[i].code == n) {
			ClientPrintf(ent, PRINT_HIGH, "Ghost code accepted, your position has been reinstated.\n");
			ctfgame.ghosts[i].ent->client->resp.ghost = NULL;
			ent->client->resp.ctf_team = ctfgame.ghosts[i].team;
			ent->client->resp.ghost = ctfgame.ghosts + i;
			ent->client->resp.score = ctfgame.ghosts[i].score;
			ent->client->resp.ctf_state = 0;
			ctfgame.ghosts[i].ent = ent;
			ent->svFlags = 0;
			ent->flags &= ~FL_GODMODE;
			PutClientInServer(ent);
			BroadcastPrintf(PRINT_HIGH, "%s has been reinstated to %s team.\n",
				ent->client->pers.netname, CTFTeamName(ent->client->resp.ctf_team));
			return;
		}
	}
	ClientPrintf(ent, PRINT_HIGH, "Invalid ghost code.\n");
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

void CTFObserver(edict_t *ent)
{
	// start as 'observer'
	if (ent->movetype == MOVETYPE_NOCLIP) {
		ClientPrintf(ent, PRINT_HIGH, "You are already an observer.\n");
		return;
	}

	CGrapple::PlayerResetGrapple(ent);
	CTFDeadDropFlag(ent);
	CTFDeadDropTech(ent);

	ent->movetype = MOVETYPE_NOCLIP;
	ent->solid = SOLID_NOT;
	ent->svFlags |= SVF_NOCLIENT;
	ent->client->resp.ctf_team = CTF_NOTEAM;
	ent->client->playerState.gunIndex = 0;
	ent->client->resp.score = 0;
	gi.linkentity (ent);
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
	int t;
	int i, j;
	char text[64];
	edict_t *ent;

	if (ctfgame.election != ELECT_NONE && ctfgame.electtime <= level.time) {
		BroadcastPrintf(PRINT_CHAT, "Election timed out and has been cancelled.\n");
		ctfgame.election = ELECT_NONE;
	}

	if (ctfgame.match != MATCH_NONE) {
		t = ctfgame.matchtime - level.time;

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
					ctfgame.matchtime = level.time + matchsetuptime->Integer() * 60;
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

		switch (ctfgame.match) {
		case MATCH_SETUP :
			for (j = 0, i = 1; i <= game.maxclients; i++) {
				ent = g_edicts + i;
				if (!ent->inUse)
					continue;
				if (ent->client->resp.ctf_team != CTF_NOTEAM &&
					!ent->client->resp.ready)
					j++;
			}

			if (competition->Integer() < 3)
				Q_snprintfz(text, sizeof(text), "%02d:%02d SETUP: %d not ready",
					t / 60, t % 60, j);
			else
				Q_snprintfz(text, sizeof(text), "SETUP: %d not ready", j);

			gi.configstring (CONFIG_CTF_MATCH, text);
			break;


		case MATCH_PREGAME :
			Q_snprintfz(text, sizeof(text), "%02d:%02d UNTIL START",
				t / 60, t % 60);
			gi.configstring (CONFIG_CTF_MATCH, text);
			break;

		case MATCH_GAME:
			Q_snprintfz(text, sizeof(text), "%02d:%02d MATCH",
				t / 60, t % 60);
			gi.configstring (CONFIG_CTF_MATCH, text);
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

/*--------------------------------------------------------------------------
 * just here to help old map conversions
 *--------------------------------------------------------------------------*/

static void old_teleporter_touch (edict_t *self, edict_t *other, plane_t *plane, cmBspSurface_t *surf)
{
	edict_t		*dest;
	int			i;
	vec3_t		forward;

	if (!other->client)
		return;
	dest = G_Find (NULL, FOFS(targetname), self->target);
	if (!dest)
	{
		DebugPrintf ("Couldn't find destination\n");
		return;
	}

//ZOID
	CGrapple::PlayerResetGrapple(other);
//ZOID

	// unlink to make sure it can't possibly interfere with KillBox
	gi.unlinkentity (other);

	Vec3Copy (dest->state.origin, other->state.origin);
	Vec3Copy (dest->state.origin, other->state.oldOrigin);
//	other->state.origin[2] += 10;

	// clear the velocity and hold them in place briefly
	Vec3Clear (other->velocity);
	other->client->playerState.pMove.pmTime = 160>>3;		// hold time
	other->client->playerState.pMove.pmFlags |= PMF_TIME_TELEPORT;

	// draw the teleport splash at source and on the player
	self->enemy->state.event = EV_PLAYER_TELEPORT;
	other->state.event = EV_PLAYER_TELEPORT;

	// set angles
	for (i=0 ; i<3 ; i++)
		other->client->playerState.pMove.deltaAngles[i] = ANGLE2SHORT(dest->state.angles[i] - other->client->resp.cmd_angles[i]);

	other->state.angles[PITCH] = 0;
	other->state.angles[YAW] = dest->state.angles[YAW];
	other->state.angles[ROLL] = 0;
	Vec3Copy (dest->state.angles, other->client->playerState.viewAngles);
	Vec3Copy (dest->state.angles, other->client->v_angle);

	// give a little forward velocity
	Angles_Vectors (other->client->v_angle, forward, NULL, NULL);
	Vec3Scale(forward, 200, other->velocity);

	// kill anything at the destination
	if (!KillBox (other))
	{
	}

	gi.linkentity (other);
}

/*QUAKED trigger_teleport (0.5 0.5 0.5) ?
Players touching this will be teleported
*/
void SP_trigger_teleport (edict_t *ent)
{
	edict_t *s;
	int i;

	if (!ent->target)
	{
		DebugPrintf ("teleporter without a target.\n");
		G_FreeEdict (ent);
		return;
	}

	ent->svFlags |= SVF_NOCLIENT;
	ent->solid = SOLID_TRIGGER;
	ent->touch = old_teleporter_touch;
	SetModel (ent, ent->model);
	gi.linkentity (ent);

	// noise maker and splash effect dude
	s = G_Spawn();
	ent->enemy = s;
	for (i = 0; i < 3; i++)
		s->state.origin[i] = ent->mins[i] + (ent->maxs[i] - ent->mins[i])/2;
	s->state.sound = SoundIndex ("world/hum1.wav");
	gi.linkentity(s);
	
}

/*QUAKED info_teleport_destination (0.5 0.5 0.5) (-16 -16 -24) (16 16 32)
Point trigger_teleports at these.
*/
void SP_info_teleport_destination (edict_t *ent)
{
	ent->state.origin[2] += 16;
}

/*----------------------------------------------------------------*/

void CTFStats(edict_t *ent)
{
	int i, e;
	ghost_t *g;
	char st[80];
	char text[1400];
	edict_t *e2;

	*text = 0;
	if (ctfgame.match == MATCH_SETUP) {
		for (i = 1; i <= game.maxclients; i++) {
			e2 = g_edicts + i;
			if (!e2->inUse)
				continue;
			if (!e2->client->resp.ready && e2->client->resp.ctf_team != CTF_NOTEAM) {
				Q_snprintfz(st, sizeof(st), "%s is not ready.\n", e2->client->pers.netname);
				if (strlen(text) + strlen(st) < sizeof(text) - 50)
					Q_strcatz(text, st, sizeof(text));
			}
		}
	}

	for (i = 0, g = ctfgame.ghosts; i < MAX_CS_CLIENTS; i++, g++)
		if (g->ent)
			break;

	if (i == MAX_CS_CLIENTS) {
		if (*text)
			ClientPrintf(ent, PRINT_HIGH, "%s", text);
		ClientPrintf(ent, PRINT_HIGH, "No statistics available.\n");
		return;
	}

	Q_strcatz(text, "  #|Name            |Score|Kills|Death|BasDf|CarDf|Effcy|\n", sizeof(text));

	for (i = 0, g = ctfgame.ghosts; i < MAX_CS_CLIENTS; i++, g++) {
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
			ClientPrintf(ent, PRINT_HIGH, "%s", text);
			return;
		}
		Q_strcatz(text, st, sizeof(text));
	}
	ClientPrintf(ent, PRINT_HIGH, "%s", text);
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
			if (!e2->IsInUse)
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
		if (!e2->IsInUse)
			continue;

		Q_snprintfz(st, sizeof(st), "%3d %-16.16s %02d:%02d %4d %3d%s%s\n",
			i + 1,
			e2->Client.pers.netname,
			(level.framenum - e2->Client.resp.enterframe) / 600,
			((level.framenum - e2->Client.resp.enterframe) % 600)/10,
			e2->Client.ping,
			e2->Client.resp.score,
			(ctfgame.match == MATCH_SETUP || ctfgame.match == MATCH_PREGAME) ?
			(e2->Client.resp.ready ? " (ready)" : " (notready)") : "",
			e2->Client.resp.admin ? " (admin)" : "");
		if (strlen(text) + strlen(st) > sizeof(text) - 50)
		{
			Q_snprintfz(text+strlen(text), sizeof(text), "And more...\n");
			ClientPrintf(ent->gameEntity, PRINT_HIGH, "%s", text);
			return;
		}
		Q_strcatz(text, st, sizeof(text));
	}
	ClientPrintf(ent->gameEntity, PRINT_HIGH, "%s", text);
}


void CTFWarp(edict_t *ent)
{
	char text[1024];
	char *mlist, *token;
	static const char *seps = " \t\n\r";
	char *nextToken;

	if (ArgCount() < 2) {
		ClientPrintf(ent, PRINT_HIGH, "Where do you want to warp to?\n");
		ClientPrintf(ent, PRINT_HIGH, "Available levels are: %s\n", warp_list->String());
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
		ClientPrintf(ent, PRINT_HIGH, "Unknown CTF level.\n");
		ClientPrintf(ent, PRINT_HIGH, "Available levels are: %s\n", warp_list->String());
		free(mlist);
		return;
	}

	free(mlist);


	if (ent->client->resp.admin) {
		BroadcastPrintf(PRINT_HIGH, "%s is warping to level %s.\n", 
			ent->client->pers.netname, ArgGets(1));
		Q_strncpyz(level.forcemap, ArgGets(1), sizeof(level.forcemap) - 1);
		EndDMLevel();
		return;
	}

	Q_snprintfz(text, sizeof(text), "%s has requested warping to level %s.", 
			ent->client->pers.netname, ArgGets(1));
	if (CTFBeginElection(ent, ELECT_MAP, text))
		Q_strncpyz(ctfgame.elevel, ArgGets(1), sizeof(ctfgame.elevel) - 1);
}

void CTFBoot(edict_t *ent)
{
	int i;
	edict_t *targ;
	char text[80];

	if (!ent->client->resp.admin) {
		ClientPrintf(ent, PRINT_HIGH, "You are not an admin.\n");
		return;
	}

	if (ArgCount() < 2) {
		ClientPrintf(ent, PRINT_HIGH, "Who do you want to kick?\n");
		return;
	}

	if (*ArgGets(1) < '0' && *ArgGets(1) > '9') {
		ClientPrintf(ent, PRINT_HIGH, "Specify the player number to kick.\n");
		return;
	}

	i = ArgGeti(1);
	if (i < 1 || i > game.maxclients) {
		ClientPrintf(ent, PRINT_HIGH, "Invalid player number.\n");
		return;
	}

	targ = g_edicts + i;
	if (!targ->inUse) {
		ClientPrintf(ent, PRINT_HIGH, "That player number is not connected.\n");
		return;
	}

	Q_snprintfz(text, sizeof(text), "kick %d\n", i - 1);
	gi.AddCommandString(text);
}
#endif