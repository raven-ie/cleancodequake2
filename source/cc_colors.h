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
// cc_colors.h
// A container with enumerations for colors and classes
//

#if !defined(__CC_COLORS_H__) || !INCLUDE_GUARDS
#define __CC_COLORS_H__

namespace NSColor
{
	enum EColors
	{
		Black,
		BlackGray,
		DimGray,
		DarkGray,
		Gray,
		SlateGray,
		Thistle,
		Silver,

		LightGray,
		Gainsboro,
		Lavender,
		AntiqueWhite,
		MistyRose,
		GhostWhite,
		LavenderBlush,
		Linen,

		DarkSalmon,
		Peru,
		MetallicBronze,
		Punga,
		WestCoast,
		Kumera,
		Shadow,
		DarkLeather,

		YellowMetal,
		TobaccoBrown,
		Hemlock,
		Millbrook,
		Birch,
		Zeus,
		Oil,
		NightRider,

		MidGray,
		Dolphin,
		SmokeGray,
		Topaz,
		Waterloo,
		Abbey,
		Mako,
		GunPowder,

		MineShaft,
		Masala,
		CodGray,
		CocoaBrown,
		Thunder,
		Tundora,
		Emperer,
		Jon,

		Domino,
		Makara,
		Coffee,
		Cement,
		Tussock,
		Whiskey,
		DiSienna,
		RawSienna,

		FuelYellow,
		Dixie,
		Geebung,
		Pizza,
		RenoSand,
		ButteredRum,
		MaiTai,
		Woodrush,

		TallPoppy,
		BurntUmber,
		MuleFawn,
		RedRobin,
		Mocha,
		FaluRed,
		Redwood,
		DarkBurgundy,

		IndianTan,
		Bean,
		RusticRed,
		Mahogany,
		Rebel,
		Kilamanjaro,
		Chocolate,
		Nero,

		SpicyMix,
		Beaver,
		DarkChestnut,
		Toast,
		MediumWood,
		CoralTree,
		Quincy,
		RomanCoffee,

		WoodBurn,
		Cola,
		DarkBirch,
		BlackMagic,
		WoodBark,
		PineTree,
		SealBrown,
		MightAsWellBeBlack,

		Russet,
		BakersChocolate,
		RawUmber,
		YukonGold,
		DarkRusset,
		DarkBrown1, // Ran out of brown color names :P
		DarkBrown2,
		DarkBrown3,

		AppleBlossum,
		ChestNut,
		Contessa,
		BrandyRose,
		LinkWater,
		Spindle,
		LightSteelBlue,
		RegentStBlue,

		Seagull,
		PictonBlue,
		SummerSky,
		Perlorous,
		PacificBlue,
		CuriousBlue,
		BostonBlue,
		Scooter,

		Sapphire,
		SherpaBlue,
		PrussianBlue,
		Cyprus,
		BlueWhale,
		Tangaroa,
		MidnightExpress,
		BlackPearl,

		RoseTaupe,
		SolidPink,
		Tosca,
		TawnyPort,
		Espresso,
		AuChico,
		Lotus,
		VanCleef,

		BulgarianRose,
		MoroccoBrown,
		Volcano,
		Moccaccino,
		LoneStar,
		RustRed,
		Heath,
		Bordeaux,

		Sage,
		Avocado,
		GreenSmoke,
		Asparagus,
		ChelseaCucumber,
		Highball,
		Wasabi,
		Sushi,

		TrendyGreen,
		Pesto,
		Olivetone,
		Rainforest,
		FijiGreen,
		ArmyGreen,
		VerdunGreen,
		Bronzetone,

		Cognac,
		MilanoRed,
		TiaMaria,
		RockSpray,
		Brown,
		MandarianOrange,
		DarkFaluRed,
		SaddleBrown,

		Rust,
		Sangria,
		DarkRed,
		GuardsmanOrange,
		FireBrick,
		VenetianRed,
		Maroon,
		Scarlett,

		MoodyBlue,
		BlueBell,
		HavelockBlue,
		FireSpeechBlue,
		CeruleanBlue,
		Danube,
		Shakespeare,
		Glacier,

		CornFlowerBlue,
		PortGore,
		DeepKoamaru,
		ParisM,
		Christalle,
		RegalBlue,
		CatalinaBlue,
		ResolutionBlue,

		Locust,
		Limerick,
		Lima,
		Christi,
		Citrus,
		Olive,
		PestoMesto,
		Himalaya,

		Caper,
		Olivine,
		Brass,
		Bahia,
		Lucky,
		LaRioja,
		Bilbao,
		Zuchini,

		Green,
		Lime, // ScreaminGreen
		FireSpeechGreen,
		Harlequin,
		MintGreen,
		Malachite,
		PastelGreen,
		White,

		Cream,
		LemonChiffon,
		Canary,
		LaserLemon,
		ParisDaisy,
		Gorse,
		Lemon,
		Gold,

		HarvestGold,
		RobRoy,
		TulipTree,
		FireBush,
		ButterCup,
		Gamboge,
		MangoTango,
		Tawny,

		Sangeria,
		SaddledBrown,
		Harley,
		PersianRed,
		Grenadier,
		RedOxide,
		CaputMortuum,
		VanClef,

		Red,
		NeonBlue,
		PatriotRed,
		PatriotBlue,
		Karaka,
		Maire,
		BokaraGray,
		TonysPink,

		Sunglo,
		Piper,
		PeruTan,
		Solitaire,
		SourDough,
		Mongoose,
		DonkeyBrown,
		TransparentPink
	};

	typedef colorb ColorPalette[256];
};

#else
FILE_WARNING
#endif