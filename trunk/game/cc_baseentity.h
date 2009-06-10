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

// CBaseEntity is abstract.
// A base entity can't do anything
class CBaseEntity abstract
{
//private:
public:
	edict_t		*gameEntity;	// The "game entity" this is linked with.
								// Kept private to make sure no mistakes are made.

	friend class	CPlayerEntity; // We are friends!
public:
	uint32			EntityFlags;

	CBaseEntity ();
	CBaseEntity (int Index);
	virtual ~CBaseEntity ();

	virtual		bool		Run () = 0; // Runs the entity

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
};

#include "cc_playerentity.h"

// EntityFlags values
enum
{
	ENT_BASE		=	1, // Can be casted to CBaseEntity
	ENT_PLAYER		=	2, // Can be casted to CPlayerEntity
};