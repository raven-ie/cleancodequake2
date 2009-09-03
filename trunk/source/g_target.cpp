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

//==========================================================

void Use_Target_Help (edict_t *ent, edict_t *other, edict_t *activator)
{
	if (ent->spawnflags & 1)
		Q_strncpyz (game.helpmessage1, ent->message, sizeof(game.helpmessage2)-1);
	else
		Q_strncpyz (game.helpmessage2, ent->message, sizeof(game.helpmessage1)-1);

	game.helpchanged++;
}

/*QUAKED target_help (1 0 1) (-16 -16 -24) (16 16 24) help1
When fired, the "message" key becomes the current personal computer string, and the message light will be set on all clients status bars.
*/
void SP_target_help(edict_t *ent)
{
	if (game.mode & GAME_DEATHMATCH)
	{	// auto-remove for deathmatch
		G_FreeEdict (ent);
		return;
	}

	if (!ent->message)
	{
		//gi.dprintf ("%s with no message at (%f %f %f)\n", ent->classname, ent->state.origin[0], ent->state.origin[1], ent->state.origin[2]);
		MapPrint (MAPPRINT_ERROR, ent, ent->state.origin, "No message\n");
		G_FreeEdict (ent);
		return;
	}
	ent->use = Use_Target_Help;
}

//==========================================================

/*QUAKED target_explosion (1 0 0) (-8 -8 -8) (8 8 8)
Spawns an explosion temporary entity when used.

"delay"		wait this long before going off
"dmg"		how much radius damage should be done, defaults to 0
*/
void target_explosion_explode (edict_t *self)
{
	float		save;

	CTempEnt_Explosions::RocketExplosion (self->state.origin, self);

	T_RadiusDamage (self, self->activator, self->dmg, NULL, self->dmg+40, MOD_EXPLOSIVE);

	save = self->delay;
	self->delay = 0;
	G_UseTargets (self, self->activator);
	self->delay = save;
}

void use_target_explosion (edict_t *self, edict_t *other, edict_t *activator)
{
	self->activator = activator;

	if (!self->delay)
	{
		target_explosion_explode (self);
		return;
	}

	self->think = target_explosion_explode;

	// Backwards compatibility
	self->nextthink = level.framenum + (self->delay * 10);
}

void SP_target_explosion (edict_t *ent)
{
	ent->use = use_target_explosion;
	ent->svFlags = SVF_NOCLIENT;
}

//==========================================================

void SP_target_location (edict_t *self)
{
	// TODO FIXME
	//G_FreeEdict (self);
}

void SP_target_position (edict_t *self)
{
	// TODO FIXME
	//G_FreeEdict (self);
}

//==========================================================

/*QUAKED target_lightramp (0 .5 .8) (-8 -8 -8) (8 8 8) TOGGLE
speed		How many seconds the ramping will take
message		two letters; starting lightlevel and ending lightlevel
*/

void target_lightramp_think (edict_t *self)
{
	char	style[2];

	style[0] = 'a' + self->movedir[0] + (level.framenum - self->timestamp) / 0.1f * self->movedir[2];
	style[1] = 0;
	ConfigString (CS_LIGHTS+self->enemy->style, style);

	if ((level.framenum - self->timestamp) < self->speed)
		self->nextthink = level.framenum + FRAMETIME;
	else if (self->spawnflags & 1)
	{
		char	temp;

		temp = self->movedir[0];
		self->movedir[0] = self->movedir[1];
		self->movedir[1] = temp;
		self->movedir[2] *= -1;
	}
}

