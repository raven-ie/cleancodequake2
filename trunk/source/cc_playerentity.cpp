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
// cc_playerentity.cpp
// Player entity
//

#include "cc_local.h"
#include "m_player.h"
#include "cc_menu.h"
#include "cc_ban.h"

#ifndef USE_EXTENDED_GAME_IMPORTS
#include "cc_pmove.h"
#endif

CPlayerState::CPlayerState (playerState_t *playerState) :
playerState(playerState)
{
};

pMoveState_t	*CPlayerState::GetPMove ()
{
	return &playerState->pMove;
} // Direct pointer
void			CPlayerState::SetPMove (pMoveState_t *newState)
{
	memcpy (&playerState->pMove, newState, sizeof(pMoveState_t));
}

// Since we can't really "return" an array we have to pass it
void			CPlayerState::GetViewAngles (vec3_t in)
{
	Vec3Copy (playerState->viewAngles, in);
}
void			CPlayerState::GetViewOffset (vec3_t in)
{
	Vec3Copy (playerState->viewOffset, in);
}

void			CPlayerState::GetGunAngles (vec3_t in)
{
	Vec3Copy (playerState->gunAngles, in);
}
void			CPlayerState::GetGunOffset (vec3_t in)
{
	Vec3Copy (playerState->gunOffset, in);
}
void			CPlayerState::GetKickAngles (vec3_t in)
{
	Vec3Copy (playerState->kickAngles, in);
}

// Unless, of course, you use the vec3f class :D
vec3f			CPlayerState::GetViewAngles ()
{
	return vec3f(playerState->viewAngles);
}
vec3f			CPlayerState::GetViewOffset ()
{
	return vec3f(playerState->viewOffset);
}

vec3f			CPlayerState::GetGunAngles ()
{
	return vec3f(playerState->gunAngles);
}
vec3f			CPlayerState::GetGunOffset ()
{
	return vec3f(playerState->gunOffset);
}
vec3f			CPlayerState::GetKickAngles ()
{
	return vec3f(playerState->kickAngles);
}

void			CPlayerState::SetViewAngles (vec3_t in)
{
	Vec3Copy (in, playerState->viewAngles);
}
void			CPlayerState::SetViewOffset (vec3_t in)
{
	Vec3Copy (in, playerState->viewOffset);
}

void			CPlayerState::SetGunAngles (vec3_t in)
{
	Vec3Copy (in, playerState->gunAngles);
}
void			CPlayerState::SetGunOffset (vec3_t in)
{
	Vec3Copy (in, playerState->gunOffset);
}
void			CPlayerState::SetKickAngles (vec3_t in)
{
	Vec3Copy (in, playerState->kickAngles);
}

MediaIndex		CPlayerState::GetGunIndex ()
{
	return playerState->gunIndex;
}

int		CPlayerState::GetGunFrame ()
{
	return playerState->gunFrame;
}

void			CPlayerState::SetGunIndex (MediaIndex val)
{
	playerState->gunIndex = val;
}

void			CPlayerState::SetGunFrame (int val)
{
	playerState->gunFrame = val;
}

void			CPlayerState::SetViewAngles (vec3f &in)
{
	playerState->viewAngles[0] = in.X;
	playerState->viewAngles[1] = in.Y;
	playerState->viewAngles[2] = in.Z;
}
void			CPlayerState::SetViewOffset (vec3f &in)
{
	playerState->viewOffset[0] = in.X;
	playerState->viewOffset[1] = in.Y;
	playerState->viewOffset[2] = in.Z;
}

void			CPlayerState::SetGunAngles (vec3f &in)
{
	playerState->gunAngles[0] = in.X;
	playerState->gunAngles[1] = in.Y;
	playerState->gunAngles[2] = in.Z;
}
void			CPlayerState::SetGunOffset (vec3f &in)
{
	playerState->gunOffset[0] = in.X;
	playerState->gunOffset[1] = in.Y;
	playerState->gunOffset[2] = in.Z;
}
void			CPlayerState::SetKickAngles (vec3f &in)
{
	playerState->kickAngles[0] = in.X;
	playerState->kickAngles[1] = in.Y;
	playerState->kickAngles[2] = in.Z;
}

void			CPlayerState::GetViewBlend (vec4_t in)
{
	Vec4Copy (playerState->viewBlend, in);
}
colorf			CPlayerState::GetViewBlend ()
{
	return colorf	(playerState->viewBlend[0],
					playerState->viewBlend[1],
					playerState->viewBlend[2],
					playerState->viewBlend[3]);
}
colorb			CPlayerState::GetViewBlendB ()
{
	// Convert it to a color byte
	return colorb	(((float)playerState->viewBlend[0]) * 255,
					((float)playerState->viewBlend[1]) * 255,
					((float)playerState->viewBlend[2]) * 255,
					((float)playerState->viewBlend[3]) * 255);
} // Name had to be different

void			CPlayerState::SetViewBlend (vec4_t in)
{
	Vec4Copy (in, playerState->viewBlend);
}
void			CPlayerState::SetViewBlend (colorf in)
{
	playerState->viewBlend[0] = in.R;
	playerState->viewBlend[1] = in.G;
	playerState->viewBlend[2] = in.B;
	playerState->viewBlend[3] = in.A;
}
void			CPlayerState::SetViewBlend (colorb in)
{
	playerState->viewBlend[0] = ((float)in.R) / 255;
	playerState->viewBlend[1] = ((float)in.G) / 255;
	playerState->viewBlend[2] = ((float)in.B) / 255;
	playerState->viewBlend[3] = ((float)in.A) / 255;
}

float			CPlayerState::GetFov ()
{
	return playerState->fov;
}
void			CPlayerState::SetFov (float value)
{
	playerState->fov = value;
}

int				CPlayerState::GetRdFlags ()
{
	return playerState->rdFlags;
}
void			CPlayerState::SetRdFlags (int value)
{
	playerState->rdFlags = value;
}

int16			CPlayerState::GetStat (uint8 index)
{
	assert (!(index < 0 || index > 32));

	return playerState->stats[index];
}
void			CPlayerState::SetStat (uint8 index, int16 val)
{
	assert (!(index < 0 || index > 32));

	playerState->stats[index] = val;
}

void CPlayerState::CopyStats (int16 *Stats)
{
	memcpy (playerState->stats, Stats, sizeof(playerState->stats));
}

int16 *CPlayerState::GetStats ()
{
	return playerState->stats;
}

void			CPlayerState::Clear ()
{
	memset (playerState, 0, sizeof(&playerState));
}

CClient::CClient (gclient_t *client) :
client(client),
PlayerState(&client->playerState)
{
};

int CClient::GetPing ()
{
	return client->ping;
}

void CClient::SetPing (int ping)
{
	client->ping = ping;
}

void CClient::Clear ()
{
	memset (client, 0, sizeof(*client));

	gclient_t *client = this->client;
	memset (this, 0, sizeof(*this));
	this->client = client;
	PlayerState = CPlayerState(&client->playerState);
}

// Players have a special way of allocating the entity.
// We won't automatically allocate it since it already exists
CPlayerEntity::CPlayerEntity (int Index) :
CBaseEntity(Index),
CHurtableEntity(Index),
CPhysicsEntity(Index),
Client(&game.clients[State.GetNumber()-1]),
NoClip(false),
TossPhysics(false)
{
	EntityFlags |= ENT_PLAYER;
	PhysicsType = PHYSICS_WALK;
};

bool CPlayerEntity::Run ()
{
	BeginServerFrame ();
	return true;
};

/*
==============
BeginServerFrame

This will be called for all players
==============
*/
void CPlayerEntity::BeginServerFrame ()
{
	if (level.intermissiontime)
		return;

	if ((game.mode & GAME_DEATHMATCH) && 
#ifdef CLEANCTF_ENABLED
		!(game.mode & GAME_CTF) &&
#endif
		Client.pers.spectator != Client.resp.spectator &&
		(level.framenum - Client.respawn_time) >= 50)
	{
		SpectatorRespawn();
		return;
	}

	// run weapon animations
	if (!Client.resp.spectator && Client.pers.Weapon)
	{
#ifdef CLEANCTF_ENABLED
		if (!(game.mode & GAME_CTF) || ((game.mode & GAME_CTF) && !NoClip))
#endif
		Client.pers.Weapon->Think (this);
	}

	if (DeadFlag)
	{
		// wait for any button just going down
		if ( level.framenum > Client.respawn_time)
		{
			int buttonMask;
			// in deathmatch, only wait for attack button
			if (game.mode & GAME_DEATHMATCH)
				buttonMask = BUTTON_ATTACK;
			else
				buttonMask = -1;

			if ( ( Client.latched_buttons & buttonMask ) ||
				((game.mode & GAME_DEATHMATCH) && dmFlags.dfForceRespawn ) )
			{
				Respawn();
				Client.latched_buttons = 0;
			}
		}
		return;
	}

	Client.latched_buttons = 0;
};

void CPlayerEntity::Respawn ()
{
	if (game.mode != GAME_SINGLEPLAYER)
	{
		// spectator's don't leave bodies
		if (!NoClip)
			CopyToBodyQueue (this);
		SetSvFlags (GetSvFlags() & ~SVF_NOCLIENT);
		PutInServer ();

		// add a teleportation effect
		State.SetEvent (EV_PLAYER_TELEPORT);

		// hold in place briefly
		Client.PlayerState.GetPMove()->pmFlags = PMF_TIME_TELEPORT;
		Client.PlayerState.GetPMove()->pmTime = 14;

		Client.respawn_time = level.framenum;
		return;
	}

	// restart the entire server
	gi.AddCommandString ("menu_loadgame\n");
}

/* 
 * only called when pers.spectator changes
 * note that resp.spectator should be the opposite of pers.spectator here
 */
void CPlayerEntity::SpectatorRespawn ()
{
	// if the user wants to become a spectator, make sure he doesn't
	// exceed max_spectators

	if (Client.pers.spectator)
	{
		char *value = Info_ValueForKey (Client.pers.userinfo, "spectator");
		if (*spectator_password->String() && 
			strcmp(spectator_password->String(), "none") && 
			strcmp(spectator_password->String(), value))
		{
			PrintToClient (PRINT_HIGH, "Spectator password incorrect.\n");
			Client.pers.spectator = false;
			WriteByte (SVC_STUFFTEXT);
			WriteString ("spectator 0\n");
			Cast(CASTFLAG_RELIABLE, gameEntity);
			return;
		}

		// count spectators
		int numspec = 0;
		for (int i = 1; i <= game.maxclients; i++)
		{
			CPlayerEntity *Player = dynamic_cast<CPlayerEntity*>(g_edicts[i].Entity);
			if (Player->IsInUse() && Player->Client.pers.spectator)
				numspec++;
		}

		if (numspec >= game.maxspectators)
		{
			PrintToClient (PRINT_HIGH, "Server spectator limit is full.");
			Client.pers.spectator = false;
			// reset his spectator var
			WriteByte (SVC_STUFFTEXT);
			WriteString ("spectator 0\n");
			Cast(CASTFLAG_RELIABLE, gameEntity);
			return;
		}
	}
	else
	{
		// he was a spectator and wants to join the game
		// he must have the right password
		char *value = Info_ValueForKey (Client.pers.userinfo, "password");
		if (*password->String() && strcmp(password->String(), "none") && 
			strcmp(password->String(), value))
		{
			PrintToClient (PRINT_HIGH, "Password incorrect.\n");
			Client.pers.spectator = true;
			WriteByte (SVC_STUFFTEXT);
			WriteString ("spectator 1\n");
			Cast(CASTFLAG_RELIABLE, gameEntity);
			return;
		}
	}

	// clear client on respawn
	Client.resp.score = Client.pers.score = 0;

	SetSvFlags (GetSvFlags() & ~SVF_NOCLIENT);
	PutInServer ();

	// add a teleportation effect
	if (!Client.pers.spectator)
	{
		// send effect
		CTempEnt::MuzzleFlash (State.GetOrigin(), State.GetNumber(), MZ_LOGIN);

		// hold in place briefly
		Client.PlayerState.GetPMove()->pmFlags = PMF_TIME_TELEPORT;
		Client.PlayerState.GetPMove()->pmTime = 14;
	}

	Client.respawn_time = level.framenum;

	if (Client.pers.spectator) 
		BroadcastPrintf (PRINT_HIGH, "%s has moved to the sidelines\n", Client.pers.netname);
	else
		BroadcastPrintf (PRINT_HIGH, "%s joined the game\n", Client.pers.netname);
}

/*
===========
PutInServer

Called when a player connects to a server or respawns in
a deathmatch.
============
*/
void CPlayerEntity::PutInServer ()
{
	vec3f					mins (-16, -16, -2);
	vec3f					maxs (16, 16, 32);
	int						index;
	vec3f					spawn_origin, spawn_angles;
	int						i;
	clientPersistent_t		saved;
	clientRespawn_t			resp;

	// find a spawn point
	// do it before setting health back up, so farthest
	// ranging doesn't count this client
	SelectSpawnPoint (spawn_origin, spawn_angles);

	index = State.GetNumber()-1;

	char		userinfo[MAX_INFO_STRING];
	switch (game.mode)
	{
	// deathmatch wipes most client data every spawn
	default:
			resp = Client.resp;
			memcpy (userinfo, Client.pers.userinfo, sizeof(userinfo));
			InitPersistent ();
			UserinfoChanged (userinfo);
		break;
	case GAME_COOPERATIVE:
			resp = Client.resp;
			memcpy (userinfo, Client.pers.userinfo, sizeof(userinfo));

			resp.coop_respawn.game_helpchanged = Client.pers.game_helpchanged;
			resp.coop_respawn.helpchanged = Client.pers.helpchanged;
			Client.pers = resp.coop_respawn;
			UserinfoChanged (userinfo);
			if (resp.score > Client.pers.score)
				Client.pers.score = resp.score;
		break;
	case GAME_SINGLEPLAYER:
			memset (&resp, 0, sizeof(resp));
		break;
	}

	// clear everything but the persistant data
	saved = Client.pers;
	Client.Clear ();
	Client.pers = saved;
	if (Client.pers.health <= 0)
		InitPersistent();
	Client.resp = resp;
	Client.pers.state = SVCS_SPAWNED;

	// copy some data from the client to the entity
	FetchEntData ();

	// clear entity values
	GroundEntity = NULL;
	CanTakeDamage = true;
	NoClip = false;
	TossPhysics = false;
	gameEntity->viewheight = 22;
	SetInUse (true);
	gameEntity->classname = "player";
	Mass = 200;
	SetSolid (SOLID_BBOX);
	DeadFlag = false;
	AirFinished = level.framenum + 120;
	SetClipmask (CONTENTS_MASK_PLAYERSOLID);
	gameEntity->model = "players/male/tris.md2";
	gameEntity->waterlevel = 0;
	gameEntity->watertype = 0;
	Flags &= ~FL_NO_KNOCKBACK;
	SetSvFlags (GetSvFlags() & ~SVF_DEADMONSTER);
	if (!Client.resp.MenuState.ent)
		Client.resp.MenuState = CMenuState(this);

	SetMins (mins);
	SetMaxs (maxs);
	Velocity.Clear ();

	// clear playerstate values
	Client.PlayerState.Clear ();

	Client.PlayerState.GetPMove()->origin[0] = spawn_origin.X*8;
	Client.PlayerState.GetPMove()->origin[1] = spawn_origin.Y*8;
	Client.PlayerState.GetPMove()->origin[2] = spawn_origin.Z*8;
//ZOID
	Client.PlayerState.GetPMove()->pmFlags &= ~PMF_NO_PREDICTION;
//ZOID

	if ((game.mode & GAME_DEATHMATCH) && dmFlags.dfFixedFov)
		Client.PlayerState.SetFov (90);
	else
	{
		float fov = atof(Info_ValueForKey(Client.pers.userinfo, "fov"));
		if (fov < 1)
			fov = 90;
		else if (fov > 160)
			fov = 160;

		Client.PlayerState.SetFov (fov);
	}

	//client->playerState.gunIndex = ModelIndex(client->pers.weapon->view_model);

	// clear entity state values
	State.SetEffects (0);
	State.SetModelIndex (255);		// will use the skin specified model
	State.SetModelIndex (255, 2);		// custom gun model
	// sknum is player num and weapon number
	// weapon number will be added in changeweapon
	State.SetSkinNum (State.GetNumber()-1);

	State.SetFrame (0);

	State.SetOrigin (spawn_origin + vec3f(0,0,1));
	State.SetOldOrigin (State.GetOrigin());

	// set the delta angle
	for (i=0 ; i<3 ; i++)
		Client.PlayerState.GetPMove()->deltaAngles[i] = ANGLE2SHORT(spawn_angles[i] - Client.resp.cmd_angles[i]);

	State.SetAngles (vec3f(0, spawn_angles[YAW], 0));
	Client.PlayerState.SetViewAngles (State.GetAngles());
	Client.ViewAngle = State.GetAngles();

#ifdef CLEANCTF_ENABLED
//ZOID
	if ((game.mode & GAME_CTF) && CTFStart())
		return;
//ZOID
#endif

	// spawn a spectator
	if (Client.pers.spectator)
	{
		Client.chase_target = NULL;

		Client.resp.spectator = true;

		NoClip = true;
		SetSolid (SOLID_NOT);
		SetSvFlags (GetSvFlags() | SVF_NOCLIENT);
		Client.PlayerState.SetGunIndex (0);
		Link ();
		return;
	}
	else
		Client.resp.spectator = false;

	if (!KillBox (this))
	{	// could't spawn in?
	}

	Link ();

	// force the current weapon up
	Client.NewWeapon = Client.pers.Weapon;
	Client.pers.Weapon->ChangeWeapon(this);
}

