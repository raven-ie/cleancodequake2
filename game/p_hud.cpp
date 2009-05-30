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


/*
======================================================================

INTERMISSION

======================================================================
*/

void MoveClientToIntermission (edict_t *ent)
{
	if (deathmatch->Integer() || coop->Integer())
		ent->client->showscores = true;
	Vec3Copy (level.intermission_origin, ent->s.origin);
	ent->client->ps.pMove.origin[0] = level.intermission_origin[0]*8;
	ent->client->ps.pMove.origin[1] = level.intermission_origin[1]*8;
	ent->client->ps.pMove.origin[2] = level.intermission_origin[2]*8;
	Vec3Copy (level.intermission_angle, ent->client->ps.viewAngles);
	ent->client->ps.pMove.pmType = PMT_FREEZE;
	ent->client->ps.gunIndex = 0;
	ent->client->ps.viewBlend[3] = 0;
	ent->client->ps.rdFlags &= ~RDF_UNDERWATER;

	// clean up powerup info
	ent->client->quad_framenum = 0;
	ent->client->invincible_framenum = 0;
	ent->client->breather_framenum = 0;
	ent->client->enviro_framenum = 0;
	ent->client->grenade_blew_up = ent->client->grenade_thrown = false;
	ent->client->grenade_time = 0;

	ent->viewheight = 0;
	ent->s.modelIndex = 0;
	ent->s.modelIndex2 = 0;
	ent->s.modelIndex3 = 0;
	ent->s.effects = 0;
	ent->s.sound = 0;
	ent->solid = SOLID_NOT;

	// add the layout

	if (deathmatch->Integer() || coop->Integer())
		DeathmatchScoreboardMessage (ent, NULL, true);

}

void BeginIntermission (edict_t *targ)
{
	int		i, n;
	edict_t	*ent, *client;

	if (level.intermissiontime)
		return;		// already activated

	game.autosaved = false;

	// respawn any dead clients
	for (i=0 ; i<maxclients->Integer() ; i++)
	{
		client = g_edicts + 1 + i;
		if (!client->inUse)
			continue;
		if (client->health <= 0)
			respawn(client);
	}

	level.intermissiontime = level.time;
	level.changemap = targ->map;

	if (strstr(level.changemap, "*"))
	{
		if (coop->Integer())
		{
			for (i=0 ; i<maxclients->Integer() ; i++)
			{
				client = g_edicts + 1 + i;
				if (!client->inUse)
					continue;
				// strip players of all keys between units
				for (n = 0; n < MAX_CS_ITEMS; n++)
				{
					if (n >= GetNumItems())
						break;
					if (GetItemByIndex(n)->Flags & ITEMFLAG_KEY)
						client->client->pers.Inventory.Set(n, 0);
				}
			}
		}
	}
	else
	{
		if (!deathmatch->Integer())
		{
			level.exitintermission = 1;		// go immediately to the next level
			return;
		}
	}

	level.exitintermission = 0;

	// find an intermission spot
	ent = G_Find (NULL, FOFS(classname), "info_player_intermission");
	if (!ent)
	{	// the map creator forgot to put in an intermission point...
		ent = G_Find (NULL, FOFS(classname), "info_player_start");
		if (!ent)
			ent = G_Find (NULL, FOFS(classname), "info_player_deathmatch");
	}
	else
	{	// chose one of four spots
		i = rand() & 3;
		while (i--)
		{
			ent = G_Find (ent, FOFS(classname), "info_player_intermission");
			if (!ent)	// wrap around the list
				ent = G_Find (ent, FOFS(classname), "info_player_intermission");
		}
	}

	Vec3Copy (ent->s.origin, level.intermission_origin);
	Vec3Copy (ent->s.angles, level.intermission_angle);

	// move all clients to the intermission point
	for (i=0 ; i<maxclients->Integer() ; i++)
	{
		client = g_edicts + 1 + i;
		if (!client->inUse)
			continue;
		MoveClientToIntermission (client);
	}
}


/*
==================
DeathmatchScoreboard

Draw instead of help message.
Note that it isn't that hard to overflow the 1400 byte message limit!
==================
*/
void DeathmatchScoreboard (edict_t *ent)
{
	DeathmatchScoreboardMessage (ent, ent->enemy, true);
}


/*
==================
Cmd_Score_f

Display the scoreboard
==================
*/
void Cmd_Score_f (edict_t *ent)
{
	ent->client->showinventory = false;
	ent->client->showhelp = false;

	if (!deathmatch->Integer() && !coop->Integer())
		return;

	if (ent->client->showscores)
	{
		ent->client->showscores = false;
		return;
	}

	ent->client->showscores = true;
	DeathmatchScoreboard (ent);
}

