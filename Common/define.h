/*

	This file is part of Frogger2, (c) 1997 Interactive Studios Ltd.


	File		: define.h
	Programmer	: Matthew Cloy
	Date		: 10/11/98

----------------------------------------------------------------------------------------------- */

#ifndef DEFINE_H_INCLUDED
#define DEFINE_H_INCLUDED


//------------------------------------------------------------------------------------------------

enum gameModes
{
	//  Frontend / Title screen Modes
	FRONTEND_MODE,

	GAME_MODE,
	INGAME_MODE,
	MENU_MODE,

	CAMEO_MODE,

	PAUSE_MODE,

	GHOST_MODE,							// Mode To Whit Your Self Against Your Self

	LEVELPLAYING_MODE,
	RECORDKEY_MODE,

	SNDVIEW_MODE,
	OBJVIEW_MODE,
	WATERVIEW_MODE,

	TITLE_MODE,
	LEVELSELECT_MODE,
	DEVELOPMENT_MODE,
};

#define SINGLEPLAYER	1
#define MULTILOCAL		2
#define MULTIREMOTE		3

struct gameStateStruct
{
	unsigned char multi;
	unsigned long mode;
	unsigned long oldMode;
	unsigned long menuMode;
};

//------------------------------------------------------------------------------------------------

enum
{
	TILESTATE_NORMAL = 0,
	TILESTATE_DEADLY,
	TILESTATE_SINK,
	TILESTATE_ICE,
	TILESTATE_SUPERHOP,
	TILESTATE_JOIN,
	TILESTATE_SAFE,
	TILESTATE_RESERVED,
	TILESTATE_GRAPPLE,
	TILESTATE_SMASH,
	TILESTATE_BARRED,

	TILESTATE_FROGGER1AREA,
	TILESTATE_FROGGER2AREA,
	TILESTATE_FROGGER3AREA,
	TILESTATE_FROGGER4AREA,

	TILESTATE_CONVEYOR0SLOW = 0x20,
	TILESTATE_CONVEYOR1SLOW,
	TILESTATE_CONVEYOR2SLOW,
	TILESTATE_CONVEYOR3SLOW,

	TILESTATE_CONVEYOR0MED	= 0x24,
	TILESTATE_CONVEYOR1MED,
	TILESTATE_CONVEYOR2MED,
	TILESTATE_CONVEYOR3MED,

	TILESTATE_CONVEYOR0FAST = 0x28,
	TILESTATE_CONVEYOR1FAST,
	TILESTATE_CONVEYOR2FAST,
	TILESTATE_CONVEYOR3FAST,
};

#define TILESTATE_CONVEYOR		0x20
#define TILESTATE_CONVEYOR_MED	0x24
#define TILESTATE_CONVEYOR_FAST	0x28

typedef struct _GAMETILE_INFO
{
	VECTOR shading;
} GAMETILE_INFO;

typedef struct _GAMETILE
{
	struct _GAMETILE	*tilePtrs[4];		// north, south, east and west
	struct _GAMETILE	*next;				// next tile in linked list
	
	GAMETILE_INFO		*gameInfo;			// tile to teleport to if this tile is a teleporter

	unsigned char		state;				// state of tile
	
	VECTOR		centre;						// centre position
	VECTOR		normal;						// face normal

	VECTOR		dirVector[4];				// the 4 possible camera vectors for this tile

} GAMETILE;

// Scenic objects //

#define	TYPE_NORMAL    (0)
#define	TYPE_HASPATH   (1<<0)
#define	TYPE_HASSPIN   (1<<1)
#define	TYPE_MAINWORLD (1<<2)

typedef struct _TYPEDATA
{
	unsigned long noPts;	
	VECTOR *movementPath;
	VECTOR *allocedVect;
	unsigned long animSpeed;
	unsigned long ofsPts;
	unsigned long dirn;
} TYPEDATA;

typedef struct _SCENIC
{
	struct _SCENIC *next;
	char name[32];
	VECTOR pos;
	QUATERNION rot;
	unsigned long type;
	TYPEDATA *typedata;
	GAMETILE *tileptrs;
} SCENIC;

////////////////////

extern char statusMessage[255];
//------------------------------------------------------------------------------------------------

#endif
