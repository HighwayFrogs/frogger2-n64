/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: general.c
	Programmer	: Matthew Cloy
	Date		: 12/01/99

----------------------------------------------------------------------------------------------- */

#include <ultra64.h>

#include "incs.h"

char statusMessage[255];

unsigned long gstrlen (const char *a)
{
	char *b = (char*)a;
	unsigned long i;
	for (i=0; b[0]==0; b++,i++);
	return i;
}

unsigned long gstrcmp(char *a,char *b)
{
	char *x = a,*y = b;

	while ((x[0] && y[0]) && (x[0]==y[0]))
	{
		x++;
		y++;
	}
	
	if (x[0]!=0)
		return 1;

	if (y[0]!=0)
		return 1;
	
	return 0;
}

void stringChange ( char *name )
{
	char *c, *d, *e;
	const char *obe = ".obe";
	const char *ndo = ".ndo";

	for(c = name; *c; c++)
	{
		d = (char*)ndo, e = c;
		while(*e == *d)	// strstr!
		{
			e++, d++;
			if (!*d) {	// end of ".obe" - found substring
				d = (char*)obe;
				while(*d) *(c++) = *(d++);	// strcpy!
				return;
			}
		}
	}
}

inline void cmemcpy( char *a, char *b, unsigned long size )
{
	while( size-- )
		a[size] = b[size];
}

inline void smemcpy( unsigned short *a, unsigned short *b, unsigned long size )
{
	while( size-- )
		a[size] = b[size];
}

inline void lmemcpy( unsigned long *a, unsigned long *b, unsigned long size )
{
	while( size-- )
		a[size] = b[size];
}


/*	--------------------------------------------------------------------------------
    Function		: FindClosestTileVector
	Parameters		: Direction to match, gametile
	Returns			: direction vector index
*/
short FindClosestTileVector( VECTOR *dir, GAMETILE *tile )
{
	int i, match=0;
	float best=-1, dp;

	for( i=0; i<4; i++ )
	{
		dp = DotProduct( &tile->dirVector[i], dir );
		if( dp > best )
		{
			best = dp;
			match = i;
		}
	}

	return match;
}


/*	--------------------------------------------------------------------------------
    Function		: FindNearestTile
	Parameters		: VECTOR
	Returns			: GAMETILE*
*/
GAMETILE* FindNearestTile(VECTOR v)
{
	GAMETILE *t, *closest;
	int dist, closestDist;

	closest = NULL; closestDist = 0x10000000;

	for (t = firstTile; t != NULL; t = t->next)
	{
		dist = DistanceBetweenPointsSquared(&v, &t->centre);

		if (dist < closestDist)
		{
			closest = t;
			closestDist = dist;
		}
	}

	return closest;
}

GAMETILE* FindNearestJoinedTile( GAMETILE *tile, VECTOR *pos )
{
	GAMETILE *t, *closest = tile;
	int i, j;
	float dist,
		closestDist = DistanceBetweenPointsSquared(pos,&tile->centre); // Closest is current tile initially

	for( i=0; i<4; i++ )
	{
		t = tile->tilePtrs[i];
		if( t )
		{
			if( (t->state != TILESTATE_BARRED) && (t->state != TILESTATE_JOIN) )
			{
				dist = DistanceBetweenPointsSquared(pos, &t->centre);
				if (dist < closestDist)
				{
					closest = t;
					closestDist = dist;
				}
			}

			for( j=0; j<4; j++ )
			{
				if( t->tilePtrs[j] )
				if( (t->tilePtrs[j]->state != TILESTATE_BARRED) && (t->tilePtrs[j]->state != TILESTATE_JOIN) )
				{
					dist = DistanceBetweenPointsSquared(pos, &t->tilePtrs[j]->centre);
					if (dist < closestDist)
					{
						closest = t->tilePtrs[j];
						closestDist = dist;
					}
				}
			}
		}
	}

	return closest;
}

GAMETILE *FindJoinedTileByDirectionAndType( GAMETILE *st, VECTOR *d, int type )
{
	float distance = 100000, t;
	int i;
	GAMETILE *res = NULL;

	for (i=0; i<4; i++)
	{
		t = DotProduct( d, &st->dirVector[i] );
		if( (t < distance) && st->tilePtrs[i] )
		if( st->tilePtrs[i]->state == type )
		{
			distance = t;
			res = st->tilePtrs[i];
		}
	}

	return res;
}

GAMETILE *FindJoinedTileByDirection( GAMETILE *st, VECTOR *d )
{
	float distance = 100000, t;
	int i;
	GAMETILE *res = NULL;

	for (i=0; i<4; i++)
	{
		t = DotProduct( d, &st->dirVector[i] );
		if( (t < distance) && st->tilePtrs[i] )
		if( (st->tilePtrs[i]->state != TILESTATE_DEADLY) && (st->tilePtrs[i]->state != TILESTATE_SINK) && (st->tilePtrs[i]->state != TILESTATE_BARRED) && (st->tilePtrs[i]->state != TILESTATE_JOIN) )
		{
			distance = t;
			res = st->tilePtrs[i];
		}
	}

	return res;
}

// As above, but constrained to an angle
GAMETILE *FindJoinedTileByDirectionConstrained( GAMETILE *st, VECTOR *d, float angle )
{
	float distance = 100000, t;
	int i;
	GAMETILE *res = NULL;

	for (i=0; i<4; i++)
	{
		t = DotProduct( d, &st->dirVector[i] );
		if( (t < distance) && (t < cosf(angle)) && st->tilePtrs[i] )
		if( (st->tilePtrs[i]->state != TILESTATE_DEADLY) && (st->tilePtrs[i]->state != TILESTATE_SINK) && (st->tilePtrs[i]->state != TILESTATE_BARRED) && (st->tilePtrs[i]->state != TILESTATE_JOIN) )
		{
			distance = t;
			res = st->tilePtrs[i];
		}
	}

	return res;
}
