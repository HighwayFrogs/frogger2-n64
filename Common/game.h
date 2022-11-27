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

// -----------------

enum
{
	NORMAL_PMODE = 0,
	TWO_PMODE = 1,
	FOUR_PMODE = 2
};

extern struct gameStateStruct gameState;

extern unsigned long INPUT_POLLPAUSE;

extern unsigned long creditsActive;
extern unsigned long fadingLogos;

extern VECTOR *pointOfInterest;
extern float	pOIDistance;

extern TIMER scoreTimer;
extern TIMER modeTimer;

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

extern long award;

extern long numHops_TOTAL;
extern long speedHops_TOTAL;
extern long numHealth_TOTAL;

extern void RunGameLoop (void);
extern void RunFrontendGameLoop (void);

extern unsigned char swingCam;
extern char playDemos;

extern unsigned long currTileNum;
extern TEXTOVERLAY *tileNum;
extern char tileString[];
extern long displayingTile;

#endif