/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: path.c
	Programmer	: Andrew Eder
	Date		: 16/3/99

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI_2

#include <ultra64.h>

#include "incs.h"

typedef struct TAG_PATHNODESTORE
{
	PATHNODE *nodes;
	struct TAG_PATHNODESTORE *next;
} PATHNODESTORE;

PATHNODESTORE *pathNodeList = NULL;

/*	--------------------------------------------------------------------------------
	Function		: AddPathNodesToList
	Purpose			: Adds an array of pathnodes to a global list
	Parameters		: PATHNODE *first, int count
	Returns			: 
*/

void AddPathNodesToList(PATHNODE *first)
{
	PATHNODESTORE *newNode;

	newNode = JallocAlloc(sizeof(PATHNODESTORE), NO, "pnode_s");
	newNode->nodes = first;
	newNode->next = pathNodeList;
	pathNodeList = newNode;
}

/*	--------------------------------------------------------------------------------
	Function		: FreePathList
	Purpose			: Free the global list of PATHs
	Parameters		: void
*/
void FreePathList(void)
{
	PATHNODESTORE *node, *next;

	for (node = pathNodeList; node; node = next)
	{
		next = node->next;
		JallocFree((UBYTE**)&node->nodes);
		JallocFree((UBYTE**)&node);
	}

	pathNodeList = NULL;
}

/*	--------------------------------------------------------------------------------
	Function		: GetPositionForPathNode
	Purpose			: gets position for specified path node
	Parameters		: VECTOR *,PATHNODE *
	Returns			: void
	Info			: 
*/
void GetPositionForPathNode(VECTOR *vecPos,PATHNODE *pNode)
{
	vecPos->v[X] = pNode->worldTile->centre.v[X] + (pNode->worldTile->normal.v[X] * pNode->offset);
	vecPos->v[Y] = pNode->worldTile->centre.v[Y] + (pNode->worldTile->normal.v[Y] * pNode->offset);
	vecPos->v[Z] = pNode->worldTile->centre.v[Z] + (pNode->worldTile->normal.v[Z] * pNode->offset);
}


/*	--------------------------------------------------------------------------------
	Function		: GetPositionForPathNodeOffset2
	Purpose			: gets position for specified path node using offset 2
	Parameters		: VECTOR *,PATHNODE *
	Returns			: void
	Info			: 
*/
void GetPositionForPathNodeOffset2(VECTOR *vecPos,PATHNODE *pNode)
{
	vecPos->v[X] = pNode->worldTile->centre.v[X] + (pNode->worldTile->normal.v[X] * pNode->offset2);
	vecPos->v[Y] = pNode->worldTile->centre.v[Y] + (pNode->worldTile->normal.v[Y] * pNode->offset2);
	vecPos->v[Z] = pNode->worldTile->centre.v[Z] + (pNode->worldTile->normal.v[Z] * pNode->offset2);
}


/*	--------------------------------------------------------------------------------
	Function		: SetAllPathNodesToSetSpeed
	Purpose			: sets all nodes in specified path to specified speed
	Parameters		: PATH *,float
	Returns			: void
	Info			: 
*/
void SetAllPathNodesToSetSpeed(PATH *pPath,float speed)
{
	int i = pPath->numNodes;
	while(i--)
		pPath->nodes[i].speed = speed;
}


/*	--------------------------------------------------------------------------------
	Function		: SetAllPathNodesToSetWaitTime
	Purpose			: sets all nodes in specified path to specified wait time
	Parameters		: PATH *,short
	Returns			: void
	Info			: 
*/
void SetAllPathNodesToSetWaitTime(PATH *pPath,short pause)
{
	int i = pPath->numNodes;
	while(i--)
		pPath->nodes[i].waitTime = pause;
}


/*	--------------------------------------------------------------------------------
	Function		: CalcDirectionVectorFromNodeToNode
	Purpose			: calculates the direction vector from one node to another
	Parameters		: VECTOR *,PATHNODE *,PATHNODE *
	Returns			: void
	Info			: 
*/
void CalcDirectionVectorFromNodeToNode(VECTOR *result,PATHNODE *fromNode,PATHNODE *toNode)
{
	VECTOR vecFrom,vecTo;

	GetPositionForPathNode(&vecFrom,fromNode);
	GetPositionForPathNode(&vecTo,toNode);

	SubVector(result,&vecTo,&vecFrom);
	MakeUnit(result);
}


/*	--------------------------------------------------------------------------------
	Function		: AssignRandomSpeedsToPathNodes
	Purpose			: assigns random speed values to path nodes
	Parameters		: PATH *,float,float
	Returns			: void
	Info			: 
*/
void AssignRandomSpeedsToPathNodes(PATH *path,float minSpeed,float maxSpeed)
{
	int i = path->numNodes;
	while(i--)
		path->nodes[i].speed = Random(maxSpeed) + minSpeed;
}


/*	--------------------------------------------------------------------------------
	Function		: AssignRandomWaitTimesToPathNodes
	Purpose			: assigns random wait times to path nodes
	Parameters		: PATH *,short,short
	Returns			: void
	Info			: 
*/
void AssignRandomWaitTimesToPathNodes(PATH *path,short minPause,float maxPause)
{
	int i = path->numNodes;
	while(i--)
		path->nodes[i].waitTime = (short)(Random(maxPause)+minPause);
}

/*	--------------------------------------------------------------------------------
	Function		: ScalePathSpeed
	Purpose			: Scales the speed of each path node by a given amount
	Parameters		: PATH *, float
	Returns			: void
*/

void ScalePathSpeed(PATH *path, float scale)
{
	int i;
	PATHNODE *n;
	for (i = path->numNodes, n = path->nodes; i; i--, n++)
		n->speed *= scale;
}