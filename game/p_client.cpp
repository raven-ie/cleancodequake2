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

void SP_misc_teleporter_dest (edict_t *ent);

//
// Gross, ugly, disgustuing hack section
//

// this function is an ugly as hell hack to fix some map flaws
//
// the coop spawn spots on some maps are SNAFU.  There are coop spots
// with the wrong targetname as well as spots with no name at all
//
// we use carnal knowledge of the maps to fix the coop spot targetnames to match
// that of the nearest named single player spot

static void SP_FixCoopSpots (edict_t *self)
{
	edict_t	*spot;
	vec3_t	d;

	spot = NULL;

	while(1)
	{
		spot = G_Find(spot, FOFS(classname), "info_player_start");
		if (!spot)
			return;
		if (!spot->targetname)
			continue;
		Vec3Subtract(self->s.origin, spot->s.origin, d);
		if (Vec3Length(d) < 384)
		{
			if ((!self->targetname) || Q_stricmp(self->targetname, spot->targetname) != 0)
			{
//				gi.dprintf("FixCoopSpots changed %s at %s targetname from %s to %s\n", self->classname, vtos(self->s.origin), self->targetname, spot->targetname);
				self->targetname = spot->targetname;
			}
			return;
		}
	}
}

// now if that one wasn't ugly enough for you then try this one on for size
// some maps don't have any coop spots at all, so we need to create them
// where they should have been

static void SP_CreateCoopSpots (edict_t *self)
{
	edict_t	*spot;

	if(Q_stricmp(level.mapname, "security") == 0)
	{
		spot = G_Spawn();
		spot->classname = "info_player_coop";
		spot->s.origin[0] = 188 - 64;
		spot->s.origin[1] = -164;
		spot->s.origin[2] = 80;
		spot->targetname = "jail3";
		spot->s.angles[1] = 90;

		spot = G_Spawn();
		spot->classname = "info_player_coop";
		spot->s.origin[0] = 188 + 64;
		spot->s.origin[1] = -164;
		spot->s.origin[2] = 80;
		spot->targetname = "jail3";
		spot->s.angles[1] = 90;

		spot = G_Spawn();
		spot->classname = "info_player_coop";
		spot->s.origin[0] = 188 + 128;
		spot->s.origin[1] = -164;
		spot->s.origin[2] = 80;
		spot->targetname = "jail3";
		spot->s.angles[1] = 90;

		return;
	}
}


/*QUAKED info_player_start (1 0 0) (-16 -16 -24) (16 16 32)
The normal starting point for a level.
*/
void SP_info_player_start(edict_t *self)
{
	if (!coop->Integer())
		return;
	if(Q_stricmp(level.mapname, "security") == 0)
	{
		// invoke one of our gross, ugly, disgusting hacks
		self->think = SP_CreateCoopSpots;
		self->nextthink = level.time + FRAMETIME;
	}
}

/*QUAKED info_player_deathmatch (1 0 1) (-16 -16 -24) (16 16 32)
potential spawning position for deathmatch games
*/
void SP_info_player_deathmatch(edict_t *self)
{
//	if (!deathmatch->Integer())
//	{
//		G_FreeEdict (self);
//		return;
//	}
	if (!deathmatch->Integer()) {
		self->solid = SOLID_NOT;
		self->svFlags = SVF_NOCLIENT;
		gi.linkentity (self);

		return;
	}

	SP_misc_teleporter_dest (self);
}

/*QUAKED info_player_coop (1 0 1) (-16 -16 -24) (16 16 32)
potential spawning position for coop games
*/

void SP_info_player_coop(edict_t *self)
{
	if (!coop->Integer())
	{
		G_FreeEdict (self);
		return;
	}

	if((Q_stricmp(level.mapname, "jail2") == 0)   ||
	   (Q_stricmp(level.mapname, "jail4") == 0)   ||
	   (Q_stricmp(level.mapname, "mine1") == 0)   ||
	   (Q_stricmp(level.mapname, "mine2") == 0)   ||
	   (Q_stricmp(level.mapname, "mine3") == 0)   ||
	   (Q_stricmp(level.mapname, "mine4") == 0)   ||
	   (Q_stricmp(level.mapname, "lab") == 0)     ||
	   (Q_stricmp(level.mapname, "boss1") == 0)   ||
	   (Q_stricmp(level.mapname, "fact3") == 0)   ||
	   (Q_stricmp(level.mapname, "biggun") == 0)  ||
	   (Q_stricmp(level.mapname, "space") == 0)   ||
	   (Q_stricmp(level.mapname, "command") == 0) ||
	   (Q_stricmp(level.mapname, "power2") == 0) ||
	   (Q_stricmp(level.mapname, "strike") == 0))
	{
		// invoke one of our gross, ugly, disgusting hacks
		self->think = SP_FixCoopSpots;
		self->nextthink = level.time + FRAMETIME;
	}
}


/*QUAKED info_player_intermission (1 0 1) (-16 -16 -24) (16 16 32)
The deathmatch intermission point will be at one of these
Use 'angles' instead of 'angle', so you can set pitch or roll as well as yaw.  'pitch yaw roll'
*/
void SP_info_player_intermission(edict_t *ent)
{
}


//=======================================================================


void player_pain (edict_t *self, edict_t *other, float kick, int damage)
{
	// player pain is handled at the end of the frame in P_DamageFeedback
}


bool IsFemale (edict_t *ent)
{
	char		*info;

	if (!ent->client)
		return false;

	info = Info_ValueForKey (ent->client->pers.userinfo, "gender");
	if (info[0] == 'f' || info[0] == 'F')
		return true;
	return false;
}

bool IsNeutral (edict_t *ent)
{
	char		*info;

	if (!ent->client)
		return false;

	info = Info_ValueForKey (ent->client->pers.userinfo, "gender");
	if (info[0] != 'f' && info[0] != 'F' && info[0] != 'm' && info[0] != 'M')
		return true;
	return false;
}

