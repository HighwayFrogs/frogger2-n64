/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: frogmove.h
	Programmer	: Andrew Eder
	Date		: 11/13/98

----------------------------------------------------------------------------------------------- */

#ifndef FROGMOVE_H_INCLUDED
#define FROGMOVE_H_INCLUDED

#define NUM_PREV_TILES		5

#define FROG_SAFEDROPHEIGHT						60.0F
#define FROG_NORMALJUMPHEIGHT					25.0F
#define FROG_SUPERJUMPHEIGHT					50.0F

#define NOINIT_VELOCITY							-100000.0
// frog movement / action flags
#define FROGSTATUS_NONE							0
#define FROGSTATUS_ISDEAD						(1 << 0)
#define FROGSTATUS_ISSTANDING					(1 << 1)
#define FROGSTATUS_ISWANTINGU					(1 << 2)
#define FROGSTATUS_ISWANTINGD					(1 << 3)
#define FROGSTATUS_ISWANTINGL					(1 << 4)
#define FROGSTATUS_ISWANTINGR					(1 << 5)
#define FROGSTATUS_ISWANTINGSUPERHOPU			(1 << 6)
#define FROGSTATUS_ISWANTINGSUPERHOPD			(1 << 7)
#define FROGSTATUS_ISWANTINGSUPERHOPL			(1 << 8)
#define FROGSTATUS_ISWANTINGSUPERHOPR			(1 << 9)
#define FROGSTATUS_ISJUMPINGTOTILE				(1 << 10)
#define FROGSTATUS_ISJUMPINGTOPLATFORM			(1 << 11)
#define FROGSTATUS_ISSUPERHOPPING				(1 << 12)
#define FROGSTATUS_ISTONGUEING					(1 << 13)
#define FROGSTATUS_ISONMOVINGPLATFORM			(1 << 14)
#define FROGSTATUS_ISFREEFALLING				(1 << 15)
#define FROGSTATUS_ISFLOATING					(1 << 21)
#define FROGSTATUS_ISSPRINGING					(1 << 22)
#define FROGSTATUS_ISCROAKING					(1 << 23)
#define FROGSTATUS_ISTELEPORTING				(1 << 24)
#define FROGSTATUS_ISSAFE						(1 << 25)


#define FROGSTATUS_HOPFLAGS	(FROGSTATUS_ISWANTINGU | FROGSTATUS_ISWANTINGD | FROGSTATUS_ISWANTINGL | FROGSTATUS_ISWANTINGR)
#define FROGSTATUS_SUPERHOPFLAGS (FROGSTATUS_ISWANTINGSUPERHOPU | FROGSTATUS_ISWANTINGSUPERHOPD | FROGSTATUS_ISWANTINGSUPERHOPL | FROGSTATUS_ISWANTINGSUPERHOPR)
#define FROGSTATUS_ALLHOPFLAGS (FROGSTATUS_HOPFLAGS|FROGSTATUS_SUPERHOPFLAGS)

enum
{
	MOVE_UP,
	MOVE_LEFT,
	MOVE_DOWN,
	MOVE_RIGHT
};

// ENUM's for different ways of 'killing' Frogger

enum
{
	DEATHBY_NORMAL,
	DEATHBY_INSTANT,
	DEATHBY_RUNOVER,
	DEATHBY_DROWNING,
	DEATHBY_SQUASHED,
	DEATHBY_EXPLOSION,
	DEATHBY_ELECTRICSHOCK,
	DEATHBY_CHOCOLATE,
};


extern GAMETILE *currTile[];
extern GAMETILE *destTile[];
extern GAMETILE *prevTile;

extern unsigned long ice[];

extern int frogFacing[4];
extern int nextFrogFacing[4];

extern float frogGravity;

extern unsigned long standardHopFrames;
extern unsigned long superHopFrames;
extern unsigned long doubleHopFrames;
extern unsigned long quickHopFrames;
extern unsigned long floatFrames;
extern float superGravity;
extern float hopGravity;
extern float frogGravity;
extern float doubleGravity;
extern float floatGravity;

extern BOOL cameoMode;

// ----- [ FUNCTION PROTOTYPES ] ---------- //

void SetFroggerStartPos(GAMETILE *startTile,long p);
void UpdateFroggerPos(long p);
BOOL MoveToRequestedDestination(int dir,long pl);
GAMETILE *GetNextTile(unsigned long *pdir,long pl);
void SlideFroggerPosition(ACTOR2 *actor2);
BOOL GameTileTooHigh(GAMETILE *tile,long pl);
BOOL KillFrog(long pl);
void RotateFrog(ACTOR2* frog,unsigned long fFacing);
void AnimateFrogHop(unsigned long direction,long pl);
long GetTilesMatchingDirection(GAMETILE *me, long direction, GAMETILE *next);
void PushFrog(VECTOR *where, VECTOR *direction, long pl);
void HopFrogToTile(GAMETILE *tile, long pl);
void SpringFrogToTile(GAMETILE *tile, float height, float time, long pl);
void ThrowFrogAtScreen(long pl);



#endif
