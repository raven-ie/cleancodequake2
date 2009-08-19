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
// cc_bodyqueue.cpp
// Bodyqueue class
//

#include "cc_local.h"
#include <list>

// TODO: Is TouchableEntity needed?
class CBody : public CHurtableEntity, public CTouchableEntity, public CTossProjectile, public CThinkableEntity
{
public:
	class CBodyQueue *BodyQueueList; // Queue the body belongs to

	CBody ();
	CBody (int Index);

	void TossHead (int damage);

	void Pain (CBaseEntity *other, float kick, int damage);
	void Die (CBaseEntity *inflictor, CBaseEntity *attacker, int damage, vec3_t point);

	void Think	(); // Only done if we're a head

	bool Run ();
};

CBody::CBody () :
CBaseEntity(),
CHurtableEntity(),
CTouchableEntity(),
BodyQueueList(NULL)
{
};

CBody::CBody (int Index) :
CBaseEntity(Index),
CHurtableEntity(Index),
CTouchableEntity(Index),
BodyQueueList(NULL)
{
};

bool CBody::Run ()
{
	return CTossProjectile::Run();
}

void CBody::Pain (CBaseEntity *other, float kick, int damage)
{
}

void VelocityForDamage (int damage, vec3f &v);
void CBody::TossHead (int damage)
{
	if (rand()&1)
	{
		State.SetModelIndex (gMedia.Gib_Head[1]);
		State.SetSkinNum (1);		// second skin is player
	}
	else
	{
		State.SetModelIndex (gMedia.Gib_Skull);
		State.SetSkinNum (0);
	}

	State.SetFrame (0);
	SetMins (vec3f(-16, -16, 0));
	SetMaxs (vec3f(16, 16, 16));

	gameEntity->takedamage = false;
	SetSolid (SOLID_NOT);
	State.SetEffects (EF_GIB);
	State.SetSound (0);
	gameEntity->flags |= FL_NO_KNOCKBACK;

	backOff = 1.5f;
	vec3f vd;
	VelocityForDamage (damage, vd);
	vec3f vel = vec3f(gameEntity->velocity) + vd;
	gameEntity->velocity[0] = vel.X;
	gameEntity->velocity[1] = vel.Y;
	gameEntity->velocity[2] = vel.Z;

	NextThink = level.framenum + 100 + random()*100;

	Link ();
}

#define BODY_QUEUE_SIZE	8

class CBodyQueue
{
public:
	std::list<CBody*> OpenList; // A list of entity numbers that are currently waiting a body
	std::list<CBody*> ClosedList; // A list of entity numbers that are currently being used, in order of accessed.
	// If OpenList is empty, pop the first one off of ClosedList.

	// Creates the body queue
	CBodyQueue (int MaxSize);

	CBody *GrabFreeBody ();

	// Grabs a free body and uses it
	void CopyBodyToQueue (CPlayerEntity *Player);
};

CBodyQueue	*BodyQueue;

void CBody::Think ()
{
	// Take us out of the closed list
	BodyQueueList->ClosedList.remove (this);

	// Add us to the open list
	BodyQueueList->OpenList.push_back (this);

	// Disappear us
	State.SetModelIndex(0);
	State.SetEffects(0);
	SetSvFlags (SVF_NOCLIENT);
}

void CBody::Die (CBaseEntity *inflictor, CBaseEntity *attacker, int damage, vec3_t point)
{
	if (gameEntity->health < -40)
	{
		PlaySound(CHAN_BODY, SoundIndex ("misc/udeath.wav"));
		for (int n = 0; n < 4; n++)
			CGibEntity::Spawn (this, gMedia.Gib_SmallMeat, damage, GIB_ORGANIC);
			
		vec3f origin = State.GetOrigin();
		origin.Z -= 16;
		State.SetOrigin(origin);
		TossHead (damage);
	}
}

CBody *CBodyQueue::GrabFreeBody ()
{
	// If there's anything in the open list..
	if (OpenList.size())
	{
		// Pop it off the front
		CBody *Body = OpenList.front();
		OpenList.pop_front();

		// Throw it in the closed list
		ClosedList.push_back (Body);
		return Body;
	}
	// Has to be something in closed list
	// Pop the first one off and return that.
	// This should, effectively, remove the last body.
	CBody *Body = ClosedList.front();
	ClosedList.pop_front();

	// Revision
	// Push this body to the end of the closed list so we get recycled last
	ClosedList.push_back (Body);
	return Body;
}

CBodyQueue::CBodyQueue (int MaxSize)
{
	// Add MaxSize entities to the body queue (reserved entities)
	for (int i = 0; i < MaxSize; i++)
	{
		CBody *Body = QNew (com_levelPool, 0) CBody ();
		Body->gameEntity->classname = "bodyque";
		Body->BodyQueueList = this;

		OpenList.push_back(Body);
	}
};

void BodyQueue_Init ()
{
	if (BodyQueue)
		QDelete BodyQueue;
	BodyQueue = QNew (com_levelPool, 0) CBodyQueue (BODY_QUEUE_SIZE);
}

void CopyToBodyQueue (CPlayerEntity *Player)
{
	BodyQueue->CopyBodyToQueue (Player);
}

void CBodyQueue::CopyBodyToQueue (CPlayerEntity *Player)
{
	// Grab a free body
	CBody *Body = GrabFreeBody();

	// Make sure it doesn't interpolate
	Body->State.SetEvent(EV_OTHER_TELEPORT);

	Player->Unlink();
	Body->Unlink();

	Body->State.SetSound (Player->State.GetSound());
	vec3f angles = Player->State.GetAngles();
	angles[0] = 0;
	Body->State.SetAngles (angles);
	Body->State.SetEffects (Player->State.GetEffects());
	Body->State.SetFrame (Player->State.GetFrame());
	Body->State.SetModelIndex (Player->State.GetModelIndex());
	Body->State.SetOldOrigin (Player->State.GetOldOrigin());
	Body->State.SetOrigin (Player->State.GetOrigin());
	Body->State.SetRenderEffects (Player->State.GetRenderEffects());
	Body->State.SetSkinNum (Player->State.GetSkinNum());

	Body->SetSvFlags (Player->GetSvFlags());
	Body->SetMins (Player->GetMins());
	Body->SetMaxs (Player->GetMaxs());
	Body->SetAbsMin (Player->GetAbsMin());
	Body->SetAbsMax (Player->GetAbsMax());
	Body->SetSize (Player->GetSize());
	Body->SetSolid (Player->GetSolid());
	Body->SetClipmask(Player->GetClipmask());
	CBaseEntity *owner;
	if ((owner = Player->GetOwner()) != 0)
		Body->SetOwner (owner);

	Body->backOff = 1.0f;
	Body->gameEntity->takedamage = true;

	// Implied that Player is a dead-head (lol)
	if (Player->gameEntity->takedamage == false)
		Body->TossHead (0);

	Body->Link();
}