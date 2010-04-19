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
#include "cc_bodyqueue.h"
#include "cc_weaponmain.h"

#if !USE_EXTENDED_GAME_IMPORTS
#include "cc_pmove.h"
#endif

IMPLEMENT_SAVE_SOURCE(CPlayerNoise);

CPersistentData::CPersistentData ()
{
	Clear ();
}

CPersistentData::~CPersistentData()
{
}

CPlayerState::CPlayerState (playerState_t *playerState) :
playerState(playerState)
{
};

void			CPlayerState::Initialize (playerState_t *playerState)
{
	this->playerState = playerState;
};

pMoveState_t	*CPlayerState::GetPMove ()
{
	return &playerState->pMove;
} // Direct pointer
void			CPlayerState::SetPMove (pMoveState_t *newState)
{
	memcpy (&playerState->pMove, newState, sizeof(pMoveState_t));
}

// Unless, of course, you use the vec3f class :D
vec3f			&CPlayerState::GetViewAngles ()
{
	return playerState->viewAngles;
}
vec3f			&CPlayerState::GetViewOffset ()
{
	return playerState->viewOffset;
}

vec3f			&CPlayerState::GetGunAngles ()
{
	return playerState->gunAngles;
}
vec3f			&CPlayerState::GetGunOffset ()
{
	return playerState->gunOffset;
}
vec3f			&CPlayerState::GetKickAngles ()
{
	return playerState->kickAngles;
}

MediaIndex		&CPlayerState::GetGunIndex ()
{
	return (MediaIndex&)playerState->gunIndex;
}

sint32		&CPlayerState::GetGunFrame ()
{
	return playerState->gunFrame;
}

colorf			&CPlayerState::GetViewBlend ()
{
	return playerState->ViewBlend;
}
colorb			CPlayerState::GetViewBlendB ()
{
	// Convert it to a color uint8
	return playerState->ViewBlend;
} // Name had to be different

float			&CPlayerState::GetFov ()
{
	return playerState->fov;
}

ERenderDefFlags	&CPlayerState::GetRdFlags ()
{
	return playerState->rdFlags;
}

sint16			&CPlayerState::GetStat (uint8 index)
{
	if (_CC_ASSERT_EXPR (!(index < 0 || index > 32), "GetStat() index out of bounds"))
		return playerState->stats[0];

	return playerState->stats[index];
}

void CPlayerState::CopyStats (EStatIndex *Stats)
{
	memcpy (playerState->stats, Stats, sizeof(playerState->stats));
}

EStatIndex *CPlayerState::GetStats ()
{
	return playerState->stats;
}

void			CPlayerState::Clear ()
{
	Mem_Zero (playerState, sizeof(&playerState));
}

CClient::CClient (gclient_t *client) :
client(client),
PlayerState(&client->playerState)
{
	Clear ();
};

void CClient::Write (CFile &File)
{
	File.Write<vec3f> (KickAngles);
	File.Write<vec3f> (KickOrigin);
	File.Write<vec3f> (ViewAngle);
	File.Write<vec3f> (DamageFrom);
	File.Write<colorf> (DamageBlend);
	File.Write<vec3f> (OldViewAngles);
	File.Write<vec3f> (OldVelocity);
	File.Write<vec2f> (ViewDamage);
	File.Write<FrameNumber_t> (ViewDamageTime);
	File.Write<float> (KillerYaw);
	File.Write<pMoveState_t> (OldPMove);
	File.Write<ELayoutFlags> (LayoutFlags);
	File.WriteArray<EDamageType> (DamageValues, DT_MAX);
	File.Write<EButtons> (Buttons);
	File.Write<EButtons> (LatchedButtons);
	File.Write<EWeaponState> (WeaponState);
	File.Write<FrameNumber_t> (FallTime);
	File.Write<float> (FallValue);
	File.Write<float> (BonusAlpha);
	File.Write<float> (BobTime);
	File.Write<uint8> (PowerArmorTime);
	File.Write<EWaterLevel> (OldWaterLevel);
	WriteIndex (File, WeaponSound, INDEX_SOUND);

	File.Write<client_Animation_t> (Anim);
	File.Write<client_Timers_t> (Timers);
	File.Write<client_Grenade_Data_t> (Grenade);
	File.Write<client_Flood_t> (Flood);

	SaveWeapon (File, NewWeapon);

#if ROGUE_FEATURES
	File.Write<sint32> ((OwnedSphere != NULL) ? OwnedSphere->State.GetNumber() : -1);
#endif

	Persistent.Save (File);
	Respawn.Save (File);
}

void CClient::Load (CFile &File)
{
	KickAngles = File.Read<vec3f> ();
	KickOrigin = File.Read<vec3f> ();
	ViewAngle = File.Read<vec3f> ();
	DamageFrom = File.Read<vec3f> ();
	DamageBlend = File.Read<colorf> ();
	OldViewAngles = File.Read<vec3f> ();
	OldVelocity = File.Read<vec3f> ();
	ViewDamage = File.Read<vec2f> ();
	ViewDamageTime = File.Read<FrameNumber_t> ();
	KillerYaw = File.Read<float> ();
	OldPMove = File.Read<pMoveState_t> ();
	LayoutFlags = File.Read<ELayoutFlags> ();
	File.ReadArray<EDamageType> (DamageValues, DT_MAX);
	Buttons = File.Read<EButtons> ();
	LatchedButtons = File.Read<EButtons> ();
	WeaponState = File.Read<EWeaponState> ();
	FallTime = File.Read<FrameNumber_t> ();
	FallValue = File.Read<float> ();
	BonusAlpha = File.Read<float> ();
	BobTime = File.Read<float> ();
	PowerArmorTime = File.Read<uint8> ();
	OldWaterLevel = File.Read<EWaterLevel> ();
	ReadIndex (File, WeaponSound, INDEX_SOUND);

	Anim = File.Read<client_Animation_t> ();
	Timers = File.Read<client_Timers_t> ();
	Grenade = File.Read<client_Grenade_Data_t> ();
	Flood = File.Read<client_Flood_t> ();

	LoadWeapon (File, &NewWeapon);

#if ROGUE_FEATURES
	sint32 Index = File.Read<sint32> ();
	if (Index != -1)
		OwnedSphere = entity_cast<CRogueBaseSphere>(Game.Entities[Index].Entity);
#endif

	Persistent.Load (File);
	Respawn.Load (File);
}

void CClient::WriteClientStructure (CFile &File)
{
	File.Write (client, sizeof(*client));
}

void CClient::ReadClientStructure (CFile &File, sint32 index)
{
	gclient_t *ptr = &Game.Clients[index];
	File.Read (ptr, sizeof(*ptr));
}

void CClient::RepositionClient (gclient_t *client)
{
	this->client = client;
	PlayerState.playerState = &client->playerState;
}

sint32 &CClient::GetPing ()
{
	return client->ping;
}

extern bool ReadingGame;
void CClient::Clear ()
{
	if (!ReadingGame)
	{
		Mem_Zero (client, sizeof(*client));

		PlayerState.Initialize (&client->playerState);
		KickAngles.Clear ();
		KickOrigin.Clear ();
		ViewAngle.Clear ();
		DamageFrom.Clear ();
		DamageBlend.Set (0,0,0,0);
		mynoise = NULL;
		mynoise2 = NULL;
		OldViewAngles.Clear ();
		OldVelocity.Clear ();
		ViewDamage.Clear ();
		ViewDamageTime = 0;
		KillerYaw = 0;
		Persistent.Clear ();
		Respawn.Clear ();
		Mem_Zero (&OldPMove, sizeof(OldPMove));
		LayoutFlags = 0;
		Buttons = 0;
		LatchedButtons = 0;
		NewWeapon = NULL;
		WeaponState = 0;
		FallTime = 0;
		FallValue = 0;
		BonusAlpha = 0;
		BobTime = 0;
		PowerArmorTime = 0;
		OldWaterLevel = 0;
		WeaponSound = 0;

		Mem_Zero (&Anim, sizeof(Anim));
		Mem_Zero (&Timers, sizeof(Timers));
		Mem_Zero (&Grenade, sizeof(Grenade));
		Mem_Zero (&Flood, sizeof(Flood));
		Mem_Zero (&Chase, sizeof(Chase));
	#if CLEANCTF_ENABLED
		Mem_Zero (&Grapple, sizeof(Grapple));
	#endif
		Mem_Zero (&Tech, sizeof(Tech));

		Mem_Zero (&DamageValues, sizeof(DamageValues));
	}
}

// Players have a special way of allocating the entity.
// We won't automatically allocate it since it already exists
CPlayerEntity::CPlayerEntity (sint32 Index) :
IBaseEntity(Index),
IHurtableEntity(Index),
IPhysicsEntity(Index),
Client(&Game.Clients[State.GetNumber()-1]),
NoClip(false),
TossPhysics(false)
{
	EntityFlags |= ENT_PLAYER;
	PhysicsType = PHYSICS_WALK;
};

CPlayerEntity::~CPlayerEntity ()
{
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
	if (Level.IntermissionTime)
		return;

	if ((Game.GameMode & GAME_DEATHMATCH) &&  
#if CLEANCTF_ENABLED
		!(Game.GameMode & GAME_CTF) &&
#endif
		Client.Persistent.Spectator != Client.Respawn.Spectator &&
		(Level.Frame - Client.Timers.RespawnTime) >= 50)
	{
		SpectatorRespawn();
		return;
	}

	// run weapon animations
	if (!Client.Respawn.Spectator && Client.Persistent.Weapon)
	{
#if CLEANCTF_ENABLED
		if (!(Game.GameMode & GAME_CTF) || ((Game.GameMode & GAME_CTF) && !NoClip))
#endif
		Client.Persistent.Weapon->Think (this);
	}

	if (DeadFlag)
	{
		// wait for any button just going down
		if ( Level.Frame > Client.Timers.RespawnTime)
		{
			sint32 buttonMask;
			// in deathmatch, only wait for attack button
			if (Game.GameMode & GAME_DEATHMATCH)
				buttonMask = BUTTON_ATTACK;
			else
				buttonMask = -1;

			if ( ( Client.LatchedButtons & buttonMask ) ||
				((Game.GameMode & GAME_DEATHMATCH) && DeathmatchFlags.dfForceRespawn.IsEnabled() ) 
#if CLEANCTF_ENABLED
				|| CTFMatchOn()
#endif
				)
			{
				Respawn();
				Client.LatchedButtons = 0;
			}
		}
		return;
	}

	// add player trail so monsters can follow
	if (!(Game.GameMode & GAME_DEATHMATCH) && !IsVisible (State.GetOrigin() + vec3f(0, 0, ViewHeight), PlayerTrail_LastSpot()->Origin, this))
		PlayerTrail_Add (State.GetOldOrigin());

	Client.LatchedButtons = 0;
};

void CPlayerEntity::Respawn ()
{
	if (!(Game.GameMode & GAME_SINGLEPLAYER))
	{
		// Spectator's don't leave bodies
		if (!NoClip)
			CopyToBodyQueue (this);
		GetSvFlags() &= ~SVF_NOCLIENT;
		PutInServer ();

		// add a teleportation effect
		State.GetEvent() = EV_PLAYER_TELEPORT;

		// hold in place briefly
		Client.PlayerState.GetPMove()->pmFlags = PMF_TIME_TELEPORT;
		Client.PlayerState.GetPMove()->pmTime = 14;

		Client.Timers.RespawnTime = Level.Frame;
		return;
	}

	// restart the entire server
	gi.AddCommandString ("menu_loadgame\n");
}

#include "cc_tent.h"

/* 
 * only called when Persistent.Spectator changes
 * note that Respawn.Spectator should be the opposite of Persistent.Spectator here
 */
void CPlayerEntity::SpectatorRespawn ()
{
	// if the user wants to become a Spectator, make sure he doesn't
	// exceed max_spectators

	if (Client.Persistent.Spectator)
	{
		std::string value = Info_ValueForKey (Client.Persistent.UserInfo, "spectator");
		if (*CvarList[CV_SPECTATOR_PASSWORD].String() && 
			strcmp(CvarList[CV_SPECTATOR_PASSWORD].String(), "none") && 
			value != CvarList[CV_SPECTATOR_PASSWORD].String())
		{
			PrintToClient (PRINT_HIGH, "Spectator password incorrect.\n");
			Client.Persistent.Spectator = false;
			StuffText ("spectator 0\n");
			return;
		}

		// count spectators
		sint32 numspec = 0;
		for (sint32 i = 1; i <= Game.MaxClients; i++)
		{
			CPlayerEntity *Player = entity_cast<CPlayerEntity>(Game.Entities[i].Entity);
			if (Player->GetInUse() && Player->Client.Persistent.Spectator)
				numspec++;
		}

		if (numspec >= Game.MaxSpectators)
		{
			PrintToClient (PRINT_HIGH, "Server Spectator limit is full.");
			Client.Persistent.Spectator = false;
			// reset his Spectator var
			StuffText ("spectator 0\n");
			return;
		}
	}
	else
	{
		// he was a Spectator and wants to join the game
		// he must have the right password
		std::string value = Info_ValueForKey (Client.Persistent.UserInfo, "password");
		if (*CvarList[CV_PASSWORD].String() && strcmp(CvarList[CV_PASSWORD].String(), "none") && 
			value != CvarList[CV_PASSWORD].String())
		{
			PrintToClient (PRINT_HIGH, "Password incorrect.\n");
			Client.Persistent.Spectator = true;
			StuffText ("spectator 1\n");
			return;
		}
	}

	// clear client on respawn
	Client.Respawn.Score = Client.Persistent.Score = 0;

	GetSvFlags() &= ~SVF_NOCLIENT;
	PutInServer ();

	// add a teleportation effect
	if (!Client.Persistent.Spectator)
	{
		// send effect
		CMuzzleFlash(State.GetOrigin(), State.GetNumber(), MZ_LOGIN).Send();

		// hold in place briefly
		Client.PlayerState.GetPMove()->pmFlags = PMF_TIME_TELEPORT;
		Client.PlayerState.GetPMove()->pmTime = 14;
	}

	Client.Timers.RespawnTime = Level.Frame;

	if (Client.Persistent.Spectator) 
		BroadcastPrintf (PRINT_HIGH, "%s has moved to the sidelines\n", Client.Persistent.Name.c_str());
	else
		BroadcastPrintf (PRINT_HIGH, "%s joined the game\n", Client.Persistent.Name.c_str());
}

