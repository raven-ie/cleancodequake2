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

void			CPlayerState::SetViewAngles (vec3f in)
{
	playerState->viewAngles[0] = in.X;
	playerState->viewAngles[1] = in.Y;
	playerState->viewAngles[2] = in.Z;
}
void			CPlayerState::SetViewOffset (vec3f in)
{
	playerState->viewOffset[0] = in.X;
	playerState->viewOffset[1] = in.Y;
	playerState->viewOffset[2] = in.Z;
}

void			CPlayerState::SetGunAngles (vec3f in)
{
	playerState->gunAngles[0] = in.X;
	playerState->gunAngles[1] = in.Y;
	playerState->gunAngles[2] = in.Z;
}
void			CPlayerState::SetGunOffset (vec3f in)
{
	playerState->gunOffset[0] = in.X;
	playerState->gunOffset[1] = in.Y;
	playerState->gunOffset[2] = in.Z;
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
}

// Players have a special way of allocating the entity.
// We won't automatically allocate it since it already exists
CPlayerEntity::CPlayerEntity (int Index) :
CBaseEntity(Index),
Client(gameEntity->client)
{
	EntityFlags |= ENT_PLAYER;
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
		Client.pers.spectator != Client.resp.spectator &&
		(level.time - Client.respawn_time) >= 5)
	{
		SpectatorRespawn();
		return;
	}

	// run weapon animations
	if (!Client.resp.spectator && Client.pers.Weapon)
			Client.pers.Weapon->Think (this);

	if (gameEntity->deadflag)
	{
		// wait for any button just going down
		if ( level.time > Client.respawn_time)
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

	// add player trail so monsters can follow
	if (!(game.mode & GAME_DEATHMATCH))
	{
		if (!visible (gameEntity, PlayerTrail_LastSpot() ) )
			PlayerTrail_Add (gameEntity->state.oldOrigin);
	}

	Client.latched_buttons = 0;
};

