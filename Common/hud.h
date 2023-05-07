/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: hud.h
	Programmer	: James Healey
	Date		: 31/03/99

----------------------------------------------------------------------------------------------- */


#ifndef HUD_H_INCLUDED
#define HUD_H_INCLUDED

extern char scoreText[32];
extern char livesText[8];
extern char timeText[32];

extern char timeTemp[6];


extern char countdownTimer;
extern char displayFullScreenPoly;

extern ANIM_STRUCTURE *livesIcon;

extern TEXTOVERLAY	*livesTextOver,*timeTextOver,*scoreTextOver;
extern TEXTOVERLAY	*gameOver1;
extern TEXTOVERLAY	*gameOverScore;
extern TEXTOVERLAY	*babySavedText;

extern TEXTOVERLAY	*levelComplete1,
					*levelComplete2,
					*levelComplete3;

extern TEXTOVERLAY  *keyCollected;

//extern TEXTOVERLAY *pauseTitle;

extern TEXTOVERLAY *continueText;
extern TEXTOVERLAY *quitText;
extern TEXTOVERLAY *spawnCollected;
extern TEXTOVERLAY *time;

extern TEXTOVERLAY *nextLev1;
extern TEXTOVERLAY *nextLev2;


extern SPRITEOVERLAY *clock;
extern SPRITEOVERLAY *spawn;

extern TEXTOVERLAY *wholeKeyText;

extern void UpDateOnScreenInfo		( void );
extern void UpdateScore				( ACTOR2 *act, long scoreUpdate);
extern void InitInGameTextOverlays	( unsigned long worldID,unsigned long levelID );
extern void RunGameOverSequence		( void );

#endif