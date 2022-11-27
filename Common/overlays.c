/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: overlays.c
	Programmer	: Andy Eder
	Date		: 20/04/99 10:24:40

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI

#include <ultra64.h>

#include "incs.h"

SPRITEOVERLAYLIST spriteOverlayList;		// the sprite overlay list

/*	--------------------------------------------------------------------------------
	Function		: CreateAndAddSpriteOverlay
	Purpose			: creates and adds a sprite overlay to the list
	Parameters		: short,short,char*,short,short,char,char,char,char,short
	Returns			: SPRITEOVERLAY *
	Info			: 
*/
SPRITEOVERLAY *CreateAndAddSpriteOverlay(float x,float y,char *txtrName,short width,short height,unsigned char alpha,short flags)
{
	SPRITEOVERLAY *newItem;

	newItem = (SPRITEOVERLAY *)JallocAlloc(sizeof(SPRITEOVERLAY),YES,"SOVER");
	newItem->next = newItem->prev = 0;
	AddSpriteOverlay(newItem);

	newItem->xPos		= x;
	newItem->yPos		= y;
	newItem->xPosTo		= x;
	newItem->yPosTo		= y;
	newItem->width		= width;
	newItem->height		= height;
	newItem->draw		= 1;
	newItem->flags		= flags;
	newItem->num		= 0;

	newItem->r			= 255;
	newItem->g			= 255;
	newItem->b			= 255;
	newItem->a			= alpha;

	newItem->speed		= 0;

	newItem->currFrame	= 0;
	newItem->animTime	= 0.0F;
	newItem->animSpeed	= 1.0F;
	newItem->numFrames	= 0;
	FindTexture(&newItem->frames[newItem->numFrames++],UpdateCRC(txtrName),YES);

	return newItem;
}


/*	--------------------------------------------------------------------------------
	Function		: AddFrameToSpriteOverlay
	Purpose			: adds a frame to the sprite overlay (to create animation)
	Parameters		: SPRITEOVERLAY *,char *
	Returns			: void
	Info			: 
*/
void AddFrameToSpriteOverlay(SPRITEOVERLAY *sOver,char *txtrName)
{
	short i = 0;
	
	FindTexture(&sOver->frames[sOver->numFrames++],UpdateCRC(txtrName),YES);

	// check animation flags to determine where animation starts (frame)
	if(sOver->flags & ANIMATION_FORWARDS)
	{
		if(sOver->flags & ANIMATION_RANDOMSTART)
		{
			// animation starts at a random frame
			i = Random(sOver->numFrames);
			sOver->currFrame = i;
			sOver->animTime  = (float)i;
		}
		else
		{
			// animation starts at first frame
			sOver->currFrame = 0;
			sOver->animTime  = 0;
		}
	}
	else if(sOver->flags & ANIMATION_BACKWARDS)
	{
		if(sOver->flags & ANIMATION_RANDOMSTART)
		{
			// animation starts at a random frame
			i = Random(sOver->numFrames);
			sOver->currFrame = i;
			sOver->animTime  = (float)i;
		}
		else
		{
			// animation starts at last frame
			sOver->currFrame = sOver->numFrames - 1;
			sOver->animTime  = sOver->numFrames - 1;
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function		: SetSpriteOverlayAnimSpeed
	Purpose			: sets the sprite overlay animation speed
	Parameters		: SPRITEOVERLAY *,float
	Returns			: void
	Info			: 
*/
void SetSpriteOverlayAnimSpeed(SPRITEOVERLAY *sOver,float newSpeed)
{
	sOver->animSpeed = newSpeed;
}


/*	--------------------------------------------------------------------------------
	Function		: EnableSpriteOverlay
	Purpose			: enables a sprite overlay for viewing
	Parameters		: SPRITEOVERLAY *
	Returns			: void
	Info			: 
*/
void EnableSpriteOverlay(SPRITEOVERLAY *sOver)
{
	sOver->draw = 1;
}


/*	--------------------------------------------------------------------------------
	Function		: DisableSpriteOverlay
	Purpose			: disables a sprite overlay
	Parameters		: SPRITEOVERLAY *
	Returns			: void
	Info			: 
*/
void DisableSpriteOverlay(SPRITEOVERLAY *sOver)
{
	sOver->draw = 0;
}



/*	--------------------------------------------------------------------------------
	Function		: InitSpriteOverlayLinkedList
	Purpose			: initialises the sprite overlay linked list
	Parameters		: 
	Returns			: void
	Info			: 
*/
void InitSpriteOverlayLinkedList()
{
	spriteOverlayList.numEntries = 0;
	spriteOverlayList.head.next = spriteOverlayList.head.prev = &spriteOverlayList.head;
}

/*	--------------------------------------------------------------------------------
	Function		: FreeSpriteOverlayLinkedList
	Purpose			: frees the sprite overlay linked list
	Parameters		: 
	Returns			: void
	Info			: 
*/
void FreeSpriteOverlayLinkedList()
{
	SPRITEOVERLAY *cur,*next;

	if(spriteOverlayList.numEntries == 0)
		return;

	dprintf"Freeing linked list : SPRITEOVERLAY : (%d elements)\n",spriteOverlayList.numEntries));
	for(cur = spriteOverlayList.head.next; cur != &spriteOverlayList.head; cur = next)
	{
		next = cur->next;

		SubSpriteOverlay(cur);
	}

	InitSpriteOverlayLinkedList();
}

/*	--------------------------------------------------------------------------------
	Function		: AddSpriteOverlay
	Purpose			: adds a sprite overlay to the list
	Parameters		: SPRITEOVERLAY *
	Returns			: void
	Info			: 
*/
void AddSpriteOverlay(SPRITEOVERLAY *sOver)
{
	if(sOver->next == NULL)
	{
		spriteOverlayList.numEntries++;
		sOver->prev = &spriteOverlayList.head;
		sOver->next = spriteOverlayList.head.next;
		spriteOverlayList.head.next->prev = sOver;
		spriteOverlayList.head.next = sOver;
	}
}

/*	--------------------------------------------------------------------------------
	Function		: SubSpriteOverlay
	Purpose			: removes a sprite overlay from the list
	Parameters		: SPRITEOVERLAY *
	Returns			: void
	Info			: 
*/
void SubSpriteOverlay(SPRITEOVERLAY *sOver)
{
	if(sOver->next == NULL)
		return;

	sOver->prev->next = sOver->next;
	sOver->next->prev = sOver->prev;
	sOver->next = NULL;
	spriteOverlayList.numEntries--;

	JallocFree((UBYTE **)&sOver);
}