/*
===========
PutInServer

Called when a player connects to a server or respawns in
a CvarList[CV_DEATHMATCH].
============
*/
void CPlayerEntity::PutInServer ()
{
	static const vec3f		mins (-16, -16, -2);
	static const vec3f		maxs (16, 16, 32);
	sint32						index;
	vec3f					spawn_origin, spawn_angles;
	sint32						i;
	static CPersistentData		saved;
	static CRespawnData			Respawn;

	// find a spawn point
	// do it before setting health back up, so farthest
	// ranging doesn't count this client
	SelectSpawnPoint (spawn_origin, spawn_angles);

	index = State.GetNumber()-1;

	char		userinfo[MAX_INFO_STRING];
	switch (Game.GameMode)
	{
	// deathmatch wipes most client data every spawn
	default:
			Respawn = Client.Respawn;
			//memcpy (userinfo, Client.Persistent.UserInfo.c_str(), sizeof(userinfo));
			Q_snprintfz (userinfo, sizeof(userinfo), "%s", Client.Persistent.UserInfo.c_str());
			InitPersistent ();
			UserinfoChanged (userinfo);
		break;
	case GAME_COOPERATIVE:
			Respawn = Client.Respawn;
			//memcpy (userinfo, Client.Persistent.UserInfo.c_str(), sizeof(userinfo));
			Q_snprintfz (userinfo, sizeof(userinfo), "%s", Client.Persistent.UserInfo.c_str());

			Respawn.CoopRespawn.GameHelpChanged = Client.Persistent.GameHelpChanged;
			Respawn.CoopRespawn.HelpChanged = Client.Persistent.HelpChanged;
			Client.Persistent = Respawn.CoopRespawn;
			UserinfoChanged (userinfo);
			if (Respawn.Score > Client.Persistent.Score)
				Client.Persistent.Score = Respawn.Score;
		break;
	case GAME_SINGLEPLAYER:
			Respawn.Clear ();
		break;
	}

	// clear everything but the persistant data
	saved = Client.Persistent;
	Client.Clear ();

	Client.Persistent = saved;
	if (Client.Persistent.Health <= 0)
	{
		InitPersistent();
	
		// Paril, this fixes occasions where a demo/cin is played first.
		UserinfoChanged (saved.UserInfo.c_str());
	}
	Client.Respawn = Respawn;
	Client.Persistent.State = SVCS_SPAWNED;

	// copy some data from the client to the entity
	FetchEntData ();

	// clear entity values
	GroundEntity = NULL;
	CanTakeDamage = true;
	NoClip = false;
	TossPhysics = false;
	ViewHeight = 22;
	GetInUse() = true;
	ClassName = "player";
	Mass = 200;
	GetSolid() = SOLID_BBOX;
	DeadFlag = false;
	AirFinished = Level.Frame + 120;
	GetClipmask() = CONTENTS_MASK_PLAYERSOLID;
	WaterInfo.Level = WATER_NONE;
	WaterInfo.Type = 0;
	Flags &= ~FL_NO_KNOCKBACK;
	GetSvFlags() &= ~SVF_DEADMONSTER;
	if (!Client.Respawn.MenuState.Player)
		Client.Respawn.MenuState.Initialize (this);

#if CLEANCODE_IRC
	Client.Respawn.IRC.Player = this;
#endif

#if ROGUE_FEATURES
	Flags &= ~FL_SAM_RAIMI;		// PGM - turn off sam raimi flag
#endif

	GetMins() = mins;
	GetMaxs() = maxs;
	Velocity.Clear ();

	// clear playerstate values
	Client.PlayerState.Clear ();

	Client.PlayerState.GetPMove()->origin[0] = spawn_origin.X*8;
	Client.PlayerState.GetPMove()->origin[1] = spawn_origin.Y*8;
	Client.PlayerState.GetPMove()->origin[2] = spawn_origin.Z*8;
//ZOID
	Client.PlayerState.GetPMove()->pmFlags &= ~PMF_NO_PREDICTION;
//ZOID

	if ((Game.GameMode & GAME_DEATHMATCH) && DeathmatchFlags.dfFixedFov.IsEnabled())
		Client.PlayerState.GetFov () = 90;
	else
	{
		float fov = atof(Info_ValueForKey(Client.Persistent.UserInfo, "fov").c_str());
		if (fov < 1)
			fov = 90;
		else if (fov > 160)
			fov = 160;

		Client.PlayerState.GetFov () = fov;
	}

	//client->playerState.gunIndex = ModelIndex(client->Persistent.weapon->view_model);

	// clear entity state values
	State.GetEffects() = 0;
	State.GetModelIndex() = 255;		// will use the skin specified model
	State.GetModelIndex(2) = 255;		// custom gun model
	// sknum is player num and weapon number
	// weapon number will be added in changeweapon
	State.GetSkinNum()--;
	State.GetFrame() = 0;

	State.GetOrigin() = (spawn_origin + vec3f(0,0,1));
	State.GetOldOrigin() = State.GetOrigin();

	// set the delta angle
	for (i = 0; i < 3; i++)
		Client.PlayerState.GetPMove()->deltaAngles[i] = ANGLE2SHORT(spawn_angles[i] - Client.Respawn.CmdAngles[i]);

	State.GetAngles().Set (0, spawn_angles[YAW], 0);
	Client.PlayerState.GetViewAngles() = State.GetAngles();
	Client.ViewAngle = State.GetAngles();

#if CLEANCTF_ENABLED
//ZOID
	if ((Game.GameMode & GAME_CTF) && CTFStart())
		return;
//ZOID
#endif

	// spawn a Spectator
	if (Client.Persistent.Spectator)
	{
		Client.Chase.Target = NULL;

		Client.Respawn.Spectator = true;

		NoClip = true;
		GetSolid() = SOLID_NOT;
		GetSvFlags() |= SVF_NOCLIENT;
		Client.PlayerState.GetGunIndex () = 0;
		Link ();
		return;
	}
	else
		Client.Respawn.Spectator = false;

	KillBox ();
	Link ();

	// force the current weapon up
	Client.NewWeapon = Client.Persistent.Weapon;
	Client.Persistent.Weapon->ChangeWeapon(this);
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
	Client.Persistent.Clear ();

	if (!CvarList[CV_MAP_DEBUG].Boolean())
	{
		NItems::Blaster->Add(this, 1);
		Client.Persistent.Weapon = &CBlaster::Weapon;
		Client.Persistent.LastWeapon = Client.Persistent.Weapon;
		Client.Persistent.Inventory.SelectedItem = Client.Persistent.Weapon->Item->GetIndex();
	}
	else
	{
		FindItem("Surface Picker")->Add(this, 1);
		Client.Persistent.Weapon = &CSurfacePicker::Weapon;
		Client.Persistent.LastWeapon = Client.Persistent.Weapon;
		Client.Persistent.Inventory.SelectedItem = Client.Persistent.Weapon->Item->GetIndex();
	}

#if CLEANCTF_ENABLED
	if (Game.GameMode & GAME_CTF)
		NItems::Grapple->Add(this, 1);
	Client.Persistent.Flag = NULL;
#endif

	Client.Persistent.Tech = NULL;
	Client.Persistent.Armor = NULL;

	Client.Persistent.Health			= 100;
	Client.Persistent.MaxHealth			= 100;

	InitItemMaxValues();
}

void CPlayerEntity::InitItemMaxValues ()
{
	Client.Persistent.MaxAmmoValues[CAmmo::AMMOTAG_SHELLS] = 100;
	Client.Persistent.MaxAmmoValues[CAmmo::AMMOTAG_BULLETS] = 200;
	Client.Persistent.MaxAmmoValues[CAmmo::AMMOTAG_GRENADES] = 50;
	Client.Persistent.MaxAmmoValues[CAmmo::AMMOTAG_ROCKETS] = 50;
	Client.Persistent.MaxAmmoValues[CAmmo::AMMOTAG_CELLS] = 200;
	Client.Persistent.MaxAmmoValues[CAmmo::AMMOTAG_SLUGS] = 50;
#if XATRIX_FEATURES
	Client.Persistent.MaxAmmoValues[CAmmo::AMMOTAG_MAGSLUGS] = 50;
	Client.Persistent.MaxAmmoValues[CAmmo::AMMOTAG_TRAP] = 5;
#endif
#if ROGUE_FEATURES
	Client.Persistent.MaxAmmoValues[CAmmo::AMMOTAG_PROX] = 50;
	Client.Persistent.MaxAmmoValues[CAmmo::AMMOTAG_FLECHETTES] = 200;
	Client.Persistent.MaxAmmoValues[CAmmo::AMMOTAG_TESLA] = 50;
	Client.Persistent.MaxAmmoValues[CAmmo::AMMOTAG_ROUNDS] = 100;
#endif
}

/*
===========
UserinfoChanged

called whenever the player updates a userinfo variable.

The game can override any of the settings in place
(forcing skins or names, etc) before copying it off.
============
*/
void CPlayerEntity::UserinfoChanged (const char *userinfo)
{
	std::string UserInfo = userinfo;

	// check for malformed or illegal info strings
	if (!Info_Validate(UserInfo))
		UserInfo = "\\name\\badinfo\\skin\\male/grunt";

	// set name
	Client.Persistent.Name = Info_ValueForKey (UserInfo, "name");

	// set Spectator
	std::string s = Info_ValueForKey (UserInfo, "spectator");
	// spectators are only supported in deathmatch
	Client.Persistent.Spectator = ((Game.GameMode & GAME_DEATHMATCH) && s.length() && s != "0");

	// set skin
	s = Info_ValueForKey (UserInfo, "skin");
	sint32 playernum = State.GetNumber() - 1;

	// combine name and skin into a configstring
#if CLEANCTF_ENABLED
//ZOID
	if (Game.GameMode & GAME_CTF)
		CTFAssignSkin(UserInfo);
	else
//ZOID
#endif
	{
		std::string temp = Client.Persistent.Name + "\\" + s;
		ConfigString (CS_PLAYERSKINS+playernum, temp.c_str());
	}

	// fov
	if ((Game.GameMode & GAME_DEATHMATCH) && DeathmatchFlags.dfFixedFov.IsEnabled())
		Client.PlayerState.GetFov () = 90;
	else
	{
		float fov = atof(Info_ValueForKey(UserInfo, "fov").c_str());
		Client.PlayerState.GetFov () = Clamp<float> (fov, 1, 160);
	}

	// handedness
	s = Info_ValueForKey (UserInfo, "hand");
	if (s.length())
		Client.Persistent.Hand = atoi(s.c_str());

	// IP
	// Paril: removed. could be changed any time in-game!
	//s = Info_ValueForKey (userinfo, "ip");

	// Gender
	s = Info_ValueForKey (UserInfo, "gender");
	if (s.length())
	{
		switch (s[0])
		{
		case 'm':
		case 'M':
			Client.Respawn.Gender = GENDER_MALE;
			break;
		case 'f':
		case 'F':
			Client.Respawn.Gender = GENDER_FEMALE;
			break;
		default:
			Client.Respawn.Gender = GENDER_NEUTRAL;
			break;
		}
	}
	else
		Client.Respawn.Gender = GENDER_MALE;

	// MSG command
	s = Info_ValueForKey (UserInfo, "msg");
	if (s.length())
		Client.Respawn.MessageLevel = atoi (s.c_str());

	// save off the userinfo in case we want to check something later
	Client.Persistent.UserInfo = userinfo;
}

#if CLEANCTF_ENABLED
void CPlayerEntity::CTFAssignSkin(std::string &s)
{
	sint32 playernum = State.GetNumber()-1;
	std::string t = Info_ValueForKey(s, "skin");

	if (t.find('/'))
		t.erase (t.find('/') + 1);
	else
		t = "male/";

	switch (Client.Respawn.CTF.Team)
	{
	case CTF_TEAM1:
		ConfigString (CS_PLAYERSKINS+playernum, (Client.Persistent.Name + t + CTF_TEAM1_SKIN).c_str());
		break;
	case CTF_TEAM2:
		ConfigString (CS_PLAYERSKINS+playernum, (Client.Persistent.Name + t + CTF_TEAM2_SKIN).c_str());
		break;
	default:
		ConfigString (CS_PLAYERSKINS+playernum, (Client.Persistent.Name + s).c_str());
		break;
	}
}

bool CPlayerEntity::CTFStart ()
{
	if (Client.Respawn.CTF.Team != CTF_NOTEAM)
		return false;

	if ((!DeathmatchFlags.dfCtfForceJoin.IsEnabled() || ctfgame.match >= MATCH_SETUP))
	{
		// start as 'observer'
		Client.Respawn.CTF.Team = CTF_NOTEAM;
		NoClip = true;
		GetSolid() = SOLID_NOT;
		GetSvFlags() |= SVF_NOCLIENT;
		Client.PlayerState.GetGunIndex () = 0;
		Link ();

		CTFOpenJoinMenu(this);
		return true;
	}
	return false;
}
#endif

void CPlayerEntity::FetchEntData ()
{
	Health = Client.Persistent.Health;
	MaxHealth = Client.Persistent.MaxHealth;
	Flags |= Client.Persistent.SavedFlags;
	if (Game.GameMode & GAME_COOPERATIVE)
		Client.Respawn.Score = Client.Persistent.Score;
}

// ==============================================
// Follows is the entire p_view.cpp file!

float	bobmove;
sint32		bobcycle;		// odd cycles are right foot going forward
float	bobfracsin;		// sin(bobfrac*M_PI)

/*
===============
CalcRoll
===============
*/
inline float CPlayerEntity::CalcRoll (vec3f &velocity, vec3f &right)
{
	float	side = Q_fabs(velocity | right);
	float	sign = side < 0 ? -1 : 1;

	if (side < CvarList[CV_ROLLSPEED].Float())
		side = side * CvarList[CV_ROLLANGLE].Float() / CvarList[CV_ROLLSPEED].Float();
	else
		side = CvarList[CV_ROLLANGLE].Float();
	
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
	Client.PlayerState.GetStat (STAT_FLASHES) = 0;
	if (Client.DamageValues[DT_BLOOD])
		Client.PlayerState.GetStat (STAT_FLASHES) = Client.PlayerState.GetStat(STAT_FLASHES) | 1;
	if (Client.DamageValues[DT_ARMOR] && !(Flags & FL_GODMODE) && (Client.Timers.Invincibility <= Level.Frame))
		Client.PlayerState.GetStat (STAT_FLASHES) = Client.PlayerState.GetStat(STAT_FLASHES) | 2;

	// total points of damage shot at the player this frame
	sint32 count = (Client.DamageValues[DT_BLOOD] + Client.DamageValues[DT_ARMOR] + Client.DamageValues[DT_POWERARMOR]);
	if (count == 0)
		return;		// didn't take any damage

	// start a pain animation if still in the player model
	if (Client.Anim.Priority < ANIM_PAIN && State.GetModelIndex() == 255)
	{
		Client.Anim.Priority = ANIM_PAIN;
		if (Client.PlayerState.GetPMove()->pmFlags & PMF_DUCKED)
		{
			State.GetFrame() = FRAME_crpain1 - 1;
			Client.Anim.EndFrame = FRAME_crpain4;
		}
		else
		{
			switch (irandom(3))
			{
			case 0:
				State.GetFrame() = FRAME_pain101 - 1;
				Client.Anim.EndFrame = FRAME_pain104;
				break;
			case 1:
				State.GetFrame() = FRAME_pain201 - 1;
				Client.Anim.EndFrame = FRAME_pain204;
				break;
			case 2:
				State.GetFrame() = FRAME_pain301 - 1;
				Client.Anim.EndFrame = FRAME_pain304;
				break;
			}
		}
	}

	sint32 realcount = count;
	if (count < 10)
		count = 10;	// always make a visible effect

	// play an apropriate pain sound
	if ((Level.Frame > PainDebounceTime) && !(Flags & FL_GODMODE) && (Client.Timers.Invincibility <= Level.Frame))
	{
		PainDebounceTime = Level.Frame + 7;

		sint32 l = Clamp<sint32>(((floorf((Max<>(0, Health-1)) / 25))), 0, 3);
		PlaySound (CHAN_VOICE, GameMedia.Player.Pain[l][(irandom(2))]);
	}

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
	if (Client.DamageValues[DT_POWERARMOR])
		NewColor.G = NewColor.G + (float)Client.DamageValues[DT_POWERARMOR]/realcount;
	if (Client.DamageValues[DT_ARMOR])
	{
		NewColor.R = NewColor.R + (float)Client.DamageValues[DT_ARMOR]/realcount;
		NewColor.G = NewColor.G + (float)Client.DamageValues[DT_ARMOR]/realcount;
		NewColor.B = NewColor.B + (float)Client.DamageValues[DT_ARMOR]/realcount;
	}
	if (Client.DamageValues[DT_BLOOD])
		NewColor.R = NewColor.R + (float)Client.DamageValues[DT_BLOOD]/realcount;

	Client.DamageBlend = NewColor;


	//
	// calculate view angle kicks
	//
	float Kick = Q_fabs(Client.DamageValues[DT_KNOCKBACK]);
	if (Kick && (Health > 0))	// kick of 0 means no view adjust at all
	{
		Kick *= 100 / Health;

		if (Kick < count*0.5)
			Kick = count*0.5;
		if (Kick > 50)
			Kick = 50;

		vec3f v = (Client.DamageFrom - State.GetOrigin ()).GetNormalized();
		
		Client.ViewDamage.Y = Kick * (v | right) * 0.3f;
		Client.ViewDamage.X = Kick * -(v | forward) * 0.3f;
		Client.ViewDamageTime = Level.Frame + DAMAGE_TIME;
	}

	//
	// clear totals
	//
	Mem_Zero (&Client.DamageValues, sizeof(Client.DamageValues));
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
		Client.PlayerState.GetViewAngles().Set (-15, Client.KillerYaw, 40);
		Client.PlayerState.GetKickAngles().Clear ();
	}
	else
	{
		// Base angles
		vec3f angles = Client.KickAngles;

		// add angles based on damage kick
		ratio = (float)(Client.ViewDamageTime - Level.Frame) / DAMAGE_TIME;
		if (ratio < 0)
		{
			ratio = 0;
			Client.ViewDamage.Clear ();
		}
		angles.X += ratio * Client.ViewDamage.X;
		angles.Z += ratio * Client.ViewDamage.Y;

		// add pitch based on fall kick
		ratio = (float)(Client.FallTime - Level.Frame) / FALL_TIME;
		if (ratio < 0)
			ratio = 0;
		angles.X += ratio * Client.FallValue;

		// add angles based on velocity
		float delta = Velocity | forward;
		angles.X += delta*CvarList[CV_RUN_PITCH].Float();
		
		delta = Velocity | right;
		angles.Z += delta*CvarList[CV_RUN_ROLL].Float();

		// add angles based on bob

		delta = bobfracsin * CvarList[CV_BOB_PITCH].Float() * xyspeed;
		if (Client.PlayerState.GetPMove()->pmFlags & PMF_DUCKED)
			delta *= 6;		// crouching
		angles.X += delta;
		delta = bobfracsin * CvarList[CV_BOB_ROLL].Float() * xyspeed;
		if (Client.PlayerState.GetPMove()->pmFlags & PMF_DUCKED)
			delta *= 6;		// crouching
		if (bobcycle & 1)
			delta = -delta;
		angles.Z += delta;

		Client.PlayerState.GetKickAngles() = angles;
	}

