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
// cc_powerups.cpp
// Powerups
//

#include "cc_local.h"

CBasePowerUp::CBasePowerUp(char *Classname, char *WorldModel, int EffectFlags,
			   char *PickupSound, char *Icon, char *Name, EItemFlags Flags,
			   char *Precache, EPowerupFlags PowerupFlags) :
CBaseItem(Classname, WorldModel, EffectFlags, PickupSound, Icon, Name, Flags, Precache),
PowerupFlags(PowerupFlags)
{
};

// Powerups!
bool CBasePowerUp::Pickup (edict_t *ent, CPlayerEntity *other)
{
	if (PowerupFlags & POWERFLAG_STORE)
	{
		if (other->Client.pers.Inventory.Has(this) > 0 &&
			(!(PowerupFlags & POWERFLAG_STACK) ||
			(PowerupFlags & (POWERFLAG_STACK|POWERFLAG_BUTNOTINCOOP) && (game.mode == GAME_COOPERATIVE)) ||
			(game.mode == GAME_COOPERATIVE) && (this->Flags & ITEMFLAG_STAY_COOP)))
			return false;

		other->Client.pers.Inventory += this;
	}

	DoPickup (ent, other);
	return true;
}

void CBasePowerUp::DoPickup (edict_t *ent, CPlayerEntity *other)
{
}

void CBasePowerUp::Use (CPlayerEntity *ent)
{
}

void CBasePowerUp::Drop (CPlayerEntity *ent)
{
	if (PowerupFlags & POWERFLAG_STORE)
	{	
		DropItem (ent->gameEntity);
		ent->Client.pers.Inventory -= this;
	}
}

CMegaHealth::CMegaHealth(char *Classname, char *WorldModel, int EffectFlags,
			   char *PickupSound, char *Icon, char *Name, EItemFlags Flags,
			   char *Precache, EPowerupFlags PowerupFlags) :
CBasePowerUp(Classname, WorldModel, EffectFlags, PickupSound, Icon, Name, Flags, Precache, PowerupFlags)
{
};

CBackPack::CBackPack(char *Classname, char *WorldModel, int EffectFlags,
			   char *PickupSound, char *Icon, char *Name, EItemFlags Flags,
			   char *Precache, EPowerupFlags PowerupFlags) :
CBasePowerUp(Classname, WorldModel, EffectFlags, PickupSound, Icon, Name, Flags, Precache, PowerupFlags)
{
};

CQuadDamage::CQuadDamage(char *Classname, char *WorldModel, int EffectFlags,
			   char *PickupSound, char *Icon, char *Name, EItemFlags Flags,
			   char *Precache, EPowerupFlags PowerupFlags) :
CBasePowerUp(Classname, WorldModel, EffectFlags, PickupSound, Icon, Name, Flags, Precache, PowerupFlags)
{
};

CInvulnerability::CInvulnerability(char *Classname, char *WorldModel, int EffectFlags,
			   char *PickupSound, char *Icon, char *Name, EItemFlags Flags,
			   char *Precache, EPowerupFlags PowerupFlags) :
CBasePowerUp(Classname, WorldModel, EffectFlags, PickupSound, Icon, Name, Flags, Precache, PowerupFlags)
{
};

CSilencer::CSilencer(char *Classname, char *WorldModel, int EffectFlags,
			   char *PickupSound, char *Icon, char *Name, EItemFlags Flags,
			   char *Precache, EPowerupFlags PowerupFlags) :
CBasePowerUp(Classname, WorldModel, EffectFlags, PickupSound, Icon, Name, Flags, Precache, PowerupFlags)
{
};

CRebreather::CRebreather(char *Classname, char *WorldModel, int EffectFlags,
			   char *PickupSound, char *Icon, char *Name, EItemFlags Flags,
			   char *Precache, EPowerupFlags PowerupFlags) :
CBasePowerUp(Classname, WorldModel, EffectFlags, PickupSound, Icon, Name, Flags, Precache, PowerupFlags)
{
};

CEnvironmentSuit::CEnvironmentSuit(char *Classname, char *WorldModel, int EffectFlags,
			   char *PickupSound, char *Icon, char *Name, EItemFlags Flags,
			   char *Precache, EPowerupFlags PowerupFlags) :
CBasePowerUp(Classname, WorldModel, EffectFlags, PickupSound, Icon, Name, Flags, Precache, PowerupFlags)
{
};

