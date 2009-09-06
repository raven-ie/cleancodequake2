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
// cc_trigger_entities.cpp
// 
//

#include "cc_local.h"
#include "cc_trigger_entities.h"

/*QUAKED trigger_always (.5 .5 .5) (-8 -8 -8) (8 8 8)
This trigger will always fire.  It is activated by the world.
*/
class CTriggerAlways : public CMapEntity, public CUsableEntity
{
public:
	char	*Message;

	CTriggerAlways () :
	  CBaseEntity (),
	  CMapEntity ()
	{
	};

	CTriggerAlways (int Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index)
	{
	};

	void Use (CBaseEntity *, CBaseEntity *)
	{
	};

	bool Run ()
	{
		return CBaseEntity::Run();
	};

	void Spawn ()
	{
		// we must have some delay to make sure our use targets are present
		if (gameEntity->delay < 0.2f)
			gameEntity->delay = 0.2f;

		if (st.message)
			Message = Mem_PoolStrDup (st.message, com_levelPool, 0);

		UseTargets (this, Message);
	};
};

LINK_CLASSNAME_TO_CLASS ("trigger_always", CTriggerAlways);

class CTriggerBase abstract : public CMapEntity, public CThinkableEntity, public CTouchableEntity, public CUsableEntity
{
public:
	char	*Message;
	enum
	{
		TRIGGER_THINK_NONE,
		TRIGGER_THINK_FREE,
		TRIGGER_THINK_WAIT,

		TRIGGER_THINK_CUSTOM
	};
	uint32 ThinkType;
	bool Touchable;

	CTriggerBase () :
	  CBaseEntity (),
	  CMapEntity (),
	  CThinkableEntity (),
	  CTouchableEntity (),
	  CUsableEntity (),
	  ThinkType (TRIGGER_THINK_NONE),
	  Touchable(true)
	{
	};

	CTriggerBase (int Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CThinkableEntity (Index),
	  CTouchableEntity (Index),
	  CUsableEntity (Index),
	  ThinkType (TRIGGER_THINK_NONE),
	  Touchable(true)
	{
	};

	bool Run ()
	{
		return CBaseEntity::Run();
	};

	virtual void Think ()
	{
		switch (ThinkType)
		{
		case TRIGGER_THINK_WAIT:
			NextThink = 0;
			break;
		case TRIGGER_THINK_FREE:
			Free ();
			break;
		default:
			break;
		};
	};

	virtual void Touch (CBaseEntity *other, plane_t *plane, cmBspSurface_t *surf)
	{
		if (!Touchable)
			return;

		if(other->EntityFlags & ENT_PLAYER)
		{
			if (gameEntity->spawnflags & 2)
				return;
		}
		else if (other->EntityFlags & ENT_MONSTER)
		{
			if (!(gameEntity->spawnflags & 1))
				return;
		}
		else
			return;

		if (vec3f(gameEntity->movedir) != vec3fOrigin)
		{
			vec3f	forward;
			other->State.GetOrigin().ToVectors (&forward, NULL, NULL);
			if (forward.Dot(vec3f(gameEntity->movedir)) < 0)
				return;
		}

		gameEntity->activator = other->gameEntity;
		Trigger ();
	};

	virtual void Use (CBaseEntity *other, CBaseEntity *activator)
	{
		gameEntity->activator = activator->gameEntity;
		Trigger ();
	};

	void Init ()
	{
		if (State.GetAngles() != vec3fOrigin)
		{
			vec3f md, angles = State.GetAngles();
			G_SetMovedir (angles, md);
			Vec3Copy (md, gameEntity->movedir);
			State.SetAngles (angles);
		}

		SetSolid (SOLID_TRIGGER);
		SetModel (gameEntity, gameEntity->model);
		SetSvFlags (SVF_NOCLIENT);
	};
	// the trigger was just activated
	// ent->activator should be set to the activator so it can be held through a delay
	// so wait for the delay time before firing
	void Trigger ()
	{
		if (NextThink)
			return;		// already been triggered

		UseTargets (gameEntity->activator->Entity, Message);

		if (gameEntity->wait > 0)	
		{
			ThinkType = TRIGGER_THINK_WAIT;

			// Paril: backwards compatibility
			NextThink = level.framenum + (gameEntity->wait * 10);
		}
		else
		{	// we can't just remove (self) here, because this is a touch function
			// called while looping through area links...
			Touchable = false;
			NextThink = level.framenum + FRAMETIME;
			ThinkType = TRIGGER_THINK_FREE;
		}
	};

	virtual void Spawn () = 0;
};

/*QUAKED trigger_multiple (.5 .5 .5) ? MONSTER NOT_PLAYER TRIGGERED
Variable sized repeatable trigger.  Must be targeted at one or more entities.
If "delay" is set, the trigger waits some time after activating before firing.
"wait" : Seconds between triggerings. (.2 default)
sounds
1)	secret
2)	beep beep
3)	large switch
4)
set "message" to text string
*/
class CTriggerMultiple : public CTriggerBase
{
public:
	bool ActivateUse;

