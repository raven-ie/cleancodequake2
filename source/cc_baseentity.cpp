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

CEntityState::CEntityState () :
state(NULL)
{
};

CEntityState::CEntityState (entityStateOld_t *state) :
state(state)
{
};

void	CEntityState::SetNumber(int value)
{
	state->number = value;
}

int		CEntityState::GetNumber		()
{
	return state->number;
}

void	CEntityState::GetOrigin		(vec3_t in)
{
	Vec3Copy (state->origin, in);
}
vec3f	CEntityState::GetOrigin		()
{
	return vec3f(state->origin);
}

void	CEntityState::GetAngles		(vec3_t in)
{
	Vec3Copy (state->angles, in);
}
vec3f	CEntityState::GetAngles		()
{
	return vec3f(state->angles);
}

void	CEntityState::GetOldOrigin	(vec3_t in)
{
	Vec3Copy (state->oldOrigin, in);
}
vec3f	CEntityState::GetOldOrigin	()
{
	return vec3f(state->oldOrigin);
}

void	CEntityState::SetOrigin		(vec3_t in)
{
	Vec3Copy (in, state->origin);
}
void	CEntityState::SetOrigin		(vec3f &in)
{
	state->origin[0] = in.X;
	state->origin[1] = in.Y;
	state->origin[2] = in.Z;
}

void	CEntityState::SetAngles		(vec3_t in)
{
	Vec3Copy (in, state->angles);
}
void	CEntityState::SetAngles		(vec3f &in)
{
	state->angles[0] = in.X;
	state->angles[1] = in.Y;
	state->angles[2] = in.Z;
}

void	CEntityState::SetOldOrigin	(vec3_t in)
{
	Vec3Copy (in, state->oldOrigin);
}
void	CEntityState::SetOldOrigin	(vec3f &in)
{
	state->oldOrigin[0] = in.X;
	state->oldOrigin[1] = in.Y;
	state->oldOrigin[2] = in.Z;
}

// Can be 1, 2, 3, or 4
int		CEntityState::GetModelIndex	(uint8 index)
{
	switch (index)
	{
	case 1:
		return state->modelIndex;
	case 2:
		return state->modelIndex2;
	case 3:
		return state->modelIndex3;
	case 4:
		return state->modelIndex4;
	default:
		_CC_ASSERT_EXPR(0, "index for GetModelIndex is out of bounds");
		return 0;
		break;
	}
}
void	CEntityState::SetModelIndex	(MediaIndex value, uint8 index)
{
	switch (index)
	{
	case 1:
		state->modelIndex = value;
		break;
	case 2:
		state->modelIndex2 = value;
		break;
	case 3:
		state->modelIndex3 = value;
		break;
	case 4:
		state->modelIndex4 = value;
		break;
	default:
		_CC_ASSERT_EXPR(0, "index for SetModelIndex is out of bounds");
		break;
	}
}

int		CEntityState::GetFrame		()
{
	return state->frame;
}
void	CEntityState::SetFrame		(int value)
{
	_CC_ASSERT_EXPR((value >= 0 && value < 512), "value for SetFrame is out of bounds");
	state->frame = value;
}

int	CEntityState::GetSkinNum		()
{
	return state->skinNum;
}
void	CEntityState::SetSkinNum		(int value)
{
	state->skinNum = value;
}

uint32	CEntityState::GetEffects		()
{
	return state->effects;
}
void	CEntityState::SetEffects		(uint32 value)
{
	state->effects = value;
}
void	CEntityState::AddEffects		(uint32 value)
{
	state->effects |= value;
}
void	CEntityState::RemoveEffects		(uint32 value)
{
	state->effects &= ~value;
}

int		CEntityState::GetRenderEffects	()
{
	return state->renderFx;
}
void	CEntityState::SetRenderEffects	(int value)
{
	state->renderFx = value;
}
void	CEntityState::AddRenderEffects	(int value)
{
	state->renderFx |= value;
}
void	CEntityState::RemoveRenderEffects	(int value)
{
	state->renderFx &= ~value;
}

MediaIndex	CEntityState::GetSound		()
{
	return state->sound;
}
void		CEntityState::SetSound		(MediaIndex value)
{
	state->sound = value;
}

int		CEntityState::GetEvent			()
{
	return state->event;
}
void	CEntityState::SetEvent			(int value)
{
	state->event = value;
}

void G_InitEdict (edict_t *e)
{
	e->inUse = true;
	e->classname = "noclass";
	e->gravity = 1.0;
	e->state.number = e - g_edicts;
}

