/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.
	
	  
	File		: eventfuncs.c
	Programmer	: Jim Hubbard
	Date		: 30/06/99

	Info		: Trigger and event functions for the event system, and setups for 
					level specific events.

----------------------------------------------------------------------------------------------- */

#include <ultra64.h>
#include "incs.h"



/*----- [ TRIGGER FUNCTIONS ] ------------------------------------------------------------------*/

/*	--------------------------------------------------------------------------------
	Function 	: EnemyOnTile
	Purpose 	: Check if an enemy in the trigger structure is on the given tile
	Parameters 	: Pointer to trigger structure
	Returns 	: Boolean
	Info 		:
*/
int EnemyOnTile( TRIGGER *trigger )
{
	ENEMY *nme = (ENEMY *)trigger->data[0];
	GAMETILE *tile = (GAMETILE *)trigger->data[1];

	if( nme->inTile == tile )
		return 1;

	return 0;
}


/*	--------------------------------------------------------------------------------
	Function 	: FrogOnTile
	Purpose 	: Check if a frog in the trigger structure is on the given tile
	Parameters 	: Pointer to trigger structure
	Returns 	: Boolean
	Info 		:
*/
int FrogOnTile( TRIGGER *trigger )
{
	int frognum = ( int ) trigger->data[0];
	GAMETILE *tile = (GAMETILE *)trigger->data[1];

	if( currTile[frognum] == tile )
		return 1;

	return 0;
}


/*	--------------------------------------------------------------------------------
	Function 	: FrogOnPlatform
	Purpose 	: Check if a frog in the trigger structure is on the given platform
	Parameters 	: Pointer to trigger structure
	Returns 	: Boolean
	Info 		:
*/
int FrogOnPlatform( TRIGGER *trigger )
{
	int pl = (int)trigger->data[0];
	int id = (int)trigger->data[1];

	if (currPlatform[pl] && (currPlatform[pl]->uid == id))
		return 1;

	return 0;
}


/*	--------------------------------------------------------------------------------
	Function 	: ActorWithinRadius
	Purpose 	: Check if the actor in the trigger structure is within a distance of a point
	Parameters 	: Pointer to trigger structure
	Returns 	: Boolean
	Info 		:
*/
int ActorWithinRadius( TRIGGER *trigger )
{
	ACTOR *actor = (ACTOR *)trigger->data[0];
	VECTOR *pos = (VECTOR *)trigger->data[1];
	float radius = *(float *)trigger->data[2];

	if( DistanceBetweenPoints(&actor->pos, pos) < radius )
		return 1;

	return 0;
}


/*	--------------------------------------------------------------------------------
	Function 	: OnTimeout
	Purpose 	: Trigger when a certain time is reached
	Parameters 	: Pointer to trigger structure
	Returns 	: Boolean
	Info 		:
*/

int OnTimeout( TRIGGER *trigger )
{
	unsigned long time = (int)trigger->data[0];

	if (actFrameCount >= time)
	{
		(unsigned long)trigger->data[0] = actFrameCount + (unsigned long)trigger->data[1];
		return TRUE;
	}
	else
		return FALSE;
}


/*	--------------------------------------------------------------------------------
	Function 	: OnTrigger
	Purpose 	: Trigger if another trigger fires
	Parameters 	: Pointer to trigger structure
	Returns 	: Boolean
	Info 		:
*/
int OnTrigger( TRIGGER *trigger )
{
	TRIGGER *t = (TRIGGER *)trigger->data[0];

	if( t->flags & TRIGGER_FIRED )
		return 1;

	return 0;
}


/*	--------------------------------------------------------------------------------
	Function 	: LogicalAND
	Purpose 	: Trigger if an arbitrary number of other triggers are all true
	Parameters 	: Pointer to trigger structure
	Returns 	: Boolean
	Info 		: First parameter is the number of other triggers, the rest are pointers to triggers
*/
int LogicalAND( TRIGGER *trigger )
{
	int numT = (int)trigger->data[0];
	long i;
	TRIGGER *t;

	for( i=0; i<numT; i++ )
	{
		t = (TRIGGER *)trigger->data[i+1];
		if( !t->func(t) )
			return 0;
	}

	return 1;
}

