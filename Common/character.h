/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: character.h
	Programmer	: jim Hubbard
	Date		: 22/12/99 10:21

----------------------------------------------------------------------------------------------- */
#ifndef __CHARACTER_H
#define __CHARACTER_H


#define MAX_SEARCH_DEPTH 16

// Types of command that can be issued to characters
enum
{
	AICOM_IDLE,
	AICOM_GOTO_TILE,
	AICOM_POINT_DIR,
	AICOM_FACE_DIR,
	AICOM_STOP,
};

// Modifiers to AICommand sequence
#define AICOMFLAG_INTERRUPT		(1 << 0)		// Replaces queue
#define AICOMFLAG_QUEUED		(1 << 1)		// Gets added to queue
#define AICOMFLAG_INSTANT		(1 << 2)		// Gets done right away, but leaves queue intact
#define AICOMFLAG_LOOP			(1 << 3)		// Repeat until bored
#define AICOMFLAG_TIMEOUT		(1 << 4)		// This task will timeout rather than complete
#define AICOMFLAG_COMPLETE		(1 << 5)		// Task finished, time to delete this command
#define AICOMFLAG_FAILED		(1 << 6)		// Couldn't get to destination, held in place etc.


enum
{
	CHAR_HUB,
	CHAR_SWAMPY,
	//etc...

	NUM_CHARACTERS
};


typedef struct _AIPATHNODE
{
	struct _AIPATHNODE *next, *prev;// Next and previous pathnode in path

	GAMETILE *tile;					// Node is on this tile
//	unsigned long start, end;		// Timing stuff
	VECTOR to;						// Vector to next node
	SPECFX *fx;						// Used in battle multiplayer mode to make tiles look dangerous

} AIPATHNODE;


typedef struct _AICOMMAND
{
	struct _AICOMMAND *next;		// List pointer

	unsigned char type;				// Go to tile, point, etc.
	unsigned long time,end;			// How long to do this for
	unsigned short flags;			// Instant, queued, etc.
	float offset;					// Prefered height above tile
	float speed;					// How fast to do this command

	GAMETILE *target;				// Go to here
	VECTOR dir;						// Point here

} AICOMMAND;


typedef struct _CHARACTER
{
	struct _CHARACTER *next;			// Ubiquitous list pointer

	ACTOR2 *act;						// The model
	
	VECTOR normal;						// Current up vector
	GAMETILE *inTile;					// Tile the enemy is in

	short type;							// Hub character, swampy or something else.
	AICOMMAND *command;					// A queue of commands
	AIPATHNODE *node, *path;			// Current node and the whole path

	void (*Update) (struct _CHARACTER*);// Call this to move the creature

} CHARACTER;


extern CHARACTER *hubChar;

extern CHARACTER *characterList;


extern CHARACTER *CreateAndAddCharacter( char *name, GAMETILE *start, float offset, unsigned char type );
extern void FreeCharacterList( );
extern void SubCharacter( CHARACTER *ch );

extern void ProcessCharacters( );
extern void IssueAICommand( CHARACTER *ch, AICOMMAND *command );

extern void TestPath( );

#endif