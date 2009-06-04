////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file	game\cc_tent.cpp
///
/// \brief	Implements the temporary entity classes.
////////////////////////////////////////////////////////////////////////////////////////////////////
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
// cc_tent.cpp
// A huge class containing containers to spawn temp entities with a single function call.
//

#include "cc_local.h"

TECast::TECast (vec3_t Origin, byte Enum, ECastFlags castFlags)
{
	Vec3Copy (Origin, Or);
	castType = castFlags;

	WriteByte (SVC_TEMP_ENTITY);
	WriteByte (Enum);
};

TECast::~TECast ()
{
	Cast (castType, Or);
};

MultiCast::MultiCast(vec3_t Origin, byte Enum, ECastFlags castFlags)
{
	Vec3Copy (Origin, Or);
	castType = castFlags;

	WriteByte (Enum);
};

MultiCast::~MultiCast ()
{
	Cast (castType, Or);
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn	void CTempEnt_Splashes::Gunshot (vec3_t Origin, vec3_t Plane)
///
/// \brief	Gunshot effect
///
/// \author	Paril
/// \date	5/10/2009
///
/// \param	Origin	 - The origin. 
/// \param	Plane	 - The plane. 
////////////////////////////////////////////////////////////////////////////////////////////////////
void CTempEnt_Splashes::Gunshot (vec3_t Origin, vec3_t Plane)
{
	TECast cast(Origin, TE_GUNSHOT);
	WritePosition (Origin);
	WriteDirection (Plane ? Plane : vec3Origin);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn	void CTempEnt_Splashes::Shotgun (vec3_t Origin, vec3_t Plane)
///
/// \brief	Shotgun splash (less ricochet sounds)
///
/// \author	Paril
/// \date	5/10/2009
///
/// \param	Origin	 - The origin. 
/// \param	Plane	 - The plane. 
////////////////////////////////////////////////////////////////////////////////////////////////////
void CTempEnt_Splashes::Shotgun (vec3_t Origin, vec3_t Plane)
{
	TECast cast(Origin, TE_SHOTGUN);
	WritePosition (Origin);
	WriteDirection (Plane ? Plane : vec3Origin);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn	void CTempEnt_Splashes::Blood (vec3_t Origin, vec3_t Plane, EBloodType BloodType)
///
/// \brief	Blood splash. 
///
/// \author	Paril
/// \date	5/10/2009
///
/// \param	Origin		 - The origin. 
/// \param	Plane		 - The plane. 
/// \param	BloodType	 - Type of blood. 
////////////////////////////////////////////////////////////////////////////////////////////////////
void CTempEnt_Splashes::Blood (vec3_t Origin, vec3_t Plane, EBloodType BloodType)
{
	TECast cast(Origin, BloodType);
	WritePosition (Origin);
	WriteDirection (Plane ? Plane : vec3Origin);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn	void CTempEnt_Splashes::Blaster (vec3_t Origin, vec3_t Plane, EBlasterType BlasterType)
///
/// \brief	Blaster splash temp entity
///
/// \author	Paril
/// \date	5/10/2009
///
/// \param	Origin		 - The origin. 
/// \param	Plane		 - The plane. 
/// \param	BlasterType	 - Type of blaster hit. 
////////////////////////////////////////////////////////////////////////////////////////////////////
void CTempEnt_Splashes::Blaster (vec3_t Origin, vec3_t Plane, EBlasterType BlasterType)
{
	TECast cast(Origin, BlasterType);
	WritePosition (Origin);

	// ...
	// ID, you impress me.
	if (BlasterType == BLBlueHyperblaster)
		WritePosition (Plane ? Plane : vec3Origin);
	else
		WriteDirection (Plane ? Plane : vec3Origin);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn	void CTempEnt_Splashes::Sparks (vec3_t Origin, vec3_t Plane, ESparkType SparkType,
/// 	ESplashType color, byte amount)
///
/// \brief	Sparks temporary entities
///
/// \author	Paril
/// \date	5/10/2009
///
/// \param	Origin		 - The origin. 
/// \param	Plane		 - The plane. 
/// \param	SparkType	 - Type of the spark. 
/// \param	color		 - The color. 
/// \param	amount		 - The amount. 
////////////////////////////////////////////////////////////////////////////////////////////////////
void CTempEnt_Splashes::Sparks (vec3_t Origin, vec3_t Plane, ESparkType SparkType, ESplashType color, byte amount)
{
	TECast cast(Origin, SparkType);

	if (SparkType != STLaserSparks && SparkType != STWeldingSparks && SparkType != STTunnelSparks)
	{
		WritePosition (Origin);
		WriteDirection (Plane ? Plane : vec3Origin);
	}
	else
	{
		WriteByte (amount);
		WritePosition (Origin);
		WriteDirection (Plane ? Plane : vec3Origin);
		WriteByte (color);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn	void CTempEnt_Splashes::Splash (vec3_t Origin, vec3_t Plane, ESplashType color,
/// 	byte amount)
///
/// \brief	Splash temporary entities
///
/// \author	Paril
/// \date	5/10/2009
///
/// \param	Origin	 - The origin. 
/// \param	Plane	 - The plane. 
/// \param	color	 - The color. 
/// \param	amount	 - The amount. 
////////////////////////////////////////////////////////////////////////////////////////////////////
void CTempEnt_Splashes::Splash (vec3_t Origin, vec3_t Plane, ESplashType color, byte amount)
{
	TECast cast(Origin, TE_SPLASH);
	WriteByte (amount);
	WritePosition (Origin);
	WriteDirection (Plane ? Plane : vec3Origin);
	WriteByte (color);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn	void CTempEnt_Splashes::ShieldSparks (vec3_t Origin, vec3_t Plane, bool Screen)
///
/// \brief	Power Shield/Screen sparks
///
/// \author	Paril
/// \date	5/10/2009
///
/// \param	Origin	 - The origin. 
/// \param	Plane	 - The plane. 
/// \param	Screen	 - True if power screen sparks. 
////////////////////////////////////////////////////////////////////////////////////////////////////
void CTempEnt_Splashes::ShieldSparks (vec3_t Origin, vec3_t Plane, bool Screen)
{
	TECast cast(Origin, Screen ? TE_SCREEN_SPARKS : TE_SHIELD_SPARKS);
	WritePosition (Origin);
	WriteDirection (Plane ? Plane : vec3Origin);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn	void CTempEnt_Splashes::Steam (vec3_t Origin, vec3_t Normal, byte count,
/// 	ESplashType color, short magnitude, short id, long endTime)
///
/// \brief	Steam temporary entity
///
/// \author	Paril
/// \date	5/10/2009
///
/// \param	Origin		 - The origin. 
/// \param	Normal		 - The normal. 
/// \param	count		 - Number of steam particles. 
/// \param	color		 - The color. 
/// \param	magnitude	 - The magnitude of the steam. 
/// \param	id			 - The identifier (normally -1). 
/// \param	endTime		 - Time till end of sparks. 
////////////////////////////////////////////////////////////////////////////////////////////////////
void CTempEnt_Splashes::Steam (vec3_t Origin, vec3_t Normal, byte count, ESplashType color, short magnitude, short id, long endTime)
{
	TECast cast(Origin, TE_STEAM);
	WriteShort (id);
	WriteByte (count);
	WritePosition (Origin);
	WriteDirection (Normal);
	WriteByte (color);
	WriteShort (magnitude);

	// FIXME: Test
	if (id != -1)
		WriteLong (endTime);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn	void CTempEnt_Splashes::HeatSteam (vec3_t Origin, vec3_t Normal)
///
/// \brief	Heat steam (from the Heat Beam)
///
/// \author	Paril
/// \date	5/10/2009
///
/// \param	Origin	 - The origin. 
/// \param	Normal	 - The normal. 
////////////////////////////////////////////////////////////////////////////////////////////////////
void CTempEnt_Splashes::HeatSteam (vec3_t Origin, vec3_t Normal)
{
	TECast cast(Origin, TE_HEATBEAM_STEAM);
	WritePosition (Origin);
	WriteDirection (Normal ? Normal : vec3Origin);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn	void CTempEnt_Splashes::ChainfistSmoke (vec3_t Origin)
///
/// \brief	Chainfist smoke
///
/// \author	Paril
/// \date	5/10/2009
///
/// \param	Origin	 - The origin. 
////////////////////////////////////////////////////////////////////////////////////////////////////
void CTempEnt_Splashes::ChainfistSmoke (vec3_t Origin)
{
	TECast cast(Origin, TE_CHAINFIST_SMOKE);
	WritePosition (Origin);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn	static void BaseTrail (vec3_t Start, vec3_t End, short Ent = -1)
///
/// \brief	Writes the first few info needed for a trail
///
/// \author	Paril
/// \date	5/10/2009
///
/// \param	Start	 - The start. 
/// \param	End		 - The end. 
/// \param	Ent		 - The entity (ent - g_edicts). 
////////////////////////////////////////////////////////////////////////////////////////////////////
static void BaseTrail (vec3_t Start, vec3_t End, short Ent = -1)
{
	if (Ent != -1)
		WriteShort (Ent);
	WritePosition (Start);
	WritePosition (End);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn	void CTempEnt_Trails::RailTrail (vec3_t Start, vec3_t End)
///
/// \brief	Railgun trail
///
/// \author	Paril
/// \date	5/10/2009
///
/// \param	Start	 - The start. 
/// \param	End		 - The end. 
////////////////////////////////////////////////////////////////////////////////////////////////////
void CTempEnt_Trails::RailTrail (vec3_t Start, vec3_t End)
{
	TECast cast(Start, TE_RAILTRAIL);
	BaseTrail(Start, End);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn	void CTempEnt_Trails::BubbleTrail (vec3_t Start, vec3_t End)
///
/// \brief	Bubble trail
///
/// \author	Paril
/// \date	5/10/2009
///
/// \param	Start	 - The start. 
/// \param	End		 - The end. 
////////////////////////////////////////////////////////////////////////////////////////////////////
void CTempEnt_Trails::BubbleTrail (vec3_t Start, vec3_t End)
{
	TECast cast(Start, TE_BUBBLETRAIL);
	BaseTrail(Start, End);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn	void CTempEnt_Trails::FleshCable (vec3_t Start, vec3_t End, short Ent)
///
/// \brief	Flesh cable (medic, parasite, etc)
///
/// \author	Paril
/// \date	5/10/2009
///
/// \param	Start	 - The start. 
/// \param	End		 - The end. 
/// \param	Ent		 - The ent. 
////////////////////////////////////////////////////////////////////////////////////////////////////
void CTempEnt_Trails::FleshCable (vec3_t Start, vec3_t End, short Ent)
{
	TECast cast(Start, TE_PARASITE_ATTACK);
	BaseTrail(Start, End, Ent);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn	void CTempEnt_Trails::BFGLaser (vec3_t Start, vec3_t End)
///
/// \brief	The BFG green laser
///
/// \author	Paril
/// \date	5/10/2009
///
/// \param	Start	 - The start. 
/// \param	End		 - The end. 
////////////////////////////////////////////////////////////////////////////////////////////////////
void CTempEnt_Trails::BFGLaser (vec3_t Start, vec3_t End)
{
	TECast cast(Start, TE_BFG_LASER);
	BaseTrail(Start, End);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn	void CTempEnt_Trails::GrappleCable (vec3_t Start, vec3_t End, short Ent, vec3_t Offset)
///
/// \brief	Grappling hook trail
///
/// \author	Paril
/// \date	5/10/2009
///
/// \param	Start	 - The start. 
/// \param	End		 - The end. 
/// \param	Ent		 - The ent. 
/// \param	Offset	 - The offset. 
////////////////////////////////////////////////////////////////////////////////////////////////////
void CTempEnt_Trails::GrappleCable (vec3_t Start, vec3_t End, short Ent, vec3_t Offset)
{
	TECast cast(Start, TE_GRAPPLE_CABLE);
	BaseTrail(Start, End, Ent);
	WritePosition (Offset);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn	void CTempEnt_Trails::Lightning (vec3_t Start, vec3_t End, short SrcEnt, short DestEnt)
///
/// \brief	Lightning, from tesla
///
/// \author	Paril
/// \date	5/10/2009
///
/// \param	Start	 - The start. 
/// \param	End		 - The end. 
/// \param	SrcEnt	 - Source entity. 
/// \param	DestEnt	 - Destination entity. 
////////////////////////////////////////////////////////////////////////////////////////////////////
void CTempEnt_Trails::Lightning (vec3_t Start, vec3_t End, short SrcEnt, short DestEnt)
{
	TECast cast(Start, TE_LIGHTNING);
	WriteShort (SrcEnt);
	WriteShort (DestEnt);
	BaseTrail(Start, End);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn	void CTempEnt_Trails::DebugTrail (vec3_t Start, vec3_t End)
///
/// \brief	Debug trail (slow fading blue trail)
///
/// \author	Paril
/// \date	5/10/2009
///
/// \param	Start	 - The start. 
/// \param	End		 - The end. 
////////////////////////////////////////////////////////////////////////////////////////////////////
void CTempEnt_Trails::DebugTrail (vec3_t Start, vec3_t End)
{
	TECast cast(Start, TE_DEBUGTRAIL);
	BaseTrail(Start, End);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn	void CTempEnt_Trails::ForceWall (vec3_t Start, vec3_t End, byte color)
///
/// \brief	Sweet effect, particles rain down from start to end
///
/// \author	Paril
/// \date	5/10/2009
///
/// \param	Start	 - The start. 
/// \param	End		 - The end. 
/// \param	color	 - The color. 
////////////////////////////////////////////////////////////////////////////////////////////////////
void CTempEnt_Trails::ForceWall (vec3_t Start, vec3_t End, byte color)
{
	TECast cast(Start, TE_FORCEWALL);
	BaseTrail(Start, End);
	WriteByte (color);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn	void CTempEnt_Trails::HeatBeam (vec3_t Start, vec3_t End, short Ent, bool Monster)
///
/// \brief	Heat Beam itself.
///
/// \author	Paril
/// \date	5/10/2009
///
/// \param	Start	 - The start. 
/// \param	End		 - The end. 
/// \param	Ent		 - The entity. 
/// \param	Monster	 - If true, uses a special monster origin. 
////////////////////////////////////////////////////////////////////////////////////////////////////
void CTempEnt_Trails::HeatBeam (vec3_t Start, vec3_t End, short Ent, bool Monster)
{
	TECast cast(Start, Monster ? TE_MONSTER_HEATBEAM : TE_HEATBEAM);
	BaseTrail(Start, End, Ent);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn	void CTempEnt_Explosions::RocketExplosion (vec3_t Start, edict_t *ent, bool Water,
/// 	bool Particles)
///
/// \brief	The rocket explosion. Note that Water + (Particles = false) = Water!
///
/// \author	Paril
/// \date	5/10/2009
///
/// \param	Start		 - The start. 
/// \param	ent			 - If non-null, the ent. 
/// \param	Water		 - true to water.
/// \param	Particles	 - true to particles. 
////////////////////////////////////////////////////////////////////////////////////////////////////
void CTempEnt_Explosions::RocketExplosion (vec3_t Start, edict_t *ent, bool Water, bool Particles)
{
	// Water and NoParticles fight over a spot.. water will win, in the end
	TECast cast (ent->state.origin, Water ? TE_ROCKET_EXPLOSION_WATER : (Particles ? TE_ROCKET_EXPLOSION : TE_EXPLOSION1_NP), CASTFLAG_PVS);
	WritePosition (Start);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn	void CTempEnt_Explosions::GrenadeExplosion (vec3_t Start, edict_t *ent, bool Water)
///
/// \brief	Grenade explosion
///
/// \author	Paril
/// \date	5/10/2009
///
/// \param	Start	 - The start. 
/// \param	ent		 - If non-null, the ent. 
/// \param	Water	 - true to water. 
////////////////////////////////////////////////////////////////////////////////////////////////////
void CTempEnt_Explosions::GrenadeExplosion (vec3_t Start, edict_t *ent, bool Water)
{
	TECast cast (ent->state.origin, Water ? TE_GRENADE_EXPLOSION_WATER : TE_GRENADE_EXPLOSION, CASTFLAG_PVS);
	WritePosition (Start);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn	void CTempEnt_Explosions::BFGExplosion (vec3_t Origin, bool Big)
///
/// \brief	BFG Explosion
///
/// \author	Paril
/// \date	5/10/2009
///
/// \param	Origin	 - The origin. 
/// \param	Big		 - If true, uses big explosion.
////////////////////////////////////////////////////////////////////////////////////////////////////
void CTempEnt_Explosions::BFGExplosion (vec3_t Origin, bool Big)
{
	TECast cast (Origin, Big ? TE_BFG_BIGEXPLOSION : TE_BFG_EXPLOSION);
	WritePosition (Origin);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn	void CTempEnt_Explosions::PlasmaExplosion (vec3_t Origin)
///
/// \brief	Plasma explosion
///
/// \author	Paril
/// \date	5/10/2009
///
/// \param	Origin	 - The origin. 
////////////////////////////////////////////////////////////////////////////////////////////////////
void CTempEnt_Explosions::PlasmaExplosion (vec3_t Origin)
{
	TECast cast (Origin, TE_PLASMA_EXPLOSION);
	WritePosition (Origin);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn	void CTempEnt_Explosions::TrackerExplosion (vec3_t Origin)
///
/// \brief	Tracker explosion
///
/// \author	Paril
/// \date	5/10/2009
///
/// \param	Origin	 - The origin. 
////////////////////////////////////////////////////////////////////////////////////////////////////
void CTempEnt_Explosions::TrackerExplosion (vec3_t Origin)
{
	TECast cast (Origin, TE_TRACKER_EXPLOSION);
	WritePosition (Origin);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn	void CTempEnt_Explosions::NukeBlast (vec3_t Origin)
///
/// \brief	Nuke blast
///
/// \author	Paril
/// \date	5/10/2009
///
/// \param	Origin	 - The origin. 
////////////////////////////////////////////////////////////////////////////////////////////////////
void CTempEnt_Explosions::NukeBlast (vec3_t Origin)
{
	TECast cast (Origin, TE_NUKEBLAST);
	WritePosition (Origin);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn	void CTempEnt::Flashlight (vec3_t Origin, short Ent)
///
/// \brief	The flashlight temporary entity (bright white light)
///
/// \author	Paril
/// \date	5/10/2009
///
/// \param	Origin	 - The origin. 
/// \param	Ent		 - The ent. 
////////////////////////////////////////////////////////////////////////////////////////////////////
void CTempEnt::Flashlight (vec3_t Origin, short Ent)
{
	TECast cast(Origin, TE_FLASHLIGHT);
	WritePosition(Origin);
	WriteShort (Ent);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn	void CTempEnt::BossTeleport (vec3_t Origin)
///
/// \brief	Boss teleport. 
///
/// \author	Paril
/// \date	5/10/2009
///
/// \param	Origin	 - The origin. 
////////////////////////////////////////////////////////////////////////////////////////////////////
void CTempEnt::BossTeleport (vec3_t Origin)
{
	TECast cast(Origin, TE_BOSSTPORT);
	WritePosition(Origin);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn	void CTempEnt::TeleportEffect (vec3_t Origin)
///
/// \brief	Teleport effect. 
///
/// \author	Paril
/// \date	5/10/2009
///
/// \param	Origin	 - The origin. 
////////////////////////////////////////////////////////////////////////////////////////////////////
void CTempEnt::TeleportEffect (vec3_t Origin)
{
	TECast cast(Origin, TE_TELEPORT_EFFECT);
	WritePosition(Origin);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn	void CTempEnt::WidowBeamOut (vec3_t Origin, short id)
///
/// \brief	Widow beam-out effect. 
///
/// \author	Paril
/// \date	5/10/2009
///
/// \param	Origin	 - The origin. 
/// \param	id		 - The identifier. 
////////////////////////////////////////////////////////////////////////////////////////////////////
void CTempEnt::WidowBeamOut (vec3_t Origin, short id)
{
	TECast cast(Origin, TE_WIDOWBEAMOUT);
	WriteShort (id);
	WritePosition(Origin);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn	void CTempEnt::WidowSplash (vec3_t Origin)
///
/// \brief	Widow splash. 
///
/// \author	Paril
/// \date	5/10/2009
///
/// \param	Origin	 - The origin. 
////////////////////////////////////////////////////////////////////////////////////////////////////
void CTempEnt::WidowSplash (vec3_t Origin)
{
	TECast cast(Origin, TE_WIDOWSPLASH);
	WritePosition(Origin);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn	void CTempEnt::MuzzleFlash (vec3_t Origin, short Ent, short id)
///
/// \brief	Muzzle flash. 
///
/// \author	Paril
/// \date	5/10/2009
///
/// \param	Origin	 - The origin. 
/// \param	Ent		 - The ent. 
/// \param	id		 - The flash identifier. 
////////////////////////////////////////////////////////////////////////////////////////////////////
void CTempEnt::MuzzleFlash (vec3_t Origin, short Ent, short id)
{
	MultiCast cast (Origin, SVC_MUZZLEFLASH);
	WriteShort (Ent);
	WriteByte (id);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn	void CTempEnt::MonsterFlash (vec3_t Origin, short Ent, short id)
///
/// \brief	Monster muzzleflash. 
///
/// \author	Paril
/// \date	5/14/2009
///
/// \param	Origin	 - The origin. 
/// \param	Ent		 - The ent. 
/// \param	id		 - The flash identifier. 
////////////////////////////////////////////////////////////////////////////////////////////////////
void CTempEnt::MonsterFlash (vec3_t Origin, short Ent, short id)
{
	MultiCast cast (Origin, SVC_MUZZLEFLASH2);
	WriteShort (Ent);
	WriteByte (id);
}