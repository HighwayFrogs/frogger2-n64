/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: collect.c
	Programmer	: Andrew Eder
	Date		: 11/27/98

----------------------------------------------------------------------------------------------- */

#include <ultra64.h>

#include "incs.h"


GARIBLIST garibList;


/*	--------------------------------------------------------------------------------
	Function		: CheckTileForCollectable
	Purpose			: checks frogs position for a collectable item
	Parameters		: 
	Returns			: void
	Info			: used to check if Frogger has jumped on a collectable item
*/
void CheckTileForCollectable(GAMETILE *tile, long pl)
{
	GARIB *garib;
	int i;
	float dist;
	VECTOR *check;
	
	// check current tile for a garib
	for(garib = garibList.head.next, i = garibList.count-1; garib != &garibList.head; garib = garib->next, i--)
	{
		// Location of actual position of garib depends on type
		if( garib->fx && garib->type == EXTRAHEALTH_GARIB ) check = &garib->fx->act[pl]->actor->pos;
		else check = &garib->pos;

		// process only garibs in visual range
		dist = DistanceBetweenPointsSquared(check, &frog[pl]->actor->pos );

		if( dist > ACTOR_DRAWDISTANCEINNER)
			continue;

		// Also don't pickup garibs that the frog is trying to tongue
		if( garib == (GARIB *)tongue[pl].thing )
			continue;

		// Now we can actually check for collection
		if( dist < PICKUP_RADIUS_SQUARED)
		{
			PickupCollectable(garib,pl);
			return;
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function		: ProcessCollectables
	Purpose			: processes collectables scattered across the many varied worldscapes...
	Parameters		: none
	Returns			: void
	Info			:
*/
void ProcessCollectables()
{
	UpdateGaribs();
	
	// update players (for spawn counters / score bonus)
	if(player[0].spawnTimer && gameState.multi == SINGLEPLAYER)
	{
		player[0].spawnTimer--;
		if(!player[0].spawnTimer)
			player[0].spawnScoreLevel = 1;
	}
}


/*	--------------------------------------------------------------------------------
	Function		: 
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/
void PickupCollectable(GARIB *garib, int pl)
{
	SPECFX *fx;
	VECTOR seUp;
	int i;

	switch(garib->type)
	{
		case SPAWN_GARIB:
			if( gameState.multi != SINGLEPLAYER && multiplayerMode == MULTIMODE_BATTLE )
			{
				mpl[pl].trail++;
				mpl[pl].score++;
			}
			else
			{
				if(player[pl].spawnTimer)
				{
					VECTOR m;
					// increase player score bonus
					if(player[pl].spawnScoreLevel < 5)
						player[pl].spawnScoreLevel++;

					XfmPoint (&m,&garib->pos);
				}

				player[pl].spawnTimer = SPAWN_SCOREUPTIMER;

				if(player[pl].spawnScoreLevel == 5)
				{
					if( (fx = CreateAndAddSpecialEffect( FXTYPE_SPARKLYTRAIL, &garib->pos, &seUp, 50, 4, 0, 6 )) )
					{
						SetFXColour(fx,0,255,255);
						fx->gravity = 0.15;
					}
				}

//				player[pl].score += (player[pl].spawnScoreLevel * 10);
				player[pl].numSpawn++;

				if (player[pl].numSpawn>100)
				{
					player[pl].numSpawn = 0;
					player[pl].numCredits++;
				}
			}

			// we need to get the up vector for this collectable...
			SetVector(&seUp,&upVec);
			RotateVectorByQuaternion(&seUp,&seUp,&frog[pl]->actor->qRot);

			if( (fx = CreateAndAddSpecialEffect( FXTYPE_SPARKLYTRAIL, &garib->pos, &seUp, 20, 2, 0, 2 )) )
			{
				SetFXColour(fx,255,255,255);
				fx->gravity = 0.2;
			}
			if( (fx = CreateAndAddSpecialEffect( FXTYPE_SPARKLYTRAIL, &garib->pos, &seUp, player[pl].spawnScoreLevel * 5, player[pl].spawnScoreLevel, 0, 3 )) )
			{
				SetFXColour(fx,255,255,0);
				fx->gravity = 0.1;
			}

			PlaySample( genSfx[GEN_COLLECT_COIN], &garib->pos, 0, SAMPLE_VOLUME, -1 );
			break;

		case EXTRAHEALTH_GARIB:
			if( player[pl].lives < 99 )
				player[pl].lives++;

			if( (fx = CreateAndAddSpecialEffect( FXTYPE_SPARKLYTRAIL, &garib->fx->act[0]->actor->pos, &garib->fx->normal, 50, 3, 0, 5 )) )
			{
				SetFXColour( fx, 10, 200, 10 );
				fx->gravity = 0.07;
			}
			if( (fx = CreateAndAddSpecialEffect( FXTYPE_SPARKLYTRAIL, &garib->fx->act[0]->actor->pos, &garib->fx->normal, 40, 2.5, 0, 6 )) )
			{
				SetFXColour( fx, 220, 220, 220 );
				fx->gravity = 0.07;
			}
			if( (fx = CreateAndAddSpecialEffect( FXTYPE_SPARKLYTRAIL, &garib->fx->act[0]->actor->pos, &garib->fx->normal, 30, 2, 0, 7 )) )
			{
				SetFXColour( fx, 10, 200, 10 );
				fx->gravity = 0.07;
			}
			if( (fx = CreateAndAddSpecialEffect( FXTYPE_SPARKLYTRAIL, &garib->fx->act[0]->actor->pos, &garib->fx->normal, 20, 1.5, 0, 8 )) )
			{
				SetFXColour( fx, 220, 220, 220 );
				fx->gravity = 0.07;
			}

			DeallocateFX( garib->fx, 1 );
			break;

		case EXTRALIFE_GARIB:
			if( player[pl].lives < 100 )
				player[pl].lives++;
			break;

		case AUTOHOP_GARIB:
			GTInit( &player[pl].autohop, 10 );
			break;

		case LONGTONGUE_GARIB:
			GTInit( &player[pl].longtongue, 30 );
			tongue[pl].radius = TONGUE_RADIUSLONG;
			break;

		case QUICKHOP_GARIB:
			GTInit( &player[pl].quickhop, 10 );
			break;

		case INVULNERABILITY_GARIB:
			GTInit( &player[pl].safe, 10 );
			break;
	}

	if( gameState.multi != SINGLEPLAYER && multiplayerMode == MULTIMODE_BATTLE )
		numMultiItems--;
	
	SparkleCoins();
	SubGarib(garib);
}


/*	--------------------------------------------------------------------------------
	Function		: InitGaribList
	Purpose			: initialises the garib linked list
	Parameters		: 
	Returns			: void
	Info			: 
*/
void InitGaribList()
{
	garibList.count = 0;
	garibList.total = 0;
	garibList.head.next = garibList.head.prev = &garibList.head;
}


/*	--------------------------------------------------------------------------------
	Function		: AddGarib
	Purpose			: adds a garib to the garib list
	Parameters		: GARIB *
	Returns			: void
	Info			: 
*/
void AddGarib(GARIB *garib)
{
	if(garib->next == NULL)
	{
		garibList.count++;
		garibList.total++;

		garib->prev = &garibList.head;
		garib->next = garibList.head.next;
		garibList.head.next->prev = garib;
		garibList.head.next = garib;
	}
}


/*	--------------------------------------------------------------------------------
	Function		: SubGarib
	Purpose			: removes a garib from the garib list
	Parameters		: GARIB *
	Returns			: void
	Info			: 
*/
void SubGarib(GARIB *garib)
{
	if(garib->next == NULL || garibList.count<=0 )
	{
		dprintf"GARIB ERROR: invalid SubGarib()\n"));
		return;
	}

	if( garib->type != EXTRAHEALTH_GARIB && garib->sprite )
		DeallocateSprites( garib->sprite, 1 );

	garib->prev->next = garib->next;
	garib->next->prev = garib->prev;
	garib->next = NULL;
	garibList.count--;

	JallocFree((UBYTE **)&garib);
}


/*	--------------------------------------------------------------------------------
	Function		: FreeGaribLinkedList
	Purpose			: frees the garib linked list
	Parameters		: 
	Returns			: void
	Info			: 
*/
void FreeGaribList()
{
	GARIB *cur,*next;

	if(garibList.count <= 0)
		return;

	dprintf"Freeing linked list : GARIB : (%d elements)\n",garibList.count));

	while( garibList.count )
		SubGarib(garibList.head.next);
}


