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
// cc_target_entities.cpp
// 
//

#include "cc_local.h"

/*QUAKED target_speaker (1 0 0) (-8 -8 -8) (8 8 8) looped-on looped-off reliable
"noise"		wav file to play
"attenuation"
-1 = none, send to whole level
1 = normal fighting sounds
2 = idle sound level
3 = ambient sound level
"volume"	0.0 to 1.0

Normal sounds play each time the target is used.  The reliable flag can be set for crucial voiceovers.

Looped sounds are always atten 3 / vol 1, and the use function toggles it on/off.
Multiple identical looping sounds will just increase volume without any speed cost.
*/

class CTargetSpeaker : public CMapEntity, public CUsableEntity
{
public:
	CTargetSpeaker () :
	  CBaseEntity (),
	  CMapEntity (),
	  CUsableEntity ()
	  {
	  };

	CTargetSpeaker (int Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CUsableEntity (Index)
	  {
	  };

	void Use (CBaseEntity *other, CBaseEntity *activator)
	{
		if (gameEntity->spawnflags & 3) // looping sound toggles
			State.SetSound (State.GetSound() ? 0 : gameEntity->noise_index); // start or stop it
		else
			// use a positioned_sound, because this entity won't normally be
			// sent to any clients because it is invisible
			PlayPositionedSound (State.GetOrigin(), (gameEntity->spawnflags & 4) ? CHAN_VOICE|CHAN_RELIABLE : CHAN_VOICE, gameEntity->noise_index, gameEntity->volume, gameEntity->attenuation);
	};

	void Spawn ()
	{
		if(!st.noise)
		{
			//gi.dprintf("target_speaker with no noise set at (%f %f %f)\n", ent->state.origin[0], ent->state.origin[1], ent->state.origin[2]);
			MapPrint (MAPPRINT_ERROR, this, State.GetOrigin(), "No noise set\n");
			return;
		}

		char	buffer[MAX_QPATH];
		if (!strstr (st.noise, ".wav"))
			Q_snprintfz (buffer, sizeof(buffer), "%s.wav", st.noise);
		else
			Q_strncpyz (buffer, st.noise, sizeof(buffer));
		gameEntity->noise_index = SoundIndex (buffer);

		if (!gameEntity->volume)
			gameEntity->volume = 1.0;

		if (!gameEntity->attenuation)
			gameEntity->attenuation = 1.0;
		else if (gameEntity->attenuation == -1)	// use -1 so 0 defaults to 1
			gameEntity->attenuation = 0;

		// check for prestarted looping sound
		if (gameEntity->spawnflags & 1)
			State.SetSound (gameEntity->noise_index);

		// must link the entity so we get areas and clusters so
		// the server can determine who to send updates to
		Link ();
	};
};

LINK_CLASSNAME_TO_CLASS ("target_speaker", CTargetSpeaker);

class CTargetExplosion : public CMapEntity, public CThinkableEntity, public CUsableEntity
{
public:
	CTargetExplosion () :
	  CBaseEntity (),
	  CMapEntity (),
	  CThinkableEntity (),
	  CUsableEntity ()
	{
	};

	CTargetExplosion (int Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CThinkableEntity (Index),
	  CUsableEntity (Index)
	{
	};

	bool Run ()
	{
		return CBaseEntity::Run();
	};

	void Think ()
	{
		vec3f or = State.GetOrigin();
		CTempEnt_Explosions::RocketExplosion (or, this);

		T_RadiusDamage (gameEntity, gameEntity->activator, gameEntity->dmg, NULL, gameEntity->dmg+40, MOD_EXPLOSIVE);

		float save = gameEntity->delay;
		gameEntity->delay = 0;
		G_UseTargets (this, gameEntity->activator->Entity);
		gameEntity->delay = save;
	};

	void Use (CBaseEntity *other, CBaseEntity *activator)
	{
		gameEntity->activator = activator->gameEntity;

		if (!gameEntity->delay)
		{
			Think ();
			return;
		}

		// Backwards compatibility
		NextThink = level.framenum + (gameEntity->delay * 10);
	};

	void Spawn ()
	{
		SetSvFlags (SVF_NOCLIENT);
	};
};

LINK_CLASSNAME_TO_CLASS ("target_explosion", CTargetExplosion);

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

	CTriggerKey () :
	  CBaseEntity (),
	  CMapEntity (),
	  CUsableEntity (),
	  Usable(true)
	{
	};

	CTriggerKey (int Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CUsableEntity (Index),
	  Usable(true)
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
			if (level.framenum < gameEntity->touch_debounce_time)
				return;
			gameEntity->touch_debounce_time = level.framenum + 50;
			Player->PrintToClient (PRINT_CENTER, "You need the %s", Item->Name);
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

		G_UseTargets (this, activator);

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
	};
};

LINK_CLASSNAME_TO_CLASS ("trigger_key", CTriggerKey);