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
// cc_utils.h
// g_utils, CleanCode style
//

#if !defined(CC_GUARD_UTILS_H) || !INCLUDE_GUARDS
#define CC_GUARD_UTILS_H

void	G_TouchTriggers (CBaseEntity *Entity);
void	G_SetMovedir (vec3f &angles, vec3f &movedir);

typedef std::vector<CBaseEntity*, std::allocator<CBaseEntity*> > TTargetList;
CBaseEntity *CC_PickTarget (char *targetname);
TTargetList CC_GetTargets (char *targetname);

template <class TEntityType, uint32 EntityFlags, size_t FieldOfs>
TEntityType *CC_Find (CBaseEntity *From, const char *Match)
{
	edict_t *gameEnt;
	if (!From)
		gameEnt = Game.Entities;
	else
	{
		gameEnt = From->gameEntity;
		gameEnt++;
	}

	for ( ; gameEnt < &Game.Entities[GameAPI.GetNumEdicts()]; gameEnt++)
	{
		if (!gameEnt->inUse)
			continue;
		if (!gameEnt->Entity)
			continue;
		if (!(gameEnt->Entity->EntityFlags & EntityFlags))
			continue;

		// Special case
		// If we're only looking for ENT_BASE
		// we don't need to cast just yet
		if (EntityFlags & ENT_BASE)
		{
			char *s = *(char **) ((uint8 *)gameEnt->Entity + FieldOfs);
			if (!s)
				continue;
			if (!Q_stricmp (s, Match))
				return entity_cast<TEntityType>(gameEnt->Entity);
		}
		else
		{
			TEntityType *Check = entity_cast<TEntityType>(gameEnt->Entity);
			char *s = *(char **) ((uint8 *)Check + FieldOfs);
			if (!s)
				continue;
			if (!Q_stricmp (s, Match))
				return Check;
		}
	}

	return NULL;
}

template <class TEntityType, uint32 EntityFlags>
TEntityType *CC_FindByClassName (CBaseEntity *From, const char *Match)
{
	edict_t *gameEnt;
	if (!From)
		gameEnt = Game.Entities;
	else
	{
		gameEnt = From->gameEntity;
		gameEnt++;
	}

	for ( ; gameEnt < &Game.Entities[GameAPI.GetNumEdicts()]; gameEnt++)
	{
		if (!gameEnt->inUse)
			continue;
		if (!gameEnt->Entity)
			continue;
		if (!(gameEnt->Entity->EntityFlags & EntityFlags))
			continue;

		if (gameEnt->Entity->ClassName.empty())
			continue;
		if (!Q_stricmp (gameEnt->Entity->ClassName.c_str(), Match))
			return entity_cast<TEntityType>(gameEnt->Entity);
	}

	return NULL;
}

// stored in cc_infoentities
CSpotBase *SelectFarthestDeathmatchSpawnPoint ();
CSpotBase *SelectRandomDeathmatchSpawnPoint ();
float	PlayersRangeFromSpot (CBaseEntity *spot);

inline CSpotBase *SelectDeathmatchSpawnPoint ()
{
	return (DeathmatchFlags.dfSpawnFarthest.IsEnabled()) ? SelectFarthestDeathmatchSpawnPoint () : SelectRandomDeathmatchSpawnPoint ();
}

class CForEachTeamChainCallback
{
public:
	virtual void Callback (CBaseEntity *Entity) = 0;
	virtual void Query (CBaseEntity *Master);
};

class CForEachPlayerCallback
{
public:
	sint32		Index;

	virtual void Callback (CPlayerEntity *Player) = 0;
	virtual void Query (bool MustBeInUse = true);
};

/*
class CMyPlayerCallback : public CForEachPlayerCallback
{
public:
	CMyPlayerCallback ()
	{
	};

	void Callback (CPlayerEntity *Player)
	{
	}
};
*/

inline float RangeFrom (vec3f left, vec3f right)
{
	return (left - right).Length();
}

inline ERangeType Range (vec3f left, vec3f right)
{
	float len = RangeFrom (left, right);
	if (len < MELEE_DISTANCE)
		return RANGE_MELEE;
	else if (len < 500)
		return RANGE_NEAR;
	else if (len < 1000)
		return RANGE_MID;
	return RANGE_FAR;
}

ERangeType Range (CBaseEntity *self, CBaseEntity *Other);
bool IsInFront (CBaseEntity *self, CBaseEntity *Other);
bool IsVisible (CBaseEntity *self, CBaseEntity *Other);

#else
FILE_WARNING
#endif
