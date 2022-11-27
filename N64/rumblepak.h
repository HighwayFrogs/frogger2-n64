/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: rumblepak.h
	Programmer	: Andy Eder
	Date		: 23/11/99

----------------------------------------------------------------------------------------------- */

#ifndef RUMBLEPAK_H_INCLUDED
#define RUMBLEPAK_H_INCLUDED


extern char anyRumblePresent;

typedef struct
{
	short stopCount;
	short rumbleOn;
	float freq;
	float inertia;
	float rumbleCount;
	int lifetime;
	short delay;

} RUMBLE;


void InitRumble();
void UpdateRumble();
void StartRumble(float freq,float inertia,short life,short num,char *string);
void StopAllRumbles();
void StartDelayedRumble(float freq,float inertia,short life,short num,char* string,short delay);


#endif