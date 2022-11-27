/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: frogger.h
	Programmer	: James Healey
	Date		: 23/03/99

----------------------------------------------------------------------------------------------- */

#ifndef FROGGER_H_INCLUDED
#define FROGGER_H_INCLUDED

#include "specfx.h"

#define MAX_IDLE_TIME			300
#define SPAWN_SCOREUPTIMER		15
#define MAX_FROGS 4

extern long NUM_FROGS;

//----- [ FROG ANIMATION ENUMS ] -----------------------------------------------------------------

enum
{
	FROG_ANIM_STDJUMP,
	FROG_ANIM_BREATHE,
	FROG_ANIM_FWDSOMERSAULT,
	FROG_ANIM_LANDONHEAD,
	FROG_ANIM_ASSONFIRE,
	FROG_ANIM_BASICSPLAT,
	FROG_ANIM_DIVE,
	FROG_ANIM_DROWNING,
	FROG_ANIM_TRYTOFLY,
	FROG_ANIM_FLOATFALL,
	FROG_ANIM_GETUPFROMFLOAT,
	FROG_ANIM_FLOATTOSPLAT,
	FROG_ANIM_LOOKLEFTANDRIGHT,
	FROG_ANIM_LOOKDOWN,
	FROG_ANIM_SUPERHOP,
	FROG_ANIM_FORWARDSOMERSAULT,
	FROG_ANIM_USINGTONGUE,
	FROG_ANIM_SCRATCHHEAD,
	FROG_ANIM_DANCE1,
	FROG_ANIM_DANCE2,
	FROG_ANIM_DANCE3,
	FROG_ANIM_GARDEN_MOLEDEATH,
	FROG_ANIM_HOPLEFT,
	FROG_ANIM_HOPRIGHT,
	FROG_ANIM_UNKNOWN1,
	FROG_ANIM_PIN,
	FROG_ANIM_PINLOOP,
	FROG_ANIM_PINNED,
	FROG_ANIM_PINNEDLOOP,
	FROG_ANIM_DANCE4,
	FROG_ANIM_DANCE5,
	FROG_ANIM_ICE1,
	FROG_ANIM_ICE2,
	FROG_ANIM_ICE3,
	FROG_ANIM_ELECTROCUTE,
	FROG_ANIM_HURT,
	FROG_ANIM_SPACETOFLOAT,
	FROG_ANIM_SPACEFLOAT,
	FROG_ANIM_CARTWHEEL,
	FROG_ANIM_LAB_VACDEATH,
	FROG_ANIM_TO_SCREENSPLAT,
	FROG_ANIM_SCREENSPLAT,
	FROG_ANIM_ROLLERDEATH,
	FROG_ANIM_PUFF,

	NUM_FROG_ANIMS,
};

enum
{
	FROG_FROGGER,
	FROG_LILLIE,
	FROG_BABYFROG,
	FROG_SWAMPY,
	FROG_TWEE,
	FROG_WART,
	FROG_GNARLY,
	FROG_FUNKY,
	FROG_ROBOFROG,

	FROG_NUMFROGS
};


//------------------------------------------------------------------------------------------------

typedef struct _FROGSTORE
{
	char name[16];
	char model[16];
	char icon[16];
	char active;

} FROGSTORE;

extern FROGSTORE frogPool[FROG_NUMFROGS];


typedef struct _PLAYER
{
	char			name[3];
	unsigned char	character;

	char			frogon;				// Which frog is on this one
	char			frogunder;			// Which frog this one is on

	unsigned long	score;
	short			numSpawn;
	short			numCredits;
	short			lives;
	unsigned char	healthPoints;

	unsigned char	deathBy;

	unsigned char	inputPause;			// used for controller input

	short			spawnTimer;
	short			spawnScoreLevel;

	unsigned char 	worldNum;
	unsigned char	levelNum;
	unsigned long	frogState;
	short			saveSlot;

	float			idleTime;
	unsigned char	idleEnable;

	// frog movement related stuff
	unsigned char 	canJump;
	unsigned char	hasJumped;
	unsigned char	hasDoubleJumped;
	unsigned char	isSuperHopping;
	unsigned char	isSinking;

	unsigned char	extendedHopDir;

	TIMER			safe;
	TIMER			stun;
	TIMER			dead;
	TIMER			isCroaking;
	TIMER			isOnFire;
	TIMER			autohop;
	TIMER			longtongue;
	TIMER			quickhop;

	//	Parabolic curve variables
	//	See frogmove.c for a longwinded and esoteric explanation
	unsigned long	jumpStartFrame;
	float			jumpTime;
	float			jumpSpeed;
	float			jumpMultiplier;
	VECTOR			jumpFwdVector;
	VECTOR			jumpUpVector;
	VECTOR			jumpOrigin;
	float			heightJumped;

} PLAYER;

extern PLAYER player[];
							

extern float globalFrogScale;

extern ACTOR2	*frog[];
extern SPRITEOVERLAY *sprHeart[3];

extern SPECFX *frogTrail[];

extern float	croakRadius;
extern float	croakVelocity;
extern char		croakFade;
extern char		croakFadeDec;
extern char		croakR,croakG,croakB;



extern void CreateFrogger ( unsigned char createFrogActor, unsigned char createFrogOverlays );

extern void FroggerIdleAnim( int i );

#endif


