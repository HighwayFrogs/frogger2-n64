/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: game.h
	Programmer	: Matthew Cloy
	Date		: 12/11/98

----------------------------------------------------------------------------------------------- */

#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED


// Tile status flags.
#define ATTACH (1<<0)


// Various modes where the game is paused
#define PM_PAUSE	1
#define PM_ENDLEVEL	2
#define PM_GAMEOVER	3

#ifndef DEFINITELY_NOT_MBR_DEMO
//#define MBR_DEMO 1
#endif

// -----------------

enum
{
	NORMAL_PMODE = 0,
	TWO_PMODE = 1,
	FOUR_PMODE = 2
};

extern long carryOnBabies;

extern struct gameStateStruct gameState;

extern unsigned long INPUT_POLLPAUSE;

extern VECTOR *pointOfInterest;
extern float	pOIDistance;

extern TIMER gameIsOver, levelIsOver, scoreTimer;
extern short showEndLevelScreen;

extern unsigned short screenNum;

extern unsigned long autoPlaying;
extern unsigned long recordKeying;

extern long move;
extern long playMode;
//extern long multiplayerRun;

extern GAMETILE *firstTile;
extern GAMETILE **gTStart;

extern ACTOR2 *levelTrophy;

extern char	scoreText[32];
extern char	timeText[32];
extern long hopAmt;

extern long babySaved;
extern long award;

extern long numHops_TOTAL;
extern long speedHops_TOTAL;
extern long numHealth_TOTAL;

extern void RunGameLoop (void);
extern void Orientate(QUATERNION *me, VECTOR *fd, VECTOR *mfd, VECTOR *up);
extern void RunLevelCompleteSequence();

extern unsigned char swingCam;

#endif