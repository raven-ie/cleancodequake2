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

#include <cctype>
#include <algorithm>

IBaseEntity *GetGameEntity (sint32 Index)
{
	return Game.Entities[Index].Entity;
}

CEntityField::CEntityField (const char *Name, size_t Offset, EFieldType FieldType) :
Name(Q_strlwr(Name)),
Offset(Offset),
FieldType(FieldType),
StrippedFields(FieldType & ~(FT_GAME_ENTITY | FT_SAVABLE | FT_NOSPAWN))
{
};

CEntityState::CEntityState () :
state(NULL)
{
};

CEntityState::CEntityState (entityState_t *state) :
state(state)
{
};

void		CEntityState::Initialize (entityState_t *state)
{
	this->state = state;
};

sint32		&CEntityState::GetNumber		()
{
	return state->number;
}

vec3f	&CEntityState::GetOrigin		()
{
	return state->origin;
}

vec3f	&CEntityState::GetAngles		()
{
	return state->angles;
}

vec3f	&CEntityState::GetOldOrigin	()
{
	return state->oldOrigin;
}

// Can be 1, 2, 3, or 4
sint32		&CEntityState::GetModelIndex	(uint8 index)
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
		CC_ASSERT_EXPR(0, "index for GetModelIndex is out of bounds");
		return state->modelIndex;
	}
}

sint32		&CEntityState::GetFrame		()
{
	return state->frame;
}

sint32	&CEntityState::GetSkinNum		()
{
	return state->skinNum;
}

EEntityStateEffects	&CEntityState::GetEffects		()
{
	return state->effects;
}

EEntityStateRenderEffects		&CEntityState::GetRenderEffects	()
{
	return state->renderFx;
}

MediaIndex	&CEntityState::GetSound		()
{
	return (MediaIndex&)state->sound;
}

EEventEffect	&CEntityState::GetEvent			()
{
	return state->event;
}

void G_InitEdict (edict_t *e)
{
	e->server.inUse = true;
	memset (&e->server.state, 0, sizeof(e->server.state));
	e->server.state.number = e - Game.Entities;
}

template <typename TCont, typename TType>
void listfill (TCont &List, TType Data, size_t numElements)
{
	List.clear ();

	for (size_t i = 0; i < numElements; i++)
		List.push_back (&Data[i]);
}

void InitEntityLists ()
{
	listfill <TEntitiesContainer, edict_t*> (Level.Entities.Open, Game.Entities, Game.MaxEntities);
	Level.Entities.Closed.clear();

	// Keep the first few entities in the closed list
	for (uint8 i = 0; i < (1 + Game.MaxClients); i++)
	{
		Level.Entities.Closed.push_back (Level.Entities.Open.front());
		Level.Entities.Open.pop_front();
	}
}

// Removes a free entity from Open, pushes into Closed.
edict_t *GetEntityFromList ()
{
	if (Level.Entities.Open.empty())
		return NULL;

	// Take entity off of list, obeying freetime
	edict_t *ent = NULL;

	TEntitiesContainer::iterator it;
	for (it = Level.Entities.Open.begin(); it != Level.Entities.Open.end(); ++it)
	{
		edict_t *check = (*it);

		// the first couple seconds of server time can involve a lot of
		// freeing and allocating, so relax the replacement policy
		if (!check->server.inUse && (check->freetime < 20 || Level.Frame - check->freetime > 5))
		{
			ent = check;
			break;
		}
	}

	if (ent == NULL)
		return NULL;

	Level.Entities.Open.erase (it);
	
	// Put into closed
	Level.Entities.Closed.push_back (ent);
	return ent; // Give it to us
}

// Removes from Open, puts into end of Closed.
void RemoveEntityFromOpen (edict_t *ent)
{
	Level.Entities.Open.remove (ent);
	Level.Entities.Closed.push_back (ent);
}

// Removes entity from Closed, pushes into front of into Open
void RemoveEntityFromList (edict_t *ent)
{
	// Take entity out of list
	Level.Entities.Closed.remove (ent);

	// Push into Open
	Level.Entities.Open.push_front (ent);
}

#include "cc_body_queue.h"

