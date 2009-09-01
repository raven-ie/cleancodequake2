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
#include "m_player.h"

void ClientUserinfoChanged (edict_t *ent, char *userinfo);

//=======================================================================


void player_pain (edict_t *self, edict_t *other, float kick, int damage)
{
	// player pain is handled at the end of the frame in P_DamageFeedback
}

void ClientObituary (CPlayerEntity *self, edict_t *attacker)
{
	char *message = "", *message2 = "";
	if (attacker == self->gameEntity)
	{
		switch (meansOfDeath)
		{
		case MOD_HELD_GRENADE:
			message = "tried to put the pin back in";
			break;
		case MOD_HG_SPLASH:
		case MOD_G_SPLASH:
			switch (self->Client.resp.Gender)
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
			switch (self->Client.resp.Gender)
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
			switch (self->Client.resp.Gender)
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
			self->Client.resp.score--;
		BroadcastPrintf (PRINT_MEDIUM, "%s %s.\n", self->Client.pers.netname, message);
	}
	else if (attacker == world)
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
		}

		if (game.mode & GAME_DEATHMATCH)
			self->Client.resp.score--;
		BroadcastPrintf (PRINT_MEDIUM, "%s %s.\n", self->Client.pers.netname, message);
	}
	else if (attacker && attacker->client)
	{
		CPlayerEntity *Attacker = dynamic_cast<CPlayerEntity*>(attacker->Entity);
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
#ifdef CLEANCTF_ENABLED
//ZOID
		case MOD_GRAPPLE:
			message = "was caught by";
			message2 = "'s grapple";
			break;
//ZOID
#endif
		}
		BroadcastPrintf (PRINT_MEDIUM,"%s %s %s%s\n", self->Client.pers.netname, message, Attacker->Client.pers.netname, message2);
		if (game.mode & GAME_DEATHMATCH)
			Attacker->Client.resp.score++;
	}
	else
	{
		BroadcastPrintf (PRINT_MEDIUM, "%s died.\n", self->Client.pers.netname);
		if (game.mode & GAME_DEATHMATCH)
			self->Client.resp.score--;
	}
}

