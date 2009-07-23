////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file	game\cc_tent.h
///
/// \brief	Declares the temporary entity classes.
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
// cc_tent.h
// A huge class containing containers to spawn temp entities with a single function call.
//

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class	CTempEnt_Splashes
///
/// \brief	Temporary entity splashes
///
/// \author	Paril
/// \date	5/10/2009
////////////////////////////////////////////////////////////////////////////////////////////////////
class CTempEnt_Splashes
{
public:
	enum ESplashType
	{
		SPTUnknown = 0,
		SPTSparks,
		SPTWater,
		SPTMud,
		SPTSlime,
		SPTLava,
		SPTBlood
	};
	enum EBloodType
	{
		BTBlood = TE_BLOOD,
		BTMoreBlood = TE_MOREBLOOD,
		BTGreenBlood = TE_GREENBLOOD
	};
	enum EBlasterType
	{
		BLBlaster = TE_BLASTER,
		BLBlueHyperblaster = TE_BLUEHYPERBLASTER,
		BLFlechette = TE_FLECHETTE,
		BLGreenBlaster = TE_BLASTER2
	};
	enum ESparkType
	{
		STSparks = TE_SPARKS,
		STBulletSparks = TE_BULLET_SPARKS,
		STHeatbeamSparks = TE_HEATBEAM_SPARKS,
		STElectricSparks = TE_ELECTRIC_SPARKS,

		// Ones that have amount/color
		STLaserSparks = TE_LASER_SPARKS,
		STWeldingSparks = TE_WELDING_SPARKS,
		STTunnelSparks = TE_TUNNEL_SPARKS
	};

	static void Gunshot	(vec3_t Origin,
					vec3_t Normal);

	static void Shotgun	(vec3_t Origin,
					vec3_t Normal);

	static void Blood	(vec3_t Origin,
				vec3_t Normal,
				EBloodType BloodType = BTBlood);

	static void Blaster	(vec3_t Origin,
					vec3_t Normal,
					EBlasterType BlasterType = BLBlaster);

	static void Sparks	(vec3_t Origin,
				vec3_t Normal,
				ESparkType SparkType = STSparks,
				ESplashType color = SPTUnknown,
				byte amount = 8);

	static void Splash	(vec3_t Origin,
				vec3_t Normal,
				ESplashType color = SPTUnknown,
				byte amount = 8);

	static void ShieldSparks	(vec3_t Origin,
						vec3_t Normal,
						bool Screen = false);

	static void Steam	(vec3_t Origin,
				vec3_t Normal,
				byte count = 8,
				ESplashType color = SPTUnknown,
				short magnitude = 12,
				short id = -1,
				long endTime = 0);

	static void HeatSteam	(vec3_t Origin,
					vec3_t Normal);

	static void ChainfistSmoke	(vec3_t Origin);

	// vec3f overloads
	static void Gunshot	(vec3f &Origin,
					vec3f &Normal);

	static void Shotgun	(vec3f &Origin,
					vec3f &Normal);

	static void Blood	(vec3f &Origin,
				vec3f &Normal,
				EBloodType BloodType = BTBlood);

	static void Blaster	(vec3f &Origin,
					vec3f &Normal,
					EBlasterType BlasterType = BLBlaster);

	static void Sparks	(vec3f &Origin,
				vec3f &Normal,
				ESparkType SparkType = STSparks,
				ESplashType color = SPTUnknown,
				byte amount = 8);

	static void Splash	(vec3f &Origin,
				vec3f &Normal,
				ESplashType color = SPTUnknown,
				byte amount = 8);

	static void ShieldSparks	(vec3f &Origin,
						vec3f &Normal,
						bool Screen = false);

	static void Steam	(vec3f &Origin,
				vec3f &Normal,
				byte count = 8,
				ESplashType color = SPTUnknown,
				short magnitude = 12,
				short id = -1,
				long endTime = 0);

	static void HeatSteam	(vec3f &Origin,
					vec3f &Normal);

	static void ChainfistSmoke	(vec3f &Origin);
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class	CTempEnt_Trails
///
/// \brief	Temporary entities that deal with trails (start & end)
///
/// \author	Paril
/// \date	5/10/2009
////////////////////////////////////////////////////////////////////////////////////////////////////
class CTempEnt_Trails
{
public:
	static void RailTrail	(vec3_t Start,
					vec3_t End);

	static void HeatBeam	(vec3_t Start,
					vec3_t End,
					short Ent,
					bool Monster = false);

