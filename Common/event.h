/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.
	
	  
	File		: event.h
	Programmer	: Jim Hubbard
	Date		: 30/06/99
	
----------------------------------------------------------------------------------------------- */

#ifndef EVENT_H_INCLUDED
#define EVENT_H_INCLUDED


/*----- [ DEFINES ] ----------------------------------------------------------------------------*/
#define TRIGGER_ONCE		(1<<0)
#define TRIGGER_DELAY		(1<<1)
#define TRIGGER_ALWAYS		(1<<2)
#define TRIGGER_FIRED		(1<<3)
#define TRIGGER_RISING		(1<<4)


/*----- [ STRUCTURES ] -------------------------------------------------------------------------*/

typedef struct TAGEVENT
{
	struct TAGEVENT *next, *prev;
	void (*func) (), **data;
} EVENT;

typedef struct
{
	EVENT head;	
	unsigned int numEntries;
} EVENTLIST;

typedef struct TAGTRIGGER
{
	struct TAGTRIGGER *next, *prev;
	void **data;
	int (*func) ();
	unsigned short flags;
	unsigned long count;
	unsigned long delay;
	EVENTLIST events;
} TRIGGER;

typedef struct
{
	TRIGGER head;	
	unsigned int numEntries;
} TRIGGERLIST;


/*----- [ DATA ] ------------------------------------------------------------------------------*/

extern TRIGGERLIST triggerList;


/*----- [ PROTOTYPES ] ------------------------------------------------------------------------*/

extern TRIGGER * MakeTrigger( int (*func)(TRIGGER *t), void **args );
extern EVENT * MakeEvent( void (*func)(EVENT *e), void **args );
extern void AttachEvent( TRIGGER *trigger, EVENT *event, unsigned short flags, unsigned long time );
extern void InitTriggerList( );
extern void InitEventList( );
extern void **AllocArgs( int numArgs );
extern void KillAllTriggers( );
extern void KillAllEvents( );
extern void SubTrigger( TRIGGER *t );
extern void UpdateEvents( );

#endif