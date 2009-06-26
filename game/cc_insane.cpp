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
// cc_insane.cpp
// Insane marine
//

#include "cc_local.h"
#include "m_insane.h"

CInsane Monster_Insane;

CInsane::CInsane ()
{
	Classname = "monster_insane";
	Scale = MODEL_SCALE;
}

void CInsane::Allocate (edict_t *ent)
{
	ent->Monster = new CInsane(Monster_Insane);
}

void CInsane::Fist ()
{
	PlaySoundFrom (Entity, CHAN_VOICE, SoundFist, 1, ATTN_IDLE);
}

void CInsane::Shake ()
{
	PlaySoundFrom (Entity, CHAN_VOICE, SoundShake, 1, ATTN_IDLE);
}

void CInsane::Moan ()
{
	PlaySoundFrom (Entity, CHAN_VOICE, SoundMoan, 1, ATTN_IDLE);
}

void CInsane::Scream ()
{
	PlaySoundFrom (Entity, CHAN_VOICE, SoundScream[rand()%8], 1, ATTN_IDLE);
}

CFrame InsaneFramesStandNormal [] =
{
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0, ConvertDerivedFunction(&CInsane::CheckDown))
};
CAnim InsaneMoveStandNormal (FRAME_stand60, FRAME_stand65, InsaneFramesStandNormal, &CMonster::Stand);

CFrame InsaneFramesStandInsane [] =
{
	CFrame (&CMonster::AI_Stand,	0,	ConvertDerivedFunction(&CInsane::Shake)),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0,	ConvertDerivedFunction(&CInsane::CheckDown))
};
CAnim InsaneMoveStandInsane (FRAME_stand65, FRAME_stand94, InsaneFramesStandInsane, &CMonster::Stand);

CFrame InsaneFramesUpToDown [] =
{
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0,	ConvertDerivedFunction(&CInsane::Moan)),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),

	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),

	CFrame (&CMonster::AI_Move,	2.7f),
	CFrame (&CMonster::AI_Move,	4.1f),
	CFrame (&CMonster::AI_Move,	6),
	CFrame (&CMonster::AI_Move,	7.6f),
	CFrame (&CMonster::AI_Move,	3.6f),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0,	ConvertDerivedFunction(&CInsane::Fist)),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),

	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0,	ConvertDerivedFunction(&CInsane::Fist)),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0)
};
CAnim InsaneMoveUpToDown (FRAME_stand1, FRAME_stand40, InsaneFramesUpToDown, ConvertDerivedFunction(&CInsane::OnGround));

CFrame InsaneFramesDownToUp [] =
{
	CFrame (&CMonster::AI_Move,	-0.7f),			// 41
	CFrame (&CMonster::AI_Move,	-1.2f),			// 42
	CFrame (&CMonster::AI_Move,	-1.5f),		// 43
	CFrame (&CMonster::AI_Move,	-4.5f),		// 44
	CFrame (&CMonster::AI_Move,	-3.5f),			// 45
	CFrame (&CMonster::AI_Move,	-0.2f),			// 46
	CFrame (&CMonster::AI_Move,	0),			// 47
	CFrame (&CMonster::AI_Move,	-1.3f),			// 48
	CFrame (&CMonster::AI_Move,	-3),				// 49
	CFrame (&CMonster::AI_Move,	-2),			// 50
	CFrame (&CMonster::AI_Move,	0),				// 51
	CFrame (&CMonster::AI_Move,	0),				// 52
	CFrame (&CMonster::AI_Move,	0),				// 53
	CFrame (&CMonster::AI_Move,	-3.3f),			// 54
	CFrame (&CMonster::AI_Move,	-1.6f),			// 55
	CFrame (&CMonster::AI_Move,	-0.3f),			// 56
	CFrame (&CMonster::AI_Move,	0),				// 57
	CFrame (&CMonster::AI_Move,	0),				// 58
	CFrame (&CMonster::AI_Move,	0)				// 59
};
CAnim InsaneMoveDownToUp (FRAME_stand41, FRAME_stand59, InsaneFramesDownToUp, &CMonster::Stand);