	static void ForceWall	(vec3_t Start,
					vec3_t End,
					byte color = NSColor::Lime);

	static void DebugTrail	(vec3_t Start,
					vec3_t End);

	static void Lightning	(vec3_t Start,
					vec3_t End,
					short SrcEnt,
					short DestEnt);

	static void GrappleCable	(vec3_t Start,
						vec3_t End,
						short Ent,
						vec3_t Offset = vec3Origin);

	static void BFGLaser	(vec3_t Start,
					vec3_t End);

	static void FleshCable		(vec3_t Start,
						vec3_t End,
						short Ent);
	static void BubbleTrail	(vec3_t Start,
						vec3_t End);

	// vec3f overloads
	static void RailTrail	(vec3f &Start,
					vec3f &End);

	static void HeatBeam	(vec3f & Start,
					vec3f &End,
					short Ent,
					bool Monster = false);

	static void ForceWall	(vec3f &Start,
					vec3f &End,
					byte color = NSColor::Lime);

	static void DebugTrail	(vec3f &Start,
					vec3f &End);

	static void Lightning	(vec3f &Start,
					vec3f &End,
					short SrcEnt,
					short DestEnt);

	static void GrappleCable	(vec3f &Start,
						vec3f &End,
						short Ent,
						vec3f &Offset = vec3fOrigin);

	static void BFGLaser	(vec3f &Start,
					vec3f &End);

	static void FleshCable		(vec3f &Start,
						vec3f &End,
						short Ent);
	static void BubbleTrail	(vec3f &Start,
						vec3f &End);

};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class	CTempEnt_Explosions
///
/// \brief	Temporary entity explosions
///
/// \author	Paril
/// \date	5/10/2009
////////////////////////////////////////////////////////////////////////////////////////////////////
class CTempEnt_Explosions
{
public:
	static void RocketExplosion	(vec3_t Origin, edict_t *ent,
							bool Water = false,
							bool Particles = true);

	static void GrenadeExplosion	(vec3_t Origin,
							edict_t *ent,
							bool Water = false);

	static void BFGExplosion		(vec3_t Origin,
							bool Big = false);

	static void PlasmaExplosion	(vec3_t Origin);

	static void TrackerExplosion	(vec3_t Origin);

	static void NukeBlast			(vec3_t Origin);

	// vec3f overloads
	static void RocketExplosion	(vec3f &Origin, CBaseEntity *ent,
							bool Water = false,
							bool Particles = true);

	static void GrenadeExplosion	(vec3f &Origin,
							CBaseEntity *ent,
							bool Water = false);

	static void BFGExplosion		(vec3f &Origin,
							bool Big = false);

	static void PlasmaExplosion	(vec3f &Origin);

	static void TrackerExplosion	(vec3f &Origin);

	static void NukeBlast			(vec3f &Origin);
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class	CTempEnt
///
/// \brief	Implements all temporary entities above and new ones
///
/// \author	Paril
/// \date	5/10/2009
////////////////////////////////////////////////////////////////////////////////////////////////////
class CTempEnt
{
public:
	// These don't go under either of the categories above
	static void Flashlight		(vec3_t Origin,
						short Ent);

	static void BossTeleport	(vec3_t Origin);

	static void TeleportEffect	(vec3_t Origin);

	static void WidowBeamOut	(vec3_t Origin,
						short id = -1);

	static void WidowSplash	(vec3_t Origin);

	static void MuzzleFlash	(vec3_t Origin,
						short Ent,
						short id);

	static void MonsterFlash	(vec3_t Origin,
						short Ent,
						short id);

	// vec3f overloads
	static void Flashlight		(vec3f &Origin,
						short Ent);

	static void BossTeleport	(vec3f &Origin);

	static void TeleportEffect	(vec3f &Origin);

	static void WidowBeamOut	(vec3f &Origin,
						short id = -1);

	static void WidowSplash	(vec3f &Origin);

	static void MuzzleFlash	(vec3f &Origin,
						short Ent,
						short id);

	static void MonsterFlash	(vec3f &Origin,
						short Ent,
						short id);
};

enum ECastType
{
	// Cast type
	CAST_MULTI,
	CAST_UNI
};
enum ECastFlags
{
	CASTFLAG_UNRELIABLE = 0,
	CASTFLAG_PVS = 1,
	CASTFLAG_PHS = 2,
	CASTFLAG_RELIABLE = 4
};