	CTriggerMultiple () :
	  CBaseEntity (),
	  CTriggerBase ()
	  {
	  };

	CTriggerMultiple (int Index) :
	  CBaseEntity(Index),
	  CTriggerBase (Index)
	  {
	  };

	void Use (CBaseEntity *other, CBaseEntity *activator)
	{
		if (ActivateUse)
		{
			ActivateUse = false;
			SetSolid (SOLID_TRIGGER);
			Link ();
			return;
		}
		CTriggerBase::Use (other, activator);
	};

	virtual void Spawn ()
	{
		if (gameEntity->sounds == 1)
			gameEntity->noise_index = SoundIndex ("misc/secret.wav");
		else if (gameEntity->sounds == 2)
			gameEntity->noise_index = SoundIndex ("misc/talk.wav");
		else if (gameEntity->sounds == 3)
			gameEntity->noise_index = SoundIndex ("misc/trigger1.wav");
		
		if (!gameEntity->wait)
			gameEntity->wait = 0.2f;
		SetSvFlags (GetSvFlags() | SVF_NOCLIENT);

		if (gameEntity->spawnflags & 4)
		{
			SetSolid (SOLID_NOT);
			ActivateUse = true;
		}
		else
		{
			SetSolid (SOLID_TRIGGER);
			ActivateUse = false;
		}

		if (State.GetAngles() != vec3fOrigin)
		{
			vec3f md, angles = State.GetAngles();
			G_SetMovedir (angles, md);
			Vec3Copy (md, gameEntity->movedir);
			State.SetAngles (angles);
		}

		SetModel (gameEntity, gameEntity->model);
		Link ();

		if (st.message)
			Message = Mem_PoolStrDup (st.message, com_levelPool, 0);
	};
};

LINK_CLASSNAME_TO_CLASS ("trigger_multiple", CTriggerMultiple);

/*QUAKED trigger_once (.5 .5 .5) ? x x TRIGGERED
Triggers once, then removes itself.
You must set the key "target" to the name of another object in the level that has a matching "targetname".

If TRIGGERED, this trigger must be triggered before it is live.

sounds
 1)	secret
 2)	beep beep
 3)	large switch
 4)

"message"	string to be displayed when triggered
*/
class CTriggerOnce : public CTriggerMultiple
{
public:
	CTriggerOnce () :
	  CBaseEntity (),
	  CTriggerMultiple ()
	  {
	  };

	CTriggerOnce (int Index) :
	  CBaseEntity (Index),
	  CTriggerMultiple (Index)
	  {
	  };

