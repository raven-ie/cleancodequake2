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
// cc_makron_stand.cpp
// 
//

#include "cc_local.h"
#include "m_boss32.h"

void Use_Boss3 (edict_t *ent, edict_t *other, edict_t *activator)
{
	CTempEnt::BossTeleport (ent->state.origin);
	G_FreeEdict (ent);
}

void Think_Boss3Stand (edict_t *ent)
{
	if (ent->state.frame == FRAME_stand260)
		ent->state.frame = FRAME_stand201;
	else
		ent->state.frame++;
	ent->nextthink = level.framenum + FRAMETIME;
}

/*QUAKED monster_boss3_stand (1 .5 0) (-32 -32 0) (32 32 90)

Just stands and cycles in one place until targeted, then teleports away.
*/
void SP_monster_boss3_stand (edict_t *self)
{
	if (game.mode & GAME_DEATHMATCH)
	{
		G_FreeEdict (self);
		return;
	}

	self->movetype = MOVETYPE_STEP;
	self->solid = SOLID_BBOX;
	self->model = "models/monsters/boss3/rider/tris.md2";
	self->state.modelIndex = ModelIndex (self->model);
	self->state.frame = FRAME_stand201;

	SoundIndex ("misc/bigtele.wav");

	Vec3Set (self->mins, -32, -32, 0);
	Vec3Set (self->maxs, 32, 32, 90);

	self->use = Use_Boss3;
	self->think = Think_Boss3Stand;
	self->nextthink = level.framenum + FRAMETIME;
	gi.linkentity (self);
}
