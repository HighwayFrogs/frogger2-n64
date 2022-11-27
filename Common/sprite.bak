/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: sprite.c
	Programmer	: Jim Hubbard
	Date		: 25/02/00

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI_2

#include <ultra64.h>
#include "incs.h"

#define MAX_SPRITES	512

FOG fog = {120,120,160,FOG_OFF,995,5000};

int numSortArraySprites = 0;
SPRITE *spriteSortArray = NULL;

SPRITELIST sprList;


/*	--------------------------------------------------------------------------------
	Function		: AllocateSprites
	Purpose			: Find a number of sprites and return a sublist
	Parameters		: number of sprites
	Returns			: pointer to first one
	Info			: 
*/
SPRITE *AllocateSprites( int number )
{
	SPRITE *s;
	// Return if allocation is impossible for any reason
	if( (number <= 0) || (sprList.stackPtr-number < 0) || (number >= MAX_SPRITES-sprList.count) ) return NULL;

	// Now we can go and allocate sprites with gay abandon
	while( number-- )
	{
		s = sprList.stack[sprList.stackPtr--];

		s->prev	= &sprList.head;
		s->next	= sprList.head.next;
		sprList.head.next->prev	= s;
		sprList.head.next = s;
		s->draw = 1;

		sprList.count++;
	}

	return sprList.stack[sprList.stackPtr+1];
}


/*	--------------------------------------------------------------------------------
	Function		: DeallocateSprites
	Purpose			: Remove sprites from list and flag as unused
	Parameters		: number of sprites
	Returns			: 
	Info			: 
*/
void DeallocateSprites( SPRITE *head, int number )
{
	SPRITE *s=head, *t;

	if( !s || !s->next || (number<=0) || (sprList.stackPtr+number >= MAX_SPRITES) ) return;

	while( number-- )
	{
		t = s->next;

		s->prev->next	= s->next;
		s->next->prev	= s->prev;
		s->next = NULL;
		s->draw = 0;

		sprList.count--;
		sprList.stack[++sprList.stackPtr] = s;

		s = t;
	}
}


/*	--------------------------------------------------------------------------------
	Function		: FreeSpriteList
	Purpose			: Deallocate all sprites and set list to empty
	Parameters		: 
	Returns			: 
	Info			: 
*/
void FreeSpriteList( )
{
	int i;

	if( sprList.array )
	{
		// Remove all sprites in array from sprite list so they don't get removed after deallocation
		for( i=0; i<MAX_SPRITES; i++ )
		{
			if( !sprList.array[i].next ) continue;

			sprList.array[i].prev->next	= sprList.array[i].next;
			sprList.array[i].next->prev	= sprList.array[i].prev;
			sprList.array[i].next = NULL;
			sprList.array[i].draw = 0;
		}

		JallocFree( (UBYTE **)&sprList.array );
		sprList.array = NULL;
	}

	if( sprList.stack )
	{
		JallocFree( (UBYTE **)&sprList.stack );
		sprList.stack = NULL;
	}

	sprList.count = 0;
	sprList.stackPtr = 0;
}


/*	--------------------------------------------------------------------------------
	Function		: InitSpriteList
	Purpose			: Set up static array and some bits of data.
	Parameters		: 
	Returns			: 
	Info			: 
*/
void InitSpriteList( )
{
	int i;

	sprList.head.next = sprList.head.prev = &sprList.head;

	// Allocate a big bunch of sprites
	sprList.array = (SPRITE *)JallocAlloc( sizeof(SPRITE)*MAX_SPRITES, YES, "Sprites" );
	sprList.stack = (SPRITE **)JallocAlloc( sizeof(SPRITE*)*MAX_SPRITES, YES, "SStack" );

	// Initially, all sprites are available
	for( i=0; i<MAX_SPRITES; i++ )
		sprList.stack[i] = &sprList.array[i];

	sprList.count = 0;
	sprList.stackPtr = i-1;
}











//----- [ SORTING STUFF ] -----------------------------------------------------------------------


/*	--------------------------------------------------------------------------------
	Function		: InitSpriteSortArray
	Purpose			: initialises the sprite sort array
	Parameters		: int
	Returns			: void
	Info			: 
*/
void InitSpriteSortArray(int numElements)
{
	if(spriteSortArray)
		FreeSpriteSortArray();

	spriteSortArray = JallocAlloc(sizeof(SPRITE) * numElements,YES,"sprSort");
	numSortArraySprites = 0;
}


/*	--------------------------------------------------------------------------------
	Function		: FreeSpriteSortArray
	Purpose			: frees the sprite sort array
	Parameters		: 
	Returns			: 
	Info			: 
*/
void FreeSpriteSortArray()
{
	if(spriteSortArray)
		JallocFree((UBYTE**)&spriteSortArray);

	spriteSortArray = NULL;
}


/*	--------------------------------------------------------------------------------
	Function		: SpriteZCompare
	Purpose			: function to compare transformed sprite z-values for sorting
	Parameters		: const void *,const void *
	Returns			: int
	Info			: -1 if less than, 0 if equal to, 1 if greater than
*/
int SpriteZCompare(const void *arg1,const void *arg2)
{
	SPRITE *s1 = (SPRITE *)arg1;
	SPRITE *s2 = (SPRITE *)arg2;

	if(s1->sc.v[Z] < s2->sc.v[Z])
		return -1;
	else if(s1->sc.v[Z] == s2->sc.v[Z])
		return 0;
	else
		return 1;
}


/*	--------------------------------------------------------------------------------
	Function		: ZSortSpriteList
	Purpose			: sorts the sprites based on z-distance
	Parameters		: 
	Returns			: void
	Info			: list to sort is specified in srcList
*/

#define SPRITE_ZSORT_DRAWDISTANCE	450

void ZSortSpriteList()
{
	SPRITE *cur;
	VECTOR frogXfm;
		
	if(sprList.count < 2)
		return;

	XfmPoint(&frogXfm,&frog[0]->actor->pos);

	// uses a quick sort

	// traverse through sprite list and create the sort array
	numSortArraySprites = 0;
	for(cur = sprList.head.next; cur != &sprList.head && numSortArraySprites < MAX_ARRAY_SPRITES; cur = cur->next)
	{
		// the static array should be large enough to hold sprites
		if((cur->sc.v[Z] - frogXfm.v[Z]) < farClip )
		{
			spriteSortArray[numSortArraySprites] = *(cur);
			numSortArraySprites++;
		}
	}

	qsort(spriteSortArray,numSortArraySprites,sizeof(SPRITE),SpriteZCompare);
}


