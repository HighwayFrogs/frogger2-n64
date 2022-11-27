/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: tongue.h
	Programmer	: Andrew Eder
	Date		: 12/15/98

----------------------------------------------------------------------------------------------- */

#ifndef TONGUE_H_INCLUDED
#define TONGUE_H_INCLUDED


#define	MAX_TONGUENODES				12
#define TONGUE_FRACTION				(1/(float)MAX_TONGUENODES)

#define TONGUE_RADIUSNORMAL			125.0F
#define TONGUE_RADIUSLONG			225.0F

#define TONGUE_STICKYRADIUS			10.0

#define TONGUE_OFFSET_UP			-15.0
#define TONGUE_OFFSET_FORWARD		0

#define TONGUE_WRAPAROUNDTHRESHOLD	-0.2


enum
{
	TONGUE_GET_BABY,
	TONGUE_GET_GARIB,
	TONGUE_GET_FROG,
	TONGUE_GET_SCENIC,
	TONGUE_GET_HEALTH,
};


typedef struct
{
	VECTOR source, target, pos, fwd, interp;

	unsigned long flags;
	float radius, progress;
	unsigned char type, canTongue;

	TEXTURE *tex;
	VECTOR *segment;

	void *thing;	// What we're trying to collect

} TONGUE;


//----- [ FROG TONGUE FLAGS ] ----------------------------------------------

#define TONGUE_NONE				0
#define TONGUE_IDLE				(1 << 0)
#define TONGUE_BEINGUSED		(1 << 1)
#define TONGUE_OUTGOING			(1 << 2)
#define TONGUE_INCOMING			(1 << 3)
#define TONGUE_HASITEMONIT		(1 << 4)
#define TONGUE_HASITEMINMOUTH	(1 << 5)
#define TONGUE_THROWING			(1 << 6)
#define TONGUE_SEARCHING		(1 << 7)
#define TONGUE_GRAPPLE          (1 << 8)

//----- [ GLOBALS ] --------------------------------------------------------


extern TONGUE tongue[MAX_FROGS];


//----- [ FUNCTION PROTOTYPES ] --------------------------------------------

extern void InitTongues( );
extern void UpdateFrogTongue( int pl );
extern void RemoveFrogTongue( int pl );

extern ENEMY *BabyFrogIsInRange(float radius, int pl);
extern GARIB *GaribIsInRange(float radius, int pl);
extern ACTOR2 *ScenicIsInRange(float radius, int pl);
extern ACTOR2 *FrogIsInRange(float radius, int pl);

extern void ThrowFrogDirection( int thrower, int throwee, int dir );

#endif