/*
==================
player_die
==================
*/
void player_die (edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
{
	int		n;
	CPlayerEntity *Player = dynamic_cast<CPlayerEntity*>(self->Entity);

	Vec3Clear (Player->gameEntity->avelocity);

	Player->gameEntity->takedamage = true;
	Player->gameEntity->movetype = MOVETYPE_TOSS;

	Player->State.SetModelIndex (0, 2);	// remove linked weapon model
	Player->State.SetModelIndex (0, 3);	// remove linked ctf flag

	vec3_t oldAngles;
	Player->State.GetAngles (oldAngles);
	Player->State.SetAngles (vec3f(0, oldAngles[1], 0));

	Player->State.SetSound (0);
	Player->Client.weapon_sound = 0;

	vec3f maxs = Player->GetMaxs();
	maxs[2] = -8;
	Player->SetMaxs (maxs);

	Player->SetSvFlags (Player->GetSvFlags() | SVF_DEADMONSTER);

	if (!Player->gameEntity->deadflag)
	{
		Player->Client.respawn_time = level.framenum + 10;
		Player->LookAtKiller (inflictor, attacker);
		Player->Client.PlayerState.GetPMove()->pmType = PMT_DEAD;
		ClientObituary (Player, attacker);

#ifdef CLEANCTF_ENABLED
		if (attacker->client)
		{
			CPlayerEntity *Attacker = dynamic_cast<CPlayerEntity*>(attacker->Entity);
//ZOID
			// if at start and same team, clear
			if (game.mode & GAME_CTF)
			{
				if ((meansOfDeath == MOD_TELEFRAG) &&
				(Player->Client.resp.ctf_state < 2) &&
				(Player->Client.resp.ctf_team == Attacker->Client.resp.ctf_team))
				{
					Attacker->Client.resp.score--;
					Player->Client.resp.ctf_state = 0;
				}

				CTFFragBonuses(Player, Attacker);
			}
		}

//ZOID
#endif

		Player->TossClientWeapon ();

#ifdef CLEANCTF_ENABLED
//ZOID
		if (game.mode & GAME_CTF)
		{
			CGrapple::PlayerResetGrapple(Player);
			CTFDeadDropFlag(Player);
			CTFDeadDropTech(Player);
		}
//ZOID
#endif

		if (game.mode & GAME_DEATHMATCH)
			Cmd_Help_f (Player);		// show scores

		// clear inventory
		// this is kind of ugly, but it's how we want to handle keys in coop
		for (n = 0; n < GetNumItems(); n++)
		{
			if ((game.mode == GAME_COOPERATIVE) && (GetItemByIndex(n)->Flags & ITEMFLAG_KEY))
				Player->Client.resp.coop_respawn.Inventory.Set(n, Player->Client.pers.Inventory.Has(n));
			Player->Client.pers.Inventory.Set(n, 0);
		}
	}

	// remove powerups
	Player->Client.quad_framenum = 0;
	Player->Client.invincible_framenum = 0;
	Player->Client.breather_framenum = 0;
	Player->Client.enviro_framenum = 0;
	Player->gameEntity->flags &= ~FL_POWER_ARMOR;

	if (Player->gameEntity->health < -40)
	{	// gib
		PlaySoundFrom (Player->gameEntity, CHAN_BODY, SoundIndex ("misc/udeath.wav"));
		for (n= 0; n < 4; n++)
			CGibEntity::Spawn (Player, gMedia.Gib_SmallMeat, damage, GIB_ORGANIC);
		Player->TossHead (damage);

		Player->gameEntity->takedamage = false;
//ZOID
		Player->Client.anim_priority = ANIM_DEATH;
		Player->Client.anim_end = 0;
//ZOID
	}
	else
	{	// normal death
		if (!Player->gameEntity->deadflag)
		{
			static int i;

			i = (i+1)%3;
			// start a death animation
			Player->Client.anim_priority = ANIM_DEATH;
			if (Player->Client.PlayerState.GetPMove()->pmFlags & PMF_DUCKED)
			{
				Player->State.SetFrame (FRAME_crdeath1-1);
				Player->Client.anim_end = FRAME_crdeath5;
			}
			else
			{
				switch (i)
				{
				case 0:
					Player->State.SetFrame (FRAME_death101-1);
					Player->Client.anim_end = FRAME_death106;
					break;
				case 1:
					Player->State.SetFrame (FRAME_death201-1);
					Player->Client.anim_end = FRAME_death206;
					break;
				case 2:
					Player->State.SetFrame (FRAME_death301-1);
					Player->Client.anim_end = FRAME_death308;
					break;
				}
			}
			PlaySoundFrom (Player->gameEntity, CHAN_VOICE, gMedia.Player.Death[(rand()%4)]);
		}
	}

	Player->gameEntity->deadflag = DEAD_DEAD;

	Player->Link ();
}

//=======================================================================

/*
=====================
ClientBeginDeathmatch

A client has just connected to the server in 
deathmatch mode, so clear everything out before starting them.
=====================
*/
void ClientBeginDeathmatch (CPlayerEntity *Player)
{
	vec3_t origin;
	G_InitEdict (Player->gameEntity);

	Player->InitResp();

	// locate ent at a spawn point
	Player->PutInServer();

	Player->State.GetOrigin (origin);
	if (level.intermissiontime)
		Player->MoveToIntermission();
	else
		// send effect
		CTempEnt::MuzzleFlash (origin, Player->State.GetNumber(), MZ_LOGIN);

	BroadcastPrintf (PRINT_HIGH, "%s entered the game\n", Player->Client.pers.netname);

	// make sure all view stuff is valid
	Player->EndServerFrame();
}


/*
===========
ClientBegin

called when a client has finished connecting, and is ready
to be placed into the game.  This will happen every level load.
============
*/
void ClientBegin (edict_t *ent)
{
	CPlayerEntity *Player = dynamic_cast<CPlayerEntity*>(ent->Entity);

	ent->client = game.clients + (Player->State.GetNumber()-1);

	if (game.mode & GAME_DEATHMATCH)
	{
		ClientBeginDeathmatch (Player);
		return;
	}

	// if there is already a body waiting for us (a loadgame), just
	// take it, otherwise spawn one from scratch
	if (ent->inUse == true)
	{
		// the client has cleared the client side viewangles upon
		// connecting to the server, which is different than the
		// state when the game is saved, so we need to compensate
		// with deltaangles
		vec3_t viewAngles;
		Player->Client.PlayerState.GetViewAngles(viewAngles);
		for (int i=0 ; i<3 ; i++)
			Player->Client.PlayerState.GetPMove()->deltaAngles[i] = ANGLE2SHORT(viewAngles[i]);
	}
	else
	{
		// a spawn point will completely reinitialize the entity
		// except for the persistant data that was initialized at
		// ClientConnect() time
		G_InitEdict (ent);
		ent->classname = "player";
		Player->InitResp ();
		Player->PutInServer ();
	}

	if (level.intermissiontime)
		Player->MoveToIntermission ();
	else
	{
		// send effect if in a multiplayer game
		if (game.maxclients > 1)
		{
			CTempEnt::MuzzleFlash (ent->state.origin, Player->State.GetNumber(), MZ_LOGIN);
			BroadcastPrintf (PRINT_HIGH, "%s entered the game\n", Player->Client.pers.netname);
		}
	}

	// make sure all view stuff is valid
	Player->EndServerFrame ();
}

/*
===========
ClientUserInfoChanged

called whenever the player updates a userinfo variable.

The game can override any of the settings in place
(forcing skins or names, etc) before copying it off.
============
*/
void ClientUserinfoChanged (edict_t *ent, char *userinfo)
{
	(dynamic_cast<CPlayerEntity*>(ent->Entity))->UserinfoChanged (userinfo);
}


/*
===========
ClientConnect

Called when a player begins connecting to the server.
The game can refuse entrance to a client by returning false.
If the client is allowed, the connection process will continue
and eventually get to ClientBegin()
Changing levels will NOT cause this to be called again, but
loadgames will.
============
*/
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

BOOL ClientConnect (edict_t *ent, char *userinfo)
{
	CPlayerEntity *Player = dynamic_cast<CPlayerEntity*>(ent->Entity);
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
			strcmp(password->String(), value)) {
			Info_SetValueForKey(userinfo, "rejmsg", "Password required or incorrect.");
			return false;
		}
	}


	// they can connect
	ent->client = game.clients + (Player->State.GetNumber()-1);

	// if there is already a body waiting for us (a loadgame), just
	// take it, otherwise spawn one from scratch
	if (!Player->IsInUse())
	{
		// clear the respawning variables
#ifdef CLEANCTF_ENABLED
//ZOID -- force team join
		Player->Client.resp.ctf_team = -1;
		Player->Client.resp.id_state = false; 
//ZOID
#endif
		Player->InitResp ();
		if (!game.autosaved || !Player->Client.pers.Weapon)
			Player->InitPersistent();
	}

	ClientUserinfoChanged (ent, userinfo);
	Player->Client.pers.IP = Adr;

	if (game.maxclients > 1)
	{
		// Tell the entire game that someone connected
		BroadcastPrintf (PRINT_MEDIUM, "%s connected\n", Player->Client.pers.netname);
		
		// But only tell the server the IP
		DebugPrintf ("%s@%s connected\n", Player->Client.pers.netname, Info_ValueForKey (userinfo, "ip"));
	}

	ent->svFlags = 0; // make sure we start with known default
	Player->Client.pers.state = SVCS_CONNECTED;
	return true;
}

