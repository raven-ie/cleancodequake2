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
// cc_pathfinding.cpp
// Pathfinding algorithm for monsters
//

#include "cc_local.h"

std::vector<CPathNode*>		Closed, Open;

CPath::CPath () :
Start(NULL),
End(NULL),
Weight(0),
NumNodes(0)
{
};

CPath::CPath (CPathNode *Start, CPathNode *End) :
Start(Start),
End(End)
{
	Weight = NumNodes = 0;
	CreatePath ();
};

bool CPath::NodeIsClosed (CPathNode *Node)
{
	for (std::vector<CPathNode*>::iterator it = Closed.begin(); it < Closed.end(); it++ )
	{
		CPathNode *Check = *it;

		if (Check == Node)
			return true;
	}
	return false;
};

void CPath::AddNodeToClosed (CPathNode *Node)
{
	Closed.push_back (Node);
}

void CPath::RemoveFromClosed (CPathNode *Node)
{
	for (size_t i = 0; i < Closed.size(); i++)
	{
		if (Closed[i] == Node)
		{
			Closed.erase (Closed.begin()+i);
			break;
		}
	}
}

bool CPath::NodeIsOpen (CPathNode *Node)
{
	for (std::vector<CPathNode*>::iterator it = Open.begin(); it < Open.end(); it++ )
	{
		CPathNode *Check = *it;

		if (Check == Node)
			return true;
	}
	return false;
};

void CPath::AddNodeToOpen (CPathNode *Node)
{
	Open.push_back (Node);
}

void CPath::RemoveFromOpen (CPathNode *Node)
{
	for (size_t i = 0; i < Open.size(); i++)
	{
		if (Open[i] == Node)
		{
			Open.erase (Open.begin()+i);
			break;
		}
	}
}

// Returns the closest node to End from Node (by searching Children)
CPathNode *CPath::GetShortestConnector (CPathNode *Node, uint32 &weight)
{
	// Do we only have one childs?
	if (Node->Children.size() == 1)
	{
		CPathNode *Child = (NodeIsClosed(Node->Children[0]) ? NULL : Node->Children[0]);

		if (Child)
			weight = Child->GetNodeWeight(Node);
		return Child;
	}
	// Do we have.. no childrens?
	else if (Node->Children.size() == 0)
		return NULL;

	uint32 ShortestWeight = UINT_MAX, CurrentWeight = 0;
	CPathNode *Shortest = NULL;

	for (std::vector<CPathNode*>::iterator it = Node->Children.begin(); it < Node->Children.end(); it++ )
	{
		CPathNode *Child = *it;

		if ((CurrentWeight = Child->GetNodeWeight(Node)) < ShortestWeight)
		{
			ShortestWeight = CurrentWeight;
			Shortest = Child;

			// If the shortest is closed, we un-close it for later.
			if (NodeIsClosed(Shortest))
				RemoveFromClosed (Shortest);
		}
	}

	weight = ShortestWeight;
	return Shortest;
}

uint32 CPath::DistToGoal (CPathNode *Node)
{
	vec3_t sub;
	Vec3Subtract (Node->Origin, End->Origin, sub);
	return Vec3Length(sub);
}

void CPath::CreatePath ()
{
/*	CPathNode *Node;
	uint32 AddWeight;
	Node = Start;
	// Find the shortest weighted node to our target connected
	// to our current spot
	while ((Node = GetShortestConnector(Node, AddWeight)) != End)
	{
		// Uh oh.
		if (Node == NULL)
			break;

		Weight += AddWeight;
		NumNodes ++;

		// If we're not a junction (ie, have more than 2 children), add us to the closed list
		AddNodeToClosed(Node);
		Path.push_back (Node);
	}*/
	Closed.clear ();
	Open.clear ();
	Start->G = 0;
	Start->H = DistToGoal (Start);
	Start->F = Start->G + Start->H;
	Start->Parent = NULL;
	Open.push_back (Start);
	while (Open.size())
	{
		CPathNode *n = Open[Open.size()-1];
		Open.pop_back();

		if (n == End)
		{
			// Construct path
			gi.dprintf ("Path!\n");

			while (n)
			{
				NumNodes++;
				Weight += n->G;
				Path.push_back (n);
				n = n->Parent;
			}
			return;
		}
		for (size_t i = 0; i < n->Children.size(); i++)
		{
			vec3_t sub;
			CPathNode *nl = n->Children[i];
			Vec3Subtract (nl->Origin, n->Origin, sub);
			uint32 newg = n->G + Vec3Length(sub);
			if (NodeIsOpen(nl))
			{
				if (nl->G <= newg)
					continue;
				RemoveFromOpen (nl);
			}
			else if (NodeIsClosed(nl))
			{
				if (nl->G <= newg)
					continue;
				RemoveFromClosed (nl);
			}
			nl->Parent = n;
			nl->G = newg;
			nl->H = DistToGoal(nl);
			nl->F = nl->G + nl->H;

			AddNodeToOpen (nl);
		}
		AddNodeToClosed (n);
	}
	return;
};

