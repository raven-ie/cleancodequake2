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
// cc_ctfitems.cpp
// Flags and Techs
//


#include "cc_local.h"

#ifdef CLEANCTF_ENABLED

void CTFResetFlags(void);

CTech::CTech (char *Classname, char *WorldModel, int EffectFlags,
			   char *PickupSound, char *Icon, char *Name, EItemFlags Flags,
			   char *Precache, uint32 TechNumber, ETechType TechType) :
CBaseItem (Classname, WorldModel, EffectFlags, PickupSound, Icon, Name, Flags,
		   Precache),
TechNumber(TechNumber),
TechType(TechType)
{
};

CTech::CTech (char *Classname, char *Model, char *Image, char *Name, CTech::ETechType TechType, uint32 TechNumber) :
CBaseItem (Classname, Model, EF_ROTATE, "items/pkup.wav", Image, Name, ITEMFLAG_GRABBABLE|ITEMFLAG_DROPPABLE|ITEMFLAG_TECH,
			NULL),
TechNumber(TechNumber),
TechType(TechType)
{
};
/*------------------------------------------------------------------------*/
/* TECH																	  */
/*------------------------------------------------------------------------*/

void CTFHasTech(CPlayerEntity *who)
{
	if (level.framenum - who->Client.ctf_lasttechmsg > 20)
	{
		who->PrintToClient(PRINT_CENTER, "You already have a TECH powerup.");
		who->Client.ctf_lasttechmsg = level.framenum;
	}
}

bool CTech::Pickup (class CItemEntity *ent, CPlayerEntity *other)
{
	if (other->Client.pers.Tech)
	{
		CTFHasTech(other);
		return false; // has this one
	}
	
	// client only gets one tech
	other->Client.pers.Inventory.Set(this, 1);
	other->Client.pers.Tech = this;
	other->Client.ctf_regentime = level.framenum;
	return true;
}

static CBaseEntity *FindTechSpawn(void)
{
	return SelectRandomDeathmatchSpawnPoint();
}

class CResistanceTech : public CTech
{
public:
	CResistanceTech (char *Classname, char *Model, char *Image, char *Name, CTech::ETechType TechType, uint32 TechNumber) :
	CTech (Classname, Model, Image, Name, TechType, TechNumber)
	{
	};

	void DoAggressiveTech	(	CPlayerEntity *Left, CBaseEntity *Right, bool Calculated, int &Damage, int &Knockback, int &DamageFlags,
										EMeansOfDeath &Mod	)
	{
		if (!Calculated)
			return;

		if (Damage)
		{
			// make noise
			Left->PlaySound (CHAN_AUTO, SoundIndex("ctf/tech1.wav"), (Left->Client.silencer_shots) ? 0.2f : 1.0f);
			Damage /= 2;
		}
	};
};

class CStrengthTech : public CTech
{
public:
	CStrengthTech (char *Classname, char *Model, char *Image, char *Name, CTech::ETechType TechType, uint32 TechNumber) :
	CTech (Classname, Model, Image, Name, TechType, TechNumber)
	{
	};

	void DoAggressiveTech	(	CPlayerEntity *Left, CBaseEntity *Right, bool Calculated, int &Damage, int &Knockback, int &DamageFlags,
										EMeansOfDeath &Mod	)
	{
		if (Calculated || (Left == Right))
			return;

		if (Damage)
			Damage *= 2;
	};
};

class CRegenTech : public CTech
{
public:
	CRegenTech (char *Classname, char *Model, char *Image, char *Name, CTech::ETechType TechType, uint32 TechNumber) :
	CTech (Classname, Model, Image, Name, TechType, TechNumber)
	{
	};

