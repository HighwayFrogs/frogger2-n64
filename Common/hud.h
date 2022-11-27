/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: hud.h
	Programmer	: James Healey
	Date		: 31/03/99

----------------------------------------------------------------------------------------------- */


#ifndef HUD_H_INCLUDED
#define HUD_H_INCLUDED

extern char countdownTimer;
extern char displayFullScreenPoly;

//extern ANIM_STRUCTURE *livesIcon;

//extern TEXTOVERLAY	*livesTextOver,*timeTextOver,*scoreTextOver;
extern TEXTOVERLAY	*babySavedText;

extern TEXTOVERLAY *continueText;
extern TEXTOVERLAY *restartText;
extern TEXTOVERLAY *quitText;
extern TEXTOVERLAY *garibCount, *creditCount;
extern TEXTOVERLAY *time;

extern void UpDateOnScreenInfo		( void );
extern void UpdateScore				( ACTOR2 *act, long scoreUpdate);
extern void InitInGameTextOverlays	( unsigned long worldID,unsigned long levelID );
extern void RunGameOverSequence		( void );
extern void DisableHUD(void);
extern void EnableHUD(void);
#endif