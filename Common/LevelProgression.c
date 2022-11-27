/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: LevelProgression.c
	Programmer	: James Healey
	yDate		: 18/06/99

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI_2

#include <ultra64.h>

#include "incs.h"

//************************************************************************************************
//	Global Defines

short keyFound;

//************************************************************************************************
//	Function Definitions

void RunLevelKeyFound ( void )
{
	keyFound--;

	if(keyFound)
	{
		wholeKeyText->a -= 8;
	}
	else
	{
		wholeKeyText->draw = 0;
		wholeKeyText->a = 255;
	}

}