	void DoPassiveTech	(	CPlayerEntity *Player	)
	{
		CBaseItem *index;
		bool noise = false;
		if (Player->Client.ctf_regentime < level.framenum)
		{
			Player->Client.ctf_regentime = level.framenum;
			if (Player->gameEntity->health < 150)
			{
				Player->gameEntity->health += 5;
				if (Player->gameEntity->health > 150)
					Player->gameEntity->health = 150;
				Player->Client.ctf_regentime += 5;
				noise = true;
			}
			index = Player->Client.pers.Armor;
			if (index && Player->Client.pers.Inventory.Has(index) < 150)
			{
				Player->Client.pers.Inventory.Add (index, 5);
				if (Player->Client.pers.Inventory.Has(index) > 150)
					Player->Client.pers.Inventory.Set(index, 150);
				Player->Client.ctf_regentime += 5;
				noise = true;
			}
		}
		if (noise && Player->Client.ctf_techsndtime < level.framenum)
		{
			Player->Client.ctf_techsndtime = level.framenum + 10;
			Player->PlaySound (CHAN_AUTO, SoundIndex("ctf/tech4.wav"), (Player->Client.silencer_shots) ? 0.2f : 1.0f);
		}
	};
};

std::vector<CTech*>		TechList;

void SpawnTech(CBaseItem *item, CBaseEntity *spot);
class CTechEntity : public CItemEntity
{
public:
	bool AvoidOwner;

	CTechEntity() :
	  CItemEntity (),
	  AvoidOwner(true)
	  {
	  };

	  CTechEntity (int Index) :
	  CItemEntity(Index),
	  AvoidOwner(true)
	  {
	  };

	void Touch (CBaseEntity *other, plane_t *plane, cmBspSurface_t *surf)
	{
		if (AvoidOwner && (other->gameEntity == gameEntity->owner))
			return;

		CItemEntity::Touch (other, plane, surf);
	};

	void Think ()
	{
		CBaseEntity *spot;

		if ((spot = FindTechSpawn()) != NULL)
		{
			SpawnTech(gameEntity->item, spot);
			Free ();
		}
		else
			NextThink = level.framenum + CTF_TECH_TIMEOUT;
	};

	void Respawn ()
	{
		SpawnTech(gameEntity->item, FindTechSpawn());
		Free ();
	};

	void BecomeExplosion (bool grenade)
	{
		Respawn(); // this frees self!
	};

	void Spawn ()
	{
		AvoidOwner = true;
	};
};

CItemEntity *CTech::DropItem (CBaseEntity *ent)
{
	CTechEntity	*dropped = QNew (com_levelPool, 0) CTechEntity();
	vec3f	forward, right;

	dropped->gameEntity->classname = Classname;
	dropped->gameEntity->item = this;
	dropped->gameEntity->spawnflags = DROPPED_ITEM;
	dropped->State.SetEffects (EffectFlags);
	dropped->State.SetRenderEffects (RF_GLOW);
	dropped->SetMins (vec3f(-15));
	dropped->SetMaxs (vec3f(15));
	dropped->State.SetModelIndex (ModelIndex(WorldModel));
	dropped->SetSolid (SOLID_TRIGGER);
	dropped->gameEntity->owner = ent->gameEntity;

	if (ent->EntityFlags & ENT_PLAYER)
	{
		CPlayerEntity *Player = dynamic_cast<CPlayerEntity*>(ent);
		CTrace	trace;

		Player->Client.ViewAngle.ToVectors (&forward, &right, NULL);
		vec3f offset (24, 0, -16);

		vec3f result;
		G_ProjectSource (ent->State.GetOrigin(), offset, forward, right, result);

		trace = CTrace (ent->State.GetOrigin(), dropped->GetMins(), dropped->GetMaxs(),
			result, ent->gameEntity, CONTENTS_SOLID);
		dropped->State.SetOrigin (trace.endPos);
	}
	else
	{
		ent->State.GetAngles().ToVectors(&forward, &right, NULL);
		dropped->State.SetOrigin (ent->State.GetOrigin());
	}

	forward *= 100;
	dropped->gameEntity->velocity[0] = forward.X;
	dropped->gameEntity->velocity[1] = forward.Y;
	dropped->gameEntity->velocity[2] = 300;

	dropped->NextThink = level.framenum + 10;
	dropped->Link ();

	return dropped;
}

void CTech::Drop (CPlayerEntity *ent)
{
	CItemEntity *tech = DropItem(ent);
	tech->NextThink = level.framenum + CTF_TECH_TIMEOUT;
	ent->Client.pers.Inventory.Set(this, 0);
	ent->Client.pers.Tech = NULL;
}

void SpawnTech(CBaseItem *item, edict_t *spot);