CBandolier::CBandolier(char *Classname, char *WorldModel, int EffectFlags,
			   char *PickupSound, char *Icon, char *Name, EItemFlags Flags,
			   char *Precache, EPowerupFlags PowerupFlags) :
CBasePowerUp(Classname, WorldModel, EffectFlags, PickupSound, Icon, Name, Flags, Precache, PowerupFlags)
{
};

CAdrenaline::CAdrenaline(char *Classname, char *WorldModel, int EffectFlags,
			   char *PickupSound, char *Icon, char *Name, EItemFlags Flags,
			   char *Precache, EPowerupFlags PowerupFlags) :
CBasePowerUp(Classname, WorldModel, EffectFlags, PickupSound, Icon, Name, Flags, Precache, PowerupFlags)
{
};

CAncientHead::CAncientHead(char *Classname, char *WorldModel, int EffectFlags,
			   char *PickupSound, char *Icon, char *Name, EItemFlags Flags,
			   char *Precache, EPowerupFlags PowerupFlags) :
CBasePowerUp(Classname, WorldModel, EffectFlags, PickupSound, Icon, Name, Flags, Precache, PowerupFlags)
{
};

CPowerShield::CPowerShield(char *Classname, char *WorldModel, int EffectFlags,
			   char *PickupSound, char *Icon, char *Name, EItemFlags Flags,
			   char *Precache, EPowerupFlags PowerupFlags) :
CBasePowerUp(Classname, WorldModel, EffectFlags, PickupSound, Icon, Name, Flags, Precache, PowerupFlags)
{
};

void CMegaHealth::MegaHealthThink (edict_t *self)
{
	if (self->owner->health > self->owner->max_health
#ifdef CLEANCTF_ENABLED
//ZOID
		&& !dynamic_cast<CPlayerEntity*>(self->owner->Entity)->CTFHasRegeneration()
//ZOID
#endif
		)
	{
		self->nextthink = level.framenum + 10;
		self->owner->health -= 1;
		return;
	}

	if (!(self->spawnflags & DROPPED_ITEM) && (game.mode & GAME_DEATHMATCH))
		self->item->SetRespawn (self, 20);
	else
		G_FreeEdict (self);
}

// Seperate powerup classes
void CMegaHealth::DoPickup (edict_t *ent, CPlayerEntity *other)
{
#ifdef CLEANCTF_ENABLED
	if (!other->CTFHasRegeneration())
	{
#endif
		ent->think = &CMegaHealth::MegaHealthThink;
		ent->nextthink = level.framenum + 50;
		ent->owner = other->gameEntity;
		ent->flags |= FL_RESPAWN;
		ent->svFlags |= SVF_NOCLIENT;
		ent->solid = SOLID_NOT;

		other->gameEntity->health += 100;
#ifdef CLEANCTF_ENABLED
	}
	else if (!(ent->spawnflags & DROPPED_ITEM) && (game.mode & GAME_DEATHMATCH))
		ent->item->SetRespawn (ent, 30);
#endif
}

void CBackPack::DoPickup (edict_t *ent, CPlayerEntity *other)
{
	// Increase their max ammo, if applicable
	for (int i = 0; i < AMMOTAG_MAX; i++)
	{
		if (other->Client.pers.maxAmmoValues[i] < maxBackpackAmmoValues[i])
			other->Client.pers.maxAmmoValues[i] = maxBackpackAmmoValues[i];
	}

	// Give them some more ammo
	CAmmo *Ammo = dynamic_cast<CAmmo*>(FindItem("Bullets"));
	Ammo->AddAmmo (other, Ammo->Quantity);

	Ammo = dynamic_cast<CAmmo*>(FindItem("Shells"));
	Ammo->AddAmmo (other, Ammo->Quantity);

	Ammo = dynamic_cast<CAmmo*>(FindItem("Grenades"));
	Ammo->AddAmmo (other, Ammo->Quantity);

	Ammo = dynamic_cast<CAmmo*>(FindItem("Cells"));
	Ammo->AddAmmo (other, Ammo->Quantity);

	Ammo = dynamic_cast<CAmmo*>(FindItem("Slugs"));
	Ammo->AddAmmo (other, Ammo->Quantity);

	Ammo = dynamic_cast<CAmmo*>(FindItem("Rockets"));
	Ammo->AddAmmo (other, Ammo->Quantity);

	if (!(ent->spawnflags & DROPPED_ITEM) && (game.mode & GAME_DEATHMATCH))
		SetRespawn (ent, 180);
}

