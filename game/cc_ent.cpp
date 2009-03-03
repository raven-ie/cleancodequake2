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
// cc_ent.cpp
// Contains the code used for entities.
//

#include "cc_local.h"

/*

Okay, so all this is commented right now because this file is a brainstorming session.
Keep ALL code either commented or named specifically not to interfere with the rest of the code.
Because of the size of changes required to re-do the entity system, it will be done once
the rest of the systematics of CleanCode are completed as we will have less to re-write.

So this is the base entity structure, inherited by the rest of the entity classes:

typedef struct clientBase_s {
	playerState_t		playerState;
	int					ping;
} clientBase_t;

typedef struct edictBase_s {
	entityStateOld_t	s;
	clientBase_t	*client;

//==================================
	int					inUse;
	int					linkCount;

	link_t				area;
	
	int					numClusters;
	int					clusterNums[MAX_ENT_CLUSTERS];
	int					headNode;
	int					areaNum, areaNum2;
//==================================

	int					svFlags;
	vec3_t				mins, maxs;
	vec3_t				absMin, absMax, size;
	solid_t				solid;
	int					clipMask;
	struct edictBase_s	*owner;
} edictBase_t;


Most of the above information (everything inbetween ====) is private to the server, they
mess with it, not us.

Now.. edictBase_t NEVER changes under any circumstances. The array of edictBases is sent
to the server (instead of edict_s) and size is set in InitGame for only edictBase.
This will conserve a TON of space, as the server will only get the information IT needs, and the
rest stays private to us! All classes inherit this base class

Okay, so now that edictBase is covered.. we move on to classes.
The first class we will create is CBaseEntity.
I know it seems weird that we create two base entities, but I like
structures to be POC (plain old code). CBaseEntity will inherit edictBase_s and contain
any game variables that ALL entities will share (won't be many), and functions
to wrap gameImport functions such as linkEntity and unlinkEntity (keep track of linkage to make sure
we don't link when we don't need to, to save a stack call).

CBaseEntity is spread off on it's own, and can be used on it's own for simple matters (such as a static model).
CBaseEntity will have a Run and a PreRun function which is called every frame and before the frame starts, respectively, which on it's own is enough to run
simple thinking and physics. It also has a Touch function, if it requires to be touched. In addition, it has a Hit function, which isn't used by CBaseEntity itself.
It is virtual, and is used by other entities for if the entity is traced and 'hit' by something.

CPlayer inherits CBaseEntity. Hopefully I don't need to explain what CPlayer is.
CPlayer will contain functions related to the player, such as unicasting sounds/stuffcmd and muzzleflashes.
Players don't "think", per say, which I suppose is a good thing as it lessens the work load;
as long as it has a client structure the client is run by itself. The only thing players need are a prethink
and a think (ClientBeginServerFrame & ClientEndServerFrame).

CMonster inherits CBaseEntity. CMonster is.. a monster.
CMonster contains functions such as the multicast for monster muzzleflashes, and it's Think runs a function
similar to monster_think. The big difference between monsters and players is that monsters use their Touch function,
and have a CMonsterInfo class. The CMonsterInfo class contains all monster-related functions, which are virtual, as monsters
will be a new instance of the class and will use the virtual function to change the way it behaves. The likes of Die, Pain, Duck,
Run, Stand and Walk will be virtual. In keeping with the spirit that monsters should be able to actually do things, the
CMonsterInfo class will contain a basic animation class (above it, used in the class itself though) which, like Quake2, will be set
to CurrentMove and such.

Now that we've covered that, let's move on to the smaller entities.

CProjectile inherits CBaseEntity.
It's a bigger class than CPlayer, amazingly, as it contains everything a projectile could imagine.
CProjectile is meant to be inherited; the virtual functions are changed so that they do the effect that you
wish to achieve with the projectile and to store whatever variables you need.

CPush inherits CBaseEntity.
CPush is meant to be inherited. It's used by things such as brush models, explo boxes, anything that
isn't static and that requires to be moved. They will have damage 'support' via the Hit function.

In addition to all of these special server entities, we have one last entity type.
CPrivate. It inherits CBaseClass.
The difference is CPrivate uses a special constructor which does NOT change the global edictBase list.
CPrivate is a game-private entity. It can do everything a real entity can; however, it can not be visible.
No matter the circumstance, CPrivate is private to the game DLL. It cannot be linked, unlinked, and it cannot be
sent to the server. The upside to this is we can use this for things such as speakers (non-looped); since gi.positioned_sound
exists, we don't need to use an extra server-sent entity just to spawn a sound. The downside is we can't use this for anything
that could actually effect other playing entities. This entity, no matter the solid, cannot be traced and cannot be hit-detected.
It is essentially invisible, and exists only to save the game a couple of entity spaces for things that shouldn't even be entities
in the first place.

That's pretty much it for the entity system.
Damage will be handled by the Hit function, which would call, for example, T_Damage or otherwise.

*/