void CTFDeadDropTech(CPlayerEntity *ent)
{
	if (!ent->Client.pers.Tech)
		return;

	CItemEntity *dropped = ent->Client.pers.Tech->DropItem(ent);
	// hack the velocity to make it bounce random
	dropped->gameEntity->velocity[0] = (randomMT() % 600) - 300;
	dropped->gameEntity->velocity[1] = (randomMT() % 600) - 300;
	dropped->NextThink = level.framenum + CTF_TECH_TIMEOUT;
	dropped->gameEntity->owner = NULL;
	ent->Client.pers.Inventory.Set(ent->Client.pers.Tech, 0);

	ent->Client.pers.Tech = NULL;
}

void SpawnTech(CBaseItem *item, CBaseEntity *spot)
{
	CTechEntity *ent = QNew (com_levelPool, 0) CTechEntity ();

	ent->gameEntity->classname = item->Classname;
	ent->gameEntity->item = item;
	ent->gameEntity->spawnflags = DROPPED_ITEM;
	ent->State.SetEffects(item->EffectFlags);
	ent->State.SetRenderEffects (RF_GLOW);
	ent->SetMins (vec3f(-15));
	ent->SetMaxs (vec3f(15));
	ent->State.SetModelIndex (ModelIndex(item->WorldModel));
	ent->SetSolid (SOLID_TRIGGER);
	ent->gameEntity->owner = ent->gameEntity;

	vec3f forward;
	vec3f(0, randomMT()%360, 0).ToVectors(&forward, NULL, NULL);

	ent->State.SetOrigin (spot->State.GetOrigin() + vec3f(0,0,16));
	forward *= 100;
	ent->gameEntity->velocity[0] = forward[0];
	ent->gameEntity->velocity[1] = forward[1];
	ent->gameEntity->velocity[2] = 300;

	ent->NextThink = level.framenum + CTF_TECH_TIMEOUT;

	ent->Link ();
}

// frees the passed edict!
void CTFRespawnTech(edict_t *ent)
{
	SpawnTech(ent->item, FindTechSpawn());
	G_FreeEdict(ent);
}

class CTechSpawner : public CThinkableEntity
{
public:
	CTechSpawner () :
	  CBaseEntity (),
	  CThinkableEntity ()
	{
		Spawn ();
	};

	CTechSpawner (int Index) :
	  CBaseEntity (Index),
	  CThinkableEntity (Index)
	{
		Spawn ();
	};

	bool Run ()
	{
		return CBaseEntity::Run();
	};

	void Think ()
	{
		//SpawnTech (NItems::Regeneration, FindTechSpawn());
		//SpawnTech (NItems::Haste, FindTechSpawn());
		//SpawnTech (NItems::Strength, FindTechSpawn());
		//SpawnTech (NItems::Resistance, FindTechSpawn());
		for (size_t i = 0; i < TechList.size(); i++)
			SpawnTech (TechList[i], FindTechSpawn ());

		Free ();
	};

	void Spawn ()
	{
		NextThink = level.framenum + 20;
	};
};

static void SpawnTechs()
{
	//SpawnTech (NItems::Regeneration, FindTechSpawn());
	//SpawnTech (NItems::Haste, FindTechSpawn());
	//SpawnTech (NItems::Strength, FindTechSpawn());
	//SpawnTech (NItems::Resistance, FindTechSpawn());
	for (size_t i = 0; i < TechList.size(); i++)
		SpawnTech (TechList[i], FindTechSpawn ());
}

void CTFSetupTechSpawn(void)
{
	if (dmFlags.dfCtfNoTech || !(game.mode & GAME_CTF))
		return;

	QNew (com_levelPool, 0) CTechSpawner;
}

void CTFResetTech(void)
{
	edict_t *ent;
	int i;

	for (ent = g_edicts + 1, i = 1; i < globals.numEdicts; i++, ent++)
	{
		if (ent->inUse && ent->item && (ent->item->Flags & ITEMFLAG_TECH))
				G_FreeEdict(ent);
	}
	SpawnTechs();
}

void	CTech::Use (CPlayerEntity *ent)
{
}