static int	quad_drop_timeout_hack;

void CQuadDamage::DoPickup (edict_t *ent, CPlayerEntity *other)
{
	if (game.mode & GAME_DEATHMATCH)
	{
		if (!(ent->spawnflags & DROPPED_ITEM) )
			SetRespawn (ent, 60);
		if (ent->spawnflags & DROPPED_PLAYER_ITEM)
			quad_drop_timeout_hack = (ent->nextthink - level.framenum);

		if (dmFlags.dfInstantItems)
			Use (other);
	}
}

void CQuadDamage::Use (CPlayerEntity *ent)
{
	int timeOut = 300;

	if (quad_drop_timeout_hack)
	{
		timeOut = quad_drop_timeout_hack;
		quad_drop_timeout_hack = 0;
	}

	if (ent->Client.quad_framenum > level.framenum)
		ent->Client.quad_framenum += timeOut;
	else
		ent->Client.quad_framenum = level.framenum + timeOut;

	ent->Client.pers.Inventory -= this;

	PlaySoundFrom(ent->gameEntity, CHAN_ITEM, SoundIndex("items/damage.wav"));
}

void CInvulnerability::DoPickup (edict_t *ent, CPlayerEntity *other)
{
	if (game.mode == GAME_DEATHMATCH)
	{
		if (!(ent->spawnflags & DROPPED_ITEM) )
			SetRespawn (ent, 300);
		if (dmFlags.dfInstantItems || (ent->spawnflags & DROPPED_PLAYER_ITEM))
			Use (other);
	}
}

void CInvulnerability::Use (CPlayerEntity *ent)
{
	ent->Client.pers.Inventory -= this;

	if (ent->Client.invincible_framenum > level.framenum)
		ent->Client.invincible_framenum += 300;
	else
		ent->Client.invincible_framenum = level.framenum + 300;

	PlaySoundFrom(ent->gameEntity, CHAN_ITEM, SoundIndex("items/protect.wav"));
}

void CSilencer::DoPickup (edict_t *ent, CPlayerEntity *other)
{
	if (game.mode & GAME_DEATHMATCH)
	{
		if (!(ent->spawnflags & DROPPED_ITEM) )
			SetRespawn (ent, 30);
		if (dmFlags.dfInstantItems || (ent->spawnflags & DROPPED_PLAYER_ITEM))
			Use (other);
	}
}

void CSilencer::Use (CPlayerEntity *ent)
{
	ent->Client.pers.Inventory -= this;
	ent->Client.silencer_shots += 30;
}

void CRebreather::DoPickup (edict_t *ent, CPlayerEntity *other)
{
	if (game.mode & GAME_DEATHMATCH)
	{
		if (!(ent->spawnflags & DROPPED_ITEM) )
			SetRespawn (ent, 60);
		if (dmFlags.dfInstantItems || (ent->spawnflags & DROPPED_PLAYER_ITEM))
			Use (other);
	}
}

void CRebreather::Use (CPlayerEntity *ent)
{
	ent->Client.pers.Inventory -= this;

	if (ent->Client.breather_framenum > level.framenum)
		ent->Client.breather_framenum += 300;
	else
		ent->Client.breather_framenum = level.framenum + 300;
}

void CEnvironmentSuit::DoPickup (edict_t *ent, CPlayerEntity *other)
{
	if (game.mode & GAME_DEATHMATCH)
	{
		if (!(ent->spawnflags & DROPPED_ITEM) )
			SetRespawn (ent, 60);
		if (dmFlags.dfInstantItems || (ent->spawnflags & DROPPED_PLAYER_ITEM))
			Use (other);
	}
}

void CEnvironmentSuit::Use (CPlayerEntity *ent)
{
	ent->Client.pers.Inventory -= this;

	if (ent->Client.enviro_framenum > level.framenum)
		ent->Client.enviro_framenum += 300;
	else
		ent->Client.enviro_framenum = level.framenum + 300;
}