CFrame InsaneFramesJumpDown [] =
{
	CFrame (&CMonster::AI_Move,	0.2f),
	CFrame (&CMonster::AI_Move,	11.5f),
	CFrame (&CMonster::AI_Move,	5.1f),
	CFrame (&CMonster::AI_Move,	7.1f),
	CFrame (&CMonster::AI_Move,	0)
};
CAnim InsaneMoveJumpDown (FRAME_stand96, FRAME_stand100, InsaneFramesJumpDown, ConvertDerivedFunction(&CInsane::OnGround));


CFrame InsaneFramesDown [] =
{
	CFrame (&CMonster::AI_Move,	0),		// 100
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),		// 110
	CFrame (&CMonster::AI_Move,	-1.7f),
	CFrame (&CMonster::AI_Move,	-1.6f),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0, ConvertDerivedFunction(&CInsane::Fist)),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),		// 120
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),		// 130
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0, ConvertDerivedFunction(&CInsane::Moan)),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),		// 140
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),		// 150
	CFrame (&CMonster::AI_Move,	0.5f),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	-0.2f, ConvertDerivedFunction(&CInsane::Scream)),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0.2f),
	CFrame (&CMonster::AI_Move,	0.4f),
	CFrame (&CMonster::AI_Move,	0.6f),
	CFrame (&CMonster::AI_Move,	0.8f),
	CFrame (&CMonster::AI_Move,	0.7f),
	CFrame (&CMonster::AI_Move,	0, ConvertDerivedFunction(&CInsane::CheckUp))		// 160
};
CAnim InsaneMoveDown (FRAME_stand100, FRAME_stand160, InsaneFramesDown, ConvertDerivedFunction(&CInsane::OnGround));

CFrame InsaneFramesWalkNormal [] =
{
	CFrame (&CMonster::AI_Walk,	0, ConvertDerivedFunction(&CInsane::Scream)),
	CFrame (&CMonster::AI_Walk,	2.5f),
	CFrame (&CMonster::AI_Walk,	3.5f),
	CFrame (&CMonster::AI_Walk,	1.7f),
	CFrame (&CMonster::AI_Walk,	2.3f),
	CFrame (&CMonster::AI_Walk,	2.4f),
	CFrame (&CMonster::AI_Walk,	2.2f),
	CFrame (&CMonster::AI_Walk,	4.2f),
	CFrame (&CMonster::AI_Walk,	5.6f),
	CFrame (&CMonster::AI_Walk,	3.3f),
	CFrame (&CMonster::AI_Walk,	2.4f),
	CFrame (&CMonster::AI_Walk,	0.9f),
	CFrame (&CMonster::AI_Walk,	0)
};
CAnim InsaneMoveWalkNormal (FRAME_walk27, FRAME_walk39, InsaneFramesWalkNormal, &CMonster::Walk);
CAnim InsaneMoveRunNormal (FRAME_walk27, FRAME_walk39, InsaneFramesWalkNormal, &CMonster::Run);