#define MAX_NODES 512
uint32 lastId = 0;

CPathNode *NodeList[MAX_NODES];

void Cmd_Node_f (edict_t *ent);

void InitNodes ()
{
	Cmd_AddCommand ("node",				Cmd_Node_f);
}

edict_t *PlayerNearby (vec3_t origin, int distance)
{
	edict_t *ent = NULL;

	while ((ent = findradius(ent, origin, distance)) != NULL)
	{
		if (ent && ent->client)
			return ent;
	}
	return NULL;
}

void PrintVerboseNodes (vec3_t origin, uint32 numNode)
{
	edict_t *ent = NULL;

	while ((ent = findradius(ent, origin, 25)) != NULL)
	{
		if (ent && ent->client)
			gi.cprintf (ent, PRINT_HIGH, "You are very close to node %i\n", numNode);
	}
}

CPathNode *DropNode (edict_t *ent)
{
	NodeList[lastId] = new CPathNode(ent->s.origin, NODE_REGULAR, lastId);
	NodeList[lastId]->Ent = G_Spawn();

	NodeList[lastId]->Ent->s.modelIndex = ModelIndex("models/objects/grenade2/tris.md2");
	Vec3Copy (NodeList[lastId]->Origin, NodeList[lastId]->Ent->s.origin);
	gi.linkentity(NodeList[lastId]->Ent);
	gi.cprintf (ent, PRINT_HIGH, "Node %i added\n", lastId);

	return NodeList[lastId++];
}

void ConnectNode (uint32 ID1, uint32 ID2);
void RunPlayerNodes (edict_t *ent)
{
	if (!ent->client->resp.DroppingNodes)
		return;

	if (!ent->client->resp.LastNode)
		ent->client->resp.LastNode = DropNode (ent);
	else
	{
		vec3_t sub;
		Vec3Subtract (ent->client->resp.LastNode->Origin, ent->s.origin, sub);
		if (Vec3Length(sub) > 64)
		{
			CPathNode *NewNode = DropNode(ent);
			ConnectNode (NewNode->NodeId, ent->client->resp.LastNode->NodeId);
			ent->client->resp.LastNode = NewNode;
		}
	}
}

void RunNodes()
{
	for (uint32 i = 0; i < lastId; i++)
	{
		if (PlayerNearby(NodeList[i]->Origin, 250))
		{
			for (std::vector<CPathNode*>::iterator it = NodeList[i]->Children.begin(); it < NodeList[i]->Children.end(); it++ )
			{
				CPathNode *Child = *it;
				TempEnts.Trails.BFGLaser (NodeList[i]->Origin, Child->Origin);
			}

			PrintVerboseNodes (NodeList[i]->Origin, i);
		}
	}
}

void AddNode (edict_t *ent, vec3_t origin)
{
	NodeList[lastId] = new CPathNode(origin, NODE_REGULAR, lastId);
	NodeList[lastId]->Ent = G_Spawn();

	NodeList[lastId]->Ent->s.modelIndex = ModelIndex("models/objects/grenade2/tris.md2");
	Vec3Copy (NodeList[lastId]->Origin, NodeList[lastId]->Ent->s.origin);
	gi.linkentity(NodeList[lastId]->Ent);
	gi.cprintf (ent, PRINT_HIGH, "Node %i added\n", lastId++);
}