/*
==============
InitClientPersistant

This is only called when the game first initializes in single player,
but is called after each death and level change in deathmatch
==============
*/
void CPlayerEntity::InitPersistent ()
{
	memset (&Client.pers, 0, sizeof(Client.pers));

	NItems::Blaster->Add(this, 1);
	Client.pers.Weapon = &WeaponBlaster;
	Client.pers.LastWeapon = Client.pers.Weapon;
	Client.pers.Inventory.SelectedItem =Client.pers.Weapon->Item->GetIndex();

#ifdef CLEANCTF_ENABLED
	if (game.mode & GAME_CTF)
		NItems::Grapple->Add(this, 1);
	Client.pers.Flag = NULL;
#endif

	Client.pers.Tech = NULL;
	Client.pers.Armor = NULL;

	Client.pers.health			= 100;
	Client.pers.max_health		= 100;

	InitItemMaxValues();
}

void CPlayerEntity::InitItemMaxValues ()
{
	Client.pers.maxAmmoValues[AMMOTAG_SHELLS] = 100;
	Client.pers.maxAmmoValues[AMMOTAG_BULLETS] = 200;
	Client.pers.maxAmmoValues[AMMOTAG_GRENADES] = 50;
	Client.pers.maxAmmoValues[AMMOTAG_ROCKETS] = 50;
	Client.pers.maxAmmoValues[AMMOTAG_CELLS] = 200;
	Client.pers.maxAmmoValues[AMMOTAG_SLUGS] = 50;
}

/*
===========
UserinfoChanged

called whenever the player updates a userinfo variable.

The game can override any of the settings in place
(forcing skins or names, etc) before copying it off.
============
*/
void CPlayerEntity::UserinfoChanged (char *userinfo)
{
	char	*s;
	int		playernum;

	// check for malformed or illegal info strings
	if (!Info_Validate(userinfo))
		Q_strncpyz (userinfo, "\\name\\badinfo\\skin\\male/grunt", MAX_INFO_STRING);

	// set name
	s = Info_ValueForKey (userinfo, "name");
	Q_strncpyz (Client.pers.netname, s, sizeof(Client.pers.netname)-1);

	// set spectator
	s = Info_ValueForKey (userinfo, "spectator");
	// spectators are only supported in deathmatch
	if ((game.mode & GAME_DEATHMATCH) && *s && strcmp(s, "0"))
		Client.pers.spectator = true;
	else
		Client.pers.spectator = false;

	// set skin
	s = Info_ValueForKey (userinfo, "skin");
	playernum = State.GetNumber()-1;

	// combine name and skin into a configstring
#ifdef CLEANCTF_ENABLED
//ZOID
	if (game.mode & GAME_CTF)
		CTFAssignSkin(s);
	else
//ZOID
#endif
		ConfigString (CS_PLAYERSKINS+playernum, Q_VarArgs ("%s\\%s", Client.pers.netname, s) );

	// fov
	if ((game.mode & GAME_DEATHMATCH) && dmFlags.dfFixedFov)
		Client.PlayerState.SetFov (90);
	else
	{
		float fov = atof(Info_ValueForKey(userinfo, "fov"));
		if (fov < 1)
			fov = 90;
		else if (fov > 160)
			fov = 160;

		Client.PlayerState.SetFov (fov);
	}

	// handedness
	s = Info_ValueForKey (userinfo, "hand");
	if (strlen(s))
		Client.pers.hand = atoi(s);

	// IP
	// Paril: removed. could be changed any time in-game!
	//s = Info_ValueForKey (userinfo, "ip");

	// Gender
	s = Info_ValueForKey (userinfo, "gender");
	if (strlen(s))
	{
		switch (s[0])
		{
		case 'm':
		case 'M':
			Client.resp.Gender = GenderMale;
			break;
		case 'f':
		case 'F':
			Client.resp.Gender = GenderFemale;
			break;
		default:
			Client.resp.Gender = GenderNeutral;
			break;
		}
	}
	else
		Client.resp.Gender = GenderMale;

	// MSG command
	s = Info_ValueForKey (userinfo, "msg");
	if (strlen (s))
		Client.resp.messageLevel = atoi (s);

	// save off the userinfo in case we want to check something later
	Q_strncpyz (Client.pers.userinfo, userinfo, sizeof(Client.pers.userinfo)-1);
}

#ifdef CLEANCTF_ENABLED
void CPlayerEntity::CTFAssignSkin(char *s)
{
	int playernum = State.GetNumber()-1;
	char *p;
	char t[64];

	Q_snprintfz(t, sizeof(t), "%s", s);

	if ((p = strrchr(t, '/')) != NULL)
		p[1] = 0;
	else
		Q_strncpyz(t, "male/", sizeof(t));

	switch (Client.resp.ctf_team)
	{
	case CTF_TEAM1:
		ConfigString (CS_PLAYERSKINS+playernum, Q_VarArgs("%s\\%s%s", 
			Client.pers.netname, t, CTF_TEAM1_SKIN) );
		break;
	case CTF_TEAM2:
		ConfigString (CS_PLAYERSKINS+playernum,
			Q_VarArgs("%s\\%s%s", Client.pers.netname, t, CTF_TEAM2_SKIN) );
		break;
	default:
		ConfigString (CS_PLAYERSKINS+playernum, 
			Q_VarArgs("%s\\%s", Client.pers.netname, s) );
		break;
	}
}

bool CPlayerEntity::CTFStart ()
{
	if (Client.resp.ctf_team != CTF_NOTEAM)
		return false;

	if ((!dmFlags.dfCtfForceJoin || ctfgame.match >= MATCH_SETUP))
	{
		// start as 'observer'
		Client.resp.ctf_team = CTF_NOTEAM;
		NoClip = true;
		SetSolid (SOLID_NOT);
		SetSvFlags (GetSvFlags() | SVF_NOCLIENT);
		Client.PlayerState.SetGunIndex (0);
		Link ();

		CTFOpenJoinMenu(this);
		return true;
	}
	return false;
}
#endif

void CPlayerEntity::FetchEntData ()
{
	Health = Client.pers.health;
	MaxHealth = Client.pers.max_health;
	Flags |= Client.pers.savedFlags;
	if (game.mode == GAME_COOPERATIVE)
		Client.resp.score = Client.pers.score;
}

// ==============================================
// Follows is the entire p_view.cpp file!

float	bobmove;
int		bobcycle;		// odd cycles are right foot going forward
float	bobfracsin;		// sin(bobfrac*M_PI)

/*
===============
CalcRoll
===============
*/
inline float CPlayerEntity::CalcRoll (vec3f &velocity, vec3f &right)
{
	float	side = Q_fabs(velocity.Dot (right));
	float	sign = side < 0 ? -1 : 1;

	if (side < sv_rollspeed->Float())
		side = side * sv_rollangle->Float() / sv_rollspeed->Float();
	else
		side = sv_rollangle->Float();
	
	return side*sign;
}

/*
===============
DamageFeedback
===============
*/
inline void CPlayerEntity::DamageFeedback (vec3f &forward, vec3f &right)
{
	static	const vec3f	PowerColor (0, 1, 0);
	static	const vec3f	ArmorColor (1, 1, 1);
	static	const vec3f	BloodColor (1, 0, 0);

	// flash the backgrounds behind the status numbers
	Client.PlayerState.SetStat(STAT_FLASHES, 0);
	if (Client.damage_blood)
		Client.PlayerState.SetStat (STAT_FLASHES, Client.PlayerState.GetStat(STAT_FLASHES) | 1);
	if (Client.damage_armor && !(Flags & FL_GODMODE) && (Client.invincible_framenum <= level.framenum))
		Client.PlayerState.SetStat (STAT_FLASHES, Client.PlayerState.GetStat(STAT_FLASHES) | 2);

	// total points of damage shot at the player this frame
	int count = (Client.damage_blood + Client.damage_armor + Client.damage_parmor);
	if (count == 0)
		return;		// didn't take any damage

	// start a pain animation if still in the player model
	if (Client.anim_priority < ANIM_PAIN && State.GetModelIndex() == 255)
	{
		Client.anim_priority = ANIM_PAIN;
		if (Client.PlayerState.GetPMove()->pmFlags & PMF_DUCKED)
		{
			State.SetFrame (FRAME_crpain1-1);
			Client.anim_end = FRAME_crpain4;
		}
		else
		{
			switch (irandom(3))
			{
			case 0:
				State.SetFrame (FRAME_pain101-1);
				Client.anim_end = FRAME_pain104;
				break;
			case 1:
				State.SetFrame (FRAME_pain201-1);
				Client.anim_end = FRAME_pain204;
				break;
			case 2:
				State.SetFrame (FRAME_pain301-1);
				Client.anim_end = FRAME_pain304;
				break;
			}
		}
	}

	int realcount = count;
	if (count < 10)
		count = 10;	// always make a visible effect

	// play an apropriate pain sound
	if ((level.framenum > gameEntity->pain_debounce_time) && !(Flags & FL_GODMODE) && (Client.invincible_framenum <= level.framenum))
	{
		gameEntity->pain_debounce_time = level.framenum + 7;

		int l = Clamp<int>(((floorf((Max<>(0, Health-1)) / 25))), 0, 3);
		PlaySound (CHAN_VOICE, gMedia.Player.Pain[l][(irandom(2))]);
	}

	// the total alpha of the blend is always proportional to count
/*	int Alpha = Client.DamageBlend.A + count*3;
	if (Alpha < 51)
		Alpha = 51;
	if (Alpha > 153)
		Alpha = 153;		// don't go too saturated

	// the color of the blend will vary based on how much was absorbed
	// by different armors
	Client.DamageBlend.Set (
							(BloodColor.R * ((float)Client.damage_blood/(float)realcount)) + (ArmorColor.R * ((float)Client.damage_armor/(float)realcount)) + (PowerColor.R * ((float)Client.damage_parmor/(float)realcount)),
							(BloodColor.G * ((float)Client.damage_blood/(float)realcount)) + (ArmorColor.G * ((float)Client.damage_armor/(float)realcount)) + (PowerColor.G * ((float)Client.damage_parmor/(float)realcount)),
							(BloodColor.B * ((float)Client.damage_blood/(float)realcount)) + (ArmorColor.B * ((float)Client.damage_armor/(float)realcount)) + (PowerColor.B * ((float)Client.damage_parmor/(float)realcount)),
							Alpha);*/
	// the total alpha of the blend is always proportional to count
	if (Client.DamageBlend.A < 0)
		Client.DamageBlend.A = 0;
	Client.DamageBlend.A += count*0.01f;
	if (Client.DamageBlend.A < 0.2f)
		Client.DamageBlend.A = 0.2f;
	if (Client.DamageBlend.A > 0.6f)
		Client.DamageBlend.A = 0.6f;		// don't go too saturated

	// the color of the blend will vary based on how much was absorbed
	// by different armors
	colorf NewColor (0, 0, 0, Client.DamageBlend.A);
	if (Client.damage_parmor)
		NewColor.G = NewColor.G + (float)Client.damage_parmor/realcount;
	if (Client.damage_armor)
	{
		NewColor.R = NewColor.R + (float)Client.damage_armor/realcount;
		NewColor.G = NewColor.G + (float)Client.damage_armor/realcount;
		NewColor.B = NewColor.B + (float)Client.damage_armor/realcount;
	}
	if (Client.damage_blood)
		NewColor.R = NewColor.R + (float)Client.damage_blood/realcount;

	Client.DamageBlend = NewColor;


	//
	// calculate view angle kicks
	//
	float kick = Q_fabs(Client.damage_knockback);
	if (kick && (Health > 0))	// kick of 0 means no view adjust at all
	{
		kick *= 100 / Health;

		if (kick < count*0.5)
			kick = count*0.5;
		if (kick > 50)
			kick = 50;

		vec3f v = (Client.DamageFrom - State.GetOrigin ()).GetNormalized();
		
		Client.ViewDamage.Y = kick*v.Dot (right)*0.3;
		Client.ViewDamage.X = kick*-v.Dot (forward)*0.3;
		Client.ViewDamageTime = level.framenum + DAMAGE_TIME;
	}

	//
	// clear totals
	//
	Client.damage_blood = Client.damage_armor = Client.damage_parmor = Client.damage_knockback = 0;
}

/*
===============
SV_CalcViewOffset
===============
*/
inline void CPlayerEntity::CalcViewOffset (vec3f &forward, vec3f &right, vec3f &up, float xyspeed)
{
	float		bob;
	float		ratio;
	vec3f		v (0, 0, 0);

	// if dead, fix the angle and don't add any kick
	if (DeadFlag)
	{
		Client.PlayerState.SetViewAngles (vec3f(-15, Client.KillerYaw, 40));
		Client.PlayerState.SetKickAngles (vec3Origin);
	}
	else
	{
		// Base angles
		vec3f angles = Client.KickAngles;

		// add angles based on damage kick
		ratio = (float)(Client.ViewDamageTime - level.framenum) / DAMAGE_TIME;
		if (ratio < 0)
		{
			ratio = 0;
			Client.ViewDamage.Clear ();
		}
		angles.X += ratio * Client.ViewDamage.X;
		angles.Z += ratio * Client.ViewDamage.Y;

		// add pitch based on fall kick
		ratio = (float)(Client.fall_time - level.framenum) / FALL_TIME;
		if (ratio < 0)
			ratio = 0;
		angles.X += ratio * Client.fall_value;

		// add angles based on velocity
		float delta = Velocity.Dot (forward);
		angles.X += delta*run_pitch->Float();
		
		delta = Velocity.Dot (right);
		angles.Z += delta*run_roll->Float();

		// add angles based on bob

		delta = bobfracsin * bob_pitch->Float() * xyspeed;
		if (Client.PlayerState.GetPMove()->pmFlags & PMF_DUCKED)
			delta *= 6;		// crouching
		angles.X += delta;
		delta = bobfracsin * bob_roll->Float() * xyspeed;
		if (Client.PlayerState.GetPMove()->pmFlags & PMF_DUCKED)
			delta *= 6;		// crouching
		if (bobcycle & 1)
			delta = -delta;
		angles.Z += delta;

		Client.PlayerState.SetKickAngles(angles);
	}

//===================================

	// add view height
	v.Z += gameEntity->viewheight;

	// add fall height
	ratio = (float)(Client.fall_time - level.framenum) / FALL_TIME;
	if (ratio < 0)
		ratio = 0;
	v.Z -= ratio * Client.fall_value * 0.4;

	// add bob height
	bob = bobfracsin * xyspeed * bob_up->Float();
	if (bob > 6)
		bob = 6;
	v.Z += bob;

	// add kick offset
	v += Client.KickOrigin;

	// absolutely bound offsets
	// so the view can never be outside the player box
	if (v.X < -14)
		v.X = -14;
	else if (v.X > 14)
		v.X = 14;
	if (v.Y < -14)
		v.Y = -14;
	else if (v.Y > 14)
		v.Y = 14;
	if (v.Z < -22)
		v.Z = -22;
	else if (v.Z > 30)
		v.Z = 30;

	Client.PlayerState.SetViewOffset(v);
}

/*
==============
SV_CalcGunOffset
==============
*/
inline void CPlayerEntity::CalcGunOffset (vec3f &forward, vec3f &right, vec3f &up, float xyspeed)
{
	// gun angles from bobbing
	vec3f gunAngles = Client.PlayerState.GetGunAngles ();
	vec3f angles	(	(bobcycle & 1) ? (-gunAngles[ROLL]) : (xyspeed * bobfracsin * 0.005), 
						(bobcycle & 1) ? (-gunAngles[YAW]) : (xyspeed * bobfracsin * 0.01),
						xyspeed * bobfracsin * 0.005
					);

	// gun angles from delta movement
	for (int i = 0; i < 3; i++)
	{
		float delta = Client.OldViewAngles[i] - angles[i];
		if (delta > 180)
			delta -= 360;
		if (delta < -180)
			delta += 360;
		if (delta > 45)
			delta = 45;
		if (delta < -45)
			delta = -45;
		if (i == YAW)
			angles[ROLL] += 0.1*delta;
		angles[i] += 0.2 * delta;
	}

	// copy it out
	Client.PlayerState.SetGunAngles (angles);

	// gun height
	Client.PlayerState.SetGunOffset (vec3Origin);

	angles.Clear ();
	// gun_x / gun_y / gun_z are development tools
	for (int i = 0; i < 3; i++)
	{
		angles[i] += forward[i] * gun_y->Float();
		angles[i] += right[i] * gun_x->Float();
		angles[i] += up[i] * -gun_z->Float();
	}

	Client.PlayerState.SetGunAngles (angles);
}