	void Spawn ()
	{
		// make old maps work because I messed up on flag assignments here
		// triggered was on bit 1 when it should have been on bit 4
		if (gameEntity->spawnflags & 1)
		{
			gameEntity->spawnflags &= ~1;
			gameEntity->spawnflags |= 4;
			MapPrint (MAPPRINT_WARNING, this, GetMins().MultiplyAngles (0.5f, GetSize()), "Fixed TRIGGERED flag\n");
		}

		gameEntity->wait = -1;
		CTriggerMultiple::Spawn ();
	};
};

LINK_CLASSNAME_TO_CLASS ("trigger_once", CTriggerOnce);

/*QUAKED trigger_counter (.5 .5 .5) ? nomessage
Acts as an intermediary for an action that takes multiple inputs.

If nomessage is not set, t will print "1 more.. " etc when triggered and "sequence complete" when finished.

After the counter has been triggered "count" times (default 2), it will fire all of it's targets and remove itself.
*/
class CTriggerCounter : public CTriggerMultiple
{
public:
	CTriggerCounter () :
	  CBaseEntity (),
	  CTriggerMultiple ()
	  {
	  };

	CTriggerCounter (int Index) :
	  CBaseEntity (Index),
	  CTriggerMultiple (Index)
	  {
	  };

	void Touch (CBaseEntity *other, plane_t *plane, cmBspSurface_t *surf)
	{
	};

	void Use (CBaseEntity *other, CBaseEntity *activator)
	{
		bool IsClient = true;
		if (gameEntity->count == 0)
			return;
		if (!(activator->EntityFlags & ENT_PLAYER))
			IsClient = false;
		
		CPlayerEntity *Player = (IsClient) ? dynamic_cast<CPlayerEntity*>(activator) : NULL;
		gameEntity->count--;

		if (gameEntity->count)
		{
			if (! (gameEntity->spawnflags & 1))
			{
				if (IsClient)
					Player->PrintToClient (PRINT_CENTER, "%i more to go...", gameEntity->count);
				activator->PlaySound (CHAN_AUTO, SoundIndex ("misc/talk1.wav"));
			}
			return;
		}
		
		if (! (gameEntity->spawnflags & 1))
		{
			if (IsClient)
				Player->PrintToClient (PRINT_CENTER, "Sequence completed!");
			activator->PlaySound (CHAN_AUTO, SoundIndex ("misc/talk1.wav"));
		}
		gameEntity->activator = activator->gameEntity;
		Trigger ();
	};

	void Spawn ()
	{
		gameEntity->wait = -1;
		if (!gameEntity->count)
			gameEntity->count = 2;
	};
};

LINK_CLASSNAME_TO_CLASS ("trigger_counter", CTriggerCounter);

#define PUSH_ONCE		1

/*QUAKED trigger_push (.5 .5 .5) ? PUSH_ONCE
Pushes the player
"speed"		defaults to 1000
*/
class CTriggerPush : public CTriggerMultiple
{
public:
	bool	Q3Touch;

	CTriggerPush () :
	  CBaseEntity (),
	  CTriggerMultiple (),
	  Q3Touch(false)
	  {
	  };

	CTriggerPush (int Index) :
	  CBaseEntity (Index),
	  CTriggerMultiple (Index),
	  Q3Touch(false)
	  {
	  };