bool RemoveEntity (edict_t *ent)
{
	if (!ent || ent->server.state.number <= (Game.MaxClients + BODY_QUEUE_SIZE))
		return false;

	if (!ent->Entity || ent->AwaitingRemoval)
	{
		if (!ent->RemovalFrames)
		{
			ent->AwaitingRemoval = false;

			QDelete ent->Entity;
			ent->Entity = NULL;

			// Push into Open
			Level.Entities.Open.push_front (ent);
			return true;
		}

		ent->RemovalFrames--;
	}
	else if (ent->Entity->GroundEntity && ent->Entity->GroundEntity->Freed)
		ent->Entity->GroundEntity = NULL;

	return false;
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
edict_t *G_Spawn ()
{
	edict_t *e = GetEntityFromList ();
	
	if (e == NULL)
	{
		GameError ("ED_Alloc: no free edicts");
		return NULL;
	}

CC_DISABLE_DEPRECATION
	G_InitEdict (e);
CC_ENABLE_DEPRECATION

	if (GameAPI.GetNumEdicts() < e->server.state.number + 1)
		GameAPI.GetNumEdicts() = e->server.state.number + 1;

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
CC_DISABLE_DEPRECATION
	gi.unlinkentity (ed);		// unlink from world
CC_ENABLE_DEPRECATION

	// Paril, hack
	IBaseEntity *Entity = ed->Entity;

	Mem_Zero (ed, sizeof(*ed));
	if (Entity)
		ed->Entity = Entity;
	ed->freetime = Level.Frame;
	ed->server.inUse = false;
	ed->server.state.number = ed - Game.Entities;

	if (!ed->AwaitingRemoval)
	{
		ed->AwaitingRemoval = true;
		ed->RemovalFrames = 2;
	}
}

typedef std::vector <IBaseEntity*> TPrivateEntitiesContainer;
TPrivateEntitiesContainer PrivateEntities;

void InitPrivateEntities ()
{
	PrivateEntities.clear ();
}

void			RunPrivateEntities ()
{
	TPrivateEntitiesContainer::iterator it = PrivateEntities.begin();
	while (it != PrivateEntities.end())
	{
		IBaseEntity *Entity = (*it);
		
		Level.CurrentEntity = Entity;

		IThinkableEntity *Thinkable = (!Entity->Freed && (Entity->EntityFlags & ENT_THINKABLE)) ? entity_cast<IThinkableEntity>(Entity) : NULL;

		if (Thinkable) 
			Thinkable->PreThink ();

		Entity->Run ();	

		if (Thinkable)
			Thinkable->RunThink ();

		// Were we freed?
		// This has to be processed after thinking and running, because
		// the entity still has to be intact after that
		if (Entity->Freed)
		{
			it = PrivateEntities.erase (it);
			QDelete Entity;
		}
		else
			++it;
	}
};

// Creating a new entity via constructor.
IBaseEntity::IBaseEntity ()
{
CC_DISABLE_DEPRECATION
	gameEntity = G_Spawn ();
CC_ENABLE_DEPRECATION
	gameEntity->Entity = this;
	ClassName = "noclass";

	Freed = false;
	EntityFlags |= ENT_BASE;

	State.Initialize (&gameEntity->server.state);
};

IBaseEntity::IBaseEntity (sint32 Index)
{
	if (Index < 0)
	{
		Freed = false;
		EntityFlags |= (ENT_PRIVATE|ENT_BASE);

		gameEntity = NULL;
		PrivateEntities.push_back (this);
	}
	else
	{
		gameEntity = &Game.Entities[Index];
		gameEntity->Entity = this;
		gameEntity->server.state.number = Index;

		Freed = false;
		EntityFlags |= ENT_BASE;
		State.Initialize (&gameEntity->server.state);
	}
}

bool ShuttingDownEntities = false;
IBaseEntity::~IBaseEntity ()
{
	if (!ShuttingDownEntities)
	{
		if (gameEntity)
		{
			gameEntity->Entity = NULL;

	CC_DISABLE_DEPRECATION
			if (!Freed && !(EntityFlags & ENT_JUNK))
				G_FreeEdict (gameEntity); // "delete" the entity
	CC_ENABLE_DEPRECATION
		}
		else
		{
			for (TPrivateEntitiesContainer::iterator it = PrivateEntities.begin(); it < PrivateEntities.end(); ++it)
			{
				if ((*it) == this)
				{
					PrivateEntities.erase (it);
					break;
				}
			}
		}
	}
};

void IBaseEntity::WriteBaseEntity (CFile &File)
{
	File.Write<bool> (Freed);
	File.Write<EEntityFlags> (EntityFlags);
	File.Write<EEdictFlags> (Flags);

	File.Write (ClassName);

	File.Write<bool> (Team.HasTeam);

	if (Team.HasTeam)
	{
		File.Write<sint32> ((Team.Chain && Team.Chain->gameEntity) ? Team.Chain->State.GetNumber() : -1);
		File.Write<sint32> ((Team.Master && Team.Master->gameEntity) ? Team.Master->State.GetNumber() : -1);
	}

	File.Write<sint32> ((GroundEntity) ? GroundEntity->State.GetNumber() : -1);

	File.Write<sint32> (GroundEntityLinkCount);
	File.Write<uint32> (SpawnFlags);

	File.Write<sint32> ((Enemy && Enemy->gameEntity) ? Enemy->State.GetNumber() : -1);

	File.Write<sint32> (ViewHeight);
}

void IBaseEntity::ReadBaseEntity (CFile &File)
{
	Freed = File.Read<bool> ();
	EntityFlags = File.Read<uint32> ();
	Flags = File.Read<EEdictFlags> ();

	ClassName = File.ReadCCString ();

	Team.HasTeam = File.Read<bool> ();

	if (Team.HasTeam)
	{
		sint32 ChainNumber = File.Read<sint32> ();
		sint32 MasterNumber = File.Read<sint32> ();

		if (ChainNumber != -1)
			Team.Chain = Game.Entities[ChainNumber].Entity;
		if (MasterNumber != -1)
			Team.Master = Game.Entities[MasterNumber].Entity;
	}

	sint32 GroundEntityNumber = File.Read<sint32> ();
	GroundEntity = (GroundEntityNumber == -1) ? NULL : Game.Entities[GroundEntityNumber].Entity;

	GroundEntityLinkCount = File.Read<sint32> ();
	SpawnFlags = File.Read<uint32> ();

	sint32 EnemyNumber = File.Read<sint32> ();
	Enemy = (EnemyNumber == -1) ? NULL : Game.Entities[EnemyNumber].Entity;

	ViewHeight = File.Read<sint32> ();
}

// Funtions below are to link the private gameEntity together
IBaseEntity		*IBaseEntity::GetOwner	()
{
	return (gameEntity->server.owner) ? gameEntity->server.owner->Entity : NULL;
}
void			IBaseEntity::SetOwner	(IBaseEntity *Entity)
{
	if (!Entity || !Entity->gameEntity)
	{
		gameEntity->server.owner = NULL;
		return;
	}

	gameEntity->server.owner = Entity->gameEntity;
}

EBrushContents	&IBaseEntity::GetClipmask	()
{
	return gameEntity->server.clipMask;
}

ESolidType		&IBaseEntity::GetSolid ()
{
	return gameEntity->server.solid;
}

// Unless, of course, you use the vec3f class :D
vec3f			&IBaseEntity::GetMins ()
{
	return gameEntity->server.mins;
}
vec3f			&IBaseEntity::GetMaxs ()
{

	return gameEntity->server.maxs;
}

vec3f			&IBaseEntity::GetAbsMin ()
{
	return gameEntity->server.absMin;
}
vec3f			&IBaseEntity::GetAbsMax ()
{
	return gameEntity->server.absMax;
}
vec3f			&IBaseEntity::GetSize ()
{
	return gameEntity->server.size;
}

EServerFlags	&IBaseEntity::GetSvFlags ()
{
	return gameEntity->server.svFlags;
}

sint32				IBaseEntity::GetAreaNum (bool second)
{
	return ((second) ? gameEntity->server.areaNum2 : gameEntity->server.areaNum);
}

link_t			*IBaseEntity::GetArea ()
{
	return &gameEntity->server.area;
}
void			IBaseEntity::ClearArea ()
{
	Mem_Zero (&gameEntity->server.area, sizeof(gameEntity->server.area));
}

sint32				IBaseEntity::GetLinkCount ()
{
	return gameEntity->server.linkCount;
}

bool			&IBaseEntity::GetInUse ()
{
	return (bool&)gameEntity->server.inUse;
}

CC_DISABLE_DEPRECATION
void			IBaseEntity::Link ()
{
	gi.linkentity (gameEntity);
}

void			IBaseEntity::Unlink ()
{
	gi.unlinkentity (gameEntity);
}
CC_ENABLE_DEPRECATION

void			IBaseEntity::Free ()
{
	if (gameEntity)
	{
		Unlink ();

		Mem_Zero (gameEntity, sizeof(*gameEntity));
		gameEntity->Entity = this;
		gameEntity->freetime = Level.Frame;
		GetInUse() = false;
		gameEntity->server.state.number = gameEntity - Game.Entities;

		if (!(EntityFlags & ENT_JUNK))
		{
			if (Level.Frame == 0)
				RemoveEntityFromList (gameEntity);
			else if (!gameEntity->AwaitingRemoval)
			{
				gameEntity->AwaitingRemoval = true;
				gameEntity->RemovalFrames = 2;
			}
		}
	}

	Freed = true;
}

void	IBaseEntity::PlaySound (ESoundChannel channel, MediaIndex soundIndex, uint8 volume, EAttenuation attenuation, uint8 timeOfs)
{
	if ((channel != CHAN_AUTO) && (channel < CHAN_MAX))
	{
		if (PlayedSounds[channel-1])
			return;
		else
			PlayedSounds[channel-1] = true;
	}

	PlaySoundFrom (this, channel, soundIndex, volume, attenuation, timeOfs);
};

void	IBaseEntity::PlayPositionedSound (vec3f origin, ESoundChannel channel, MediaIndex soundIndex, uint8 volume, EAttenuation attenuation, uint8 timeOfs)
{
	if ((channel != CHAN_AUTO) && (channel < CHAN_MAX))
	{
		if (PlayedSounds[channel-1])
			return;
		else
			PlayedSounds[channel-1] = true;
	}

	PlaySoundAt (origin, this, channel, soundIndex, volume, attenuation, timeOfs);
};

void	IBaseEntity::KillBox ()
{
	while (1)
	{
		CTrace tr (State.GetOrigin(), GetMins(), GetMaxs(), State.GetOrigin(), NULL, CONTENTS_MASK_PLAYERSOLID);
		if (!tr.ent || !tr.Ent)
			break;

		if ((tr.Ent->EntityFlags & ENT_HURTABLE) && entity_cast<IHurtableEntity>(tr.Ent)->CanTakeDamage)
		{
			// nail it
			entity_cast<IHurtableEntity>(tr.Ent)->TakeDamage (this, this, vec3fOrigin, State.GetOrigin(),
																vec3fOrigin, 100000, 0, DAMAGE_NO_PROTECTION, MOD_TELEFRAG);
		}

		if (tr.Ent->GetSolid())
			break;
	}
};

void IBaseEntity::SplashDamage (IBaseEntity *Attacker, float damage, IBaseEntity *ignore, float radius, EMeansOfDeath mod)
{
	IHurtableEntity	*Entity = NULL;

	while ((Entity = FindRadius<IHurtableEntity, ENT_HURTABLE> (Entity, State.GetOrigin(), radius)) != NULL)
	{
		if (Entity == ignore)
			continue;
		if (!Entity->CanTakeDamage)
			continue;

		float points = damage - 0.5 * (State.GetOrigin() - Entity->State.GetOrigin().MultiplyAngles (0.5f, Entity->GetMins() + Entity->GetMaxs())).Length();

		if (Entity == Attacker)
			points *= 0.5;
		if ((points > 0) && Entity->CanDamage (this))
			Entity->TakeDamage (this, Attacker, Entity->State.GetOrigin() - State.GetOrigin(), State.GetOrigin(), vec3fOrigin, (sint32)points, (sint32)points, DAMAGE_RADIUS, mod);
	}
}

#if ROGUE_FEATURES

/*
============
NukeSplashDamage

Like SplashDamage, but ignores walls (skips CanDamage check, among others)
// up to KILLZONE radius, do 10,000 points
// after that, do damage linearly out to KILLZONE2 radius
============
*/
class CNukePlayerCallBack : public CForEachPlayerCallback
{
public:
	IBaseEntity *Inflictor;

	CNukePlayerCallBack (IBaseEntity *Inflictor) :
	Inflictor(Inflictor)
	{
	};

	void Callback (CPlayerEntity *Player)
	{
		if (Player->Client.Timers.Nuke != Level.Frame + 20)
		{
			CTrace tr (Inflictor->State.GetOrigin(), Player->State.GetOrigin(), Inflictor, CONTENTS_MASK_SOLID);

			if (tr.fraction == 1.0)
				Player->Client.Timers.Nuke = Level.Frame + 20;
			else
				Player->Client.Timers.Nuke = Max<>(Player->Client.Timers.Nuke, Level.Frame + (RangeFrom (Player->State.GetOrigin(), Inflictor->State.GetOrigin()) < 2048) ? 15 : 10);
		}
	}
};

void IBaseEntity::NukeSplashDamage (IBaseEntity *Attacker, float damage, IBaseEntity *ignore, float radius, EMeansOfDeath mod)
{
	float	points;
	IHurtableEntity	*Entity = NULL;
	vec3f	v;
	vec3f	dir;
	float	len;

	float killzone = radius;
	float killzone2 = radius*2.0;

	while ((Entity = FindRadius<IHurtableEntity, ENT_HURTABLE> (Entity, State.GetOrigin(), radius)) != NULL)
	{
// ignore nobody
		if (Entity == ignore)
			continue;
		if (!Entity->CanTakeDamage)
			continue;
		if (!Entity->GetInUse())
			continue;

		v = (Entity->GetMins() + Entity->GetMaxs());
		v = Entity->State.GetOrigin().MultiplyAngles (0.5f, v);
		v = State.GetOrigin() - v;

		len = v.Length();

		if (len <= killzone)
		{
			if (Entity->EntityFlags & ENT_PLAYER)
				Entity->Flags |= FL_NOGIB;
			points = 10000;
		}
		else if (len <= killzone2)
			points = (damage/killzone)*(killzone2 - len);
		else
			points = 0;

		if (points > 0)
		{
			if (Entity->EntityFlags & ENT_PLAYER)
				(entity_cast<CPlayerEntity>(Entity))->Client.Timers.Nuke = Level.Frame + 20;
			dir = Entity->State.GetOrigin() - State.GetOrigin();
			Entity->TakeDamage (this, Attacker, dir, State.GetOrigin(), vec3fOrigin, (int)points, (int)points, DAMAGE_RADIUS, mod);
		}
	}

	CNukePlayerCallBack (this).Query ();
}
#endif

IMapEntity::IMapEntity () : 
IBaseEntity()
{
	EntityFlags |= ENT_MAP;
};

IMapEntity::IMapEntity (sint32 Index) : 
IBaseEntity(Index)
{
	EntityFlags |= ENT_MAP;
};

IMapEntity::~IMapEntity ()
{
};

#include "cc_temporary_entities.h"

void IBaseEntity::BecomeExplosion (bool grenade)
{
	if (grenade)
		CGrenadeExplosion (State.GetOrigin()).Send();
	else
		CRocketExplosion (State.GetOrigin()).Send();
	Free ();
}

void IBaseEntity::CastTo (ECastFlags CastFlags)
{
	Cast (CastFlags, this);
}

void IBaseEntity::StuffText (const char *text)
{
   	WriteByte (SVC_STUFFTEXT);	        
	WriteString (text);
    CastTo (CASTFLAG_RELIABLE);	
}

ENTITYFIELDS_BEGIN(IMapEntity)
{
	CEntityField ("spawnflags",		EntityMemberOffset(IBaseEntity,SpawnFlags),		FT_UINT | FT_SAVABLE),
	CEntityField ("origin",			GameEntityMemberOffset(server.state.origin),			FT_VECTOR | FT_GAME_ENTITY),
	CEntityField ("angles",			GameEntityMemberOffset(server.state.angles),			FT_VECTOR | FT_GAME_ENTITY),
	CEntityField ("angle",			GameEntityMemberOffset(server.state.angles),			FT_YAWANGLE | FT_GAME_ENTITY),
	CEntityField ("light",			0,												FT_IGNORE),
	CEntityField ("team",			EntityMemberOffset(IBaseEntity,Team.String),	FT_LEVEL_STRING),
};
ENTITYFIELDS_END(IMapEntity)

const CEntityField IMapEntity::FieldsForParsing_Map[] =
{
	CEntityField ("targetname",		EntityMemberOffset(IMapEntity,TargetName),		FT_LEVEL_STRING | FT_SAVABLE),
};
const size_t IMapEntity::FieldsForParsingSize_Map = ArrayCount(IMapEntity::FieldsForParsing_Map);

bool			IMapEntity::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<IMapEntity, IBaseEntity> (this, Key, Value))
		return true;
	else
	{
		for (size_t i = 0; i < IMapEntity::FieldsForParsingSize_Map; i++)
		{
			if (!(IMapEntity::FieldsForParsing_Map[i].FieldType & FT_NOSPAWN) && (strcmp (Key, IMapEntity::FieldsForParsing_Map[i].Name.c_str()) == 0))
			{
				IMapEntity::FieldsForParsing_Map[i].Create<IMapEntity> (this, Value);
				return true;
			}
		}
	}

	// Couldn't find it here
	return false;
};