/*
=============
SV_AddBlend
=============
*/
// Not a part of CPlayerEntity
static inline void SV_AddBlend (colorf color, colorf &v_blend)
{
/*	if (color.A <= 0)
		return;

	byte a2 = v_blend.A  + color.A;	// new total alpha
	float a3 = (float)(v_blend.A) / (float)(a2);		// fraction of color from old

	v_blend.R = (float)(v_blend.R)*a3 + color.R*(1-a3);
	v_blend.G = (float)(v_blend.G)*a3 + color.G*(1-a3);
	v_blend.B = (float)(v_blend.B)*a3 + color.B*(1-a3);
	v_blend.A = a2;*/
	if (color.A <= 0)
		return;
	float a2 = v_blend.A + (1-v_blend.A)*color.A;	// new total alpha
	float a3 = v_blend.A/a2;		// fraction of color from old

	v_blend.R = v_blend.R*a3 + color.R*(1-a3);
	v_blend.G = v_blend.G*a3 + color.G*(1-a3);
	v_blend.B = v_blend.B*a3 + color.B*(1-a3);
	v_blend.A = a2;
}


/*
=============
SV_CalcBlend
=============
*/
static const colorf LavaColor (1.0f, 0.3f, 0.0f, 0.6f);
static const colorf SlimeColor (0.0f, 0.1f, 0.05f, 0.6f);
static const colorf WaterColor (0.5f, 0.3f, 0.2f, 0.4f);
static const colorf QuadColor (0, 0, 1, 0.08f);
static const colorf InvulColor (1, 1, 0, 0.08f);
static const colorf EnviroColor (0, 1, 0, 0.08f);
static const colorf BreatherColor (0.4f, 1, 0.4f, 0.04f);
static const colorf ClearColor (0,0,0,0);
static colorf BonusColor (0.85f, 0.7f, 0.3f, 0);

inline void CPlayerEntity::CalcBlend ()
{
	// add for contents
	vec3f	vieworg = State.GetOrigin() + Client.PlayerState.GetViewOffset();
	int contents = PointContents (vieworg);

	if (contents & (CONTENTS_LAVA|CONTENTS_SLIME|CONTENTS_WATER) )
		Client.PlayerState.SetRdFlags (Client.PlayerState.GetRdFlags() | RDF_UNDERWATER);
	else
		Client.PlayerState.SetRdFlags (Client.PlayerState.GetRdFlags() & ~RDF_UNDERWATER);

	// add for powerups
	if (Client.quad_framenum > level.framenum)
	{
		int remaining = Client.quad_framenum - level.framenum;

		if (remaining == 30)	// beginning to fade
			PlaySound (CHAN_ITEM, SoundIndex("items/damage2.wav"));

		if (remaining > 30 || (remaining & 4) )
			SV_AddBlend (QuadColor, Client.pers.viewBlend);
	}
	else if (Client.invincible_framenum > level.framenum)
	{
		int remaining = Client.invincible_framenum - level.framenum;

		if (remaining == 30)	// beginning to fade
			PlaySound (CHAN_ITEM, SoundIndex("items/protect2.wav"));

		if (remaining > 30 || (remaining & 4) )
			SV_AddBlend (InvulColor, Client.pers.viewBlend);
	}
	else if (Client.enviro_framenum > level.framenum)
	{
		int remaining = Client.enviro_framenum - level.framenum;

		if (remaining == 30)	// beginning to fade
			PlaySound (CHAN_ITEM, SoundIndex("items/airout.wav"));

		if (remaining > 30 || (remaining & 4) )
			SV_AddBlend (EnviroColor, Client.pers.viewBlend);
	}
	else if (Client.breather_framenum > level.framenum)
	{
		int remaining = Client.breather_framenum - level.framenum;

		if (remaining == 30)	// beginning to fade
			PlaySound (CHAN_ITEM, SoundIndex("items/airout.wav"));

		if (remaining > 30 || (remaining & 4) )
			SV_AddBlend (BreatherColor, Client.pers.viewBlend);
	}

	// add for damage
	if (Client.DamageBlend.A)
		SV_AddBlend (Client.DamageBlend, Client.pers.viewBlend);

	// drop the damage value
	if (Client.DamageBlend.A)
	{
		Client.DamageBlend.A -= 0.06f;
		if (Client.DamageBlend.A < 0)
			Client.DamageBlend.A = 0;
	}

	// add bonus and drop the value
	if (Client.bonus_alpha)
	{
		BonusColor.A = Client.bonus_alpha;
		SV_AddBlend (BonusColor, Client.pers.viewBlend);

		Client.bonus_alpha -= 0.1f;
		if (Client.bonus_alpha < 0)
			Client.bonus_alpha = 0;
	}

	if (contents & (CONTENTS_LAVA))
		SV_AddBlend (LavaColor, Client.pers.viewBlend);
	else if (contents & CONTENTS_SLIME)
		SV_AddBlend (SlimeColor, Client.pers.viewBlend);
	else if (contents & CONTENTS_WATER)
		SV_AddBlend (WaterColor, Client.pers.viewBlend);

	Client.PlayerState.SetViewBlend (Client.pers.viewBlend);
	Client.pers.viewBlend.Set(ClearColor);
}


/*
=================
P_FallingDamage
=================
*/
inline void CPlayerEntity::FallingDamage ()
{
	if (dmFlags.dfNoFallingDamage)
		return;

	if (State.GetModelIndex() != 255)
		return;		// not in the player model

	if (NoClip)
		return;

#ifdef CLEANCTF_ENABLED
//ZOID
	// never take damage if just release grapple or on grapple
	if (level.framenum - Client.ctf_grapplereleasetime <= 2 ||
		(Client.ctf_grapple && 
		Client.ctf_grapplestate > CTF_GRAPPLE_STATE_FLY))
		return;
//ZOID
#endif
	float	delta;

	if ((Client.OldVelocity.Z < 0) && (Velocity.Z > Client.OldVelocity.Z) && (!GroundEntity))
		delta = Client.OldVelocity.Z;
	else
	{
		if (!GroundEntity)
			return;
		delta = Velocity.Z - Client.OldVelocity.Z;
	}
	delta = delta*delta * 0.0001;

	// never take falling damage if completely underwater
	if (gameEntity->waterlevel == 3)
		return;
	if (gameEntity->waterlevel == 2)
		delta *= 0.25;
	if (gameEntity->waterlevel == 1)
		delta *= 0.5;

	if (delta < 1)
		return;

	if (delta < 15)
	{
		State.SetEvent (EV_FOOTSTEP);
		return;
	}

	Client.fall_value = delta*0.5;
	if (Client.fall_value > 40)
		Client.fall_value = 40;
	Client.fall_time = level.framenum + FALL_TIME;

	if (delta > 30)
	{
		if (Health > 0)
		{
			if (delta >= 55)
				State.SetEvent (EV_FALLFAR);
			else
				State.SetEvent (EV_FALL);
		}
		gameEntity->pain_debounce_time = level.framenum;	// no normal pain sound
		int damage = (delta-30)/2;
		if (damage < 1)
			damage = 1;

		static vec3f dir (0, 0, 1);

		if (!dmFlags.dfNoFallingDamage )
			TakeDamage (World, World, dir, State.GetOrigin(), vec3Origin, damage, 0, 0, MOD_FALLING);
	}
	else
	{
		State.SetEvent (EV_FALLSHORT);
		return;
	}
}

/*
=============
P_WorldEffects
=============
*/
inline void CPlayerEntity::WorldEffects ()
{
	bool	breather;
	bool	envirosuit;
	int		waterlevel, old_waterlevel;
	vec3f origin = State.GetOrigin();

	if (NoClip)
	{
		AirFinished = level.framenum + 120;	// don't need air
		return;
	}

	waterlevel = gameEntity->waterlevel;
	old_waterlevel = Client.old_waterlevel;
	Client.old_waterlevel = waterlevel;

	breather = (bool)(Client.breather_framenum > level.framenum);
	envirosuit = (bool)(Client.enviro_framenum > level.framenum);

	//
	// if just entered a water volume, play a sound
	//
	if (!old_waterlevel && waterlevel)
	{
		PlayerNoiseAt (origin, PNOISE_SELF);
		if (gameEntity->watertype & CONTENTS_LAVA)
			PlaySound (CHAN_BODY, SoundIndex("player/lava_in.wav"));
		else if (gameEntity->watertype & CONTENTS_SLIME)
			PlaySound (CHAN_BODY, SoundIndex("player/watr_in.wav"));
		else if (gameEntity->watertype & CONTENTS_WATER)
			PlaySound (CHAN_BODY, SoundIndex("player/watr_in.wav"));
		Flags |= FL_INWATER;

		// clear damage_debounce, so the pain sound will play immediately
		gameEntity->pain_debounce_time = level.framenum - 1;
	}

	//
	// if just completely exited a water volume, play a sound
	//
	if (old_waterlevel && ! waterlevel)
	{
		PlayerNoiseAt (origin, PNOISE_SELF);
		PlaySound (CHAN_BODY, SoundIndex("player/watr_out.wav"));
		Flags &= ~FL_INWATER;
	}

	//
	// check for head just going under water
	//
	if (old_waterlevel != 3 && waterlevel == 3)
		PlaySound (CHAN_BODY, SoundIndex("player/watr_un.wav"));

	//
	// check for head just coming out of water
	//
	if (old_waterlevel == 3 && waterlevel != 3)
	{
		if (AirFinished < level.framenum)
		{	// gasp for air
			PlaySound (CHAN_VOICE, SoundIndex("player/gasp1.wav"));
			PlayerNoiseAt (origin, PNOISE_SELF);
		}
		else  if (AirFinished < level.framenum + 110) // just break surface
			PlaySound (CHAN_VOICE, SoundIndex("player/gasp2.wav"));
	}

	//
	// check for drowning
	//
	if (waterlevel == 3)
	{
		// breather or envirosuit give air
		if (breather || envirosuit)
		{
			AirFinished = level.framenum + 100;

			if (((int)(Client.breather_framenum - level.framenum) % 25) == 0)
			{
				PlaySound (CHAN_AUTO, SoundIndex((!Client.breather_sound) ? "player/u_breath1.wav" : "player/u_breath2.wav"));
				Client.breather_sound = !Client.breather_sound;
				PlayerNoiseAt (origin, PNOISE_SELF);
			}
		}

		// if out of air, start drowning
		if (AirFinished < level.framenum)
		{	// drown!
			if (Client.next_drown_time < level.framenum 
				&& Health > 0)
			{
				Client.next_drown_time = level.framenum + 10;

				// take more damage the longer underwater
				gameEntity->dmg += 2;
				if (gameEntity->dmg > 15)
					gameEntity->dmg = 15;

				// play a gurp sound instead of a normal pain sound
				if (Health <= gameEntity->dmg)
					PlaySound (CHAN_VOICE, SoundIndex("player/drown1.wav"));
				else
					PlaySound (CHAN_VOICE, gMedia.Player.Gurp[(irandom(2))]);

				gameEntity->pain_debounce_time = level.framenum;

				TakeDamage (World, World, vec3fOrigin, origin, vec3fOrigin, gameEntity->dmg, 0, DAMAGE_NO_ARMOR, MOD_WATER);
			}
		}
	}
	else
	{
		AirFinished = level.framenum + 120;
		gameEntity->dmg = 2;
	}

	//
	// check for sizzle damage
	//
	if (waterlevel && (gameEntity->watertype&(CONTENTS_LAVA|CONTENTS_SLIME)) )
	{
		if (gameEntity->watertype & CONTENTS_LAVA)
		{
			if (Health > 0
				&& gameEntity->pain_debounce_time <= level.framenum
				&& Client.invincible_framenum < level.framenum)
			{
				PlaySound (CHAN_VOICE, SoundIndex((irandom(2)) ? "player/burn1.wav" : "player/burn2.wav"));
				gameEntity->pain_debounce_time = level.framenum + 10;
			}

			// take 1/3 damage with envirosuit
			TakeDamage (World, World, vec3fOrigin, origin, vec3fOrigin, (envirosuit) ? 1*waterlevel : 3*waterlevel, 0, 0, MOD_LAVA);
		}

		if (gameEntity->watertype & CONTENTS_SLIME)
		{
			if (!envirosuit) // no damage from slime with envirosuit
				TakeDamage (World, World, vec3fOrigin, origin, vec3fOrigin, 1*waterlevel, 0, 0, MOD_SLIME);
		}
	}
}

/*
===============
G_SetClientEffects
===============
*/
int CPlayerEntity::PowerArmorType ()
{
	if (!(Flags & FL_POWER_ARMOR))
		return POWER_ARMOR_NONE;
	else if (Client.pers.Inventory.Has(NItems::PowerShield) > 0)
		return POWER_ARMOR_SHIELD;
	else if (Client.pers.Inventory.Has(NItems::PowerScreen) > 0)
		return POWER_ARMOR_SCREEN;

	return POWER_ARMOR_NONE;
}

inline void CPlayerEntity::SetClientEffects ()
{
	State.SetEffects (0);
	State.SetRenderEffects (0);

	if (Health <= 0 || level.intermissiontime)
		return;

	if (gameEntity->powerarmor_time > level.framenum)
	{
		int pa_type = PowerArmorType ();
		if (pa_type == POWER_ARMOR_SCREEN)
			State.AddEffects (EF_POWERSCREEN);
		else if (pa_type == POWER_ARMOR_SHIELD)
		{
			State.AddEffects (EF_COLOR_SHELL);
			State.AddRenderEffects (RF_SHELL_GREEN);
		}
	}

#ifdef CLEANCTF_ENABLED
	if (game.mode & GAME_CTF)
	{
		State.RemoveEffects (EF_FLAG1 | EF_FLAG2);
		if (Client.pers.Flag)
		{
			if (Health > 0)
				State.AddEffects (Client.pers.Flag->EffectFlags);
			State.SetModelIndex (ModelIndex(Client.pers.Flag->WorldModel), 3);
		}
		else
			State.SetModelIndex (0, 3);
	}
#endif

	if (Client.quad_framenum > level.framenum)
	{
		int remaining = Client.quad_framenum - level.framenum;
		if (remaining > 30 || (remaining & 4) )
			State.AddEffects (EF_QUAD);
	}

	if (Client.invincible_framenum > level.framenum)
	{
		int remaining = Client.invincible_framenum - level.framenum;
		if (remaining > 30 || (remaining & 4) )
			State.AddEffects (EF_PENT);
	}

	// show cheaters!!!
	if (Flags & FL_GODMODE)
	{
		State.AddEffects (EF_COLOR_SHELL);
		State.AddRenderEffects (RF_SHELL_RED|RF_SHELL_GREEN|RF_SHELL_BLUE);
	}
}


/*
===============
G_SetClientEvent
===============
*/
inline void CPlayerEntity::SetClientEvent (float xyspeed)
{
	if (State.GetEvent())
		return;

	if ( GroundEntity && xyspeed > 225)
	{
		if ( (int)(Client.bobtime+bobmove) != bobcycle )
			State.SetEvent (EV_FOOTSTEP);
	}
}

/*
===============
G_SetClientSound
===============
*/
inline void CPlayerEntity::SetClientSound ()
{
	if (Client.pers.game_helpchanged != game.helpchanged)
	{
		Client.pers.game_helpchanged = game.helpchanged;
		Client.pers.helpchanged = 1;
	}

	// help beep (no more than three times)
	if (Client.pers.helpchanged && Client.pers.helpchanged <= 3 && !(level.framenum&63) )
	{
		Client.pers.helpchanged++;
		PlaySound (CHAN_VOICE, SoundIndex ("misc/pc_up.wav"), 1, ATTN_STATIC);
	}

	if (gameEntity->waterlevel && (gameEntity->watertype & (CONTENTS_LAVA|CONTENTS_SLIME)))
		State.SetSound (gMedia.FrySound());
	else if (Client.pers.Weapon && Client.pers.Weapon->WeaponSoundIndex)
		State.SetSound (Client.pers.Weapon->WeaponSoundIndex);
	else if (Client.weapon_sound)
		State.SetSound (Client.weapon_sound);
	else
		State.SetSound (0);
}