CFrame InsaneFramesWalkInsane [] =
{
	CFrame (&CMonster::AI_Walk,	0, ConvertDerivedFunction(&CInsane::Scream)),		// walk 1
	CFrame (&CMonster::AI_Walk,	3.4f),		// walk 2
	CFrame (&CMonster::AI_Walk,	3.6f),		// 3
	CFrame (&CMonster::AI_Walk,	2.9f),		// 4
	CFrame (&CMonster::AI_Walk,	2.2f),		// 5
	CFrame (&CMonster::AI_Walk,	2.6f),		// 6
	CFrame (&CMonster::AI_Walk,	0),		// 7
	CFrame (&CMonster::AI_Walk,	0.7f),		// 8
	CFrame (&CMonster::AI_Walk,	4.8f),		// 9
	CFrame (&CMonster::AI_Walk,	5.3f),		// 10
	CFrame (&CMonster::AI_Walk,	1.1f),		// 11
	CFrame (&CMonster::AI_Walk,	2),		// 12
	CFrame (&CMonster::AI_Walk,	0.5f),		// 13
	CFrame (&CMonster::AI_Walk,	0),		// 14
	CFrame (&CMonster::AI_Walk,	0),		// 15
	CFrame (&CMonster::AI_Walk,	4.9f),		// 16
	CFrame (&CMonster::AI_Walk,	6.7f),		// 17
	CFrame (&CMonster::AI_Walk,	3.8f),		// 18
	CFrame (&CMonster::AI_Walk,	2),		// 19
	CFrame (&CMonster::AI_Walk,	0.2f),		// 20
	CFrame (&CMonster::AI_Walk,	0),		// 21
	CFrame (&CMonster::AI_Walk,	3.4f),		// 22
	CFrame (&CMonster::AI_Walk,	6.4f),		// 23
	CFrame (&CMonster::AI_Walk,	5),		// 24
	CFrame (&CMonster::AI_Walk,	1.8f),		// 25
	CFrame (&CMonster::AI_Walk,	0)		// 26
};
CAnim InsaneMoveWalkInsane (FRAME_walk1, FRAME_walk26, InsaneFramesWalkInsane, &CMonster::Walk);
CAnim InsaneMoveRunInsane (FRAME_walk1, FRAME_walk26, InsaneFramesWalkInsane, &CMonster::Run);

CFrame InsaneFramesStandPain [] =
{
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0)
};
CAnim InsaneMoveStandPain (FRAME_st_pain2, FRAME_st_pain12, InsaneFramesStandPain, &CMonster::Run);

CFrame InsaneFramesStandDeath [] =
{
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0)
};
CAnim InsaneMoveStandDeath (FRAME_st_death2, FRAME_st_death18, InsaneFramesStandDeath, ConvertDerivedFunction(&CInsane::Dead));

CFrame InsaneFramesCrawl [] =
{
	CFrame (&CMonster::AI_Walk,	0,		ConvertDerivedFunction(&CInsane::Scream)),
	CFrame (&CMonster::AI_Walk,	1.5f),
	CFrame (&CMonster::AI_Walk,	2.1f),
	CFrame (&CMonster::AI_Walk,	3.6f),
	CFrame (&CMonster::AI_Walk,	2),
	CFrame (&CMonster::AI_Walk,	0.9f),
	CFrame (&CMonster::AI_Walk,	3),
	CFrame (&CMonster::AI_Walk,	3.4f),
	CFrame (&CMonster::AI_Walk,	2.4f)
};
CAnim InsaneMoveCrawl (FRAME_crawl1, FRAME_crawl9, InsaneFramesCrawl);
CAnim InsaneMoveRunCrawl (FRAME_crawl1, FRAME_crawl9, InsaneFramesCrawl);

CFrame InsaneFramesCrawlPain [] =
{
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0)
};
CAnim InsaneMoveCrawlPain (FRAME_cr_pain2, FRAME_cr_pain10, InsaneFramesCrawlPain, &CMonster::Run);

CFrame InsaneFramesCrawlDeath [] =
{
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0)
};
CAnim InsaneMoveCrawlDeath (FRAME_cr_death10, FRAME_cr_death16, InsaneFramesCrawlDeath, ConvertDerivedFunction(&CInsane::Dead));

CFrame InsaneFramesCross [] =
{
	CFrame (&CMonster::AI_Move,	0, ConvertDerivedFunction(&CInsane::Moan)),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0)
};
CAnim InsaneMoveCross (FRAME_cross1, FRAME_cross15, InsaneFramesCross, ConvertDerivedFunction(&CInsane::Cross));

CFrame InsaneFramesStruggleCross [] =
{
	CFrame (&CMonster::AI_Move,	0, ConvertDerivedFunction(&CInsane::Scream)),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0)
};
CAnim InsaneMoveStruggleCross (FRAME_cross16, FRAME_cross30, InsaneFramesStruggleCross, ConvertDerivedFunction(&CInsane::Cross));