void _ClientObituary (edict_t *self, edict_t *inflictor, edict_t *attacker)
{
	int			mod;
	char		*message;
	char		*message2;
	bool	ff;

	if (coop->Integer() && attacker->client)
		meansOfDeath |= MOD_FRIENDLY_FIRE;

	if (deathmatch->Integer() || coop->Integer())
	{
		ff = (meansOfDeath & MOD_FRIENDLY_FIRE) ? true : false;
		mod = meansOfDeath & ~MOD_FRIENDLY_FIRE;
		message = NULL;
		message2 = "";

		switch (mod)
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
		if (attacker == self)
		{
			switch (mod)
			{
			case MOD_HELD_GRENADE:
				message = "tried to put the pin back in";
				break;
			case MOD_HG_SPLASH:
			case MOD_G_SPLASH:
				if (IsNeutral(self))
					message = "tripped on its own grenade";
				else if (IsFemale(self))
					message = "tripped on her own grenade";
				else
					message = "tripped on his own grenade";
				break;
			case MOD_R_SPLASH:
				if (IsNeutral(self))
					message = "blew itself up";
				else if (IsFemale(self))
					message = "blew herself up";
				else
					message = "blew himself up";
				break;
			case MOD_BFG_BLAST:
				message = "should have used a smaller gun";
				break;
			default:
				if (IsNeutral(self))
					message = "killed itself";
				else if (IsFemale(self))
					message = "killed herself";
				else
					message = "killed himself";
				break;
			}
		}
		if (message)
		{
			BroadcastPrintf (PRINT_MEDIUM, "%s %s.\n", self->client->pers.netname, message);
			if (deathmatch->Integer())
				self->client->resp.score--;
			self->enemy = NULL;
			return;
		}

		self->enemy = attacker;
		if (attacker && attacker->client)
		{
			switch (mod)
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
			case MOD_HANDGRENADE:
				message = "caught";
				message2 = "'s handgrenade";
				break;
			case MOD_HG_SPLASH:
				message = "didn't see";
				message2 = "'s handgrenade";
				break;
			case MOD_HELD_GRENADE:
				message = "feels";
				message2 = "'s pain";
				break;
			case MOD_TELEFRAG:
				message = "tried to invade";
				message2 = "'s personal space";
				break;
			}
			if (message)
			{
				BroadcastPrintf (PRINT_MEDIUM,"%s %s %s%s\n", self->client->pers.netname, message, attacker->client->pers.netname, message2);
				if (deathmatch->Integer())
				{
					if (ff)
						attacker->client->resp.score--;
					else
						attacker->client->resp.score++;
				}
				return;
			}
		}
	}

	BroadcastPrintf (PRINT_MEDIUM,"%s died.\n", self->client->pers.netname);
	if (deathmatch->Integer())
		self->client->resp.score--;
}

void ClientObituary (edict_t *self, edict_t *attacker)
{
	char *message = "", *message2 = "";
	if (attacker == self)
	{
		switch (meansOfDeath)
		{
		case MOD_HELD_GRENADE:
			message = "tried to put the pin back in";
			break;
		case MOD_HG_SPLASH:
		case MOD_G_SPLASH:
			switch (self->client->resp.Gender)
			{
			case GenderMale:
				message = "tripped on its his grenade";
				break;
			case GenderFemale:
				message = "tripped on its her grenade";
				break;
			default:
				message = "tripped on its own grenade";
				break;
			}
			break;
		case MOD_R_SPLASH:
			switch (self->client->resp.Gender)
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
			switch (self->client->resp.Gender)
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
		if (deathmatch->Integer())
			self->client->resp.score--;
		BroadcastPrintf (PRINT_MEDIUM, "%s %s.\n", self->client->pers.netname, message);
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

		if (deathmatch->Integer())
			self->client->resp.score--;
		BroadcastPrintf (PRINT_MEDIUM, "%s %s.\n", self->client->pers.netname, message);
	}
	else if (attacker && attacker->client)
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
		case MOD_HANDGRENADE:
			message = "caught";
			message2 = "'s handgrenade";
			break;
		case MOD_HG_SPLASH:
			message = "didn't see";
			message2 = "'s handgrenade";
			break;
		case MOD_HELD_GRENADE:
			message = "feels";
			message2 = "'s pain";
			break;
		case MOD_TELEFRAG:
			message = "tried to invade";
			message2 = "'s personal space";
			break;
		}
		BroadcastPrintf (PRINT_MEDIUM,"%s %s %s%s\n", self->client->pers.netname, message, attacker->client->pers.netname, message2);
		if (deathmatch->Integer())
		{
			if ((self->client && attacker->client) && OnSameTeam(self, attacker))
				attacker->client->resp.score--;
			else
				attacker->client->resp.score++;
		}
	}
	else
	{
		BroadcastPrintf (PRINT_MEDIUM, "%s died.\n", self->client->pers.netname);
		if (deathmatch->Integer())
			self->client->resp.score--;
	}
}

void TouchItem (edict_t *ent, edict_t *other, plane_t *plane, cmBspSurface_t *surf);
void TossClientWeapon (edict_t *self)
{
	if (!deathmatch->Integer())
		return;

	CBaseItem *Item = ((self->client->pers.Weapon) ? ((self->client->pers.Weapon->WeaponItem) ? self->client->pers.Weapon->WeaponItem : self->client->pers.Weapon->Item) : NULL);
	// Personally, this is dumb.
	//if (! self->client->pers.Inventory.Has(Item->Ammo) )
	//	item = NULL;
	if (!Item->WorldModel)
		Item = NULL;

	bool quad = (!dmFlags.dfQuadDrop) ? false : (bool)(self->client->quad_framenum > (level.framenum + 10));
	float spread = (Item && quad) ? 22.5f : 0.0f;

	if (Item)
	{
		self->client->v_angle[YAW] -= spread;
		edict_t *drop = Item->DropItem (self);
		self->client->v_angle[YAW] += spread;
		drop->spawnflags |= DROPPED_PLAYER_ITEM;
		if (self->client->pers.Weapon->WeaponItem)
			drop->count = self->client->pers.Inventory.Has(self->client->pers.Weapon->WeaponItem->Ammo);
		else
			drop->count = self->client->pers.Inventory.Has(self->client->pers.Weapon->Item);
	}

	if (quad)
	{
		self->client->v_angle[YAW] += spread;
		edict_t *drop = FindItem("Quad Damage")->DropItem (self);
		self->client->v_angle[YAW] -= spread;
		drop->spawnflags |= DROPPED_PLAYER_ITEM;

		drop->touch = TouchItem;
		drop->nextthink = level.time + (self->client->quad_framenum - level.framenum) * FRAMETIME;
		drop->think = G_FreeEdict;
	}
}

