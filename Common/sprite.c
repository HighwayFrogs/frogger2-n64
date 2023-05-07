/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: sprite.c
	Programmer	: Matthew Cloy
	Date		: 13/11/98

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI_2

#include <ultra64.h>

#include "incs.h"


#define MOVE
#define ANIM

unsigned int numTextures   = -1;
unsigned int numOverlays   = -1;
unsigned int numAnimations = -1;

TEXTURE_STRUCTURE   *textureList	 = NULL;
ANIM_STRUCTURE		*animationList	 = NULL;


FOG fog = {120,120,160,FOG_OFF,995,5000};

	/***********************************************************************************
	/********* TEXTURE LIST FUNCTIONS **************************************************
	/***********************************************************************************


/*	--------------------------------------------------------------------------------
	Function		: AddTextureToList
	Purpose			: adds a texture to the texture list
	Parameters		:
	Returns			: 
	Info			:
*/
void AddTextureToList ( char *txtrName, char *textID )
{
	TEXTURE_STRUCTURE *newItem;

	newItem = ( TEXTURE_STRUCTURE * ) JallocAlloc ( sizeof ( TEXTURE_STRUCTURE ), YES, "TXTRLIST" );


	FindTexture ( &newItem->txtr, UpdateCRC ( txtrName ), YES);
	
	numTextures++;

	newItem->textID				= textID;
	newItem->ID					= numTextures;

		
	newItem->next		= textureList;
	textureList			= newItem;
	
/*	if ( numTextures == 0 )
	{
		textureList		= newItem;
		currentTexture	= textureList;
	}
	else
	{
		currentTexture->next	= newItem;
		currentTexture			= currentTexture->next;
	}
	// endif

*/
}


/*	--------------------------------------------------------------------------------
	Function		: FreeTextureList
	Purpose			: adds a texture to the texture list
	Parameters		:
	Returns			: 
	Info			:
*/
void FreeTextureList ( void )
{
	TEXTURE_STRUCTURE *next,*cur;
	
	cur = textureList;
	while ( cur )
	{
		next = cur->next;		
		JallocFree ( (UBYTE**)&cur );
		cur  = next;
	}
	// endwhile
	textureList		= NULL;
	numTextures		= -1;
}

	/***********************************************************************************
	/********* ANIMATION FUNCTIONS *****************************************************
	/***********************************************************************************


/*	--------------------------------------------------------------------------------
	Function		: CreateAnimation
	Purpose			: 
	Parameters		:
	Returns			: ptr to a anim_structure
	Info			:
*/
ANIM_STRUCTURE *CreateAnimation ( char *textID[], int numFrames, int numAnimFrames,
  	    					      int initX, int initY, int initZ,
 	    					      int startX, int startY, int startZ,
 	    					      int finishX, int finishY, int finishZ,
								  int width, int height,
								  char r, char g, char b, char a,
								  int animFlags, int moveFlags, int delay,
								  int active, int animated, float velocityCounter, int type )
{
	int c;
	int i;
	ANIM_STRUCTURE	  *newItem;
	TEXTURE_STRUCTURE *tstruct;

	newItem = ( ANIM_STRUCTURE * ) JallocAlloc ( sizeof ( ANIM_STRUCTURE ), YES, "ANIMATION" );

	if ( animated )
		for ( c = 0; c <= numAnimFrames; c++ )
		{
			(char*) newItem->textID[c] = textID[c];
			(int)   newItem->ID[c]     = -1;
		}
		// endfor
	else
		for ( c = 0; c < numFrames; c++ )
		{
			(char*) newItem->textID[c] = textID[c];
			(int)   newItem->ID[c]     = -1;
		}
		// endfor
	// endelseif

	for ( c = 0; c <= numTextures; c++ )
	{
		tstruct = &textureList[c];
		if ( !animated )
			for ( i = 0; i < numFrames; i++ )
			{
				if ( tstruct->textID == newItem->textID[i] )
				{
					(int)   newItem->ID[i] = c;
				}
				// endif
			}
			// endfor
		else
			for ( i = 0; i < numAnimFrames; i++ )
			{
				if ( tstruct->textID == newItem->textID[i] )
				{
					(int)   newItem->ID[i] = c;
				}
				// endif
			}
			// endfor
		// endelseif
	}
	// endfor


	newItem->active			= active;
	newItem->animated		= animated;
	newItem->numFrames		= numFrames;
	newItem->numAnimFrames  = numAnimFrames;

	newItem->currFrameNum	= 0;

	newItem->curXPos  = initX;
	newItem->curYPos  = initY;
	newItem->curZPos  = initZ;

	newItem->startXPos  = startX;
	newItem->startYPos  = startY;
	newItem->startZPos  = startZ;

	newItem->finishXPos = finishX;
	newItem->finishYPos = finishY;
	newItem->finishZPos	= finishZ;

	newItem->velocityCounter = velocityCounter;

	newItem->width		= width;
	newItem->height		= height;

	newItem->r			= r;
	newItem->g			= g;
	newItem->b			= b;
	newItem->a			= a;

	newItem->animFlags	= animFlags;
	newItem->moveFlags	= moveFlags;
	newItem->delay		= delay;
	newItem->counterDelay = 0;

	newItem->type		= type;

	numAnimations++;

	newItem->next	   = animationList;
	animationList	   = newItem;

}


