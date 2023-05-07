/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: overlays.h
	Programmer	: Andy Eder
	Date		: 20/04/99 10:08:52

----------------------------------------------------------------------------------------------- */

#ifndef OVERLAYS_H_INCLUDED
#define OVERLAYS_H_INLCUDED


#include "types2dgfx.h"


//----- [ DEFINES / FLAGS ] --------------------------------------------------------------------//

#define ROUND2SHORT(x)				((short)(x + 0.5F))
#define ROUND2INT(x)				((int)(x + 0.5F))
#define ROUND2LONG(x)				((long)(x + 0.5F))


// overlay flags

#define OVERLAY_IA				(1 << 0)

// sprite animation flags

#define ANIMATION_FORWARDS		(1 << 0)
#define ANIMATION_BACKWARDS		(1 << 1)
#define ANIMATION_CYCLE			(1 << 2)
#define ANIMATION_PINGPONG		(1 << 3)
#define ANIMATION_RANDOMFRAMES	(1 << 4)
#define ANIMATION_PLAYONCE		(1 << 5)
#define ANIMATION_RANDOMSTART	(1 << 6)


//----- [ GLOBALS ] ----------------------------------------------------------------------------//

typedef struct TAGSPRITEOVERLAYLIST
{
	int				numEntries;
	SPRITEOVERLAY	head;

} SPRITEOVERLAYLIST;


extern SPRITEOVERLAYLIST spriteOverlayList;

extern SPRITEOVERLAY *bronzeCup[8];
extern SPRITEOVERLAY *silverCup[8];
extern SPRITEOVERLAY *goldCup[8];

//----- [ FUNCTION PROTOTYPES ] ----------------------------------------------------------------//

extern SPRITEOVERLAY *CreateAndAddSpriteOverlay(float x,float y,char *txtrName,short width,short height,unsigned char alpha,short flags);
extern void AddFrameToSpriteOverlay(SPRITEOVERLAY *sOver,char *txtrName);
extern void SetSpriteOverlayAnimSpeed(SPRITEOVERLAY *sOver,float newSpeed);

extern void EnableSpriteOverlay(SPRITEOVERLAY *sOver);
extern void DisableSpriteOverlay(SPRITEOVERLAY *sOver);

extern void InitSpriteOverlayLinkedList();
extern void FreeSpriteOverlayLinkedList();
extern void AddSpriteOverlay();
extern void SubSpriteOverlay(SPRITEOVERLAY *sOver);


#endif