	void Touch (CBaseEntity *other, plane_t *plane, cmBspSurface_t *surf)
	{
		vec3f vel = gameEntity->movedir;
		vel.Scale (gameEntity->speed * 10);
		if (Q3Touch)
		{
			Vec3Copy (vel, other->gameEntity->velocity);

			if (other->EntityFlags & ENT_PLAYER)
			{
				// don't take falling damage immediately from this
				CPlayerEntity *Player = dynamic_cast<CPlayerEntity*>(other);
				Vec3Copy (other->gameEntity->velocity, Player->Client.oldvelocity);
			}
		}
		else
		{
			// FIXME: replace this shit
			if (strcmp(other->gameEntity->classname, "grenade") == 0)
				Vec3Copy (vel, other->gameEntity->velocity);
			else if (other->gameEntity->health > 0)
			{
				Vec3Copy (vel, other->gameEntity->velocity);

				if (other->EntityFlags & ENT_PLAYER)
				{
					CPlayerEntity *Player = dynamic_cast<CPlayerEntity*>(other);
					// don't take falling damage immediately from this
					Vec3Copy (other->gameEntity->velocity, Player->Client.oldvelocity);
					if (Player->FlySoundDebounceTime < level.framenum)
					{
						Player->FlySoundDebounceTime = level.framenum + 15;
						other->PlaySound (CHAN_AUTO, gMedia.FlySound);
					}
				}
			}
			if (gameEntity->spawnflags & PUSH_ONCE)
				Free ();
		}
	};

	void Use (CBaseEntity *other, CBaseEntity *activator)
	{
	};

	void Spawn ()
	{
		Init ();

		if (!gameEntity->speed)
			gameEntity->speed = 1000;

		CBaseEntity *target;
		if (!gameEntity->target)
			Q3Touch = false;
		else if ((target = CC_Find (NULL, FOFS(targetname), gameEntity->target)) != NULL)
		{
			// Quake3
			//self->touch = trigger_push_q3touch;
			//Q3Touch = true;
			// TODO FIXME
			Free ();
			return;
		}

		Link ();
	};
};

LINK_CLASSNAME_TO_CLASS ("trigger_push", CTriggerPush);

/*QUAKED trigger_hurt (.5 .5 .5) ? START_OFF TOGGLE SILENT NO_PROTECTION SLOW
Any entity that touches this will be hurt.

It does dmg points of damage each server frame

SILENT			supresses playing the sound
SLOW			changes the damage rate to once per second
NO_PROTECTION	*nothing* stops the damage

"dmg"			default 5 (whole numbers only)

*/
class CTriggerHurt : public CTriggerMultiple
{
public:
	int32		NextHurt;

	CTriggerHurt () :
	  CBaseEntity (),
	  CTriggerMultiple (),
	  NextHurt(0)
	  {
	  };

	CTriggerHurt (int Index) :
	  CBaseEntity (Index),
	  CTriggerMultiple (Index),
	  NextHurt(0)
	  {
	  };

	void Touch (CBaseEntity *other, plane_t *plane, cmBspSurface_t *surf)
	{
		if (!((other->EntityFlags & ENT_HURTABLE) && dynamic_cast<CHurtableEntity*>(other)->CanTakeDamage))
			return;

		if (NextHurt > level.framenum)
			return;

		NextHurt = level.framenum + ((gameEntity->spawnflags & 16) ? 10 : FRAMETIME);
		if (!(gameEntity->spawnflags & 4))
		{
			if ((level.framenum % 10) == 0)
				other->PlaySound (CHAN_AUTO, gameEntity->noise_index);
		}

		dynamic_cast<CHurtableEntity*>(other)->TakeDamage (this, this, vec3fOrigin, other->State.GetOrigin(),
															vec3fOrigin, gameEntity->dmg, gameEntity->dmg,
															(gameEntity->spawnflags & 8) ? DAMAGE_NO_PROTECTION : 0, MOD_TRIGGER_HURT);
	};

	void Use (CBaseEntity *other, CBaseEntity *activator)
	{
		if (ActivateUse)
			return;

		SetSolid ((GetSolid() == SOLID_NOT) ? SOLID_TRIGGER : SOLID_NOT);
		Link ();

		if (!(gameEntity->spawnflags & 2))
			ActivateUse = true;
	};

