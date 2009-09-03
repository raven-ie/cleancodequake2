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
// cc_media.cpp
// Storage for constant media
//

#include "cc_local.h"

SGameMedia_t gMedia;

void InitGameMedia ()
{
	char buffer[MAX_INFO_KEY];

	for (int i = 0; i < 4; i++)
	{
		Q_snprintfz (buffer, sizeof(buffer), "*death%i.wav", i+1);
		gMedia.Player.Death[i] = SoundIndex(buffer);
	}

	gMedia.Player.Fall[0] = SoundIndex("*fall1.wav");
	gMedia.Player.Fall[1] = SoundIndex("*fall2.wav");

	gMedia.Player.Gurp[0] = SoundIndex("*gurp1.wav");
	gMedia.Player.Gurp[1] = SoundIndex("*gurp2.wav");

	gMedia.Player.Jump = SoundIndex("*jump1.wav");

	for (int i = 25; i <= 100; i += 25)
	{
		Q_snprintfz (buffer, sizeof(buffer), "*pain%i_1.wav", i);
		gMedia.Player.Pain[(int)(i / 25) - 1][0] = SoundIndex(buffer);

		Q_snprintfz (buffer, sizeof(buffer), "*pain%i_2.wav", i);
		gMedia.Player.Pain[(int)(i / 25) - 1][1] = SoundIndex(buffer);
	}

	gMedia.Gib_SmallMeat	=	ModelIndex ("models/objects/gibs/sm_meat/tris.md2");
	gMedia.Gib_Arm			=	ModelIndex ("models/objects/gibs/arm/tris.md2");
	gMedia.Gib_Bone[0]		=	ModelIndex ("models/objects/gibs/bone/tris.md2");
	gMedia.Gib_Bone[1]		=	ModelIndex ("models/objects/gibs/bone2/tris.md2");
	gMedia.Gib_Chest		=	ModelIndex ("models/objects/gibs/chest/tris.md2");
	gMedia.Gib_Gear			=	ModelIndex ("models/objects/gibs/gear/tris.md2");
	gMedia.Gib_Skull		=	ModelIndex ("models/objects/gibs/skull/tris.md2");
	gMedia.Gib_Head[0]		=	ModelIndex ("models/objects/gibs/head/tris.md2");
	gMedia.Gib_Head[1]		=	ModelIndex ("models/objects/gibs/head2/tris.md2");
	gMedia.Gib_SmallMetal	=	ModelIndex ("models/objects/gibs/sm_metal/tris.md2");
	gMedia.Gib_Leg			=	ModelIndex ("models/objects/gibs/leg/tris.md2");

	gMedia.FrySound			=	SoundIndex ("player/fry.wav");
	gMedia.FrySound			=	SoundIndex ("misc/windfly.wav");

	// Hud stuff
	gMedia.Hud.HealthPic		=	ImageIndex ("i_health");
	gMedia.Hud.EnviroPic		=	ImageIndex ("p_envirosuit");
	gMedia.Hud.HelpPic			=	ImageIndex ("i_help");
	gMedia.Hud.InvulPic			=	ImageIndex ("p_invulnerability");
	gMedia.Hud.PowerShieldPic	=	ImageIndex ("i_powershield");
	gMedia.Hud.QuadPic			=	ImageIndex ("p_quad");
	gMedia.Hud.RebreatherPic	=	ImageIndex ("p_rebreather");
	gMedia.Hud.SilencerPic		=	ImageIndex ("p_silencer");

	InitItemMedia ();
}