//===================================

	// add view height
	v.Z += ViewHeight;

	// add fall height
	ratio = (float)(Client.FallTime - Level.Frame) / FALL_TIME;
	if (ratio < 0)
		ratio = 0;
	v.Z -= ratio * Client.FallValue * 0.4;

	// add bob height
	bob = bobfracsin * xyspeed * CvarList[CV_BOB_UP].Float();
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

	Client.PlayerState.GetViewOffset() = v;
}

/*
==============
SV_CalcGunOffset
==============
*/
inline void CPlayerEntity::CalcGunOffset (vec3f &forward, vec3f &right, vec3f &up, float xyspeed)
{
	// FIXME: heatbeam no bob

	// gun angles from bobbing
	vec3f angles	(	(bobcycle & 1) ? (-Client.PlayerState.GetGunAngles ().Z) : (xyspeed * bobfracsin * 0.005), 
						(bobcycle & 1) ? (-Client.PlayerState.GetGunAngles ().Y) : (xyspeed * bobfracsin * 0.01),
						xyspeed * bobfracsin * 0.005
					);

	// gun angles from delta movement
	for (sint32 i = 0; i < 3; i++)
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
	Client.PlayerState.GetGunAngles() = angles;

	// gun height
	Client.PlayerState.GetGunOffset().Clear ();

	angles.Clear ();
	// gun_x / gun_y / gun_z are development tools
	for (sint32 i = 0; i < 3; i++)
	{
		angles[i] += forward[i] * CvarList[CV_GUN_Y].Float();
		angles[i] += right[i] * CvarList[CV_GUN_X].Float();
		angles[i] += up[i] * -CvarList[CV_GUN_Z].Float();
	}

	Client.PlayerState.GetGunAngles() = angles;
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

	uint8 a2 = v_blend.A  + color.A;	// new total alpha
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
#if XATRIX_FEATURES
static const colorf QuadFireColor (1, 0.2f, 0.5f, 0.08f);
#endif
#if ROGUE_FEATURES
static const colorf DoubleColor (0.9f, 0.7f, 0, 0.08f);
static const colorf IRColor (0, 0, 0, 0.2f);
#endif
static const colorf InvulColor (1, 1, 0, 0.08f);
static const colorf EnviroColor (0, 1, 0, 0.08f);
static const colorf BreatherColor (0.4f, 1, 0.4f, 0.04f);
static const colorf ClearColor (0,0,0,0);
static colorf BonusColor (0.85f, 0.7f, 0.3f, 0);

inline void CPlayerEntity::CalcBlend ()
{
	// add for contents
	vec3f	vieworg = State.GetOrigin() + Client.PlayerState.GetViewOffset();
	EBrushContents contents = PointContents (vieworg);

	if (contents & (CONTENTS_LAVA|CONTENTS_SLIME|CONTENTS_WATER) )
		Client.PlayerState.GetRdFlags () |= RDF_UNDERWATER;
	else
		Client.PlayerState.GetRdFlags () &= ~RDF_UNDERWATER;

	// add for powerups
	if (Client.Timers.QuadDamage > Level.Frame)
	{
		sint32 remaining = Client.Timers.QuadDamage - Level.Frame;

		if (remaining == 30)	// beginning to fade
			PlaySound (CHAN_ITEM, SoundIndex("items/damage2.wav"));

		if (remaining > 30 || (remaining & 4) )
			SV_AddBlend (QuadColor, Client.Persistent.ViewBlend);
	}
#if ROGUE_FEATURES
	else if (Client.Timers.Double > Level.Frame)
	{
		sint32 remaining = Client.Timers.Double - Level.Frame;

		if (remaining == 30)	// beginning to fade
			PlaySound (CHAN_ITEM, SoundIndex("misc/ddamage2.wav"));

		if (remaining > 30 || (remaining & 4) )
			SV_AddBlend (DoubleColor, Client.Persistent.ViewBlend);
	}
#endif
#if XATRIX_FEATURES
	else if (Client.Timers.QuadFire > Level.Frame)
	{
		sint32 remaining = Client.Timers.QuadFire - Level.Frame;

		if (remaining == 30)	// beginning to fade
			PlaySound (CHAN_ITEM, SoundIndex("items/quadfire2.wav"));

		if (remaining > 30 || (remaining & 4) )
			SV_AddBlend (QuadFireColor, Client.Persistent.ViewBlend);
	}
#endif
	else if (Client.Timers.Invincibility > Level.Frame)
	{
		sint32 remaining = Client.Timers.Invincibility - Level.Frame;

		if (remaining == 30)	// beginning to fade
			PlaySound (CHAN_ITEM, SoundIndex("items/protect2.wav"));

		if (remaining > 30 || (remaining & 4) )
			SV_AddBlend (InvulColor, Client.Persistent.ViewBlend);
	}
	else if (Client.Timers.EnvironmentSuit > Level.Frame)
	{
		sint32 remaining = Client.Timers.EnvironmentSuit - Level.Frame;

		if (remaining == 30)	// beginning to fade
			PlaySound (CHAN_ITEM, SoundIndex("items/airout.wav"));

		if (remaining > 30 || (remaining & 4) )
			SV_AddBlend (EnviroColor, Client.Persistent.ViewBlend);
	}
	else if (Client.Timers.Rebreather > Level.Frame)
	{
		sint32 remaining = Client.Timers.Rebreather - Level.Frame;

		if (remaining == 30)	// beginning to fade
			PlaySound (CHAN_ITEM, SoundIndex("items/airout.wav"));

		if (remaining > 30 || (remaining & 4) )
			SV_AddBlend (BreatherColor, Client.Persistent.ViewBlend);
	}

#if ROGUE_FEATURES
//PGM
	if (Client.Timers.Nuke > Level.Frame)
		SV_AddBlend (colorf (1, 1, 1, (Client.Timers.Nuke - Level.Frame) / 20.0), Client.Persistent.ViewBlend);

	if (Client.Timers.IR > Level.Frame)
	{
		sint32 remaining = Client.Timers.IR - Level.Frame;

		if(remaining > 30 || (remaining & 4))
		{
			Client.PlayerState.GetRdFlags() |= RDF_IRGOGGLES;
			SV_AddBlend (IRColor, Client.Persistent.ViewBlend);
		}
		else
			Client.PlayerState.GetRdFlags() &= ~RDF_IRGOGGLES;
	}
	else
		Client.PlayerState.GetRdFlags() &= ~RDF_IRGOGGLES;
//PGM
#endif

	// add for damage
	if (Client.DamageBlend.A)
		SV_AddBlend (Client.DamageBlend, Client.Persistent.ViewBlend);

	// drop the damage value
	if (Client.DamageBlend.A)
	{
		Client.DamageBlend.A -= 0.06f;
		if (Client.DamageBlend.A < 0)
			Client.DamageBlend.A = 0;
	}

	// add bonus and drop the value
	if (Client.BonusAlpha)
	{
		BonusColor.A = Client.BonusAlpha;
		SV_AddBlend (BonusColor, Client.Persistent.ViewBlend);

		Client.BonusAlpha -= 0.1f;
		if (Client.BonusAlpha < 0)
			Client.BonusAlpha = 0;
	}

	if (contents & (CONTENTS_LAVA))
		SV_AddBlend (LavaColor, Client.Persistent.ViewBlend);
	else if (contents & CONTENTS_SLIME)
		SV_AddBlend (SlimeColor, Client.Persistent.ViewBlend);
	else if (contents & CONTENTS_WATER)
		SV_AddBlend (WaterColor, Client.Persistent.ViewBlend);

	Client.PlayerState.GetViewBlend () = Client.Persistent.ViewBlend;
	Client.Persistent.ViewBlend.Set(ClearColor);
}


/*
=================
P_FallingDamage
=================
*/
inline void CPlayerEntity::FallingDamage ()
{
	if (DeathmatchFlags.dfNoFallingDamage.IsEnabled())
		return;

	if (State.GetModelIndex() != 255)
		return;		// not in the player model

	if (NoClip)
		return;

#if CLEANCTF_ENABLED
//ZOID
	// never take damage if just release grapple or on grapple
	if (Level.Frame - Client.Grapple.ReleaseTime <= 2 ||
		(Client.Grapple.Entity && 
		Client.Grapple.State > CTF_GRAPPLE_STATE_FLY))
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
	if (WaterInfo.Level == WATER_UNDER)
		return;
	if (WaterInfo.Level == WATER_WAIST)
		delta *= 0.25;
	if (WaterInfo.Level == WATER_FEET)
		delta *= 0.5;

	if (delta < 1)
		return;

	if (delta < 15)
	{
		State.GetEvent() = EV_FOOTSTEP;
		return;
	}

	Client.FallValue = delta*0.5;
	if (Client.FallValue > 40)
		Client.FallValue = 40;
	Client.FallTime = Level.Frame + FALL_TIME;

	if (delta > 30)
	{
		if (Health > 0)
			State.GetEvent() = (delta >= 55) ? EV_FALLFAR : EV_FALL;

		PainDebounceTime = Level.Frame;	// no normal pain sound
		sint32 Damage = (delta-30)/2;
		if (Damage < 1)
			Damage = 1;

		static vec3f dir (0, 0, 1);
		TakeDamage (World, World, dir, State.GetOrigin(), vec3fOrigin, Damage, 0, 0, MOD_FALLING);
	}
	else
	{
		State.GetEvent() = EV_FALLSHORT;
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
	bool			breather, envirosuit;
	EWaterLevel		waterlevel, OldWaterLevel;

	if (NoClip)
	{
		AirFinished = Level.Frame + 120;	// don't need air
		return;
	}

	waterlevel = WaterInfo.Level;
	OldWaterLevel = Client.OldWaterLevel;
	Client.OldWaterLevel = waterlevel;

	breather = (bool)(Client.Timers.Rebreather > Level.Frame);
	envirosuit = (bool)(Client.Timers.EnvironmentSuit > Level.Frame);

	//
	// if just entered a water volume, play a sound
	//
	if (!OldWaterLevel && waterlevel)
	{
		PlayerNoiseAt (State.GetOrigin(), PNOISE_SELF);
		if (WaterInfo.Type & CONTENTS_LAVA)
			PlaySound (CHAN_BODY, SoundIndex("player/lava_in.wav"));
		else if (WaterInfo.Type & CONTENTS_SLIME)
			PlaySound (CHAN_BODY, SoundIndex("player/watr_in.wav"));
		else if (WaterInfo.Type & CONTENTS_WATER)
			PlaySound (CHAN_BODY, SoundIndex("player/watr_in.wav"));
		Flags |= FL_INWATER;

		// clear damage_debounce, so the pain sound will play immediately
		PainDebounceTime = Level.Frame - 1;
	}

	//
	// if just completely exited a water volume, play a sound
	//
	if (OldWaterLevel && !waterlevel)
	{
		PlayerNoiseAt (State.GetOrigin(), PNOISE_SELF);
		PlaySound (CHAN_BODY, SoundIndex("player/watr_out.wav"));
		Flags &= ~FL_INWATER;
	}

	//
	// check for head just going under water
	//
	if (OldWaterLevel != WATER_UNDER && waterlevel == WATER_UNDER)
		PlaySound (CHAN_BODY, SoundIndex("player/watr_un.wav"));

	//
	// check for head just coming out of water
	//
	if (OldWaterLevel == WATER_UNDER && waterlevel != WATER_UNDER)
	{
		if (AirFinished < Level.Frame)
		{	// gasp for air
			PlaySound (CHAN_VOICE, SoundIndex("player/gasp1.wav"));
			PlayerNoiseAt (State.GetOrigin(), PNOISE_SELF);
		}
		else  if (AirFinished < Level.Frame + 110) // just break surface
			PlaySound (CHAN_VOICE, SoundIndex("player/gasp2.wav"));
	}

	//
	// check for drowning
	//
	if (waterlevel == WATER_UNDER)
	{
		// breather or envirosuit give air
		if (breather || envirosuit)
		{
			AirFinished = Level.Frame + 100;

			if (((sint32)(Client.Timers.Rebreather - Level.Frame) % 25) == 0)
			{
				PlaySound (CHAN_AUTO, SoundIndex((!Client.Timers.BreatherSound) ? "player/u_breath1.wav" : "player/u_breath2.wav"));
				Client.Timers.BreatherSound = !Client.Timers.BreatherSound;
				PlayerNoiseAt (State.GetOrigin(), PNOISE_SELF);
			}
		}

		// if out of air, start drowning
		if (AirFinished < Level.Frame)
		{	// drown!
			if (NextDrownTime < Level.Frame 
				&& Health > 0)
			{
				NextDrownTime = Level.Frame + 10;

				// take more damage the longer underwater
				NextDrownDamage += 2;
				if (NextDrownDamage > 15)
					NextDrownDamage = 15;

				// play a gurp sound instead of a normal pain sound
				if (Health <= NextDrownDamage)
					PlaySound (CHAN_VOICE, SoundIndex("player/drown1.wav"));
				else
					PlaySound (CHAN_VOICE, GameMedia.Player.Gurp[(irandom(2))]);

				PainDebounceTime = Level.Frame;

				TakeDamage (World, World, vec3fOrigin, State.GetOrigin(), vec3fOrigin, NextDrownDamage, 0, DAMAGE_NO_ARMOR, MOD_WATER);
			}
		}
	}
	else
	{
		AirFinished = Level.Frame + 120;
		NextDrownDamage = 2;
	}

	//
	// check for sizzle damage
	//
	if (waterlevel && (WaterInfo.Type & (CONTENTS_LAVA|CONTENTS_SLIME)))
	{
		if (WaterInfo.Type & CONTENTS_LAVA)
		{
			if (Health > 0
				&& PainDebounceTime <= Level.Frame
				&& Client.Timers.Invincibility < Level.Frame)
			{
				PlaySound (CHAN_VOICE, SoundIndex((irandom(2)) ? "player/burn1.wav" : "player/burn2.wav"));
				PainDebounceTime = Level.Frame + 10;
			}

			// take 1/3 damage with envirosuit
			TakeDamage (World, World, vec3fOrigin, State.GetOrigin(), vec3fOrigin, (envirosuit) ? 1*waterlevel : 3*waterlevel, 0, 0, MOD_LAVA);
		}

		if (WaterInfo.Type & CONTENTS_SLIME)
		{
			if (!envirosuit) // no damage from slime with envirosuit
				TakeDamage (World, World, vec3fOrigin, State.GetOrigin(), vec3fOrigin, 1*waterlevel, 0, 0, MOD_SLIME);
		}
	}
}

/*
===============
G_SetClientEffects
===============
*/
EPowerArmorType CPlayerEntity::PowerArmorType ()
{
	if (!(Flags & FL_POWER_ARMOR))
		return POWER_ARMOR_NONE;
	else if (Client.Persistent.Inventory.Has(NItems::PowerShield) > 0)
		return POWER_ARMOR_SHIELD;
	else if (Client.Persistent.Inventory.Has(NItems::PowerScreen) > 0)
		return POWER_ARMOR_SCREEN;

	return POWER_ARMOR_NONE;
}

inline void CPlayerEntity::SetClientEffects ()
{
	State.GetEffects() = State.GetRenderEffects() = 0;

#if ROGUE_FEATURES
	State.GetRenderEffects() = RF_IR_VISIBLE;
#endif

	if (Health <= 0 || Level.IntermissionTime)
		return;

#if ROGUE_FEATURES
	if (Flags & FL_DISGUISED)
		State.GetRenderEffects() |= RF_USE_DISGUISE;
#endif

	if (Client.PowerArmorTime)
	{
		sint32 pa_type = PowerArmorType ();
		if (pa_type == POWER_ARMOR_SCREEN)
			State.GetEffects() |= EF_POWERSCREEN;
		else if (pa_type == POWER_ARMOR_SHIELD)
		{
			State.GetEffects() |= EF_COLOR_SHELL;
			State.GetRenderEffects() |= RF_SHELL_GREEN;
		}

		Client.PowerArmorTime--;
	}

#if CLEANCTF_ENABLED
	if (Game.GameMode & GAME_CTF)
	{
		State.GetEffects() &= ~(EF_FLAG1 | EF_FLAG2);
		if (Client.Persistent.Flag)
		{
			if (Health > 0)
				State.GetEffects() |= Client.Persistent.Flag->EffectFlags;
			State.GetModelIndex(3) = ModelIndex(Client.Persistent.Flag->WorldModel);
		}
		else
			State.GetModelIndex(3) = 0;
	}
#endif

	if (Client.Timers.QuadDamage > Level.Frame)
	{
		sint32 remaining = Client.Timers.QuadDamage - Level.Frame;
		if (remaining > 30 || (remaining & 4) )
			State.GetEffects() |= EF_QUAD;
	}

#if XATRIX_FEATURES
	if (Client.Timers.QuadFire > Level.Frame)
	{
		sint32 remaining = Client.Timers.QuadFire - Level.Frame;
		if (remaining > 30 || (remaining & 4) )
			State.GetEffects() |= EF_DOUBLE; // Paril disting.
	}
#endif

#if ROGUE_FEATURES
	if (Client.Timers.Double > Level.Frame)
	{
		sint32 remaining = Client.Timers.Double - Level.Frame;
		if (remaining > 30 || (remaining & 4) )
			State.GetEffects() |= EF_DOUBLE;
	}
	if ((Client.OwnedSphere) && (Client.OwnedSphere->SpawnFlags == 1))
		State.GetEffects() |= EF_HALF_DAMAGE;

	if (Client.Timers.Tracker > Level.Frame)
		State.GetEffects() |= EF_TRACKERTRAIL;
#endif

	if (Client.Timers.Invincibility > Level.Frame)
	{
		sint32 remaining = Client.Timers.Invincibility - Level.Frame;
		if (remaining > 30 || (remaining & 4) )
			State.GetEffects() |= EF_PENT;
	}

	// show cheaters!!!
	if (Flags & FL_GODMODE)
	{
		State.GetEffects() |= EF_COLOR_SHELL;
		State.GetRenderEffects() |= (RF_SHELL_RED|RF_SHELL_GREEN|RF_SHELL_BLUE);
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
		if ( (sint32)(Client.BobTime+bobmove) != bobcycle )
			State.GetEvent() = EV_FOOTSTEP;
	}
}

/*
===============
G_SetClientSound
===============
*/
inline void CPlayerEntity::SetClientSound ()
{
	if (Client.Persistent.GameHelpChanged != Game.HelpChanged)
	{
		Client.Persistent.GameHelpChanged = Game.HelpChanged;
		Client.Persistent.HelpChanged = 1;
	}

	// help beep (no more than three times)
	if (Client.Persistent.HelpChanged && Client.Persistent.HelpChanged <= 3 && !(Level.Frame&63) )
	{
		Client.Persistent.HelpChanged++;
		PlaySound (CHAN_VOICE, SoundIndex ("misc/pc_up.wav"), 255, ATTN_STATIC);
	}

	if (WaterInfo.Level && (WaterInfo.Type & (CONTENTS_LAVA|CONTENTS_SLIME)))
		State.GetSound() = GameMedia.FrySound();
	else if (Client.Persistent.Weapon && Client.Persistent.Weapon->GetWeaponSound ())
		State.GetSound() = Client.Persistent.Weapon->GetWeaponSound ();
	else if (Client.WeaponSound)
		State.GetSound() = Client.WeaponSound;
	else
		State.GetSound() = 0;
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
	if ((duck != Client.Anim.Duck && Client.Anim.Priority < ANIM_DEATH) ||
		(run != Client.Anim.Run && Client.Anim.Priority == ANIM_BASIC) ||
		(!GroundEntity && Client.Anim.Priority <= ANIM_WAVE))
		isNewAnim = true;

	if (!isNewAnim)
	{
		if(Client.Anim.Priority == ANIM_REVERSE)
		{
			if(State.GetFrame() > Client.Anim.EndFrame)
			{
				State.GetFrame()--;
				return;
			}
		}
		else if (State.GetFrame() < Client.Anim.EndFrame)
		{
			// continue an animation
			State.GetFrame()++;
			return;
		}

		if (Client.Anim.Priority == ANIM_DEATH)
			return;		// stay there
		if (Client.Anim.Priority == ANIM_JUMP)
		{
			if (!GroundEntity)
				return;		// stay there
			Client.Anim.Priority = ANIM_WAVE;

			State.GetFrame() = FRAME_jump3;
			Client.Anim.EndFrame = FRAME_jump6;
			return;
		}
	}

	// return to either a running or standing frame
	Client.Anim.Priority = ANIM_BASIC;
	Client.Anim.Duck = duck;
	Client.Anim.Run = run;

	if (!GroundEntity)
	{
#if CLEANCTF_ENABLED
//ZOID: if on grapple, don't go into jump frame, go into standing
//frame
		if (Client.Grapple.Entity)
		{
			State.GetFrame() = FRAME_stand01;
			Client.Anim.EndFrame = FRAME_stand40;
		}
		else
		{
//ZOID
#endif
		Client.Anim.Priority = ANIM_JUMP;
		if (State.GetFrame() != FRAME_jump2)
			State.GetFrame() = FRAME_jump1;
		Client.Anim.EndFrame = FRAME_jump2;
#if CLEANCTF_ENABLED
	}
#endif
	}
	else if (run)
	{	// running
		if (duck)
		{
			State.GetFrame() = FRAME_crwalk1;
			Client.Anim.EndFrame = FRAME_crwalk6;
		}
		else
		{
			State.GetFrame() = FRAME_run1;
			Client.Anim.EndFrame = FRAME_run6;
		}
	}
	else
	{	// standing
		if (duck)
		{
			State.GetFrame() = FRAME_crstnd01;
			Client.Anim.EndFrame = FRAME_crstnd19;
		}
		else
		{
			State.GetFrame() = FRAME_stand01;
			Client.Anim.EndFrame = FRAME_stand40;
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
	if (!gameEntity)
		return;

	float	BobTime;
	sint32		i;

	//
	// If the origin or velocity have changed since ClientThink(),
	// update the pmove values.  This will happen when the client
	// is pushed by a bmodel or kicked by an explosion.
	// 
	// If it wasn't updated here, the view position would lag a frame
	// behind the body position when pushed -- "sinking into plats"
	//
	for (i = 0; i < 3; i++)
	{
		Client.PlayerState.GetPMove()->origin[i] = State.GetOrigin()[i]*8.0;
		Client.PlayerState.GetPMove()->velocity[i] = Velocity[i]*8.0;
	}

	//
	// If the end of unit layout is displayed, don't give
	// the player any normal movement attributes
	//
	if (Level.IntermissionTime)
	{
		// FIXME: add view drifting here?
		Client.PlayerState.GetViewBlend ().A = 0;
		Client.PlayerState.GetFov () = 90;
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
	State.GetAngles().Set (
		(Client.ViewAngle.X > 180) ? (-360 + Client.ViewAngle.X)/3 : Client.ViewAngle.X/3,
		Client.ViewAngle.Y,
		CalcRoll (Velocity, right)*4
		);

	//
	// calculate speed and cycle to be used for
	// all cyclic walking effects
	//
	float xyspeed = sqrtf(Velocity.X*Velocity.X + Velocity.Y*Velocity.Y);

	if (xyspeed < 5 || Client.PlayerState.GetPMove()->pmFlags & PMF_DUCKED)
	{
		bobmove = 0;
		Client.BobTime = 0;	// start at beginning of cycle again
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
	
	BobTime = (Client.BobTime += bobmove);

	if (Client.PlayerState.GetPMove()->pmFlags & PMF_DUCKED)
		BobTime *= 4;

	bobcycle = (sint32)BobTime;
	bobfracsin = Q_fabs(sinf(BobTime*M_PI));

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
	if (Client.Respawn.Spectator)
		SetSpectatorStats();
	else
#if CLEANCTF_ENABLED
//ZOID
	if (!Client.Chase.Target)
//ZOID
#endif
		SetStats ();

#if CLEANCTF_ENABLED
//ZOID
//update chasecam follower stats
	for (i = 1; i <= Game.MaxClients; i++)
	{
		CPlayerEntity *e = entity_cast<CPlayerEntity>(Game.Entities[i].Entity);
		if (!e->GetInUse() || e->Client.Chase.Target != this)
			continue;

		e->Client.PlayerState.CopyStats (Client.PlayerState.GetStats());
		e->Client.PlayerState.GetStat (STAT_LAYOUTS) = 1;

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

	if ((
#if CLEANCTF_ENABLED
		(Game.GameMode & GAME_CTF) || 
#endif
		DeathmatchFlags.dfDmTechs.IsEnabled()) && (Client.Persistent.Tech && (Client.Persistent.Tech->TechType == CTech::TECH_PASSIVE)))
		Client.Persistent.Tech->DoPassiveTech (this);

	// if the scoreboard is up, update it
	if ((Client.LayoutFlags & LF_SHOWSCORES) && !(Level.Frame & 31) )
		DeathmatchScoreboardMessage (false);
	else if (Client.Respawn.MenuState.InMenu && !(Level.Frame & 4))
		Client.Respawn.MenuState.CurrentMenu->Draw (false);
}

#if CLEANCTF_ENABLED
void CPlayerEntity::CTFDeadDropFlag ()
{
	if (Client.Persistent.Flag)
	{
		Client.Persistent.Flag->DropItem (this);
		Client.Persistent.Inventory.Set (Client.Persistent.Flag, 0);
		BroadcastPrintf (PRINT_HIGH, "%s lost the %s flag!\n", Client.Persistent.Name.c_str(), CTFTeamName(Client.Persistent.Flag->team));
		Client.Persistent.Flag = NULL;
	}
}

/*
==================
CTFScoreboardMessage
==================
*/
void CPlayerEntity::CTFScoreboardMessage (bool reliable)
{
	CStatusBar			Bar;
	static char			entry[1024];
	size_t				len;
	static sint32			sorted[2][MAX_CS_CLIENTS];
	static sint32			sortedscores[2][MAX_CS_CLIENTS];
	static sint32			Score, total[2], totalscore[2];
	sint32					last[2];
	sint32					n, j, k;
	static char			str[MAX_COMPRINT/4];
	sint32					team;

	// sort the clients by team and Score
	total[0] = total[1] = 0;
	last[0] = last[1] = 0;
	totalscore[0] = totalscore[1] = 0;
	for (sint32 i = 0; i < Game.MaxClients; i++)
	{
		CPlayerEntity *cl_ent = entity_cast<CPlayerEntity>((Game.Entities + 1 + i)->Entity);
		if (!cl_ent->GetInUse())
			continue;
		if (cl_ent->Client.Respawn.CTF.Team == CTF_TEAM1)
			team = 0;
		else if (cl_ent->Client.Respawn.CTF.Team == CTF_TEAM2)
			team = 1;
		else
			continue; // unknown team?

		Score = cl_ent->Client.Respawn.Score;
		for (j = 0; j < total[team]; j++)
		{
			if (Score > sortedscores[team][j])
				break;
		}
		for (k =total[team]; k > j; k--)
		{
			sorted[team][k] = sorted[team][k-1];
			sortedscores[team][k] = sortedscores[team][k-1];
		}
		sorted[team][j] = i;
		sortedscores[team][j] = Score;
		totalscore[team] += Score;
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

	for (sint32 i = 0; i < 16; i++)
	{
		if (i >= total[0] && i >= total[1])
			break; // we're done

		*entry = 0;

		// left side
		if (i < total[0])
		{
			CPlayerEntity *cl_ent = entity_cast<CPlayerEntity>((Game.Entities + 1 + sorted[0][i])->Entity);

			Bar.AddClientBlock (0, 42 + i * 8, sorted[0][i], cl_ent->Client.Respawn.Score, Clamp<sint32>(cl_ent->Client.GetPing(), 0, 999));

			if (cl_ent->Client.Persistent.Flag == NItems::BlueFlag)
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
			CPlayerEntity *cl_ent = entity_cast<CPlayerEntity>((Game.Entities + 1 + sorted[1][i])->Entity);

			Bar.AddClientBlock (160, 42 + i * 8, sorted[1][i], cl_ent->Client.Respawn.Score, Clamp<sint32>(cl_ent->Client.GetPing(), 0, 999));

			if (cl_ent->Client.Persistent.Flag == NItems::RedFlag)
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
		for (sint32 i = 0; i < Game.MaxClients; i++)
		{
			CPlayerEntity *cl_ent = entity_cast<CPlayerEntity>((Game.Entities + 1 + i)->Entity);
			if (!cl_ent->GetInUse() ||
				cl_ent->GetSolid() != SOLID_NOT ||
				cl_ent->Client.Respawn.CTF.Team != CTF_NOTEAM)
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

			Bar.AddClientBlock ((n & 1) ? 160 : 0, j, i, cl_ent->Client.Respawn.Score, Clamp<sint32>(cl_ent->Client.GetPing(), 0, 999));
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

	Bar.SendMsg (this, reliable);
}
#endif

/*
==================
DeathmatchScoreboardMessage
==================
*/
void CPlayerEntity::DeathmatchScoreboardMessage (bool reliable)
{
#if CLEANCTF_ENABLED
//ZOID
	if (Game.GameMode & GAME_CTF)
	{
		CTFScoreboardMessage (reliable);
		return;
	}
//ZOID
#endif

	CStatusBar			Scoreboard;
	static sint32			sorted[MAX_CS_CLIENTS];
	static sint32			sortedscores[MAX_CS_CLIENTS];
	sint32					Score, total;
	CPlayerEntity		*Killer = (Enemy && (Enemy->EntityFlags & ENT_PLAYER)) ? entity_cast<CPlayerEntity>(Enemy) : NULL;

	// sort the clients by Score
	total = 0;
	for (sint32 i = 0; i < Game.MaxClients ; i++)
	{
		CPlayerEntity *cl_ent = entity_cast<CPlayerEntity>((Game.Entities + 1 + i)->Entity);
		if (!cl_ent->GetInUse() || cl_ent->Client.Respawn.Spectator)
			continue;
		Score = cl_ent->Client.Respawn.Score;
		sint32 j = 0;
		for ( ; j < total; j++)
		{
			if (Score > sortedscores[j])
				break;
		}
		sint32 k = total;
		for ( ; k > j; k--)
		{
			sorted[k] = sorted[k-1];
			sortedscores[k] = sortedscores[k-1];
		}
		sorted[j] = i;
		sortedscores[j] = Score;
		total++;
	}

	// add the clients in sorted order
	if (total > 12)
		total = 12;

	for (sint32 i = 0; i < total; i++)
	{
		sint32		x, y;
		const char	*tag = NULL;
		CPlayerEntity *cl_ent = entity_cast<CPlayerEntity>((Game.Entities + 1 + sorted[i])->Entity);

		if (!cl_ent)
			continue;

		x = (i>=6) ? 160 : 0;
		y = 32 + 32 * (i%6);

		// add a dogtag
		if (cl_ent == this)
			tag = "tag1";
		else if (cl_ent == Killer)
			tag = "tag2";

		if (tag)
		{
			Scoreboard.AddVirtualPoint_X (x+32);
			Scoreboard.AddVirtualPoint_Y (y);
			Scoreboard.AddPic (tag);
		}

		// send the layout
		Scoreboard.AddClientBlock (x, y, sorted[i], cl_ent->Client.Respawn.Score, cl_ent->Client.GetPing(), (Level.Frame - cl_ent->Client.Respawn.EnterFrame)/600);
	}

	Scoreboard.SendMsg (this, reliable);
}

void CPlayerEntity::SetStats ()
{
	if (CvarList[CV_MAP_DEBUG].Boolean())
	{
		Client.PlayerState.GetStat (STAT_PICKUP_STRING) = CS_POINTING_SURFACE;
		Client.PlayerState.GetStat (STAT_TIMER_ICON) = CS_POINTING_SURFACE-1;
	}
	else
	{
		//
		// health
		//
		Client.PlayerState.GetStat (STAT_HEALTH_ICON) = GameMedia.Hud.HealthPic;
		Client.PlayerState.GetStat (STAT_HEALTH) = Health;

		//
		// ammo
		//
		if (Client.Persistent.Weapon)
		{
			if (Client.Persistent.Weapon->Item->Ammo)
			{
				Client.PlayerState.GetStat (STAT_AMMO_ICON) = Client.Persistent.Weapon->Item->Ammo->GetIconIndex();
				Client.PlayerState.GetStat (STAT_AMMO) = Client.Persistent.Inventory.Has(Client.Persistent.Weapon->Item->Ammo->GetIndex());
			}
			else
			{
				Client.PlayerState.GetStat (STAT_AMMO_ICON) = 0;
				Client.PlayerState.GetStat (STAT_AMMO) = 0;
			}
		}
		else
		{
			Client.PlayerState.GetStat (STAT_AMMO_ICON) = 0;
			Client.PlayerState.GetStat (STAT_AMMO) = 0;
		}
		
		//
		// armor
		//
		sint32			cells = 0;
		sint32			power_armor_type = PowerArmorType ();
		if (power_armor_type)
		{
			cells = Client.Persistent.Inventory.Has(NItems::Cells);
			if (cells == 0)
			{	// ran out of cells for power armor
				Flags &= ~FL_POWER_ARMOR;
				PlaySound (CHAN_ITEM, SoundIndex("misc/power2.wav"));
				power_armor_type = 0;
			}
		}

		CArmor *Armor = Client.Persistent.Armor;
		if (power_armor_type && (!Armor || (Level.Frame & 8) ) )
		{	// flash between power armor and other armor icon
			Client.PlayerState.GetStat (STAT_ARMOR_ICON) = NItems::PowerShield->GetIconIndex();
			Client.PlayerState.GetStat (STAT_ARMOR) = cells;
		}
		else if (Armor)
		{
			Client.PlayerState.GetStat (STAT_ARMOR_ICON) = Armor->GetIconIndex();
			Client.PlayerState.GetStat (STAT_ARMOR) = Client.Persistent.Inventory.Has(Armor);
		}
		else
		{
			Client.PlayerState.GetStat (STAT_ARMOR_ICON) = 0;
			Client.PlayerState.GetStat (STAT_ARMOR) = 0;
		}

		//
		// pickup message
		//
		if (Level.Frame > Client.Timers.PickupMessageTime)
		{
			Client.PlayerState.GetStat (STAT_PICKUP_ICON) = 0;
			Client.PlayerState.GetStat (STAT_PICKUP_STRING) = 0;
		}

		//
		// timers
		//
		if (Client.Timers.QuadDamage > Level.Frame)
		{
			Client.PlayerState.GetStat (STAT_TIMER_ICON) = NItems::Quad->GetIconIndex();
			Client.PlayerState.GetStat (STAT_TIMER) = (Client.Timers.QuadDamage - Level.Frame)/10;
		}
#if ROGUE_FEATURES
		else if (Client.Timers.Double > Level.Frame)
		{
			Client.PlayerState.GetStat (STAT_TIMER_ICON) = NItems::Double->GetIconIndex();
			Client.PlayerState.GetStat (STAT_TIMER) = (Client.Timers.Double - Level.Frame)/10;
		}
#endif
#if XATRIX_FEATURES
		else if (Client.Timers.QuadFire > Level.Frame)
		{
			Client.PlayerState.GetStat (STAT_TIMER_ICON) = NItems::QuadFire->GetIconIndex();
			Client.PlayerState.GetStat (STAT_TIMER) = (Client.Timers.QuadFire - Level.Frame)/10;
		}
#endif
		else if (Client.Timers.Invincibility > Level.Frame)
		{
			Client.PlayerState.GetStat (STAT_TIMER_ICON) = NItems::Invul->GetIconIndex();
			Client.PlayerState.GetStat (STAT_TIMER) = (Client.Timers.Invincibility - Level.Frame)/10;
		}
		else if (Client.Timers.EnvironmentSuit > Level.Frame)
		{
			Client.PlayerState.GetStat (STAT_TIMER_ICON) = NItems::EnvironmentSuit->GetIconIndex();
			Client.PlayerState.GetStat (STAT_TIMER) = (Client.Timers.EnvironmentSuit - Level.Frame)/10;
		}
		else if (Client.Timers.Rebreather > Level.Frame)
		{
			Client.PlayerState.GetStat (STAT_TIMER_ICON) = NItems::Rebreather->GetIconIndex();
			Client.PlayerState.GetStat (STAT_TIMER) = (Client.Timers.Rebreather - Level.Frame)/10;
		}
#if ROGUE_FEATURES
		else if (Client.OwnedSphere)
		{
			Client.PlayerState.GetStat (STAT_TIMER_ICON) = Client.OwnedSphere->Item->GetIconIndex();
			Client.PlayerState.GetStat (STAT_TIMER) = (Client.OwnedSphere->Wait - Level.Frame) / 10;
		}
		else if (Client.Timers.IR > Level.Frame)
		{
			Client.PlayerState.GetStat (STAT_TIMER_ICON) = NItems::IRGoggles->GetIconIndex();
			Client.PlayerState.GetStat (STAT_TIMER) = (Client.Timers.IR - Level.Frame)/10;
		}
#endif
		// Paril, show silencer
		else if (Client.Timers.SilencerShots)
		{
			Client.PlayerState.GetStat (STAT_TIMER_ICON) = NItems::Silencer->GetIconIndex();
			Client.PlayerState.GetStat (STAT_TIMER) = Client.Timers.SilencerShots;
		}
		// Paril
		else
		{
			Client.PlayerState.GetStat (STAT_TIMER_ICON) = 0;
			Client.PlayerState.GetStat (STAT_TIMER) = 0;
		}

		//
		// selected item
		//
		Client.PlayerState.GetStat (STAT_SELECTED_ICON) = (Client.Persistent.Inventory.SelectedItem == -1) ? 0 : GetItemByIndex(Client.Persistent.Inventory.SelectedItem)->GetIconIndex();
		Client.PlayerState.GetStat (STAT_SELECTED_ITEM) = Client.Persistent.Inventory.SelectedItem;

		//
		// layouts
		//
		Client.PlayerState.GetStat (STAT_LAYOUTS) = 0;

		if (Client.Persistent.Health <= 0 || Client.Respawn.MenuState.InMenu ||
			(Level.IntermissionTime || (Client.LayoutFlags & LF_SHOWSCORES)) || 
			(!(Game.GameMode & GAME_DEATHMATCH)) && (Client.LayoutFlags & LF_SHOWHELP))
			Client.PlayerState.GetStat (STAT_LAYOUTS) = Client.PlayerState.GetStat(STAT_LAYOUTS) | 1;
		if ((Client.LayoutFlags & LF_SHOWINVENTORY) && Client.Persistent.Health > 0)
			Client.PlayerState.GetStat (STAT_LAYOUTS) = Client.PlayerState.GetStat(STAT_LAYOUTS) | 2;

		//
		// frags
		//
		Client.PlayerState.GetStat (STAT_FRAGS) = Client.Respawn.Score;

		//
		// help icon / current weapon if not shown
		//
		Client.PlayerState.GetStat (STAT_HELPICON) = (Client.Persistent.HelpChanged && (Level.Frame&8) ) ? GameMedia.Hud.HelpPic :
			(((Client.Persistent.Hand == CENTER_HANDED || Client.PlayerState.GetFov() > 91)
			&& Client.Persistent.Weapon && Client.Persistent.Weapon->Item) ? Client.Persistent.Weapon->Item->GetIconIndex() : 0);

		Client.PlayerState.GetStat (STAT_SPECTATOR) = 0;
		Client.PlayerState.GetStat (STAT_TECH) = 0;

		if (Client.Persistent.Tech)
			Client.PlayerState.GetStat (STAT_TECH) = Client.Persistent.Tech->GetIconIndex();

	#if CLEANCTF_ENABLED
	//ZOID
		if (Game.GameMode & GAME_CTF)
			SetCTFStats();
	//ZOID
	#endif
	}
}

/*
===============
G_SetSpectatorStats
===============
*/
void CPlayerEntity::SetSpectatorStats ()
{
	if (!Client.Chase.Target)
		SetStats ();

	Client.PlayerState.GetStat (STAT_SPECTATOR) = 1;

	// layouts are independant in Spectator
	Client.PlayerState.GetStat (STAT_LAYOUTS) = 0;

	if (Client.Persistent.Health <= 0 || Level.IntermissionTime || (Client.LayoutFlags & LF_SHOWSCORES))
		Client.PlayerState.GetStat (STAT_LAYOUTS) = Client.PlayerState.GetStat(STAT_LAYOUTS) | 1;
	if ((Client.LayoutFlags & LF_SHOWINVENTORY) && Client.Persistent.Health > 0)
		Client.PlayerState.GetStat (STAT_LAYOUTS) = Client.PlayerState.GetStat(STAT_LAYOUTS) | 2;

	Client.PlayerState.GetStat (STAT_CHASE) = 
		(Client.Chase.Target && Client.Chase.Target->GetInUse()) ? (CS_PLAYERSKINS + (Client.Chase.Target->State.GetNumber() - 1)) : 0;
}

#if CLEANCTF_ENABLED
void CPlayerEntity::SetCTFStats()
{
	Client.PlayerState.GetStat (STAT_CTF_MATCH) = (ctfgame.match > MATCH_NONE) ? CONFIG_CTF_MATCH : 0;

	//ghosting
	if (Client.Respawn.CTF.Ghost)
	{
		Client.Respawn.CTF.Ghost->Score = Client.Respawn.Score;
		Client.Respawn.CTF.Ghost->name = Client.Persistent.Name;
		Client.Respawn.CTF.Ghost->number = State.GetNumber();
	}

	// logo headers for the frag display
	Client.PlayerState.GetStat (STAT_CTF_TEAM1_HEADER) = ImageIndex ("ctfsb1");
	Client.PlayerState.GetStat (STAT_CTF_TEAM2_HEADER) = ImageIndex ("ctfsb2");

	// if during intermission, we must blink the team header of the winning team
	if (Level.IntermissionTime && (Level.Frame & 8))
	{
		// blink 1/8th second
		// note that ctfgame.total[12] is set when we go to intermission
		if (ctfgame.team1 > ctfgame.team2)
			Client.PlayerState.GetStat (STAT_CTF_TEAM1_HEADER) = 0;
		else if (ctfgame.team2 > ctfgame.team1)
			Client.PlayerState.GetStat (STAT_CTF_TEAM2_HEADER) = 0;
		else if (ctfgame.total1 > ctfgame.total2) // frag tie breaker
			Client.PlayerState.GetStat (STAT_CTF_TEAM1_HEADER) = 0;
		else if (ctfgame.total2 > ctfgame.total1) 
			Client.PlayerState.GetStat (STAT_CTF_TEAM2_HEADER) = 0;
		else
		{
			// tie game!
			Client.PlayerState.GetStat (STAT_CTF_TEAM1_HEADER) = 0;
			Client.PlayerState.GetStat (STAT_CTF_TEAM2_HEADER) = 0;
		}
	}

	// figure out what icon to display for team logos
	// three states:
	//   flag at base
	//   flag taken
	//   flag dropped

	CFlagTransponder *Transponder = FindTransponder(CTF_TEAM1);
	if (!Transponder)
		Client.PlayerState.GetStat (STAT_CTF_TEAM1_PIC) = ImageIndex ("i_ctf1");
	else
	{
		switch (Transponder->Location)
		{
		case CFlagTransponder::FLAG_AT_BASE:
		default:
			Client.PlayerState.GetStat (STAT_CTF_TEAM1_PIC) = ImageIndex ("i_ctf1");
			break;
		case CFlagTransponder::FLAG_DROPPED:
			Client.PlayerState.GetStat (STAT_CTF_TEAM1_PIC) = ImageIndex ("i_ctf1d");
			break;
		case CFlagTransponder::FLAG_TAKEN:
			Client.PlayerState.GetStat (STAT_CTF_TEAM1_PIC) = ImageIndex ("i_ctf1t");
			break;
		};
	}

	Transponder = FindTransponder(CTF_TEAM2);
	if (!Transponder)
		Client.PlayerState.GetStat (STAT_CTF_TEAM2_PIC) = ImageIndex ("i_ctf2");
	else
	{
		switch (Transponder->Location)
		{
		case CFlagTransponder::FLAG_AT_BASE:
		default:
			Client.PlayerState.GetStat (STAT_CTF_TEAM2_PIC) = ImageIndex ("i_ctf2");
			break;
		case CFlagTransponder::FLAG_DROPPED:
			Client.PlayerState.GetStat (STAT_CTF_TEAM2_PIC) = ImageIndex ("i_ctf2d");
			break;
		case CFlagTransponder::FLAG_TAKEN:
			Client.PlayerState.GetStat (STAT_CTF_TEAM2_PIC) = ImageIndex ("i_ctf2t");
			break;
		};
	}

	if (ctfgame.last_flag_capture && Level.Frame - ctfgame.last_flag_capture < 50)
	{
		if (ctfgame.last_capture_team == CTF_TEAM1)
		{
			if (!(Level.Frame & 8))
				Client.PlayerState.GetStat (STAT_CTF_TEAM1_PIC) = 0;
		}
		else
		{
			if (!(Level.Frame & 8))
				Client.PlayerState.GetStat (STAT_CTF_TEAM2_PIC) = 0;
		}
	}

	Client.PlayerState.GetStat (STAT_CTF_TEAM1_CAPS) = ctfgame.team1;
	Client.PlayerState.GetStat (STAT_CTF_TEAM2_CAPS) = ctfgame.team2;

	Client.PlayerState.GetStat (STAT_CTF_FLAG_PIC) = 0;
	if (Client.Respawn.CTF.Team == CTF_TEAM1 &&
		(Client.Persistent.Flag == NItems::BlueFlag) &&
		(Level.Frame & 8))
		Client.PlayerState.GetStat (STAT_CTF_FLAG_PIC) = ImageIndex ("i_ctf2");

	else if (Client.Respawn.CTF.Team == CTF_TEAM2 &&
		(Client.Persistent.Flag == NItems::RedFlag) &&
		(Level.Frame & 8))
		Client.PlayerState.GetStat (STAT_CTF_FLAG_PIC) = ImageIndex ("i_ctf1");

	Client.PlayerState.GetStat (STAT_CTF_JOINED_TEAM1_PIC) = 0;
	Client.PlayerState.GetStat (STAT_CTF_JOINED_TEAM2_PIC) = 0;

	if (Client.Respawn.CTF.Team == CTF_TEAM1)
		Client.PlayerState.GetStat (STAT_CTF_JOINED_TEAM1_PIC) = ImageIndex ("i_ctfj");
	else if (Client.Respawn.CTF.Team == CTF_TEAM2)
		Client.PlayerState.GetStat (STAT_CTF_JOINED_TEAM2_PIC) = ImageIndex ("i_ctfj");

	Client.PlayerState.GetStat (STAT_CTF_ID_VIEW) = 0;
	if (Client.Respawn.CTF.IDState)
		CTFSetIDView();
}

bool loc_CanSee (IBaseEntity *targ, IBaseEntity *Inflictor);
void CPlayerEntity::CTFSetIDView()
{
	Client.PlayerState.GetStat (STAT_CTF_ID_VIEW) = 0;

	vec3f forward, oldForward;
	Client.ViewAngle.ToVectors(&forward, NULL, NULL);
	oldForward = forward;
	forward = (forward * 1024) + State.GetOrigin();

	CTrace tr (State.GetOrigin(), forward, this, CONTENTS_MASK_SOLID);
	if (tr.fraction < 1 && tr.ent && ((tr.ent - Game.Entities) >= 1 && (tr.ent - Game.Entities) <= Game.MaxClients))
	{
		Client.PlayerState.GetStat (STAT_CTF_ID_VIEW) = CS_PLAYERSKINS + (State.GetNumber() - 1);
		return;
	}

	forward = oldForward;
	CPlayerEntity *best = NULL;
	float bd = 0;
	for (sint32 i = 1; i <= Game.MaxClients; i++)
	{
		CPlayerEntity *who = entity_cast<CPlayerEntity>((Game.Entities + i)->Entity);
		if (!who->GetInUse() || who->GetSolid() == SOLID_NOT)
			continue;

		float d = forward | (who->State.GetOrigin() - State.GetOrigin()).GetNormalizedFast ();
		if (d > bd && loc_CanSee(this, who))
		{
			bd = d;
			best = who;
		}
	}
	if (bd > 0.90)
		Client.PlayerState.GetStat (STAT_CTF_ID_VIEW) = CS_PLAYERSKINS + (best->State.GetNumber() - 1);
}

void CPlayerEntity::CTFAssignGhost()
{
	CCTFGhost *Ghost = QNew (TAG_LEVEL) CCTFGhost;

	Ghost->team = Client.Respawn.CTF.Team;
	Ghost->Score = 0;

	// Find a key for the ghost
	sint32 code;
	while (true)
	{
		code = 10000 + (irandom(90000));

		if (ctfgame.Ghosts.find(code) == ctfgame.Ghosts.end())
			break;
	}

	ctfgame.Ghosts[code] = Ghost;

	Ghost->Player = this;
	Ghost->Code = code;
	Ghost->name = Client.Persistent.Name;
	Client.Respawn.CTF.Ghost = Ghost;
	PrintToClient (PRINT_CHAT, "Your ghost code is **** %d ****\n", code);
	PrintToClient (PRINT_HIGH, "If you lose connection, you can rejoin with your Score "
		"intact by typing \"ghost %d\".\n", code);
}
#endif

void CPlayerEntity::MoveToIntermission ()
{
	if (!(Game.GameMode & GAME_SINGLEPLAYER))
		Client.LayoutFlags |= LF_SHOWSCORES;

	State.GetOrigin() = Level.IntermissionOrigin;

	Client.PlayerState.GetPMove()->origin[0] = Level.IntermissionOrigin.X*8;
	Client.PlayerState.GetPMove()->origin[1] = Level.IntermissionOrigin.Y*8;
	Client.PlayerState.GetPMove()->origin[2] = Level.IntermissionOrigin.Z*8;
	Client.PlayerState.GetViewAngles() = Level.IntermissionAngles;
	Client.PlayerState.GetPMove()->pmType = PMT_FREEZE;
	Client.PlayerState.GetGunIndex () = 0;

	Client.PlayerState.GetViewBlend ().A = 0;
	Client.PlayerState.GetRdFlags () &= ~RDF_UNDERWATER;

	// clean up powerup info
	Client.Timers.QuadDamage = 0;
	Client.Timers.Invincibility = 0;
	Client.Timers.Rebreather = 0;
	Client.Timers.EnvironmentSuit = 0;

#if ROGUE_FEATURES
	Client.PlayerState.GetRdFlags() &= ~RDF_IRGOGGLES;		// PGM
	Client.Timers.IR = 0;					// PGM
	Client.Timers.Nuke = 0;					// PMM
	Client.Timers.Double = 0;				// PMM
#endif

#if XATRIX_FEATURES
	Client.Timers.QuadFire = 0;
#endif
	Client.Grenade.BlewUp = Client.Grenade.Thrown = false;
	Client.Grenade.Time = 0;

	ViewHeight = 0;
	State.GetModelIndex() = State.GetModelIndex(2) = State.GetModelIndex(3) = 0;
	State.GetEffects() = 0;
	State.GetSound() = 0;
	GetSolid() = SOLID_NOT;

	// add the layout
	Enemy = NULL;
	if (!(Game.GameMode & GAME_SINGLEPLAYER))
		DeathmatchScoreboardMessage (true);
}

bool CPlayerEntity::ApplyStrengthSound()
{
	if (Client.Persistent.Tech && (Client.Persistent.Tech->GetTechNumber() == CTFTECH_STRENGTH_NUMBER))
	{
		if (Client.Tech.SoundTime < Level.Frame)
		{
			Client.Tech.SoundTime = Level.Frame + 10;
			PlaySound (CHAN_AUTO, SoundIndex((Client.Timers.QuadDamage > Level.Frame) ? "ctf/tech2x.wav" : "ctf/tech2.wav"), (Client.Timers.SilencerShots) ? 51 : 255);
		}
		return true;
	}
	return false;
}

bool CPlayerEntity::ApplyHaste()
{
	return (Client.Persistent.Tech && (Client.Persistent.Tech->GetTechNumber() == CTFTECH_HASTE_NUMBER));
}

void CPlayerEntity::ApplyHasteSound()
{
	if (Client.Persistent.Tech && (Client.Persistent.Tech->GetTechNumber() == CTFTECH_HASTE_NUMBER) && Client.Tech.SoundTime < Level.Frame)
	{
		Client.Tech.SoundTime = Level.Frame + 10;
		PlaySound (CHAN_AUTO, SoundIndex("ctf/tech3.wav"), (Client.Timers.SilencerShots) ? 51 : 255);
	}
}

bool CPlayerEntity::HasRegeneration()
{
	return (Client.Persistent.Tech && (Client.Persistent.Tech->GetTechNumber() == CTFTECH_REGEN_NUMBER));
}


/*
==================
LookAtKiller
==================
*/
void CPlayerEntity::LookAtKiller (IBaseEntity *Inflictor, IBaseEntity *Attacker)
{
	vec3f dir;
	if (Attacker && (Attacker != World) && (Attacker != this))
		dir = Attacker->State.GetOrigin() - State.GetOrigin();
	else if (Inflictor && (Inflictor != World) && (Inflictor != this))
		dir = Inflictor->State.GetOrigin() - State.GetOrigin();
	else
	{
		Client.KillerYaw = State.GetAngles().Y;
		return;
	}

	Client.KillerYaw = (dir.X) ? (180 / M_PI * atan2f(dir.Y, dir.X)) : (((dir.Y > 0) ? 90 : (dir.Y < 0 ? 270 : 0)));
	if (Client.KillerYaw < 0)
		Client.KillerYaw += 360;
}

void CPlayerEntity::DeadDropTech ()
{
	if (!Client.Persistent.Tech)
		return;

	CItemEntity *dropped = Client.Persistent.Tech->DropItem(this);
	// hack the velocity to make it bounce random
	dropped->Velocity.X = (frand() * 600) - 300;
	dropped->Velocity.Y = (frand() * 600) - 300;
	dropped->NextThink = Level.Frame + CTF_TECH_TIMEOUT;
	dropped->SetOwner (NULL);
	Client.Persistent.Inventory.Set(Client.Persistent.Tech, 0);

	Client.Persistent.Tech = NULL;
}

void CPlayerEntity::TossClientWeapon ()
{
	if (!(Game.GameMode & GAME_DEATHMATCH))
		return;

	CWeaponItem *Item = (Client.Persistent.Weapon) ? Client.Persistent.Weapon->Item : NULL;

	if (Item)
	{
		if (!Item->Ammo)
			Item = NULL;
		if (Item && !Client.Persistent.Inventory.Has(Item->Ammo))
			Item = NULL;
		if (Item && !Item->WorldModel)
			Item = NULL;
	}

	bool quad = (!DeathmatchFlags.dfQuadDrop.IsEnabled()) ? false : (bool)(Client.Timers.QuadDamage > (Level.Frame + 10));
#if XATRIX_FEATURES
	bool quadfire = (!DeathmatchFlags.dfQuadFireDrop.IsEnabled()) ? false : (bool)(Client.Timers.QuadFire > (Level.Frame + 10));
#endif
	float spread = (Item && quad) ? 22.5f : 0.0f;

	if (Item)
	{
		Client.ViewAngle.Y -= spread;
		CItemEntity *drop = Item->DropItem (this);
		Client.ViewAngle.Y += spread;
		drop->SpawnFlags |= DROPPED_PLAYER_ITEM;
		drop->AmmoCount = Client.Persistent.Weapon->Item->Ammo->Quantity;
	}

	if (quad)
	{
		Client.ViewAngle.Y += spread;
		CItemEntity *drop = NItems::Quad->DropItem (this);
		Client.ViewAngle.Y -= spread;
		drop->SpawnFlags |= DROPPED_PLAYER_ITEM;

		drop->NextThink = Level.Frame + (Client.Timers.QuadDamage - Level.Frame);
		drop->ThinkState = ITS_FREE;
	}

#if XATRIX_FEATURES
	if (quadfire)
	{
		Client.ViewAngle.Y += spread;
		CItemEntity *drop = NItems::QuadFire->DropItem (this);
		Client.ViewAngle.Y -= spread;
		drop->SpawnFlags |= DROPPED_PLAYER_ITEM;

		drop->NextThink = Level.Frame + (Client.Timers.QuadFire - Level.Frame);
		drop->ThinkState = ITS_FREE;
	}
#endif
}

#if USE_EXTENDED_GAME_IMPORTS
CPlayerEntity	*pm_passent;

// pmove doesn't need to know about passent and contentmask
cmTrace_t	PM_trace (vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end)
{
_CC_DISABLE_DEPRECATION
	return gi.trace(start, mins, maxs, end, pm_passent->gameEntity, (pm_passent->Health > 0) ? CONTENTS_MASK_PLAYERSOLID : CONTENTS_MASK_DEADSOLID);
_CC_ENABLE_DEPRECATION
}
#endif

void CPlayerEntity::ClientThink (userCmd_t *ucmd)
{
#if USE_EXTENDED_GAME_IMPORTS
	pMove_t		pm;
#else
	pMoveNew_t	pm;
#endif

	Level.CurrentEntity = this;

	if (Level.IntermissionTime)
	{
		Client.PlayerState.GetPMove()->pmType = PMT_FREEZE;
		// can exit intermission after five seconds
		if (Level.Frame > Level.IntermissionTime + 50 
			&& (ucmd->buttons & BUTTON_ANY) )
			Level.ExitIntermission = true;
		return;
	}

#if USE_EXTENDED_GAME_IMPORTS
	pm_passent = this;
#endif

//ZOID
	static sint32 oldbuttons;
	oldbuttons = Client.Buttons;
	Client.Buttons = ucmd->buttons;
	Client.LatchedButtons |= Client.Buttons & ~oldbuttons;

	if (Client.Chase.Target)
	{
		for (sint32 i = 0; i < 3; i++)
			Client.Respawn.CmdAngles[i] = SHORT2ANGLE(ucmd->angles[i]);

		if (Client.LatchedButtons & BUTTON_ATTACK)
		{
			if (Client.Chase.Target)
				ChaseNext();
			else
				GetChaseTarget();
			Client.LatchedButtons &= ~BUTTON_ATTACK;
		}

		if (ucmd->upMove >= 10)
		{
			if (!(Client.PlayerState.GetPMove()->pmFlags & PMF_JUMP_HELD))
			{
				Client.PlayerState.GetPMove()->pmFlags |= PMF_JUMP_HELD;

				Client.LatchedButtons = 0;

				if (Client.Chase.Target)
				{
					Client.Chase.Target = NULL;
					Client.PlayerState.GetPMove()->pmFlags &= ~PMF_NO_PREDICTION;
					Client.PlayerState.GetGunIndex () = 0;
					Client.PlayerState.GetGunFrame() = 0;
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
	Mem_Zero (&pm, sizeof(pm));

	if (NoClip)
		Client.PlayerState.GetPMove()->pmType = PMT_SPECTATOR;
	else if (State.GetModelIndex() != 255)
		Client.PlayerState.GetPMove()->pmType = PMT_GIB;
	else if (DeadFlag)
		Client.PlayerState.GetPMove()->pmType = PMT_DEAD;
	else
		Client.PlayerState.GetPMove()->pmType = PMT_NORMAL;

	Client.PlayerState.GetPMove()->gravity = CvarList[CV_GRAVITY].Float() * GravityMultiplier;
	pm.state = *Client.PlayerState.GetPMove();

	for (sint32 i = 0; i < 3; i++)
	{
		pm.state.origin[i] = State.GetOrigin()[i]*8;
		pm.state.velocity[i] = Velocity[i]*8;
	}

	if (memcmp (&Client.OldPMove, &pm.state, sizeof(pm.state)))
		pm.snapInitial = true;

	pm.cmd = *ucmd;

#if USE_EXTENDED_GAME_IMPORTS
	pm.trace = PM_trace;
	pm.pointContents = gi.pointcontents;
#endif

	// perform a pmove
#if USE_EXTENDED_GAME_IMPORTS
	gi.Pmove (&pm);
#else
	SV_Pmove (this, &pm, CvarList[CV_AIRACCELERATE].Float());
#endif

	// save results of pmove
	Client.PlayerState.SetPMove (&pm.state);
	Client.OldPMove = pm.state;

	State.GetOrigin().Set (pm.state.origin[0]*0.125, pm.state.origin[1]*0.125, pm.state.origin[2]*0.125);
	for (sint32 i = 0; i < 3; i++)
		Velocity[i] = pm.state.velocity[i]*0.125;

	GetMins() = pm.mins;
	GetMaxs() = pm.maxs;

	for (sint32 i = 0; i < 3; i++)
	Client.Respawn.CmdAngles[i] = SHORT2ANGLE(ucmd->angles[i]);

	if (GroundEntity && !pm.groundEntity && Velocity[2] > 0 && (pm.cmd.upMove >= 10) && (pm.waterLevel == WATER_NONE))
	{
		PlaySound (CHAN_VOICE, GameMedia.Player.Jump);
		PlayerNoiseAt (State.GetOrigin(), PNOISE_SELF);
	}

	ViewHeight = pm.viewHeight;

#if ROGUE_FEATURES
		if (Flags & FL_SAM_RAIMI)
			ViewHeight = 8;
#endif

	WaterInfo.Level = pm.waterLevel;
	WaterInfo.Type = pm.waterType;
	GroundEntity = (pm.groundEntity) ? pm.groundEntity->Entity : NULL;
	if (GroundEntity)
		GroundEntityLinkCount = GroundEntity->GetLinkCount();

	if (DeadFlag)
		Client.PlayerState.GetViewAngles().Set (-15, Client.KillerYaw, 40);
	else
	{
		Client.ViewAngle.Set (pm.viewAngles);
		Client.PlayerState.GetViewAngles().Set (pm.viewAngles);
	}

#if CLEANCTF_ENABLED
//ZOID
	if (Client.Grapple.Entity)
		Client.Grapple.Entity->GrapplePull ();
//ZOID
#endif

	Link ();
	GravityMultiplier = 1.0f;

	if (!NoClip && !(CvarList[CV_MAP_DEBUG].Boolean()))
		G_TouchTriggers (this);

	// touch other objects
	if (!CvarList[CV_MAP_DEBUG].Boolean())
	{
		for (sint32 i = 0; i < pm.numTouch; i++)
		{
			edict_t *Other = pm.touchEnts[i];
			if (Other->Entity)
			{
				if ((Other->Entity->EntityFlags & ENT_TOUCHABLE) && Other->Entity->GetInUse())
				{
					ITouchableEntity *Touchered = entity_cast<ITouchableEntity>(Other->Entity);

					if (Touchered->Touchable)
						Touchered->Touch (this, NULL, NULL);
				}
				continue;
			}
		}
	}

	// save light level the player is standing on for
	// monster sighting AI
	// Paril: Removed. See definition of userCmd_t::lightlevel for more info.

	if (Client.Respawn.Spectator
#if CLEANCTF_ENABLED
		|| ((Game.GameMode & GAME_CTF) && NoClip)
#endif
		)
	{
		if (Client.LatchedButtons & BUTTON_ATTACK)
		{
			if (Client.Chase.Target)
				ChaseNext();
			else
				GetChaseTarget();
		}

		if (ucmd->upMove >= 10)
		{
			if (!(Client.PlayerState.GetPMove()->pmFlags & PMF_JUMP_HELD))
			{
				Client.PlayerState.GetPMove()->pmFlags |= PMF_JUMP_HELD;

				Client.LatchedButtons = 0;

				if (Client.Chase.Target)
				{
					Client.Chase.Target = NULL;
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
	for (sint32 i = 1; i <= Game.MaxClients; i++)
	{
		CPlayerEntity *Other = entity_cast<CPlayerEntity>((Game.Entities + i)->Entity);
		if (Other->GetInUse() && Other->Client.Chase.Target == this)
			Other->UpdateChaseCam();
	}
}

#if CLEANCTF_ENABLED
void CPlayerEntity::CTFAssignTeam()
{
	sint32 i;
	sint32 team1count = 0, team2count = 0;

	Client.Respawn.CTF.State = 0;

	if (!DeathmatchFlags.dfCtfForceJoin.IsEnabled())
	{
		Client.Respawn.CTF.Team = CTF_NOTEAM;
		return;
	}

	for (i = 1; i <= Game.MaxClients; i++)
	{
		CPlayerEntity *player = entity_cast<CPlayerEntity>(Game.Entities[i].Entity);

		if (!player->GetInUse() || player == this)
			continue;

		switch (player->Client.Respawn.CTF.Team)
		{
		case CTF_TEAM1:
			team1count++;
			break;
		case CTF_TEAM2:
			team2count++;
		}
	}
	if (team1count < team2count)
		Client.Respawn.CTF.Team = CTF_TEAM1;
	else if (team2count < team1count)
		Client.Respawn.CTF.Team = CTF_TEAM2;
	else if (irandom(2))
		Client.Respawn.CTF.Team = CTF_TEAM1;
	else
		Client.Respawn.CTF.Team = CTF_TEAM2;
}
#endif

void CPlayerEntity::InitResp ()
{
#if CLEANCTF_ENABLED
//ZOID
	sint32 Team = Client.Respawn.CTF.Team;
	bool IDState = Client.Respawn.CTF.IDState;
//ZOID
#endif

	Client.Respawn.Clear ();

#if CLEANCTF_ENABLED
//ZOID
	Client.Respawn.CTF.Team = Team;
	Client.Respawn.CTF.IDState = IDState;
//ZOID
#endif

	Client.Respawn.EnterFrame = Level.Frame;
	Client.Respawn.CoopRespawn = Client.Persistent;

#if CLEANCTF_ENABLED
//ZOID
	if ((Game.GameMode & GAME_CTF) && Client.Respawn.CTF.Team < CTF_TEAM1)
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
CPersistentData *SavedClients;

void CPlayerEntity::BackupClientData ()
{
	SavedClients = QNew (TAG_GENERIC) CPersistentData[Game.MaxClients];
	for (sint32 i = 0; i < Game.MaxClients; i++)
	{
		if (!Game.Entities[1+i].Entity)
			return; // Not set up

		CPlayerEntity *Player = entity_cast<CPlayerEntity>(Game.Entities[1+i].Entity);

		SavedClients[i] = Player->Client.Persistent;
		SavedClients[i].Health = Player->Health;
		SavedClients[i].MaxHealth = Player->MaxHealth;
		SavedClients[i].SavedFlags = (Player->Flags & (FL_GODMODE|FL_NOTARGET|FL_POWER_ARMOR));
		if (Game.GameMode & GAME_COOPERATIVE)
			SavedClients[i].Score = Player->Client.Respawn.Score;
	}
}

void CPlayerEntity::SaveClientData ()
{
	for (sint32 i = 0; i < Game.MaxClients; i++)
	{
		if (!Game.Entities[1+i].Entity)
			return; // Not set up

		CPlayerEntity *Player = entity_cast<CPlayerEntity>(Game.Entities[1+i].Entity);

		Player->Client.Persistent.Health = Player->Health;
		Player->Client.Persistent.MaxHealth = Player->MaxHealth;
		Player->Client.Persistent.SavedFlags = (Player->Flags & (FL_GODMODE|FL_NOTARGET|FL_POWER_ARMOR));
		if (Game.GameMode & GAME_COOPERATIVE)
			Player->Client.Persistent.Score = Player->Client.Respawn.Score;
	}
}

void CPlayerEntity::RestoreClientData ()
{
	for (sint32 i = 0; i < Game.MaxClients; i++)
	{
		// Reset the entity states
		//Game.Entities[i+1].Entity = SavedClients[i];
		CPlayerEntity *Player = entity_cast<CPlayerEntity>(Game.Entities[i+1].Entity);
		//memcpy (&Player->Client.Persistent, &SavedClients[i], sizeof(CPersistentData));
		Player->Client.Persistent = SavedClients[i];
		Player->Health = SavedClients[i].Health;
		Player->MaxHealth = SavedClients[i].MaxHealth;
		Player->Flags = SavedClients[i].SavedFlags;
		if (Game.GameMode & GAME_COOPERATIVE)
			Player->Client.Respawn.Score = SavedClients[i].Score;
		Game.Entities[i+1].client = Game.Clients + i;
	}

	QDelete[] SavedClients;
	SavedClients = NULL;
}

vec3f VelocityForDamage (sint32 Damage);
void CPlayerEntity::TossHead (sint32 Damage)
{
	if (irandom(2))
	{
		State.GetModelIndex() = GameMedia.Gib_Head[1];
		State.GetSkinNum() = 1;		// second skin is player
	}
	else
	{
		State.GetModelIndex() = GameMedia.Gib_Skull;
		State.GetSkinNum() = 0;
	}

	State.GetFrame() = 0;
	GetMins().Set (-16, -16, 0);
	GetMaxs().Set (16);

	CanTakeDamage = false;
	GetSolid() = SOLID_NOT;
	State.GetEffects() = EF_GIB;
	State.GetSound() = 0;
	Flags |= FL_NO_KNOCKBACK;

	Velocity += VelocityForDamage (Damage);

	Link ();
}

EMeansOfDeath meansOfDeath;
void Cmd_Help_f (CPlayerEntity *Player);

void CPlayerEntity::Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &point)
{
	CanTakeDamage = true;
	TossPhysics = true;

	State.GetModelIndex(2) = State.GetModelIndex(3) = 0;	// remove linked weapon model and ctf flag

	State.GetAngles().Set (0, State.GetAngles().Y, 0);

	State.GetSound() = 0;
	Client.WeaponSound = 0;

	GetMaxs().Z = -8;

	GetSvFlags() |= SVF_DEADMONSTER;

	if (!DeadFlag)
	{
		Client.Timers.RespawnTime = Level.Frame + 10;
		LookAtKiller (Inflictor, Attacker);
		Client.PlayerState.GetPMove()->pmType = PMT_DEAD;
		Obituary (Attacker);

#if CLEANCTF_ENABLED
		if (Attacker->EntityFlags & ENT_PLAYER)
		{
			CPlayerEntity *PlayerAttacker = entity_cast<CPlayerEntity>(Attacker);
//ZOID
			// if at start and same team, clear
			if ((Game.GameMode & GAME_CTF) && (meansOfDeath == MOD_TELEFRAG) &&
				(Client.Respawn.CTF.State < 2) &&
				(Client.Respawn.CTF.Team == PlayerAttacker->Client.Respawn.CTF.Team))
			{
				PlayerAttacker->Client.Respawn.Score--;
				Client.Respawn.CTF.State = 0;
			}

			CTFFragBonuses(this, PlayerAttacker);
		}

//ZOID
#endif
		TossClientWeapon ();

#if CLEANCTF_ENABLED
//ZOID
		if ((Game.GameMode & GAME_CTF) || Client.Grapple.Entity || Client.Persistent.Flag)
		{
			CGrapple::PlayerResetGrapple(this);
			CTFDeadDropFlag();
		}
//ZOID
#endif
		if (
#if CLEANCTF_ENABLED
			(Game.GameMode & GAME_CTF) || 
#endif
			DeathmatchFlags.dfDmTechs.IsEnabled()) 
			DeadDropTech();

		if (Game.GameMode & GAME_DEATHMATCH)
			Cmd_Help_f (this);		// show scores

		// clear inventory
		// this is kind of ugly, but it's how we want to handle keys in coop
		for (sint32 n = 0; n < GetNumItems(); n++)
		{
			if ((Game.GameMode & GAME_COOPERATIVE) && (GetItemByIndex(n)->Flags & ITEMFLAG_KEY))
				Client.Respawn.CoopRespawn.Inventory.Set(n, Client.Persistent.Inventory.Has(n));
			Client.Persistent.Inventory.Set(n, 0);
		}
	}

	// remove powerups
	Client.Timers.QuadDamage = 0;
	Client.Timers.Invincibility = 0;
	Client.Timers.Rebreather = 0;
	Client.Timers.EnvironmentSuit = 0;
	Flags &= ~FL_POWER_ARMOR;

#if ROGUE_FEATURES
	Client.Timers.Double = 0;

	// if there's a sphere around, let it know the player died.
	// vengeance and hunter will die if they're not attacking,
	// defender should always die
	if (Client.OwnedSphere)
		Client.OwnedSphere->Die (this, this, 0, vec3fOrigin);

	// if we've been killed by the tracker, GIB!
	if ((meansOfDeath & ~MOD_FRIENDLY_FIRE) == MOD_TRACKER)
	{
		Health = -100;
		Damage = 400;
	}

	// make sure no trackers are still hurting us.
	if (Client.Timers.Tracker)
		RemoveAttackingPainDaemons ();
	
	// if we got obliterated by the nuke, don't gib
	if ((Health < -80) && (meansOfDeath == MOD_NUKE))
		Flags |= FL_NOGIB;
#endif

	if (Health < -40)
	{
#if ROGUE_FEATURES
		// don't toss gibs if we got vaped by the nuke
		if (!(Flags & FL_NOGIB))
		{
#endif
			// gib
			PlaySound (CHAN_BODY, SoundIndex ("misc/udeath.wav"));
			for (sint32 n = 0; n < 4; n++)
				CGibEntity::Spawn (this, GameMedia.Gib_SmallMeat, Damage, GIB_ORGANIC);
			TossHead (Damage);
#if ROGUE_FEATURES
		}
		Flags &= ~FL_NOGIB;
#endif

		CanTakeDamage = false;
//ZOID
		Client.Anim.Priority = ANIM_DEATH;
		Client.Anim.EndFrame = 0;
//ZOID
	}
	else
	{	// normal death
		if (!DeadFlag)
		{
			static sint32 i;

			i = (i+1)%3;
			// start a death animation
			Client.Anim.Priority = ANIM_DEATH;
			if (Client.PlayerState.GetPMove()->pmFlags & PMF_DUCKED)
			{
				State.GetFrame() = FRAME_crdeath1 - 1;
				Client.Anim.EndFrame = FRAME_crdeath5;
			}
			else
			{
				switch (i)
				{
				case 0:
					State.GetFrame() = FRAME_death101 - 1;
					Client.Anim.EndFrame = FRAME_death106;
					break;
				case 1:
					State.GetFrame() = FRAME_death201 - 1;
					Client.Anim.EndFrame = FRAME_death206;
					break;
				case 2:
					State.GetFrame() = FRAME_death301 - 1;
					Client.Anim.EndFrame = FRAME_death308;
					break;
				}
			}
			PlaySound (CHAN_VOICE, GameMedia.Player.Death[(irandom(4))]);
		}
	}

	DeadFlag = true;

	Link ();
};

void CPlayerEntity::PrintToClient (EGamePrintLevel printLevel, const char *fmt, ...)
{
	va_list		argptr;
	static char	text[MAX_COMPRINT];

	va_start (argptr, fmt);
	vsnprintf (text, sizeof(text), fmt, argptr);
	va_end (argptr);

	ClientPrintf (gameEntity, printLevel, "%s", text);
};

void CPlayerEntity::UpdateChaseCam()
{
	vec3f forward, right;
	CPlayerEntity *targ;

	// is our chase target gone?
	if (!Client.Chase.Target->GetInUse()
		|| Client.Chase.Target->Client.Respawn.Spectator || Client.Chase.Target->Client.Chase.Target)
	{
		CPlayerEntity *old = Client.Chase.Target;
		ChaseNext();
		if (Client.Chase.Target == old)
		{
			Client.Chase.Target = NULL;
			Client.PlayerState.GetPMove()->pmFlags &= ~PMF_NO_PREDICTION;
			return;
		}
	}

	targ = Client.Chase.Target;
	Client.PlayerState.GetGunIndex() = 0;

	switch (Client.Chase.Mode)
	{
	case 0:
		{
			vec3f ownerv = targ->State.GetOrigin();
			vec3f oldgoal = State.GetOrigin();
			ownerv.Z += targ->ViewHeight;

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

			CTrace trace (ownerv, o, targ, CONTENTS_MASK_SOLID);

			vec3f goal = trace.EndPos.MultiplyAngles (2, forward);
			o = goal + vec3f(0, 0, 6);
			trace (goal, o, targ, CONTENTS_MASK_SOLID);

			if (trace.fraction < 1)
				goal = trace.EndPos - vec3f(0, 0, 6);

			o = goal - vec3f(0, 0, 6);
			trace (goal, o, targ, CONTENTS_MASK_SOLID);

			if(trace.fraction < 1)
				goal = trace.EndPos + vec3f(0, 0, 6);

			if (targ->DeadFlag)
				Client.PlayerState.GetPMove()->pmType = PMT_DEAD;
			else
				Client.PlayerState.GetPMove()->pmType = PMT_FREEZE;

			State.GetOrigin() = goal;

			for (sint32 i = 0; i < 3; i++)
				Client.PlayerState.GetPMove()->deltaAngles[i] = ANGLE2SHORT(targ->Client.ViewAngle[i] - Client.Respawn.CmdAngles[i]);

			if (targ->DeadFlag)
				Client.PlayerState.GetViewAngles().Set (-15, targ->Client.KillerYaw, 40);
			else
			{
				angles = targ->Client.ViewAngle + targ->Client.KickAngles;
				Client.PlayerState.GetViewAngles().Set (angles);
				Client.ViewAngle = angles;
			}
		}
		break;
	case 1:
		{
			Client.PlayerState.GetFov () = 90;

			if(Client.Respawn.CmdAngles.X > 89)
				Client.Respawn.CmdAngles.X = 89;

			if(Client.Respawn.CmdAngles.X < -89)
				Client.Respawn.CmdAngles.X = -89;

			vec3f ownerv = targ->State.GetOrigin() + vec3f(0, 0, targ->ViewHeight);

			Client.PlayerState.GetViewAngles().ToVectors (&forward, &right, NULL);
			forward.NormalizeFast ();
			vec3f o = ownerv.MultiplyAngles (-150, forward);

			if (!targ->GroundEntity)
				o.Z += 16;

			CTrace trace(ownerv, o, targ, CONTENTS_MASK_SOLID);

			vec3f goal = trace.EndPos.MultiplyAngles (2, forward);
			o = goal + vec3f(0, 0, 6);

			trace (goal, o, targ, CONTENTS_MASK_SOLID);

			if(trace.fraction < 1)
				goal = trace.EndPos - vec3f(0, 0, 6);

			o = goal - vec3f(0, 0, 6);

			trace (goal, o, targ, CONTENTS_MASK_SOLID);

			if(trace.fraction < 1)
				goal = trace.EndPos + vec3f(0, 0, 6);

			if (targ->DeadFlag)
				Client.PlayerState.GetPMove()->pmType = PMT_DEAD;
			else
				Client.PlayerState.GetPMove()->pmType = PMT_FREEZE;

			State.GetOrigin() = goal;

			for (sint32 i = 0; i < 3; i++)
				Client.PlayerState.GetPMove()->deltaAngles[i] = ANGLE2SHORT(targ->Client.ViewAngle[i] - Client.Respawn.CmdAngles[i]);

			Client.PlayerState.GetViewAngles() = Client.Respawn.CmdAngles;
		}
		break;
	default:
		{
			vec3f ownerv = targ->State.GetOrigin();
			vec3f angles = targ->Client.ViewAngle;

			angles.ToVectors (&forward, &right, NULL);
			forward.NormalizeFast ();
			vec3f o = ownerv.MultiplyAngles (16, forward);
			o.Z += targ->ViewHeight;

			State.GetOrigin() = o;
			Client.PlayerState.GetFov () = targ->Client.PlayerState.GetFov();

			Client.PlayerState.GetGunIndex() = targ->Client.PlayerState.GetGunIndex();
			Client.PlayerState.GetGunAngles() = targ->Client.PlayerState.GetGunAngles();
			Client.PlayerState.GetGunFrame() = targ->Client.PlayerState.GetGunFrame();
			Client.PlayerState.GetGunOffset() = targ->Client.PlayerState.GetGunOffset();

			for (sint32 i = 0; i < 3; i++)
				Client.PlayerState.GetPMove()->deltaAngles[i] = ANGLE2SHORT(targ->Client.ViewAngle[i] - Client.Respawn.CmdAngles[i]);

			if (targ->DeadFlag)
				Client.PlayerState.GetViewAngles().Set (-15, targ->Client.KillerYaw, 40);
			else
			{
				angles = targ->Client.ViewAngle + targ->Client.KickAngles;
				Client.PlayerState.GetViewAngles().Set (angles);
				Client.ViewAngle = angles;
			}
		}
		break;
	};

	ViewHeight = 0;
	Client.PlayerState.GetPMove()->pmFlags |= PMF_NO_PREDICTION;
	Link ();

	if ((!(Client.LayoutFlags & LF_SHOWSCORES) && !Client.Respawn.MenuState.InMenu &&
		!(Client.LayoutFlags & LF_SHOWINVENTORY) && !(Client.LayoutFlags & LF_SHOWHELP) &&
		!(Level.Frame & 31)) || (Client.LayoutFlags & LF_UPDATECHASE))
	{
		CStatusBar Chasing;
		char temp[128];
		Q_snprintfz (temp, sizeof(temp), "Chasing %s\n%s", targ->Client.Persistent.Name.c_str(), (Client.Chase.Mode == 0) ? "Tight Chase" : ((Client.Chase.Mode == 1) ? "Freeform Chase" : "FPS Chase"));

		Chasing.AddVirtualPoint_X (0);
		Chasing.AddVirtualPoint_Y (-68);
		Chasing.AddString (temp, true, false);
		Chasing.SendMsg (this, false);

		Client.LayoutFlags &= ~LF_UPDATECHASE;
	}
}

void CPlayerEntity::ChaseNext()
{
	if (!Client.Chase.Target)
		return;

	switch (Client.Chase.Mode)
	{
	case 0:
	case 1:
		Client.Chase.Mode++;
		Client.LayoutFlags |= LF_UPDATECHASE;
		return;
	};

	sint32 i = Client.Chase.Target->State.GetNumber();
	CPlayerEntity *e;
	do {
		i++;
		if (i > Game.MaxClients)
			i = 1;
		e = entity_cast<CPlayerEntity>(Game.Entities[i].Entity);
		if (!e->GetInUse())
			continue;
		if (e->NoClip)
			continue;
		if (!e->Client.Respawn.Spectator)
			break;
	} while (e != Client.Chase.Target);

	Client.Chase.Target = e;
	Client.Chase.Mode = 0;
	Client.LayoutFlags |= LF_UPDATECHASE;
}

void CPlayerEntity::ChasePrev()
{
	if (!Client.Chase.Target)
		return;

	sint32 i = Client.Chase.Target->State.GetNumber();
	CPlayerEntity *e;
	do {
		i--;
		if (i < 1)
			i = Game.MaxClients;
		e = entity_cast<CPlayerEntity>(Game.Entities[i].Entity);
		if (!e->GetInUse())
			continue;
		if (e->NoClip)
			continue;
		if (!e->Client.Respawn.Spectator)
			break;
	} while (e != Client.Chase.Target);

	Client.Chase.Target = e;
	Client.Chase.Mode = 0;
	Client.LayoutFlags |= LF_UPDATECHASE;
}

void CPlayerEntity::GetChaseTarget()
{
	for (sint32 i = 1; i <= Game.MaxClients; i++)
	{
		CPlayerEntity *Other = entity_cast<CPlayerEntity>(Game.Entities[i].Entity);
		if (Other->GetInUse() && !Other->Client.Respawn.Spectator && !Other->NoClip)
		{
			Client.Chase.Target = Other;
			Client.LayoutFlags |= LF_UPDATECHASE;
			Client.Chase.Mode = 0;
			UpdateChaseCam();
			return;
		}
	}
	PrintToClient(PRINT_CENTER, "No other players to chase.");
}

void CPlayerEntity::P_ProjectSource (vec3f distance, vec3f &forward, vec3f &right, vec3f &result)
{
	switch (Client.Persistent.Hand)
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

void CPlayerEntity::PlayerNoiseAt (vec3f Where, sint32 type)
{
	if (type == PNOISE_WEAPON)
	{
		if (Client.Timers.SilencerShots)
		{
			Client.Timers.SilencerShots--;
			return;
		}
	}

	if (Game.GameMode & GAME_DEATHMATCH)
		return;

	if (Flags & FL_NOTARGET)
		return;

	if (!Client.mynoise)
	{
		CPlayerNoise *noise = QNewEntityOf CPlayerNoise;
		noise->ClassName = "player_noise";
		noise->GetMins().Set (-8);
		noise->GetMaxs().Set (8);
		noise->SetOwner (this);
		noise->GetSvFlags() = SVF_NOCLIENT;
		Client.mynoise = noise;

		noise = QNewEntityOf CPlayerNoise;
		noise->ClassName = "player_noise";
		noise->GetMins().Set (-8);
		noise->GetMaxs().Set (8);
		noise->SetOwner (this);
		noise->GetSvFlags() = SVF_NOCLIENT;
		Client.mynoise2 = noise;
	}

	CPlayerNoise *noise;
	if (type == PNOISE_SELF || type == PNOISE_WEAPON)
	{
		noise = entity_cast<CPlayerNoise>(Client.mynoise);
		Level.SoundEntity = noise;
		Level.SoundEntityFrame = Level.Frame;
	}
	else // type == PNOISE_IMPACT
	{
		noise = entity_cast<CPlayerNoise>(Client.mynoise2);
		Level.SoundEntity2 = noise;
		Level.SoundEntity2Frame = Level.Frame;
	}

	noise->State.GetOrigin() = Where;
	noise->GetAbsMin() = (Where - noise->GetMins());
	noise->GetAbsMax() = (Where + noise->GetMaxs());
	noise->Time = Level.Frame;
	noise->Link ();
}

void CPlayerEntity::BeginDeathmatch ()
{
_CC_DISABLE_DEPRECATION
	G_InitEdict (gameEntity);
_CC_ENABLE_DEPRECATION

	InitResp();

	// locate ent at a spawn point
	PutInServer();

	if (Level.IntermissionTime)
		MoveToIntermission();
	else
		// send effect
		CMuzzleFlash(State.GetOrigin (), State.GetNumber(), MZ_LOGIN).Send();

	BroadcastPrintf (PRINT_HIGH, "%s entered the game\n", Client.Persistent.Name.c_str());

	// make sure all view stuff is valid
	EndServerFrame();
}

void CPlayerEntity::Begin ()
{
	gameEntity->client = Game.Clients + (State.GetNumber()-1);

	if (Game.GameMode & GAME_DEATHMATCH)
	{
		BeginDeathmatch ();
		return;
	}

	// if there is already a body waiting for us (a loadgame), just
	// take it, otherwise spawn one from scratch
	if (GetInUse())
	{
		// the client has cleared the client side viewangles upon
		// connecting to the server, which is different than the
		// state when the game is saved, so we need to compensate
		// with deltaangles
		for (sint32 i = 0; i < 3; i++)
			Client.PlayerState.GetPMove()->deltaAngles[i] = ANGLE2SHORT(Client.PlayerState.GetViewAngles()[i]);

		// also re-build the weapon model
		Client.PlayerState.GetGunIndex() = Client.Persistent.Weapon->GetWeaponModel();
	}
	else
	{
		// a spawn point will completely reinitialize the entity
		// except for the persistant data that was initialized at
		// ClientConnect() time

_CC_DISABLE_DEPRECATION
		G_InitEdict (gameEntity);
_CC_ENABLE_DEPRECATION

		ClassName = "player";
		InitResp ();
		PutInServer ();
	}

	if (Level.IntermissionTime)
		MoveToIntermission ();
	else
	{
		// send effect if in a multiplayer game
		if (Game.MaxClients > 1)
		{
			CMuzzleFlash(State.GetOrigin(), State.GetNumber(), MZ_LOGIN).Send();
			BroadcastPrintf (PRINT_HIGH, "%s entered the game\n", Client.Persistent.Name.c_str());
		}
	}

	// make sure all view stuff is valid
	EndServerFrame ();

	Client.Persistent.State = SVCS_SPAWNED;
}

IPAddress CopyIP (const char *val)
{
	// Do we have a :?
	std::string str (val);

	size_t loc = str.find_first_of (':');

	if (loc != std::string::npos)
		str = str.substr(0, loc);

	IPAddress Adr;

	_CC_ASSERT_EXPR (!(str.length() > sizeof(Adr.str)), "IP copied is longer than sizeof(Adr.str)");
	Q_snprintfz (Adr.str, sizeof(Adr.str), "%s", str.c_str());

	return Adr;
}

bool CPlayerEntity::Connect (char *userinfo)
{
	std::string	UserInfo = userinfo;

	// check to see if they are on the banned IP list
	std::string value = Info_ValueForKey (UserInfo, "ip");
	IPAddress Adr = CopyIP (value.c_str());

	if (Bans.IsBanned(Adr) || Bans.IsBanned(Info_ValueForKey(UserInfo, "name").c_str()))
	{
		Info_SetValueForKey(UserInfo, "rejmsg", "Connection refused.");
		return false;
	}

	// check for a Spectator
	value = Info_ValueForKey (UserInfo, "spectator");
	if ((Game.GameMode & GAME_DEATHMATCH) && value.length() && value != "0")
	{
		sint32 i, numspec;

		if (Bans.IsBannedFromSpectator(Adr) || Bans.IsBannedFromSpectator(Info_ValueForKey(UserInfo, "name").c_str()))
		{
			Info_SetValueForKey(UserInfo, "rejmsg", "Not permitted to enter spectator mode");
			return false;
		}
		if (*CvarList[CV_SPECTATOR_PASSWORD].String() && 
			strcmp(CvarList[CV_SPECTATOR_PASSWORD].String(), "none") && 
			CvarList[CV_SPECTATOR_PASSWORD].String() != value)
		{
			Info_SetValueForKey(UserInfo, "rejmsg", "Spectator password required or incorrect.");
			return false;
		}

		// count spectators
		for (i = numspec = 0; i < Game.MaxClients; i++)
		{
			CPlayerEntity *Ent = entity_cast<CPlayerEntity>(Game.Entities[i+1].Entity);
			if (Ent->GetInUse() && Ent->Client.Persistent.Spectator)
				numspec++;
		}

		if (numspec >= Game.MaxSpectators)
		{
			Info_SetValueForKey(UserInfo, "rejmsg", "Server Spectator limit is full.");
			return false;
		}
	}
	else
	{
		// check for a password
		value = Info_ValueForKey (UserInfo, "password");
		if (*CvarList[CV_PASSWORD].String() && strcmp(CvarList[CV_PASSWORD].String(), "none") && 
			CvarList[CV_PASSWORD].String() != value)
		{
			Info_SetValueForKey(UserInfo, "rejmsg", "Password required or incorrect.");
			return false;
		}
	}


	// they can connect
	gameEntity->client = Game.Clients + (State.GetNumber()-1);

	// if there is already a body waiting for us (a loadgame), just
	// take it, otherwise spawn one from scratch
	// Paril: Fix for an engine bug that causes
	// players to be kicked for the same IP but
	// takes over an existing spot.
	bool SameConnection = (GetInUse() && (Q_strlwr(Client.Persistent.UserInfo).compare(Q_strlwr(userinfo))));

	if (!GetInUse() || SameConnection)
	{
		// clear the respawning variables
#if CLEANCTF_ENABLED
//ZOID -- force team join
		Client.Respawn.CTF.Team = -1;
		Client.Respawn.CTF.IDState = false; 
//ZOID
#endif
		InitResp ();

		if (!Game.AutoSaved || !Client.Persistent.Weapon)
			InitPersistent();
		else if (SameConnection)
		{
			GetInUse() = false;
			InitPersistent();
		}
	}

	UserinfoChanged (userinfo);
	Client.Persistent.IP = Adr;

	if (Game.MaxClients > 1)
	{
		// Tell the entire game that someone connected
		BroadcastPrintf (PRINT_MEDIUM, "%s connected\n", Client.Persistent.Name.c_str());
		
		// But only tell the server the IP
		ServerPrintf ("%s @ %s connected\n", Client.Persistent.Name.c_str(), Info_ValueForKey (UserInfo, "ip").c_str());
	}

	GetSvFlags() = 0; // make sure we start with known default
	Client.Persistent.State = SVCS_CONNECTED;
	return true;
}

void CPlayerEntity::Disconnect ()
{
	if (!gameEntity->client)
		return;

	Client.Persistent.State = SVCS_FREE;
	BroadcastPrintf (PRINT_HIGH, "%s disconnected\n", Client.Persistent.Name.c_str());

#if CLEANCTF_ENABLED
//ZOID
	if (Game.GameMode & GAME_CTF)
		CTFDeadDropFlag();
//ZOID
#endif

	if (
#if CLEANCTF_ENABLED
		(Game.GameMode & GAME_CTF) || 
#endif
		DeathmatchFlags.dfDmTechs.IsEnabled()) 
		DeadDropTech();

#if ROGUE_FEATURES
	if(Client.Timers.Tracker)
		RemoveAttackingPainDaemons ();

	if (Client.OwnedSphere)
	{
		if (Client.OwnedSphere->GetInUse())
			Client.OwnedSphere->Free ();
		Client.OwnedSphere = NULL;
	}
#endif

	// send effect
	CMuzzleFlash(State.GetOrigin(), State.GetNumber(), MZ_LOGIN).Send();

	Unlink ();
	State.GetModelIndex() = 0;
	GetSolid() = SOLID_NOT;
	GetInUse() = false;
	ClassName = "disconnected";

	ConfigString (CS_PLAYERSKINS+(State.GetNumber()-1), "");

#if CLEANCODE_IRC
	if (Client.Respawn.IRC.Connected())
		Client.Respawn.IRC.Disconnect();
#endif
}

inline const char *MonsterAOrAn (const char *Name)
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

void CPlayerEntity::Obituary (IBaseEntity *Attacker)
{
	static std::string message, message2;
	message.clear();
	message2.clear();

	if (Attacker == this)
	{
		switch (meansOfDeath)
		{
		case MOD_HELD_GRENADE:
			message = "tried to put the pin back in";
			break;
		case MOD_HG_SPLASH:
		case MOD_G_SPLASH:
			switch (Client.Respawn.Gender)
			{
			case GENDER_MALE:
				message = "tripped on his own grenade";
				break;
			case GENDER_FEMALE:
				message = "tripped on her own grenade";
				break;
			default:
				message = "tripped on its own grenade";
				break;
			}
			break;
		case MOD_R_SPLASH:
			switch (Client.Respawn.Gender)
			{
			case GENDER_MALE:
				message = "blew himself up";
				break;
			case GENDER_FEMALE:
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

#if XATRIX_FEATURES
		case MOD_TRAP:
			switch (Client.Respawn.Gender)
			{
			case GENDER_MALE:
				message = "got sucked into his own trap";
				break;
			case GENDER_FEMALE:
				message = "got sucked into her own trap";
				break;
			default:
				message = "got sucked into it's own trap";
				break;
			}
			break;
#endif

#if ROGUE_FEATURES
		case MOD_DOPPLE_EXPLODE:
			switch (Client.Respawn.Gender)
			{
			case GENDER_MALE:
				message = "got caught in his own trap";
				break;
			case GENDER_FEMALE:
				message = "got caught in her own trap";
				break;
			default:
				message = "got caught in it's own trap";
				break;
			}
			break;
#endif
		default:
			switch (Client.Respawn.Gender)
			{
			case GENDER_MALE:
				message = "killed himself";
				break;
			case GENDER_FEMALE:
				message = "killed herself";
				break;
			default:
				message = "killed itself";
				break;
			}
			break;
		}
		if (Game.GameMode & GAME_DEATHMATCH)
			Client.Respawn.Score--;
		BroadcastPrintf (PRINT_MEDIUM, "%s %s.\n", Client.Persistent.Name.c_str(), message.c_str());
	}
	else if (Attacker && (Attacker->EntityFlags & ENT_PLAYER))
	{
		CPlayerEntity *PlayerAttacker = entity_cast<CPlayerEntity>(Attacker);
		bool endsInS = (PlayerAttacker->Client.Persistent.Name[PlayerAttacker->Client.Persistent.Name.size()-1] == 's');

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

#if CLEANCTF_ENABLED
//ZOID
		case MOD_GRAPPLE:
			message = "was caught by";
			message2 = "'s grapple";
			break;
//ZOID
#endif

#if XATRIX_FEATURES
		case MOD_RIPPER:
			message = "was ripped to shreds by";
			message2 = "'s ripper gun";
			break;
		case MOD_TRAP:
			message = "was caught in trap by";
			break;
#endif

#if ROGUE_FEATURES
		case MOD_CHAINFIST:
			message = "was shredded by";
			message2 = "'s ripsaw";
			break;
		case MOD_DISINTEGRATOR:
			message = "lost his grip courtesy of";
			message2 = "'s disintegrator";
			break;
		case MOD_ETF_RIFLE:
			message = "was perforated by";
			break;
		case MOD_HEATBEAM:
			message = "was scorched by";
			message2 = "'s plasma beam";
			break;
		case MOD_TESLA:
			message = "was enlightened by";
			message2 = "'s tesla mine";
			break;
		case MOD_PROX:
			message = "got too close to";
			message2 = "'s proximity mine";
			break;
		case MOD_NUKE:
			message = "was nuked by";
			message2 = "'s antimatter bomb";
			break;
		case MOD_VENGEANCE_SPHERE:
			message = "was purged by";
			message2 = "'s vengeance sphere";
			break;
		case MOD_DEFENDER_SPHERE:
			message = "had a blast with";
			message2 = "'s defender sphere";
			break;
		case MOD_HUNTER_SPHERE:
			message = "was killed like a dog by";
			message2 = "'s hunter sphere";
			break;
		case MOD_TRACKER:
			message = "was annihilated by";
			message2 = "'s disruptor";
			break;
		case MOD_DOPPLE_EXPLODE:
			message = "was blown up by";
			message2 = "'s doppleganger";
			break;
		case MOD_DOPPLE_VENGEANCE:
			message = "was purged by";
			message2 = "'s doppleganger";
			break;
		case MOD_DOPPLE_HUNTER:
			message = "was hunted down by";
			message2 = "'s doppleganger";
			break;
#endif
		}
		BroadcastPrintf (PRINT_MEDIUM,"%s %s %s%s.\n", Client.Persistent.Name.c_str(), message.c_str(), PlayerAttacker->Client.Persistent.Name.c_str(), message2.c_str());
		if (Game.GameMode & GAME_DEATHMATCH)
			PlayerAttacker->Client.Respawn.Score++;
	}
	else if (Attacker && (Attacker->EntityFlags & ENT_MONSTER))
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
#if XATRIX_FEATURES
		case MOD_RIPPER:
			message = "was ripped to shreds by";
			message2 = "'s ripper gun";
			break;
#endif
		default:
			message = "was killed by";
			break;
		};

		CMonsterEntity *Monster = entity_cast<CMonsterEntity>(Attacker);
		const char *Name = Monster->Monster->MonsterName.c_str();

		if (Game.GameMode & GAME_DEATHMATCH)
			Client.Respawn.Score--;
		BroadcastPrintf (PRINT_MEDIUM, "%s %s %s %s%s.\n", Client.Persistent.Name.c_str(), message.c_str(), MonsterAOrAn(Name), Name, message2.c_str());
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

		if (Game.GameMode & GAME_DEATHMATCH)
			Client.Respawn.Score--;
		BroadcastPrintf (PRINT_MEDIUM, "%s %s.\n", Client.Persistent.Name.c_str(), message.c_str());
	}
}

void CPlayerEntity::PushInDirection (vec3f vel)
{
	if (Health > 0)
	{
		Velocity = vel;

		// don't take falling damage immediately from this
		Client.OldVelocity = Velocity;
		if (FlySoundDebounceTime < Level.Frame)
		{
			FlySoundDebounceTime = Level.Frame + 15;
			PlaySound (CHAN_AUTO, GameMedia.FlySound());
		}
	}
}

#if ROGUE_FEATURES
void CPlayerEntity::RemoveAttackingPainDaemons ()
{
	IBaseEntity *tracker = NULL;

	do
	{
		tracker = CC_FindByClassName<IBaseEntity, ENT_BASE> (tracker, "pain daemon");

		if (tracker && tracker->Enemy == this)
			tracker->Free ();
	}
	while (tracker);

	Client.Timers.Tracker = 0;
}
#endif
