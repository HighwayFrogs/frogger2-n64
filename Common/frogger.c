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

FROGSTORE frogPool[FROG_NUMFROGS] = 
{
	{ "Frogger",	"frogger.obe",	"frogger.bmp",	1 },
	{ "Lillie",		"femfrog.obe",	"lillie.bmp",	1 },
	{ "Tad",		"tad.obe",		"babyfrog.bmp",	0 },
	{ "Swampy",		"swampy.obe",	"swampy.bmp",	0 },
	{ "Twee",		"twee.obe",		"twee.bmp",		1 },
	{ "Toad",		"toad.obe",		"wart.bmp",		1 },
	{ "Gnarl",		"frogger.obe",	"gnarly.bmp",	0 },
	{ "Funky"		"frogger.obe",	"funky.bmp",	0 },
	{ "Tank",		"tank.obe",		"robofrog.bmp",	0 },
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

float globalFrogScale			= 0.6F;

TEXTURE *frogEyeOpen,*frogEyeClosed;

//----------------------------------------------------------------------------//
//----- FUNCTIONS ------------------------------------------------------------//
//----------------------------------------------------------------------------//

void CreateFrogActor (GAMETILE *where, char *name,long p)
{
	ACTOR2 *me;

	me = frog[p] = CreateAndAddActor(name,0,0,200.0,INIT_ANIMATION | INIT_SHADOW);
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
	me->flags	= ACTOR_DRAW_ALWAYS;
	
	tongue[p].flags = TONGUE_NONE | TONGUE_IDLE;
	
	InitActorAnim (me->actor);
	AnimateActor  (me->actor,FROG_ANIM_DANCE1,YES,NO,0.75F,0,0);

	me->actor->scale.v[0] = globalFrogScale;
	me->actor->scale.v[1] = globalFrogScale;
	me->actor->scale.v[2] = globalFrogScale;
	
	SetFroggerStartPos(where,p);

	player[p].healthPoints	= 1;
	player[p].frogon		= -1;
	player[p].frogunder		= -1;

	me->radius				= 22.0F;
}

void CreateFrogger(unsigned char createFrogActor,unsigned char createFrogOverlays )
{
	int i, num;

	if(createFrogActor)
	{
		for (i=0; i<NUM_FROGS; i++)
		{
			num = player[i].character;
			if( num > FROG_NUMFROGS || !frogPool[num].active )
			{
				player[i].character = num = 0;
			}

			if (gTStart[i])
				CreateFrogActor (gTStart[i],frogPool[num].model,i);
			else
				CreateFrogActor (gTStart[0],frogPool[num].model,i);

			currPlatform[i] = NULL;
		}
	}

	InitCamera();
/*
	if (gameState.multi == SINGLEPLAYER)
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
	else if( multiplayerMode != MULTIMODE_BATTLE )
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
*/
}


/*	--------------------------------------------------------------------------------
	Function 	: FroggerIdleAnim
	Purpose 	: Choose and play an idle animation
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void FroggerIdleAnim( int i )
{
	GAMETILE *t = currTile[i]->tilePtrs[frogFacing[(i + 2) & 3]];

	// 2/3 chance of doing an idle based on the tile we're facing
	if (t && (Random(3) > 0))	
	{
		// Play a looking-over-the-edge animation when we're next to a drop or deadly tile
		if (t->state == TILESTATE_JOIN)
		{
			VECTOR v;
			float height;
			
			SubVector(&v, &t->centre, &currTile[i]->centre);
			height = DotProduct(&v, &currTile[i]->normal);

			if (height < -10)
			{
				AnimateActor(frog[i]->actor, FROG_ANIM_LOOKDOWN, NO, NO, 0.15f, 0, 0);
				AnimateActor(frog[i]->actor,FROG_ANIM_BREATHE,YES,YES,0.4F,0,0);
				return;
			}
		}
		else if (t->state == TILESTATE_DEADLY)
		{
			AnimateActor(frog[i]->actor, FROG_ANIM_LOOKDOWN, NO, NO, 0.15f, 0, 0);
			AnimateActor(frog[i]->actor,FROG_ANIM_BREATHE,YES,YES,0.4F,0,0);
			return;
		}
	}

	// otherwise, play a normal idle
	switch (Random(4))
	{
		case 0:
			AnimateActor(frog[i]->actor,FROG_ANIM_SCRATCHHEAD,NO,NO,0.4F,0,0);
			if (Random(10)>6)
				AnimateActor(frog[i]->actor,FROG_ANIM_SCRATCHHEAD,NO,YES,0.4F,0,0);
			AnimateActor(frog[i]->actor,FROG_ANIM_BREATHE,YES,YES,0.4F,0,0);
			break;
		case 1:
			AnimateActor(frog[i]->actor,FROG_ANIM_DANCE1,YES,NO,0.3F,0,0);
			break;
		case 2:
			AnimateActor(frog[i]->actor,FROG_ANIM_DANCE2,YES,NO,0.3F,0,0);
			break;
		case 3:
			AnimateActor(frog[i]->actor,FROG_ANIM_DANCE3,NO,NO,0.3F,0,0);
			if (Random(10)>6)
				AnimateActor(frog[i]->actor,FROG_ANIM_DANCE1,YES,YES,0.3F,0,0);
			else
				AnimateActor(frog[i]->actor,FROG_ANIM_BREATHE,YES,YES,0.4F,0,0);
			break;
		case 4:
			AnimateActor(frog[i]->actor,FROG_ANIM_BREATHE,YES,YES,0.4F,0,0);
			break;
	}
}

