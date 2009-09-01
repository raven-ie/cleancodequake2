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

/*QUAKED func_timer (0.3 0.1 0.6) (-8 -8 -8) (8 8 8) START_ON
"wait"			base time between triggering all targets, default is 1
"random"		wait variance, default is 0

so, the basic time between firing is a random time between
(wait - random) and (wait + random)

"delay"			delay before first firing when turned on, default is 0

"pausetime"		additional delay used only the very first time
				and only if spawned with START_ON

These can used but not touched.
*/
void func_timer_think (edict_t *self)
{
	G_UseTargets (self, self->activator);
	self->nextthink = level.framenum + ((self->wait + crandom() * self->random) * 10);
}

void func_timer_use (edict_t *self, edict_t *other, edict_t *activator)
{
	self->activator = activator;

	// if on, turn it off
	if (self->nextthink)
	{
		self->nextthink = 0;
		return;
	}

	// turn it on
	if (self->delay)
		self->nextthink = level.framenum + (self->delay * 10);
	else
		func_timer_think (self);
}

void SP_func_timer (edict_t *self)
{
	if (!self->wait)
		self->wait = 1.0;

	self->use = func_timer_use;
	self->think = func_timer_think;

	if (self->random >= self->wait)
	{
		self->random = self->wait - FRAMETIME;
		// Paril FIXME
		// This to me seems like a very silly warning.
		MapPrint (MAPPRINT_WARNING, self, self->state.origin, "Random is greater than or equal to wait\n");
		//gi.dprintf("func_timer at (%f %f %f) has random >= wait\n", self->state.origin[0], self->state.origin[1], self->state.origin[2]);
	}

	if (self->spawnflags & 1)
	{
		// lots of backwards compatibility
		self->nextthink = level.framenum + 10 + (st.pausetime + self->delay + self->wait + (crandom() * self->random) * 10);
		self->activator = self;
	}

	self->svFlags = SVF_NOCLIENT;
}


/*QUAKED func_conveyor (0 .5 .8) ? START_ON TOGGLE
Conveyors are stationary brushes that move what's on them.
The brush should be have a surface with at least one current content enabled.
speed	default 100
*/

void func_conveyor_use (edict_t *self, edict_t *other, edict_t *activator)
{
	if (self->spawnflags & 1)
	{
		self->speed = 0;
		self->spawnflags &= ~1;
	}
	else
	{
		self->speed = self->count;
		self->spawnflags |= 1;
	}

	if (!(self->spawnflags & 2))
		self->count = 0;
}

void SP_func_conveyor (edict_t *self)
{
	if (!self->speed)
		self->speed = 100;

	if (!(self->spawnflags & 1))
	{
		self->count = self->speed;
		self->speed = 0;
	}

	self->use = func_conveyor_use;

	SetModel (self, self->model);
	self->solid = SOLID_BSP;
	gi.linkentity (self);
}

/*QUAKED func_killbox (1 0 0) ?
Kills everything inside when fired, irrespective of protection.
*/
void use_killbox (edict_t *self, edict_t *other, edict_t *activator)
{
	KillBox (self);
}

void SP_func_killbox (edict_t *ent)
{
	SetModel (ent, ent->model);
	ent->use = use_killbox;
	ent->svFlags = SVF_NOCLIENT;
}