/*	--------------------------------------------------------------------------------
	Function		: CreateNewGarib
	Purpose			: creates a new garib
	Parameters		: VECTOR,int
	Returns			: GARIB *
	Info			: 
*/
GARIB *CreateNewGarib(VECTOR pos,int type)
{
	GARIB *garib;

	garib = (GARIB *)JallocAlloc(sizeof(GARIB),YES,"garib");
	AddGarib(garib);

	SetVector(&garib->pos,&pos);

	garib->type = type;
	garib->active = 1;
	garib->scale = 0;
	garib->scaleAim = 1;

	if( garib->type == EXTRAHEALTH_GARIB )
	{
		SPECFX *fx;
		if( (garib->fx = CreateAndAddSpecialEffect( FXTYPE_HEALTHFLY, &garib->pos, &upVec, 1, 1, 0.06, 0 )) )
		{
//			garib->fx->gravity = -0.5;
			if( (fx = CreateAndAddSpecialEffect( FXTYPE_TRAIL, &garib->fx->act[0]->actor->pos, &upVec, 5, 0.95, 0.00, 0.6 )) )
			{
				fx->follow = garib->fx->act[0]->actor;
				SetFXColour( fx, 0, 128, 255 );
			}
		}
	}
	else
	{
		// initialise garib sprite
		garib->sprite = AllocateSprites(1);
		SetVector(&garib->sprite->pos,&pos);
		FindTexture( &garib->sprite->texture, UpdateCRC("coin0001.bmp"), NO );

		garib->sprite->r = 255;
		garib->sprite->g = 255;
		garib->sprite->b = 255;
		garib->sprite->a = 127;
		garib->sprite->scaleX = garib->sprite->scaleY = 0;

		// ok - make the spawn garib a rotating sprite - ANDYE
		garib->sprite->flags |= SPRITE_FLAGS_ROTATE;
		garib->sprite->angle = 1 / (1 + (rand() % 10));
		garib->sprite->angleInc = 0.1f;
		
#ifndef PC_VERSION
		garib->sprite->offsetX = -garib->sprite->texture->sx / 2;
		garib->sprite->offsetY = -garib->sprite->texture->sy / 2;
		garib->sprite->flags &= -1 - SPRITE_TRANSLUCENT;
#else
		garib->sprite->offsetX = -16;
		garib->sprite->offsetY = -16;
		garib->sprite->flags &= -1 - SPRITE_TRANSLUCENT;
#endif

		garib->sprite->flags |= SPRITE_TRANSLUCENT;
		garib->sprite->a = 200;
	}

	return garib;
}