void IMapEntity::SaveFields (CFile &File)
{
	for (size_t i = 0; i < IMapEntity::FieldsForParsingSize_Map; i++)
	{
#if (MSVS_VERSION >= VS_9)
#pragma warning (suppress : 6385 6386)
#endif
		if (IMapEntity::FieldsForParsing_Map[i].FieldType & FT_SAVABLE)
		{
#if (MSVS_VERSION >= VS_9)
#pragma warning (suppress : 6385 6386)
#endif
			IMapEntity::FieldsForParsing_Map[i].Save<IMapEntity> (this, File);
		}
	}
};

void IMapEntity::LoadFields (CFile &File)
{
	for (size_t i = 0; i < IMapEntity::FieldsForParsingSize_Map; i++)
	{
#if (MSVS_VERSION >= VS_9)
#pragma warning (suppress : 6385 6386)
#endif
		if (IMapEntity::FieldsForParsing_Map[i].FieldType & FT_SAVABLE)
		{
#if (MSVS_VERSION >= VS_9)
#pragma warning (suppress : 6385 6386)
#endif
			IMapEntity::FieldsForParsing_Map[i].Load<IMapEntity> (this, File);
		}
	}
};

bool				IMapEntity::CheckValidity ()
{
	// Remove things (except the world) from different skill levels or deathmatch
	if (this != World)
	{
		if (!CvarList[CV_MAP_DEBUG].Boolean())
		{
			if (Game.GameMode & GAME_DEATHMATCH)
			{
				if ( SpawnFlags & SPAWNFLAG_NOT_DEATHMATCH )
				{
					Free ();
					return false;
				}
			}
			else
			{
				if ( /* ((Game.GameMode & GAME_COOPERATIVE) && (SpawnFlags & SPAWNFLAG_NOT_COOP)) || */
					((CvarList[CV_SKILL].Integer() == 0) && (SpawnFlags & SPAWNFLAG_NOT_EASY)) ||
					((CvarList[CV_SKILL].Integer() == 1) && (SpawnFlags & SPAWNFLAG_NOT_MEDIUM)) ||
					((CvarList[CV_SKILL].Integer() >= 2) && (SpawnFlags & SPAWNFLAG_NOT_HARD))
					)
					{
						Free ();
						return false;
					}
			}

			SpawnFlags &= ~(SPAWNFLAG_NOT_EASY|SPAWNFLAG_NOT_MEDIUM|SPAWNFLAG_NOT_HARD|SPAWNFLAG_NOT_COOP|SPAWNFLAG_NOT_DEATHMATCH);
		}
	}
	return true;
};

void IMapEntity::ParseFields ()
{
	if (!Level.ParseData.size())
		return;

	// Go through all the dictionary pairs
	TKeyValuePairContainer::iterator it = Level.ParseData.begin();
	while (it != Level.ParseData.end())
	{
		CKeyValuePair *PairPtr = (*it);
		if (ParseField (PairPtr->Key, PairPtr->Value))
			Level.ParseData.erase (it++);
		else
			++it;
	}

	// Since this is the last part, go through the rest of the list now
	// and report ones that are still there.
	if (Level.ParseData.size())
	{
		for (TKeyValuePairContainer::iterator it = Level.ParseData.begin(); it != Level.ParseData.end(); ++it)
		{
			CKeyValuePair *PairPtr = (*it);
			MapPrint (MAPPRINT_WARNING, this, State.GetOrigin(), "\"%s\" is not a field (value = \"%s\")\n", PairPtr->Key, PairPtr->Value);
		}
	}
};