/*	--------------------------------------------------------------------------------
	Function		: CreateAnimation
	Purpose			: 
	Parameters		:
	Returns			: ptr to a anim_structure
	Info			:
*/
void FreeAnimationList ( void )
{
	ANIM_STRUCTURE *next,*cur;
	
	cur = animationList;
	while ( cur )
	{
		next = cur->next;		
		JallocFree ( (UBYTE**)&cur );
		cur  = next;
	}
	// endwhile
	animationList	= NULL;
}





//----- [ SPRITE RELATED ] ----------------------------------------------------------------------//


// FRAMELISTS FOR SPRITE ANIMS

FRAMELIST spriteFrameList[NUM_SPRITE_ANIMS] =
{
	{ NULL,0,0,"coin00"},				// SPAWN_ANIM			//0
	{ NULL,0,0,"spa" },				// EXTRAHEALTH_ANIM		//1
	{ NULL,0,0,"spa" },				// EXTRALIFE_ANIM		//2
	{ NULL,0,0,"spa" },				// AUTOHOP_ANIM			//3
	{ NULL,0,0,"spa" },				// LONGHOP_ANIM			//4
	{ NULL,0,0,"spa" },				// LONGTONGUE_ANIM		//5
	{ NULL,0,0,"wholekey" },		// WHOLEKEY_ANIM		//6
	{ NULL,0,0,"halflkey" },		// WHOLEKEY_ANIM		//7
	{ NULL,0,0,"halfrkey" },		// WHOLEKEY_ANIM		//8
};



SPRITELIST spriteList;
BACKDROPLIST backdropList;

SPRITE *testSpr = NULL;

/*	--------------------------------------------------------------------------------
	Function		: InitSpriteLinkedList
	Purpose			: initialises the sprite list
	Parameters		: 
	Returns			: void
	Info			: 
*/
void InitSpriteLinkedList()
{
	spriteList.head.next = spriteList.head.prev = &spriteList.head;
	spriteList.numEntries = 0;
	spriteList.head.flags = SPRITE_TRANSLUCENT;
}

/*	--------------------------------------------------------------------------------
	Function		: FreeSpriteLinkedList
	Purpose			: frees the sprite linked list
	Parameters		: 
	Returns			: void
	Info			: 
*/
void FreeSpriteLinkedList()
{
	SPRITE *cur,*next;

	if(spriteList.numEntries == 0)
		return;

	dprintf"Freeing linked list : SPRITE : (%d elements)\n",spriteList.numEntries));
	for(cur = spriteList.head.next; cur != &spriteList.head; cur = next)
	{
		next = cur->next;

		SubSprite(cur);
	}
}

/*	--------------------------------------------------------------------------------
	Function		: AddSprite
	Purpose			: adds a sprite to the sprite list
	Parameters		: SPRITE *,SPRITE *
	Returns			: void
	Info			: 
*/
void AddSprite(SPRITE *sprite,SPRITE *after)
{
	SPRITE *ptr;

	if(sprite->next == NULL)
	{
		if(after)
			ptr = after;
		else
		{
			for(ptr = &spriteList.head; ptr != spriteList.head.prev; ptr = ptr->next)
			{
				if(((sprite->flags & SPRITE_TRANSLUCENT) && ((sprite->flags & SPRITE_FLAGS_CHEAT) == 0)) && (((ptr->next->flags & SPRITE_TRANSLUCENT) == 0) || (ptr->next->flags & SPRITE_FLAGS_CHEAT)))
					continue;
				if((((sprite->flags & SPRITE_TRANSLUCENT) == 0) || (sprite->flags & SPRITE_FLAGS_CHEAT)) && ((ptr->next->flags & SPRITE_TRANSLUCENT) || ((ptr->next->flags & SPRITE_FLAGS_CHEAT) == 0)))
					break;
			}
		}

		sprite->kill	= 0;

		sprite->prev	= ptr;
		sprite->next	= ptr->next;
		ptr->next->prev	= sprite;
		ptr->next		= sprite;
		spriteList.numEntries++;
	}
}

