/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: collect.c
	Programmer	: Andrew Eder
	Date		: 11/27/98

----------------------------------------------------------------------------------------------- */

#include <ultra64.h>

#include "incs.h"

#define storeListLength  16

SCREENSPAWN *spawnList = NULL;

unsigned long autoHop			= 0;
unsigned long longTongue		= 0;
unsigned long superFrog			= 0;
unsigned long croakFloat		= 0;

unsigned char garibStoreList [4][storeListLength ];

unsigned char garibListPos = 0;

int reset = 0;

//----- [ TEMPLATES FOR GARIB SPRITE ANIMATIONS ] -----//

SPRITE_ANIMATION_TEMPLATE garibAnimation[NUM_GARIB_TYPES] =
{
	{ &spriteFrameList[SPAWN_ANIM],0,SPRITE_ANIM_CYCLE_RANDOM,255,255,GARIB_SCALE,GARIB_SCALE,SPRITE_TRANSLUCENT },
	{ &spriteFrameList[EXTRAHEALTH_ANIM],0,SPRITE_ANIM_CYCLE,255,255,GARIB_SCALE,GARIB_SCALE,0 },
	{ &spriteFrameList[EXTRALIFE_ANIM],0,SPRITE_ANIM_CYCLE,255,255,GARIB_SCALE,GARIB_SCALE,0 },
	{ &spriteFrameList[AUTOHOP_ANIM],0,SPRITE_ANIM_CYCLE,255,255,GARIB_SCALE,GARIB_SCALE,0 },
	{ &spriteFrameList[LONGHOP_ANIM],0,SPRITE_ANIM_CYCLE,255,255,GARIB_SCALE,GARIB_SCALE,0 },
	{ &spriteFrameList[LONGTONGUE_ANIM],0,SPRITE_ANIM_CYCLE,255,255,GARIB_SCALE,GARIB_SCALE,0 },
	{ &spriteFrameList[WHOLEKEY_ANIM],0,SPRITE_ANIM_CYCLE,255,255,GARIB_SCALE,GARIB_SCALE,0 },
	{ &spriteFrameList[HALFLKEY_ANIM],0,SPRITE_ANIM_CYCLE,255,255,GARIB_SCALE,GARIB_SCALE,0 },
	{ &spriteFrameList[HALFRKEY_ANIM],0,SPRITE_ANIM_CYCLE,255,255,GARIB_SCALE,GARIB_SCALE,0 },
};


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
	
	// check current tile for a garib
	for(garib = garibCollectableList.head.next, i = garibCollectableList.numEntries-1;
		garib != &garibCollectableList.head; garib = garib->next, i--)
	{
		// process only garibs in visual range
		if( DistanceBetweenPointsSquared(&garib->sprite.pos, &frog[0]->actor->pos ) > ACTOR_DRAWDISTANCEINNER)
			continue;

		if( DistanceBetweenPointsSquared(&garib->sprite.pos, &frog[0]->actor->pos ) < PICKUP_RADIUS_SQUARED)
		{
			garibStoreList[player[0].levelNum-3][i / 8] &= ~(1 << (i & 7));
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
	
	// update state of powerups
	if(autoHop)
		autoHop--;
	if(longTongue)
		longTongue--;

	// update players (for spawn counters / score bonus)
	if(player[0].spawnTimer)
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
	switch(garib->type)
	{
		case SPAWN_GARIB:
			if(player[0].spawnTimer)
			{
				VECTOR m;
				// increase player score bonus
				if(player[0].spawnScoreLevel < 5)
					player[0].spawnScoreLevel++;

				XfmPoint (&m,&garib->sprite.pos);
			}

			player[0].spawnTimer = SPAWN_SCOREUPTIMER;

			CreateAndAddSpawnScoreSprite(&garib->sprite.pos,player[0].spawnScoreLevel);

			CreateAndAddSpecialEffect( FXTYPE_GARIBCOLLECT, &garib->sprite.pos, &upVec, 25, 0.0, 0.0, 2.0 );

			player[0].score += (player[0].spawnScoreLevel * 10);
			player[0].numSpawn++;
			//PlaySample(0,&garib->sprite.pos,192,118 + (player[0].spawnScoreLevel * 10));
			break;

		case EXTRAHEALTH_GARIB:
			player[0].score += 100;
			break;

		case EXTRALIFE_GARIB:
			player[0].score += 500;
			break;

		case AUTOHOP_GARIB:
			autoHop	= 150;
			break;

		case QUICKHOP_GARIB:
			player[pl].isQuickHopping = 150;
			break;

		case LONGTONGUE_GARIB:
			longTongue = 150;
			break;
		case WHOLEKEY_GARIB:
			wholeKeyText->draw = 1;
			keyFound = 85;
			break;
		case HALFLKEY_GARIB:
			break;
		case HALFRKEY_GARIB:
			break;
	}

	// remove the collected garib
	SubGarib(garib);
}

//-------------------------------------------------------------------------------------------------


//----- [ GARIB RELATED ] -----//

GARIBLIST garibCollectableList;


/*	--------------------------------------------------------------------------------
	Function		: InitGaribLinkedList
	Purpose			: initialises the garib linked list
	Parameters		: 
	Returns			: void
	Info			: 
*/
void InitGaribLinkedList()
{
	garibCollectableList.numEntries = 0;
	garibCollectableList.head.next = garibCollectableList.head.prev = &garibCollectableList.head;

	if ( !reset )
	{
		memset ( garibStoreList, 0xff, storeListLength*4 );
		reset = 1;
	}

	garibListPos = 0;
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
		garibCollectableList.numEntries++;
		garib->prev = &garibCollectableList.head;
		garib->next = garibCollectableList.head.next;
		garibCollectableList.head.next->prev = garib;
		garibCollectableList.head.next = garib;
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
	if(garib->next == NULL)
		return;

	SubSprite(&garib->sprite);

	garib->prev->next = garib->next;
	garib->next->prev = garib->prev;
	garib->next = NULL;
	garibCollectableList.numEntries--;

	JallocFree((UBYTE **)&garib);
}


/*	--------------------------------------------------------------------------------
	Function		: FreeGaribLinkedList
	Purpose			: frees the garib linked list
	Parameters		: 
	Returns			: void
	Info			: 
*/
void FreeGaribLinkedList()
{
	GARIB *cur,*next;

	if(garibCollectableList.numEntries == 0)
		return;

	dprintf"Freeing linked list : GARIB : (%d elements)\n",garibCollectableList.numEntries));
	for(cur = garibCollectableList.head.next; cur != &garibCollectableList.head; cur = next)
	{
		next = cur->next;

		SubGarib(cur);
	}
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
	static indexPos = 0;

	unsigned char value;

	GARIB *garib;

	if ( worldVisualData [ player[0].worldNum ].levelVisualData [ player[0].levelNum ].multiPartLevel == MULTI_PART )
	{
		value = garibStoreList[player[0].levelNum-3][garibListPos / 8] & (1 << (garibListPos & 7));   // position to retrive from
		garibListPos++;

		if ( value == 0 )
		{
			dprintf"Returned, did not  garib ( %d : %d : %d )\n",value, garibListPos-1, player[0].levelNum-3));
			return NULL;
		}
	}

	garib = (GARIB *)JallocAlloc(sizeof(GARIB),YES,"garib");
	AddGarib(garib);

	SetVector(&garib->sprite.pos,&pos);

