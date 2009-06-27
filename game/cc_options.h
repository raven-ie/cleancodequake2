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
// cc_options.h
// Compiler options
//

// Global macros
// Monsters won't attack allies and will try not to hurt allies with shots
#define MONSTERS_ARENT_STUPID

// Use the Rogue AI instead of regular AI
#define MONSTER_USE_ROGUE_AI

// Makes pathfinding work.
#define MONSTERS_USE_PATHFINDING

// Specific monster options
#define SUPERTANK_USES_GRENADE_LAUNCHER
#define FLYER_KNOWS_HOW_TO_DODGE
#define INFANTRY_DOES_REVERSE_GUN_ATTACK

// "Extended game imports" refers to:
// gi.cprintf, gi.dprintf, gi.centerprintf, gi.bprintf, gi.sound, gi.positioned_sound, gi.pmove
//#define USE_EXTENDED_GAME_IMPORTS

// Define this if you don't want old function calls to warn you about deprecation.
// THIS IS AT YOUR OWN RISK! CLEANCODE HAS PERFECTLY FUNCTIONAL ALTERNATIVES.
//#define _NO_DEPRECATING_OLD_FUNCTIONS

#ifndef _CC_INSECURE_DEPRECATE
#ifdef _NO_DEPRECATING_OLD_FUNCTIONS
#define _CC_INSECURE_DEPRECATE(_Replacement)
#else
#define _CC_INSECURE_DEPRECATE(_Replacement) _CRT_DEPRECATE_TEXT("CleanCode has a better replacement for this function. Consider using " #_Replacement " instead.\nTo disable deprecation, use _NO_DEPRECATING_OLD_FUNCTIONS in cc_options.h.")
#endif
#endif

// This is a simple macro to disable deprecation for everything inside the macro.
// This is only used internally; using this in your code could cause big problems.
#define _CC_DISABLE_DEPRECATION(x)	__pragma(warning(push)) \
									__pragma(warning(disable:4996)) \
									x \
									__pragma(warning(pop))

// Define this if you want to use Microsoft Visual Studio 7.0+ safe functions
#define CRT_USE_UNDEPRECATED_FUNCTIONS

// Define this to use the CleanCode exception handler
#define CC_USE_EXCEPTION_HANDLER

// Define this to allow GAME_CTF
#define CLEANCTF_ENABLED

// Don't touch this.
#define GAME_IS_BEING_COMPILED_NOT_ENGINE_GO_AWAY

// Define this if you want monsters to not be able to go through monstersolids
//#define MONSTERS_HIT_MONSTERSOLID

// Define this if you intend to use quaternions
//#define SHARED_ALLOW_QUATERNIONS

// Define this if you intend to use 3x3 matrix'
//#define SHARED_ALLOW_3x3_MATRIX

// Define this if you intend to use 4x4 matrix'
//#define SHARED_ALLOW_4x4_MATRIX

// Define this to enable some custom asserts that we place in some places
#define ALLOW_ASSERTS

// Define this if you have "pragma once"
#define HAS_PRAGMA_ONCE

#ifdef ALLOW_ASSERTS
#ifndef _DEBUG
#define _CC_ASSERT_EXPR(expr, msg)
#else
#define _CC_ASSERT_EXPR(expr, msg) \
        (void) ((!!(expr)) || \
                (1 != _CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, NULL, msg)) || \
                (_CrtDbgBreak(), 0))
#endif
#endif