	void Spawn ()
	{
		Init ();
		gameEntity->noise_index = SoundIndex ("world/electro.wav");

		if (!gameEntity->dmg)
			gameEntity->dmg = 5;

		SetSolid ((gameEntity->spawnflags & 1) ? SOLID_NOT : SOLID_TRIGGER);

		ActivateUse = (gameEntity->spawnflags & 2) ? false : true;
		Link ();
	};
};

LINK_CLASSNAME_TO_CLASS ("trigger_hurt", CTriggerHurt);

/*QUAKED trigger_monsterjump (.5 .5 .5) ?
Walking monsters that touch this will jump in the direction of the trigger's angle
"speed" default to 200, the speed thrown forward
"height" default to 200, the speed thrown upwards
*/
class CTriggerMonsterJump : public CTriggerMultiple
{
public:
	CTriggerMonsterJump () :
	  CBaseEntity (),
	  CTriggerMultiple ()
	  {
	  };

	CTriggerMonsterJump (int Index) :
	  CBaseEntity (Index),
	  CTriggerMultiple (Index)
	  {
	  };

	void Touch (CBaseEntity *other, plane_t *plane, cmBspSurface_t *surf)
	{
	};

	void Use (CBaseEntity *other, CBaseEntity *activator)
	{
		if ( !(other->EntityFlags & ENT_MONSTER))
			return;
		if (other->Flags & (FL_FLY | FL_SWIM) )
			return;
		if (other->GetSvFlags() & SVF_DEADMONSTER)
			return;

	// set XY even if not on ground, so the jump will clear lips
		other->gameEntity->velocity[0] = gameEntity->movedir[0] * gameEntity->speed;
		other->gameEntity->velocity[1] = gameEntity->movedir[1] * gameEntity->speed;
		
		if (!other->gameEntity->groundentity)
			return;
		
		other->gameEntity->groundentity = NULL;
		other->gameEntity->velocity[2] = gameEntity->movedir[2];
	};

	void Spawn ()
	{
		if (!gameEntity->speed)
			gameEntity->speed = 200;
		if (!st.height)
			st.height = 200;
		if (State.GetAngles().Y == 0)
		{
			vec3f Ang = State.GetAngles();
			Ang.Y = 360;
			State.SetAngles (Ang);
		}
		Init ();
		gameEntity->movedir[2] = st.height;
	};
};

LINK_CLASSNAME_TO_CLASS ("trigger_monsterjump", CTriggerMonsterJump);

/*QUAKED trigger_gravity (.5 .5 .5) ?
Changes the touching entites gravity to
the value of "gravity".  1.0 is standard
gravity for the level.
*/
class CTriggerGravity : public CTriggerMultiple
{
public:
	CTriggerGravity () :
	  CBaseEntity (),
	  CTriggerMultiple ()
	  {
	  };

	CTriggerGravity (int Index) :
	  CBaseEntity (Index),
	  CTriggerMultiple (Index)
	  {
	  };

	void Touch (CBaseEntity *other, plane_t *plane, cmBspSurface_t *surf)
	{
		other->gameEntity->gravity = gameEntity->gravity;
	};

	void Use (CBaseEntity *other, CBaseEntity *activator)
	{
	};

	void Spawn ()
	{
		if (st.gravity == 0)
		{
			//gi.dprintf("trigger_gravity without gravity set at (%f %f %f)\n", self->state.origin[0], self->state.origin[1], self->state.origin[2]);
			MapPrint (MAPPRINT_ERROR, this, State.GetOrigin(), "No gravity set\n");
			Free ();
			return;
		}

		Init ();
		gameEntity->gravity = atoi(st.gravity);
	};
};

LINK_CLASSNAME_TO_CLASS ("trigger_gravity", CTriggerGravity);

/*
==============================================================================

trigger_key

==============================================================================
*/

/*QUAKED trigger_key (.5 .5 .5) (-8 -8 -8) (8 8 8)
A relay trigger that only fires it's targets if player has the proper key.
Use "item" to specify the required key, for example "key_data_cd"
*/
class CTriggerKey : public CMapEntity, public CUsableEntity
{
public:
	bool		Usable;
	CBaseItem	*Item;
	int32		TouchDebounce;
	char		*Message;