/*
===============
G_SetClientFrame
===============
*/
inline void CPlayerEntity::SetClientFrame (float xyspeed)
{
	bool		duck = (Client.PlayerState.GetPMove()->pmFlags & PMF_DUCKED);
	bool		run = !!(xyspeed);
	bool		isNewAnim = false;

	if (State.GetModelIndex() != 255)
		return;		// not in the player model

	// check for stand/duck and stop/go transitions
	if ((duck != Client.anim_duck && Client.anim_priority < ANIM_DEATH) ||
		(run != Client.anim_run && Client.anim_priority == ANIM_BASIC) ||
		(!GroundEntity && Client.anim_priority <= ANIM_WAVE))
		isNewAnim = true;

	if (!isNewAnim)
	{
		if(Client.anim_priority == ANIM_REVERSE)
		{
			if(State.GetFrame() > Client.anim_end)
			{
				State.SetFrame (State.GetFrame() - 1);
				return;
			}
		}
		else if (State.GetFrame() < Client.anim_end)
		{	// continue an animation
			State.SetFrame (State.GetFrame() + 1);
			return;
		}

		if (Client.anim_priority == ANIM_DEATH)
			return;		// stay there
		if (Client.anim_priority == ANIM_JUMP)
		{
			if (!GroundEntity)
				return;		// stay there
			Client.anim_priority = ANIM_WAVE;
			State.SetFrame (FRAME_jump3);
			Client.anim_end = FRAME_jump6;
			return;
		}
	}

	// return to either a running or standing frame
	Client.anim_priority = ANIM_BASIC;
	Client.anim_duck = duck;
	Client.anim_run = run;

	if (!GroundEntity)
	{
#ifdef CLEANCTF_ENABLED
//ZOID: if on grapple, don't go into jump frame, go into standing
//frame
		if (Client.ctf_grapple)
		{
			State.SetFrame (FRAME_stand01);
			Client.anim_end = FRAME_stand40;
		}
		else
		{
//ZOID
#endif
		Client.anim_priority = ANIM_JUMP;
		if (State.GetFrame() != FRAME_jump2)
			State.SetFrame (FRAME_jump1);
		Client.anim_end = FRAME_jump2;
#ifdef CLEANCTF_ENABLED
	}
#endif
	}
	else if (run)
	{	// running
		if (duck)
		{
			State.SetFrame (FRAME_crwalk1);
			Client.anim_end = FRAME_crwalk6;
		}
		else
		{
			State.SetFrame (FRAME_run1);
			Client.anim_end = FRAME_run6;
		}
	}
	else
	{	// standing
		if (duck)
		{
			State.SetFrame (FRAME_crstnd01);
			Client.anim_end = FRAME_crstnd19;
		}
		else
		{
			State.SetFrame (FRAME_stand01);
			Client.anim_end = FRAME_stand40;
		}
	}
}

/*
=================
ClientEndServerFrame

Called for each player at the end of the server frame
and right after spawning
=================
*/
void CPlayerEntity::EndServerFrame ()
{
	float	bobtime;
	int		i;

	//
	// If the origin or velocity have changed since ClientThink(),
	// update the pmove values.  This will happen when the client
	// is pushed by a bmodel or kicked by an explosion.
	// 
	// If it wasn't updated here, the view position would lag a frame
	// behind the body position when pushed -- "sinking into plats"
	//
	vec3f origin = State.GetOrigin();
	for (i=0 ; i<3 ; i++)
	{
		Client.PlayerState.GetPMove()->origin[i] = origin[i]*8.0;
		Client.PlayerState.GetPMove()->velocity[i] = Velocity[i]*8.0;
	}

	//
	// If the end of unit layout is displayed, don't give
	// the player any normal movement attributes
	//
	if (level.intermissiontime)
	{
		// FIXME: add view drifting here?
		vec4_t blend;
		Client.PlayerState.GetViewBlend (blend);

		blend[3] = 0;
		Client.PlayerState.SetViewBlend (blend);

		Client.PlayerState.SetFov (90);
		SetStats ();
		return;
	}

	vec3f forward, right, up;
	Client.ViewAngle.ToVectors (&forward, &right, &up);

	// burn from lava, etc
	WorldEffects ();

	//
	// set model angles from view angles so other things in
	// the world can tell which direction you are looking
	//
	State.SetAngles (vec3f(
		(Client.ViewAngle.X > 180) ? (-360 + Client.ViewAngle.X)/3 : Client.ViewAngle.X/3,
		Client.ViewAngle.Y,
		CalcRoll (Velocity, right)*4));

	//
	// calculate speed and cycle to be used for
	// all cyclic walking effects
	//
	float xyspeed = sqrtf(Velocity.X*Velocity.X + Velocity.Y*Velocity.Y);

	if (xyspeed < 5 || Client.PlayerState.GetPMove()->pmFlags & PMF_DUCKED)
	{
		bobmove = 0;
		Client.bobtime = 0;	// start at beginning of cycle again
	}
	else if (GroundEntity)
	{
		// so bobbing only cycles when on ground
		if (xyspeed > 210)
			bobmove = 0.25;
		else if (xyspeed > 100)
			bobmove = 0.125;
		else
			bobmove = 0.0625;
	}
	
	bobtime = (Client.bobtime += bobmove);

	if (Client.PlayerState.GetPMove()->pmFlags & PMF_DUCKED)
		bobtime *= 4;

	bobcycle = (int)bobtime;
	bobfracsin = Q_fabs(sinf(bobtime*M_PI));

	// detect hitting the floor
	FallingDamage ();

	// apply all the damage taken this frame
	DamageFeedback (forward, right);

	// determine the view offsets
	CalcViewOffset (forward, right, up, xyspeed);

	// determine the gun offsets
	CalcGunOffset (forward, right, up, xyspeed);

	// determine the full screen color blend
	// must be after viewoffset, so eye contents can be
	// accurately determined
	CalcBlend ();

	// chase cam stuff
	if (Client.resp.spectator)
		SetSpectatorStats();
	else
#ifdef CLEANCTF_ENABLED
//ZOID
	if (!Client.chase_target)
//ZOID
#endif
		SetStats ();

#ifdef CLEANCTF_ENABLED
//ZOID
//update chasecam follower stats
	for (i = 1; i <= game.maxclients; i++)
	{
		CPlayerEntity *e = dynamic_cast<CPlayerEntity*>(g_edicts[i].Entity);
		if (!e->IsInUse() || e->Client.chase_target != this)
			continue;

		e->Client.PlayerState.CopyStats (Client.PlayerState.GetStats());
		e->Client.PlayerState.SetStat(STAT_LAYOUTS, 1);

		// Paril: was this break a mistake on Zoid's part?
		//break;
	}
//ZOID
#endif

	SetClientEvent (xyspeed);

	SetClientEffects ();

	SetClientSound ();

	SetClientFrame (xyspeed);

	Client.OldVelocity = Velocity;
	Client.OldViewAngles = Client.PlayerState.GetViewAngles();

	// clear weapon kicks
	Client.KickOrigin.Clear();
	Client.KickAngles.Clear();

	if (((game.mode & GAME_CTF) || dmFlags.dfDmTechs) && (Client.pers.Tech && (Client.pers.Tech->TechType == CTech::TechPassive)))
		Client.pers.Tech->DoPassiveTech (this);

	// if the scoreboard is up, update it
	if (Client.showscores && !(level.framenum & 31) )
		DeathmatchScoreboardMessage (false);
	else if (Client.resp.MenuState.InMenu && !(level.framenum & 4))
		Client.resp.MenuState.CurrentMenu->Draw (false);
}

#ifdef CLEANCTF_ENABLED
/*
==================
CTFScoreboardMessage
==================
*/
void CPlayerEntity::CTFScoreboardMessage (bool reliable)
{
	CStatusBar			Bar;
	char				entry[1024];
	size_t				len;
	int					sorted[2][MAX_CS_CLIENTS];
	int					sortedscores[2][MAX_CS_CLIENTS];
	int					score, total[2], totalscore[2];
	int					last[2];
	int					n, j, k;
	char				str[MAX_COMPRINT/4];
	int					team;

	// sort the clients by team and score
	total[0] = total[1] = 0;
	last[0] = last[1] = 0;
	totalscore[0] = totalscore[1] = 0;
	for (int i=0 ; i<game.maxclients ; i++)
	{
		CPlayerEntity *cl_ent = dynamic_cast<CPlayerEntity*>((g_edicts + 1 + i)->Entity);
		if (!cl_ent->IsInUse())
			continue;
		if (cl_ent->Client.resp.ctf_team == CTF_TEAM1)
			team = 0;
		else if (cl_ent->Client.resp.ctf_team == CTF_TEAM2)
			team = 1;
		else
			continue; // unknown team?

		score = cl_ent->Client.resp.score;
		for (j=0 ; j<total[team] ; j++)
		{
			if (score > sortedscores[team][j])
				break;
		}
		for (k=total[team] ; k>j ; k--)
		{
			sorted[team][k] = sorted[team][k-1];
			sortedscores[team][k] = sortedscores[team][k-1];
		}
		sorted[team][j] = i;
		sortedscores[team][j] = score;
		totalscore[team] += score;
		total[team]++;
	}

	// print level name and exit rules
	// add the clients in sorted order
	len = 0;

	// team one
	// Team 1
	Bar.AddIf (STAT_CTF_TEAM1_HEADER);
		Bar.AddVirtualPoint_X (8);
		Bar.AddVirtualPoint_Y (8);
		Bar.AddPicStat (STAT_CTF_TEAM1_HEADER);
	Bar.AddEndIf ();
	Bar.AddVirtualPoint_X (40);
	Bar.AddVirtualPoint_Y (28);
	Q_snprintfz (str, sizeof(str), "%4d/%-3d", totalscore[0], total[0]);
	Bar.AddString (str, false, false);
	Bar.AddVirtualPoint_X (98);
	Bar.AddVirtualPoint_Y (12);
	Bar.AddNumStat (STAT_CTF_TEAM1_CAPS, 2);

	// Team2
	Bar.AddIf (STAT_CTF_TEAM2_HEADER);
		Bar.AddVirtualPoint_X (168);
		Bar.AddVirtualPoint_Y (8);
		Bar.AddPicStat (STAT_CTF_TEAM2_HEADER);
	Bar.AddEndIf ();
	Bar.AddVirtualPoint_X (200);
	Bar.AddVirtualPoint_Y (28);
	Q_snprintfz (str, sizeof(str), "%4d/%-3d", totalscore[1], total[1]);
	Bar.AddString (str, false, false);
	Bar.AddVirtualPoint_X (256);
	Bar.AddVirtualPoint_Y (12);
	Bar.AddNumStat (STAT_CTF_TEAM2_CAPS, 2);

	len = Bar.Length ();

	for (int i=0 ; i<16 ; i++)
	{
		if (i >= total[0] && i >= total[1])
			break; // we're done

		*entry = 0;

		// left side
		if (i < total[0])
		{
			CPlayerEntity *cl_ent = dynamic_cast<CPlayerEntity*>((g_edicts + 1 + sorted[0][i])->Entity);

			Bar.AddClientBlock (0, 42 + i * 8, sorted[0][i], cl_ent->Client.resp.score, Clamp<int>(cl_ent->Client.GetPing(), 0, 999));

			if (cl_ent->Client.pers.Flag == NItems::BlueFlag)
			{
				Bar.AddVirtualPoint_X (56);
				Bar.AddVirtualPoint_Y (42 + i * 8);
				Bar.AddPic ("sbfctf2");
			}

			len = Bar.Length ();
			last[0] = i;
		}

		// right side
		if (i < total[1])
		{
			CPlayerEntity *cl_ent = dynamic_cast<CPlayerEntity*>((g_edicts + 1 + sorted[1][i])->Entity);

			Bar.AddClientBlock (160, 42 + i * 8, sorted[1][i], cl_ent->Client.resp.score, Clamp<int>(cl_ent->Client.GetPing(), 0, 999));

			if (cl_ent->Client.pers.Flag == NItems::RedFlag)
			{
				Bar.AddVirtualPoint_X (216);
				Bar.AddVirtualPoint_Y (42 + i * 8);
				Bar.AddPic ("sbfctf1");
			}

			len = Bar.Length ();
			last[1] = i;
		}
	}

	// put in spectators if we have enough room
	if (last[0] > last[1])
		j = last[0];
	else
		j = last[1];
	j = (j + 2) * 8 + 42;

	k = n = 0;
	if ((MAX_COMPRINT / 4) - len > 50)
	{
		for (int i = 0; i < game.maxclients; i++)
		{
			CPlayerEntity *cl_ent = dynamic_cast<CPlayerEntity*>((g_edicts + 1 + i)->Entity);
			if (!cl_ent->IsInUse() ||
				cl_ent->GetSolid() != SOLID_NOT ||
				cl_ent->Client.resp.ctf_team != CTF_NOTEAM)
				continue;

			if (!k)
			{
				k = 1;
				Bar.AddVirtualPoint_X (0);
				Bar.AddVirtualPoint_Y (j);
				Bar.AddString ("Spectators", true, false);
				len = Bar.Length();
				j += 8;
			}

			Bar.AddClientBlock ((n & 1) ? 160 : 0, j, i, cl_ent->Client.resp.score, Clamp<int>(cl_ent->Client.GetPing(), 0, 999));
			len = Bar.Length();
			
			if (n & 1)
				j += 8;
			n++;
		}
	}

	if (total[0] - last[0] > 1) // couldn't fit everyone
	{
		//Q_snprintfz(string + strlen(string), "xv 8 yv %d string \"..and %d more\" ",
		//	42 + (last[0]+1)*8, total[0] - last[0] - 1);
		Bar.AddVirtualPoint_X (8);
		Bar.AddVirtualPoint_Y (42 + (last[0]+1)*8);
		Q_snprintfz (str, sizeof(str), "..and %d more", total[0] - last[0] - 1);
		Bar.AddString (str, false, false);
	}
	if (total[1] - last[1] > 1) // couldn't fit everyone
	{
		Bar.AddVirtualPoint_X (168);
		Bar.AddVirtualPoint_Y (42 + (last[1]+1)*8);
		Q_snprintfz (str, sizeof(str), "..and %d more", total[1] - last[1] - 1);
		Bar.AddString (str, false, false);
	}

	Bar.SendMsg (gameEntity, reliable);
}
#endif

/*
==================
DeathmatchScoreboardMessage
==================
*/
void CPlayerEntity::DeathmatchScoreboardMessage (bool reliable)
{
#ifdef CLEANCTF_ENABLED
//ZOID
	if (game.mode & GAME_CTF)
	{
		CTFScoreboardMessage (reliable);
		return;
	}
//ZOID
#endif

	CStatusBar			Scoreboard;
	int					sorted[MAX_CS_CLIENTS];
	int					sortedscores[MAX_CS_CLIENTS];
	int					score, total;
	CPlayerEntity		*Killer = (Enemy) ? dynamic_cast<CPlayerEntity*>(Enemy) : NULL;

	// sort the clients by score
	total = 0;
	for (int i = 0; i < game.maxclients ; i++)
	{
		CPlayerEntity *cl_ent = dynamic_cast<CPlayerEntity*>((g_edicts + 1 + i)->Entity);
		if (!cl_ent->IsInUse() || cl_ent->Client.resp.spectator)
			continue;
		score = cl_ent->Client.resp.score;
		int j = 0;
		for ( ; j < total ; j++)
		{
			if (score > sortedscores[j])
				break;
		}
		int k = total;
		for ( ; k > j ; k--)
		{
			sorted[k] = sorted[k-1];
			sortedscores[k] = sortedscores[k-1];
		}
		sorted[j] = i;
		sortedscores[j] = score;
		total++;
	}

	// add the clients in sorted order
	if (total > 12)
		total = 12;

	for (int i = 0 ; i < total ; i++)
	{
		int		x, y;
		char	*tag;
		CPlayerEntity *cl_ent = dynamic_cast<CPlayerEntity*>((g_edicts + 1 + sorted[i])->Entity);

		x = (i>=6) ? 160 : 0;
		y = 32 + 32 * (i%6);

		// add a dogtag
		if (cl_ent == this)
			tag = "tag1";
		else if (cl_ent == Killer)
			tag = "tag2";
		else
			tag = NULL;

		if (tag)
		{
			Scoreboard.AddVirtualPoint_X (x+32);
			Scoreboard.AddVirtualPoint_Y (y);
			Scoreboard.AddPic (tag);
		}

		// send the layout
		Scoreboard.AddClientBlock (x, y, sorted[i], cl_ent->Client.resp.score, cl_ent->Client.GetPing(), (level.framenum - cl_ent->Client.resp.enterframe)/600);
	}

	Scoreboard.SendMsg (gameEntity, reliable);
}