void CInsane::Cross ()
{
	CurrentMove = (random() < 0.8) ? &InsaneMoveCross : &InsaneMoveStruggleCross;
}

void CInsane::Walk ()
{
	if ( (Entity->spawnflags & 16) && (Entity->state.frame == FRAME_cr_pain10) )			// Hold Ground?
	{
		CurrentMove = &InsaneMoveDown;
		return;
	}

	if (Entity->spawnflags & 4)
		CurrentMove = &InsaneMoveCrawl;
	else
		CurrentMove = (random() <= 0.5) ? &InsaneMoveWalkNormal : &InsaneMoveWalkInsane;
}

void CInsane::Run ()
{
	if ( (Entity->spawnflags & 16) && (Entity->state.frame == FRAME_cr_pain10))			// Hold Ground?
	{
		CurrentMove = &InsaneMoveDown;
		return;
	}
	
	if (Entity->spawnflags & 4)				// Crawling?
		CurrentMove = &InsaneMoveRunCrawl;
	else // Else, mix it up
		CurrentMove = (random() <= 0.5) ? &InsaneMoveRunNormal : &InsaneMoveRunInsane;
}


void CInsane::Pain (edict_t *other, float kick, int damage)
{
	int	l,r;

//	if (self->health < (self->max_health / 2))
//		self->state.skinnum = 1;

	if (level.framenum < Entity->pain_debounce_time)
		return;

	Entity->pain_debounce_time = level.framenum + 30;

	// Paril
	// As much as I hate this, this needs to stay
	// until further notice.

	// START SHIT
	r = 1 + (rand()&1);
	if (Entity->health < 25)
		l = 25;
	else if (Entity->health < 50)
		l = 50;
	else if (Entity->health < 75)
		l = 75;
	else
		l = 100;
	PlaySoundFrom (Entity, CHAN_VOICE, SoundIndex (Q_VarArgs("player/male/pain%i_%i.wav", l, r)), 1, ATTN_IDLE);
	// END SHIT

	// Don't go into pain frames if crucified.
	if (Entity->spawnflags & 8)
	{
		CurrentMove = &InsaneMoveStruggleCross;			
		return;
	}
	
	if  ( ((Entity->state.frame >= FRAME_crawl1) && (Entity->state.frame <= FRAME_crawl9)) || ((Entity->state.frame >= FRAME_stand99) && (Entity->state.frame <= FRAME_stand160)) )
		CurrentMove = &InsaneMoveCrawlPain;
	else
		CurrentMove = &InsaneMoveStandPain;
}

void CInsane::OnGround ()
{
	CurrentMove = &InsaneMoveDown;
}

void CInsane::CheckDown ()
{
	if (Entity->spawnflags & 32)				// Always stand
		return;
	if (random() < 0.3)
		CurrentMove = (random() < 0.5) ? &InsaneMoveUpToDown : &InsaneMoveJumpDown;
}

void CInsane::CheckUp ()
{
	// If Hold_Ground and Crawl are set
	if ( (Entity->spawnflags & 4) && (Entity->spawnflags & 16) )
		return;
	if (random() < 0.5)
		CurrentMove = &InsaneMoveUpToDown;
}

void CInsane::Stand ()
{
	if (Entity->spawnflags & 8)			// If crucified
	{
		CurrentMove = &InsaneMoveCross;
		AIFlags |= AI_STAND_GROUND;
	}
	// If Hold_Ground and Crawl are set
	else if ( (Entity->spawnflags & 4) && (Entity->spawnflags & 16) )
		CurrentMove = &InsaneMoveDown;
	else
		CurrentMove = (random() < 0.5) ? &InsaneMoveStandNormal : &InsaneMoveStandInsane;
}