/*	--------------------------------------------------------------------------------
	Function		: SubSprite
	Purpose			: removes a sprite
	Parameters		: SPRITE *
	Returns			: void
	Info			: 
*/
void SubSprite(SPRITE *sprite)
{
	if(sprite->next == NULL)
		return;

	sprite->anim.type	= SPRITE_ANIM_NONE;
	sprite->prev->next	= sprite->next;
	sprite->next->prev	= sprite->prev;

	sprite->next		= NULL;
	spriteList.numEntries--;
}

/*	--------------------------------------------------------------------------------
	Function		: AnimateSprites
	Purpose			: goes through the spritelist and animates
	Parameters		: 
	Returns			: void
	Info			: 
*/
void AnimateSprites()
{
	SPRITE *cur,*next;

	for(cur = spriteList.head.next; cur != &spriteList.head; cur = next)
	{
		next = cur->next;

		// check for sprites to be killed
		if(cur->kill)
		{
			SubSprite(cur);
			continue;
		}

		// check if this sprite is a spawn score anim...
		if(cur->anim.type == SPRITE_ANIM_SPAWNSCORE)
		{
			cur->pos.v[Y] += 6;
			cur->a -= 12;
			if(cur->a < 20)
				cur->kill = 1;
			
			continue;
		}

		if(cur->anim.type != SPRITE_ANIM_NONE)
			AnimateSprite(cur);
	}
}

/*	--------------------------------------------------------------------------------
	Function		: AnimateSprite
	Purpose			: animates a sprite
	Parameters		: SPRITE *
	Returns			: void
	Info			: 
*/
void AnimateSprite(SPRITE *sprite)
{
	SPRITE_ANIMATION *anim = &sprite->anim;

	if(anim->lifespan > 0)
	{
		anim->lifespan--;
		if(!anim->lifespan)
		{
			sprite->texture = NULL;
			anim->type		= SPRITE_ANIM_NONE;
			return;
		}
	}

	if(anim->type == SPRITE_ANIM_LIFETIME)
		return;

	// sort out alphas...
	if(anim->endAlpha != anim->startAlpha)
	{
		if(anim->lifespan > 0)
			sprite->a = anim->endAlpha + ((anim->startAlpha - anim->endAlpha) * anim->lifetime) / anim->lifespan;
		else
			sprite->a = anim->endAlpha + ((anim->startAlpha - anim->endAlpha) * (anim->frameList->numFrames - anim->currentFrame - 1)) / (anim->frameList->numFrames - 1);
	}

	// sort out scales...
	if(anim->endScale != anim->startScale)
	{
		if(anim->lifespan > 0)
			sprite->scaleX = sprite->scaleY = anim->endScale + ((anim->startScale - anim->endScale) * anim->lifetime) / anim->lifespan;
		else
			sprite->scaleX = sprite->scaleY = anim->endScale + ((anim->startScale - anim->endScale) * (anim->frameList->numFrames - anim->currentFrame - 1)) / (anim->frameList->numFrames - 1);
	}

	if(anim->type != SPRITE_ANIM_SCALE_ALPHA)
	{
		if(anim->currentFrame >= anim->frameList->numFrames)
		{
			if(anim->type == SPRITE_ANIM_CYCLE)
				anim->currentFrame = 0;
			else if(anim->type == SPRITE_ANIM_ONESHOT)
			{
				sprite->texture = NULL;
				return;
			}
		}
		else if((anim->type == SPRITE_ANIM_REVERSE) && (anim->currentFrame == 0))
		{
			sprite->texture = NULL;
			return;
		}

		sprite->texture = *(anim->frameList->texture + anim->currentFrame);

		if(anim->counter > 0)
		{
			anim->counter -= 32;
		}
		else
		{
			if(sprite->flags & SPRITE_REVERSING)
				anim->currentFrame--;
			else
				anim->currentFrame++;
			anim->counter += anim->delay;
			if(anim->type == SPRITE_ANIM_FLIPFLOP)
			{
				if(anim->currentFrame >= anim->frameList->numFrames)
				{
					sprite->flags |= SPRITE_REVERSING;
					anim->currentFrame--;
				}
				else if(anim->currentFrame == 0)
				{
					sprite->flags &= -1 - SPRITE_REVERSING;
				}
			}
		}
	}
}

