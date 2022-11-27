/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: particle.h
	Programmer	: Jim Hubbard
	Date		: 12/03/00

----------------------------------------------------------------------------------------------- */


#ifndef PARTICLE_H_INCLUDED
#define PARTICLE_H_INCLUDED


typedef struct _PARTICLE
{
	struct _PARTICLE *next, *prev;

	VECTOR pos, vel;
	VECTOR *poly;								// Used for motion trails
	float *rMtrx;
	unsigned char r, g, b, a, bounce;

} PARTICLE;


typedef struct
{
	// Statically allocated array of particles
	PARTICLE *array;
	// Number of particles allocated
	short count;

	// Defines and data for the particle allocation stack
	PARTICLE **stack;
	short stackPtr;

	// Static head of list
	PARTICLE head;

} PARTICLELIST;


extern PARTICLELIST partList;


PARTICLE *AllocateParticles( int number );
void DeallocateParticles( PARTICLE *head, int number );

void InitParticleList( );
void FreeParticleList( );


#endif