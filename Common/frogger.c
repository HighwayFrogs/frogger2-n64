/*

	This file is part of Frogger2, (c) 1997 Interactive Studios Ltd.


	File		: frogger.c
	Programmer	: James Healey
	Date		: 23/03/99

----------------------------------------------------------------------------------------------- */


#define F3DEX_GBI_2

#include <ultra64.h>

#include "incs.h"

VECTOR *pointOfInterest;
float	pOIDistance = 20000.0;

char frogModel[4][16] = 
{
	"frogger.obe",
	"toad.obe",
	"femfrog.obe",
	"frogger.obe"
};

//----------------------------------------------------------------------------//
//----- GLOBALS --------------------------------------------------------------//
//----------------------------------------------------------------------------//

PLAYER player[MAX_FROGS];

ACTOR2 *frog[MAX_FROGS]					= {0,0,0,0};
ACTOR2 *hat[MAX_FROGS]					= {0,0,0,0};

SPRITEOVERLAY *sprHeart[3]		= { NULL,NULL,NULL};
SPECFX *frogTrail[MAX_FROGS] = {NULL,NULL,NULL,NULL};

long NUM_FROGS = 1;

float CROAK_SOUND_RANGE			= 400;

float croakRadius				= 15;
float croakVelocity				= 1.1F;
char croakFade					= 255;
char croakFadeDec				= 16;
char croakR						= 255;
char croakG						= 255;
char croakB						= 255;

float globalFrogScale			= 0.6F;

TEXTURE *frogEyeOpen,*frogEyeClosed;

//----------------------------------------------------------------------------//
//----- FUNCTIONS ------------------------------------------------------------//
//----------------------------------------------------------------------------//

void CreateFrogActor (GAMETILE *where, char *name,long p)
{
	ACTOR2 *me;

	me = frog[p] = CreateAndAddActor(name,0,0,200.0,INIT_ANIMATION | INIT_SHADOW,0,0);
/*	
	hat[p] = CreateAndAddActor("hat-top.obe",0,0,200.0,INIT_ANIMATION | INIT_SHADOW,0,0);
	
	if (hat[p])
	{
		actList = actList->next;
		actList->prev = 0;
	}
*/
	me->actor->shadow->radius = 30;
	me->actor->shadow->alpha = 191;
//	me->flags	|= ACTOR_DRAW_ALWAYS;
	me->flags	= ACTOR_DRAW_ALWAYS;
	
	tongue[p].flags = TONGUE_NONE | TONGUE_IDLE;
	
	InitActorAnim (me->actor);
	AnimateActor  (me->actor,FROG_ANIM_DANCE1,YES,NO,0.75F,0,0);

	me->actor->scale.v[0] = globalFrogScale;	//0.09;
	me->actor->scale.v[1] = globalFrogScale;	//0.09;
	me->actor->scale.v[2] = globalFrogScale;	//0.09;
	
	SetFroggerStartPos(where,p);

	me->action.healthPoints	= 3;
	me->action.frogon		= -1;
	me->action.frogunder		= -1;

	me->radius				= 22.0F;
}

void CreateFrogger(unsigned char createFrogActor,unsigned char createFrogOverlays )
{
	int i;

	if(createFrogActor)
	{
		for (i=0; i<NUM_FROGS; i++)
			if (gTStart[i])
				CreateFrogActor (gTStart[i],frogModel[i],i);
			else
				CreateFrogActor (gTStart[0],frogModel[i],i);
		
		for (i=0; i<NUM_FROGS; i++)
			frog[i]->draw = 0;
	}

	if (NUM_FROGS == 1)
	{
		if(createFrogOverlays)
		{
			int f;
			// get health icons ready
			i = 3;
			while(i--)
			{
				sprHeart[i] = CreateAndAddSpriteOverlay((short)(20+(i*10)), 205, "heart001.bmp",
					16,16,192,ANIMATION_FORWARDS | ANIMATION_CYCLE);
				for (f = 2; f <= 10; f++)
				{
					char fname[13];
					sprintf(fname, "heart%03d.bmp", f);
					AddFrameToSpriteOverlay(sprHeart[i], fname);
				}
				SetSpriteOverlayAnimSpeed(sprHeart[i],1.0F);
				sprHeart[i]->animTime = (i * 3);
			}
		}
	}
	else
	{
		int j,k = 0;
		for (j=0; j<NUM_FROGS; j++)
		{
			i = 3;
			k = !k;
			while(i--)
			{
				sprHeart[i+j*3] = CreateAndAddSpriteOverlay((
					(short)(j>1)?270:20)+(i*10),(short)(k?35:205),
					"mplaypts.bmp",32,32,192,0);
			}
		}
		
	}


}
