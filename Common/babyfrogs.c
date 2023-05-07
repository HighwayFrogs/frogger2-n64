/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: babyfrog.c
	Programmer	: James Healey
	Date		: 17/03/99

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI_2

#include <ultra64.h>

#include "incs.h"

BABY babyList[NUM_BABIES];

GAMETILE **bTStart;

int nearestBaby		= -1;
int lastBabySaved	= -1;

SPRITEOVERLAY *babyIcons[NUM_BABIES];

unsigned long numBabies = 0;
unsigned long babiesSaved = 0;

const char* baby_filenames[NUM_BABIES] = { "frog", "ylfrg", "blfrog", "prfrg", "rdfrg" };


void InitBabyList( unsigned char createOverlays )
{
	unsigned long i,j;

	nearestBaby		= -1;
	lastBabySaved	= -1;
	babiesSaved		= 0;
	
	for (i=0; i<numBabies; i++)
	{
		babyList[i].isSaved = 0;
		babyList[i].baby = NULL;

		// determine baby colour and set values accordingly
		switch(i)
		{
			case 0:
				// green frog
				babyList[i].fxColour[R] = 0;
				babyList[i].fxColour[G] = 255;
				babyList[i].fxColour[B] = 0;
				babyList[i].fxColour[A] = 255;
				break;
			case 1:
				// yellow frog
				babyList[i].fxColour[R] = 255;
				babyList[i].fxColour[G] = 255;
				babyList[i].fxColour[B] = 0;
				babyList[i].fxColour[A] = 255;
				break;
			case 2:
				// blue frog
				babyList[i].fxColour[R] = 0;
				babyList[i].fxColour[G] = 0;
				babyList[i].fxColour[B] = 255;
				babyList[i].fxColour[A] = 255;
				break;
			case 3:
				// purple frog
				babyList[i].fxColour[R] = 255;
				babyList[i].fxColour[G] = 0;
				babyList[i].fxColour[B] = 255;
				babyList[i].fxColour[A] = 255;
				break;
			case 4:
				// red frog
				babyList[i].fxColour[R] = 255;
				babyList[i].fxColour[G] = 0;
				babyList[i].fxColour[B] = 0;
				babyList[i].fxColour[A] = 255;
				break;
		}
	}

	if ( createOverlays )
	{
		for(i=0; i<numBabies; i++)
		{
			char fn[14];

			sprintf(fn, "%s001.bmp", baby_filenames[i]);

			
			babyIcons[i] = CreateAndAddSpriteOverlay( (280-(i*20)),205,fn,16,16,91,ANIMATION_FORWARDS|ANIMATION_CYCLE);

			// Add animation frames

			for (j=2; j<=8; j++)
			{
				sprintf(fn, "%s%03d.bmp", baby_filenames[i], j);
				AddFrameToSpriteOverlay(babyIcons[i], fn);
			}
		}
	}
}

void ResetBabies( )
{
	int i;

	for( i=0; i<numBabies; i++ )
	{
		babyList[i].isSaved = 0;
		babyList[i].baby = NULL;
		babiesSaved = 0;
		lastBabySaved = -1;
		nearestBaby = -1;
	}
}


/*	--------------------------------------------------------------------------------
	Function		: PickupBabyFrog
	Purpose			: called when a baby frog is picked up...
	Parameters		: ACTOR2
	Returns			: truth
	Info			:
*/
int PickupBabyFrog( ACTOR2 *baby )
{
	int i;

	for( i=0; i<numBabies; i++ ) if( babyList[i].baby == baby ) break;

	if( i==numBabies ) return FALSE;

	// check if this baby has been collected
	if(babyList[i].isSaved)
		return FALSE;

	babyList[i].isSaved	= 1;
	baby->draw = 0;

	lastBabySaved = i;

	babyIcons[i]->a = 255;
	babyIcons[i]->animSpeed = 1.0F;
	babiesSaved++;
	
	// make baby position the new start position ?
	if(carryOnBabies)
		gTStart[0] = &firstTile[(int)baby->value1]; //bTStart[i];

	player[0].score += (1500 * babiesSaved);
	babySaved = 30;
//	PlaySample(17, &baby->actor->pos, 192, 128);

	return TRUE;
}


/*	--------------------------------------------------------------------------------
	Function		: GetNearestBabyFrog
	Purpose			: determines the nearest baby frog
	Parameters		: 
	Returns			: ACTOR2 *
	Info			: used for, say, determining
*/
int GetNearestBabyFrog()
{
	int nearest = -1;
	float t,distance = 99999;
	int i;

	i = numBabies;
	while(i--)
	{
		if(!babyList[i].isSaved && babyList[i].baby )
		{
			t = DistanceBetweenPoints(&frog[0]->actor->pos,&babyList[i].baby->actor->pos);
			if(t < distance)
			{
				distance	= t;
				nearest		= i;
			}
		}
	}

//	if(nearest && (distance < CROAK_SOUND_RANGE))
//		PlaySample( GEN_BABY_FROG, &babies[nearest]->actor->pos, 0, 255, (short)(128+(128-(distance/4))) );

	return i;
}