/*	--------------------------------------------------------------------------------
	Function		: InitSpriteAnimation
	Purpose			: initialises a sprite animation
	Parameters		: SPRITE *,SPRITE_ANIMATION_TEMPLATE *,USHORT
	Returns			: void
	Info			: 
*/
void InitSpriteAnimation(SPRITE *sprite,SPRITE_ANIMATION_TEMPLATE *temp,USHORT lifespan)
{
	memcpy(&sprite->anim,temp,sizeof(SPRITE_ANIMATION_TEMPLATE));

	sprite->flags	= sprite->anim.spriteFlags;
	sprite->a		= sprite->anim.startAlpha;

	sprite->anim.counter = 0;
	if(sprite->anim.type == SPRITE_ANIM_REVERSE)
	{
		sprite->anim.currentFrame = sprite->anim.frameList->numFrames - 1;
		sprite->flags |= SPRITE_REVERSING;
		sprite->scaleX = sprite->scaleY = sprite->anim.endScale;
	}
	else
	{
		sprite->anim.currentFrame = 0;
		sprite->flags &= -1 - SPRITE_REVERSING;
		sprite->scaleX = sprite->scaleY = sprite->anim.startScale;
	}

	sprite->anim.lifespan = lifespan;
	sprite->anim.lifetime = lifespan;

	if(sprite->anim.type == SPRITE_ANIM_CYCLE_RANDOM)
	{
		sprite->anim.currentFrame = Random(sprite->anim.frameList->numFrames);
		sprite->anim.type = SPRITE_ANIM_CYCLE;
	}

#ifndef PC_VERSION
	sprite->texture = *(temp->frameList->texture + sprite->anim.currentFrame);
#else
	sprite->texture = temp->frameList->texture;
#endif
}




/*	--------------------------------------------------------------------------------
	Function		: InitSpriteFrameLists
	Purpose			: initialises the sprite frame lists
	Parameters		: 
	Returns			: void
	Info			: 
*/
void InitSpriteFrameLists()
{
	int i,j,found,totalFrames = 0;
	char temp[256];
	TEXTURE *texture;

	for(i=0; i<NUM_SPRITE_ANIMS; i++)
	{
		if(spriteFrameList[i].overrideNumFrames)
		{
			totalFrames += spriteFrameList[i].numFrames;
			continue;
		}
		found = YES;
		j = 0;
		while(found == YES)
		{
			sprintf(temp,"%s%02d.bmp",spriteFrameList[i].filename,j+1);
			FindTexture(&texture,UpdateCRC(temp),NO);
			if(!texture)
				found = NO;
			else
				j++;
		}

		spriteFrameList[i].numFrames = j;
		totalFrames += j;
	}

	if ( spriteFrameList[0].texture )
	{
		JallocFree((UBYTE **)&spriteFrameList[0].texture);
	}
	// ENDIF

	spriteFrameList[0].texture = (TEXTURE **)JallocAlloc(totalFrames * sizeof(TEXTURE *),NO,"SFL");
	for(i=0; i<NUM_SPRITE_ANIMS; i++)
	{
		if(i)
			spriteFrameList[i].texture = spriteFrameList[i - 1].texture + spriteFrameList[i - 1].numFrames;

		if(spriteFrameList[i].numFrames)
		{
			for(j=0; j<spriteFrameList[i].numFrames; j++)
			{
				sprintf(temp,"%s%02d.bmp",spriteFrameList[i].filename,j+1);
				FindTexture(&texture,UpdateCRC(temp),YES);
				*(spriteFrameList[i].texture + j) = texture;
			}
		}
	}
}

/*	--------------------------------------------------------------------------------
	Function		: FreeSpriteFrameLists
	Purpose			: frees the sprite frame lists
	Parameters		: 
	Returns			: void
	Info			: 
*/
void FreeSpriteFrameLists()
{
/*
	int j;

	for(j=0; j<NUM_SPRITE_ANIMS; j++)
		if(spriteFrameList[j].texture)
			JallocFree((UBYTE **)&spriteFrameList[j].texture);

	dprintf"FREED SPRITE FRAME LISTS\n"));
*/
}

/*	--------------------------------------------------------------------------------
	Function		: AddNewSpriteToList
	Purpose			: adds an arbitrary sprite to the list
	Parameters		: float, float, float, short, char *, short
	Returns			: SPRITE *
	Info			: 
*/
SPRITE *AddNewSpriteToList(float x,float y,float z,short size,char *txtrName,short flags)
{
	SPRITE *sprite;

	sprite = (SPRITE *)JallocAlloc(sizeof(SPRITE),YES,"sprite");

	FindTexture(&sprite->texture,UpdateCRC(txtrName),YES);

	sprite->pos.v[X]	= x;
	sprite->pos.v[Y]	= y;
	sprite->pos.v[Z]	= z;

	sprite->r			= 255;
	sprite->g			= 255;
	sprite->b			= 255;
	sprite->a			= 255;

	sprite->offsetX		= -16; //-sprite->texture->sx / 2;
	sprite->offsetY		= -16; //-sprite->texture->sy / 2;
	sprite->flags		= flags;

	sprite->scaleX		= size;
	sprite->scaleY		= size;

	sprite->anim.type	= SPRITE_ANIM_NONE;

	AddSprite(sprite,NULL);

	return sprite;
}