void CopyToBodyQue (edict_t *ent);
void CPlayerEntity::Respawn ()
{
	if (game.mode != GAME_SINGLEPLAYER)
	{
		// spectator's don't leave bodies
		if (gameEntity->movetype != MOVETYPE_NOCLIP)
			CopyToBodyQue (gameEntity);
		SetSvFlags (GetSvFlags() & ~SVF_NOCLIENT);
		PutInServer ();

		// add a teleportation effect
		gameEntity->state.event = EV_PLAYER_TELEPORT;

		// hold in place briefly
		Client.PlayerState.GetPMove()->pmFlags = PMF_TIME_TELEPORT;
		Client.PlayerState.GetPMove()->pmTime = 14;

		Client.respawn_time = level.time;
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
	int i, numspec;

	// if the user wants to become a spectator, make sure he doesn't
	// exceed max_spectators

	if (Client.pers.spectator)
	{
		char *value = Info_ValueForKey (Client.pers.userinfo, "spectator");
		if (*spectator_password->String() && 
			strcmp(spectator_password->String(), "none") && 
			strcmp(spectator_password->String(), value))
		{
			ClientPrintf(gameEntity, PRINT_HIGH, "Spectator password incorrect.\n");
			Client.pers.spectator = false;
			WriteByte (SVC_STUFFTEXT);
			WriteString ("spectator 0\n");
			Cast(CASTFLAG_RELIABLE, gameEntity);
			return;
		}

		// count spectators
		for (i = 1, numspec = 0; i <= game.maxclients; i++)
		{
			CPlayerEntity *Player = dynamic_cast<CPlayerEntity*>(g_edicts[i].Entity);
			if (Player->IsInUse() && Player->Client.pers.spectator)
				numspec++;
		}

		if (numspec >= game.maxspectators)
		{
			ClientPrintf(gameEntity, PRINT_HIGH, "Server spectator limit is full.");
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
			ClientPrintf(gameEntity, PRINT_HIGH, "Password incorrect.\n");
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
		CTempEnt::MuzzleFlash (gameEntity->state.origin, gameEntity-g_edicts, MZ_LOGIN);

		// hold in place briefly
		Client.PlayerState.GetPMove()->pmFlags = PMF_TIME_TELEPORT;
		Client.PlayerState.GetPMove()->pmTime = 14;
	}

	Client.respawn_time = level.time;

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
	vec3_t					mins = {-16, -16, -24};
	vec3_t					maxs = {16, 16, 32};
	int						index;
	vec3_t					spawn_origin, spawn_angles;
	int						i;
	clientPersistent_t		saved;
	clientRespawn_t			resp;

	// find a spawn point
	// do it before setting health back up, so farthest
	// ranging doesn't count this client
	SelectSpawnPoint (gameEntity, spawn_origin, spawn_angles);

	index = gameEntity-g_edicts-1;

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
	gameEntity->groundentity = NULL;
	// FIXME: is this needed?!
	Client = CClient(&game.clients[index]);
	//ent->client = &game.clients[index];
	gameEntity->takedamage = DAMAGE_AIM;
	gameEntity->movetype = MOVETYPE_WALK;
	gameEntity->viewheight = 22;
	SetInUse (true);
	gameEntity->classname = "player";
	gameEntity->mass = 200;
	SetSolid (SOLID_BBOX);
	gameEntity->deadflag = DEAD_NO;
	gameEntity->air_finished = level.time + 12;
	SetClipmask (CONTENTS_MASK_PLAYERSOLID);
	gameEntity->model = "players/male/tris.md2";
	gameEntity->pain = player_pain;
	gameEntity->die = player_die;
	gameEntity->waterlevel = 0;
	gameEntity->watertype = 0;
	gameEntity->flags &= ~FL_NO_KNOCKBACK;
	SetSvFlags (GetSvFlags() & ~SVF_DEADMONSTER);
	if (!Client.resp.MenuState.ent)
		Client.resp.MenuState = CMenuState(this);

	SetMins (mins);
	SetMaxs (maxs);
	Vec3Clear (gameEntity->velocity);

	// clear playerstate values
	Client.PlayerState.Clear ();

	Client.PlayerState.GetPMove()->origin[0] = spawn_origin[0]*8;
	Client.PlayerState.GetPMove()->origin[1] = spawn_origin[1]*8;
	Client.PlayerState.GetPMove()->origin[2] = spawn_origin[2]*8;
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
	gameEntity->state.effects = 0;
	gameEntity->state.modelIndex = 255;		// will use the skin specified model
	gameEntity->state.modelIndex2 = 255;		// custom gun model
	// sknum is player num and weapon number
	// weapon number will be added in changeweapon
	gameEntity->state.skinNum = gameEntity - g_edicts - 1;

	gameEntity->state.frame = 0;
	Vec3Copy (spawn_origin, gameEntity->state.origin);
	gameEntity->state.origin[2] += 1;	// make sure off ground
	Vec3Copy (gameEntity->state.origin, gameEntity->state.oldOrigin);

	// set the delta angle
	for (i=0 ; i<3 ; i++)
		Client.PlayerState.GetPMove()->deltaAngles[i] = ANGLE2SHORT(spawn_angles[i] - Client.resp.cmd_angles[i]);

	gameEntity->state.angles[PITCH] = 0;
	gameEntity->state.angles[YAW] = spawn_angles[YAW];
	gameEntity->state.angles[ROLL] = 0;
	Client.PlayerState.SetViewAngles (gameEntity->state.angles);
	Vec3Copy (gameEntity->state.angles, Client.v_angle);

#ifdef CLEANCTF_ENABLED
//ZOID
	if (CTFStart())
		return;
//ZOID
#endif

	// spawn a spectator
	if (Client.pers.spectator)
	{
		Client.chase_target = NULL;

		Client.resp.spectator = true;

		gameEntity->movetype = MOVETYPE_NOCLIP;
		SetSolid (SOLID_NOT);
		SetSvFlags (GetSvFlags() | SVF_NOCLIENT);
		Client.PlayerState.SetGunIndex (0);
		Link ();
		return;
	}
	else
		Client.resp.spectator = false;

	if (!KillBox (gameEntity))
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

	FindItem("Blaster")->Add(this, 1);
	Client.pers.Weapon = &WeaponBlaster;
	Client.pers.LastWeapon = Client.pers.Weapon;
	Client.pers.Inventory.SelectedItem =Client.pers.Weapon->Item->GetIndex();

#ifdef CLEANCTF_ENABLED
	if (game.mode & GAME_CTF)
		FindItem("Grapple")->Add(this, 1);
	Client.pers.Flag = NULL;
	Client.pers.Tech = NULL;
#endif

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
	playernum = gameEntity-g_edicts-1;

	// combine name and skin into a configstring
#ifdef CLEANCTF_ENABLED
//ZOID
	if (game.mode & GAME_CTF)
		CTFAssignSkin(s);
	else
//ZOID
#endif
		gi.configstring (CS_PLAYERSKINS+playernum, Q_VarArgs ("%s\\%s", Client.pers.netname, s) );

	// fov
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

	// handedness
	s = Info_ValueForKey (userinfo, "hand");
	if (strlen(s))
		Client.pers.hand = atoi(s);

	// IP
	s = Info_ValueForKey (userinfo, "ip");
	if (strlen(s))
		Client.pers.IP = IPStringToArrays(s);

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
	int playernum = gameEntity-g_edicts-1;
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
		gi.configstring (CS_PLAYERSKINS+playernum, Q_VarArgs("%s\\%s%s", 
			Client.pers.netname, t, CTF_TEAM1_SKIN) );
		break;
	case CTF_TEAM2:
		gi.configstring (CS_PLAYERSKINS+playernum,
			Q_VarArgs("%s\\%s%s", Client.pers.netname, t, CTF_TEAM2_SKIN) );
		break;
	default:
		gi.configstring (CS_PLAYERSKINS+playernum, 
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
		gameEntity->movetype = MOVETYPE_NOCLIP;
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
	gameEntity->health = Client.pers.health;
	gameEntity->max_health = Client.pers.max_health;
	gameEntity->flags |= Client.pers.savedFlags;
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
inline float CPlayerEntity::CalcRoll (vec3_t angles, vec3_t velocity, vec3_t right)
{
	float	side = fabs(DotProduct (velocity, right));
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
inline void CPlayerEntity::DamageFeedback (vec3_t forward, vec3_t right, vec3_t up)
{
	static	const colorb	PowerColor = colorb(0, 255, 0, 0);
	static	const colorb	ArmorColor = colorb(255, 255, 255, 0);
	static	const colorb	BloodColor = colorb(255, 0, 0, 0);

	// flash the backgrounds behind the status numbers
	Client.PlayerState.SetStat(STAT_FLASHES, 0);
	if (Client.damage_blood)
		Client.PlayerState.SetStat (STAT_FLASHES, Client.PlayerState.GetStat(STAT_FLASHES) | 1);
	if (Client.damage_armor && !(gameEntity->flags & FL_GODMODE) && (Client.invincible_framenum <= level.framenum))
		Client.PlayerState.SetStat (STAT_FLASHES, Client.PlayerState.GetStat(STAT_FLASHES) | 2);

	// total points of damage shot at the player this frame
	int count = (Client.damage_blood + Client.damage_armor + Client.damage_parmor);
	if (count == 0)
		return;		// didn't take any damage

	// start a pain animation if still in the player model
	if (Client.anim_priority < ANIM_PAIN && gameEntity->state.modelIndex == 255)
	{
		Client.anim_priority = ANIM_PAIN;
		if (Client.PlayerState.GetPMove()->pmFlags & PMF_DUCKED)
		{
			gameEntity->state.frame = FRAME_crpain1-1;
			Client.anim_end = FRAME_crpain4;
		}
		else
		{
			switch (rand()%3)
			{
			case 0:
				gameEntity->state.frame = FRAME_pain101-1;
				Client.anim_end = FRAME_pain104;
				break;
			case 1:
				gameEntity->state.frame = FRAME_pain201-1;
				Client.anim_end = FRAME_pain204;
				break;
			case 2:
				gameEntity->state.frame = FRAME_pain301-1;
				Client.anim_end = FRAME_pain304;
				break;
			}
		}
	}

	int realcount = count;
	if (count < 10)
		count = 10;	// always make a visible effect

	// play an apropriate pain sound
	if ((level.time > gameEntity->pain_debounce_time) && !(gameEntity->flags & FL_GODMODE) && (Client.invincible_framenum <= level.framenum))
	{
		gameEntity->pain_debounce_time = level.time + 0.7;

		int l = clamp(((floorf((max(0, gameEntity->health-1)) / 25))), 0, 3);
		PlaySoundFrom (gameEntity, CHAN_VOICE, gMedia.Player.Pain[l][(rand()&1)]);
	}

	// the total alpha of the blend is always proportional to count
	byte Alpha = Client.damage_blend.A + count*3;
	if (Alpha < 51)
		Alpha = 51;
	if (Alpha > 153)
		Alpha = 153;		// don't go too saturated

	// the color of the blend will vary based on how much was absorbed
	// by different armors
	Client.damage_blend.Set (
							(BloodColor.R * (Client.damage_blood/realcount)) + (ArmorColor.R * (Client.damage_armor/realcount)) + (PowerColor.R * (Client.damage_parmor/realcount)),
							(BloodColor.G * (Client.damage_blood/realcount)) + (ArmorColor.G * (Client.damage_armor/realcount)) + (PowerColor.G * (Client.damage_parmor/realcount)),
							(BloodColor.B * (Client.damage_blood/realcount)) + (ArmorColor.B * (Client.damage_armor/realcount)) + (PowerColor.B * (Client.damage_parmor/realcount)),
							Alpha);

	//
	// calculate view angle kicks
	//
	float kick = abs(Client.damage_knockback);
	if (kick && (gameEntity->health > 0))	// kick of 0 means no view adjust at all
	{
		vec3_t v;
		float side;

		kick = kick * 100 / gameEntity->health;

		if (kick < count*0.5)
			kick = count*0.5;
		if (kick > 50)
			kick = 50;

		Vec3Subtract (Client.damage_from, gameEntity->state.origin, v);
		VectorNormalizef (v, v);
		
		side = DotProduct (v, right);
		Client.v_dmg_roll = kick*side*0.3;
		
		side = -DotProduct (v, forward);
		Client.v_dmg_pitch = kick*side*0.3;

		Client.v_dmg_time = level.time + DAMAGE_TIME;
	}

	//
	// clear totals
	//
	Client.damage_blood = 0;
	Client.damage_armor = 0;
	Client.damage_parmor = 0;
	Client.damage_knockback = 0;
}

/*
===============
SV_CalcViewOffset
===============
*/
inline void CPlayerEntity::CalcViewOffset (vec3_t forward, vec3_t right, vec3_t up, float xyspeed)
{
	float		bob;
	float		ratio;
	float		delta;
	vec3_t		v = {0,0,0};
	vec3_t		angles = {0,0,0};

	// if dead, fix the angle and don't add any kick
	if (gameEntity->deadflag)
		Client.PlayerState.SetViewAngles (vec3f(40, -15, Client.killer_yaw));
	else
	{
		// Base angles
		Client.PlayerState.GetKickAngles(angles);

		// add angles based on damage kick
		ratio = (Client.v_dmg_time - level.time) / DAMAGE_TIME;
		if (ratio < 0)
		{
			ratio = 0;
			Client.v_dmg_pitch = 0;
			Client.v_dmg_roll = 0;
		}
		angles[PITCH] += ratio * Client.v_dmg_pitch;
		angles[ROLL] += ratio * Client.v_dmg_roll;

		// add pitch based on fall kick
		ratio = (Client.fall_time - level.time) / FALL_TIME;
		if (ratio < 0)
			ratio = 0;
		angles[PITCH] += ratio * Client.fall_value;

		// add angles based on velocity
		delta = DotProduct (gameEntity->velocity, forward);
		angles[PITCH] += delta*run_pitch->Float();
		
		delta = DotProduct (gameEntity->velocity, right);
		angles[ROLL] += delta*run_roll->Float();

		// add angles based on bob

		delta = bobfracsin * bob_pitch->Float() * xyspeed;
		if (Client.PlayerState.GetPMove()->pmFlags & PMF_DUCKED)
			delta *= 6;		// crouching
		angles[PITCH] += delta;
		delta = bobfracsin * bob_roll->Float() * xyspeed;
		if (Client.PlayerState.GetPMove()->pmFlags & PMF_DUCKED)
			delta *= 6;		// crouching
		if (bobcycle & 1)
			delta = -delta;
		angles[ROLL] += delta;
	}

//===================================

	// add view height
	v[2] += gameEntity->viewheight;

	// add fall height
	ratio = (Client.fall_time - level.time) / FALL_TIME;
	if (ratio < 0)
		ratio = 0;
	v[2] -= ratio * Client.fall_value * 0.4;

	// add bob height
	bob = bobfracsin * xyspeed * bob_up->Float();
	if (bob > 6)
		bob = 6;
	v[2] += bob;

	// add kick offset
	Vec3Add (v, Client.kick_origin, v);

	// absolutely bound offsets
	// so the view can never be outside the player box
	if (v[0] < -14)
		v[0] = -14;
	else if (v[0] > 14)
		v[0] = 14;
	if (v[1] < -14)
		v[1] = -14;
	else if (v[1] > 14)
		v[1] = 14;
	if (v[2] < -22)
		v[2] = -22;
	else if (v[2] > 30)
		v[2] = 30;

	Client.PlayerState.SetViewOffset(v);
}

/*
==============
SV_CalcGunOffset
==============
*/
inline void CPlayerEntity::CalcGunOffset (vec3_t forward, vec3_t right, vec3_t up, float xyspeed)
{
	int		i;
	float	delta;
	vec3_t	angles, gunAngles;

	// gun angles from bobbing
	Client.PlayerState.GetGunAngles (gunAngles);
	angles[ROLL] = (bobcycle & 1) ? (-gunAngles[ROLL]) : (xyspeed * bobfracsin * 0.005);
	angles[YAW] = (bobcycle & 1) ? (-gunAngles[YAW]) : (xyspeed * bobfracsin * 0.01);
	angles[PITCH] = xyspeed * bobfracsin * 0.005;

	// gun angles from delta movement
	for (i=0 ; i<3 ; i++)
	{
		delta = Client.oldviewangles[i] - angles[i];
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
//	ent->ps->gunorigin[2] += bob;

	Vec3Clear (angles);

	// gun_x / gun_y / gun_z are development tools
	for (i=0 ; i<3 ; i++)
	{
		angles[i] += forward[i] *gun_y->Float();
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
static inline void SV_AddBlend (colorb color, colorb &v_blend)
{
	byte	a2, a3;

	if (color.A <= 0)
		return;

	a2 = v_blend.A + color.A;	// new total alpha
	a3 = v_blend.A/a2;		// fraction of color from old

	v_blend.R = v_blend.R*a3 + color.R;
	v_blend.G = v_blend.G*a3 + color.G;
	v_blend.B = v_blend.B*a3 + color.B;
	v_blend.A = a2;
}


/*
=============
SV_CalcBlend
=============
*/
static const colorb LavaColor = colorb(255, 76, 0, 153);
static const colorb SlimeColor = colorb(0, 25, 13, 153);
static const colorb WaterColor = colorb(127, 76, 51, 102);
static const colorb QuadColor = colorb(0, 0, 255, 20);
static const colorb InvulColor = colorb(255, 255, 0, 20);
static const colorb EnviroColor = colorb(0, 255, 0, 20);
static const colorb BreatherColor = colorb(102, 255, 102, 10);
static const colorb ClearColor = colorb(0,0,0,0);
static colorb BonusColor = colorb(217, 178, 76, 0);

inline void CPlayerEntity::CalcBlend ()
{
	// add for contents
	vec3_t	vieworg, vOff;
	Client.PlayerState.GetViewOffset(vOff);
	Vec3Add (gameEntity->state.origin, vOff, vieworg);

	int contents = gi.pointcontents (vieworg);

	if (contents & (CONTENTS_LAVA|CONTENTS_SLIME|CONTENTS_WATER) )
		Client.PlayerState.SetRdFlags (Client.PlayerState.GetRdFlags() | RDF_UNDERWATER);
	else
		Client.PlayerState.SetRdFlags (Client.PlayerState.GetRdFlags() & ~RDF_UNDERWATER);

	// add for powerups
	if (Client.quad_framenum > level.framenum)
	{
		int remaining = Client.quad_framenum - level.framenum;

		if (remaining == 30)	// beginning to fade
			PlaySoundFrom(gameEntity, CHAN_ITEM, SoundIndex("items/damage2.wav"));

		if (remaining > 30 || (remaining & 4) )
			SV_AddBlend (QuadColor, Client.pers.viewBlend);
	}
	else if (Client.invincible_framenum > level.framenum)
	{
		int remaining = Client.invincible_framenum - level.framenum;

		if (remaining == 30)	// beginning to fade
			PlaySoundFrom(gameEntity, CHAN_ITEM, SoundIndex("items/protect2.wav"));

		if (remaining > 30 || (remaining & 4) )
			SV_AddBlend (InvulColor, Client.pers.viewBlend);
	}
	else if (Client.enviro_framenum > level.framenum)
	{
		int remaining = Client.enviro_framenum - level.framenum;

		if (remaining == 30)	// beginning to fade
			PlaySoundFrom(gameEntity, CHAN_ITEM, SoundIndex("items/airout.wav"));

		if (remaining > 30 || (remaining & 4) )
			SV_AddBlend (EnviroColor, Client.pers.viewBlend);
	}
	else if (Client.breather_framenum > level.framenum)
	{
		int remaining = Client.breather_framenum - level.framenum;

		if (remaining == 30)	// beginning to fade
			PlaySoundFrom(gameEntity, CHAN_ITEM, SoundIndex("items/airout.wav"));

		if (remaining > 30 || (remaining & 4) )
			SV_AddBlend (BreatherColor, Client.pers.viewBlend);
	}

	// add for damage
	if (Client.damage_blend.A)
		SV_AddBlend (Client.damage_blend, Client.pers.viewBlend);

	// drop the damage value
	if (Client.damage_blend.A)
	{
		if (Client.damage_blend.A < 15)
			Client.damage_blend.A = 0;
		else
			Client.damage_blend.A -= 15;
	}

	// add bonus and drop the value
	if (Client.bonus_alpha)
	{
		BonusColor.A = Client.bonus_alpha;
		SV_AddBlend (BonusColor, Client.pers.viewBlend);

		if (Client.bonus_alpha < 15)
			Client.bonus_alpha = 0;
		else
			Client.bonus_alpha -= 15;
	}

	if (contents & (CONTENTS_SOLID|CONTENTS_LAVA))
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

	if (gameEntity->state.modelIndex != 255)
		return;		// not in the player model

	if (gameEntity->movetype == MOVETYPE_NOCLIP)
		return;

#ifdef CLEANCTF_ENABLED
//ZOID
	// never take damage if just release grapple or on grapple
	if (level.time - Client.ctf_grapplereleasetime <= FRAMETIME * 2 ||
		(Client.ctf_grapple && 
		Client.ctf_grapplestate > CTF_GRAPPLE_STATE_FLY))
		return;
//ZOID
#endif
	float	delta;
	int		damage;
	vec3_t	dir;

	if ((Client.oldvelocity[2] < 0) && (gameEntity->velocity[2] > Client.oldvelocity[2]) && (!gameEntity->groundentity))
		delta = Client.oldvelocity[2];
	else
	{
		if (!gameEntity->groundentity)
			return;
		delta = gameEntity->velocity[2] - Client.oldvelocity[2];
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
		gameEntity->state.event = EV_FOOTSTEP;
		return;
	}

	Client.fall_value = delta*0.5;
	if (Client.fall_value > 40)
		Client.fall_value = 40;
	Client.fall_time = level.time + FALL_TIME;

	if (delta > 30)
	{
		if (gameEntity->health > 0)
		{
			if (delta >= 55)
				gameEntity->state.event = EV_FALLFAR;
			else
				gameEntity->state.event = EV_FALL;
		}
		gameEntity->pain_debounce_time = level.time;	// no normal pain sound
		damage = (delta-30)/2;
		if (damage < 1)
			damage = 1;
		Vec3Set (dir, 0, 0, 1);

		if (!dmFlags.dfNoFallingDamage )
			T_Damage (gameEntity, world, world, dir, gameEntity->state.origin, vec3Origin, damage, 0, 0, MOD_FALLING);
	}
	else
	{
		gameEntity->state.event = EV_FALLSHORT;
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

	if (gameEntity->movetype == MOVETYPE_NOCLIP)
	{
		gameEntity->air_finished = level.time + 12;	// don't need air
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
		PlayerNoise(this, gameEntity->state.origin, PNOISE_SELF);
		if (gameEntity->watertype & CONTENTS_LAVA)
			PlaySoundFrom (gameEntity, CHAN_BODY, SoundIndex("player/lava_in.wav"));
		else if (gameEntity->watertype & CONTENTS_SLIME)
			PlaySoundFrom (gameEntity, CHAN_BODY, SoundIndex("player/watr_in.wav"));
		else if (gameEntity->watertype & CONTENTS_WATER)
			PlaySoundFrom (gameEntity, CHAN_BODY, SoundIndex("player/watr_in.wav"));
		gameEntity->flags |= FL_INWATER;

		// clear damage_debounce, so the pain sound will play immediately
		gameEntity->pain_debounce_time = level.time - 1;
	}

	//
	// if just completely exited a water volume, play a sound
	//
	if (old_waterlevel && ! waterlevel)
	{
		PlayerNoise(this, gameEntity->state.origin, PNOISE_SELF);
		PlaySoundFrom (gameEntity, CHAN_BODY, SoundIndex("player/watr_out.wav"));
		gameEntity->flags &= ~FL_INWATER;
	}

	//
	// check for head just going under water
	//
	if (old_waterlevel != 3 && waterlevel == 3)
		PlaySoundFrom (gameEntity, CHAN_BODY, SoundIndex("player/watr_un.wav"));

	//
	// check for head just coming out of water
	//
	if (old_waterlevel == 3 && waterlevel != 3)
	{
		if (gameEntity->air_finished < level.time)
		{	// gasp for air
			PlaySoundFrom (gameEntity, CHAN_VOICE, SoundIndex("player/gasp1.wav"));
			PlayerNoise(this, gameEntity->state.origin, PNOISE_SELF);
		}
		else  if (gameEntity->air_finished < level.time + 11) // just break surface
			PlaySoundFrom (gameEntity, CHAN_VOICE, SoundIndex("player/gasp2.wav"));
	}

	//
	// check for drowning
	//
	if (waterlevel == 3)
	{
		// breather or envirosuit give air
		if (breather || envirosuit)
		{
			gameEntity->air_finished = level.time + 10;

			if (((int)(Client.breather_framenum - level.framenum) % 25) == 0)
			{
				PlaySoundFrom (gameEntity, CHAN_AUTO, SoundIndex((!Client.breather_sound) ? "player/u_breath1.wav" : "player/u_breath2.wav"));
				Client.breather_sound = !Client.breather_sound;
				PlayerNoise(this, gameEntity->state.origin, PNOISE_SELF);
			}
		}

		// if out of air, start drowning
		if (gameEntity->air_finished < level.time)
		{	// drown!
			if (Client.next_drown_time < level.time 
				&& gameEntity->health > 0)
			{
				Client.next_drown_time = level.time + 1;

				// take more damage the longer underwater
				gameEntity->dmg += 2;
				if (gameEntity->dmg > 15)
					gameEntity->dmg = 15;

				// play a gurp sound instead of a normal pain sound
				if (gameEntity->health <= gameEntity->dmg)
					PlaySoundFrom (gameEntity, CHAN_VOICE, SoundIndex("player/drown1.wav"));
				else
					PlaySoundFrom (gameEntity, CHAN_VOICE, gMedia.Player.Gurp[(rand()&1)]);

				gameEntity->pain_debounce_time = level.time;

				T_Damage (gameEntity, world, world, vec3Origin, gameEntity->state.origin, vec3Origin, gameEntity->dmg, 0, DAMAGE_NO_ARMOR, MOD_WATER);
			}
		}
	}
	else
	{
		gameEntity->air_finished = level.time + 12;
		gameEntity->dmg = 2;
	}

	//
	// check for sizzle damage
	//
	if (waterlevel && (gameEntity->watertype&(CONTENTS_LAVA|CONTENTS_SLIME)) )
	{
		if (gameEntity->watertype & CONTENTS_LAVA)
		{
			if (gameEntity->health > 0
				&& gameEntity->pain_debounce_time <= level.time
				&& Client.invincible_framenum < level.framenum)
			{
				PlaySoundFrom (gameEntity, CHAN_VOICE, SoundIndex((rand()&1) ? "player/burn1.wav" : "player/burn2.wav"));
				gameEntity->pain_debounce_time = level.time + 1;
			}

			// take 1/3 damage with envirosuit
			T_Damage (gameEntity, world, world, vec3Origin, gameEntity->state.origin, vec3Origin, (envirosuit) ? 1*waterlevel : 3*waterlevel, 0, 0, MOD_LAVA);
		}

		if (gameEntity->watertype & CONTENTS_SLIME)
		{
			if (!envirosuit) // no damage from slime with envirosuit
				T_Damage (gameEntity, world, world, vec3Origin, gameEntity->state.origin, vec3Origin, 1*waterlevel, 0, 0, MOD_SLIME);
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
	if (!(gameEntity->flags & FL_POWER_ARMOR))
		return POWER_ARMOR_NONE;
	else if (Client.pers.Inventory.Has(FindItem("Power Shield")) > 0)
		return POWER_ARMOR_SHIELD;
	else if (Client.pers.Inventory.Has(FindItem("Power Screen")) > 0)
		return POWER_ARMOR_SCREEN;

	return POWER_ARMOR_NONE;
}

inline void CPlayerEntity::SetClientEffects ()
{
	gameEntity->state.effects = 0;
	gameEntity->state.renderFx = 0;

	if (gameEntity->health <= 0 || level.intermissiontime)
		return;

	if (gameEntity->powerarmor_time > level.time)
	{
		int pa_type = PowerArmorType ();
		if (pa_type == POWER_ARMOR_SCREEN)
			gameEntity->state.effects |= EF_POWERSCREEN;
		else if (pa_type == POWER_ARMOR_SHIELD)
		{
			gameEntity->state.effects |= EF_COLOR_SHELL;
			gameEntity->state.renderFx |= RF_SHELL_GREEN;
		}
	}

#ifdef CLEANCTF_ENABLED
	gameEntity->state.effects &= ~(EF_FLAG1 | EF_FLAG2);
	if (Client.pers.Flag)
	{
		if (gameEntity->health > 0)
			gameEntity->state.effects |= Client.pers.Flag->EffectFlags;
		gameEntity->state.modelIndex3 = ModelIndex(Client.pers.Flag->WorldModel);
	}
	else
		gameEntity->state.modelIndex3 = 0;
#endif

	if (Client.quad_framenum > level.framenum)
	{
		int remaining = Client.quad_framenum - level.framenum;
		if (remaining > 30 || (remaining & 4) )
			gameEntity->state.effects |= EF_QUAD;
	}

	if (Client.invincible_framenum > level.framenum)
	{
		int remaining = Client.invincible_framenum - level.framenum;
		if (remaining > 30 || (remaining & 4) )
			gameEntity->state.effects |= EF_PENT;
	}

	// show cheaters!!!
	if (gameEntity->flags & FL_GODMODE)
	{
		gameEntity->state.effects |= EF_COLOR_SHELL;
		gameEntity->state.renderFx |= (RF_SHELL_RED|RF_SHELL_GREEN|RF_SHELL_BLUE);
	}
}


/*
===============
G_SetClientEvent
===============
*/
inline void CPlayerEntity::SetClientEvent (float xyspeed)
{
	if (gameEntity->state.event)
		return;

	if ( gameEntity->groundentity && xyspeed > 225)
	{
		if ( (int)(Client.bobtime+bobmove) != bobcycle )
			gameEntity->state.event = EV_FOOTSTEP;
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
		PlaySoundFrom (gameEntity, CHAN_VOICE, SoundIndex ("misc/pc_up.wav"), 1, ATTN_STATIC);
	}

	if (gameEntity->waterlevel && (gameEntity->watertype & (CONTENTS_LAVA|CONTENTS_SLIME)))
		gameEntity->state.sound = gMedia.FrySound;
	else if (Client.pers.Weapon && Client.pers.Weapon->WeaponSoundIndex)
		gameEntity->state.sound = Client.pers.Weapon->WeaponSoundIndex;
	else if (Client.weapon_sound)
		gameEntity->state.sound = Client.weapon_sound;
	else
		gameEntity->state.sound = 0;
}

/*
===============
G_SetClientFrame
===============
*/
inline void CPlayerEntity::SetClientFrame (float xyspeed)
{
	bool	duck = (Client.PlayerState.GetPMove()->pmFlags & PMF_DUCKED);
	bool	run = !(xyspeed == 0);

	if (gameEntity->state.modelIndex != 255)
		return;		// not in the player model

	// check for stand/duck and stop/go transitions
	if ((duck != Client.anim_duck && Client.anim_priority < ANIM_DEATH) || 
		(run != Client.anim_run && Client.anim_priority == ANIM_BASIC) ||
		(!gameEntity->groundentity && Client.anim_priority <= ANIM_WAVE))
	{
		// return to either a running or standing frame
		Client.anim_priority = ANIM_BASIC;
		Client.anim_duck = duck;
		Client.anim_run = run;

		if (!gameEntity->groundentity && !duck)
		{
#ifdef CLEANCTF_ENABLED
	//ZOID: if on grapple, don't go into jump frame, go into standing frame
			if (Client.ctf_grapple)
			{
				gameEntity->state.frame = FRAME_stand01;
				Client.anim_end = FRAME_stand40;
			}
			else
			{
#endif
				Client.anim_priority = ANIM_JUMP;
				if (gameEntity->state.frame != FRAME_jump2)
					gameEntity->state.frame = FRAME_jump1;
				Client.anim_end = FRAME_jump2;
#ifdef CLEANCTF_ENABLED
			}
#endif
		}
		else if (run)
		{
			// Running
			gameEntity->state.frame = (duck) ? FRAME_crwalk1 : FRAME_run1;
			Client.anim_end = (duck) ? FRAME_crwalk6 : FRAME_run6;
		}
		else
		{
			// Standing
			gameEntity->state.frame = (duck) ? FRAME_crstnd01 : FRAME_stand01;
			Client.anim_end = (duck) ? FRAME_crstnd19 : FRAME_stand40;
		}
	}
	else
	{
		if(Client.anim_priority == ANIM_REVERSE)
		{
			if(gameEntity->state.frame > Client.anim_end)
			{
				gameEntity->state.frame--;
				return;
			}
		}
		else if (gameEntity->state.frame < Client.anim_end)
		{	// continue an animation
			gameEntity->state.frame++;
			return;
		}

		if (Client.anim_priority == ANIM_DEATH)
			return;		// stay there
		if (Client.anim_priority == ANIM_JUMP)
		{
			if (!gameEntity->groundentity)
				return;		// stay there
			Client.anim_priority = ANIM_WAVE;
			gameEntity->state.frame = FRAME_jump3;
			Client.anim_end = FRAME_jump6;
			return;
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
	for (i=0 ; i<3 ; i++)
	{
		Client.PlayerState.GetPMove()->origin[i] = gameEntity->state.origin[i]*8.0;
		Client.PlayerState.GetPMove()->velocity[i] = gameEntity->velocity[i]*8.0;
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

		blend[4] = 0;
		Client.PlayerState.SetViewBlend (blend);

		Client.PlayerState.SetFov (90);
		SetStats ();
		return;
	}

	vec3_t forward, right, up;
	Angles_Vectors (Client.v_angle, forward, right, up);

	// burn from lava, etc
	WorldEffects ();

	//
	// set model angles from view angles so other things in
	// the world can tell which direction you are looking
	//
	if (Client.v_angle[PITCH] > 180)
		gameEntity->state.angles[PITCH] = (-360 + Client.v_angle[PITCH])/3;
	else
		gameEntity->state.angles[PITCH] = Client.v_angle[PITCH]/3;
	gameEntity->state.angles[YAW] = Client.v_angle[YAW];
	gameEntity->state.angles[ROLL] = 0;
	gameEntity->state.angles[ROLL] = CalcRoll (gameEntity->state.angles, gameEntity->velocity, right)*4;

	//
	// calculate speed and cycle to be used for
	// all cyclic walking effects
	//
	float xyspeed = sqrtf(gameEntity->velocity[0]*gameEntity->velocity[0] + gameEntity->velocity[1]*gameEntity->velocity[1]);

	if (xyspeed < 5 || Client.PlayerState.GetPMove()->pmFlags & PMF_DUCKED)
	{
		bobmove = 0;
		Client.bobtime = 0;	// start at beginning of cycle again
	}
	else if (gameEntity->groundentity)
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
	bobfracsin = fabs(sinf(bobtime*M_PI));

	// detect hitting the floor
	FallingDamage ();

	// apply all the damage taken this frame
	DamageFeedback (forward, right, up);

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

	Vec3Copy (gameEntity->velocity, Client.oldvelocity);

	vec3_t viewAngles;
	Client.PlayerState.GetViewAngles(viewAngles);
	Vec3Copy (viewAngles, Client.oldviewangles);

	// clear weapon kicks
	Vec3Clear (Client.kick_origin);
	Vec3Clear (Client.kick_angles);

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
	int					len;
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

			Bar.AddClientBlock (0, 42 + i * 8, sorted[0][i], cl_ent->Client.resp.score, clamp(cl_ent->Client.GetPing(), 0, 999));

			if (cl_ent->Client.pers.Flag == BlueFlag)
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

			Bar.AddClientBlock (160, 42 + i * 8, sorted[1][i], cl_ent->Client.resp.score, clamp(cl_ent->Client.GetPing(), 0, 999));

			if (cl_ent->Client.pers.Flag == RedFlag)
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

			Bar.AddClientBlock ((n & 1) ? 160 : 0, j, i, cl_ent->Client.resp.score, clamp(cl_ent->Client.GetPing(), 0, 999));
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
	CPlayerEntity		*Killer = (gameEntity->enemy) ? dynamic_cast<CPlayerEntity*>(gameEntity->enemy->Entity) : NULL;

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

	Scoreboard.SendMsg (this->gameEntity, reliable);
}

void CPlayerEntity::SetStats ()
{
	//
	// health
	//
	Client.PlayerState.SetStat(STAT_HEALTH_ICON, gMedia.Hud.HealthPic);
	Client.PlayerState.SetStat(STAT_HEALTH, gameEntity->health);

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
		cells = Client.pers.Inventory.Has(FindItem("Cells")->GetIndex());
		if (cells == 0)
		{	// ran out of cells for power armor
			gameEntity->flags &= ~FL_POWER_ARMOR;
			PlaySoundFrom(gameEntity, CHAN_ITEM, SoundIndex("misc/power2.wav"));
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
	if (level.time > Client.pickup_msg_time)
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
		(((game.mode & GAME_DEATHMATCH) && (level.intermissiontime || Client.showscores)) || 
		(!(game.mode & GAME_DEATHMATCH)) && Client.showhelp))
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

#ifdef CLEANCTF_ENABLED
//ZOID
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
			(Client.chase_target->gameEntity - g_edicts) - 1));
	else
		Client.PlayerState.SetStat(STAT_CHASE, 0);
}

#ifdef CLEANCTF_ENABLED
void CPlayerEntity::SetCTFStats()
{
	int i;
	int p1, p2;
	edict_t *e;

	if (ctfgame.match > MATCH_NONE)
		Client.PlayerState.SetStat(STAT_CTF_MATCH, CONFIG_CTF_MATCH);
	else
		Client.PlayerState.SetStat(STAT_CTF_MATCH, 0);

	//ghosting
	if (Client.resp.ghost)
	{
		Client.resp.ghost->score = Client.resp.score;
		Q_strncpyz(Client.resp.ghost->netname, Client.pers.netname, sizeof(Client.resp.ghost->netname));
		Client.resp.ghost->number = gameEntity->state.number;
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

	// tech icon
	i = 0;
	Client.PlayerState.SetStat(STAT_CTF_TECH, 0);
	if (Client.pers.Tech)
		Client.PlayerState.SetStat(STAT_CTF_TECH, Client.pers.Tech->IconIndex);

	// figure out what icon to display for team logos
	// three states:
	//   flag at base
	//   flag taken
	//   flag dropped
	p1 = ImageIndex ("i_ctf1");
	e = G_Find(NULL, FOFS(classname), "item_flag_team1");
	if (e != NULL)
	{
		if (e->solid == SOLID_NOT)
		{
			int i;

			// not at base
			// check if on player
			p1 = ImageIndex ("i_ctf1d"); // default to dropped
			for (i = 1; i <= game.maxclients; i++)
			{
				CPlayerEntity *Player = dynamic_cast<CPlayerEntity*>(g_edicts[i].Entity);

				if (Player->IsInUse() &&
					(Player->Client.pers.Flag == RedFlag))
				{
					// enemy has it
					p1 = ImageIndex ("i_ctf1t");
					break;
				}
			}
		}
		else if (e->spawnflags & DROPPED_ITEM)
			p1 = ImageIndex ("i_ctf1d"); // must be dropped
	}
	p2 = ImageIndex ("i_ctf2");
	e = G_Find(NULL, FOFS(classname), "item_flag_team2");
	if (e != NULL)
	{
		if (e->solid == SOLID_NOT)
		{
			int i;

			// not at base
			// check if on player
			p2 = ImageIndex ("i_ctf2d"); // default to dropped
			for (i = 1; i <= game.maxclients; i++)
			{
				CPlayerEntity *Player = dynamic_cast<CPlayerEntity*>(g_edicts[i].Entity);

				if (Player->IsInUse() &&
					(Player->Client.pers.Flag == BlueFlag))
				{
					// enemy has it
					p2 = ImageIndex ("i_ctf2t");
					break;
				}
			}
		}
		else if (e->spawnflags & DROPPED_ITEM)
			p2 = ImageIndex ("i_ctf2d"); // must be dropped
	}


	Client.PlayerState.SetStat(STAT_CTF_TEAM1_PIC, p1);
	Client.PlayerState.SetStat(STAT_CTF_TEAM2_PIC, p2);

	if (ctfgame.last_flag_capture && level.time - ctfgame.last_flag_capture < 5)
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
		(Client.pers.Flag == BlueFlag) &&
		(level.framenum & 8))
		Client.PlayerState.SetStat(STAT_CTF_FLAG_PIC, ImageIndex ("i_ctf2"));

	else if (Client.resp.ctf_team == CTF_TEAM2 &&
		(Client.pers.Flag == RedFlag) &&
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

bool loc_CanSee (edict_t *targ, edict_t *inflictor);
void CPlayerEntity::CTFSetIDView()
{
	vec3_t	forward, dir;
	CTrace	tr;
	CPlayerEntity	*who, *best;
	float	bd = 0, d;
	int i;

	Client.PlayerState.SetStat(STAT_CTF_ID_VIEW, 0);

	Angles_Vectors(Client.v_angle, forward, NULL, NULL);
	Vec3Scale(forward, 1024, forward);
	Vec3Add(gameEntity->state.origin, forward, forward);
	tr = CTrace(gameEntity->state.origin, forward, gameEntity, CONTENTS_MASK_SOLID);
	if (tr.fraction < 1 && tr.ent && ((tr.ent - g_edicts) >= 1 && (tr.ent - g_edicts) <= game.maxclients))
	{
		Client.PlayerState.SetStat(STAT_CTF_ID_VIEW, CS_PLAYERSKINS + (gameEntity - g_edicts - 1));
		return;
	}

	Angles_Vectors(Client.v_angle, forward, NULL, NULL);
	best = NULL;
	for (i = 1; i <= game.maxclients; i++)
	{
		who = dynamic_cast<CPlayerEntity*>((g_edicts + i)->Entity);
		if (!who->IsInUse() || who->GetSolid() == SOLID_NOT)
			continue;
		Vec3Subtract(who->gameEntity->state.origin, gameEntity->state.origin, dir);
		VectorNormalizeFastf(dir);
		d = DotProduct(forward, dir);
		if (d > bd && loc_CanSee(gameEntity, who->gameEntity))
		{
			bd = d;
			best = who;
		}
	}
	if (bd > 0.90)
		Client.PlayerState.SetStat(STAT_CTF_ID_VIEW, CS_PLAYERSKINS + (best->gameEntity - g_edicts - 1));
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
		ctfgame.ghosts[ghost].code = 10000 + (rand() % 90000);
		for (i = 0; i < MAX_CS_CLIENTS; i++)
		{
			if (i != ghost && ctfgame.ghosts[i].code == ctfgame.ghosts[ghost].code)
				break;
		}
		if (i == MAX_CS_CLIENTS)
			break;
	}
	ctfgame.ghosts[ghost].ent = gameEntity;
	Q_strncpyz(ctfgame.ghosts[ghost].netname, Client.pers.netname, sizeof(ctfgame.ghosts[ghost].netname));
	Client.resp.ghost = ctfgame.ghosts + ghost;
	ClientPrintf(gameEntity, PRINT_CHAT, "Your ghost code is **** %d ****\n", ctfgame.ghosts[ghost].code);
	ClientPrintf(gameEntity, PRINT_HIGH, "If you lose connection, you can rejoin with your score "
		"intact by typing \"ghost %d\".\n", ctfgame.ghosts[ghost].code);
}
#endif