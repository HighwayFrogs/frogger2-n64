/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: particle.c
	Programmer	: Jim Hubbard
	Date		: 12/03/00

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI_2

#include <ultra64.h>
#include "incs.h"

#define MAX_PARTICLES 256

PARTICLELIST partList;


/*	--------------------------------------------------------------------------------
	Function		: AllocateParticles
	Purpose			: Find a number of particles and return a sublist
	Parameters		: number
	Returns			: pointer to first one
	Info			: 
*/
PARTICLE *AllocateParticles( int number )
{
	PARTICLE *p;
	// Return if allocation is impossible for any reason
	if( (number <= 0) || (partList.stackPtr-number < 0) || (number >= MAX_PARTICLES-partList.count) ) return NULL;

	// Now we can go and allocate particles with gay abandon
	while( number-- )
	{
		p = partList.stack[partList.stackPtr--];

		p->prev	= &partList.head;
		p->next	= partList.head.next;
		partList.head.next->prev = p;
		partList.head.next = p;

		partList.count++;
	}

	return partList.stack[partList.stackPtr+1];
}


/*	--------------------------------------------------------------------------------
	Function		: DeallocateParticles
	Purpose			: Remove particles from list and flag as unused
	Parameters		: number of particles
	Returns			: 
	Info			: 
*/
void DeallocateParticles( PARTICLE *head, int number )
{
	PARTICLE *p=head, *t;

	if( !p || !p->next || (number<=0) || (partList.stackPtr+number >= MAX_PARTICLES) ) return;

	while( number-- )
	{
		t = p->next;

		p->prev->next	= p->next;
		p->next->prev	= p->prev;
		p->next = NULL;

		// Deallocate any random data that may exist and set everything to zero
		if( p->poly ) JallocFree( (UBYTE **)&p->poly );
		if( p->rMtrx ) JallocFree( (UBYTE **)&p->rMtrx );
		memset( p, 0, sizeof(PARTICLE) );

		partList.count--;
		partList.stack[++partList.stackPtr] = p;

		p = t;
	}
}


/*	--------------------------------------------------------------------------------
	Function		: FreeParticleList
	Purpose			: Deallocate all particles and set list to empty
	Parameters		: 
	Returns			: 
	Info			: 
*/
void FreeParticleList( )
{
	int i;

	if( partList.array )
	{
		// Remove all particles in array from particle list so they don't get removed after deallocation
		for( i=0; i<MAX_PARTICLES; i++ )
		{
			if( !partList.array[i].next ) continue;

			partList.array[i].prev->next	= partList.array[i].next;
			partList.array[i].next->prev	= partList.array[i].prev;
			partList.array[i].next = NULL;
		}

		JallocFree( (UBYTE **)&partList.array );
		partList.array = NULL;
	}

	if( partList.stack )
	{
		JallocFree( (UBYTE **)&partList.stack );
		partList.stack = NULL;
	}

	partList.count = 0;
	partList.stackPtr = 0;
}


/*	--------------------------------------------------------------------------------
	Function		: InitParticleList
	Purpose			: Set up static array and some bits of data.
	Parameters		: 
	Returns			: 
	Info			: 
*/
void InitParticleList( )
{
	int i;

	partList.head.next = partList.head.prev = &partList.head;

	// Allocate a big bunch of particles
	partList.array = (PARTICLE *)JallocAlloc( sizeof(PARTICLE)*MAX_PARTICLES, YES, "Particles" );
	partList.stack = (PARTICLE **)JallocAlloc( sizeof(PARTICLE*)*MAX_PARTICLES, YES, "SStack" );

	// Initially, all particles are available
	for( i=0; i<MAX_PARTICLES; i++ )
		partList.stack[i] = &partList.array[i];

	partList.count = 0;
	partList.stackPtr = i-1;
}

