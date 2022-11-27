/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: general.h
	Programmer	: Matthew Cloy
	Date		: 12/01/99

----------------------------------------------------------------------------------------------- */


#ifndef GENERAL_H_INCLUDED
#define GENERAL_H_INCLUDED

#define inline __inline


typedef struct
{
	unsigned short time;
	unsigned long lastCount;

} TIMER;

#define GTInit( t, s ) { (t)->time = s; (t)->lastCount = 0; }
#define GTUpdate( t, s ) { if(actFrameCount > ((t)->lastCount+(60*Fabs(s)))){(t)->lastCount=actFrameCount; (t)->time+=s;} }

extern unsigned long gstrcmp(char *a, char *b);
extern unsigned long gstrlen(const char *a);
extern void stringChange ( char *name );
inline extern void smemcpy( unsigned short *a, unsigned short *b, unsigned long size );
inline extern void lmemcpy( unsigned long *a, unsigned long *b, unsigned long size );
inline extern void cmemcpy( char *a, char *b, unsigned long size );

extern short FindClosestTileVector( VECTOR *dir, GAMETILE *tile );

extern GAMETILE* FindNearestTile(VECTOR v);
extern GAMETILE* FindNearestJoinedTile(GAMETILE *tile, VECTOR *pos);
extern GAMETILE *FindJoinedTileByDirection( GAMETILE *st, VECTOR *d );
extern GAMETILE *FindJoinedTileByDirectionConstrained( GAMETILE *st, VECTOR *d, float angle );
extern GAMETILE *FindJoinedTileByDirectionAndType( GAMETILE *st, VECTOR *d, int type );

#endif