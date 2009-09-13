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
// cc_itementity.cpp
// The entity that lets you pick items up.
// Moved here because it's seperate from the item system.
//

#include "cc_local.h"

CItemEntity::CItemEntity () :
CBaseEntity(),
CMapEntity(),
CTossProjectile(),
CTouchableEntity(),
CThinkableEntity(),
CUsableEntity(),
NoPhysics(false)
{
	EntityFlags |= ENT_ITEM;
};

CItemEntity::CItemEntity (int Index) :
CBaseEntity(Index),
CMapEntity(Index),
CTossProjectile(Index),
CTouchableEntity(Index),
CThinkableEntity(Index),
CUsableEntity(Index),
NoPhysics(false)
{
	EntityFlags |= ENT_ITEM;
};

void CItemEntity::Spawn ()
{
	if (st.message)
		Message = Mem_PoolStrDup (st.message, com_levelPool, 0);
} // Just to fill CMapEntity

void CItemEntity::Touch(CBaseEntity *other, plane_t *plane, cmBspSurface_t *surf)
{
	if (!other)
		return;
	if (!(other->EntityFlags & ENT_PLAYER))
		return;

	if (other->gameEntity->health < 1)
		return;		// dead people can't pickup
	if (!(gameEntity->item->Flags & ITEMFLAG_GRABBABLE))
		return;		// not a grabbable item?

	CPlayerEntity *Player = dynamic_cast<CPlayerEntity*>(other);

	if (!(gameEntity->spawnflags & ITEM_TARGETS_USED))
	{
		UseTargets (other, Message);
		gameEntity->spawnflags |= ITEM_TARGETS_USED;
	}

	if (!gameEntity->item->Pickup(this,Player))
		return;

	// flash the screen
	Player->Client.bonus_alpha = 64;	

	// show icon and name on status bar
	Player->Client.PlayerState.SetStat(STAT_PICKUP_ICON, gameEntity->item->IconIndex);
	Player->Client.PlayerState.SetStat(STAT_PICKUP_STRING, gameEntity->item->GetConfigStringNumber());
	Player->Client.pickup_msg_time = level.framenum + 30;

	// change selected item
	if (gameEntity->item->Flags & ITEMFLAG_USABLE)
	{
		Player->Client.pers.Inventory.SelectedItem = gameEntity->item->GetIndex();
		Player->Client.PlayerState.SetStat(STAT_SELECTED_ITEM, Player->Client.pers.Inventory.SelectedItem);
	}

	if (gameEntity->item->PickupSound)
		Player->PlaySound(CHAN_ITEM, gameEntity->item->PickupSoundIndex
#ifdef CLEANCTF_ENABLED
		, 1, (gameEntity->item == NItems::RedFlag || gameEntity->item == NItems::BlueFlag) ? ATTN_NONE : ATTN_NORM
#endif
		);

	//if ((game.mode != GAME_COOPERATIVE && (ent->item->Flags & ITEMFLAG_STAY_COOP)) || (ent->spawnflags & (DROPPED_ITEM | DROPPED_PLAYER_ITEM)))
	if (!((game.mode & GAME_COOPERATIVE) &&  (gameEntity->item->Flags & ITEMFLAG_STAY_COOP)) || (gameEntity->spawnflags & (DROPPED_ITEM | DROPPED_PLAYER_ITEM)))
	{
		if (Flags & FL_RESPAWN)
			Flags &= ~FL_RESPAWN;
		else
			Free ();
	}
}

bool CItemEntity::Run ()
{
	if (NoPhysics)
		return false;
	return CTossProjectile::Run();
}

void CItemEntity::Use (CBaseEntity *other, CBaseEntity *activator)
{
	SetSvFlags (GetSvFlags() & ~SVF_NOCLIENT);
	Usable = false;

	if (gameEntity->spawnflags & ITEM_NO_TOUCH)
	{
		SetSolid (SOLID_BBOX);
		Touchable = false;
	}
	else
	{
		SetSolid (SOLID_TRIGGER);
		Touchable = true;
	}

	Link ();
}

