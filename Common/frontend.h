/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: frontend.h
	Programmer	: Andy Eder (originally Random Healey, but now rewritten)
	Date		: 14/10/99 11:58:26

----------------------------------------------------------------------------------------------- */

#ifndef FRONTEND_H_INCLUDED
#define FRONTEND_H_INCLUDED


typedef struct
{
	char *texname;
	unsigned short width,height;
	unsigned short tiles[16];
} LOGO;


//----- [ GLOBALS ] ----------------------------------------------------------------------------//

extern int frameCount;
extern int frameCount2;
extern int dispFrameCount;

extern char	ActiveController;


//----- [ FUNCTION PROTOTYPES ] ----------------------------------------------------------------//

extern void GameLoop();

extern void RunLevelSelect();
extern void RunTitleScreen();

// Init routines
extern void	StartLevelComplete( );
extern void	StartWorldComplete( );
extern void	StartGameComplete( );
extern void StartGameIntro( );
extern void StartGameOver();

// Run routines
extern void	RunLevelComplete( );
extern void	RunWorldComplete( );
extern void	RunGameComplete( );
extern void	RunGameOver( );
extern void RunGameIntro( );


extern void CreateOverlaysFromLogo(const LOGO *logo, int x, int y);

#endif