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
// g_misc.c

#include "g_local.h"

/*
=================
debris
=================
*/
void debris_die (edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
{
	G_FreeEdict (self);
}

void ThrowDebris (edict_t *self, char *modelname, float speed, vec3_t origin)
{
	edict_t	*chunk;
	vec3_t	v;

	chunk = G_Spawn();
	Vec3Copy (origin, chunk->state.origin);
	chunk->state.modelIndex = ModelIndex(modelname);
	v[0] = 100 * crandom();
	v[1] = 100 * crandom();
	v[2] = 100 + 100 * crandom();
	Vec3MA (self->velocity, speed, v, chunk->velocity);
	chunk->movetype = MOVETYPE_BOUNCE;
	chunk->solid = SOLID_NOT;
	chunk->avelocity[0] = random()*600;
	chunk->avelocity[1] = random()*600;
	chunk->avelocity[2] = random()*600;
	chunk->think = G_FreeEdict;
	chunk->nextthink = level.framenum + 50 + random()*50;
	chunk->state.frame = 0;
	chunk->flags = 0;
	chunk->classname = "debris";
	chunk->takedamage = true;
	chunk->die = debris_die;
	gi.linkentity (chunk);
}


void BecomeExplosion1 (edict_t *self)
{
#ifdef CLEANCTF_ENABLED
//ZOID
	//flags are important
	if (strcmp(self->classname, "item_flag_team1") == 0)
	{
		CTFResetFlag(CTF_TEAM1); // this will free self!
		BroadcastPrintf(PRINT_HIGH, "The %s flag has returned!\n",
			CTFTeamName(CTF_TEAM1));
		return;
	}
	if (strcmp(self->classname, "item_flag_team2") == 0)
	{
		CTFResetFlag(CTF_TEAM2); // this will free self!
		BroadcastPrintf(PRINT_HIGH, "The %s flag has returned!\n",
			CTFTeamName(CTF_TEAM2));
		return;
	}
	// techs are important too
	if (self->item && (self->item->Flags & ITEMFLAG_TECH))
	{
		CTFRespawnTech(self); // this frees self!
		return;
	}
//ZOID
#endif

	CTempEnt_Explosions::RocketExplosion (self->state.origin, self);
	G_FreeEdict (self);
}

void BecomeExplosion2 (edict_t *self)
{
	CTempEnt_Explosions::GrenadeExplosion (self->state.origin, self);
	G_FreeEdict (self);
}

/*QUAKED viewthing (0 .5 .8) (-8 -8 -8) (8 8 8)
Just for the debugging level.  Don't use
*/
void TH_viewthing(edict_t *ent)
{
	ent->state.frame = (ent->state.frame + 1) % 7;
	ent->nextthink = level.framenum + FRAMETIME;
}

void SP_viewthing(edict_t *ent)
{
	DebugPrintf ("viewthing spawned\n");

	ent->movetype = MOVETYPE_NONE;
	ent->solid = SOLID_BBOX;
	ent->state.renderFx = RF_FRAMELERP;
	Vec3Set (ent->mins, -16, -16, -24);
	Vec3Set (ent->maxs, 16, 16, 32);
	ent->state.modelIndex = ModelIndex ("models/objects/banner/tris.md2");
	gi.linkentity (ent);
	ent->nextthink = level.framenum + 5;
	ent->think = TH_viewthing;
	return;
}