/*
=================
G_Spawn

Either finds a free edict, or allocates a new one.
Try to avoid reusing an entity that was recently freed, because it
can cause the client to think the entity morphed into something else
instead of being removed and recreated, which can cause interpolated
angles and bad trails.
=================
*/
edict_t *G_Spawn (void)
{
	int			i;
	edict_t		*e;

	e = &g_edicts[game.maxclients+1];
	for (i = game.maxclients+1; i < globals.numEdicts; i++, e++)
	{
		// the first couple seconds of server time can involve a lot of
		// freeing and allocating, so relax the replacement policy
		if (!e->inUse && (e->freetime < 20 || level.framenum - e->freetime > 5))
		{
			if (e->Entity && e->Entity->Freed)
			{
				QDelete e->Entity;
				e->Entity = NULL;
			}

_CC_DISABLE_DEPRECATION
			G_InitEdict (e);
_CC_ENABLE_DEPRECATION

			return e;
		}
	}
	
	if (i == game.maxentities)
		GameError ("ED_Alloc: no free edicts");
		
	globals.numEdicts++;

_CC_DISABLE_DEPRECATION
	G_InitEdict (e);
_CC_ENABLE_DEPRECATION

	return e;
}

/*
=================
G_FreeEdict

Marks the edict as free
=================
*/
void G_FreeEdict (edict_t *ed)
{
	gi.unlinkentity (ed);		// unlink from world

	// Paril, hack
	CBaseEntity *Entity = ed->Entity;
	memset (ed, 0, sizeof(*ed));
	ed->Entity = Entity;
	ed->classname = "freed";
	ed->freetime = level.framenum;
	ed->inUse = false;
}


// Creating a new entity via constructor.
CBaseEntity::CBaseEntity ()
{
_CC_DISABLE_DEPRECATION
	gameEntity = G_Spawn ();
_CC_ENABLE_DEPRECATION
	gameEntity->Entity = this;

	Freed = false;
	EntityFlags |= ENT_BASE;

	State = CEntityState(&gameEntity->state);
};

CBaseEntity::CBaseEntity (int Index)
{
	gameEntity = &g_edicts[Index];
	gameEntity->Entity = this;
	gameEntity->state.number = Index;

	Freed = false;
	EntityFlags |= ENT_BASE;
	State = CEntityState(&gameEntity->state);
}

CBaseEntity::~CBaseEntity ()
{
	gameEntity->Entity = NULL;

_CC_DISABLE_DEPRECATION
	G_FreeEdict (gameEntity); // "delete" the entity
_CC_ENABLE_DEPRECATION
};

