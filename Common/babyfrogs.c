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
float BABY_SCALE = 0.4;
GAMETILE **bTStart;

int lastBabySaved	= -1;

SPRITEOVERLAY *babyIcons[NUM_BABIES];

unsigned long numBabies = 0;
unsigned long babiesSaved = 0;


void InitBabyList( unsigned char createOverlays )
{
	unsigned long i,j;

	lastBabySaved	= -1;
	babiesSaved		= 0;
	
	for (i=0; i<numBabies; i++)
	{
		babyList[i].isSaved = 0;
		babyList[i].baby = NULL;
		babyList[i].idle = 0;
		babyList[i].collect = 0;

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
			babyIcons[i] = CreateAndAddSpriteOverlay( ((234-15)-(i*15)),240-16,"00baby06.bmp",15,12,100,0 );

/*			char fn[] = "frog001.bmp\0";

			babyIcons[i] = CreateAndAddSpriteOverlay( ((234-15)-(i*15)),240-16,fn,10,10,91,ANIMATION_FORWARDS|ANIMATION_CYCLE);

			// Add animation frames
			for (j=2; j<=8; j++)
			{
				sprintf(fn, "frog%03d.bmp", j);
				AddFrameToSpriteOverlay(babyIcons[i], fn);
			}
*/		}
	}
}

void ResetBabies( )
{
	unsigned long i;

	for( i=0; i<numBabies; i++ )
	{
		babyList[i].isSaved = 0;
		babyList[i].baby = NULL;
		babyList[i].idle = 0;
		babyList[i].collect = 0;
		babiesSaved = 0;
		lastBabySaved = -1;
	}
}


/*	--------------------------------------------------------------------------------
	Function		: PickupBabyFrog
	Purpose			: called when a baby frog is picked up...
	Parameters		: ACTOR2
	Returns			: truth
	Info			:
*/
int PickupBabyFrog( ACTOR2 *baby, GAMETILE *tile )
{
	unsigned long i;
	SPECFX *fx;
	VECTOR pos, up;
	SparkleBabies();

	for( i=0; i<numBabies; i++ ) if( babyList[i].baby == baby ) break;

	if( i==numBabies ) return FALSE;

	// check if this baby has been collected
	if(babyList[i].isSaved)
		return FALSE;

	babyList[i].isSaved	= 1;
	babyList[i].collect = 45;

	if( babiesSaved == numBabies-1 )
		baby->draw = 0;

	lastBabySaved = i;

	babyIcons[i]->a = 255;
	babyIcons[i]->animSpeed = 1.0F;
	babiesSaved++;

	// If baby references a placeholder then set the respawn position to be the placeholder
	if( baby->value1 )
	{
		ENEMY *respawn = GetEnemyFromUID( baby->value1 );
		if( respawn )
			gTStart[0] = respawn->path->nodes->worldTile;
		else
			gTStart[0] = tile;
	}
	else // Else respawn on the current baby tile
	{
		gTStart[0] = tile;
	}

	startCamFacing = camFacing[0];
	startFrogFacing = frogFacing[0];

//	player[0].score += (1500 * babiesSaved);

	PlaySample( genSfx[GEN_COLLECT_BABY], &baby->actor->pos, 0, SAMPLE_VOLUME, -1 );

	// Ring 1
	SetVector( &up, &tile->normal );
	ScaleVector( &up, 20 );
	AddVector( &pos, &up, &baby->actor->pos );
	fx = CreateAndAddSpecialEffect( FXTYPE_WAKE, &pos, &tile->normal, 16, 1, 0.02, 1 );
	SetFXColour( fx, babyList[i].fxColour[0], babyList[i].fxColour[1], babyList[i].fxColour[2] );

	// Ring 2
	AddToVector( &pos, &up );
	fx = CreateAndAddSpecialEffect( FXTYPE_WAKE, &pos, &tile->normal, 8, 0.5, 0.02, 1.2 );
	SetFXColour( fx, babyList[i].fxColour[0], babyList[i].fxColour[1], babyList[i].fxColour[2] );

	// Ring 3
	AddToVector( &pos, &up );
	fx = CreateAndAddSpecialEffect( FXTYPE_WAKE, &pos, &tile->normal, 4, 0.3, 0.02, 1.4 );
	SetFXColour( fx, babyList[i].fxColour[0], babyList[i].fxColour[1], babyList[i].fxColour[2] );
	
	// Ring 4
	AddToVector( &pos, &up );
	fx = CreateAndAddSpecialEffect( FXTYPE_WAKE, &pos, &tile->normal, 2, 0.1, 0.02, 1.6 );
	SetFXColour( fx, babyList[i].fxColour[0], babyList[i].fxColour[1], babyList[i].fxColour[2] );

	// Ring 5
	AddToVector( &pos, &up );
	fx = CreateAndAddSpecialEffect( FXTYPE_WAKE, &pos, &tile->normal, 1, 0.05, 0.02, 1.8 );
	SetFXColour( fx, babyList[i].fxColour[0], babyList[i].fxColour[1], babyList[i].fxColour[2] );

	
	AnimateActor(frog[0]->actor,FROG_ANIM_DANCE4,NO,NO,0.3F,0,0);
	AnimateActor(frog[0]->actor,FROG_ANIM_BREATHE,YES,YES,0.4F,0,0);
   	
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
	int baby=-1;
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
				baby = i;
			}
		}
	}

	return baby;
}


