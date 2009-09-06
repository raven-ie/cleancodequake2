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
// cc_func_entities.cpp
// Func_ entities that aren't brush models
//

#include "cc_local.h"
#include "cc_brushmodels.h"
#include "cc_func_entities.h"

/*QUAKED func_timer (0.3 0.1 0.6) (-8 -8 -8) (8 8 8) START_ON
"wait"			base time between triggering all targets, default is 1
"random"		wait variance, default is 0

so, the basic time between firing is a random time between
(wait - random) and (wait + random)

"delay"			delay before first firing when turned on, default is 0

"pausetime"		additional delay used only the very first time
				and only if spawned with START_ON

These can used but not touched.
*/

CFuncTimer::CFuncTimer () :
	CBaseEntity (),
	CMapEntity (),
	CThinkableEntity (),
	CUsableEntity ()
	{
	};

CFuncTimer::CFuncTimer (int Index) :
	CBaseEntity (Index),
	CMapEntity (Index),
	CThinkableEntity (Index),
	CUsableEntity (Index)
	{
	};

void CFuncTimer::Think ()
{
	UseTargets (gameEntity->activator->Entity, Message);
	NextThink = level.framenum + ((gameEntity->wait + (crandom() * gameEntity->random)) * 10);
}

bool CFuncTimer::Run ()
{
	return CBaseEntity::Run ();
};

void CFuncTimer::Use (CBaseEntity *other, CBaseEntity *activator)
{
	gameEntity->activator = activator->gameEntity;

	// if on, turn it off
	if (NextThink)
	{
		NextThink = 0;
		return;
	}

	// turn it on
	if (gameEntity->delay)
		NextThink = level.framenum + (gameEntity->delay * 10);
	else
		Think ();
}

void CFuncTimer::Spawn ()
{
	if (!gameEntity->wait)
		gameEntity->wait = 1.0;

	if (gameEntity->random >= gameEntity->wait)
	{
		gameEntity->random = gameEntity->wait - FRAMETIME;
		// Paril FIXME
		// This to me seems like a very silly warning.
		MapPrint (MAPPRINT_WARNING, this, State.GetOrigin(), "Random is greater than or equal to wait\n");
		//gi.dprintf("func_timer at (%f %f %f) has random >= wait\n", self->state.origin[0], self->state.origin[1], self->state.origin[2]);
	}

	if (gameEntity->spawnflags & 1)
	{
		// lots of backwards compatibility
		NextThink = level.framenum + 10 + ((st.pausetime + gameEntity->delay + gameEntity->wait + (crandom() * gameEntity->random))* 10);
		gameEntity->activator = this->gameEntity;
	}

	SetSvFlags (SVF_NOCLIENT);

	if (st.message)
		Message = Mem_PoolStrDup (st.message, com_levelPool, 0);
}

LINK_CLASSNAME_TO_CLASS ("func_timer", CFuncTimer);

// These two are required for func_clock, so they're here.

/*QUAKED target_character (0 0 1) ?
used with target_string (must be on same "team")
"count" is position in the string (starts at 1)
*/

CTargetCharacter::CTargetCharacter () :
	CBaseEntity (),
	CMapEntity (),
	CBrushModel ()
	{
	};

CTargetCharacter::CTargetCharacter (int Index) :
	CBaseEntity (Index),
	CMapEntity (Index),
	CBrushModel (Index)
	{
	};

bool CTargetCharacter::Run ()
{
	return CBrushModel::Run ();
};

void CTargetCharacter::Spawn ()
{
	PhysicsType = PHYSICS_PUSH;
	SetModel (gameEntity, gameEntity->model);
	SetSolid (SOLID_BSP);
	State.SetFrame (12);
	Link ();
};

LINK_CLASSNAME_TO_CLASS ("target_character", CTargetCharacter);

/*QUAKED target_string (0 0 1) (-8 -8 -8) (8 8 8)
*/

CTargetString::CTargetString () :
	CBaseEntity (),
	CMapEntity ()
	{
	};

CTargetString::CTargetString (int Index) :
	CBaseEntity (Index),
	CMapEntity (Index)
	{
	};

class CTargetStringForEachCallback : public CForEachTeamChainCallback
{
public:
	size_t	StrLen;
	char	*Message;

	void Callback (CBaseEntity *e)
	{
		if (!e->gameEntity->count)
			return;
		size_t n = e->gameEntity->count - 1;
		if (n > StrLen)
		{
			e->State.SetFrame (12);
			return;
		}

		char c = Message[n];
		if (c >= '0' && c <= '9')
			e->State.SetFrame (c - '0');
		else if (c == '-')
			e->State.SetFrame (10);
		else if (c == ':')
			e->State.SetFrame (11);
		else
			e->State.SetFrame (12);
	};
};

void CTargetString::Use (CBaseEntity *other, CBaseEntity *activator)
{
	CTargetStringForEachCallback cb;
	cb.StrLen = strlen(Message);
	cb.Message = Message;

	ForEachTeamChain (this, &cb);
}

void CTargetString::Spawn ()
{
	if (!st.message)
		Message = "";
	else
		Message = Mem_PoolStrDup (st.message, com_levelPool, 0);
}

LINK_CLASSNAME_TO_CLASS ("target_string", CTargetString);