void CPlayerEntity::SetStats ()
{
	//
	// health
	//
	Client.PlayerState.SetStat(STAT_HEALTH_ICON, gMedia.Hud.HealthPic);
	Client.PlayerState.SetStat(STAT_HEALTH, Health);

	//
	// ammo
	//
	if (Client.pers.Weapon)
	{
		if (Client.pers.Weapon->WeaponItem && Client.pers.Weapon->WeaponItem->Ammo)
		{
			Client.PlayerState.SetStat(STAT_AMMO_ICON, Client.pers.Weapon->WeaponItem->Ammo->IconIndex);
			Client.PlayerState.SetStat(STAT_AMMO, Client.pers.Inventory.Has(Client.pers.Weapon->WeaponItem->Ammo->GetIndex()));
		}
		else if (Client.pers.Weapon->Item && (Client.pers.Weapon->Item->Flags & ITEMFLAG_AMMO))
		{
			Client.PlayerState.SetStat(STAT_AMMO_ICON, Client.pers.Weapon->Item->IconIndex);
			Client.PlayerState.SetStat(STAT_AMMO, Client.pers.Inventory.Has(Client.pers.Weapon->Item->GetIndex()));
		}
		else
		{
			Client.PlayerState.SetStat(STAT_AMMO_ICON, 0);
			Client.PlayerState.SetStat(STAT_AMMO, 0);
		}
	}
	else
	{
		Client.PlayerState.SetStat(STAT_AMMO_ICON, 0);
		Client.PlayerState.SetStat(STAT_AMMO, 0);
	}
	
	//
	// armor
	//
	int			cells = 0;
	int			power_armor_type = PowerArmorType ();
	if (power_armor_type)
	{
		cells = Client.pers.Inventory.Has(NItems::Cells);
		if (cells == 0)
		{	// ran out of cells for power armor
			Flags &= ~FL_POWER_ARMOR;
			PlaySound (CHAN_ITEM, SoundIndex("misc/power2.wav"));
			power_armor_type = 0;
		}
	}

	CArmor *Armor = Client.pers.Armor;
	if (power_armor_type && (!Armor || (level.framenum & 8) ) )
	{	// flash between power armor and other armor icon
		Client.PlayerState.SetStat(STAT_ARMOR_ICON, gMedia.Hud.PowerShieldPic);
		Client.PlayerState.SetStat(STAT_ARMOR, cells);
	}
	else if (Armor)
	{
		Client.PlayerState.SetStat(STAT_ARMOR_ICON, Armor->IconIndex);
		Client.PlayerState.SetStat(STAT_ARMOR, Client.pers.Inventory.Has(Armor));
	}
	else
	{
		Client.PlayerState.SetStat(STAT_ARMOR_ICON, 0);
		Client.PlayerState.SetStat(STAT_ARMOR, 0);
	}

	//
	// pickup message
	//
	if (level.framenum > Client.pickup_msg_time)
	{
		Client.PlayerState.SetStat(STAT_PICKUP_ICON, 0);
		Client.PlayerState.SetStat(STAT_PICKUP_STRING, 0);
	}

	//
	// timers
	//
	if (Client.quad_framenum > level.framenum)
	{
		Client.PlayerState.SetStat(STAT_TIMER_ICON, gMedia.Hud.QuadPic);
		Client.PlayerState.SetStat(STAT_TIMER, (Client.quad_framenum - level.framenum)/10);
	}
	else if (Client.invincible_framenum > level.framenum)
	{
		Client.PlayerState.SetStat(STAT_TIMER_ICON, gMedia.Hud.InvulPic);
		Client.PlayerState.SetStat(STAT_TIMER, (Client.invincible_framenum - level.framenum)/10);
	}
	else if (Client.enviro_framenum > level.framenum)
	{
		Client.PlayerState.SetStat(STAT_TIMER_ICON, gMedia.Hud.EnviroPic);
		Client.PlayerState.SetStat(STAT_TIMER, (Client.enviro_framenum - level.framenum)/10);
	}
	else if (Client.breather_framenum > level.framenum)
	{
		Client.PlayerState.SetStat(STAT_TIMER_ICON, gMedia.Hud.RebreatherPic);
		Client.PlayerState.SetStat(STAT_TIMER, (Client.breather_framenum - level.framenum)/10);
	}
	// Paril, show silencer
	else if (Client.silencer_shots)
	{
		Client.PlayerState.SetStat(STAT_TIMER_ICON, gMedia.Hud.SilencerPic);
		Client.PlayerState.SetStat(STAT_TIMER, Client.silencer_shots);
	}
	// Paril
	else
	{
		Client.PlayerState.SetStat(STAT_TIMER_ICON, 0);
		Client.PlayerState.SetStat(STAT_TIMER, 0);
	}

	//
	// selected item
	//
	if (Client.pers.Inventory.SelectedItem == -1)
		Client.PlayerState.SetStat(STAT_SELECTED_ICON, 0);
	else
		Client.PlayerState.SetStat(STAT_SELECTED_ICON, GetItemByIndex(Client.pers.Inventory.SelectedItem)->IconIndex);

	Client.PlayerState.SetStat(STAT_SELECTED_ITEM, Client.pers.Inventory.SelectedItem);

	//
	// layouts
	//
	Client.PlayerState.SetStat(STAT_LAYOUTS, 0);

	if (Client.pers.health <= 0 || Client.resp.MenuState.InMenu ||
		(level.intermissiontime || Client.showscores) || 
		(!(game.mode & GAME_DEATHMATCH)) && Client.showhelp)
		Client.PlayerState.SetStat(STAT_LAYOUTS, Client.PlayerState.GetStat(STAT_LAYOUTS) | 1);
	if (Client.showinventory && Client.pers.health > 0)
		Client.PlayerState.SetStat(STAT_LAYOUTS, Client.PlayerState.GetStat(STAT_LAYOUTS) | 2);

	//
	// frags
	//
	Client.PlayerState.SetStat(STAT_FRAGS, Client.resp.score);

	//
	// help icon / current weapon if not shown
	//
	if (Client.pers.helpchanged && (level.framenum&8) )
		Client.PlayerState.SetStat(STAT_HELPICON, gMedia.Hud.HelpPic);

	else if ( (Client.pers.hand == CENTER_HANDED || Client.PlayerState.GetFov() > 91)
		&& Client.pers.Weapon)
		Client.PlayerState.SetStat(STAT_HELPICON, Client.pers.Weapon->Item->IconIndex);
	else
		Client.PlayerState.SetStat(STAT_HELPICON, 0);

	Client.PlayerState.SetStat(STAT_SPECTATOR, 0);

	Client.PlayerState.SetStat(STAT_TECH, 0);
	if (Client.pers.Tech)
		Client.PlayerState.SetStat(STAT_TECH, Client.pers.Tech->IconIndex);

#ifdef CLEANCTF_ENABLED
//ZOID
	if (game.mode & GAME_CTF)
		SetCTFStats();
//ZOID
#endif
}

/*
===============
G_SetSpectatorStats
===============
*/
void CPlayerEntity::SetSpectatorStats ()
{
	if (!Client.chase_target)
		SetStats ();

	Client.PlayerState.SetStat(STAT_SPECTATOR, 1);

	// layouts are independant in spectator
	Client.PlayerState.SetStat(STAT_LAYOUTS, 0);
	if (Client.pers.health <= 0 || level.intermissiontime || Client.showscores)
		Client.PlayerState.SetStat(STAT_LAYOUTS, Client.PlayerState.GetStat(STAT_LAYOUTS) | 1);
	if (Client.showinventory && Client.pers.health > 0)
		Client.PlayerState.SetStat(STAT_LAYOUTS, Client.PlayerState.GetStat(STAT_LAYOUTS) | 2);

	if (Client.chase_target && Client.chase_target->IsInUse())
		Client.PlayerState.SetStat(STAT_CHASE, (CS_PLAYERSKINS + 
			(Client.chase_target->State.GetNumber()- 1)));
	else
		Client.PlayerState.SetStat(STAT_CHASE, 0);
}

#ifdef CLEANCTF_ENABLED
void CPlayerEntity::SetCTFStats()
{
	int i;
	int p1, p2;
	CFlagEntity *e;

	if (ctfgame.match > MATCH_NONE)
		Client.PlayerState.SetStat(STAT_CTF_MATCH, CONFIG_CTF_MATCH);
	else
		Client.PlayerState.SetStat(STAT_CTF_MATCH, 0);

	//ghosting
	if (Client.resp.ghost)
	{
		Client.resp.ghost->score = Client.resp.score;
		Q_strncpyz(Client.resp.ghost->netname, Client.pers.netname, sizeof(Client.resp.ghost->netname));
		Client.resp.ghost->number = State.GetNumber();
	}

	// logo headers for the frag display
	Client.PlayerState.SetStat(STAT_CTF_TEAM1_HEADER, ImageIndex ("ctfsb1"));
	Client.PlayerState.SetStat(STAT_CTF_TEAM2_HEADER, ImageIndex ("ctfsb2"));

	// if during intermission, we must blink the team header of the winning team
	if (level.intermissiontime && (level.framenum & 8))
	{ // blink 1/8th second
		// note that ctfgame.total[12] is set when we go to intermission
		if (ctfgame.team1 > ctfgame.team2)
			Client.PlayerState.SetStat(STAT_CTF_TEAM1_HEADER, 0);
		else if (ctfgame.team2 > ctfgame.team1)
			Client.PlayerState.SetStat(STAT_CTF_TEAM2_HEADER, 0);
		else if (ctfgame.total1 > ctfgame.total2) // frag tie breaker
			Client.PlayerState.SetStat(STAT_CTF_TEAM1_HEADER, 0);
		else if (ctfgame.total2 > ctfgame.total1) 
			Client.PlayerState.SetStat(STAT_CTF_TEAM2_HEADER, 0);
		else
		{ // tie game!
			Client.PlayerState.SetStat(STAT_CTF_TEAM1_HEADER, 0);
			Client.PlayerState.SetStat(STAT_CTF_TEAM2_HEADER, 0);
		}
	}

	i = 0;

	// figure out what icon to display for team logos
	// three states:
	//   flag at base
	//   flag taken
	//   flag dropped
	p1 = ImageIndex ("i_ctf1");
	e = dynamic_cast<CFlagEntity*>(CC_Find(NULL, FOFS(classname), "item_flag_team1"));
	if (e != NULL)
	{
		if (e->GetSolid() == SOLID_NOT)
		{
			int i;

			// not at base
			// check if on player
			p1 = ImageIndex ("i_ctf1d"); // default to dropped
			for (i = 1; i <= game.maxclients; i++)
			{
				CPlayerEntity *Player = dynamic_cast<CPlayerEntity*>(g_edicts[i].Entity);

				if (Player->IsInUse() &&
					(Player->Client.pers.Flag == NItems::RedFlag))
				{
					// enemy has it
					p1 = ImageIndex ("i_ctf1t");
					break;
				}
			}
		}
		else if (e->SpawnFlags & DROPPED_ITEM)
			p1 = ImageIndex ("i_ctf1d"); // must be dropped
	}
	p2 = ImageIndex ("i_ctf2");
	e = dynamic_cast<CFlagEntity*>(CC_Find(NULL, FOFS(classname), "item_flag_team2"));
	if (e != NULL)
	{
		if (e->GetSolid() == SOLID_NOT)
		{
			int i;

			// not at base
			// check if on player
			p2 = ImageIndex ("i_ctf2d"); // default to dropped
			for (i = 1; i <= game.maxclients; i++)
			{
				CPlayerEntity *Player = dynamic_cast<CPlayerEntity*>(g_edicts[i].Entity);

				if (Player->IsInUse() &&
					(Player->Client.pers.Flag == NItems::BlueFlag))
				{
					// enemy has it
					p2 = ImageIndex ("i_ctf2t");
					break;
				}
			}
		}
		else if (e->SpawnFlags & DROPPED_ITEM)
			p2 = ImageIndex ("i_ctf2d"); // must be dropped
	}


	Client.PlayerState.SetStat(STAT_CTF_TEAM1_PIC, p1);
	Client.PlayerState.SetStat(STAT_CTF_TEAM2_PIC, p2);

	if (ctfgame.last_flag_capture && level.framenum - ctfgame.last_flag_capture < 50)
	{
		if (ctfgame.last_capture_team == CTF_TEAM1)
		{
			if (level.framenum & 8)
				Client.PlayerState.SetStat(STAT_CTF_TEAM1_PIC, p1);
			else
				Client.PlayerState.SetStat(STAT_CTF_TEAM1_PIC, 0);
		}
		else
		{
			if (level.framenum & 8)
				Client.PlayerState.SetStat(STAT_CTF_TEAM2_PIC, p2);
			else
				Client.PlayerState.SetStat(STAT_CTF_TEAM2_PIC, 0);
		}
	}

	Client.PlayerState.SetStat(STAT_CTF_TEAM1_CAPS, ctfgame.team1);
	Client.PlayerState.SetStat(STAT_CTF_TEAM2_CAPS, ctfgame.team2);

	Client.PlayerState.SetStat(STAT_CTF_FLAG_PIC, 0);
	if (Client.resp.ctf_team == CTF_TEAM1 &&
		(Client.pers.Flag == NItems::BlueFlag) &&
		(level.framenum & 8))
		Client.PlayerState.SetStat(STAT_CTF_FLAG_PIC, ImageIndex ("i_ctf2"));

	else if (Client.resp.ctf_team == CTF_TEAM2 &&
		(Client.pers.Flag == NItems::RedFlag) &&
		(level.framenum & 8))
		Client.PlayerState.SetStat(STAT_CTF_FLAG_PIC, ImageIndex ("i_ctf1"));

	Client.PlayerState.SetStat(STAT_CTF_JOINED_TEAM1_PIC, 0);
	Client.PlayerState.SetStat(STAT_CTF_JOINED_TEAM2_PIC, 0);
	if (Client.resp.ctf_team == CTF_TEAM1)
		Client.PlayerState.SetStat(STAT_CTF_JOINED_TEAM1_PIC, ImageIndex ("i_ctfj"));
	else if (Client.resp.ctf_team == CTF_TEAM2)
		Client.PlayerState.SetStat(STAT_CTF_JOINED_TEAM2_PIC, ImageIndex ("i_ctfj"));

	Client.PlayerState.SetStat(STAT_CTF_ID_VIEW, 0);
	if (Client.resp.id_state)
		CTFSetIDView();
}

bool loc_CanSee (CBaseEntity *targ, CBaseEntity *inflictor);
void CPlayerEntity::CTFSetIDView()
{
	Client.PlayerState.SetStat(STAT_CTF_ID_VIEW, 0);

	vec3f forward, oldForward;
	Client.ViewAngle.ToVectors(&forward, NULL, NULL);
	oldForward = forward;
	forward = (forward * 1024) + State.GetOrigin();

	CTrace tr (State.GetOrigin(), forward, gameEntity, CONTENTS_MASK_SOLID);
	if (tr.fraction < 1 && tr.ent && ((tr.ent - g_edicts) >= 1 && (tr.ent - g_edicts) <= game.maxclients))
	{
		Client.PlayerState.SetStat(STAT_CTF_ID_VIEW, CS_PLAYERSKINS + (State.GetNumber()-1));
		return;
	}

	forward = oldForward;
	CPlayerEntity *best = NULL;
	float bd = 0;
	for (int i = 1; i <= game.maxclients; i++)
	{
		CPlayerEntity *who = dynamic_cast<CPlayerEntity*>((g_edicts + i)->Entity);
		if (!who->IsInUse() || who->GetSolid() == SOLID_NOT)
			continue;
		vec3f dir = who->State.GetOrigin() - State.GetOrigin();
		dir.NormalizeFast ();
		float d = forward.Dot(dir);
		if (d > bd && loc_CanSee(this, who))
		{
			bd = d;
			best = who;
		}
	}
	if (bd > 0.90)
		Client.PlayerState.SetStat(STAT_CTF_ID_VIEW, CS_PLAYERSKINS + (best->State.GetNumber()-1));
}

void CPlayerEntity::CTFAssignGhost()
{
	int ghost, i;

	for (ghost = 0; ghost < MAX_CS_CLIENTS; ghost++)
	{
		if (!ctfgame.ghosts[ghost].code)
			break;
	}
	if (ghost == MAX_CS_CLIENTS)
		return;
	ctfgame.ghosts[ghost].team = Client.resp.ctf_team;
	ctfgame.ghosts[ghost].score = 0;
	for (;;)
	{
		ctfgame.ghosts[ghost].code = 10000 + (irandom(90000));
		for (i = 0; i < MAX_CS_CLIENTS; i++)
		{
			if (i != ghost && ctfgame.ghosts[i].code == ctfgame.ghosts[ghost].code)
				break;
		}
		if (i == MAX_CS_CLIENTS)
			break;
	}
	ctfgame.ghosts[ghost].ent = this;
	Q_strncpyz(ctfgame.ghosts[ghost].netname, Client.pers.netname, sizeof(ctfgame.ghosts[ghost].netname));
	Client.resp.ghost = ctfgame.ghosts + ghost;
	PrintToClient (PRINT_CHAT, "Your ghost code is **** %d ****\n", ctfgame.ghosts[ghost].code);
	PrintToClient (PRINT_HIGH, "If you lose connection, you can rejoin with your score "
		"intact by typing \"ghost %d\".\n", ctfgame.ghosts[ghost].code);
}
#endif

void CPlayerEntity::MoveToIntermission ()
{
	if (game.mode != GAME_SINGLEPLAYER)
		Client.showscores = true;
	State.SetOrigin (level.IntermissionOrigin);

	Client.PlayerState.GetPMove()->origin[0] = level.IntermissionOrigin.X*8;
	Client.PlayerState.GetPMove()->origin[1] = level.IntermissionOrigin.Y*8;
	Client.PlayerState.GetPMove()->origin[2] = level.IntermissionOrigin.Z*8;
	Client.PlayerState.SetViewAngles (level.IntermissionAngles);
	Client.PlayerState.GetPMove()->pmType = PMT_FREEZE;
	Client.PlayerState.SetGunIndex (0);

	vec4_t viewBlend;
	Client.PlayerState.GetViewBlend (viewBlend);

	viewBlend[3] = 0;
	Client.PlayerState.SetViewBlend (viewBlend);

	Client.PlayerState.SetRdFlags (Client.PlayerState.GetRdFlags() & ~RDF_UNDERWATER);

	// clean up powerup info
	Client.quad_framenum = 0;
	Client.invincible_framenum = 0;
	Client.breather_framenum = 0;
	Client.enviro_framenum = 0;
	Client.grenade_blew_up = Client.grenade_thrown = false;
	Client.grenade_time = 0;

	gameEntity->viewheight = 0;
	State.SetModelIndex (0);
	State.SetModelIndex (0, 2);
	State.SetModelIndex (0, 3);
	State.SetEffects (0);
	State.SetSound (0);
	SetSolid (SOLID_NOT);

	// add the layout
	Enemy = NULL;
	if (game.mode != GAME_SINGLEPLAYER)
		DeathmatchScoreboardMessage (true);
}

