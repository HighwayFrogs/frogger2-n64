/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: actor.h
	Programmer	: Matthew Cloy
	Date		: 11/11/98

----------------------------------------------------------------------------------------------- */

#ifndef ACTOR_H_INCLUDED
#define ACTOR_H_INCLUDED


//----- [ ACTOR2 DRAWING FLAGS ] -----//

#define	ACTOR_DRAW_NEVER			(1 << 0)	//1
#define ACTOR_DRAW_CULLED			(1 << 1)	//2
#define ACTOR_DRAW_ALWAYS			(1 << 2)	//4
#define ACTOR_DRAW_LAST				(1 << 3)	//8
#define ACTOR_WATER					(1 << 4)	//16
#define ACTOR_SLIDYTEX				(1 << 5)	//32
#define ACTOR_SLIDYTEX2				(1 << 6)	//64
#define ACTOR_MODGETEX				(1 << 7)	//128


#define LOOKAT_ANYWHERE		1
#define LOOKAT_2D			2


//------------------------------------------------------------------------------------------------

#include "general.h"

typedef struct TAGACTION
{	
	TIMER safe;
	TIMER stun;
	TIMER dead;

	TIMER isCroaking;
	TIMER isOnFire;

	char frogon;
	char frogunder;

	char healthPoints;
	unsigned char deathBy;

} ACTION;


typedef struct TAGACTOR2
{
	ACTOR			*actor;
	ACTION			action;	
	
	struct TAGACTOR2	*next,*prev;

	char			draw;
	unsigned short	flags;
	unsigned long	effects;
	unsigned long	fxCount;
	float			radius;
	float			angle;
	float			animSpeed;
	float			value1;

	float			distanceFromFrog;		// <--- will need changing !!!

	float			speed;
	float			offset;

} ACTOR2;


extern unsigned long ACTOR_DRAWDISTANCEINNER;
extern unsigned long ACTOR_DRAWDISTANCEOUTER;

extern int objectMatrix;

extern ACTOR2 *actList;
extern ACTOR2 *globalLevelActor;

extern char dprintbuf[255];

extern int uniqueActorCRC[];
extern char numUniqueActors;

ACTOR2 *CreateAndAddActor(char *name,float cx,float cy,float cz,int initFlags,float offset,int startNode);
void DrawActorList();
void FreeActorList();

void AddObjectsSpritesToSpriteList(OBJECT *obj,short flags);
void RemoveObjectSprites(OBJECT *obj,BOOL free);
void FreeObjectSprites(OBJECT *obj);

void MakeUniqueActor(ACTOR *actor,int type);
void RemoveUniqueActor(ACTOR *actor,int type);

void MakeUniqueVtx(OBJECT_CONTROLLER *objC);
OBJECT *MakeUniqueObject(OBJECT *object);
void RemoveUniqueObject(OBJECT *object);

void ResetUniqueActorList();

void ActorLookAt( ACTOR *act, VECTOR *at, long flags );
void Orientate(QUATERNION *me, VECTOR *fd, VECTOR *mfd, VECTOR *up);
void SitAndFace(ACTOR2 *me, GAMETILE *tile, long fFacing);

#endif