void CInsane::Dead ()
{
	if (Entity->spawnflags & 8)
		Entity->flags |= FL_FLY;
	else
	{
		Vec3Set (Entity->mins, -16, -16, -24);
		Vec3Set (Entity->maxs, 16, 16, -8);
		Entity->movetype = MOVETYPE_TOSS;
	}
	Entity->svFlags |= SVF_DEADMONSTER;
	NextThink = 0;
	gi.linkentity (Entity);
}


void CInsane::Die (edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
{
	if (Entity->health <= Entity->gib_health)
	{
		PlaySoundFrom (Entity, CHAN_VOICE, SoundIndex ("misc/udeath.wav"), 1, ATTN_IDLE);
		for (int n= 0; n < 2; n++)
			ThrowGib (Entity, gMedia.Gib_Bone[0], damage, GIB_ORGANIC);
		for (int n= 0; n < 4; n++)
			ThrowGib (Entity, gMedia.Gib_SmallMeat, damage, GIB_ORGANIC);
		ThrowHead (Entity, gMedia.Gib_Head[1], damage, GIB_ORGANIC);
		Entity->deadflag = DEAD_DEAD;
		return;
	}

	if (Entity->deadflag == DEAD_DEAD)
		return;

	PlaySoundFrom (Entity, CHAN_VOICE, SoundIndex(Q_VarArgs("player/male/death%i.wav", (rand()%4)+1)), 1, ATTN_IDLE);

	Entity->deadflag = DEAD_DEAD;
	Entity->takedamage = DAMAGE_YES;

	if (Entity->spawnflags & 8)
		Dead ();
	else
	{
		if ( ((Entity->state.frame >= FRAME_crawl1) && (Entity->state.frame <= FRAME_crawl9)) || ((Entity->state.frame >= FRAME_stand99) && (Entity->state.frame <= FRAME_stand160)) )		
			CurrentMove = &InsaneMoveCrawlDeath;
		else
			CurrentMove = &InsaneMoveStandDeath;
	}
}


/*QUAKED misc_insane (1 .5 0) (-16 -16 -24) (16 16 32) Ambush Trigger_Spawn CRAWL CRUCIFIED STAND_GROUND ALWAYS_STAND
*/
void CInsane::Spawn ()
{
	SoundFist = SoundIndex ("insane/insane11.wav");
	SoundShake = SoundIndex ("insane/insane5.wav");
	SoundMoan = SoundIndex ("insane/insane7.wav");
	SoundScream[0] = SoundIndex ("insane/insane1.wav");
	SoundScream[1] = SoundIndex ("insane/insane2.wav");
	SoundScream[2] = SoundIndex ("insane/insane3.wav");
	SoundScream[3] = SoundIndex ("insane/insane4.wav");
	SoundScream[4] = SoundIndex ("insane/insane6.wav");
	SoundScream[5] = SoundIndex ("insane/insane8.wav");
	SoundScream[6] = SoundIndex ("insane/insane9.wav");
	SoundScream[7] = SoundIndex ("insane/insane10.wav");

	Entity->movetype = MOVETYPE_STEP;
	Entity->solid = SOLID_BBOX;
	Entity->state.modelIndex = ModelIndex("models/monsters/insane/tris.md2");

	Vec3Set (Entity->mins, -16, -16, -24);
	Vec3Set (Entity->maxs, 16, 16, 32);

	Entity->health = 100;
	Entity->gib_health = -50;
	Entity->mass = 300;

	AIFlags |= AI_GOOD_GUY;

	gi.linkentity (Entity);

	if (Entity->spawnflags & 16)				// Stand Ground
		AIFlags |= AI_STAND_GROUND;

	CurrentMove = &InsaneMoveStandNormal;

	if (Entity->spawnflags & 8)					// Crucified ?
	{
		Vec3Set (Entity->mins, -16, 0, 0);
		Vec3Set (Entity->maxs, 16, 8, 32);
		Entity->flags |= FL_NO_KNOCKBACK;
		FlyMonsterStart ();
	}
	else
	{
		WalkMonsterStart ();
	}
	Entity->state.skinNum = rand()%3;
}