void target_lightramp_use (edict_t *self, edict_t *other, edict_t *activator)
{
	if (!self->enemy)
	{
		edict_t		*e;

		// check all the targets
		e = NULL;
		while (1)
		{
			e = G_Find (e, FOFS(targetname), self->target);
			if (!e)
				break;
			if (strcmp(e->classname, "light") != 0)
			{
				//gi.dprintf("%s at (%f %f %f): target %s (%s at (%f %f %f)) is not a light\n", self->classname, self->state.origin[0], self->state.origin[1], self->state.origin[2], self->target, e->classname, e->state.origin[0], e->state.origin[1], e->state.origin[2]);
				MapPrint (MAPPRINT_WARNING, self, self->state.origin, "Target \"%s\" is not a light\n", self->target);
			}
			else
			{
				self->enemy = e;
			}
		}

		if (!self->enemy)
		{
			//gi.dprintf("%s target %s not found at (%f %f %f)\n", self->classname, self->target, self->state.origin[0], self->state.origin[1], self->state.origin[2]);
			MapPrint (MAPPRINT_ERROR, self, self->state.origin, "Target \"%s\" not found\n", self->target);
			G_FreeEdict (self);
			return;
		}
	}

	self->timestamp = level.framenum;
	target_lightramp_think (self);
}

void SP_target_lightramp (edict_t *self)
{
	if (!self->message || strlen(self->message) != 2 || self->message[0] < 'a' || self->message[0] > 'z' || self->message[1] < 'a' || self->message[1] > 'z' || self->message[0] == self->message[1])
	{
		//gi.dprintf("target_lightramp has bad ramp (%s) at (%f %f %f)\n", self->message, self->state.origin[0], self->state.origin[1], self->state.origin[2]);
		MapPrint (MAPPRINT_ERROR, self, self->state.origin, "Bad ramp (%s)\n", self->message);
		G_FreeEdict (self);
		return;
	}

	if (game.mode & GAME_DEATHMATCH)
	{
		G_FreeEdict (self);
		return;
	}

	if (!self->target)
	{
		//gi.dprintf("%s with no target at (%f %f %f)\n", self->classname, self->state.origin[0], self->state.origin[1], self->state.origin[2]);
		MapPrint (MAPPRINT_ERROR, self, self->state.origin, "No target\n");
		G_FreeEdict (self);
		return;
	}

	self->svFlags |= SVF_NOCLIENT;
	self->use = target_lightramp_use;
	self->think = target_lightramp_think;

	self->movedir[0] = self->message[0] - 'a';
	self->movedir[1] = self->message[1] - 'a';
	self->movedir[2] = (self->movedir[1] - self->movedir[0]) / (self->speed / 0.1f);
}

//==========================================================

/*QUAKED target_earthquake (1 0 0) (-8 -8 -8) (8 8 8)
When triggered, this initiates a level-wide earthquake.
All players and monsters are affected.
"speed"		severity of the quake (default:200)
"count"		duration of the quake (default:5)
*/

void target_earthquake_think (edict_t *self)
{
	int		i;
	edict_t	*e;

	if (self->last_move_time < level.framenum)
	{
		PlaySoundAt (self->state.origin, self, CHAN_AUTO, self->noise_index, 1.0, ATTN_NONE);
		self->last_move_time = level.framenum + 5;
	}

	for (i=1, e=g_edicts+i; i < globals.numEdicts; i++,e++)
	{
		if (!e->inUse)
			continue;
		if (!e->client)
			continue;
		if (!e->groundentity)
			continue;

		e->groundentity = NULL;
		e->velocity[0] += crandom()* 150;
		e->velocity[1] += crandom()* 150;
		e->velocity[2] = self->speed * (100.0 / e->mass);
	}

	if (level.framenum < self->timestamp)
		self->nextthink = level.framenum + FRAMETIME;
}

void target_earthquake_use (edict_t *self, edict_t *other, edict_t *activator)
{
	// Paril, Backwards compatibility
	self->timestamp = level.framenum + (self->count * 10);
	self->nextthink = level.framenum + FRAMETIME;
	self->activator = activator;
	self->last_move_time = 0;
}

void SP_target_earthquake (edict_t *self)
{
	if (!self->targetname)
		MapPrint (MAPPRINT_ERROR, self, self->state.origin, "No targetname\n");
		//gi.dprintf("untargeted %s at (%f %f %f)\n", self->classname, self->state.origin[0], self->state.origin[1], self->state.origin[2]);

	if (!self->count)
		self->count = 5;

	if (!self->speed)
		self->speed = 200;

	self->svFlags |= SVF_NOCLIENT;
	self->think = target_earthquake_think;
	self->use = target_earthquake_use;

	self->noise_index = SoundIndex ("world/quake.wav");
}
