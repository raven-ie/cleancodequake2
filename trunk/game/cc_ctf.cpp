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

/*--------------------------------------------------------------------------*/

edict_t *SelectRandomDeathmatchSpawnPoint (void);
edict_t *SelectFarthestDeathmatchSpawnPoint (void);
float	PlayersRangeFromSpot (edict_t *spot);


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
	edict_t *flag;
	char *c;
	vec3_t v1, v2;

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
	flag_item = (targ->Client.resp.ctf_team == CTF_TEAM1) ? RedFlag : BlueFlag;
	enemy_flag_item = (flag_item == RedFlag) ? BlueFlag : RedFlag;

	// did the attacker frag the flag carrier?
	if (targ->Client.pers.Inventory.Has(enemy_flag_item))
	{
		attacker->Client.resp.ctf_lastfraggedcarrier = level.framenum;
		attacker->Client.resp.score += CTF_FRAG_CARRIER_BONUS;
		ClientPrintf(attacker->gameEntity, PRINT_MEDIUM, "BONUS: %d points for fragging enemy flag carrier.\n",
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

	flag = NULL;
	while ((flag = G_Find (flag, FOFS(classname), c)) != NULL)
	{
		if (!(flag->spawnflags & DROPPED_ITEM))
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
	vec3_t origin;
	targ->State.GetOrigin (origin);
	Vec3Subtract(origin, flag->state.origin, v1);
	attacker->State.GetOrigin (origin);
	Vec3Subtract(origin, flag->state.origin, v2);

	if ((Vec3Length(v1) < CTF_TARGET_PROTECT_RADIUS ||
		Vec3Length(v2) < CTF_TARGET_PROTECT_RADIUS ||
		loc_CanSee(flag, targ->gameEntity) || loc_CanSee(flag, attacker->gameEntity)) &&
		attacker->Client.resp.ctf_team != targ->Client.resp.ctf_team)
	{
		// we defended the base flag
		attacker->Client.resp.score += CTF_FLAG_DEFENSE_BONUS;
		if (flag->solid == SOLID_NOT)
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
		targ->State.GetOrigin (origin);
		carrier->State.GetOrigin (v1);
		Vec3Subtract(origin, v1, v1);
		attacker->State.GetOrigin (origin);
		carrier->State.GetOrigin (v2);
		Vec3Subtract(origin, v2, v2);

		if (Vec3Length(v1) < CTF_ATTACKER_PROTECT_RADIUS ||
			Vec3Length(v2) < CTF_ATTACKER_PROTECT_RADIUS ||
			loc_CanSee(carrier->gameEntity, targ->gameEntity) || loc_CanSee(carrier->gameEntity, attacker->gameEntity))
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
		flag_item = BlueFlag;
	else
		flag_item = RedFlag;

	if (targ->Client.pers.Inventory.Has(flag_item) &&
		targ->Client.resp.ctf_team != attacker->Client.resp.ctf_team)
		attacker->Client.resp.ctf_lasthurtcarrier = level.framenum;
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
		ent->nextthink - level.framenum > CTF_AUTO_FLAG_RETURN_TIMEOUT-2)
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
void CTFDeadDropFlag(CPlayerEntity *self)
{
	edict_t *dropped = NULL;

	if (self->Client.pers.Flag)
	{
		dropped = self->Client.pers.Flag->DropItem (self->gameEntity);
		self->Client.pers.Inventory.Set (self->Client.pers.Flag, 0);
		BroadcastPrintf (PRINT_HIGH, "%s lost the %s flag!\n", self->Client.pers.netname, CTFTeamName(self->Client.pers.Flag->team));
		self->Client.pers.Flag = NULL;
	}

	if (dropped)
	{
		dropped->think = CTFDropFlagThink;
		dropped->nextthink = level.framenum + CTF_AUTO_FLAG_RETURN_TIMEOUT;
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
	ent->nextthink = level.framenum + FRAMETIME;
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

	ent->nextthink = level.framenum + FRAMETIME;
	ent->think = CTFFlagThink;
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
	ClientPrintf (ent->gameEntity, PRINT_HIGH, "%s player identification display\n", (ent->Client.resp.id_state) ? "Activating" : "Deactivating");
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

void CTFTeam_f (CPlayerEntity *ent)
{
	char *t, *s;
	int desired_team;

	t = ArgGetConcatenatedString();
	if (!*t)
	{
		ClientPrintf(ent->gameEntity, PRINT_HIGH, "You are on the %s team.\n",
			CTFTeamName(ent->Client.resp.ctf_team));
		return;
	}

	if (ctfgame.match > MATCH_SETUP)
	{
		ClientPrintf(ent->gameEntity, PRINT_HIGH, "Can't change teams in a match.\n");
		return;
	}

	if (Q_stricmp(t, "red") == 0)
		desired_team = CTF_TEAM1;
	else if (Q_stricmp(t, "blue") == 0)
		desired_team = CTF_TEAM2;
	else
	{
		ClientPrintf(ent->gameEntity, PRINT_HIGH, "Unknown team %s.\n", t);
		return;
	}

	if (ent->Client.resp.ctf_team == desired_team)
	{
		ClientPrintf(ent->gameEntity, PRINT_HIGH, "You are already on the %s team.\n",
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
	player_die (ent->gameEntity, ent->gameEntity, ent->gameEntity, 100000, vec3Origin);
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

	vec3_t origin;
	who->State.GetOrigin(origin);
	while ((what = loc_findradius(what, origin, 1024)) != NULL)
	{
		// find what in loc_classnames
		for (i = 0; loc_names[i].classname; i++)
			if (strcmp(what->classname, loc_names[i].classname) == 0)
				break;
		if (!loc_names[i].classname)
			continue;
		// something we can see get priority over something we can't
		cansee = loc_CanSee(what, who->gameEntity);
		if (cansee && !hotsee)
		{
			hotsee = true;
			hotindex = loc_names[i].priority;
			hot = what;
			Vec3Subtract(what->state.origin, origin, v);
			hotdist = Vec3Length(v);
			continue;
		}
		// if we can't see this, but we have something we can see, skip it
		if (hotsee && !cansee)
			continue;
		if (hotsee && hotindex < loc_names[i].priority)
			continue;
		Vec3Subtract(what->state.origin, origin, v);
		newdist = Vec3Length(v);
		if (newdist < hotdist || 
			(cansee && loc_names[i].priority < hotindex))
		{
			hot = what;
			hotdist = newdist;
			hotindex = i;
			hotsee = loc_CanSee(hot, who->gameEntity);
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
	while ((what = G_Find(what, FOFS(classname), hot->classname)) != NULL)
	{
		if (what == hot)
			continue;
		// if we are here, there is more than one, find out if hot
		// is closer to red flag or blue flag
		if ((flag1 = G_Find(NULL, FOFS(classname), "item_flag_team1")) != NULL &&
			(flag2 = G_Find(NULL, FOFS(classname), "item_flag_team2")) != NULL)
		{
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

	if ((item = FindItemByClassname(hot->classname)) == NULL)
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
	Vec3Subtract(origin, hot->state.origin, v);
	if (fabs(v[2]) > fabs(v[0]) && fabs(v[2]) > fabs(v[1]))
		if (v[2] > 0)
			Q_strcatz(buf, "above ", bufSize);
		else
			Q_strcatz(buf, "below ", bufSize);
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
	int			cells;
	int			power_armor_type;

	*buf = 0;

	power_armor_type = who->PowerArmorType ();
	if (power_armor_type)
	{
		cells = who->Client.pers.Inventory.Has(FindItem ("cells"));
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
			!loc_CanSee(targ->gameEntity, who->gameEntity))
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
			ClientPrintf(cl_ent->gameEntity, PRINT_CHAT, "(%s): %s\n", 
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
static void misc_ctf_banner_think (edict_t *ent)
{
	ent->state.frame = (ent->state.frame + 1) % 16;
	ent->nextthink = level.framenum + FRAMETIME;
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
	ent->nextthink = level.framenum + FRAMETIME;
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
	ent->nextthink = level.framenum + FRAMETIME;
}

/* ELECTIONS */

bool CTFBeginElection(CPlayerEntity *ent, elect_t type, char *msg)
{
	int i;
	int count;

	if (electpercentage->Integer() == 0) {
		ClientPrintf(ent->gameEntity, PRINT_HIGH, "Elections are disabled, only an admin can process this action.\n");
		return false;
	}


	if (ctfgame.election != ELECT_NONE) {
		ClientPrintf(ent->gameEntity, PRINT_HIGH, "Election already in progress.\n");
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
		ClientPrintf(ent->gameEntity, PRINT_HIGH, "Not enough players for election.\n");
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

void DoRespawn (edict_t *ent);

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
		edict_t *ent = &g_edicts[i];

		if (ent->inUse && !ent->client)
		{
			if (ent->solid == SOLID_NOT && (ent->think == DoRespawn) &&
				ent->nextthink >= level.framenum)
			{
				ent->nextthink = 0;
				DoRespawn(ent);
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

		CenterPrintf(ent->gameEntity, "******************\n\nMATCH HAS STARTED!\n\n******************");

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
			ent->gameEntity->movetype = MOVETYPE_NOCLIP;
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
		ClientPrintf(ctfgame.etarget->gameEntity, PRINT_HIGH, "Type 'admin' to access the adminstration menu.\n");
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
		ClientPrintf(ent->gameEntity, PRINT_HIGH, "No election is in progress.\n");
		return;
	}
	if (ent->Client.resp.voted)
	{
		ClientPrintf(ent->gameEntity, PRINT_HIGH, "You already voted.\n");
		return;
	}
	if (ctfgame.etarget == ent)
	{
		ClientPrintf(ent->gameEntity, PRINT_HIGH, "You can't vote for yourself.\n");
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
		ClientPrintf(ent->gameEntity, PRINT_HIGH, "No election is in progress.\n");
		return;
	}
	if (ent->Client.resp.voted) {
		ClientPrintf(ent->gameEntity, PRINT_HIGH, "You already voted.\n");
		return;
	}
	if (ctfgame.etarget == ent) {
		ClientPrintf(ent->gameEntity, PRINT_HIGH, "You can't vote for yourself.\n");
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
		ClientPrintf(ent->gameEntity, PRINT_HIGH, "Pick a team first (hit <TAB> for menu)\n");
		return;
	}

	if (ctfgame.match != MATCH_SETUP)
	{
		ClientPrintf(ent->gameEntity, PRINT_HIGH, "A match is not being setup.\n");
		return;
	}

	if (ent->Client.resp.ready)
	{
		ClientPrintf(ent->gameEntity, PRINT_HIGH, "You have already commited.\n");
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
		ClientPrintf(ent->gameEntity, PRINT_HIGH, "Pick a team first (hit <TAB> for menu)\n");
		return;
	}

	if (ctfgame.match != MATCH_SETUP && ctfgame.match != MATCH_PREGAME) {
		ClientPrintf(ent->gameEntity, PRINT_HIGH, "A match is not being setup.\n");
		return;
	}

	if (!ent->Client.resp.ready) {
		ClientPrintf(ent->gameEntity, PRINT_HIGH, "You haven't commited.\n");
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
		ClientPrintf(ent->gameEntity, PRINT_HIGH, "Usage:  ghost <code>\n");
		return;
	}

	if (ent->Client.resp.ctf_team != CTF_NOTEAM)
	{
		ClientPrintf(ent->gameEntity, PRINT_HIGH, "You are already in the game.\n");
		return;
	}
	if (ctfgame.match != MATCH_GAME)
	{
		ClientPrintf(ent->gameEntity, PRINT_HIGH, "No match is in progress.\n");
		return;
	}

	n = ArgGeti(1);

	for (i = 0; i < MAX_CS_CLIENTS; i++)
	{
		if (ctfgame.ghosts[i].code && ctfgame.ghosts[i].code == n)
		{
			ClientPrintf(ent->gameEntity, PRINT_HIGH, "Ghost code accepted, your position has been reinstated.\n");
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
	ClientPrintf(ent->gameEntity, PRINT_HIGH, "Invalid ghost code.\n");
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
	if (ent->gameEntity->movetype == MOVETYPE_NOCLIP)
	{
		ClientPrintf(ent->gameEntity, PRINT_HIGH, "You are already an observer.\n");
		return;
	}

	CGrapple::PlayerResetGrapple(ent);
	CTFDeadDropFlag(ent);
	CTFDeadDropTech(ent);

	ent->gameEntity->movetype = MOVETYPE_NOCLIP;
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
	int t;
	int i, j;
	char text[64];

	if (ctfgame.election != ELECT_NONE && ctfgame.electtime <= level.framenum) {
		BroadcastPrintf(PRINT_CHAT, "Election timed out and has been cancelled.\n");
		ctfgame.election = ELECT_NONE;
	}

	if (ctfgame.match != MATCH_NONE) {
		t = ctfgame.matchtime - level.framenum;

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

		switch (ctfgame.match) {
		case MATCH_SETUP :
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

/*QUAKED trigger_teleport (0.5 0.5 0.5) ?
Players touching this will be teleported
*/
void teleporter_touch (edict_t *self, edict_t *other, plane_t *plane, cmBspSurface_t *surf);

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
	ent->touch = teleporter_touch;
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
			ClientPrintf(ent->gameEntity, PRINT_HIGH, "%s", text);
		ClientPrintf(ent->gameEntity, PRINT_HIGH, "No statistics available.\n");
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
			ClientPrintf(ent->gameEntity, PRINT_HIGH, "%s", text);
			return;
		}
		Q_strcatz(text, st, sizeof(text));
	}
	ClientPrintf(ent->gameEntity, PRINT_HIGH, "%s", text);
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
			ClientPrintf(ent->gameEntity, PRINT_HIGH, "%s", text);
			return;
		}
		Q_strcatz(text, st, sizeof(text));
	}
	ClientPrintf(ent->gameEntity, PRINT_HIGH, "%s", text);
}


void CTFWarp(CPlayerEntity *ent)
{
	char text[1024];
	char *mlist, *token;
	static const char *seps = " \t\n\r";
	char *nextToken;

	if (ArgCount() < 2)
	{
		ClientPrintf(ent->gameEntity, PRINT_HIGH, "Where do you want to warp to?\n");
		ClientPrintf(ent->gameEntity, PRINT_HIGH, "Available levels are: %s\n", warp_list->String());
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
		ClientPrintf(ent->gameEntity, PRINT_HIGH, "Unknown CTF level.\n");
		ClientPrintf(ent->gameEntity, PRINT_HIGH, "Available levels are: %s\n", warp_list->String());
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
		ClientPrintf(ent->gameEntity, PRINT_HIGH, "You are not an admin.\n");
		return;
	}

	if (ArgCount() < 2) {
		ClientPrintf(ent->gameEntity, PRINT_HIGH, "Who do you want to kick?\n");
		return;
	}

	if (*ArgGets(1) < '0' && *ArgGets(1) > '9') {
		ClientPrintf(ent->gameEntity, PRINT_HIGH, "Specify the player number to kick.\n");
		return;
	}

	i = ArgGeti(1);
	if (i < 1 || i > game.maxclients) {
		ClientPrintf(ent->gameEntity, PRINT_HIGH, "Invalid player number.\n");
		return;
	}

	CPlayerEntity *targ = dynamic_cast<CPlayerEntity*>(g_edicts[i].Entity);
	if (!targ->IsInUse())
	{
		ClientPrintf(ent->gameEntity, PRINT_HIGH, "That player number is not connected.\n");
		return;
	}

	Q_snprintfz(text, sizeof(text), "kick %d\n", i - 1);
	gi.AddCommandString(text);
}
#endif