//	garib->gameTile = gameTile;
//	garib->dropSpeed = dropSpeed;
	garib->type = type;
	garib->active = 1;
	garib->scale = 0;
	garib->scaleAim = 1;

#ifndef PC_VERSION
	memcpy(&garib->shadow.vert,shadowVtx,sizeof(Vtx) * 4);
#endif
//	garib->shadow.altitude	= 0;
	garib->shadow.radius	= 20;
	garib->shadow.alpha		= 192;

	// Initialise garib sprite
	InitSpriteAnimation(&garib->sprite,&garibAnimation[garib->type],0);
	garib->sprite.r = 255;
	garib->sprite.g = 255;
	garib->sprite.b = 255;
	garib->sprite.a = 127;
	garib->sprite.scaleX = garib->sprite.scaleY = 0;
	
#ifndef PC_VERSION
	garib->sprite.offsetX = -garib->sprite.texture->sx / 2;
	garib->sprite.offsetY = -garib->sprite.texture->sy / 2;
	garib->sprite.flags &= -1 - SPRITE_TRANSLUCENT;
#else
	garib->sprite.offsetX = -16;
	garib->sprite.offsetY = -16;
	garib->sprite.flags &= -1 - SPRITE_TRANSLUCENT;
#endif
	if(garib->active)
		AddSprite(&garib->sprite,NULL);
	
	if(garib->type == SPAWN_GARIB)
	{
		garib->sprite.flags |= SPRITE_TRANSLUCENT;
		garib->sprite.a = 200;
	}
	else if(garib->type == WHOLEKEY_GARIB)
	{
		garib->sprite.flags |= SPRITE_TRANSLUCENT;
		garib->sprite.a = 200;
	}
	else if(garib->type == HALFLKEY_GARIB)
	{
		garib->sprite.flags |= SPRITE_TRANSLUCENT;
		garib->sprite.a = 200;
	}
	else if(garib->type == HALFRKEY_GARIB)
	{
		garib->sprite.flags |= SPRITE_TRANSLUCENT;
		garib->sprite.a = 200;
	}

