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
// cc_baseentity.cpp
// Base entity class code
//

#include "cc_local.h"

// Creating a new entity via constructor.
CBaseEntity::CBaseEntity ()
{
	gameEntity = G_Spawn ();
	gameEntity->Entity = this;

	EntityFlags |= ENT_BASE;
};

CBaseEntity::CBaseEntity (int Index)
{
	gameEntity = &g_edicts[Index];
	gameEntity->Entity = this;

	EntityFlags |= ENT_BASE;
}

CBaseEntity::~CBaseEntity ()
{
	gameEntity->Entity = NULL;
	G_FreeEdict (gameEntity); // "delete" the entity
};

// Funtions below are to link the private gameEntity together
CBaseEntity		*CBaseEntity::GetOwner	()
{
	return (gameEntity->owner) ? gameEntity->owner->Entity : NULL;
}
void			CBaseEntity::SetOwner	(CBaseEntity *ent)
{
	gameEntity->owner = ent->gameEntity;
}

int				CBaseEntity::GetClipmask	()
{
	return gameEntity->clipMask;
}
void			CBaseEntity::SetClipmask (int mask)
{
	gameEntity->clipMask = mask;
}

solid_t			CBaseEntity::GetSolid ()
{
	return gameEntity->solid;
}
void			CBaseEntity::SetSolid (solid_t solid)
{
	gameEntity->solid = solid;
}

// Vectors are a little weird.
// Since we can't really "return" an array we have to pass it
void			CBaseEntity::GetMins (vec3_t in)
{
	Vec3Copy (gameEntity->mins, in);
}
void			CBaseEntity::GetMaxs (vec3_t in)
{
	Vec3Copy (gameEntity->maxs, in);
}

void			CBaseEntity::GetAbsMin (vec3_t in)
{
	Vec3Copy (gameEntity->absMin, in);
}
void			CBaseEntity::GetAbsMax (vec3_t in)
{
	Vec3Copy (gameEntity->absMax, in);
}
void			CBaseEntity::GetSize (vec3_t in)
{
	Vec3Copy (gameEntity->size, in);
}

// Unless, of course, you use the vec3f class :D
vec3f			CBaseEntity::GetMins ()
{
	return vec3f(gameEntity->mins);
}
vec3f			CBaseEntity::GetMaxs ()
{
	return vec3f(gameEntity->maxs);
}

vec3f			CBaseEntity::GetAbsMin ()
{
	return vec3f(gameEntity->absMin);
}
vec3f			CBaseEntity::GetAbsMax ()
{
	return vec3f(gameEntity->absMax);
}
vec3f			CBaseEntity::GetSize ()
{
	return vec3f(gameEntity->size);
}

// Same for setting
void			CBaseEntity::SetMins (vec3_t in)
{
	Vec3Copy (in, gameEntity->mins);
}
void			CBaseEntity::SetMaxs (vec3_t in)
{
	Vec3Copy (in, gameEntity->maxs);
}

void			CBaseEntity::SetAbsMin (vec3_t in)
{
	Vec3Copy (in, gameEntity->absMin);
}
void			CBaseEntity::SetAbsMax (vec3_t in)
{
	Vec3Copy (in, gameEntity->absMax);
}
void			CBaseEntity::SetSize (vec3_t in)
{
	Vec3Copy (in, gameEntity->size);
}

// Vec3f
void			CBaseEntity::SetMins (vec3f in)
{
	gameEntity->mins[0] = in.X;
	gameEntity->mins[1] = in.Y;
	gameEntity->mins[2] = in.Z;
}
void			CBaseEntity::SetMaxs (vec3f in)
{
	gameEntity->maxs[0] = in.X;
	gameEntity->maxs[1] = in.Y;
	gameEntity->maxs[2] = in.Z;
}

void			CBaseEntity::SetAbsMin (vec3f in)
{
	gameEntity->absMin[0] = in.X;
	gameEntity->absMin[1] = in.Y;
	gameEntity->absMin[2] = in.Z;
}
void			CBaseEntity::SetAbsMax (vec3f in)
{
	gameEntity->absMax[0] = in.X;
	gameEntity->absMax[1] = in.Y;
	gameEntity->absMax[2] = in.Z;
}
void			CBaseEntity::SetSize (vec3f in)
{
	gameEntity->size[0] = in.X;
	gameEntity->size[1] = in.Y;
	gameEntity->size[2] = in.Z;
}

int				CBaseEntity::GetSvFlags ()
{
	return gameEntity->svFlags;
}
void			CBaseEntity::SetSvFlags (int SVFlags)
{
	gameEntity->svFlags = SVFlags;
}

int				CBaseEntity::GetAreaNum (bool second)
{
	return ((second) ? gameEntity->areaNum2 : gameEntity->areaNum);
}
void			CBaseEntity::SetAreaNum (int num, bool second)
{
	((second) ? gameEntity->areaNum2 : gameEntity->areaNum) = num;
}

link_t			*CBaseEntity::GetArea ()
{
	return &gameEntity->area;
}
void			CBaseEntity::ClearArea ()
{
	memset (&gameEntity->area, 0, sizeof(gameEntity->area));
}

int				CBaseEntity::GetLinkCount ()
{
	return gameEntity->linkCount;
}

bool			CBaseEntity::IsInUse ()
{
	return !!(gameEntity->inUse);
}
void			CBaseEntity::SetInUse (bool inuse)
{
	gameEntity->inUse = (inuse == true) ? 1 : 0;
}

void			CBaseEntity::Link ()
{
	gi.linkentity (gameEntity);
}

void			CBaseEntity::Unlink ()
{
	gi.unlinkentity (gameEntity);
}