/*QUAKED func_clock (0 0 1) (-8 -8 -8) (8 8 8) TIMER_UP TIMER_DOWN START_OFF MULTI_USE
target a target_string with this

The default is to be a time of day clock

TIMER_UP and TIMER_DOWN run for "count" seconds and the fire "pathtarget"
If START_OFF, this entity must be used before it starts

"style"		0 "xx"
			1 "xx:xx"
			2 "xx:xx:xx"
*/

#define CLOCK_MESSAGE_SIZE	16

CFuncClock::CFuncClock () :
	CBaseEntity (),
	CMapEntity (),
	CThinkableEntity (),
	CUsableEntity (),
	Usable(false)
	{
	};

CFuncClock::CFuncClock (int Index) :
	CBaseEntity (Index),
	CMapEntity (Index),
	CThinkableEntity (Index),
	CUsableEntity (Index),
	Usable(false)
	{
	};

bool CFuncClock::Run ()
{
	return CBaseEntity::Run ();
};

// don't let field width of any clock messages change, or it
// could cause an overwrite after a game load

void CFuncClock::Reset ()
{
	gameEntity->activator = NULL;
	if (gameEntity->spawnflags & 1)
	{
		Seconds = 0;
		gameEntity->wait = gameEntity->count;
	}
	else if (gameEntity->spawnflags & 2)
	{
		Seconds = gameEntity->count;
		gameEntity->wait = 0;
	}
}

void CFuncClock::FormatCountdown ()
{
	switch (gameEntity->style)
	{
	case 0:
	default:
		Q_snprintfz (Message, CLOCK_MESSAGE_SIZE, "%2i", Seconds);
		break;
	case 1:
		Q_snprintfz(Message, CLOCK_MESSAGE_SIZE, "%2i:%2i", Seconds / 60, Seconds % 60);
		if (Message[3] == ' ')
			Message[3] = '0';
		break;
	case 2:
		Q_snprintfz(Message, CLOCK_MESSAGE_SIZE, "%2i:%2i:%2i", Seconds / 3600, (Seconds - (Seconds / 3600) * 3600) / 60, Seconds % 60);
		if (Message[3] == ' ')
			Message[3] = '0';
		if (Message[6] == ' ')
			Message[6] = '0';
		break;
	};
}

void CFuncClock::Think ()
{
	if (!String)
	{
		String = dynamic_cast<CTargetString*>(CC_Find (NULL, FOFS(targetname), gameEntity->target));
		if (!String)
			return;
	}

	if (gameEntity->spawnflags & 1)
	{
		FormatCountdown ();
		Seconds++;
	}
	else if (gameEntity->spawnflags & 2)
	{
		FormatCountdown ();
		Seconds--;
	}
	else
	{
		struct tm	ltime;
		time_t		gmtime;

		time(&gmtime);
		localtime_s (&ltime, &gmtime);
		Q_snprintfz (Message, CLOCK_MESSAGE_SIZE, "%2i:%2i:%2i", ltime.tm_hour, ltime.tm_min, ltime.tm_sec);
		if (Message[3] == ' ')
			Message[3] = '0';
		if (Message[6] == ' ')
			Message[6] = '0';
	}

	if (String->Message)
		QDelete String->Message; // Already got one, free it first
	String->Message = Message;
	String->Use (this, this);

	if (((gameEntity->spawnflags & 1) && (Seconds > gameEntity->wait)) ||
		((gameEntity->spawnflags & 2) && (Seconds < gameEntity->wait)))
	{
		if (gameEntity->pathtarget)
		{
			char *savetarget = gameEntity->target;
			char *savemessage = Message;
			gameEntity->target = gameEntity->pathtarget;
			Message = NULL;
			UseTargets (gameEntity->activator->Entity, Message);
			gameEntity->target = savetarget;
			Message = savemessage;
		}

		if (!(gameEntity->spawnflags & 8))
			return;

		Reset ();

		if (gameEntity->spawnflags & 4)
			return;
	}

	NextThink = level.framenum + 10;
}

void CFuncClock::Use (CBaseEntity *other, CBaseEntity *activator)
{
	if (!Usable)
		return;

	if (!(gameEntity->spawnflags & 8))
		Usable = false;
	
	if (gameEntity->activator)
		return;

	gameEntity->activator = activator->gameEntity;
	Think ();
}

void CFuncClock::Spawn ()
{
	if (!gameEntity->target)
	{
		//gi.dprintf("%s with no target at (%f %f %f)\n", self->classname, self->state.origin[0], self->state.origin[1], self->state.origin[2]);
		MapPrint (MAPPRINT_ERROR, this, GetAbsMin(), "No target\n");
		Free ();
		return;
	}

	if ((gameEntity->spawnflags & 2) && (!gameEntity->count))
	{
		//gi.dprintf("%s with no count at (%f %f %f)\n", self->classname, self->state.origin[0], self->state.origin[1], self->state.origin[2]);
		MapPrint (MAPPRINT_ERROR, this, GetAbsMin(), "No count\n");
		Free ();
		return;
	}

	if ((gameEntity->spawnflags & 1) && (!gameEntity->count))
		gameEntity->count = 60*60;

	Reset ();
	Message = QNew (com_levelPool, 0) char[CLOCK_MESSAGE_SIZE];

	if (gameEntity->spawnflags & 4)
		Usable = true;
	else
		NextThink = level.framenum + 10;
}