void AddTechsToList ()
{
/*	NItems::Regeneration = QNew (com_gamePool, 0) CTech ("item_tech4", "models/ctf/regeneration/tris.md2", EF_ROTATE, "items/pkup.wav", "tech4", "AutoDoc", ITEMFLAG_GRABBABLE|ITEMFLAG_DROPPABLE|ITEMFLAG_TECH, NULL);
	NItems::Haste = QNew (com_gamePool, 0) CTech ("item_tech3", "models/ctf/haste/tris.md2", EF_ROTATE, "items/pkup.wav", "tech3", "Time Accel", ITEMFLAG_GRABBABLE|ITEMFLAG_DROPPABLE|ITEMFLAG_TECH, NULL);
	NItems::Strength = QNew (com_gamePool, 0) CTech ("item_tech2", "models/ctf/strength/tris.md2", EF_ROTATE, "items/pkup.wav", "tech2", "Power Amplifier", ITEMFLAG_GRABBABLE|ITEMFLAG_DROPPABLE|ITEMFLAG_TECH, NULL);
	NItems::Resistance = QNew (com_gamePool, 0) CTech ("item_tech1", "models/ctf/resistance/tris.md2", EF_ROTATE, "items/pkup.wav", "tech1", "Disruptor Shield", ITEMFLAG_GRABBABLE|ITEMFLAG_DROPPABLE|ITEMFLAG_TECH, NULL);

	ItemList->AddItemToList (NItems::Regeneration);
	ItemList->AddItemToList (NItems::Haste);
	ItemList->AddItemToList (NItems::Strength);
	ItemList->AddItemToList (NItems::Resistance); */
	TechList.push_back (QNew (com_gamePool, 0) CResistanceTech ("item_tech1", "models/ctf/resistance/tris.md2",
														"tech1", "Disruptor Shield", CTech::TechAggressive, CTFTECH_RESISTANCE_NUMBER));

	TechList.push_back (QNew (com_gamePool, 0) CStrengthTech ("item_tech2", "models/ctf/strength/tris.md2",
														"tech2", "Power Amplifier", CTech::TechAggressive, CTFTECH_STRENGTH_NUMBER));

	TechList.push_back (QNew (com_gamePool, 0) CTech ("item_tech3", "models/ctf/haste/tris.md2",
														"tech3", "Time Accel", CTech::TechCustom, CTFTECH_HASTE_NUMBER));

	TechList.push_back (QNew (com_gamePool, 0) CRegenTech ("item_tech4", "models/ctf/regeneration/tris.md2",
														"tech4", "AutoDoc", CTech::TechPassive, CTFTECH_REGEN_NUMBER));

	for (size_t i = 0; i < TechList.size(); i++)
		ItemList->AddItemToList (TechList[i]);
}


CFlag::CFlag (char *Classname, char *WorldModel, int EffectFlags,
			   char *PickupSound, char *Icon, char *Name, EItemFlags Flags,
			   char *Precache, int team) :
CBaseItem (Classname, WorldModel, EffectFlags, PickupSound, Icon, Name, Flags,
		   Precache),
team(team)
{
};

void	CFlag::Drop (CPlayerEntity *ent)
{
}

void	CFlag::Use (CPlayerEntity *ent)
{
}

