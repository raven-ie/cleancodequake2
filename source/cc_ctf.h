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

#if !defined(__CC_CTF_H__) || !defined(INCLUDE_GUARDS)
#define __CC_CTF_H__

#define CTF_VERSION			1.09b
#define CTF_VSTRING2(x) #x
#define CTF_VSTRING(x) CTF_VSTRING2(x)
#define CTF_STRING_VERSION  CTF_VSTRING(CTF_VERSION)

#define CONFIG_CTF_MATCH (CS_MAXCLIENTS-1)

enum
{
	CTF_NOTEAM,
	CTF_TEAM1,
	CTF_TEAM2
};

enum
{
	CTF_GRAPPLE_STATE_FLY,
	CTF_GRAPPLE_STATE_PULL,
	CTF_GRAPPLE_STATE_HANG
};

typedef struct ghost_s
{
	char netname[16];
	int number;

	// stats
	int deaths;
	int kills;
	int caps;
	int basedef;
	int carrierdef;

	int code; // ghost code
	int team; // team
	int score; // frags at time of disconnect
	CPlayerEntity *ent;
} ghost_t;

enum match_t{
	MATCH_NONE,
	MATCH_SETUP,
	MATCH_PREGAME,
	MATCH_GAME,
	MATCH_POST
};

enum elect_t{
	ELECT_NONE,
	ELECT_MATCH,
	ELECT_ADMIN,
	ELECT_MAP
};

typedef struct ctfgame_s
{
	int team1, team2;
	int total1, total2; // these are only set when going into intermission!
	FrameNumber_t last_flag_capture;
	int last_capture_team;

	match_t match;		// match state
	float matchtime;	// time for match start/end (depends on state)
	int lasttime;		// last time update

	elect_t election;	// election type
	CPlayerEntity *etarget;	// for admin election, who's being elected
	char elevel[32];	// for map election, target level
	int evotes;			// votes so far
	int needvotes;		// votes needed
	FrameNumber_t electtime;	// remaining time until election times out
	char emsg[256];		// election name


	ghost_t ghosts[MAX_CS_CLIENTS]; // ghost codes
} ctfgame_t;

extern CCvar *ctf;

#define CTF_TEAM1_SKIN "ctf_r"
#define CTF_TEAM2_SKIN "ctf_b"

#define CTF_CAPTURE_BONUS		15	// what you get for capture
#define CTF_TEAM_BONUS			10	// what your team gets for capture
#define CTF_RECOVERY_BONUS		1	// what you get for recovery
#define CTF_FLAG_BONUS			0	// what you get for picking up enemy flag
#define CTF_FRAG_CARRIER_BONUS	2	// what you get for fragging enemy flag carrier
#define CTF_FLAG_RETURN_TIME	400	// seconds until auto return

#define CTF_CARRIER_DANGER_PROTECT_BONUS	2	// bonus for fraggin someone who has recently hurt your flag carrier
#define CTF_CARRIER_PROTECT_BONUS			1	// bonus for fraggin someone while either you or your target are near your flag carrier
#define CTF_FLAG_DEFENSE_BONUS				1	// bonus for fraggin someone while either you or your target are near your flag
#define CTF_RETURN_FLAG_ASSIST_BONUS		1	// awarded for returning a flag that causes a capture to happen almost immediately
#define CTF_FRAG_CARRIER_ASSIST_BONUS		2	// award for fragging a flag carrier if a capture happens almost immediately

#define CTF_TARGET_PROTECT_RADIUS			400	// the radius around an object being defended where a target will be worth extra frags
#define CTF_ATTACKER_PROTECT_RADIUS			400	// the radius around an object being defended where an attacker will get extra frags when making kills

#define CTF_CARRIER_DANGER_PROTECT_TIMEOUT	80
#define CTF_FRAG_CARRIER_ASSIST_TIMEOUT		100
#define CTF_RETURN_FLAG_ASSIST_TIMEOUT		100

#define CTF_AUTO_FLAG_RETURN_TIMEOUT		300	// number of seconds before dropped flag auto-returns

#define CTF_GRAPPLE_SPEED					650 // speed of grapple in flight
#define CTF_GRAPPLE_PULL_SPEED				650	// speed player is pulled at

void CTFInit(void);
void CTFSpawn(void);

void SP_info_player_team1(edict_t *self);
void SP_info_player_team2(edict_t *self);

char *CTFTeamName(int team);
char *CTFOtherTeamName(int team);
void CTFAssignSkin(edict_t *ent, char *s);
void CTFAssignTeam(gclient_t *who);
edict_t *SelectCTFSpawnPoint (edict_t *ent);
bool CTFPickup_Flag(edict_t *ent, edict_t *other);
bool CTFDrop_Flag(edict_t *ent, CBaseItem *item);
void CTFEffects(edict_t *player);
void CTFCalcScores(void);
void SetCTFStats(edict_t *ent);
void CTFDeadDropFlag(CPlayerEntity *self);
void CTFScoreboardMessage (edict_t *ent, edict_t *killer, bool reliable);
void CTFTeam_f (CPlayerEntity *ent);
void CTFID_f (CPlayerEntity *ent);
void CTFSay_Team(CPlayerEntity *who, char *msg);
void CTFFlagSetup (edict_t *ent);
void CTFResetFlag(int ctf_team);
void CTFFragBonuses(CPlayerEntity *targ, CPlayerEntity *attacker);
void CTFCheckHurtCarrier(CPlayerEntity *targ, CPlayerEntity *attacker);

//TECH
void CTFOpenJoinMenu(CPlayerEntity *ent);
void CTFVoteYes(CPlayerEntity *ent);
void CTFVoteNo(CPlayerEntity *ent);
void CTFReady(CPlayerEntity *ent);
void CTFNotReady(CPlayerEntity *ent);
bool CTFNextMap(void);
bool CTFMatchSetup(void);
bool CTFMatchOn(void);
void CTFGhost(CPlayerEntity *ent);
void CTFAdmin(CPlayerEntity *ent);
bool CTFInMatch(void);
void CTFStats(CPlayerEntity *ent);
void CTFWarp(CPlayerEntity *ent);
void CTFBoot(CPlayerEntity *ent);
void CTFPlayerList(CPlayerEntity *ent);

bool CTFCheckRules(void);

void SP_misc_ctf_banner (edict_t *ent);
void SP_misc_ctf_small_banner (edict_t *ent);

void CreateCTFStatusbar ();

void CTFObserver(CPlayerEntity *ent);

void SP_trigger_teleport (edict_t *ent);
void SP_info_teleport_destination (edict_t *ent);

extern ctfgame_t ctfgame;

extern	CCvar *ctf;
extern	CCvar *ctf_forcejoin;

extern	CCvar *competition;
extern	CCvar *matchlock;
extern	CCvar *electpercentage;
extern	CCvar *matchtime;
extern	CCvar *matchsetuptime;
extern	CCvar *matchstarttime;
extern	CCvar *admin_password;
extern	CCvar *warp_list;

#else
FILE_WARNING
#endif