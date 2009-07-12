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
// cc_baseentity.h
// Base entity class
//

class CEntityState
{
private:
	entityStateOld_t	*state;

protected:
	void	SetNumber		(int value);
public:
	CEntityState			(entityStateOld_t *state);
	CEntityState			();

	int		GetNumber		();

	void	GetOrigin		(vec3_t in);
	vec3f	GetOrigin		();

	void	GetAngles		(vec3_t in);
	vec3f	GetAngles		();

	void	GetOldOrigin	(vec3_t in);
	vec3f	GetOldOrigin	();

	void	SetOrigin		(vec3_t in);
	void	SetOrigin		(vec3f in);

	void	SetAngles		(vec3_t in);
	void	SetAngles		(vec3f in);

	void	SetOldOrigin	(vec3_t in);
	void	SetOldOrigin	(vec3f in);

	// Can be 1, 2, 3, or 4
	int		GetModelIndex	(uint8 index = 1);
	void	SetModelIndex	(MediaIndex value, uint8 index = 1);

	int		GetFrame		();
	void	SetFrame		(int value);

	int		GetSkinNum		();
	void	SetSkinNum		(int value);

	uint32	GetEffects		();
	void	SetEffects		(uint32 value);
	void	AddEffects		(uint32 value);
	void	RemoveEffects	(uint32 value);

	int		GetRenderEffects	();
	void	SetRenderEffects	(int value);
	void	AddRenderEffects	(int value);
	void	RemoveRenderEffects	(int value);

	MediaIndex	GetSound		();
	void		SetSound		(MediaIndex value);

	int		GetEvent			();
	void	SetEvent			(int value);
};

// CBaseEntity is abstract.
// A base entity can't do anything
class CBaseEntity abstract
{
//private:
public: // Kept public for now because there are lots of functions that require edict_t
	edict_t			*gameEntity;	// The "game entity" this is linked with.
									// Kept private to make sure no mistakes are made.
public:
	bool			Freed;
	int32			EntityFlags;
	CEntityState	State;

	CBaseEntity ();
	CBaseEntity (int Index);
	virtual ~CBaseEntity ();

	virtual		bool		Run () {return true;}; // Runs the entity

	// Funtions below are to link the private gameEntity together
	CBaseEntity		*GetOwner	();
	void			SetOwner	(CBaseEntity *ent);

	int				GetClipmask	();
	void			SetClipmask (int mask);

	solid_t			GetSolid ();
	void			SetSolid (solid_t solid);

	// Vectors are a little weird.
	// Since we can't really "return" an array we have to pass it
	void			GetMins (vec3_t in);
	void			GetMaxs (vec3_t in);

	void			GetAbsMin (vec3_t in);
	void			GetAbsMax (vec3_t in);
	void			GetSize (vec3_t in);

	// Unless, of course, you use the vec3f class :D
	vec3f			GetMins ();
	vec3f			GetMaxs ();

	vec3f			GetAbsMin ();
	vec3f			GetAbsMax ();
	vec3f			GetSize ();

	// Same for setting
	void			SetMins (vec3_t in);
	void			SetMaxs (vec3_t in);

	void			SetAbsMin (vec3_t in);
	void			SetAbsMax (vec3_t in);
	void			SetSize (vec3_t in);

	// Vec3f
	void			SetMins (vec3f in);
	void			SetMaxs (vec3f in);

	void			SetAbsMin (vec3f in);
	void			SetAbsMax (vec3f in);
	void			SetSize (vec3f in);

	int				GetSvFlags ();
	void			SetSvFlags (int SVFlags);

	int				GetNumClusters ();

	int				*GetClusterNums ();

	int				GetHeadNode ();

	int				GetAreaNum (bool second = false);
	void			SetAreaNum (int num, bool second = false); // FIXME: needed?

	link_t			*GetArea ();
	void			ClearArea ();

	int				GetLinkCount ();

	bool			IsInUse ();
	void			SetInUse (bool inuse);

	void			Link ();
	void			Unlink ();

	void			Free ();

	// Sound functions
	void			PlaySound (EEntSndChannel channel, MediaIndex soundIndex, float volume = 1.0f, int attenuation = ATTN_NORM, float timeOfs = 0.0f);
	void			PlayPositionedSound (vec3_t origin, EEntSndChannel channel, MediaIndex soundIndex, float volume = 1.0f, int attenuation = ATTN_NORM, float timeOfs = 0.0f);
};

// EntityFlags values
enum
{
	ENT_BASE		=	1, // Can be casted to CBaseEntity
	ENT_HURTABLE	=	2, // Can be casted to CHurtableEntity
	ENT_THINKABLE	=	4, // Can be casted to CThinkableEntity
	ENT_TOUCHABLE	=	8, // Can be casted to CTouchableEntity
	ENT_PLAYER		=	16, // Can be casted to CPlayerEntity
};

// Base classes
#include "cc_entitytypes.h"

// Derivitives
#include "cc_playerentity.h"
#include "cc_weaponentities.h"

class CWorldEntity : public CBaseEntity
{
public:
	CWorldEntity ();
	CWorldEntity (int Index);
};