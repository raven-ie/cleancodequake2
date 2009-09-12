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
// cc_techs.cpp
// 
//

#include "cc_local.h"

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


bool CTech::Pickup (class CItemEntity *ent, CPlayerEntity *other)
{
	if (other->Client.pers.Tech)
	{
		if (level.framenum - other->Client.lasttechmsg > 20)
		{
			other->PrintToClient(PRINT_CENTER, "You already have a TECH powerup.");
			other->Client.lasttechmsg = level.framenum;
		}
		return false; // has this one
	}
	
	// client only gets one tech
	other->Client.pers.Inventory.Set(this, 1);
	other->Client.pers.Tech = this;
	other->Client.regentime = level.framenum;
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
										EMeansOfDeath &Mod, bool Defending	)
	{
		if (!Calculated)
			return;

		if (Defending && Damage)
		{
			// make noise
			Left->PlaySound (CHAN_ITEM, SoundIndex("ctf/tech1.wav"), (Left->Client.silencer_shots) ? 0.2f : 1.0f);
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
										EMeansOfDeath &Mod, bool Defending	)
	{
		if (Calculated || (Left == Right))
			return;

		if (!Defending && Damage)
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
		if (Player->Client.regentime < level.framenum)
		{
			Player->Client.regentime = level.framenum;
			if (Player->gameEntity->health < 150)
			{
				Player->gameEntity->health += 5;
				if (Player->gameEntity->health > 150)
					Player->gameEntity->health = 150;
				Player->Client.regentime += 5;
				noise = true;
			}
			index = Player->Client.pers.Armor;
			if (index && Player->Client.pers.Inventory.Has(index) < 150)
			{
				Player->Client.pers.Inventory.Add (index, 5);
				if (Player->Client.pers.Inventory.Has(index) > 150)
					Player->Client.pers.Inventory.Set(index, 150);
				Player->Client.regentime += 5;
				noise = true;
			}
		}
		if (noise && Player->Client.techsndtime < level.framenum)
		{
			Player->Client.techsndtime = level.framenum + 10;
			Player->PlaySound (CHAN_AUTO, SoundIndex("ctf/tech4.wav"), (Player->Client.silencer_shots) ? 0.2f : 1.0f);
		}
	};
};

#ifdef AMMO_REGEN_TECH

#define SHELL_REGEN_COUNT	3
#define BULLET_REGEN_COUNT	6
#define GRENADE_REGEN_COUNT	2
#define CELL_REGEN_COUNT	3
#define ROCKET_REGEN_COUNT	2
#define SLUG_REGEN_COUNT	1
#define AMMO_REGEN_TIME		17

class CAmmoRegenTech : public CTech
{
public:
	CAmmoRegenTech (char *Classname, char *Model, char *Image, char *Name, CTech::ETechType TechType, uint32 TechNumber) :
	CTech (Classname, Model, Image, Name, TechType, TechNumber)
	{
	};

	void DoPassiveTech	(	CPlayerEntity *Player	)
	{
		const int RegenAmts[] = {SHELL_REGEN_COUNT, BULLET_REGEN_COUNT, GRENADE_REGEN_COUNT, ROCKET_REGEN_COUNT, CELL_REGEN_COUNT, SLUG_REGEN_COUNT};
		//CAmmo *Ptrs[] = {NItems::Shells, NItems::Bullets, NItems::Grenades, NItems::Rockets, NItems::Cells, NItems::Slugs};

		bool noise = false;
		if (Player->Client.regentime < level.framenum)
		{
			if (Player->Client.pers.Weapon)
			{
				if (Player->Client.pers.Weapon->WeaponItem)
				{
					CAmmo *Ammo = Player->Client.pers.Weapon->WeaponItem->Ammo;
	
					if (Ammo && Player->Client.pers.Inventory.Has(Player->Client.pers.Weapon->WeaponItem) && Player->Client.pers.Inventory.Has(Player->Client.pers.Weapon->WeaponItem) < Player->Client.pers.maxAmmoValues[Ammo->Tag])
					{			
						noise = true;
						Ammo->AddAmmo (Player, RegenAmts[Ammo->Tag]);
					}
				}
				else if (Player->Client.pers.Weapon->Item && (Player->Client.pers.Weapon->Item->Flags & ITEMFLAG_AMMO))
				{
					CAmmo *Ammo = dynamic_cast<CAmmo*>(Player->Client.pers.Weapon->Item);
					if (Player->Client.pers.Inventory.Has(Ammo) < Player->Client.pers.maxAmmoValues[Ammo->Tag])
					{
						noise = true;
						Ammo->AddAmmo (Player, RegenAmts[Ammo->Tag]);
					}
				}
			}

			Player->Client.regentime = level.framenum + AMMO_REGEN_TIME;
		}
		if (noise && Player->Client.techsndtime < level.framenum)
		{
			Player->Client.techsndtime = level.framenum + AMMO_REGEN_TIME;
			Player->PlaySound (CHAN_AUTO, SoundIndex("ctf/tech5.wav"), (Player->Client.silencer_shots) ? 0.2f : 1.0f);
		}
	};
};
#endif

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
	for (size_t i = 0; i < TechList.size(); i++)
		SpawnTech (TechList[i], FindTechSpawn ());
}

void SetupTechSpawn()
{
	if (dmFlags.dfCtfNoTech || (!(game.mode & GAME_CTF) && !dmFlags.dfDmTechs))
		return;

	QNew (com_levelPool, 0) CTechSpawner;
}

void ResetTechs()
{
	edict_t *ent;
	int i;

	for (ent = g_edicts + 1, i = 1; i < globals.numEdicts; i++, ent++)
	{
		CBaseEntity *Entity = ent->Entity;

		if (Entity && Entity->IsInUse() && (Entity->EntityFlags & ENT_ITEM))
		{
			CItemEntity *Item = dynamic_cast<CItemEntity*>(Entity);

			if (Item->Flags & ITEMFLAG_TECH)
				Item->Free ();
		}
	}
	SpawnTechs();
}

void	CTech::Use (CPlayerEntity *ent)
{
}

void AddTechsToList ()
{
	TechList.push_back (QNew (com_gamePool, 0) CResistanceTech ("item_tech1", "models/ctf/resistance/tris.md2",
														"tech1", "Disruptor Shield", CTech::TechAggressive, CTFTECH_RESISTANCE_NUMBER));

	TechList.push_back (QNew (com_gamePool, 0) CStrengthTech ("item_tech2", "models/ctf/strength/tris.md2",
														"tech2", "Power Amplifier", CTech::TechAggressive, CTFTECH_STRENGTH_NUMBER));

	TechList.push_back (QNew (com_gamePool, 0) CTech ("item_tech3", "models/ctf/haste/tris.md2",
														"tech3", "Time Accel", CTech::TechCustom, CTFTECH_HASTE_NUMBER));

	TechList.push_back (QNew (com_gamePool, 0) CRegenTech ("item_tech4", "models/ctf/regeneration/tris.md2",
														"tech4", "AutoDoc", CTech::TechPassive, CTFTECH_REGEN_NUMBER));

#ifdef AMMO_REGEN_TECH
	TechList.push_back (QNew (com_gamePool, 0) CAmmoRegenTech ("item_tech5", "models/ctf/ammo/tris.md2",
														"tech5", "Ammo Regen", CTech::TechPassive, CTFTECH_AMMOREGEN_NUMBER));
#endif

	for (size_t i = 0; i < TechList.size(); i++)
		ItemList->AddItemToList (TechList[i]);
}