// Funtions below are to link the private gameEntity together
CBaseEntity		*CBaseEntity::GetOwner	()
{
	return (gameEntity->owner) ? gameEntity->owner->Entity : NULL;
}
void			CBaseEntity::SetOwner	(CBaseEntity *ent)
{
	if (!ent || !ent->gameEntity)
	{
		gameEntity->owner = NULL;
		return;
	}

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
void			CBaseEntity::SetMins (vec3f &in)
{
	gameEntity->mins[0] = in.X;
	gameEntity->mins[1] = in.Y;
	gameEntity->mins[2] = in.Z;
}
void			CBaseEntity::SetMaxs (vec3f &in)
{
	gameEntity->maxs[0] = in.X;
	gameEntity->maxs[1] = in.Y;
	gameEntity->maxs[2] = in.Z;
}

void			CBaseEntity::SetAbsMin (vec3f &in)
{
	gameEntity->absMin[0] = in.X;
	gameEntity->absMin[1] = in.Y;
	gameEntity->absMin[2] = in.Z;
}
void			CBaseEntity::SetAbsMax (vec3f &in)
{
	gameEntity->absMax[0] = in.X;
	gameEntity->absMax[1] = in.Y;
	gameEntity->absMax[2] = in.Z;
}
void			CBaseEntity::SetSize (vec3f &in)
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
	return gameEntity->inUse;
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

void			CBaseEntity::Free ()
{
	Unlink ();

	memset (gameEntity, 0, sizeof(*gameEntity));
	gameEntity->Entity = this;
	gameEntity->classname = "freed";
	gameEntity->freetime = level.framenum;
	SetInUse(false);

	Freed = true;
}

void	CBaseEntity::PlaySound (EEntSndChannel channel, MediaIndex soundIndex, float volume, int attenuation, float timeOfs)
{
	PlaySoundFrom (gameEntity, channel, soundIndex, volume, attenuation, timeOfs);
};

void	CBaseEntity::PlayPositionedSound (vec3_t origin, EEntSndChannel channel, MediaIndex soundIndex, float volume, int attenuation, float timeOfs)
{
	PlaySoundAt (origin, gameEntity, channel, soundIndex, volume, attenuation, timeOfs);
};

void	CBaseEntity::KillBox ()
{
	CTrace		tr;

	while (1)
	{
		tr = CTrace (State.GetOrigin(), GetMins(), GetMaxs(), State.GetOrigin(), NULL, CONTENTS_MASK_PLAYERSOLID);
		if (!tr.ent || !tr.Ent)
			break;

		if ((tr.Ent->EntityFlags & ENT_HURTABLE) && entity_cast<CHurtableEntity>(tr.Ent)->CanTakeDamage)
		{
			// nail it
			entity_cast<CHurtableEntity>(tr.Ent)->TakeDamage (this, this, vec3fOrigin, State.GetOrigin(),
																vec3fOrigin, 100000, 0, DAMAGE_NO_PROTECTION, MOD_TELEFRAG);
		}

		if (tr.Ent->GetSolid())
			break;
	}
};

CMapEntity::CMapEntity () : 
CBaseEntity()
{
};

CMapEntity::CMapEntity (int Index) : 
CBaseEntity(Index)
{
};

CPrivateEntity::CPrivateEntity (int Index)
{
};

CPrivateEntity::CPrivateEntity ()
{
	InUse = true;
};

bool			CPrivateEntity::IsInUse ()
{
	return InUse;
}
void			CPrivateEntity::SetInUse (bool inuse)
{
	InUse = inuse;
}

void CPrivateEntity::Free ()
{
	Freed = true;
}

void CBaseEntity::BecomeExplosion (bool grenade)
{
	if (grenade)
		CTempEnt_Explosions::GrenadeExplosion (State.GetOrigin(), gameEntity);
	else
		CTempEnt_Explosions::RocketExplosion (State.GetOrigin(), gameEntity);
	Free ();
}

void CBaseEntity::SetBrushModel ()
{
	if (!gameEntity->model || gameEntity->model[0] != '*')
	{
		DebugPrintf ("CleanCode warning: SetBrushModel on a non-brush model!\n");
		State.SetModelIndex (ModelIndex(gameEntity->model));
		return;
	}

_CC_DISABLE_DEPRECATION
	gi.setmodel (gameEntity, gameEntity->model);
_CC_ENABLE_DEPRECATION
}

void CBaseEntity::CastTo (ECastFlags CastFlags)
{
	Cast (CastFlags, gameEntity);
}

void CBaseEntity::StuffText (char *text)
{
   	WriteByte (SVC_STUFFTEXT);	        
	WriteString (text);
    CastTo (CASTFLAG_RELIABLE);	
}

const CEntityField CMapEntity::FieldsForParsing[] =
{
	CEntityField ("spawnflags",		EntityMemberOffset(CBaseEntity,SpawnFlags),		FT_UINT),
	CEntityField ("origin",			GameEntityMemberOffset(state.origin),			FT_VECTOR | FT_GAME_ENTITY),
	CEntityField ("angles",			GameEntityMemberOffset(state.angles),			FT_VECTOR | FT_GAME_ENTITY),
	CEntityField ("angle",			GameEntityMemberOffset(state.angles),			FT_YAWANGLE | FT_GAME_ENTITY),
	CEntityField ("model",			GameEntityMemberOffset(model),					FT_LEVEL_STRING | FT_GAME_ENTITY),
	CEntityField ("light",			0,												FT_IGNORE),

	CEntityField ("item",			GameEntityMemberOffset(item),					FT_ITEM | FT_GAME_ENTITY),
	CEntityField ("target",			GameEntityMemberOffset(target),					FT_LEVEL_STRING | FT_GAME_ENTITY),
	CEntityField ("targetname",		GameEntityMemberOffset(targetname),				FT_LEVEL_STRING | FT_GAME_ENTITY),
	CEntityField ("pathtarget",		GameEntityMemberOffset(pathtarget),				FT_LEVEL_STRING | FT_GAME_ENTITY),
	CEntityField ("deathtarget",	GameEntityMemberOffset(deathtarget),			FT_LEVEL_STRING | FT_GAME_ENTITY),
	CEntityField ("killtarget",		GameEntityMemberOffset(killtarget),				FT_LEVEL_STRING | FT_GAME_ENTITY),
	CEntityField ("combattarget",	GameEntityMemberOffset(combattarget),			FT_LEVEL_STRING | FT_GAME_ENTITY),
	CEntityField ("team",			GameEntityMemberOffset(team),					FT_LEVEL_STRING | FT_GAME_ENTITY),
	CEntityField ("style",			GameEntityMemberOffset(style),					FT_INT | FT_GAME_ENTITY),
	CEntityField ("count",			GameEntityMemberOffset(count),					FT_INT | FT_GAME_ENTITY),
	CEntityField ("sounds",			GameEntityMemberOffset(sounds),					FT_INT | FT_GAME_ENTITY),

	CEntityField ("goalentity",		GameEntityMemberOffset(goalentity),				FT_ENTITY | FT_GAME_ENTITY | FT_NOSPAWN | FT_SAVABLE),
	CEntityField ("movetarget",		GameEntityMemberOffset(movetarget),				FT_ENTITY | FT_GAME_ENTITY | FT_NOSPAWN | FT_SAVABLE),
	CEntityField ("owner",			GameEntityMemberOffset(owner),					FT_ENTITY | FT_GAME_ENTITY | FT_NOSPAWN | FT_SAVABLE),

	// temp spawn vars -- only valid when the spawn function is called
	CEntityField ("lip",			SpawnTempMemberOffset(lip),						FT_INT | FT_SPAWNTEMP),
	CEntityField ("height",			SpawnTempMemberOffset(height),					FT_INT | FT_SPAWNTEMP),

	CEntityField ("gravity",		SpawnTempMemberOffset(gravity),					FT_LEVEL_STRING | FT_SPAWNTEMP),
	CEntityField ("sky",			SpawnTempMemberOffset(sky),						FT_LEVEL_STRING | FT_SPAWNTEMP),
	CEntityField ("skyrotate",		SpawnTempMemberOffset(skyrotate),				FT_FLOAT | FT_SPAWNTEMP),
	CEntityField ("skyaxis",		SpawnTempMemberOffset(skyaxis),					FT_VECTOR | FT_SPAWNTEMP),
	CEntityField ("nextmap",		SpawnTempMemberOffset(nextmap),					FT_LEVEL_STRING | FT_SPAWNTEMP),
};
const size_t CMapEntity::FieldsForParsingSize = FieldSize<CMapEntity>();

bool			CMapEntity::ParseField (char *Key, char *Value)
{
	if (CheckFields<CMapEntity, CBaseEntity> (this, Key, Value))
		return true;

	// Couldn't find it here
	return false;
};

bool				CMapEntity::CheckValidity ()
{
	// Yet another map hack
	if (!Q_stricmp(level.mapname, "command") && !Q_stricmp(gameEntity->classname, "trigger_once") && !Q_stricmp(gameEntity->model, "*27"))
		SpawnFlags &= ~SPAWNFLAG_NOT_HARD;

	// Remove things (except the world) from different skill levels or deathmatch
	if (this != World)
	{
		if (game.mode & GAME_DEATHMATCH)
		{
			if ( SpawnFlags & SPAWNFLAG_NOT_DEATHMATCH )
			{
				Free ();
				return false;
			}
		}
		else
		{
			if ( /* ((game.mode == GAME_COOPERATIVE) && (SpawnFlags & SPAWNFLAG_NOT_COOP)) || */
				((skill->Integer() == 0) && (SpawnFlags & SPAWNFLAG_NOT_EASY)) ||
				((skill->Integer() == 1) && (SpawnFlags & SPAWNFLAG_NOT_MEDIUM)) ||
				(((skill->Integer() == 2) || (skill->Integer() == 3)) && (SpawnFlags & SPAWNFLAG_NOT_HARD))
				)
				{
					Free ();
					return false;
				}
		}

		SpawnFlags &= ~(SPAWNFLAG_NOT_EASY|SPAWNFLAG_NOT_MEDIUM|SPAWNFLAG_NOT_HARD|SPAWNFLAG_NOT_COOP|SPAWNFLAG_NOT_DEATHMATCH);
	}
	return true;
};

void CMapEntity::ParseFields ()
{
	if (!gameEntity->ParseData || !gameEntity->ParseData->size())
		return;

	// Go through all the dictionary pairs
	std::list<CKeyValuePair*>::iterator it = gameEntity->ParseData->begin();
	while (it != gameEntity->ParseData->end())
	{
		CKeyValuePair *PairPtr = (*it);
		if (ParseField (PairPtr->Key, PairPtr->Value))
			gameEntity->ParseData->erase (it++);
		else
			++it;
	}

	// Since this is the last part, go through the rest of the list now
	// and report ones that are still there.
	if (gameEntity->ParseData->size())
	{
		for (std::list<CKeyValuePair*>::iterator it = gameEntity->ParseData->begin(); it != gameEntity->ParseData->end(); ++it)
		{
			CKeyValuePair *PairPtr = (*it);
			MapPrint (MAPPRINT_ERROR, this, State.GetOrigin(), "\"%s\" is not a field (value = \"%s\")\n", PairPtr->Key, PairPtr->Value);
		}
	}
	QDelete gameEntity->ParseData;
};