bool CPlayerEntity::ApplyStrengthSound()
{
	if (Client.pers.Tech && (Client.pers.Tech->GetTechNumber() == CTFTECH_STRENGTH_NUMBER))
	{
		if (Client.techsndtime < level.framenum)
		{
			Client.techsndtime = level.framenum + 10;
			PlaySound (CHAN_AUTO, SoundIndex((Client.quad_framenum > level.framenum) ? "ctf/tech2x.wav" : "ctf/tech2.wav"), (Client.silencer_shots) ? 0.2f : 1.0f);
		}
		return true;
	}
	return false;
}

bool CPlayerEntity::ApplyHaste()
{
	return (Client.pers.Tech && (Client.pers.Tech->GetTechNumber() == CTFTECH_HASTE_NUMBER));
}

void CPlayerEntity::ApplyHasteSound()
{
	if (Client.pers.Tech && (Client.pers.Tech->GetTechNumber() == CTFTECH_HASTE_NUMBER) && Client.techsndtime < level.framenum)
	{
		Client.techsndtime = level.framenum + 10;
		PlaySound (CHAN_AUTO, SoundIndex("ctf/tech3.wav"), (Client.silencer_shots) ? 0.2f : 1.0f);
	}
}

bool CPlayerEntity::HasRegeneration()
{
	return (Client.pers.Tech && (Client.pers.Tech->GetTechNumber() == CTFTECH_REGEN_NUMBER));
}


/*
==================
LookAtKiller
==================
*/
void CPlayerEntity::LookAtKiller (CBaseEntity *inflictor, CBaseEntity *attacker)
{
	vec3f dir;
	if (attacker && (attacker != World) && (attacker != this))
		dir = attacker->State.GetOrigin() - State.GetOrigin();
	else if (inflictor && (inflictor != World) && (inflictor != this))
		dir = inflictor->State.GetOrigin() - State.GetOrigin();
	else
	{
		Client.KillerYaw = State.GetAngles().Y;
		return;
	}

	Client.KillerYaw = (dir.X) ? (180/M_PI*atan2f(dir.Y, dir.X)) : (((dir.Y > 0) ? 90 : dir.Y < 0) ? -90 : 0);
	if (Client.KillerYaw < 0)
		Client.KillerYaw += 360;
}

void CPlayerEntity::DeadDropTech ()
{
	if (!Client.pers.Tech)
		return;

	CItemEntity *dropped = Client.pers.Tech->DropItem(this);
	// hack the velocity to make it bounce random
	dropped->Velocity.X = (frand() * 600) - 300;
	dropped->Velocity.Y = (frand() * 600) - 300;
	dropped->NextThink = level.framenum + CTF_TECH_TIMEOUT;
	dropped->SetOwner (NULL);
	Client.pers.Inventory.Set(Client.pers.Tech, 0);

	Client.pers.Tech = NULL;
}

void CPlayerEntity::TossClientWeapon ()
{
	if (!(game.mode & GAME_DEATHMATCH))
		return;

	CBaseItem *Item = ((Client.pers.Weapon) ? ((Client.pers.Weapon->WeaponItem) ? Client.pers.Weapon->WeaponItem : Client.pers.Weapon->Item) : NULL);
	// Personally, this is dumb.
	//if (! self->client->pers.Inventory.Has(Item->Ammo) )
	//	item = NULL;
	if (Item && !Item->WorldModel)
		Item = NULL;

	bool quad = (!dmFlags.dfQuadDrop) ? false : (bool)(Client.quad_framenum > (level.framenum + 10));
	float spread = (Item && quad) ? 22.5f : 0.0f;

	if (Item)
	{
		Client.ViewAngle.Y -= spread;
		CItemEntity *drop = Item->DropItem (this);
		Client.ViewAngle.Y += spread;
		drop->SpawnFlags |= DROPPED_PLAYER_ITEM;
		if (Client.pers.Weapon->WeaponItem)
			drop->gameEntity->count = Client.pers.Weapon->WeaponItem->Ammo->Quantity;
		else
			drop->gameEntity->count = (static_cast<CAmmo*>(Client.pers.Weapon->Item))->Quantity;
	}

	if (quad)
	{
		Client.ViewAngle.Y += spread;
		CItemEntity *drop = NItems::Quad->DropItem (this);
		Client.ViewAngle.Y -= spread;
		drop->SpawnFlags |= DROPPED_PLAYER_ITEM;

		drop->NextThink = level.framenum + (Client.quad_framenum - level.framenum);
		drop->ThinkState = ITS_FREE;
	}
}

#ifdef USE_EXTENDED_GAME_IMPORTS
edict_t	*pm_passent;

// pmove doesn't need to know about passent and contentmask
cmTrace_t	PM_trace (vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end)
{
_CC_DISABLE_DEPRECATION
	if (pm_passent->health > 0)
		return gi.trace(start, mins, maxs, end, pm_passent, CONTENTS_MASK_PLAYERSOLID);
	else
		return gi.trace(start, mins, maxs, end, pm_passent, CONTENTS_MASK_DEADSOLID);
_CC_ENABLE_DEPRECATION
}
#endif

void CPlayerEntity::ClientThink (userCmd_t *ucmd)
{
#ifdef USE_EXTENDED_GAME_IMPORTS
	pMove_t		pm;
#else
	pMoveNew_t	pm;
#endif

	level.CurrentEntity = this;

	if (level.intermissiontime)
	{
		Client.PlayerState.GetPMove()->pmType = PMT_FREEZE;
		// can exit intermission after five seconds
		if (level.framenum > level.intermissiontime + 50 
			&& (ucmd->buttons & BUTTON_ANY) )
			level.exitintermission = true;
		return;
	}

#ifdef USE_EXTENDED_GAME_IMPORTS
	pm_passent = ent;
#endif

//ZOID
	int oldbuttons = Client.buttons;
	Client.buttons = ucmd->buttons;
	Client.latched_buttons |= Client.buttons & ~oldbuttons;

	if (Client.chase_target)
	{
		Client.resp.cmd_angles[0] = SHORT2ANGLE(ucmd->angles[0]);
		Client.resp.cmd_angles[1] = SHORT2ANGLE(ucmd->angles[1]);
		Client.resp.cmd_angles[2] = SHORT2ANGLE(ucmd->angles[2]);

		if (Client.latched_buttons & BUTTON_ATTACK)
		{
			if (Client.chase_target)
				ChaseNext();
			else
				GetChaseTarget();
			Client.latched_buttons &= ~BUTTON_ATTACK;
		}

		if (ucmd->upMove >= 10)
		{
			if (!(Client.PlayerState.GetPMove()->pmFlags & PMF_JUMP_HELD))
			{
				Client.PlayerState.GetPMove()->pmFlags |= PMF_JUMP_HELD;

				Client.latched_buttons = 0;

				if (Client.chase_target)
				{
					Client.chase_target = NULL;
					Client.PlayerState.GetPMove()->pmFlags &= ~PMF_NO_PREDICTION;
					Client.PlayerState.SetGunIndex (0);
					Client.PlayerState.SetGunFrame (0);
				}
				else
					GetChaseTarget();
			}
		}
		else
			Client.PlayerState.GetPMove()->pmFlags &= ~PMF_JUMP_HELD;
		return;
	}
//ZOID

	// set up for pmove
	memset (&pm, 0, sizeof(pm));

	if (NoClip)
		Client.PlayerState.GetPMove()->pmType = PMT_SPECTATOR;
	else if (State.GetModelIndex() != 255)
		Client.PlayerState.GetPMove()->pmType = PMT_GIB;
	else if (DeadFlag)
		Client.PlayerState.GetPMove()->pmType = PMT_DEAD;
	else
		Client.PlayerState.GetPMove()->pmType = PMT_NORMAL;

	Client.PlayerState.GetPMove()->gravity = sv_gravity->Float();
	pm.state = *Client.PlayerState.GetPMove();

	for (int i = 0; i < 3; i++)
	{
		pm.state.origin[i] = State.GetOrigin()[i]*8;
		pm.state.velocity[i] = Velocity[i]*8;
	}

	if (memcmp (&Client.old_pmove, &pm.state, sizeof(pm.state)))
	{
		pm.snapInitial = true;
//		gi.dprintf ("pmove changed!\n");
	}

	pm.cmd = *ucmd;

#ifdef USE_EXTENDED_GAME_IMPORTS
	pm.trace = PM_trace;
	pm.pointContents = PointContents;
#endif

	// perform a pmove
#ifdef USE_EXTENDED_GAME_IMPORTS
	gi.Pmove (&pm);
#else
	SV_Pmove (this, &pm, CCvar("sv_airaccelerate", 0, 0).Float());
#endif

	// save results of pmove
	Client.PlayerState.SetPMove (&pm.state);
	Client.old_pmove = pm.state;

	State.SetOrigin(vec3f(pm.state.origin[0]*0.125, pm.state.origin[1]*0.125, pm.state.origin[2]*0.125));
	for (int i = 0; i < 3; i++)
		Velocity[i] = pm.state.velocity[i]*0.125;

	Vec3Copy (pm.mins, gameEntity->mins);
	Vec3Copy (pm.maxs, gameEntity->maxs);

	Client.resp.cmd_angles[0] = SHORT2ANGLE(ucmd->angles[0]);
	Client.resp.cmd_angles[1] = SHORT2ANGLE(ucmd->angles[1]);
	Client.resp.cmd_angles[2] = SHORT2ANGLE(ucmd->angles[2]);

	if (GroundEntity && !pm.groundEntity && (pm.cmd.upMove >= 10) && (pm.waterLevel == 0))
	{
		PlaySound (CHAN_VOICE, gMedia.Player.Jump);
		PlayerNoiseAt (State.GetOrigin(), PNOISE_SELF);
	}

	gameEntity->viewheight = pm.viewHeight;
	gameEntity->waterlevel = pm.waterLevel;
	gameEntity->watertype = pm.waterType;
	GroundEntity = (pm.groundEntity) ? pm.groundEntity->Entity : NULL;
	if (GroundEntity)
		GroundEntityLinkCount = GroundEntity->GetLinkCount();

	if (DeadFlag)
		Client.PlayerState.SetViewAngles (vec3f(-15, Client.KillerYaw, 40));
	else
	{
		Client.ViewAngle.Set (pm.viewAngles);
		Client.PlayerState.SetViewAngles (pm.viewAngles);
	}

#ifdef CLEANCTF_ENABLED
//ZOID
	if (Client.ctf_grapple)
		Client.ctf_grapple->GrapplePull ();
//ZOID
#endif

	Link ();

	if (!NoClip && !(map_debug->Boolean()))
		G_TouchTriggers (this);

	// touch other objects
	if (!map_debug->Boolean())
	{
		for (int i = 0; i < pm.numTouch; i++)
		{
			edict_t *other = pm.touchEnts[i];
			if (other->Entity)
			{
				if ((other->Entity->EntityFlags & ENT_TOUCHABLE) && other->Entity->IsInUse())
				{
					CTouchableEntity *Touchered = dynamic_cast<CTouchableEntity*>(other->Entity);

					if (Touchered->Touchable)
						Touchered->Touch (this, NULL, NULL);
				}
				continue;
			}
		}
	}

	// save light level the player is standing on for
	// monster sighting AI
	gameEntity->light_level = ucmd->lightLevel;

	if (Client.resp.spectator
#ifdef CLEANCTF_ENABLED
		|| ((game.mode & GAME_CTF) && NoClip)
#endif)
		)
	{
		if (Client.latched_buttons & BUTTON_ATTACK)
		{
			if (Client.chase_target)
				ChaseNext();
			else
				GetChaseTarget();
		}

		if (ucmd->upMove >= 10)
		{
			if (!(Client.PlayerState.GetPMove()->pmFlags & PMF_JUMP_HELD))
			{
				Client.PlayerState.GetPMove()->pmFlags |= PMF_JUMP_HELD;

				Client.latched_buttons = 0;

				if (Client.chase_target)
				{
					Client.chase_target = NULL;
					Client.PlayerState.GetPMove()->pmFlags &= ~PMF_NO_PREDICTION;
				}
				else
					GetChaseTarget();
			}
		}
		else
			Client.PlayerState.GetPMove()->pmFlags &= ~PMF_JUMP_HELD;
	}

	// update chase cam if being followed
	for (int i = 1; i <= game.maxclients; i++)
	{
		CPlayerEntity *other = dynamic_cast<CPlayerEntity*>((g_edicts + i)->Entity);
		if (other->IsInUse() && other->Client.chase_target == this)
			other->UpdateChaseCam();
	}
}

#ifdef CLEANCTF_ENABLED
void CPlayerEntity::CTFAssignTeam()
{
	int i;
	int team1count = 0, team2count = 0;

	Client.resp.ctf_state = 0;

	if (!dmFlags.dfCtfForceJoin)
	{
		Client.resp.ctf_team = CTF_NOTEAM;
		return;
	}

	for (i = 1; i <= game.maxclients; i++)
	{
		CPlayerEntity *player = dynamic_cast<CPlayerEntity*>(g_edicts[i].Entity);

		if (!player->IsInUse() || player == this)
			continue;

		switch (player->Client.resp.ctf_team)
		{
		case CTF_TEAM1:
			team1count++;
			break;
		case CTF_TEAM2:
			team2count++;
		}
	}
	if (team1count < team2count)
		Client.resp.ctf_team = CTF_TEAM1;
	else if (team2count < team1count)
		Client.resp.ctf_team = CTF_TEAM2;
	else if (irandom(2))
		Client.resp.ctf_team = CTF_TEAM1;
	else
		Client.resp.ctf_team = CTF_TEAM2;
}
#endif

void CPlayerEntity::InitResp ()
{
#ifdef CLEANCTF_ENABLED
//ZOID
	int ctf_team = Client.resp.ctf_team;
	bool id_state = Client.resp.id_state;
//ZOID
#endif

	memset (&Client.resp, 0, sizeof(Client.resp));

#ifdef CLEANCTF_ENABLED
//ZOID
	Client.resp.ctf_team = ctf_team;
	Client.resp.id_state = id_state;
//ZOID
#endif

	Client.resp.enterframe = level.framenum;
	Client.resp.coop_respawn = Client.pers;

#ifdef CLEANCTF_ENABLED
//ZOID
	if ((game.mode & GAME_CTF) && Client.resp.ctf_team < CTF_TEAM1)
		CTFAssignTeam();
//ZOID
#endif
}

/*
==================
SaveClientData

Some information that should be persistant, like health, 
is still stored in the edict structure, so it needs to
be mirrored out to the client structure before all the
edicts are wiped.
==================
*/
clientPersistent_t *SavedClients;

void CPlayerEntity::SaveClientData ()
{
	SavedClients = QNew (com_gamePool, 0) clientPersistent_t[game.maxclients];
	for (int i=0 ; i<game.maxclients ; i++)
	{
		memset (&SavedClients[i], 0, sizeof(clientPersistent_t));
		if (!g_edicts[1+i].Entity)
			return; // Not set up

		CPlayerEntity *ent = dynamic_cast<CPlayerEntity*>(g_edicts[1+i].Entity);
		if (!ent->IsInUse())
			continue;

		memcpy (&SavedClients[i], &ent->Client.pers, sizeof(clientPersistent_t));
		SavedClients[i].health = ent->Health;
		SavedClients[i].max_health = ent->MaxHealth;
		SavedClients[i].savedFlags = (ent->Flags & (FL_GODMODE|FL_NOTARGET|FL_POWER_ARMOR));
		if (game.mode & GAME_COOPERATIVE)
			SavedClients[i].score = ent->Client.resp.score;
	}
}

CBaseEntity *CPlayerEntity::SelectCoopSpawnPoint ()
{
	int index = State.GetNumber()-1;

	// player 0 starts in normal player spawn point
	if (!index)
		return NULL;

	CBaseEntity *spot = NULL;

	// assume there are four coop spots at each spawnpoint
	while (1)
	{
		spot = CC_Find (spot, FOFS(classname), "info_player_coop");
		if (!spot)
			return NULL;	// we didn't have enough...

		char *target = spot->gameEntity->targetname;
		if (!target)
			target = "";
		if ( Q_stricmp(game.spawnpoint, target) == 0 )
		{	// this is a coop spawn point for one of the clients here
			index--;
			if (!index)
				return spot;		// this is it
		}
	}

	return spot;
}

