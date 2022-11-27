/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: levelplaying.h
	Programmer	: James Healey
	Date		: 17/03/99

----------------------------------------------------------------------------------------------- */

#ifndef LEVELPLAYING_H_INCLUDED
#define LEVELPLAYING_H_INCLUDED

//----------------------------------------------------------------------------//
//----- DEFINE ---------------------------------------------------------------//
//----------------------------------------------------------------------------//

// Camera Action Flags
#define CAMERA_IN		( 1 << 0 )
#define CAMERA_OUT		( 1 << 1 )
#define CAMERA_LEFT		( 1 << 2 )
#define CAMERA_RIGHT	( 1 << 3 )

// Frogger Action Flags
#define MOVEMENT_UP		( 1 << 4 )
#define MOVEMENT_DOWN	( 1 << 5 )
#define MOVEMENT_LEFT	( 1 << 6 )
#define MOVEMENT_RIGHT	( 1 << 7 )

//----------------------------------------------------------------------------//
//----- TYPDE DEFS -----------------------------------------------------------//
//----------------------------------------------------------------------------//
typedef struct _PLAYINGACTIONS
{
	struct _PLAYINGACTIONS *nextAction;
	unsigned long frameNum;
	unsigned long actions;

} PLAYINGACTIONS;

typedef struct _LEVELPLAYING
{
	PLAYINGACTIONS *actions;
} LEVELPLAYING;

//----------------------------------------------------------------------------//
//----- GLOBALS --------------------------------------------------------------//
//----------------------------------------------------------------------------//
extern unsigned long recordKeying;
extern LEVELPLAYING *levelPlaying;

//----------------------------------------------------------------------------//
//----- FUNCTIONS EXTERNS ----------------------------------------------------//
//----------------------------------------------------------------------------//
extern u16 GetCurrentRecordKey		( void );

extern void AddPlayingActionToList	( unsigned long flags, unsigned long frameNum );

// Free Functions
extern void FreeLevelPlaying		( void );
extern void FreePlayingActions		( void );

#endif