/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: path.h
	Programmer	: Andrew Eder
	Date		: 16/3/99

----------------------------------------------------------------------------------------------- */

#ifndef PATH_H_INCLUDED
#define PATH_H_INCLUDED


#define NODE_RANGETHRESHOLD				10.0F


#define GET_PATHLASTNODE(x)		(x->numNodes - 1)


//----- [ PATH FLAGS ] -------------------------------------------------------------------------//

#define PATH_MAKENODETILEPTRS			(1 << 0)



//----- [ PATHNODE DATA STRUCTURE ] -----//

typedef struct TAGPATHNODE
{
	GAMETILE				*worldTile;				// ptr to underlying world tile
	float					offset;	 				// offset(1) from underlying world tile
	float					offset2;	 			// offset(2) from underlying world tile

	float					speed;					// speed at this node
	short					waitTime;				// pause time at this node

} PATHNODE;

//----- [ PATH DATA STRUCTURE ] -----//

typedef struct TAGPATH
{
	short					numNodes;				// number of nodes in path
	short					fromNode;				// from node
	short					toNode;					// node we're heading towards
	short					startNode;				// start node for this path
	unsigned long			startFrame;				// The start frame
	unsigned long			endFrame;				// The start frame
	PATHNODE				*nodes;					// actual path nodes

} PATH;


//----- [ FUNCTION PROTOTYPES ] -----//

void GetPositionForPathNode(VECTOR *vecPos,PATHNODE *pNode);
void GetPositionForPathNodeOffset2(VECTOR *vecPos,PATHNODE *pNode);
BOOL ActorHasArrivedAtNode(ACTOR2 *pAct,PATH *pPath,int nodeID);

void SetPathNodeSpeed(PATHNODE *pNode,float speed);
void SetPathNodeWaitTime(PATHNODE *pNode,short pause);
void SetAllPathNodesToSetSpeed(PATH *pPath,float speed);
void SetAllPathNodesToSetWaitTime(PATH *pPath,short pause);
float GetSpeedFromIndexedNode(PATH *path,short index);
short GetWaitTimeFromIndexedNode(PATH *path,short index);

void CalcDirectionVectorFromNodeToNode(VECTOR *result,PATHNODE *fromNode,PATHNODE *toNode);

void AssignRandomSpeedsToPathNodes(PATH *path,float minSpeed,float maxSpeed);
void AssignRandomWaitTimesToPathNodes(PATH *path,short minPause,float maxPause);

void ScalePathSpeed(PATH *path, float scale);

void AddPathNodesToList(PATHNODE*);
void FreePathList(void);

#endif