/*
===========
SelectSpawnPoint

Chooses a player start, deathmatch start, coop start, etc
============
*/
void	CPlayerEntity::SelectSpawnPoint (vec3f &origin, vec3f &angles)
{
	CBaseEntity	*spot = NULL;

	if (!(game.mode & GAME_SINGLEPLAYER))
		spot = 
#ifdef CLEANCTF_ENABLED
		(game.mode & GAME_CTF) ? SelectCTFSpawnPoint() :
#endif
		(game.mode & GAME_DEATHMATCH) ? SelectDeathmatchSpawnPoint () : SelectCoopSpawnPoint ();

	// find a single player start spot
	if (!spot)
	{
		while ((spot = CC_Find (spot, FOFS(classname), "info_player_start")) != NULL)
		{
			if (!game.spawnpoint[0] && !spot->gameEntity->targetname)
				break;

			if (!game.spawnpoint[0] || !spot->gameEntity->targetname)
				continue;

			if (Q_stricmp(game.spawnpoint, spot->gameEntity->targetname) == 0)
				break;
		}

		if (!spot)
		{
			// There wasn't a spawnpoint without a target, so use any
			if (!game.spawnpoint[0]) {
				spot = CC_Find (spot, FOFS(classname), "info_player_start");

				if (!spot)
					spot = CC_Find (spot, FOFS(classname), "info_player_deathmatch");
			}
			// FIXME: Remove.
			if (!spot)
				GameError ("Couldn't find spawn point %s", game.spawnpoint);
		}
	}

	origin = spot->State.GetOrigin () + vec3f(0, 0, 9);
	angles = spot->State.GetAngles ();
}

#ifdef CLEANCTF_ENABLED
/*
================
SelectCTFSpawnPoint

go to a ctf point, but NOT the two points closest
to other players
================
*/
CBaseEntity *CPlayerEntity::SelectCTFSpawnPoint ()
{
	CBaseEntity	*spot = NULL, *spot1 = NULL, *spot2 = NULL;
	int		count = 0;
	int		selection;
	float	range, range1 = 99999, range2 = 99999;
	char	*cname;

	if (Client.resp.ctf_state)
		return (dmFlags.dfSpawnFarthest) ? SelectFarthestDeathmatchSpawnPoint () : SelectRandomDeathmatchSpawnPoint ();

	Client.resp.ctf_state++;

	switch (Client.resp.ctf_team)
	{
	case CTF_TEAM1:
		cname = "info_player_team1";
		break;
	case CTF_TEAM2:
		cname = "info_player_team2";
		break;
	default:
		return SelectRandomDeathmatchSpawnPoint();
	}

	while ((spot = CC_Find (spot, FOFS(classname), cname)) != NULL)
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
		spot1 = spot2 = NULL;
	else
		count -= 2;

	selection = irandom(count);

	spot = NULL;
	do
	{
		spot = CC_Find (spot, FOFS(classname), cname);
		if (spot == spot1 || spot == spot2)
			selection++;
	} while(selection--);

	return spot;
}
#endif

vec3f VelocityForDamage (int damage);
void CPlayerEntity::TossHead (int damage)
{
	if (irandom(2))
	{
		State.SetModelIndex (gMedia.Gib_Head[1]);
		State.SetSkinNum (1);		// second skin is player
	}
	else
	{
		State.SetModelIndex (gMedia.Gib_Skull);
		State.SetSkinNum (0);
	}

	State.SetFrame (0);
	SetMins (vec3f(-16, -16, 0));
	SetMaxs (vec3f(16, 16, 16));

	CanTakeDamage = false;
	SetSolid (SOLID_NOT);
	State.SetEffects (EF_GIB);
	State.SetSound (0);
	Flags |= FL_NO_KNOCKBACK;

	Velocity += VelocityForDamage (damage);

	Link ();
}

void CPlayerEntity::Die (CBaseEntity *inflictor, CBaseEntity *attacker, int damage, vec3f &point)
{
	CanTakeDamage = true;
	TossPhysics = true;

	State.SetModelIndex (0, 2);	// remove linked weapon model
	State.SetModelIndex (0, 3);	// remove linked ctf flag

	State.SetAngles (vec3f(0, State.GetAngles().Y, 0));

	State.SetSound (0);
	Client.weapon_sound = 0;

	vec3f maxs = GetMaxs();
	maxs[2] = -8;
	SetMaxs (maxs);

	SetSvFlags (GetSvFlags() | SVF_DEADMONSTER);

	if (!DeadFlag)
	{
		Client.respawn_time = level.framenum + 10;
		LookAtKiller (inflictor, attacker);
		Client.PlayerState.GetPMove()->pmType = PMT_DEAD;
		Obituary (attacker);

#ifdef CLEANCTF_ENABLED
		if (attacker->EntityFlags & ENT_PLAYER)
		{
			CPlayerEntity *Attacker = dynamic_cast<CPlayerEntity*>(attacker);
//ZOID
			// if at start and same team, clear
			if ((game.mode & GAME_CTF) && (meansOfDeath == MOD_TELEFRAG) &&
				(Client.resp.ctf_state < 2) &&
				(Client.resp.ctf_team == Attacker->Client.resp.ctf_team))
			{
				Attacker->Client.resp.score--;
				Client.resp.ctf_state = 0;
			}

			CTFFragBonuses(this, Attacker);
		}

//ZOID
#endif
		TossClientWeapon ();

#ifdef CLEANCTF_ENABLED
//ZOID
		if (game.mode & GAME_CTF)
		{
			CGrapple::PlayerResetGrapple(this);
			CTFDeadDropFlag(this);
		}
//ZOID
#endif
		if ((game.mode & GAME_CTF) || dmFlags.dfDmTechs) 
			DeadDropTech();

		if (game.mode & GAME_DEATHMATCH)
			Cmd_Help_f (this);		// show scores

		// clear inventory
		// this is kind of ugly, but it's how we want to handle keys in coop
		for (int n = 0; n < GetNumItems(); n++)
		{
			if ((game.mode == GAME_COOPERATIVE) && (GetItemByIndex(n)->Flags & ITEMFLAG_KEY))
				Client.resp.coop_respawn.Inventory.Set(n, Client.pers.Inventory.Has(n));
			Client.pers.Inventory.Set(n, 0);
		}
	}

	// remove powerups
	Client.quad_framenum = 0;
	Client.invincible_framenum = 0;
	Client.breather_framenum = 0;
	Client.enviro_framenum = 0;
	Flags &= ~FL_POWER_ARMOR;

	if (Health < -40)
	{	// gib
		PlaySound (CHAN_BODY, SoundIndex ("misc/udeath.wav"));
		for (int n = 0; n < 4; n++)
			CGibEntity::Spawn (this, gMedia.Gib_SmallMeat, damage, GIB_ORGANIC);
		TossHead (damage);

		CanTakeDamage = false;
//ZOID
		Client.anim_priority = ANIM_DEATH;
		Client.anim_end = 0;
//ZOID
	}
	else
	{	// normal death
		if (!DeadFlag)
		{
			static int i;

			i = (i+1)%3;
			// start a death animation
			Client.anim_priority = ANIM_DEATH;
			if (Client.PlayerState.GetPMove()->pmFlags & PMF_DUCKED)
			{
				State.SetFrame (FRAME_crdeath1-1);
				Client.anim_end = FRAME_crdeath5;
			}
			else
			{
				switch (i)
				{
				case 0:
					State.SetFrame (FRAME_death101-1);
					Client.anim_end = FRAME_death106;
					break;
				case 1:
					State.SetFrame (FRAME_death201-1);
					Client.anim_end = FRAME_death206;
					break;
				case 2:
					State.SetFrame (FRAME_death301-1);
					Client.anim_end = FRAME_death308;
					break;
				}
			}
			PlaySound (CHAN_VOICE, gMedia.Player.Death[(irandom(4))]);
		}
	}

	DeadFlag = true;

	Link ();
};

void CPlayerEntity::PrintToClient (EGamePrintLevel printLevel, char *fmt, ...)
{
	va_list		argptr;
	char		text[MAX_COMPRINT];

	va_start (argptr, fmt);
	vsnprintf_s (text, sizeof(text), MAX_COMPRINT, fmt, argptr);
	va_end (argptr);

	ClientPrintf (gameEntity, printLevel, "%s", text);
};

void CPlayerEntity::UpdateChaseCam()
{
	vec3f forward, right;
	CPlayerEntity *targ;

	// is our chase target gone?
	if (!Client.chase_target->IsInUse()
		|| Client.chase_target->Client.resp.spectator || Client.chase_target->Client.chase_target)
	{
			CPlayerEntity *old = Client.chase_target;
			ChaseNext();
			if (Client.chase_target == old)
			{
				Client.chase_target = NULL;
				Client.PlayerState.GetPMove()->pmFlags &= ~PMF_NO_PREDICTION;
				return;
			}
	}

	targ = Client.chase_target;

	if (Client.PlayerState.GetGunIndex())
		Client.PlayerState.SetGunIndex (0);

	switch (Client.chase_mode)
	{
	case 0:
		{
			vec3f ownerv = targ->State.GetOrigin();
			vec3f oldgoal = State.GetOrigin();
			ownerv.Z += targ->gameEntity->viewheight;

			vec3f angles = targ->Client.ViewAngle;

			if(angles.X > 56)
				angles.X = 56;

			angles.ToVectors (&forward, &right, NULL);
			forward.NormalizeFast ();
			vec3f o = ownerv.MultiplyAngles (-30, forward);

			if(o.Z < targ->State.GetOrigin().Z + 20)
				o.Z = targ->State.GetOrigin().Z + 20;

			if (!targ->GroundEntity)
				o.Z += 16;

			CTrace trace (ownerv, o, targ->gameEntity, CONTENTS_MASK_SOLID);

			vec3f goal = trace.EndPos.MultiplyAngles (2, forward);
			o = goal + vec3f(0, 0, 6);
			trace = CTrace (goal, o, targ->gameEntity, CONTENTS_MASK_SOLID);

			if (trace.fraction < 1)
				goal = trace.EndPos - vec3f(0, 0, 6);

			o = goal - vec3f(0, 0, 6);
			trace = CTrace(goal, o, targ->gameEntity, CONTENTS_MASK_SOLID);

			if(trace.fraction < 1)
				goal = trace.EndPos + vec3f(0, 0, 6);

			if (targ->DeadFlag)
				Client.PlayerState.GetPMove()->pmType = PMT_DEAD;
			else
				Client.PlayerState.GetPMove()->pmType = PMT_FREEZE;

			State.SetOrigin (goal);

			for (int i = 0; i < 3; i++)
				Client.PlayerState.GetPMove()->deltaAngles[i] = ANGLE2SHORT(targ->Client.ViewAngle[i] - Client.resp.cmd_angles[i]);

			if (targ->DeadFlag)
				Client.PlayerState.SetViewAngles (vec3f(-15, targ->Client.KillerYaw, 40));
			else
			{
				angles = targ->Client.ViewAngle + targ->Client.KickAngles;
				Client.PlayerState.SetViewAngles (angles);
				Client.ViewAngle = angles;
			}
		}
		break;
	case 1:
		{
			Client.PlayerState.SetFov (90);

			if(Client.resp.cmd_angles[PITCH] > 89)
				Client.resp.cmd_angles[PITCH] = 89;

			if(Client.resp.cmd_angles[PITCH] < -89)
				Client.resp.cmd_angles[PITCH] = -89;

			vec3f ownerv = targ->State.GetOrigin() + vec3f(0, 0, targ->gameEntity->viewheight);

			vec3f angles = Client.PlayerState.GetViewAngles();
			angles.ToVectors (&forward, &right, NULL);
			forward.NormalizeFast ();
			vec3f o = ownerv.MultiplyAngles (-150, forward);

			if (!targ->GroundEntity)
				o.Z += 16;

			CTrace trace(ownerv, o, targ->gameEntity, CONTENTS_MASK_SOLID);

			vec3f goal = trace.EndPos.MultiplyAngles (2, forward);
			o = goal + vec3f(0, 0, 6);

			trace = CTrace(goal, o, targ->gameEntity, CONTENTS_MASK_SOLID);

			if(trace.fraction < 1)
				goal = trace.EndPos - vec3f(0, 0, 6);

			o = goal - vec3f(0, 0, 6);

			trace = CTrace(goal, o, targ->gameEntity, CONTENTS_MASK_SOLID);

			if(trace.fraction < 1)
				goal = trace.EndPos + vec3f(0, 0, 6);

			if (targ->DeadFlag)
				Client.PlayerState.GetPMove()->pmType = PMT_DEAD;
			else
				Client.PlayerState.GetPMove()->pmType = PMT_FREEZE;

			State.SetOrigin (goal);

			for (int i = 0; i < 3; i++)
				Client.PlayerState.GetPMove()->deltaAngles[i] = ANGLE2SHORT(targ->Client.ViewAngle[i] - Client.resp.cmd_angles[i]);

			Client.PlayerState.SetViewAngles (Client.resp.cmd_angles);
		}
		break;
	default:
		{
			vec3f ownerv = targ->State.GetOrigin();
			vec3f angles = targ->Client.ViewAngle;

			angles.ToVectors (&forward, &right, NULL);
			forward.NormalizeFast ();
			vec3f o = ownerv.MultiplyAngles (16, forward);
			o.Z += targ->gameEntity->viewheight;

			State.SetOrigin (o);
			Client.PlayerState.SetFov (targ->Client.PlayerState.GetFov());

			Client.PlayerState.SetGunIndex (targ->Client.PlayerState.GetGunIndex());
			Client.PlayerState.SetGunAngles (targ->Client.PlayerState.GetGunAngles());
			Client.PlayerState.SetGunFrame (targ->Client.PlayerState.GetGunFrame());
			Client.PlayerState.SetGunOffset (targ->Client.PlayerState.GetGunOffset());

			for (int i = 0; i < 3; i++)
				Client.PlayerState.GetPMove()->deltaAngles[i] = ANGLE2SHORT(targ->Client.ViewAngle[i] - Client.resp.cmd_angles[i]);

			if (targ->DeadFlag)
				Client.PlayerState.SetViewAngles (vec3f(-15, targ->Client.KillerYaw, 40));
			else
			{
				angles = targ->Client.ViewAngle + targ->Client.KickAngles;
				Client.PlayerState.SetViewAngles (angles);
				Client.ViewAngle = angles;
			}
		}
		break;
	};

	gameEntity->viewheight = 0;
	Client.PlayerState.GetPMove()->pmFlags |= PMF_NO_PREDICTION;
	Link ();

	if ((!Client.showscores && !Client.resp.MenuState.InMenu &&
		!Client.showinventory && !Client.showhelp &&
		!(level.framenum & 31)) || Client.update_chase)
	{
		CStatusBar Chasing;
		char temp[128];
		Q_snprintfz (temp, sizeof(temp), "Chasing %s\n%s", targ->Client.pers.netname, (Client.chase_mode == 0) ? "Tight Chase" : ((Client.chase_mode == 1) ? "Freeform Chase" : "FPS Chase"));

		Chasing.AddVirtualPoint_X (0);
		Chasing.AddVirtualPoint_Y (-68);
		Chasing.AddString (temp, true, false);
		Chasing.SendMsg (gameEntity, false);

		Client.update_chase = false;
	}
}

void CPlayerEntity::ChaseNext()
{
	if (!Client.chase_target)
		return;

	switch (Client.chase_mode)
	{
	case 0:
	case 1:
		Client.chase_mode++;
		Client.update_chase = true;
		return;
	};

	int i = Client.chase_target->State.GetNumber();
	CPlayerEntity *e;
	do {
		i++;
		if (i > game.maxclients)
			i = 1;
		e = dynamic_cast<CPlayerEntity*>(g_edicts[i].Entity);
		if (!e->IsInUse())
			continue;
		if (e->NoClip)
			continue;
		if (!e->Client.resp.spectator)
			break;
	} while (e != Client.chase_target);

	Client.chase_target = e;
	Client.chase_mode = 0;
	Client.update_chase = true;
}

void CPlayerEntity::ChasePrev()
{
	if (!Client.chase_target)
		return;

	int i = Client.chase_target->State.GetNumber();
	CPlayerEntity *e;
	do {
		i--;
		if (i < 1)
			i = game.maxclients;
		e = dynamic_cast<CPlayerEntity*>(g_edicts[i].Entity);
		if (!e->IsInUse())
			continue;
		if (e->NoClip)
			continue;
		if (!e->Client.resp.spectator)
			break;
	} while (e != Client.chase_target);

	Client.chase_target = e;
	Client.chase_mode = 0;
	Client.update_chase = true;
}

void CPlayerEntity::GetChaseTarget()
{
	for (int i = 1; i <= game.maxclients; i++)
	{
		CPlayerEntity *other = dynamic_cast<CPlayerEntity*>(g_edicts[i].Entity);
		if (other->IsInUse() && !other->Client.resp.spectator && !other->NoClip)
		{
			Client.chase_target = other;
			Client.update_chase = true;
			Client.chase_mode = 0;
			UpdateChaseCam();
			return;
		}
	}
	PrintToClient(PRINT_CENTER, "No other players to chase.");
}

void CPlayerEntity::P_ProjectSource (vec3f distance, vec3f &forward, vec3f &right, vec3f &result)
{
	switch (Client.pers.hand)
	{
	case LEFT_HANDED:
		distance.Y *= -1;
		break;
	case CENTER_HANDED:
		distance.Y = 0;
		break;
	default:
		break;
	}

	G_ProjectSource (State.GetOrigin(), distance, forward, right, result);
}