	CTriggerKey () :
	  CBaseEntity (),
	  CMapEntity (),
	  CUsableEntity (),
	  Usable(true),
	  TouchDebounce(0)
	{
	};

	CTriggerKey (int Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CUsableEntity (Index),
	  Usable(true),
	  TouchDebounce(0)
	{
	};

	bool Run ()
	{
		return CBaseEntity::Run();
	};

	void Use (CBaseEntity *other, CBaseEntity *activator)
	{
		if (!Usable)
			return;
		if (!Item)
			return;
		if (!(activator->EntityFlags & ENT_PLAYER))
			return;

		CPlayerEntity *Player = dynamic_cast<CPlayerEntity*>(activator);

		int index = Item->GetIndex();
		if (!Player->Client.pers.Inventory.Has(index))
		{
			if (level.framenum < TouchDebounce)
				return;
			TouchDebounce = level.framenum + 50;

			if (!Message)
				Player->PrintToClient (PRINT_CENTER, "You need the %s", Item->Name);
			else
				Player->PrintToClient (PRINT_CENTER, "%s", Message);
			Player->PlaySound (CHAN_AUTO, SoundIndex ("misc/keytry.wav"));
			return;
		}

		Player->PlaySound (CHAN_AUTO, SoundIndex ("misc/keyuse.wav"));
		if (game.mode == GAME_COOPERATIVE)
		{
			if (Item == NItems::PowerCube)
			{
				int	cube;
				for (cube = 0; cube < 8; cube++)
				{
					if (Player->Client.pers.power_cubes & (1 << cube))
						break;
				}

				for (int player = 1; player <= game.maxclients; player++)
				{
					CPlayerEntity *ent = dynamic_cast<CPlayerEntity*>(g_edicts[player].Entity);
					if (!ent->IsInUse())
						continue;
					if (ent->Client.pers.power_cubes & (1 << cube))
					{
						ent->Client.pers.Inventory -= index;
						ent->Client.pers.power_cubes &= ~(1 << cube);
					}
				}
			}
			else
			{
				for (int player = 1; player <= game.maxclients; player++)
				{
					CPlayerEntity *ent = dynamic_cast<CPlayerEntity*>(g_edicts[player].Entity);
					if (!ent->IsInUse())
						continue;
					ent->Client.pers.Inventory.Set(index, 0);
				}
			}
		}
		else
			Player->Client.pers.Inventory -= index;

		UseTargets (activator, Message);

		Usable = false;
	};

	void Spawn ()
	{
		if (!st.item)
		{
			//gi.dprintf("no key item for trigger_key at (%f %f %f)\n", self->state.origin[0], self->state.origin[1], self->state.origin[2]);
			MapPrint (MAPPRINT_ERROR, this, State.GetOrigin(), "No key item\n");
			return;
		}
		Item = FindItemByClassname (st.item);

		if (!Item)
		{
			MapPrint (MAPPRINT_ERROR, this, State.GetOrigin(), "Item \"%s\" not found\n", st.item);
			//gi.dprintf("item %s not found for trigger_key at (%f %f %f)\n", st.item, self->state.origin[0], self->state.origin[1], self->state.origin[2]);
			return;
		}

		if (!gameEntity->target)
		{
			MapPrint (MAPPRINT_ERROR, this, State.GetOrigin(), "No target\n");
			//gi.dprintf("%s at (%f %f %f) has no target\n", self->classname, self->state.origin[0], self->state.origin[1], self->state.origin[2]);
			return;
		}

		SoundIndex ("misc/keytry.wav");
		SoundIndex ("misc/keyuse.wav");

		if (st.message)
			Message = Mem_PoolStrDup (st.message, com_levelPool, 0);
	};
};

LINK_CLASSNAME_TO_CLASS ("trigger_key", CTriggerKey);