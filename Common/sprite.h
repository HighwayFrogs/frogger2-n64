/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: sprite.h
	Programmer	: Matthew Cloy
	Date		: 13/11/98

----------------------------------------------------------------------------------------------- */

#ifndef SPRITE_H_INCLUDED
#define SPRITE_H_INCLUDED

//----- [ DEFINE / FLAGS ] ---------------------------------------------------//

#define NUM_template_BMS		2

#define MAX_FRAMES				17

#define MOVE_WEST				( 1 << 0 )
#define MOVE_EAST				( 1 << 1 )
#define MOVE_NORTH				( 1 << 2 )
#define MOVE_SOUTH				( 1 << 3 )

#define ANIM_CYCLE				( 1 << 4 )
#define ANIM_FORWARDS			( 1 << 5 )
#define ANIM_BACKWARDS			( 1 << 6 )
#define ANIM_START_END			( 1 << 7 )
#define ANIM_BOUNCE				( 1 << 8 )
#define ANIM_ONCE				( 1 << 9 )

#define LAND   1
#define WATER  2


////////   Fog related
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

enum
{
	SPRITE_ANIM_NONE,
	SPRITE_ANIM_CYCLE,
	SPRITE_ANIM_ONESHOT,
	SPRITE_ANIM_FLIPFLOP,
	SPRITE_ANIM_CYCLE_RANDOM,
	SPRITE_ANIM_SCALE_ALPHA,
	SPRITE_ANIM_REVERSE,
	SPRITE_ANIM_LIFETIME,
	SPRITE_ANIM_SPAWNSCORE,
};

#define SPRITE_DRAWDISTANCE		200000


//----- [ DATA STRUCTURES ] --------------------------------------------------//

typedef struct _TEXTURE_STRUCTURE
{
	struct _TEXTURE_STRUCTURE *next;

	char *textID;
	int  ID;

	TEXTURE	*txtr;

} TEXTURE_STRUCTURE;


typedef struct _ANIM_STRUCTURE
{
	struct	_ANIM_STRUCTURE *next;		// ptr to next animation range

	char	*textID [ MAX_FRAMES ];
	int		*ID		[ MAX_FRAMES ];						// anim range ID

	int numFrames;
	int	currFrameNum;
		
	int active;

	int	curXPos,    curYPos,    curZPos;
	int	startXPos,  startYPos,  startZPos;
	int	finishXPos, finishYPos, finishZPos;

	int	width, height;

	float	velocityX, velocityY;
	float   velocityCounter;
	int		moveFlags;					// 
	
	char	r, g, b, a;
	char	draw;	

	int		animated;
	int		numAnimFrames;
	int		animFlags;					// 
	int		startFrame;
	int     finishFrame;
	int		delay;						// 
	int		counterDelay;				// 

	int		type;
	int		moved;


} ANIM_STRUCTURE;




//----------------------------------------------------------------------------//

extern unsigned int numTextures;
extern unsigned int numOverlays;
extern unsigned int numAnimations;

extern TEXTURE_STRUCTURE	*textureList;
extern ANIM_STRUCTURE		*animationList;

extern void AddTextureToList ( char *txtrName, char *textID );
extern void FreeTextureList  ( void );

extern ANIM_STRUCTURE *CreateAnimation ( char *textID[], int numFrames, int numAnimFrames,
			    					     int initX, int initY, int initZ,
			    					     int startX, int startY, int startZ,
			    					     int finishX, int finishY, int finishZ,
								         int width, int height,
								         char r, char g, char b, char a,
		  								 int animFlags, int moveFlags, int delay,
										 int active, int animated, float velocityCounter, int type );
extern void FreeAnimationList (void );


//----- [ SPRITE RELATED ] ----------------------------------------------------------------------//


#define SPRITE_TRANSLUCENT						(1 << 0)
#define SPRITE_FLIPX							(1 << 1)
#define SPRITE_FLIPY							(1 << 2)
#define SPRITE_DONE								(1 << 3)
#define SPRITE_REVERSING						(1 << 4)

#define SPRITE_FLAGS_COLOUR_BLEND				(1 << 5)
#define SPRITE_FLAGS_COLOUR_BLEND_AFTERLIGHT	(1 << 6)
#define SPRITE_FLAGS_PIXEL_OUT					(1 << 7)

#define SPRITE_FLAGS_CHEAT						(1 << 8)
#define SPRITE_FLAGS_PAUSE						(1 << 9)
#define SPRITE_FLAGS_NOZB						(1 << 10)

#define MAX_OVERLAYS	260


typedef struct TAGSPRITELIST
{
	SPRITE		head;
	int			numEntries;
	int			xluMode;
	TEXTURE		*lastTexture;

} SPRITELIST;


// ---- [ SPRITE ANIMATION RELATED ] ----- //

enum
{
	SPAWN_ANIM,					//0
	EXTRAHEALTH_ANIM,			//1
	EXTRALIFE_ANIM,				//2
	AUTOHOP_ANIM,				//3
	LONGHOP_ANIM,				//4
	LONGTONGUE_ANIM,			//5
	WHOLEKEY_ANIM,				//6
	HALFLKEY_ANIM,				//7
	HALFRKEY_ANIM,				//8

	NUM_SPRITE_ANIMS
};



extern FRAMELIST spriteFrameList[NUM_SPRITE_ANIMS];

extern SPRITE *testSpr;

extern SPRITELIST spriteList;
extern float printSpritesProj[4][4][4];

void InitSpriteLinkedList();
void FreeSpriteLinkedList();
void AddSprite(SPRITE *sprite,SPRITE *after);
void SubSprite(SPRITE *sprite);
void AnimateSprites();
void AnimateSprite(SPRITE *sprite);
void InitSpriteAnimation(SPRITE *sprite,SPRITE_ANIMATION_TEMPLATE *temp,USHORT lifespan);


void InitSpriteFrameLists();

SPRITE *AddNewSpriteToList(float x,float y,float z,short size,char *txtrName,short flags);


#endif