/*	--------------------------------------------------------------------------------
	Function		: UpdateBabies
	Purpose			: Determine behaviour of baby frogs
	Parameters		: 
	Returns			: 
	Info			: Mope when far from frog, then do sequence of attention grabbing animations.
						Later, dynamically swap to lower poly model when far from camera.
*/
void UpdateBabies( )
{
	int i;
	ACTOR *act;
	ACTOR2 *baby;
	VECTOR frogV;
	QUATERNION q;
	long dist;
	float speed;

	for( i=0; ((i<numBabies) && (babyList[i].baby)); i++ )
	{
		baby = babyList[i].baby;
		act = baby->actor;

		if( !babyList[i].baby->draw ) continue;

		if( babyList[i].collect )
		{
			// Spin around and move up
			VECTOR fwd, up;
			QUATERNION q;

			SetVector( &up, &currTile[0]->normal );
			SetVector( (VECTOR *)&q, &up );
			q.w = (actFrameCount%360)*0.15;

			RotateVectorByRotation( &fwd, &inVec, &q );
			Orientate( &baby->actor->qRot, &fwd, &up );

			babyList[i].collect -= gameSpeed;

			if( babyList[i].collect < 1 )
			{
				SPECFX *fx;
				if( (fx = CreateAndAddSpecialEffect( FXTYPE_SPARKLYTRAIL, &babyList[i].baby->actor->pos, &up, 50, 3, 0, 5 )) )
				{
					SetFXColour( fx, babyList[i].fxColour[0], babyList[i].fxColour[1], babyList[i].fxColour[2] );
					SetVector( &fx->rebound->point, &currTile[0]->centre );
					fx->gravity = 0.07;
				}
				if( (fx = CreateAndAddSpecialEffect( FXTYPE_SPARKLYTRAIL, &babyList[i].baby->actor->pos, &up, 40, 2.5, 0, 6 )) )
				{
					SetFXColour( fx, 220, 220, 220 );
					SetVector( &fx->rebound->point, &currTile[0]->centre );
					fx->gravity = 0.07;
				}
				if( (fx = CreateAndAddSpecialEffect( FXTYPE_SPARKLYTRAIL, &babyList[i].baby->actor->pos, &up, 30, 2, 0, 7 )) )
				{
					SetFXColour( fx, babyList[i].fxColour[0], babyList[i].fxColour[1], babyList[i].fxColour[2] );
					SetVector( &fx->rebound->point, &currTile[0]->centre );
					fx->gravity = 0.07;
				}
				if( (fx = CreateAndAddSpecialEffect( FXTYPE_SPARKLYTRAIL, &babyList[i].baby->actor->pos, &up, 20, 1.5, 0, 8 )) )
				{
					SetFXColour( fx, 220, 220, 220 );
					SetVector( &fx->rebound->point, &currTile[0]->centre );
					fx->gravity = 0.07;
				}
				babyList[i].baby->draw = 0;
				babyList[i].collect = 0;
			}

			ScaleVector( &up, gameSpeed*3 );
			AddToVector( &baby->actor->pos, &up );

			continue;
		}

		SubVector( &frogV, &frog[0]->actor->pos, &act->pos );
		dist = MagnitudeSquared(&frogV);

		if( babyList[i].idle > 0 )
			babyList[i].idle -= gameSpeed;

		// Do animation dependent on distance from frog
		// Maybe store last distance too?
		if( dist < BABY_ACTIVE_RADIUS )
		{
			MakeUnit( &frogV );
			SetQuaternion(&q,&act->qRot);
			Orientate( &act->qRot, &frogV, &currTile[0]->normal );
			speed = 0.2*gameSpeed;
			if( speed > 0.999 ) speed = 0.999;
			QuatSlerp( &q, &act->qRot, speed, &act->qRot );

			if( babyList[i].idle < 1 )
			{
				if( dist < BABY_ACTIVE_RADIUS*0.25 )
				{
					AnimateActor( act, Random(3)+2, NO, YES, 0.4, 0,0 );
					StartAnimateActor( act, BABY_ANIM_HOP, YES, YES, 0.6, 0,0 );
				}
				else
				{
					// Randomly play attract animation
					AnimateActor( act, Random(3)+2, NO, YES, 0.4, 0,0 );
					AnimateActor( act, BABY_ANIM_SNOOZE, YES, YES, 0.4, 0,0 );
					babyList[i].idle = BABY_IDLE_TIME + Random(BABY_IDLE_TIME);
				}
			}
		}
		else StartAnimateActor( act, BABY_ANIM_SNOOZE, YES, YES, 0.4, 0,0 );
	}
}