//	garibListPos++;

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
	float radius;
	float scale;
	VECTOR fwd;
	VECTOR actualPos;

	// update garib scales and calculate distance from Frog
	for(garib = garibCollectableList.head.next; garib != &garibCollectableList.head; garib = garib->next)
	{
		if(garib->active == NO)
			continue;
		else if(garib->active > 1)
			garib->active--;

		garib->distanceFromFrog = DistanceBetweenPointsSquared(&garib->sprite.pos,&frog[0]->actor->pos);
		radius = 10;
		scale = garib->scale;
		scale -= (scale - garib->scaleAim) / 5;
		garib->scale = scale;

		garib->sprite.scaleX = (64 + SineWave(2,frameCount + garib->type * 2,0) * 10) * scale;
		garib->sprite.scaleY = (64 + SineWave(2,frameCount + garib->type * 2,0) * 10) * scale;

		// Drop Garibs.............

		if ( garib->gameTile != NULL )
		{			
			SetVector ( &actualPos, &garib->gameTile->centre );
			actualPos.v[Y] += 20;
			SubVector ( &fwd, &actualPos, &garib->sprite.pos );
			MakeUnit  ( &fwd );
			garib->sprite.pos.v[X] += ( fwd.v[X] * garib->dropSpeed );
			garib->sprite.pos.v[Y] += ( fwd.v[Y] * garib->dropSpeed );
			garib->sprite.pos.v[Z] += ( fwd.v[Z] * garib->dropSpeed );

		}
	}
}


/*	--------------------------------------------------------------------------------
	Function		: 
	Purpose			: 
	Parameters		: 
	Returns			: frog
	Info			: 
*/
void CreateAndAddSpawnScoreSprite(VECTOR *pos,char scoreType)
{
	char bmpBuff[16];

	SPRITE *sprite = (SPRITE *)JallocAlloc(sizeof(SPRITE),YES,"SPRITE");

	SetVector(&sprite->pos,pos);

	sprite->scaleX		= 64;
	sprite->scaleY		= 64;

	sprite->anim.type	= SPRITE_ANIM_SPAWNSCORE;

	sprite->r			= 255;
	sprite->g			= 255;
	sprite->b			= 255;
	sprite->a			= 255;

//	sprite->kill		= 0;	// commented out to matched glover 2 structures
	sprite->flags		= SPRITE_TRANSLUCENT;

	sprintf(bmpBuff,"spnu%d.bmp",(scoreType * 10));
	FindTexture(&sprite->texture,UpdateCRC(bmpBuff),YES);
	
	sprite->offsetX = -16;
	sprite->offsetY = -16;

	AddSprite(sprite,NULL);
}

void DropGaribToTile(GARIB *garib, GAMETILE *tile, float dropSpeed)
{
	garib->gameTile = tile;
	garib->dropSpeed = dropSpeed;
}