/*	--------------------------------------------------------------------------------
	Function		: UpdateGaribs
	Purpose			: updates the garibs
	Parameters		: 
	Returns			: void
	Info			: 
*/
void UpdateGaribs()
{
	GARIB *garib = NULL;
	float scale;
	VECTOR fwd;
	VECTOR actualPos;

	// update garib scales and calculate distance from Frog
	for(garib = garibList.head.next; garib != &garibList.head; garib = garib->next)
	{
		if(garib->active == NO)
			continue;
		else if(garib->active > 1)
			garib->active--;

		scale = garib->scale;
		scale -= (scale - garib->scaleAim) / 5;
		garib->scale = scale;

		if( garib->sprite )
		{
			garib->sprite->scaleX = 64;
			garib->sprite->scaleY = 64;

			// rotate the little blighters
			garib->sprite->angle += (garib->sprite->angleInc * gameSpeed);
		}

		// Drop Garibs.............
		if ( garib->gameTile )
		{			
			SetVector( &actualPos, &garib->gameTile->centre );
			actualPos.v[Y] += 20;
			SubVector( &fwd, &actualPos, &garib->pos );
			MakeUnit( &fwd );
			garib->pos.v[X] += ( fwd.v[X] * garib->dropSpeed );
			garib->pos.v[Y] += ( fwd.v[Y] * garib->dropSpeed );
			garib->pos.v[Z] += ( fwd.v[Z] * garib->dropSpeed );
			SetVector( &garib->sprite->pos, &garib->pos );
		}
	}
}


void DropGaribToTile(GARIB *garib, GAMETILE *tile, float dropSpeed)
{
	garib->gameTile = tile;
	garib->dropSpeed = dropSpeed;
}