/*	--------------------------------------------------------------------------------
	Function 	: LogicalOR
	Purpose 	: Trigger if any one of an arbitrary number of other triggers is true
	Parameters 	: Pointer to trigger structure
	Returns 	: Boolean
	Info 		: First parameter is the number of other triggers, the rest are 
					pointers to triggers
*/
int LogicalOR( TRIGGER *trigger )
{
	int numT = (int)trigger->data[0];
	long i;
	TRIGGER *t;

	for( i=0; i<numT; i++ )
	{
		t = (TRIGGER *)trigger->data[i+1];
		if( t->func(t) )
			return 1;
	}

	return 0;
}

int EnemyAtFlag(TRIGGER *trigger)
{
	int id, flag;
	ENEMY *e;
	
	id = (int)trigger->data[0];
	flag = (int)trigger->data[1];

	for(e = enemyList.head.next; e != &enemyList.head; e = e->next)
	{
		if ((!id || e->uid == id) && (e->path->fromNode == flag))
			return 1;
	}
	return 0;
}

int PlatformAtFlag(TRIGGER *trigger)
{
	int id, flag;
	PLATFORM *p;
	
	id = (int)trigger->data[0];
	flag = (int)trigger->data[1];

	for(p = platformList.head.next; p != &platformList.head; p = p->next)
	{
		if ((!id || p->uid == id) && (p->path->fromNode == flag))
			return 1;
	}
	return 0;
}

int PathAtFlag( TRIGGER *trigger )
{
	PATH *path;
	int flag;

	path = (PATH*)trigger->data[0];
	flag = *(int*)trigger->data[1];

	return (path->fromNode == flag);
}


int LevelIsOpen( TRIGGER *trigger )
{
	int world, level;

	world = (int)trigger->data[0];
	level = (int)trigger->data[1];

	if (level == 0xFF)
		return worldVisualData[world].worldOpen;
	else
		return (worldVisualData[world].worldOpen && worldVisualData[world].levelVisualData[level].levelOpen);
}

/*	--------------------------------------------------------------------------------
	Function 	: BitCheck
	Purpose 	: Checks if a bit is set in a variable
	Parameters 	: 
	Returns 	: 
	Info 		: data[0] is a pointer to an existing integer.
					data[1] is a value that is and-ed with the first thing
*/
int BitCheck( TRIGGER *trigger )
{
	return( *(int *)trigger->data[0] & (int)trigger->data[1] );
}


int FrogIsDead(TRIGGER *trigger)
{
	return (player[(int)trigger->data[0]].frogState & FROGSTATUS_ISDEAD) != 0;	// mm, convoluted.
}

/*----- [ EVENT FUNCTIONS ] --------------------------------------------------------------------*/

/*	--------------------------------------------------------------------------------
	Function 	: ChangeActorScale
	Purpose 	: Alters the scale property of the actor
	Parameters 	: Pointer to event structure
	Returns 	: 
	Info 		:
*/
void ChangeActorScale( EVENT *event )
{
	ACTOR *actor = (ACTOR *)event->data[0];
	VECTOR *scale = (VECTOR *)event->data[1];

	actor->scale = *scale;
}


/*	--------------------------------------------------------------------------------
	Function 	: AssignFloatToFloat
	Purpose 	: Assigns a number to any float.
	Parameters 	: Pointer to event structure
	Returns 	: 
	Info 		:
*/
void AssignFloatToFloat( EVENT *event )
{
	float *ass = (float *)event->data[0];
	float val = *(float *)event->data[1];

	*ass = val;
}


/*	--------------------------------------------------------------------------------
	Function 	: AssignIntToInt
	Purpose 	: Assigns a number to any float.
	Parameters 	: Pointer to event structure
	Returns 	: 
	Info 		:
*/
void AssignIntToInt( EVENT *event )
{
	int *ass = (int *)event->data[0];
	int val = *(int *)event->data[1];

	*ass = val;
}