void ConnectNode (uint32 ID1, uint32 ID2)
{
	if (!NodeList[ID1] || !NodeList[ID2])
		return;

	NodeList[ID1]->Children.push_back (NodeList[ID2]);
	NodeList[ID2]->Children.push_back (NodeList[ID1]);
}

void CalculateTestPath (edict_t *ent, uint32 IDStart, uint32 IDEnd)
{
/*	std::vector <CPath*> PathList;
	Closed.clear ();

	CPath *Test = NULL;
	while (!Test)
	{
		Test = new CPath (NodeList[IDStart], NodeList[IDEnd]);

		if (Test->NumNodes)
		{
			PathList.push_back (Test);
			Test = NULL;
		}
		else
		{
			delete Test;
			break;
		}
	}

	uint32 ShortestPathByWeight = UINT_MAX;
	int ShortestPathByWeightIndex = 0;
	uint32 ShortestPathByNodes = UINT_MAX;
	int ShortestPathByNodesIndex = 0;

	for (size_t i = 0; i < PathList.size(); i++)
	{
		Test = PathList[i];

		if (Test->Weight < ShortestPathByWeight)
		{
			ShortestPathByWeight = Test->Weight;
			ShortestPathByWeightIndex = i;
		}
		if (Test->NumNodes < ShortestPathByNodes)
		{
			ShortestPathByNodes = Test->NumNodes;
			ShortestPathByNodesIndex = i;
		}
	}

	if (PathList.size())
	{
		CPath *BestByWeight = PathList[ShortestPathByWeightIndex];
		CPath *BestByNodes = PathList[ShortestPathByNodesIndex];

		for (size_t i = 0; i < PathList.size(); i++)
		{
			for (std::vector<CPathNode*>::iterator it = PathList[i]->Path.begin(); it < PathList[i]->Path.end(); it++ )
			{
				CPathNode *Check = *it;

				if (i == ShortestPathByWeightIndex)
					Check->Ent->s.modelIndex = ModelIndex("models/objects/grenade/tris.md2");
				else if (i == ShortestPathByNodesIndex)
					Check->Ent->s.modelIndex = ModelIndex("models/objects/flash/tris.md2");
				else if (Check->Ent->s.modelIndex == ModelIndex("models/objects/grenade2/tris.md2"))
					Check->Ent->s.modelIndex = ModelIndex("models/objects/debris3/tris.md2");
			}
		}

		//gi.cprintf (ent, PRINT_HIGH, "%u nodes to path (%u)\n", Test->NumNodes, Test->Weight);
		gi.cprintf (ent, PRINT_HIGH, "%i path possibilit%s\nBEST BY WEIGHT: (%u n, %u w)\nBEST BY NODES: (%u n, %u w)\n",
									PathList.size(), (PathList.size() > 1) ? "ies" : "y",
									BestByWeight->NumNodes, BestByWeight->Weight,
									BestByNodes->NumNodes, BestByNodes->Weight);
	}
	else
		gi.cprintf (ent, PRINT_HIGH, "no path possibilities\n");

	for (size_t i = 0; i < PathList.size(); i++)
	{
		Test = PathList[i];
		delete Test;
	}*/
	CPath *Test = new CPath (NodeList[IDStart], NodeList[IDEnd]);

	gi.cprintf (ent, PRINT_HIGH, "BEST: (%u n, %u w)\n",
									Test->NumNodes, Test->Weight);

	for (size_t i = 0; i < Test->Path.size(); i++)
	{
		Test->Path[i]->Ent->s.modelIndex = ModelIndex("models/objects/grenade/tris.md2");
	}

	delete Test;

	Open.clear();
	Closed.clear();
}