/*
==================
Cmd_Help_f

Display the current help message
==================
*/
void Cmd_Help_f (edict_t *ent)
{
	// this is for backwards compatability
	if (deathmatch->Integer())
	{
		Cmd_Score_f (ent);
		return;
	}

	ent->client->showinventory = false;
	ent->client->showscores = false;

	if (ent->client->showhelp && (ent->client->pers.game_helpchanged == game.helpchanged))
	{
		ent->client->showhelp = false;
		return;
	}

	ent->client->showhelp = true;
	ent->client->pers.helpchanged = 0;
	HelpComputer (ent);
}


//=======================================================================

/*
===============
G_SetStats
===============
*/
int PowerArmorType (edict_t *ent);
int ArmorIndex (edict_t *ent);
void G_SetStats (edict_t *ent)
{
	//
	// health
	//
	ent->client->ps.stats[STAT_HEALTH_ICON] = gMedia.Hud.HealthPic;
	ent->client->ps.stats[STAT_HEALTH] = ent->health;

	//
	// ammo
	//
	if (ent->client->pers.Weapon)
	{
		if (ent->client->pers.Weapon->WeaponItem && ent->client->pers.Weapon->WeaponItem->Ammo)
		{
			ent->client->ps.stats[STAT_AMMO_ICON] = ent->client->pers.Weapon->WeaponItem->Ammo->IconIndex;
			ent->client->ps.stats[STAT_AMMO] = ent->client->pers.Inventory.Has(ent->client->pers.Weapon->WeaponItem->Ammo->GetIndex());
		}
		else if (ent->client->pers.Weapon->Item && (ent->client->pers.Weapon->Item->Flags & ITEMFLAG_AMMO))
		{
			ent->client->ps.stats[STAT_AMMO_ICON] = ent->client->pers.Weapon->Item->IconIndex;
			ent->client->ps.stats[STAT_AMMO] = ent->client->pers.Inventory.Has(ent->client->pers.Weapon->Item->GetIndex());
		}
		else
		{
			ent->client->ps.stats[STAT_AMMO_ICON] = 0;
			ent->client->ps.stats[STAT_AMMO] = 0;
		}
	}
	else
	{
		ent->client->ps.stats[STAT_AMMO_ICON] = 0;
		ent->client->ps.stats[STAT_AMMO] = 0;
	}
	
	//
	// armor
	//
	int			cells = 0;
	int			power_armor_type = PowerArmorType (ent);
	if (power_armor_type)
	{
		cells = ent->client->pers.Inventory.Has(FindItem("Cells")->GetIndex());
		if (cells == 0)
		{	// ran out of cells for power armor
			ent->flags &= ~FL_POWER_ARMOR;
			PlaySoundFrom(ent, CHAN_ITEM, SoundIndex("misc/power2.wav"));
			power_armor_type = 0;;
		}
	}

	CArmor *Armor = ent->client->pers.Armor;
	if (power_armor_type && (!Armor || (level.framenum & 8) ) )
	{	// flash between power armor and other armor icon
		ent->client->ps.stats[STAT_ARMOR_ICON] = gMedia.Hud.PowerShieldPic;
		ent->client->ps.stats[STAT_ARMOR] = cells;
	}
	else if (Armor)
	{
		ent->client->ps.stats[STAT_ARMOR_ICON] = Armor->IconIndex;
		ent->client->ps.stats[STAT_ARMOR] = ent->client->pers.Inventory.Has(Armor);
	}
	else
	{
		ent->client->ps.stats[STAT_ARMOR_ICON] = 0;
		ent->client->ps.stats[STAT_ARMOR] = 0;
	}

	//
	// pickup message
	//
	if (level.time > ent->client->pickup_msg_time)
	{
		ent->client->ps.stats[STAT_PICKUP_ICON] = 0;
		ent->client->ps.stats[STAT_PICKUP_STRING] = 0;
	}

	//
	// timers
	//
	if (ent->client->quad_framenum > level.framenum)
	{
		ent->client->ps.stats[STAT_TIMER_ICON] = gMedia.Hud.QuadPic;
		ent->client->ps.stats[STAT_TIMER] = (ent->client->quad_framenum - level.framenum)/10;
	}
	else if (ent->client->invincible_framenum > level.framenum)
	{
		ent->client->ps.stats[STAT_TIMER_ICON] = gMedia.Hud.InvulPic;
		ent->client->ps.stats[STAT_TIMER] = (ent->client->invincible_framenum - level.framenum)/10;
	}
	else if (ent->client->enviro_framenum > level.framenum)
	{
		ent->client->ps.stats[STAT_TIMER_ICON] = gMedia.Hud.EnviroPic;
		ent->client->ps.stats[STAT_TIMER] = (ent->client->enviro_framenum - level.framenum)/10;
	}
	else if (ent->client->breather_framenum > level.framenum)
	{
		ent->client->ps.stats[STAT_TIMER_ICON] = gMedia.Hud.RebreatherPic;
		ent->client->ps.stats[STAT_TIMER] = (ent->client->breather_framenum - level.framenum)/10;
	}
	// Paril, show silencer
	else if (ent->client->silencer_shots)
	{
		ent->client->ps.stats[STAT_TIMER_ICON] = gMedia.Hud.SilencerPic;
		ent->client->ps.stats[STAT_TIMER] = ent->client->silencer_shots;
	}
	// Paril
	else
	{
		ent->client->ps.stats[STAT_TIMER_ICON] = 0;
		ent->client->ps.stats[STAT_TIMER] = 0;
	}

	//
	// selected item
	//
	if (ent->client->pers.Inventory.SelectedItem == -1)
		ent->client->ps.stats[STAT_SELECTED_ICON] = 0;
	else
		ent->client->ps.stats[STAT_SELECTED_ICON] = GetItemByIndex(ent->client->pers.Inventory.SelectedItem)->IconIndex;//ImageIndex (itemlist[ent->client->pers.selected_item].icon);

	ent->client->ps.stats[STAT_SELECTED_ITEM] = ent->client->pers.Inventory.SelectedItem;

	//
	// layouts
	//
	ent->client->ps.stats[STAT_LAYOUTS] = 0;

	if (ent->client->pers.health <= 0 || ent->client->resp.MenuState.InMenu ||
		((deathmatch->Integer() && (level.intermissiontime || ent->client->showscores)) || 
		!deathmatch->Integer() && ent->client->showhelp))
		ent->client->ps.stats[STAT_LAYOUTS] |= 1;
	if (ent->client->showinventory && ent->client->pers.health > 0)
		ent->client->ps.stats[STAT_LAYOUTS] |= 2;

	//
	// frags
	//
	ent->client->ps.stats[STAT_FRAGS] = ent->client->resp.score;

	//
	// help icon / current weapon if not shown
	//
	if (ent->client->pers.helpchanged && (level.framenum&8) )
		ent->client->ps.stats[STAT_HELPICON] = gMedia.Hud.HelpPic;

	else if ( (ent->client->pers.hand == CENTER_HANDED || ent->client->ps.fov > 91)
		&& ent->client->pers.Weapon)
		ent->client->ps.stats[STAT_HELPICON] = ent->client->pers.Weapon->Item->IconIndex;
	else
		ent->client->ps.stats[STAT_HELPICON] = 0;

	ent->client->ps.stats[STAT_SPECTATOR] = 0;
}