void CBandolier::DoPickup (edict_t *ent, CPlayerEntity *other)
{
	// Increase their max ammo, if applicable
	for (int i = 0; i < AMMOTAG_MAX; i++)
	{
		if (other->Client.pers.maxAmmoValues[i] < maxBandolierAmmoValues[i])
			other->Client.pers.maxAmmoValues[i] = maxBandolierAmmoValues[i];
	}

	// Give them some more ammo
	CAmmo *Ammo = dynamic_cast<CAmmo*>(FindItem("Bullets"));
	Ammo->AddAmmo (other, Ammo->Quantity);

	Ammo = dynamic_cast<CAmmo*>(FindItem("Shells"));
	Ammo->AddAmmo (other, Ammo->Quantity);

	if (!(ent->spawnflags & DROPPED_ITEM) && (game.mode & GAME_DEATHMATCH))
		SetRespawn (ent, 60);
}

void CAdrenaline::DoPickup (edict_t *ent, CPlayerEntity *other)
{
	if (!(game.mode & GAME_DEATHMATCH))
		other->gameEntity->max_health += 1;

	if (other->gameEntity->health < other->gameEntity->max_health)
		other->gameEntity->health = other->gameEntity->max_health;

	if (!(ent->spawnflags & DROPPED_ITEM) && (game.mode & GAME_DEATHMATCH))
		SetRespawn (ent, 60);
}

void CAncientHead::DoPickup (edict_t *ent, CPlayerEntity *other)
{
	other->gameEntity->max_health += 2;

	if (!(ent->spawnflags & DROPPED_ITEM) && (game.mode & GAME_DEATHMATCH))
		SetRespawn (ent, 60);
}

void CPowerShield::DoPickup (edict_t *ent, CPlayerEntity *other)
{
	if (game.mode & GAME_DEATHMATCH)
	{
		if (!(ent->spawnflags & DROPPED_ITEM) )
			SetRespawn (ent, 60);

		// auto-use for DM only if we didn't already have one
		if (!other->Client.pers.Inventory.Has(this))
			Use (other);
	}
}

void CPowerShield::Use (CPlayerEntity *ent)
{
	if (ent->gameEntity->flags & FL_POWER_ARMOR)
	{
		ent->gameEntity->flags &= ~FL_POWER_ARMOR;
		PlaySoundFrom(ent->gameEntity, CHAN_AUTO, SoundIndex("misc/power2.wav"));
	}
	else
	{
		if (!ent->Client.pers.Inventory.Has(FindItem("Cells")))
		{
			ClientPrintf (ent->gameEntity, PRINT_HIGH, "No cells for power armor.\n");
			return;
		}
		ent->gameEntity->flags |= FL_POWER_ARMOR;
		PlaySoundFrom(ent->gameEntity, CHAN_AUTO, SoundIndex("misc/power1.wav"));
	}
}

void CPowerShield::Drop (CPlayerEntity *ent)
{
	if ((ent->gameEntity->flags & FL_POWER_ARMOR) && (ent->Client.pers.Inventory.Has(this) == 1))
		Use (ent);
	CBasePowerUp::Drop (ent);
}