void Cmd_Node_f (edict_t *ent)
{
	char *cmd = gi.argv(1);

	if (Q_stricmp(cmd, "drop") == 0)
		AddNode (ent, ent->s.origin);
	else if (Q_stricmp(cmd, "dropper") == 0)
	{
		ent->client->resp.LastNode = NULL;
		ent->client->resp.DroppingNodes = !ent->client->resp.DroppingNodes;
	}
	else if (Q_stricmp(cmd, "connect") == 0)
	{
		uint32 firstId = atoi(gi.argv(2));
		uint32 secondId = atoi(gi.argv(3));

		if (firstId > lastId)
		{
			gi.cprintf (ent, PRINT_HIGH, "Node %i doesn't exist!\n", firstId);
			return;
		}
		if (secondId > lastId)
		{
			gi.cprintf (ent, PRINT_HIGH, "Node %i doesn't exist!\n", secondId);
			return;
		}

		gi.cprintf (ent, PRINT_HIGH, "Connecting nodes %i and %i...\n", firstId, secondId);
		ConnectNode (firstId, secondId);
	}
	else if (Q_stricmp(cmd, "testpath") == 0)
	{
		uint32 firstId = atoi(gi.argv(2));
		uint32 secondId = atoi(gi.argv(3));

		if (firstId >= lastId)
		{
			gi.cprintf (ent, PRINT_HIGH, "Node %i doesn't exist!\n", firstId);
			return;
		}
		if (secondId >= lastId)
		{
			gi.cprintf (ent, PRINT_HIGH, "Node %i doesn't exist!\n", secondId);
			return;
		}

		gi.cprintf (ent, PRINT_HIGH, "Testing path %i and %i...\n", firstId, secondId);
		CalculateTestPath (ent, firstId, secondId);
	}
	else if (Q_stricmp(cmd, "clearstate") == 0)
	{
		for (uint32 i = 0; i < lastId; i++)
			NodeList[i]->Ent->s.modelIndex = ModelIndex("models/objects/grenade2/tris.md2");
	}
	else if (Q_stricmp(cmd, "settype") == 0)
	{
		if (!gi.argv(2) || (uint32)atoi(gi.argv(2)) >= lastId)
			return;

		CPathNode *Node = NodeList[atoi(gi.argv(2))];

		if (Q_stricmp(gi.argv(3), "door") == 0)
			Node->Type = NODE_DOOR;
		else if (Q_stricmp(gi.argv(3), "jump") == 0)
			Node->Type = NODE_PLATFORM;
	}
	else if (Q_stricmp(cmd, "linkwith") == 0)
	{
		if (!gi.argv(2) || (uint32)atoi(gi.argv(2)) >= lastId)
			return;

		CPathNode *Node = NodeList[atoi(gi.argv(2))];

		vec3_t end, forward;
		Angles_Vectors (ent->client->v_angle, forward, NULL, NULL);
		Vec3MA (ent->s.origin, 8192, forward, end);

		CTrace trace = CTrace(ent->s.origin, end, ent, CONTENTS_MASK_ALL);

		if (trace.ent && trace.ent->model[0] == '*')
		{
			Node->LinkedEntity = trace.ent;
			gi.dprintf ("Linked %u with %s\n", Node->NodeId, trace.ent->classname);
		}
	}
	else if (Q_stricmp(cmd, "monstergoal") == 0)
	{
		vec3_t end, forward;
		Angles_Vectors (ent->client->v_angle, forward, NULL, NULL);
		Vec3MA (ent->s.origin, 8192, forward, end);

		CTrace trace = CTrace(ent->s.origin, end, ent, CONTENTS_MASK_ALL);

		if (trace.ent && trace.ent->Monster)
		{
			trace.ent->Monster->P_CurrentGoalNode = NodeList[atoi(gi.argv(3))];
			trace.ent->Monster->P_CurrentNode = NodeList[atoi(gi.argv(2))];
			trace.ent->Monster->FoundPath ();
		}
	}
	else if (Q_stricmp (cmd, "move") == 0)
	{
		if (!gi.argv(2) || (uint32)atoi(gi.argv(2)) >= lastId)
			return;

		CPathNode *Node = NodeList[atoi(gi.argv(2))];

		float x = atof(gi.argv(3));
		float y = atof(gi.argv(4));
		float z = atof(gi.argv(5));

		Node->Origin[0] += x;
		Node->Origin[1] += y;
		Node->Origin[2] += z;

		Node->Ent->s.origin[0] += x;
		Node->Ent->s.origin[1] += y;
		Node->Ent->s.origin[2] += z;
		gi.linkentity(Node->Ent);
	}
}