/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: sprite.h
	Programmer	: Jim Hubbard
	Date		: 25/02/00

----------------------------------------------------------------------------------------------- */

#ifndef SPRITE_H_INCLUDED
#define SPRITE_H_INCLUDED



////////	Basic sprite stuff
#define SPRITE_TRANSLUCENT						(1 << 0)
#define SPRITE_DONE								(1 << 1)
#define SPRITE_FLAGS_COLOUR_BLEND				(1 << 2)
#define SPRITE_FLAGS_COLOUR_BLEND_AFTERLIGHT	(1 << 3)
#define SPRITE_FLAGS_ROTATE						(1 << 4)


typedef struct TAGSPRITE
{
	struct TAGSPRITE *next,*prev;

	TEXTURE *texture;
	VECTOR pos;
	short scaleX;
	short scaleY;
	short flags;
	UBYTE r,g,b,a;
	UBYTE red2,green2,blue2,alpha2;
	BYTE  offsetX;
	BYTE  offsetY;
	VECTOR sc;
	char draw;

	float angle;
	float angleInc;
	short arrayIndex;

}SPRITE;


typedef struct
{
	// Statically allocated array of sprites
	SPRITE *array;
	// Number of sprites allocated
	short count;

	// Defines and data for the sprite allocation stack
	SPRITE **stack;
	short stackPtr;

	// Static head of list
	SPRITE head;

} SPRITELIST;


extern SPRITELIST sprList;


SPRITE *AllocateSprites( int number );
void DeallocateSprites( SPRITE *head, int number );

void InitSpriteList( );
void FreeSpriteList( );



////////	Fog related
enum
{
	FOG_OFF,
	FOG_TO_COLOUR,
};

typedef struct
{
	UBYTE r,g,b,mode;
	short min,max;
}FOG;

extern FOG fog;



/////// 	Sprite sorting related
#define MAX_ARRAY_SPRITES		768

extern int numSortArraySprites;
extern SPRITE *spriteSortArray;

void InitSpriteSortArray(int numElements);
void FreeSpriteSortArray();
int SpriteZCompare(const void *arg1,const void *arg2);
void ZSortSpriteList();


#endif