bool CFlag::Pickup(CItemEntity *ent, CPlayerEntity *other)
{
	if (team == other->Client.resp.ctf_team)
	{
		if (!(ent->gameEntity->spawnflags & DROPPED_ITEM))
		{
			// If we have the flag, but the flag isn't this, then we have another flag.
			// FIXME this code here will break with > 2 teams (when we get there)!!
			if (other->Client.pers.Flag && (other->Client.pers.Flag != this))
			{
				BroadcastPrintf(PRINT_HIGH, "%s captured the %s flag!\n",
						other->Client.pers.netname, CTFOtherTeamName(team));
				other->Client.pers.Inventory.Set(other->Client.pers.Flag, 0);
				other->Client.pers.Flag = NULL;

				ctfgame.last_flag_capture = level.framenum;
				ctfgame.last_capture_team = team;

				if (team == CTF_TEAM1)
					ctfgame.team1++;
				else
					ctfgame.team2++;

				ent->PlaySound(CHAN_RELIABLE+CHAN_NO_PHS_ADD+CHAN_VOICE, SoundIndex("ctf/flagcap.wav"), 1, ATTN_NONE, 0);

				// other gets another 10 frag bonus
				other->Client.resp.score += CTF_CAPTURE_BONUS;
				if (other->Client.resp.ghost)
					other->Client.resp.ghost->caps++;

				// Ok, let's do the player loop, hand out the bonuses
				for (int i = 1; i <= game.maxclients; i++)
				{
					CPlayerEntity *player = dynamic_cast<CPlayerEntity*>(g_edicts[i].Entity);
					if (!player->IsInUse())
						continue;

					if (player->Client.resp.ctf_team != other->Client.resp.ctf_team)
						player->Client.resp.ctf_lasthurtcarrier = -5;
					else if (player->Client.resp.ctf_team == other->Client.resp.ctf_team)
					{
						if (player != other)
							player->Client.resp.score += CTF_TEAM_BONUS;
						// award extra points for capture assists
						if (player->Client.resp.ctf_lastreturnedflag + CTF_RETURN_FLAG_ASSIST_TIMEOUT > level.framenum)
						{
							BroadcastPrintf(PRINT_HIGH, "%s gets an assist for returning the flag!\n", player->Client.pers.netname);
							player->Client.resp.score += CTF_RETURN_FLAG_ASSIST_BONUS;
						}
						if (player->Client.resp.ctf_lastfraggedcarrier + CTF_FRAG_CARRIER_ASSIST_TIMEOUT > level.framenum)
						{
							BroadcastPrintf(PRINT_HIGH, "%s gets an assist for fragging the flag carrier!\n", player->Client.pers.netname);
							player->Client.resp.score += CTF_FRAG_CARRIER_ASSIST_BONUS;
						}
					}
				}

				CTFResetFlags();
				return false;
			}
			return false; // its at home base already
		}	
		// hey, its not home.  return it by teleporting it back
		BroadcastPrintf(PRINT_HIGH, "%s returned the %s flag!\n", 
			other->Client.pers.netname, CTFTeamName(team));
		other->Client.resp.score += CTF_RECOVERY_BONUS;
		other->Client.resp.ctf_lastreturnedflag = level.framenum;
		ent->PlaySound (CHAN_RELIABLE+CHAN_NO_PHS_ADD+CHAN_VOICE, SoundIndex("ctf/flagret.wav"), 1, ATTN_NONE, 0);
		//CTFResetFlag will remove this entity!  We must return false
		CTFResetFlag(team);
		return false;
	}

	// hey, its not our flag, pick it up
	BroadcastPrintf(PRINT_HIGH, "%s got the %s flag!\n",
		other->Client.pers.netname, CTFTeamName(team));
	other->Client.resp.score += CTF_FLAG_BONUS;

	other->Client.pers.Inventory.Set(this, 1);
	other->Client.pers.Flag = this;
	other->Client.resp.ctf_flagsince = level.framenum;

	// pick up the flag
	// if it's not a dropped flag, we just make is disappear
	// if it's dropped, it will be removed by the pickup caller
	if (!(ent->gameEntity->spawnflags & DROPPED_ITEM))
	{
		ent->Flags |= FL_RESPAWN;
		ent->SetSvFlags (ent->GetSvFlags() | SVF_NOCLIENT);
		ent->SetSolid (SOLID_NOT);
	}
	return true;
}

void AddFlagsToList ()
{
	NItems::RedFlag = QNew (com_gamePool, 0) CFlag ("item_flag_team1", "players/male/flag1.md2", EF_FLAG1, "ctf/flagtk.wav", "i_ctf1", "Red Flag", ITEMFLAG_GRABBABLE, NULL, CTF_TEAM1);
	NItems::BlueFlag = QNew (com_gamePool, 0) CFlag ("item_flag_team2", "players/male/flag2.md2", EF_FLAG2, "ctf/flagtk.wav", "i_ctf2", "Blue Flag", ITEMFLAG_GRABBABLE, NULL, CTF_TEAM2);

	ItemList->AddItemToList (NItems::RedFlag);
	ItemList->AddItemToList (NItems::BlueFlag);
}

#endif