void AddPowerupsToList ()
{
	CMegaHealth *MegaHealth = QNew (com_gamePool, 0) CMegaHealth("item_health_mega", "models/items/mega_h/tris.md2", 0, "items/m_health.wav", "i_health", "MegaHealth", ITEMFLAG_HEALTH|ITEMFLAG_POWERUP|ITEMFLAG_GRABBABLE, "", 0);
	CBackPack *BackPack = QNew (com_gamePool, 0) CBackPack ("item_pack", "models/items/pack/tris.md2", EF_ROTATE, "items/pkup.wav", "i_pack", "Ammo Pack", ITEMFLAG_POWERUP|ITEMFLAG_GRABBABLE, "", POWERFLAG_STACK|POWERFLAG_BUTNOTINCOOP);
	CQuadDamage *Quad = QNew (com_gamePool, 0) CQuadDamage ("item_quad", "models/items/quaddama/tris.md2", EF_ROTATE, "items/pkup.wav", "p_quad", "Quad Damage", ITEMFLAG_POWERUP|ITEMFLAG_GRABBABLE|ITEMFLAG_DROPPABLE|ITEMFLAG_USABLE, "", POWERFLAG_STORE|POWERFLAG_STACK|POWERFLAG_BUTNOTINCOOP);
	CInvulnerability *Invul = QNew (com_gamePool, 0) CInvulnerability ("item_invulnerability", "models/items/invulner/tris.md2", EF_ROTATE, "items/pkup.wav", "p_invulnerability", "Invulnerability", ITEMFLAG_POWERUP|ITEMFLAG_GRABBABLE|ITEMFLAG_DROPPABLE|ITEMFLAG_USABLE, "", POWERFLAG_STORE|POWERFLAG_STACK|POWERFLAG_BUTNOTINCOOP);
	CSilencer *Silencer = QNew (com_gamePool, 0) CSilencer ("item_silencer", "models/items/silencer/tris.md2", EF_ROTATE, "items/pkup.wav", "p_silencer", "Silencer", ITEMFLAG_POWERUP|ITEMFLAG_GRABBABLE|ITEMFLAG_DROPPABLE|ITEMFLAG_USABLE, "", POWERFLAG_STORE|POWERFLAG_STACK|POWERFLAG_BUTNOTINCOOP);
	CRebreather *Rebreather = QNew (com_gamePool, 0) CRebreather ("item_breather", "models/items/breather/tris.md2", EF_ROTATE, "items/pkup.wav", "p_rebreather", "Rebreather", ITEMFLAG_POWERUP|ITEMFLAG_STAY_COOP|ITEMFLAG_GRABBABLE|ITEMFLAG_DROPPABLE|ITEMFLAG_USABLE, "", POWERFLAG_STORE|POWERFLAG_STACK|POWERFLAG_BUTNOTINCOOP);
	CEnvironmentSuit *EnvironmentSuit = QNew (com_gamePool, 0) CEnvironmentSuit ("item_enviro", "models/items/enviro/tris.md2", EF_ROTATE, "items/pkup.wav", "p_envirosuit", "Environment Suit", ITEMFLAG_POWERUP|ITEMFLAG_GRABBABLE|ITEMFLAG_DROPPABLE|ITEMFLAG_USABLE, "", POWERFLAG_STORE|POWERFLAG_STACK|POWERFLAG_BUTNOTINCOOP);
	CBandolier *Bandolier = QNew (com_gamePool, 0) CBandolier ("item_bandolier", "models/items/band/tris.md2", EF_ROTATE, "items/pkup.wav", "p_bandolier", "Bandolier", ITEMFLAG_POWERUP|ITEMFLAG_GRABBABLE, "", POWERFLAG_STACK|POWERFLAG_BUTNOTINCOOP);
	CAdrenaline *Adrenaline = QNew (com_gamePool, 0) CAdrenaline ("item_adrenaline", "models/items/adrenal/tris.md2", EF_ROTATE, "items/pkup.wav", "p_adrenaline", "Adrenaline", ITEMFLAG_POWERUP|ITEMFLAG_GRABBABLE, "", 0);
	CAncientHead *AncientHead = QNew (com_gamePool, 0) CAncientHead ("item_ancient_head", "models/items/c_head/tris.md2", EF_ROTATE, "items/pkup.wav", "i_fixme", "Ancient Head", ITEMFLAG_POWERUP|ITEMFLAG_GRABBABLE, "", 0);
	CPowerShield *PowerShield = QNew (com_gamePool, 0) CPowerShield ("item_power_shield", "models/items/armor/shield/tris.md2", EF_ROTATE, "misc/ar3_pkup.wav", "i_powershield", "Power Shield", ITEMFLAG_ARMOR|ITEMFLAG_POWERUP|ITEMFLAG_GRABBABLE|ITEMFLAG_USABLE|ITEMFLAG_DROPPABLE, "", POWERFLAG_STORE|POWERFLAG_STACK|POWERFLAG_BUTNOTINCOOP);
	CPowerShield *PowerScreen = QNew (com_gamePool, 0) CPowerShield ("item_power_screen", "models/items/armor/screen/tris.md2", EF_ROTATE, "misc/ar3_pkup.wav", "i_powerscreen", "Power Screen", ITEMFLAG_ARMOR|ITEMFLAG_POWERUP|ITEMFLAG_GRABBABLE|ITEMFLAG_USABLE|ITEMFLAG_DROPPABLE, "", POWERFLAG_STORE|POWERFLAG_STACK|POWERFLAG_BUTNOTINCOOP);

	ItemList->AddItemToList (MegaHealth);
	ItemList->AddItemToList (BackPack);
	ItemList->AddItemToList (Quad);
	ItemList->AddItemToList (Invul);
	ItemList->AddItemToList (Silencer);
	ItemList->AddItemToList (Rebreather);
	ItemList->AddItemToList (EnvironmentSuit);
	ItemList->AddItemToList (Bandolier);
	ItemList->AddItemToList (Adrenaline);
	ItemList->AddItemToList (AncientHead);
	ItemList->AddItemToList (PowerShield);
	ItemList->AddItemToList (PowerScreen);
}