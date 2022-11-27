/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: graphic.h
	Programmer	: Andy Eder
	Date		: 06/08/99 12:38:46

----------------------------------------------------------------------------------------------- */

#ifndef GRAPHIC_H_INCLUDED
#define GRAPHIC_H_INCLUDED


#define MAX_SLICES 10


typedef struct
{
	
	int status;
	unsigned int start[MAX_SLICES];
	unsigned int end[MAX_SLICES];
	unsigned int total[MAX_SLICES];
	unsigned int frameStart;
	char name[31];
	char slice;

} TIMECOUNT;

extern TIMECOUNT timers[10];
extern unsigned int frameStart;
extern char timerMode;

extern TEXTOVERLAY *timerTxt[10];

void TIMER_InitTimers();
void TIMER_StartTimer(int number,char *name);
void TIMER_EndTimer(int number);
void TIMER_PrintTimers();
void TIMER_ClearTimers();



//----- [ TYPES ] ------------------------------------------------------------------------------//

#define BACKDROP_FILTER	(1<<0)
#define BACKDROP_WRAP	(1<<1)

typedef struct TAGBACKDROP
{
	struct			TAGBACKDROP *next,*prev;
	struct			TAGBACKDROP *parent;
	TEXTURE			*texture;
	UBYTE			r,g,b,a;
	int				xPos,yPos,zPos;
	short			scaleX,scaleY;
	short			draw;
	short			flags;
	uObjBg			background;

} BACKDROP;

typedef struct TAGBACKDROPLIST
{
	int				numEntries;
	BACKDROP		head;

} BACKDROPLIST;


//----- [ GLOBALS ] ----------------------------------------------------------------------------//

extern BACKDROPLIST backdropList;


//----- [ FUNCTION PROTOTYPES ] ----------------------------------------------------------------//

void InitBackdropLinkedList();
void FreeBackdropLinkedList();
void AddBackdrop(BACKDROP *bDrop);
void SubBackdrop(BACKDROP *bDrop);
BACKDROP *SetupBackdrop(BACKDROP *backdrop,int texID,int sourceX,int sourceY,int z,int destX,int destY,int destWidth,int destHeight,int scalex,int scaley,int flags);



#endif