/*
==================
LookAtKiller
==================
*/
void LookAtKiller (edict_t *self, edict_t *inflictor, edict_t *attacker)
{
	vec3_t		dir;

	if (attacker && attacker != world && attacker != self)
	{
		Vec3Subtract (attacker->s.origin, self->s.origin, dir);
	}
	else if (inflictor && inflictor != world && inflictor != self)
	{
		Vec3Subtract (inflictor->s.origin, self->s.origin, dir);
	}
	else
	{
		self->client->killer_yaw = self->s.angles[YAW];
		return;
	}

	if (dir[0])
		self->client->killer_yaw = 180/M_PI*atan2f(dir[1], dir[0]);
	else {
		self->client->killer_yaw = 0;
		if (dir[1] > 0)
			self->client->killer_yaw = 90;
		else if (dir[1] < 0)
			self->client->killer_yaw = -90;
	}
	if (self->client->killer_yaw < 0)
		self->client->killer_yaw += 360;
	

}

/*
==================
player_die
==================
*/
void player_die (edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
{
	int		n;

	Vec3Clear (self->avelocity);

	self->takedamage = DAMAGE_YES;
	self->movetype = MOVETYPE_TOSS;

	self->s.modelIndex2 = 0;	// remove linked weapon model

	self->s.angles[0] = 0;
	self->s.angles[2] = 0;

	self->s.sound = 0;
	self->client->weapon_sound = 0;

	self->maxs[2] = -8;

//	self->solid = SOLID_NOT;
	self->svFlags |= SVF_DEADMONSTER;

	if (!self->deadflag)
	{
		self->client->respawn_time = level.time + 1.0;
		LookAtKiller (self, inflictor, attacker);
		self->client->ps.pMove.pmType = PMT_DEAD;
		ClientObituary (self, attacker);
		TossClientWeapon (self);
		if (deathmatch->Integer())
			Cmd_Help_f (self);		// show scores

		// clear inventory
		// this is kind of ugly, but it's how we want to handle keys in coop
		for (n = 0; n < GetNumItems(); n++)
		{
			if (coop->Integer() && GetItemByIndex(n)->Flags & ITEMFLAG_KEY)
				self->client->resp.coop_respawn.Inventory.Set(n, self->client->pers.Inventory.Has(n));
			self->client->pers.Inventory.Set(n, 0);
		}
	}

	// remove powerups
	self->client->quad_framenum = 0;
	self->client->invincible_framenum = 0;
	self->client->breather_framenum = 0;
	self->client->enviro_framenum = 0;
	self->flags &= ~FL_POWER_ARMOR;

	if (self->health < -40)
	{	// gib
		PlaySoundFrom (self, CHAN_BODY, SoundIndex ("misc/udeath.wav"));
		for (n= 0; n < 4; n++)
			ThrowGib (self, gMedia.Gib_SmallMeat, damage, GIB_ORGANIC);
		ThrowClientHead (self, damage);

		self->takedamage = DAMAGE_NO;
	}
	else
	{	// normal death
		if (!self->deadflag)
		{
			static int i;

			i = (i+1)%3;
			// start a death animation
			self->client->anim_priority = ANIM_DEATH;
			if (self->client->ps.pMove.pmFlags & PMF_DUCKED)
			{
				self->s.frame = FRAME_crdeath1-1;
				self->client->anim_end = FRAME_crdeath5;
			}
			else
			{
				switch (i)
				{
				case 0:
					self->s.frame = FRAME_death101-1;
					self->client->anim_end = FRAME_death106;
					break;
				case 1:
					self->s.frame = FRAME_death201-1;
					self->client->anim_end = FRAME_death206;
					break;
				case 2:
					self->s.frame = FRAME_death301-1;
					self->client->anim_end = FRAME_death308;
					break;
				}
			}
			PlaySoundFrom (self, CHAN_VOICE, gMedia.Player.Death[(rand()%4)]);
		}
	}

	self->deadflag = DEAD_DEAD;

	gi.linkentity (self);
}

//=======================================================================

/*
==============
InitClientPersistant

This is only called when the game first initializes in single player,
but is called after each death and level change in deathmatch
==============
*/
void InitClientPersistant (edict_t *ent)
{
	gclient_t	*client = ent->client;

	memset (&client->pers, 0, sizeof(client->pers));

	/*item = FindItem("Blaster");
	client->pers.selected_item = ITEM_INDEX(item);
	client->pers.inventory[client->pers.selected_item] = 1;*/
	FindItem("Blaster")->Add(ent, 1);

	//client->pers.weapon = item;
	client->pers.Weapon = &WeaponBlaster;
	client->pers.Inventory.SelectedItem = client->pers.Weapon->Item->GetIndex();

	client->pers.Armor = NULL;

	client->pers.health			= 100;
	client->pers.max_health		= 100;

	InitItemMaxValues(ent);
}


void InitClientResp (gclient_t *client)
{
	memset (&client->resp, 0, sizeof(client->resp));
	client->resp.enterframe = level.framenum;
	client->resp.coop_respawn = client->pers;
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
void SaveClientData (void)
{
	int		i;
	edict_t	*ent;

	for (i=0 ; i<game.maxclients ; i++)
	{
		ent = &g_edicts[1+i];
		if (!ent->inUse)
			continue;
		game.clients[i].pers.health = ent->health;
		game.clients[i].pers.max_health = ent->max_health;
		game.clients[i].pers.savedFlags = (ent->flags & (FL_GODMODE|FL_NOTARGET|FL_POWER_ARMOR));
		if (coop->Integer())
			game.clients[i].pers.score = ent->client->resp.score;
	}
}

void FetchClientEntData (edict_t *ent)
{
	ent->health = ent->client->pers.health;
	ent->max_health = ent->client->pers.max_health;
	ent->flags |= ent->client->pers.savedFlags;
	if (coop->Integer())
		ent->client->resp.score = ent->client->pers.score;
}



/*
=======================================================================

  SelectSpawnPoint

=======================================================================
*/

/*
================
PlayersRangeFromSpot

Returns the distance to the nearest player from the given spot
================
*/
float	PlayersRangeFromSpot (edict_t *spot)
{
	edict_t	*player;
	float	bestplayerdistance;
	vec3_t	v;
	int		n;
	float	playerdistance;


	bestplayerdistance = 9999999;

	for (n = 1; n <= maxclients->Integer(); n++)
	{
		player = &g_edicts[n];

		if (!player->inUse)
			continue;

		if (player->health <= 0)
			continue;

		Vec3Subtract (spot->s.origin, player->s.origin, v);
		playerdistance = Vec3Length (v);

		if (playerdistance < bestplayerdistance)
			bestplayerdistance = playerdistance;
	}

	return bestplayerdistance;
}

/*
================
SelectRandomDeathmatchSpawnPoint

go to a random point, but NOT the two points closest
to other players
================
*/
edict_t *SelectRandomDeathmatchSpawnPoint (void)
{
	edict_t	*spot, *spot1, *spot2;
	int		count = 0;
	int		selection;
	float	range, range1, range2;

	spot = NULL;
	range1 = range2 = 99999;
	spot1 = spot2 = NULL;

	while ((spot = G_Find (spot, FOFS(classname), "info_player_deathmatch")) != NULL)
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
		return NULL;

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
		spot = G_Find (spot, FOFS(classname), "info_player_deathmatch");
		if (spot == spot1 || spot == spot2)
			selection++;
	} while(selection--);

	return spot;
}

/*
================
SelectFarthestDeathmatchSpawnPoint

================
*/
edict_t *SelectFarthestDeathmatchSpawnPoint (void)
{
	edict_t	*bestspot;
	float	bestdistance, bestplayerdistance;
	edict_t	*spot;


	spot = NULL;
	bestspot = NULL;
	bestdistance = 0;
	while ((spot = G_Find (spot, FOFS(classname), "info_player_deathmatch")) != NULL)
	{
		bestplayerdistance = PlayersRangeFromSpot (spot);

		if (bestplayerdistance > bestdistance)
		{
			bestspot = spot;
			bestdistance = bestplayerdistance;
		}
	}

	if (bestspot)
	{
		return bestspot;
	}

	// if there is a player just spawned on each and every start spot
	// we have no choice to turn one into a telefrag meltdown
	spot = G_Find (NULL, FOFS(classname), "info_player_deathmatch");

	return spot;
}

edict_t *SelectDeathmatchSpawnPoint (void)
{
	if (dmFlags.dfSpawnFarthest)
		return SelectFarthestDeathmatchSpawnPoint ();
	else
		return SelectRandomDeathmatchSpawnPoint ();
}


edict_t *SelectCoopSpawnPoint (edict_t *ent)
{
	int		index;
	edict_t	*spot = NULL;
	char	*target;

	index = ent->client - game.clients;

	// player 0 starts in normal player spawn point
	if (!index)
		return NULL;

	spot = NULL;

	// assume there are four coop spots at each spawnpoint
	while (1)
	{
		spot = G_Find (spot, FOFS(classname), "info_player_coop");
		if (!spot)
			return NULL;	// we didn't have enough...

		target = spot->targetname;
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
void	SelectSpawnPoint (edict_t *ent, vec3_t origin, vec3_t angles)
{
	edict_t	*spot = NULL;

	if (deathmatch->Integer())
		spot = SelectDeathmatchSpawnPoint ();
	else if (coop->Integer())
		spot = SelectCoopSpawnPoint (ent);

	// find a single player start spot
	if (!spot)
	{
		while ((spot = G_Find (spot, FOFS(classname), "info_player_start")) != NULL)
		{
			if (!game.spawnpoint[0] && !spot->targetname)
				break;

			if (!game.spawnpoint[0] || !spot->targetname)
				continue;

			if (Q_stricmp(game.spawnpoint, spot->targetname) == 0)
				break;
		}

		if (!spot)
		{
			// There wasn't a spawnpoint without a target, so use any
			if (!game.spawnpoint[0]) {
				spot = G_Find (spot, FOFS(classname), "info_player_start");

				if (!spot)
					spot = G_Find (spot, FOFS(classname), "info_player_deathmatch");
			}
			if (!spot)
				gi.error ("Couldn't find spawn point %s", game.spawnpoint);
		}
	}

	Vec3Copy (spot->s.origin, origin);
	origin[2] += 9;
	Vec3Copy (spot->s.angles, angles);
}

//======================================================================


void InitBodyQue (void)
{
	int		i;
	edict_t	*ent;

	level.body_que = 0;
	for (i=0; i<BODY_QUEUE_SIZE ; i++)
	{
		ent = G_Spawn();
		ent->classname = "bodyque";
	}
}

void body_die (edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
{
	int	n;

	if (self->health < -40)
	{
		PlaySoundFrom (self, CHAN_BODY, SoundIndex ("misc/udeath.wav"));
		for (n= 0; n < 4; n++)
			ThrowGib (self, gMedia.Gib_SmallMeat, damage, GIB_ORGANIC);
		self->s.origin[2] -= 48;
		ThrowClientHead (self, damage);
		self->takedamage = DAMAGE_NO;
	}
}

void CopyToBodyQue (edict_t *ent)
{
	edict_t		*body;

	// grab a body que and cycle to the next one
	body = &g_edicts[maxclients->Integer() + level.body_que + 1];
	level.body_que = (level.body_que + 1) % BODY_QUEUE_SIZE;

	// FIXME: send an effect on the removed body

	gi.unlinkentity (ent);

	gi.unlinkentity (body);
	body->s = ent->s;
	body->s.number = body - g_edicts;

	body->svFlags = ent->svFlags;
	Vec3Copy (ent->mins, body->mins);
	Vec3Copy (ent->maxs, body->maxs);
	Vec3Copy (ent->absMin, body->absMin);
	Vec3Copy (ent->absMax, body->absMax);
	Vec3Copy (ent->size, body->size);
	body->solid = ent->solid;
	body->clipMask = ent->clipMask;
	body->owner = ent->owner;
	body->movetype = ent->movetype;

	body->die = body_die;
	body->takedamage = DAMAGE_YES;

	gi.linkentity (body);
}


void respawn (edict_t *self)
{
	if (deathmatch->Integer() || coop->Integer())
	{
		// spectator's don't leave bodies
		if (self->movetype != MOVETYPE_NOCLIP)
			CopyToBodyQue (self);
		self->svFlags &= ~SVF_NOCLIENT;
		PutClientInServer (self);

		// add a teleportation effect
		self->s.event = EV_PLAYER_TELEPORT;

		// hold in place briefly
		self->client->ps.pMove.pmFlags = PMF_TIME_TELEPORT;
		self->client->ps.pMove.pmTime = 14;

		self->client->respawn_time = level.time;

		return;
	}

	// restart the entire server
	gi.AddCommandString ("menu_loadgame\n");
}

/* 
 * only called when pers.spectator changes
 * note that resp.spectator should be the opposite of pers.spectator here
 */
void spectator_respawn (edict_t *ent)
{
	int i, numspec;

	// if the user wants to become a spectator, make sure he doesn't
	// exceed max_spectators

	if (ent->client->pers.spectator) {
		char *value = Info_ValueForKey (ent->client->pers.userinfo, "spectator");
		if (*spectator_password->String() && 
			strcmp(spectator_password->String(), "none") && 
			strcmp(spectator_password->String(), value)) {
			ClientPrintf(ent, PRINT_HIGH, "Spectator password incorrect.\n");
			ent->client->pers.spectator = false;
			WriteByte (SVC_STUFFTEXT);
			WriteString ("spectator 0\n");
			Cast(CASTFLAG_RELIABLE, ent);
			return;
		}

		// count spectators
		for (i = 1, numspec = 0; i <= maxclients->Integer(); i++)
			if (g_edicts[i].inUse && g_edicts[i].client->pers.spectator)
				numspec++;

		if (numspec >= maxspectators->Integer()) {
			ClientPrintf(ent, PRINT_HIGH, "Server spectator limit is full.");
			ent->client->pers.spectator = false;
			// reset his spectator var
			WriteByte (SVC_STUFFTEXT);
			WriteString ("spectator 0\n");
			Cast(CASTFLAG_RELIABLE, ent);
			return;
		}
	} else {
		// he was a spectator and wants to join the game
		// he must have the right password
		char *value = Info_ValueForKey (ent->client->pers.userinfo, "password");
		if (*password->String() && strcmp(password->String(), "none") && 
			strcmp(password->String(), value)) {
			ClientPrintf(ent, PRINT_HIGH, "Password incorrect.\n");
			ent->client->pers.spectator = true;
			WriteByte (SVC_STUFFTEXT);
			WriteString ("spectator 1\n");
			Cast(CASTFLAG_RELIABLE, ent);
			return;
		}
	}

	// clear client on respawn
	ent->client->resp.score = ent->client->pers.score = 0;

	ent->svFlags &= ~SVF_NOCLIENT;
	PutClientInServer (ent);

	// add a teleportation effect
	if (!ent->client->pers.spectator)  {
		// send effect
		CTempEnt::MuzzleFlash (ent->s.origin, ent-g_edicts, MZ_LOGIN);

		// hold in place briefly
		ent->client->ps.pMove.pmFlags = PMF_TIME_TELEPORT;
		ent->client->ps.pMove.pmTime = 14;
	}

	ent->client->respawn_time = level.time;

	if (ent->client->pers.spectator) 
		BroadcastPrintf (PRINT_HIGH, "%s has moved to the sidelines\n", ent->client->pers.netname);
	else
		BroadcastPrintf (PRINT_HIGH, "%s joined the game\n", ent->client->pers.netname);
}

//==============================================================


/*
===========
PutClientInServer

Called when a player connects to a server or respawns in
a deathmatch.
============
*/
void PutClientInServer (edict_t *ent)
{
	vec3_t	mins = {-16, -16, -24};
	vec3_t	maxs = {16, 16, 32};
	int		index;
	vec3_t	spawn_origin, spawn_angles;
	gclient_t	*client;
	int		i;
	client_persistant_t	saved;
	client_respawn_t	resp;

	// find a spawn point
	// do it before setting health back up, so farthest
	// ranging doesn't count this client
	SelectSpawnPoint (ent, spawn_origin, spawn_angles);

	index = ent-g_edicts-1;
	client = ent->client;

	// deathmatch wipes most client data every spawn
	if (deathmatch->Integer())
	{
		char		userinfo[MAX_INFO_STRING];

		resp = client->resp;
		memcpy (userinfo, client->pers.userinfo, sizeof(userinfo));
		InitClientPersistant (ent);
		ClientUserinfoChanged (ent, userinfo);
	}
	else if (coop->Integer())
	{
//		int			n;
		char		userinfo[MAX_INFO_STRING];

		resp = client->resp;
		memcpy (userinfo, client->pers.userinfo, sizeof(userinfo));
		// this is kind of ugly, but it's how we want to handle keys in coop
//		for (n = 0; n < game.num_items; n++)
//		{
//			if (itemlist[n].flags & IT_KEY)
//				resp.coop_respawn.inventory[n] = client->pers.inventory[n];
//		}
		resp.coop_respawn.game_helpchanged = client->pers.game_helpchanged;
		resp.coop_respawn.helpchanged = client->pers.helpchanged;
		client->pers = resp.coop_respawn;
		ClientUserinfoChanged (ent, userinfo);
		if (resp.score > client->pers.score)
			client->pers.score = resp.score;
	}
	else
	{
		memset (&resp, 0, sizeof(resp));
	}

	// clear everything but the persistant data
	saved = client->pers;
	memset (client, 0, sizeof(*client));
	client->pers = saved;
	if (client->pers.health <= 0)
		InitClientPersistant(ent);
	client->resp = resp;
	client->pers.state = SVCS_SPAWNED;

	// copy some data from the client to the entity
	FetchClientEntData (ent);

	// clear entity values
	ent->groundentity = NULL;
	ent->client = &game.clients[index];
	ent->takedamage = DAMAGE_AIM;
	ent->movetype = MOVETYPE_WALK;
	ent->viewheight = 22;
	ent->inUse = true;
	ent->classname = "player";
	ent->mass = 200;
	ent->solid = SOLID_BBOX;
	ent->deadflag = DEAD_NO;
	ent->air_finished = level.time + 12;
	ent->clipMask = CONTENTS_MASK_PLAYERSOLID;
	ent->model = "players/male/tris.md2";
	ent->pain = player_pain;
	ent->die = player_die;
	ent->waterlevel = 0;
	ent->watertype = 0;
	ent->flags &= ~FL_NO_KNOCKBACK;
	ent->svFlags &= ~SVF_DEADMONSTER;
	if (!ent->client->resp.MenuState.ent)
		ent->client->resp.MenuState = CMenuState(ent);

	Vec3Copy (mins, ent->mins);
	Vec3Copy (maxs, ent->maxs);
	Vec3Clear (ent->velocity);

	// clear playerstate values
	memset (&ent->client->ps, 0, sizeof(client->ps));

	client->ps.pMove.origin[0] = spawn_origin[0]*8;
	client->ps.pMove.origin[1] = spawn_origin[1]*8;
	client->ps.pMove.origin[2] = spawn_origin[2]*8;

	if (deathmatch->Integer() && dmFlags.dfFixedFov)
	{
		client->ps.fov = 90;
	}
	else
	{
		client->ps.fov = atoi(Info_ValueForKey(client->pers.userinfo, "fov"));
		if (client->ps.fov < 1)
			client->ps.fov = 90;
		else if (client->ps.fov > 160)
			client->ps.fov = 160;
	}

	//client->ps.gunIndex = ModelIndex(client->pers.weapon->view_model);

	// clear entity state values
	ent->s.effects = 0;
	ent->s.modelIndex = 255;		// will use the skin specified model
	ent->s.modelIndex2 = 255;		// custom gun model
	// sknum is player num and weapon number
	// weapon number will be added in changeweapon
	ent->s.skinNum = ent - g_edicts - 1;

	ent->s.frame = 0;
	Vec3Copy (spawn_origin, ent->s.origin);
	ent->s.origin[2] += 1;	// make sure off ground
	Vec3Copy (ent->s.origin, ent->s.oldOrigin);

	// set the delta angle
	for (i=0 ; i<3 ; i++)
	{
		client->ps.pMove.deltaAngles[i] = ANGLE2SHORT(spawn_angles[i] - client->resp.cmd_angles[i]);
	}

	ent->s.angles[PITCH] = 0;
	ent->s.angles[YAW] = spawn_angles[YAW];
	ent->s.angles[ROLL] = 0;
	Vec3Copy (ent->s.angles, client->ps.viewAngles);
	Vec3Copy (ent->s.angles, client->v_angle);

	// spawn a spectator
	if (client->pers.spectator) {
		client->chase_target = NULL;

		client->resp.spectator = true;

		ent->movetype = MOVETYPE_NOCLIP;
		ent->solid = SOLID_NOT;
		ent->svFlags |= SVF_NOCLIENT;
		ent->client->ps.gunIndex = 0;
		gi.linkentity (ent);
		return;
	} else
		client->resp.spectator = false;

	if (!KillBox (ent))
	{	// could't spawn in?
	}

	gi.linkentity (ent);

	// force the current weapon up
	//client->newweapon = client->pers.weapon;
	//ChangeWeapon (ent);
	client->NewWeapon = client->pers.Weapon;
	client->pers.Weapon->ChangeWeapon(ent);
}

/*
=====================
ClientBeginDeathmatch

A client has just connected to the server in 
deathmatch mode, so clear everything out before starting them.
=====================
*/
void ClientBeginDeathmatch (edict_t *ent)
{
	G_InitEdict (ent);

	InitClientResp (ent->client);

	// locate ent at a spawn point
	PutClientInServer (ent);

	if (level.intermissiontime)
	{
		MoveClientToIntermission (ent);
	}
	else
		// send effect
		CTempEnt::MuzzleFlash (ent->s.origin, ent-g_edicts, MZ_LOGIN);

	BroadcastPrintf (PRINT_HIGH, "%s entered the game\n", ent->client->pers.netname);

	// make sure all view stuff is valid
	ClientEndServerFrame (ent);
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
	int		i;

	ent->client = game.clients + (ent - g_edicts - 1);

	if (deathmatch->Integer())
	{
		ClientBeginDeathmatch (ent);
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
		for (i=0 ; i<3 ; i++)
			ent->client->ps.pMove.deltaAngles[i] = ANGLE2SHORT(ent->client->ps.viewAngles[i]);
	}
	else
	{
		// a spawn point will completely reinitialize the entity
		// except for the persistant data that was initialized at
		// ClientConnect() time
		G_InitEdict (ent);
		ent->classname = "player";
		InitClientResp (ent->client);
		PutClientInServer (ent);
	}

	if (level.intermissiontime)
	{
		MoveClientToIntermission (ent);
	}
	else
	{
		// send effect if in a multiplayer game
		if (game.maxclients > 1)
		{
			CTempEnt::MuzzleFlash (ent->s.origin, ent-g_edicts, MZ_LOGIN);
			BroadcastPrintf (PRINT_HIGH, "%s entered the game\n", ent->client->pers.netname);
		}
	}

	// make sure all view stuff is valid
	ClientEndServerFrame (ent);
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
	char	*s;
	int		playernum;

	// check for malformed or illegal info strings
	if (!Info_Validate(userinfo))
		Q_strncpyz (userinfo, "\\name\\badinfo\\skin\\male/grunt", MAX_INFO_STRING);

	// set name
	s = Info_ValueForKey (userinfo, "name");
	Q_strncpyz (ent->client->pers.netname, s, sizeof(ent->client->pers.netname)-1);

	// set spectator
	s = Info_ValueForKey (userinfo, "spectator");
	// spectators are only supported in deathmatch
	if (deathmatch->Integer() && *s && strcmp(s, "0"))
		ent->client->pers.spectator = true;
	else
		ent->client->pers.spectator = false;

	// set skin
	s = Info_ValueForKey (userinfo, "skin");

	playernum = ent-g_edicts-1;

	// combine name and skin into a configstring
	gi.configstring (CS_PLAYERSKINS+playernum, Q_VarArgs ("%s\\%s", ent->client->pers.netname, s) );

	// fov
	if (deathmatch->Integer() && dmFlags.dfFixedFov)
		ent->client->ps.fov = 90;
	else
	{
		ent->client->ps.fov = atoi(Info_ValueForKey(userinfo, "fov"));
		if (ent->client->ps.fov < 1)
			ent->client->ps.fov = 90;
		else if (ent->client->ps.fov > 160)
			ent->client->ps.fov = 160;
	}

	// handedness
	s = Info_ValueForKey (userinfo, "hand");
	if (strlen(s))
		ent->client->pers.hand = atoi(s);

	// IP
	s = Info_ValueForKey (userinfo, "ip");
	if (strlen(s))
		ent->client->pers.IP = IPStringToArrays(s);

	// Gender
	s = Info_ValueForKey (userinfo, "gender");
	if (strlen(s))
	{
		switch (s[0])
		{
		case 'm':
		case 'M':
			ent->client->resp.Gender = GenderMale;
			break;
		case 'f':
		case 'F':
			ent->client->resp.Gender = GenderFemale;
			break;
		default:
			ent->client->resp.Gender = GenderNeutral;
		}
	}
	else
		ent->client->resp.Gender = GenderMale;

	// MSG command
	s = Info_ValueForKey (userinfo, "msg");
	if (strlen (s))
		ent->client->resp.messageLevel = atoi (s);

	// save off the userinfo in case we want to check something later
	Q_strncpyz (ent->client->pers.userinfo, userinfo, sizeof(ent->client->pers.userinfo)-1);
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
BOOL ClientConnect (edict_t *ent, char *userinfo)
{
	char	*value;

	// check to see if they are on the banned IP list
	value = Info_ValueForKey (userinfo, "ip");
	IPAddress Adr = IPStringToArrays(value);
	if (Bans.IsBanned(Adr) || Bans.IsBanned(Info_ValueForKey(userinfo, "name")))
	{
		Info_SetValueForKey(userinfo, "rejmsg", "Banned from server.");
		return false;
	}

	// check for a spectator
	value = Info_ValueForKey (userinfo, "spectator");
	if (deathmatch->Integer() && *value && strcmp(value, "0")) {
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
		for (i = numspec = 0; i < maxclients->Integer(); i++)
			if (g_edicts[i+1].inUse && g_edicts[i+1].client->pers.spectator)
				numspec++;

		if (numspec >= maxspectators->Integer()) {
			Info_SetValueForKey(userinfo, "rejmsg", "Server spectator limit is full.");
			return false;
		}
	} else {
		// check for a password
		value = Info_ValueForKey (userinfo, "password");
		if (*password->String() && strcmp(password->String(), "none") && 
			strcmp(password->String(), value)) {
			Info_SetValueForKey(userinfo, "rejmsg", "Password required or incorrect.");
			return false;
		}
	}


	// they can connect
	ent->client = game.clients + (ent - g_edicts - 1);

	// if there is already a body waiting for us (a loadgame), just
	// take it, otherwise spawn one from scratch
	if (ent->inUse == false)
	{
		// clear the respawning variables
		InitClientResp (ent->client);
		if (!game.autosaved || !ent->client->pers.Weapon)
			InitClientPersistant (ent);
	}

	ClientUserinfoChanged (ent, userinfo);

	if (game.maxclients > 1)
	{
		// Tell the entire game that someone connected
		BroadcastPrintf (PRINT_MEDIUM, "%s connected\n", ent->client->pers.netname);
		
		// But only tell the server the IP
		DebugPrintf ("%s@%s connected\n", ent->client->pers.netname, Info_ValueForKey (userinfo, "ip"));
	}

	ent->svFlags = 0; // make sure we start with known default
	ent->client->pers.state = SVCS_CONNECTED;
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
	int		playernum;

	if (!ent->client)
		return;

	BroadcastPrintf (PRINT_HIGH, "%s disconnected\n", ent->client->pers.netname);

	// send effect
	CTempEnt::MuzzleFlash (ent->s.origin, ent-g_edicts, MZ_LOGIN);

	gi.unlinkentity (ent);
	ent->s.modelIndex = 0;
	ent->solid = SOLID_NOT;
	ent->inUse = false;
	ent->classname = "disconnected";
	ent->client->pers.state = SVCS_FREE;

	playernum = ent-g_edicts-1;
	gi.configstring (CS_PLAYERSKINS+playernum, "");
}


//==============================================================

#ifdef USE_EXTENDED_GAME_IMPORTS
edict_t	*pm_passent;

// pmove doesn't need to know about passent and contentmask
cmTrace_t	PM_trace (vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end)
{
_CC_DISABLE_DEPRECATION
(
	if (pm_passent->health > 0)
		return gi.trace(start, mins, maxs, end, pm_passent, CONTENTS_MASK_PLAYERSOLID);
	else
		return gi.trace(start, mins, maxs, end, pm_passent, CONTENTS_MASK_DEADSOLID);
)
}
#endif

/*
==============
ClientThink

This will be called once for each client frame, which will
usually be a couple times for each server frame.
==============
*/
void ClientThink (edict_t *ent, userCmd_t *ucmd)
{
	gclient_t	*client;
	edict_t	*other;
	int		i, j;
#ifdef USE_EXTENDED_GAME_IMPORTS
	pMove_t		pm;
#else
	pMoveNew_t	pm;
#endif

	level.current_entity = ent;
	client = ent->client;

	if (level.intermissiontime)
	{
		client->ps.pMove.pmType = PMT_FREEZE;
		// can exit intermission after five seconds
		if (level.time > level.intermissiontime + 5.0 
			&& (ucmd->buttons & BUTTON_ANY) )
			level.exitintermission = true;
		return;
	}

#ifdef USE_EXTENDED_GAME_IMPORTS
	pm_passent = ent;
#endif

	if (ent->client->chase_target) {

		client->resp.cmd_angles[0] = SHORT2ANGLE(ucmd->angles[0]);
		client->resp.cmd_angles[1] = SHORT2ANGLE(ucmd->angles[1]);
		client->resp.cmd_angles[2] = SHORT2ANGLE(ucmd->angles[2]);

	}
	else {
		// set up for pmove
		memset (&pm, 0, sizeof(pm));

		if (ent->movetype == MOVETYPE_NOCLIP)
			client->ps.pMove.pmType = PMT_SPECTATOR;
		else if (ent->s.modelIndex != 255)
			client->ps.pMove.pmType = PMT_GIB;
		else if (ent->deadflag)
			client->ps.pMove.pmType = PMT_DEAD;
		else
			client->ps.pMove.pmType = PMT_NORMAL;

		client->ps.pMove.gravity = sv_gravity->Float();
		pm.state = client->ps.pMove;

		for (i=0 ; i<3 ; i++)
		{
			pm.state.origin[i] = ent->s.origin[i]*8;
			pm.state.velocity[i] = ent->velocity[i]*8;
		}

		if (memcmp (&client->old_pmove, &pm.state, sizeof(pm.state)))
		{
			pm.snapInitial = true;
	//		gi.dprintf ("pmove changed!\n");
		}

		pm.cmd = *ucmd;

#ifdef USE_EXTENDED_GAME_IMPORTS
		pm.trace = PM_trace;
		pm.pointContents = gi.pointcontents;
#endif

		// perform a pmove
#ifdef USE_EXTENDED_GAME_IMPORTS
		gi.Pmove (&pm);
#else
		SV_Pmove (ent, &pm, 0);
#endif

		// save results of pmove
		client->ps.pMove = pm.state;
		client->old_pmove = pm.state;

		for (i=0 ; i<3 ; i++)
		{
			ent->s.origin[i] = pm.state.origin[i]*0.125;
			ent->velocity[i] = pm.state.velocity[i]*0.125;
		}

		Vec3Copy (pm.mins, ent->mins);
		Vec3Copy (pm.maxs, ent->maxs);

		client->resp.cmd_angles[0] = SHORT2ANGLE(ucmd->angles[0]);
		client->resp.cmd_angles[1] = SHORT2ANGLE(ucmd->angles[1]);
		client->resp.cmd_angles[2] = SHORT2ANGLE(ucmd->angles[2]);

		if (ent->groundentity && !pm.groundEntity && (pm.cmd.upMove >= 10) && (pm.waterLevel == 0))
		{
			PlaySoundFrom(ent, CHAN_VOICE, gMedia.Player.Jump);
			PlayerNoise(ent, ent->s.origin, PNOISE_SELF);
		}

		ent->viewheight = pm.viewHeight;
		ent->waterlevel = pm.waterLevel;
		ent->watertype = pm.waterType;
		ent->groundentity = pm.groundEntity;
		if (pm.groundEntity)
			ent->groundentity_linkcount = pm.groundEntity->linkCount;

		if (ent->deadflag)
		{
			client->ps.viewAngles[ROLL] = 40;
			client->ps.viewAngles[PITCH] = -15;
			client->ps.viewAngles[YAW] = client->killer_yaw;
		}
		else
		{
			Vec3Copy (pm.viewAngles, client->v_angle);
			Vec3Copy (pm.viewAngles, client->ps.viewAngles);
		}

		gi.linkentity (ent);

		if (ent->movetype != MOVETYPE_NOCLIP)
			G_TouchTriggers (ent);

		// touch other objects
		for (i=0 ; i<pm.numTouch ; i++)
		{
			other = pm.touchEnts[i];
			for (j=0 ; j<i ; j++)
				if (pm.touchEnts[j] == other)
					break;
			if (j != i)
				continue;	// duplicated
			if (!other->touch)
				continue;
			other->touch (other, ent, NULL, NULL);
		}

	}

	int oldbuttons = client->buttons;
	client->buttons = ucmd->buttons;
	client->latched_buttons |= client->buttons & ~oldbuttons;

	// save light level the player is standing on for
	// monster sighting AI
	ent->light_level = ucmd->lightLevel;

	// fire weapon from final position if needed
	if (client->latched_buttons & BUTTON_ATTACK)
	{
		if (client->resp.spectator) {

			client->latched_buttons = 0;

			if (client->chase_target) {
				client->chase_target = NULL;
				client->ps.pMove.pmFlags &= ~PMF_NO_PREDICTION;
			} else
				GetChaseTarget(ent);

		}
	}

	if (client->resp.spectator) {
		if (ucmd->upMove >= 10) {
			if (!(client->ps.pMove.pmFlags & PMF_JUMP_HELD)) {
				client->ps.pMove.pmFlags |= PMF_JUMP_HELD;
				if (client->chase_target)
					ChaseNext(ent);
				else
					GetChaseTarget(ent);
			}
		} else
			client->ps.pMove.pmFlags &= ~PMF_JUMP_HELD;
	}

	// update chase cam if being followed
	for (i = 1; i <= maxclients->Integer(); i++) {
		other = g_edicts + i;
		if (other->inUse && other->client->chase_target == ent)
			UpdateChaseCam(other);
	}
}


/*
==============
ClientBeginServerFrame

This will be called once for each server frame, before running
any other entities in the world.
==============
*/
void ClientBeginServerFrame (edict_t *ent)
{
	gclient_t	*client;
	int			buttonMask;

	if (level.intermissiontime)
		return;

	client = ent->client;

	if (deathmatch->Integer() &&
		client->pers.spectator != client->resp.spectator &&
		(level.time - client->respawn_time) >= 5) {
		spectator_respawn(ent);
		return;
	}

	// run weapon animations
	if (!client->resp.spectator && client->pers.Weapon)
		client->pers.Weapon->Think (ent);

	if (ent->deadflag)
	{
		// wait for any button just going down
		if ( level.time > client->respawn_time)
		{
			// in deathmatch, only wait for attack button
			if (deathmatch->Integer())
				buttonMask = BUTTON_ATTACK;
			else
				buttonMask = -1;

			if ( ( client->latched_buttons & buttonMask ) ||
				(deathmatch->Integer() && dmFlags.dfForceRespawn ) )
			{
				respawn(ent);
				client->latched_buttons = 0;
			}
		}
		return;
	}

	// add player trail so monsters can follow
	if (!deathmatch->Integer())
		if (!visible (ent, PlayerTrail_LastSpot() ) )
			PlayerTrail_Add (ent->s.oldOrigin);

	client->latched_buttons = 0;
}