/*
===========
ClientDisconnect

Called when a player drops from the server.
Will not be called between levels.
============
*/
void ClientDisconnect (edict_t *ent)
{
	CPlayerEntity *Player = dynamic_cast<CPlayerEntity*>(ent->Entity);

	if (!ent->client)
		return;

	Player->Client.pers.state = SVCS_FREE;
	BroadcastPrintf (PRINT_HIGH, "%s disconnected\n", Player->Client.pers.netname);

#ifdef CLEANCTF_ENABLED
//ZOID
	if (game.mode & GAME_CTF)
	{
		CTFDeadDropFlag(Player);
		CTFDeadDropTech(Player);
	}
//ZOID
#endif

	// send effect
	CTempEnt::MuzzleFlash (ent->state.origin, Player->State.GetNumber(), MZ_LOGIN);

	Player->Unlink ();
	ent->state.modelIndex = 0;
	Player->SetSolid (SOLID_NOT);
	Player->SetInUse (false);
	ent->classname = "disconnected";

	ConfigString (CS_PLAYERSKINS+(Player->State.GetNumber()-1), "");
}


//==============================================================

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

/*
==============
ClientThink

This will be called once for each client frame, which will
usually be a couple times for each server frame.
==============
*/
/*
uint32 frametest[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
uint16 curIndex = 0;
*/
void ClientThink (edict_t *ent, userCmd_t *ucmd)
{
	/*
	if (curIndex == 20)
	{
		uint32 frameCounts[19];
		float av = 0;

		// Count em up
		for (int i = 0; i < 19; i++)
		{
			frameCounts[i] = (frametest[i+1] - frametest[i]);
			av += frameCounts[i];
		}

		av /= 19;

		DebugPrintf ("%f\n", av);

		memset (frametest, 0, sizeof(frametest));
		curIndex = 0;
	}
	else
		frametest[curIndex++] = Sys_Milliseconds();
	*/
	(dynamic_cast<CPlayerEntity*>(ent->Entity))->ClientThink (ucmd);
}
