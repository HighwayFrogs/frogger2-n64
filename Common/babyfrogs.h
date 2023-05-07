/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: babyfrog.h
	Programmer	: James Healey
	Date		: 17/03/99

----------------------------------------------------------------------------------------------- */


#ifndef BABYFROGS_H_INCLUDED
#define BABYFROGS_H_INCLUDED

#define NUM_BABIES 5

extern GAMETILE **bTStart;

enum
{
	BABY_ANIM_HOP,
	BABY_ANIM_CARTWHEEL,
	BABY_ANIM_FLIP,
	BABY_ANIM_WAGGLE,
	BABY_ANIM_WAVE
};

// baby stuff
typedef struct TAGBABY
{
	unsigned long isSaved;
	unsigned char fxColour[4];
	ACTOR2 *baby;

} BABY;

extern BABY babyList[NUM_BABIES];
extern int nearestBaby;
extern int lastBabySaved;

extern SPRITEOVERLAY *babyIcons[NUM_BABIES];
extern unsigned long babiesSaved;
extern unsigned long numBabies;

void InitBabyList(unsigned char createOverlays);
void ResetBabies( );
int PickupBabyFrog( ACTOR2 *baby );
int GetNearestBabyFrog();

#endif