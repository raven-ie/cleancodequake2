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
// cc_itemlist.h
// Itemlist
// Basically the link between items and the real world
//

#if !defined(CC_GUARD_ITEMLIST_H) || !INCLUDE_GUARDS
#define CC_GUARD_ITEMLIST_H

// Just to conform to the configstrings
const int MAX_ITEMS			= 256;
const int MAX_ITEMS_HASH	= (MAX_ITEMS/2);

typedef std::pair<size_t, size_t> THashedItemListPairType;
typedef std::multimap<size_t, size_t> THashedItemListType;
typedef std::vector<CBaseItem*> TItemListType;

// Generic itemlist.
class CItemList
{
public:
	sint32			numItems;

	CItemList();
	void SendItemNames ();

	// Revision 2.0
	// Instead of having the 9 functions I used to have to add
	// each TYPE of weapon, you create the item, and send it to here.
	void AddItemToList (CBaseItem *Item);

	// Sorts the list and adds the items
	void SortAndFinalize ();

	TItemListType			*TempList;

	TItemListType			Items;

	// There are two hash tables for items; hashed by classname, and hashed by item name.
	THashedItemListType		HashedClassnameItemList;
	THashedItemListType		HashedNameItemList;
};

extern CItemList *ItemList;

void InitItemlist ();
CBaseItem *FindItemByClassname (const char *name);
CBaseItem *FindItem (const char *name);
CBaseItem *GetItemByIndex (uint32 Index);
sint32 GetNumItems ();

void InitItemMedia ();

// A non-yuck way to my yuck in Weapons!
namespace NItems
{
	// Armor
	extern CArmor *JacketArmor;
	extern CArmor *CombatArmor;
	extern CArmor *BodyArmor;
	extern CArmor *ArmorShard;

	// Health
	extern CHealth *StimPack;
	extern CHealth *SmallHealth;
	extern CHealth *LargeHealth;

	// Keys
	extern CPowerCube *PowerCube;
#if CLEANCTF_ENABLED
	extern CFlag *RedFlag;
	extern CFlag *BlueFlag;
#endif

	// Weapons
	extern CWeaponItem *Blaster;
	extern CWeaponItem *Shotgun;
	extern CWeaponItem *SuperShotgun;
	extern CWeaponItem *Machinegun;
	extern CWeaponItem *Chaingun;
	extern CWeaponItem *GrenadeLauncher;
	extern CWeaponItem *RocketLauncher;
	extern CWeaponItem *HyperBlaster;
	extern CWeaponItem *Railgun;
	extern CWeaponItem *BFG;
#if CLEANCTF_ENABLED
	extern CWeaponItem *Grapple;
#endif
#if XATRIX_FEATURES
	extern CWeaponItem *IonRipper;	
	extern CWeaponItem *Phalanx;	
#endif

	// Ammo
	extern CAmmo *Shells;
	extern CAmmo *Bullets;
	extern CAmmo *Slugs;
	extern CAmmo *Rockets;
	extern CAmmo *Cells;
	extern CAmmo *Grenades;
	extern CAmmo *Trap;
#if XATRIX_FEATURES
	extern CAmmo *MagSlugs;
#endif
#if ROGUE_FEATURES
	extern CAmmo *Prox;
	extern CAmmo *Flechettes;
	extern CAmmo *Rounds;
	extern CAmmoWeapon *Tesla;
#endif

	// Powerups
	extern CMegaHealth *MegaHealth;
	extern CBackPack *BackPack;
	extern CQuadDamage *Quad;
#if XATRIX_FEATURES
	extern CBasePowerUp *QuadFire;
#endif
#if ROGUE_FEATURES
	extern CDoubleDamage *Double;
	extern CIRGoggles *IRGoggles;
#endif
	extern CInvulnerability *Invul;
	extern CSilencer *Silencer;
	extern CRebreather *Rebreather;
	extern CEnvironmentSuit *EnvironmentSuit;
	extern CBandolier *Bandolier;
	extern CAdrenaline *Adrenaline;
	extern CAncientHead *AncientHead;
	extern CPowerShield *PowerShield;
	extern CPowerShield *PowerScreen;
};

#else
FILE_WARNING
#endif
