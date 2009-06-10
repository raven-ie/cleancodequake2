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

	void			SetViewAngles (vec3f in);
	void			SetViewOffset (vec3f in);

	void			SetGunAngles (vec3f in);
	void			SetGunOffset (vec3f in);
	void			SetKickAngles (vec3f in);

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
	CTech		*Tech; // Set if holding a tech
#endif
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
	float		ctf_lasthurtcarrier;
	float		ctf_lastreturnedflag;
	float		ctf_flagsince;
	float		ctf_lastfraggedcarrier;
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
	vec3_t		damage_from;		// origin for vector calculation

	float		killer_yaw;			// when dead, look at killer

	EWeaponState weaponstate;
	vec3_t		kick_angles;	// weapon kicks
	vec3_t		kick_origin;
	float		v_dmg_roll, v_dmg_pitch, v_dmg_time;	// damage kicks
	float		fall_time, fall_value;		// for view drop on fall
	byte		bonus_alpha;
	colorb		damage_blend;
	vec3_t		v_angle;			// aiming direction
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
	int			quad_framenum;
	int			invincible_framenum;
	int			breather_framenum;
	int			enviro_framenum;

	bool		grenade_blew_up;
	bool		grenade_thrown;
	float		grenade_time;
	int			silencer_shots;
	MediaIndex	weapon_sound;

	float		pickup_msg_time;

	float		flood_locktill;		// locked from talking
	float		flood_when[10];		// when messages were said
	int			flood_whenhead;		// head pointer for when said

	float		respawn_time;		// can respawn when time > this

	CPlayerEntity		*chase_target;		// player we are chasing
	bool				update_chase;		// need to update chase info?

#ifdef CLEANCTF_ENABLED
//ZOID
	edict_t		*ctf_grapple;		// entity of grapple
	int			ctf_grapplestate;		// true if pulling
	float		ctf_grapplereleasetime;	// time of grapple release
	float		ctf_regentime;		// regen tech
	float		ctf_techsndtime;
	float		ctf_lasttechmsg;
//ZOID
#endif
};

// Players don't think or have (game) controlled physics
class CPlayerEntity : public CBaseEntity
{
public:
	CClient		Client;

	CPlayerEntity (int Index);
	bool Run ();

	void BeginServerFrame ();

	void SpectatorRespawn ();
	void Respawn ();
	void PutInServer ();
	void InitPersistent ();
	void InitItemMaxValues ();
	void UserinfoChanged (char *userinfo);
	void FetchEntData ();

	// EndServerFrame-related functions
	inline float	CalcRoll (vec3_t angles, vec3_t forward, vec3_t right);
	inline void		DamageFeedback (vec3_t forward, vec3_t right, vec3_t up);
	inline void		CalcViewOffset (vec3_t forward, vec3_t right, vec3_t up, float xyspeed);
	inline void		CalcGunOffset (vec3_t forward, vec3_t right, vec3_t up, float xyspeed);
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
#endif

	void			MoveToIntermission ();

	void			DeathmatchScoreboardMessage (bool reliable);
	void			EndServerFrame ();
#ifdef CLEANCTF_ENABLED
	void CTFAssignSkin (char *s);
	bool CTFStart ();
#endif
};