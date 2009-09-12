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
// cc_playerentity.h
// Player entity
//

#if !defined(__CC_PLAYERENTITY_H__) || !defined(INCLUDE_GUARDS)
#define __CC_PLAYERENTITY_H__

class CPlayerState
{
protected:
	playerState_t	*playerState; // Private so no one mucks with it if they shouldn't

public:
	CPlayerState (playerState_t *playerState);
	
	pMoveState_t	*GetPMove (); // Direct pointer
	void			SetPMove (pMoveState_t *newState);

	// Since we can't really "return" an array we have to pass it
	void			GetViewAngles (vec3_t in);
	void			GetViewOffset (vec3_t in);

	void			GetGunAngles (vec3_t in);
	void			GetGunOffset (vec3_t in);
	void			GetKickAngles (vec3_t in);

	// Unless, of course, you use the vec3f class :D
	vec3f			GetViewAngles ();
	vec3f			GetViewOffset ();

	vec3f			GetGunAngles ();
	vec3f			GetGunOffset ();
	vec3f			GetKickAngles ();

	void			SetViewAngles (vec3_t in);
	void			SetViewOffset (vec3_t in);

	void			SetGunAngles (vec3_t in);
	void			SetGunOffset (vec3_t in);
	void			SetKickAngles (vec3_t in);

	void			SetViewAngles (vec3f &in);
	void			SetViewOffset (vec3f &in);

	void			SetGunAngles (vec3f &in);
	void			SetGunOffset (vec3f &in);
	void			SetKickAngles (vec3f &in);

	MediaIndex		GetGunIndex ();
	void			SetGunIndex (MediaIndex val);

	int				GetGunFrame ();
	void			SetGunFrame (int val);

	void			GetViewBlend (vec4_t in);
	colorf			GetViewBlend ();
	colorb			GetViewBlendB (); // Name had to be different

	void			SetViewBlend (vec4_t in);
	void			SetViewBlend (colorf in);
	void			SetViewBlend (colorb in);

	float			GetFov ();
	void			SetFov (float value);

	int				GetRdFlags ();
	void			SetRdFlags (int value);

	int16			GetStat (uint8 index);
	void			SetStat (uint8 index, int16 val);
	void			CopyStats (int16 *Stats);
	int16			*GetStats ();

	void			Clear ();
};

enum EGender
{
	GenderMale,
	GenderFemale,
	GenderNeutral
};

// svClient->state options
enum EClientState
{
	SVCS_FREE,		// can be reused for a new connection
	SVCS_CONNECTED,	// has been assigned to a svClient_t, but not in game yet
	SVCS_SPAWNED	// client is fully in game
};

// client data that stays across multiple level loads
typedef struct
{
	char		userinfo[MAX_INFO_STRING];
	IPAddress	IP;
	char		netname[16];
	int			hand;

	EClientState state;			// a loadgame will leave valid entities that
									// just don't have a connection yet

	// values saved and restored from edicts when changing levels
	int			health;
	int			max_health;
	int			savedFlags;

	CInventory	Inventory;

	// ammo capacities
	int			maxAmmoValues[AMMOTAG_MAX];

	CWeapon		*Weapon, *LastWeapon;
	CArmor		*Armor; // Current armor.
#ifdef CLEANCTF_ENABLED
	CFlag		*Flag; // Set if holding a flag
#endif
	CTech		*Tech; // Set if holding a tech
	// Stored here for convenience. (dynamic_cast ew)

	int			power_cubes;	// used for tracking the cubes in coop games
	int			score;			// for calculating total unit score in coop games

	int			game_helpchanged;
	int			helpchanged;

	bool		spectator;			// client is a spectator

	colorb		viewBlend; // View blending
} clientPersistent_t;

// client data that stays across deathmatch respawns
typedef struct
{
	clientPersistent_t	coop_respawn;	// what to set client->pers to on a respawn
	int			enterframe;			// level.framenum the client entered the game
	int			score;				// frags, etc
	vec3_t		cmd_angles;			// angles sent over in the last command

	bool		spectator;			// client is a spectator
	EGender		Gender;
	int			messageLevel;

#ifdef MONSTERS_USE_PATHFINDING
	CPathNode	*LastNode;
#endif

	CMenuState	MenuState;

#ifdef CLEANCTF_ENABLED
//ZOID
	int			ctf_team;			// CTF team
	int			ctf_state;
	FrameNumber_t		ctf_lasthurtcarrier;
	FrameNumber_t		ctf_lastreturnedflag;
	FrameNumber_t		ctf_flagsince;
	FrameNumber_t		ctf_lastfraggedcarrier;
	bool		id_state;
	bool		voted; // for elections
	bool		ready;
	bool		admin;
	struct ghost_s *ghost; // for ghost codes
//ZOID
#endif
} clientRespawn_t;

class CClient
{
protected:
	gclient_t	*client; // Private so no one messes it up!

public:
	CPlayerState	PlayerState;

	vec3f			KickAngles;	// weapon kicks
	vec3f			KickOrigin;
	vec3f			ViewAngle;			// aiming direction
	vec3f			DamageFrom;		// origin for vector calculation
	colorb			DamageBlend;

	CClient (gclient_t *client);

	int				GetPing ();
	void			SetPing (int ping);

	void			Clear ();

	// Member variables
	clientPersistent_t	pers;
	clientRespawn_t		resp;
	pMoveState_t		old_pmove;	// for detecting out-of-pmove changes

	bool		showscores;			// set layout stat
	bool		showinventory;		// set layout stat
	bool		showhelp;
	bool		showhelpicon;