/*
===============
G_CheckChaseStats
===============
*/
void G_CheckChaseStats (edict_t *ent)
{
	int i;
	gclient_t *cl;

	for (i = 1; i <= maxclients->Integer(); i++) {
		cl = g_edicts[i].client;
		if (!g_edicts[i].inUse || cl->chase_target != ent)
			continue;
		memcpy(cl->ps.stats, ent->client->ps.stats, sizeof(cl->ps.stats));
		G_SetSpectatorStats(g_edicts + i);
	}
}

/*
===============
G_SetSpectatorStats
===============
*/
void G_SetSpectatorStats (edict_t *ent)
{
	gclient_t *cl = ent->client;

	if (!cl->chase_target)
		G_SetStats (ent);

	cl->ps.stats[STAT_SPECTATOR] = 1;

	// layouts are independant in spectator
	cl->ps.stats[STAT_LAYOUTS] = 0;
	if (cl->pers.health <= 0 || level.intermissiontime || cl->showscores)
		cl->ps.stats[STAT_LAYOUTS] |= 1;
	if (cl->showinventory && cl->pers.health > 0)
		cl->ps.stats[STAT_LAYOUTS] |= 2;

	if (cl->chase_target && cl->chase_target->inUse)
		cl->ps.stats[STAT_CHASE] = CS_PLAYERSKINS + 
			(cl->chase_target - g_edicts) - 1;
	else
		cl->ps.stats[STAT_CHASE] = 0;
}