// Returns a random team member of ent
CItemEntity *CItemEntity::GetRandomTeamMember (CItemEntity *Master)
{
	CItemEntity *Member = Master;
	int count = 0;

	for (count = 0, Member = Master; Member; Member = dynamic_cast<CItemEntity*>(Member->Chain), count++);
	int choice = irandom(count);
	for (count = 0, Member = Master; count < choice; Member = dynamic_cast<CItemEntity*>(Member->Chain), count++);

	return Member;
}

void CItemEntity::Think ()
{
	switch (ThinkState)
	{
	case ITS_DROPTOFLOOR:
		ThinkState = ITS_NONE;

		{
			SetMins (vec3f(-15,-15,-15));
			SetMaxs (vec3f(15,15,15));

			State.SetModelIndex (ModelIndex((gameEntity->model) ? gameEntity->model : gameEntity->item->WorldModel));
			SetSolid (SOLID_TRIGGER); 
			NoTouch = false;
			NoPhysics = false;

			CTrace tr = CTrace (State.GetOrigin(), GetMins(), GetMaxs(), (State.GetOrigin() + vec3f(0,0,-128)), gameEntity, CONTENTS_MASK_SOLID);
			if (tr.startSolid)
			{
				//gi.dprintf ("droptofloor: %s startsolid at (%f %f %f)\n", ent->classname, ent->state.origin[0], ent->state.origin[1], ent->state.origin[2]);
				MapPrint (MAPPRINT_WARNING, this, State.GetOrigin(), "Entity origin is in solid\n");
				Free ();
				return;
			}

			State.SetOrigin (tr.EndPos);

			if (gameEntity->team)
			{
				Flags &= ~FL_TEAMSLAVE;
				Chain = TeamChain;
				TeamChain = NULL;

				SetSvFlags (GetSvFlags() | SVF_NOCLIENT);
				SetSolid (SOLID_NOT);
				if (TeamMaster == this)
				{
					NextThink = level.framenum + FRAMETIME;
					ThinkState = ITS_RESPAWN;
				}
			}

			if (gameEntity->spawnflags & ITEM_NO_TOUCH)
			{
				SetSolid (SOLID_BBOX);
				NoTouch = true;
				State.RemoveEffects (EF_ROTATE);
				State.RemoveRenderEffects (RF_GLOW);
			}

			if (gameEntity->spawnflags & ITEM_TRIGGER_SPAWN)
			{
				SetSvFlags(GetSvFlags() | SVF_NOCLIENT);
				SetSolid (SOLID_NOT);
				Usable = true;
			}

			Link ();
		}
		break;

	case ITS_RESPAWN:
		ThinkState = ITS_NONE;

		{
			CBaseEntity *RespawnedEntity = this;
			if (gameEntity->team)
			{
				CBaseEntity *Master = TeamMaster;

		#ifdef CLEANCTF_ENABLED
		//ZOID
		//in ctf, when we are weapons stay, only the master of a team of weapons
		//is spawned
				if ((game.mode & GAME_CTF) &&
					dmFlags.dfWeaponsStay &&
					Master->gameEntity->item && (Master->gameEntity->item->Flags & ITEMFLAG_WEAPON))
					RespawnedEntity = Master; // This the way to do it?
				else
		//ZOID
		#endif
				RespawnedEntity = GetRandomTeamMember(dynamic_cast<CItemEntity*>(Master));
			}

			RespawnedEntity->SetSvFlags (RespawnedEntity->GetSvFlags() & ~SVF_NOCLIENT);
			RespawnedEntity->SetSolid (SOLID_TRIGGER);
			RespawnedEntity->Link ();

			// send an effect
			RespawnedEntity->State.SetEvent (EV_ITEM_RESPAWN);
		}
		break;
	case ITS_FREE:
		ThinkState = ITS_NONE;
		Free();
		break;
	};
}

// Generic item spawn function
void CItemEntity::Spawn (CBaseItem *item)
{
	gameEntity->item = item;

	assert (item != NULL);
	NextThink = level.framenum + 2;    // items start after other solids
	ThinkState = ITS_DROPTOFLOOR;
	NoPhysics = true;

	State.SetEffects(item->EffectFlags);
	State.SetRenderEffects(RF_GLOW);
}