/*	--------------------------------------------------------------------------------
	Function 	: TogglePlatformMove
	Purpose 	: Make a floating platform halt or continue its path
	Parameters 	: Pointer to event structure
	Returns 	: 
	Info 		: Sets wait timer on next tile to achieve its purpose
*/
void TogglePlatformMove( EVENT *event )
{
	PLATFORM *plt = (PLATFORM *)event->data[0];

	plt->isWaiting = (plt->isWaiting) ? 0 : -1;
}


/*	--------------------------------------------------------------------------------
	Function 	: ToggleEnemyMove
	Purpose 	: Make an enemy halt or continue its path
	Parameters 	: Pointer to event structure
	Returns 	: 
	Info 		: Sets speed of enemies
*/
void ToggleEnemyMove( EVENT *event )
{
	ENEMY *nme = (ENEMY *)event->data[0];

	nme->isWaiting = (nme->isWaiting) ? 0 : -1;
}


/*	--------------------------------------------------------------------------------
	Function 	: ToggleTileLink
	Purpose 	: Change whether it is possible to go from one tile to another
	Parameters 	: Pointer to event structure
	Returns 	: 
	Info 		: Sets/unsets TILESTATE_BARRED flag in tile->state.
*/
void ToggleTileLink( EVENT *event )
{
	GAMETILE *tile = (GAMETILE *)event->data[0];

	tile->state = (tile->state==TILESTATE_BARRED) ? TILESTATE_NORMAL : TILESTATE_BARRED;
}


/*	--------------------------------------------------------------------------------
	Function 	: PlaySFX
	Purpose 	: Play a sound, either sourceless or at a point
	Parameters 	: Pointer to event structure
	Returns 	: 
	Info 		: 
*/
void PlaySFX( EVENT *event )
{
	short snum = *(int *)event->data[0];
	short vol = *(int *)event->data[1];
	short pitch = *(int *)event->data[2];
	VECTOR *point = (VECTOR *)event->data[3];
	float radius = *(float *)event->data[4];

	PlaySample( FindSample(snum), point, radius, vol, -1/*pitch*/ );
}

/*	--------------------------------------------------------------------------------
	Function 	: EvAnimateActor
	Purpose 	: Animate an actor
	Parameters 	: Pointer to event structure
*/

void EvAnimateActor( EVENT *event )
{
	ACTOR *actor = (ACTOR *)event->data[0];
	short anim = *(int*)event->data[1];
	short flags = *(int*)event->data[2];
	float speed = *(float*)event->data[3];

	AnimateActor(actor, anim, (char)(flags & 1), (char)(flags & 2), speed, 0, 0);
}


/*	--------------------------------------------------------------------------------
	Function 	: ChangeLevel
	Purpose 	: Go to a new level in the game
	Parameters 	: Pointer to event structure
	Returns 	: 
	Info 		: I hope this deallocs everything...
*/
void ChangeLevel( EVENT *event )
{
	int wNum = *(int *)event->data[0],
		lNum = *(int *)event->data[1];

	player[0].worldNum = wNum;
	player[0].levelNum = lNum;
	
/*	StopDrawing ( "changelevel" );
	FreeAllLists();
	InitLevel ( player[0].worldNum, player[0].levelNum );
	StartDrawing ( "changelevel" );
	frameCount = 0;
	NUM_FROGS = 1;*/


	gameState.mode = LEVELCOMPLETE_MODE;
	GTInit( &modeTimer, 1 );
	showEndLevelScreen = 0;
}

void TeleportFrog( EVENT *event )
{
	int f = (int)event->data[0];
	GAMETILE *tile = (GAMETILE*)event->data[1];

	TeleportActorToTile(frog[f], tile, f);
//	CreateTeleportEffect(&frog[f]->actor->pos, &upVec, 255,255,255);
}