class CPlayerNoise : public virtual CBaseEntity
{
public:
	CPlayerNoise () :
	  CBaseEntity ()
	{
	};

	CPlayerNoise (int Index) :
	  CBaseEntity (Index)
	{
	};
};

#ifdef MONSTERS_USE_PATHFINDING
class CPathNode *GetClosestNodeTo (vec3f origin);
#endif

void CPlayerEntity::PlayerNoiseAt (vec3f Where, int type)
{
	if (type == PNOISE_WEAPON)
	{
		if (Client.silencer_shots)
		{
			Client.silencer_shots--;
			return;
		}
	}

	if (game.mode & GAME_DEATHMATCH)
		return;

	//if (who->flags & FL_NOTARGET)
	//	return;

#ifndef MONSTERS_USE_PATHFINDING
	if (!Client.mynoise)
	{
		CPlayerNoise *noise = QNew (com_levelPool, 0) CPlayerNoise;
		noise->gameEntity->classname = "player_noise";
		noise->SetMins (vec3f(-8, -8, -8));
		noise->SetMaxs (vec3f(8, 8, 8));
		noise->SetOwner (this);
		noise->SetSvFlags (SVF_NOCLIENT);
		Client.mynoise = noise;

		noise = QNew (com_levelPool, 0) CPlayerNoise;
		noise->gameEntity->classname = "player_noise";
		noise->SetMins (vec3f(-8, -8, -8));
		noise->SetMaxs (vec3f(8, 8, 8));
		noise->SetOwner (this);
		noise->SetSvFlags (SVF_NOCLIENT);
		Client.mynoise2 = noise;
	}

	CPlayerNoise *noise;
	if (type == PNOISE_SELF || type == PNOISE_WEAPON)
	{
		noise = dynamic_cast<CPlayerNoise*>(Client.mynoise);
		level.sound_entity = noise;
		level.sound_entity_framenum = level.framenum;
	}
	else // type == PNOISE_IMPACT
	{
		noise = dynamic_cast<CPlayerNoise*>(Client.mynoise2);
		level.sound2_entity = noise;
		level.sound2_entity_framenum = level.framenum;
	}

	noise->State.SetOrigin (Where);
	noise->SetAbsMin (Where - noise->GetMins());
	noise->SetAbsMax (Where + noise->GetMaxs());
	noise->gameEntity->teleport_time = level.framenum;
	noise->Link ();
#else
	level.NoiseNode = GetClosestNodeTo(Where);
	level.SoundEntityFramenum = level.framenum;
	level.SoundEntity = this;
#endif
}

void CPlayerEntity::BeginDeathmatch ()
{
	G_InitEdict (gameEntity);
	InitResp();

	// locate ent at a spawn point
	PutInServer();

	if (level.intermissiontime)
		MoveToIntermission();
	else
		// send effect
		CTempEnt::MuzzleFlash (State.GetOrigin (), State.GetNumber(), MZ_LOGIN);

	BroadcastPrintf (PRINT_HIGH, "%s entered the game\n", Client.pers.netname);

	// make sure all view stuff is valid
	EndServerFrame();
}

void CPlayerEntity::Begin ()
{
	gameEntity->client = game.clients + (State.GetNumber()-1);

	if (game.mode & GAME_DEATHMATCH)
	{
		BeginDeathmatch ();
		return;
	}

	// if there is already a body waiting for us (a loadgame), just
	// take it, otherwise spawn one from scratch
	if (IsInUse())
	{
		// the client has cleared the client side viewangles upon
		// connecting to the server, which is different than the
		// state when the game is saved, so we need to compensate
		// with deltaangles
		vec3f viewAngles = Client.PlayerState.GetViewAngles();
		for (int i=0 ; i<3 ; i++)
			Client.PlayerState.GetPMove()->deltaAngles[i] = ANGLE2SHORT(viewAngles[i]);
	}
	else
	{
		// a spawn point will completely reinitialize the entity
		// except for the persistant data that was initialized at
		// ClientConnect() time
		G_InitEdict (gameEntity);
		gameEntity->classname = "player";
		InitResp ();
		PutInServer ();
	}

	if (level.intermissiontime)
		MoveToIntermission ();
	else
	{
		// send effect if in a multiplayer game
		if (game.maxclients > 1)
		{
			CTempEnt::MuzzleFlash (State.GetOrigin(), State.GetNumber(), MZ_LOGIN);
			BroadcastPrintf (PRINT_HIGH, "%s entered the game\n", Client.pers.netname);
		}
	}

	// make sure all view stuff is valid
	EndServerFrame ();
}

IPAddress CopyIP (const char *val)
{
	// Do we have a :?
	std::string str (val);

	size_t loc = str.find_first_of (':');

	if (loc != std::string::npos)
		str = str.substr(0, loc);

	IPAddress Adr;
	if (str.length() > sizeof(Adr.str))
		assert (0);

	Q_snprintfz (Adr.str, sizeof(Adr.str), "%s", str.c_str());

	return Adr;
}

bool CPlayerEntity::Connect (char *userinfo)
{
	char	*value;

	// check to see if they are on the banned IP list
	value = Info_ValueForKey (userinfo, "ip");
	IPAddress Adr;

	Adr = CopyIP (value);
	if (Bans.IsBanned(Adr) || Bans.IsBanned(Info_ValueForKey(userinfo, "name")))
	{
		Info_SetValueForKey(userinfo, "rejmsg", "Connection refused.");
		return false;
	}

	// check for a spectator
	value = Info_ValueForKey (userinfo, "spectator");
	if ((game.mode & GAME_DEATHMATCH) && *value && strcmp(value, "0"))
	{
		int i, numspec;

		if (Bans.IsBannedFromSpectator(Adr) || Bans.IsBannedFromSpectator(Info_ValueForKey(userinfo, "name")))
		{
			Info_SetValueForKey(userinfo, "rejmsg", "Not permitted to enter spectator mode");
			return false;
		}
		if (*spectator_password->String() && 
			strcmp(spectator_password->String(), "none") && 
			strcmp(spectator_password->String(), value)) {
			Info_SetValueForKey(userinfo, "rejmsg", "Spectator password required or incorrect.");
			return false;
		}

		// count spectators
		for (i = numspec = 0; i < game.maxclients; i++)
		{
			CPlayerEntity *Ent = dynamic_cast<CPlayerEntity*>(g_edicts[i+1].Entity);
			if (Ent->IsInUse() && Ent->Client.pers.spectator)
				numspec++;
		}

		if (numspec >= game.maxspectators)
		{
			Info_SetValueForKey(userinfo, "rejmsg", "Server spectator limit is full.");
			return false;
		}
	}
	else
	{
		// check for a password
		value = Info_ValueForKey (userinfo, "password");
		if (*password->String() && strcmp(password->String(), "none") && 
			strcmp(password->String(), value))
		{
			Info_SetValueForKey(userinfo, "rejmsg", "Password required or incorrect.");
			return false;
		}
	}


	// they can connect
	gameEntity->client = game.clients + (State.GetNumber()-1);

	// if there is already a body waiting for us (a loadgame), just
	// take it, otherwise spawn one from scratch
	if (!IsInUse())
	{
		// clear the respawning variables
#ifdef CLEANCTF_ENABLED
//ZOID -- force team join
		Client.resp.ctf_team = -1;
		Client.resp.id_state = false; 
//ZOID
#endif
		InitResp ();
		if (!game.autosaved || !Client.pers.Weapon)
			InitPersistent();
	}

	UserinfoChanged (userinfo);
	Client.pers.IP = Adr;

	if (game.maxclients > 1)
	{
		// Tell the entire game that someone connected
		BroadcastPrintf (PRINT_MEDIUM, "%s connected\n", Client.pers.netname);
		
		// But only tell the server the IP
		DebugPrintf ("%s@%s connected\n", Client.pers.netname, Info_ValueForKey (userinfo, "ip"));
	}

	SetSvFlags (0); // make sure we start with known default
	Client.pers.state = SVCS_CONNECTED;
	return true;
}

void CPlayerEntity::Disconnect ()
{
	if (!gameEntity->client)
		return;

	Client.pers.state = SVCS_FREE;
	BroadcastPrintf (PRINT_HIGH, "%s disconnected\n", Client.pers.netname);

#ifdef CLEANCTF_ENABLED
//ZOID
	if (game.mode & GAME_CTF)
		CTFDeadDropFlag(this);
//ZOID
#endif

	if ((game.mode & GAME_CTF) || dmFlags.dfDmTechs) 
		DeadDropTech();

	// send effect
	CTempEnt::MuzzleFlash (State.GetOrigin(), State.GetNumber(), MZ_LOGIN);

	Unlink ();
	State.SetModelIndex (0);
	SetSolid (SOLID_NOT);
	SetInUse (false);
	gameEntity->classname = "disconnected";

	ConfigString (CS_PLAYERSKINS+(State.GetNumber()-1), "");
}

const char *MonsterAOrAn (const char *Name)
{
	static const char *_a = "a";
	static const char *_an = "an";

	switch (Q_tolower(Name[0]))
	{
	case 'a':
	case 'e':
	case 'i':
	case 'o':
	case 'u':
		return _an;
	default:
		return _a;
	};
}

void CPlayerEntity::Obituary (CBaseEntity *attacker)
{
	char *message = "", *message2 = "";
	if (attacker == this)
	{
		switch (meansOfDeath)
		{
		case MOD_HELD_GRENADE:
			message = "tried to put the pin back in";
			break;
		case MOD_HG_SPLASH:
		case MOD_G_SPLASH:
			switch (Client.resp.Gender)
			{
			case GenderMale:
				message = "tripped on his own grenade";
				break;
			case GenderFemale:
				message = "tripped on her own grenade";
				break;
			default:
				message = "tripped on its own grenade";
				break;
			}
			break;
		case MOD_R_SPLASH:
			switch (Client.resp.Gender)
			{
			case GenderMale:
				message = "blew himself up";
				break;
			case GenderFemale:
				message = "blew herself up";
				break;
			default:
				message = "blew itself up";
				break;
			}
			break;
		case MOD_BFG_BLAST:
			message = "should have used a smaller gun";
			break;
		default:
			switch (Client.resp.Gender)
			{
			case GenderMale:
				message = "killed himself";
				break;
			case GenderFemale:
				message = "killed herself";
				break;
			default:
				message = "killed himself";
				break;
			}
			break;
		}
		if (game.mode & GAME_DEATHMATCH)
			Client.resp.score--;
		BroadcastPrintf (PRINT_MEDIUM, "%s %s.\n", Client.pers.netname, message);
	}
	else if (attacker && (attacker->EntityFlags & ENT_PLAYER))
	{
		CPlayerEntity *Attacker = dynamic_cast<CPlayerEntity*>(attacker);
		bool endsInS = (Attacker->Client.pers.netname[strlen(Attacker->Client.pers.netname)] == 's');
		switch (meansOfDeath)
		{
		case MOD_BLASTER:
			message = "was blasted by";
			break;
		case MOD_SHOTGUN:
			message = "was gunned down by";
			break;
		case MOD_SSHOTGUN:
			message = "was blown away by";
			message2 = (endsInS) ? "' super shotgun" : "'s super shotgun";
			break;
		case MOD_MACHINEGUN:
			message = "was machinegunned by";
			break;
		case MOD_CHAINGUN:
			message = "was cut in half by";
			message2 = (endsInS) ? "' chaingun" : "'s chaingun";
			break;
		case MOD_GRENADE:
			message = "was popped by";
			message2 = (endsInS) ? "' grenade" : "'s grenade";
			break;
		case MOD_G_SPLASH:
			message = "was shredded by";
			message2 = (endsInS) ? "' shrapnel" : "'s shrapnel";
			break;
		case MOD_ROCKET:
			message = "ate";
			message2 = (endsInS) ? "' rocket" : "'s rocket";
			break;
		case MOD_R_SPLASH:
			message = "almost dodged";
			message2 = (endsInS) ? "' rocket" : "'s rocket";
			break;
		case MOD_HYPERBLASTER:
			message = "was melted by";
			message2 = (endsInS) ? "' hyperblaster" : "'s hyperblaster";
			break;
		case MOD_RAILGUN:
			message = "was railed by";
			break;
		case MOD_BFG_LASER:
			message = "saw the pretty lights from";
			message2 = (endsInS) ? "' BFG" : "'s BFG";
			break;
		case MOD_BFG_BLAST:
			message = "was disintegrated by";
			message2 = (endsInS) ? "' BFG blast" : "'s BFG blast";
			break;
		case MOD_BFG_EFFECT:
			message = "couldn't hide from";
			message2 = (endsInS) ? "' BFG" : "'s BFG";
			break;
		case MOD_HANDGRENADE:
			message = "caught";
			message2 = (endsInS) ? "' handgrenade" : "'s handgrenade";
			break;
		case MOD_HG_SPLASH:
			message = "didn't see";
			message2 = (endsInS) ? "' handgrenade" : "'s handgrenade";
			break;
		case MOD_HELD_GRENADE:
			message = "feels";
			message2 = (endsInS) ? "' pain" : "'s pain";
			break;
		case MOD_TELEFRAG:
			message = "tried to invade";
			message2 = (endsInS) ? "' personal space" : "'s personal space";
			break;
		case MOD_EXPLOSIVE:
		case MOD_BARREL:
			message = "was blown to smithereens by";
			break;
#ifdef CLEANCTF_ENABLED
//ZOID
		case MOD_GRAPPLE:
			message = "was caught by";
			message2 = "'s grapple";
			break;
//ZOID
#endif
		}
		BroadcastPrintf (PRINT_MEDIUM,"%s %s %s%s.\n", Client.pers.netname, message, Attacker->Client.pers.netname, message2);
		if (game.mode & GAME_DEATHMATCH)
			Attacker->Client.resp.score++;
	}
	else if (attacker && (attacker->EntityFlags & ENT_MONSTER))
	{
		switch (meansOfDeath)
		{
		case MOD_BLASTER:
			message = "was blasted by";
			break;
		case MOD_SHOTGUN:
			message = "was gunned down by";
			break;
		case MOD_SSHOTGUN:
			message = "was blown away by";
			message2 = "'s super shotgun";
			break;
		case MOD_MACHINEGUN:
			message = "was machinegunned by";
			break;
		case MOD_CHAINGUN:
			message = "was cut in half by";
			message2 = "'s chaingun";
			break;
		case MOD_GRENADE:
			message = "was popped by";
			message2 = "'s grenade";
			break;
		case MOD_G_SPLASH:
			message = "was shredded by";
			message2 = "'s shrapnel";
			break;
		case MOD_ROCKET:
			message = "ate";
			message2 = "'s rocket";
			break;
		case MOD_R_SPLASH:
			message = "almost dodged";
			message2 = "'s rocket";
			break;
		case MOD_HYPERBLASTER:
			message = "was melted by";
			message2 = "'s hyperblaster";
			break;
		case MOD_RAILGUN:
			message = "was railed by";
			break;
		case MOD_BFG_LASER:
			message = "saw the pretty lights from";
			message2 = "'s BFG";
			break;
		case MOD_BFG_BLAST:
			message = "was disintegrated by";
			message2 = "'s BFG blast";
			break;
		case MOD_BFG_EFFECT:
			message = "couldn't hide from";
			message2 = "'s BFG";
			break;
		case MOD_EXPLOSIVE:
		case MOD_BARREL:
			message = "was blown to smithereens by";
			break;
		default:
			message = "was killed by";
			break;
		};

		CMonsterEntity *Monster = dynamic_cast<CMonsterEntity*>(attacker);
		char *Name = Monster->Monster->MonsterName;

		if (game.mode & GAME_DEATHMATCH)
			Client.resp.score--;
		BroadcastPrintf (PRINT_MEDIUM, "%s %s %s %s%s.\n", Client.pers.netname, message, MonsterAOrAn(Name), Name, message2);
	}
	else
	{
		switch (meansOfDeath)
		{
		case MOD_SUICIDE:
			message = "suicides";
			break;
		case MOD_FALLING:
			message = "cratered";
			break;
		case MOD_CRUSH:
			message = "was squished";
			break;
		case MOD_WATER:
			message = "sank like a rock";
			break;
		case MOD_SLIME:
			message = "melted";
			break;
		case MOD_LAVA:
			message = "does a back flip into the lava";
			break;
		case MOD_EXPLOSIVE:
		case MOD_BARREL:
			message = "blew up";
			break;
		case MOD_EXIT:
			message = "found a way out";
			break;
		case MOD_TARGET_LASER:
			message = "saw the light";
			break;
		case MOD_TARGET_BLASTER:
			message = "got blasted";
			break;
		case MOD_BOMB:
		case MOD_SPLASH:
		case MOD_TRIGGER_HURT:
			message = "was in the wrong place";
			break;
		default:
			message = "died";
			break;
		}

		if (game.mode & GAME_DEATHMATCH)
			Client.resp.score--;
		BroadcastPrintf (PRINT_MEDIUM, "%s %s.\n", Client.pers.netname, message);
	}
}