	int			buttons;
	int			latched_buttons;

	CWeapon		*NewWeapon;

	// sum up damage over an entire frame, so
	// shotgun blasts give a single big kick
	int			damage_armor;		// damage absorbed by armor
	int			damage_parmor;		// damage absorbed by power armor
	int			damage_blood;		// damage taken out of health
	int			damage_knockback;	// impact damage

	float		killer_yaw;			// when dead, look at killer

	EWeaponState weaponstate;
	float		v_dmg_roll, v_dmg_pitch;	// damage kicks
	FrameNumber_t		v_dmg_time;
	FrameNumber_t		fall_time;
	float		fall_value;		// for view drop on fall
	byte		bonus_alpha;
	float		bobtime;			// so off-ground doesn't change it
	vec3_t		oldviewangles;
	vec3_t		oldvelocity;

	float		next_drown_time;
	int			old_waterlevel;
	int			breather_sound;

	int			machinegun_shots;	// for weapon raising

	// animation vars
	int			anim_end;
	int			anim_priority;
	bool		anim_duck;
	bool		anim_run;

	// powerup timers
	FrameNumber_t		quad_framenum;
	FrameNumber_t		invincible_framenum;
	FrameNumber_t		breather_framenum;
	FrameNumber_t		enviro_framenum;

	bool		grenade_blew_up;
	bool		grenade_thrown;
	FrameNumber_t		grenade_time;
	int			silencer_shots;
	MediaIndex	weapon_sound;

	FrameNumber_t		pickup_msg_time;

	FrameNumber_t		flood_locktill;		// locked from talking
	FrameNumber_t		flood_when[10];		// when messages were said
	int			flood_whenhead;		// head pointer for when said

	FrameNumber_t		respawn_time;		// can respawn when time > this

	CPlayerEntity		*chase_target;		// player we are chasing
	bool				update_chase;		// need to update chase info?
	int					chase_mode;

#ifdef CLEANCTF_ENABLED
//ZOID
	class CGrappleEntity	*ctf_grapple;		// entity of grapple
	int			ctf_grapplestate;		// true if pulling
	FrameNumber_t		ctf_grapplereleasetime;	// time of grapple release
//ZOID
#endif
	FrameNumber_t		regentime;		// regen tech
	FrameNumber_t		techsndtime;
	FrameNumber_t		lasttechmsg;
};

// Players don't think or have (game) controlled physics
class CPlayerEntity : public CHurtableEntity
{
public:
	CClient		Client;
	bool		NoClip;
	bool		TossPhysics;
	FrameNumber_t		FlySoundDebounceTime;
	FrameNumber_t		DamageDebounceTime;
	FrameNumber_t		AirFinished;

	CPlayerEntity (int Index);
	bool Run ();

	void BeginServerFrame ();

	void BeginDeathmatch ();
	void Begin ();
	bool Connect (char *userinfo);
	void Disconnect ();
	void Obituary (CBaseEntity *attacker);

	void SpectatorRespawn ();
	void Respawn ();
	void PutInServer ();
	void InitPersistent ();
	void InitItemMaxValues ();
	void UserinfoChanged (char *userinfo);
	void FetchEntData ();

	// EndServerFrame-related functions
	inline float	CalcRoll (vec3f &forward, vec3f &right);
	inline void		DamageFeedback (vec3f &forward, vec3f &right);
	inline void		CalcViewOffset (vec3f &forward, vec3f &right, vec3f &up, float xyspeed);
	inline void		CalcGunOffset (vec3f &forward, vec3f &right, vec3f &up, float xyspeed);
	inline void		CalcBlend ();
	inline void		FallingDamage ();
	inline void		WorldEffects ();
	int				PowerArmorType ();
	void			SetClientEffects ();
	inline void		SetClientEvent (float xyspeed);
	inline void		SetClientSound ();
	inline void		SetClientFrame (float xyspeed);
	void			SetStats ();
	void			SetSpectatorStats ();
#ifdef CLEANCTF_ENABLED
	void			SetCTFStats ();
	void			CTFSetIDView ();
	void			CTFScoreboardMessage (bool reliable);
	void			CTFAssignGhost ();

	bool			ApplyStrengthSound();
	bool			ApplyHaste();
	void			ApplyHasteSound();
	bool			HasRegeneration();
#endif

	void			DeadDropTech ();
	void			TossClientWeapon ();
	void			MoveToIntermission ();
	void			ClientThink (userCmd_t *ucmd);

	void			DeathmatchScoreboardMessage (bool reliable);
	void			EndServerFrame ();
	void			LookAtKiller (edict_t *inflictor, edict_t *attacker);

	void			InitResp ();
	static void		SaveClientData ();
	void			SelectSpawnPoint (vec3f &origin, vec3f &angles);
	CBaseEntity		*SelectCoopSpawnPoint ();
#ifdef CLEANCTF_ENABLED

	CBaseEntity		*SelectCTFSpawnPoint ();
	void			CTFAssignTeam ();
	void CTFAssignSkin (char *s);
	bool CTFStart ();
#endif

	void Die (CBaseEntity *inflictor, CBaseEntity *attacker, int damage, vec3f &point);

	// Printing routines
	inline void		PrintToClient (EGamePrintLevel printLevel, char *fmt, ...);

	void UpdateChaseCam();
	void ChaseNext();
	void ChasePrev();
	void GetChaseTarget ();

	void TossHead (int damage);

	void P_ProjectSource (vec3f distance, vec3f &forward, vec3f &right, vec3f &result);
	void PlayerNoiseAt (vec3f Where, int type);
};

#else
FILE_WARNING
#endif