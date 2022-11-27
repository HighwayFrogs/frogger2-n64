/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: backdrops.c
	Programmer	: Andy Eder
	Date		: 19/04/99 09:43:13

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI

#include <ultra64.h>

#include "incs.h"


//----- [ GLOBALS ] ----------------------------------------------------------------------------//


//----- [ BACKDROP HANDLING ] ------------------------------------------------------------------//

/*	--------------------------------------------------------------------------------
	Function		: CreateAndInitBackdrop
	Purpose			: creates and initialises a backdrop structure
	Parameters		: texture filename to be used as the backdrop
	Returns			: BACKDROP*
	Info			:
*/
BACKDROP *CreateAndInitBackdrop(char *txtrName)
{
	BACKDROP *bDrop;

	dprintf"Creating and initialising backdrop %s...",txtrName));

	bDrop			= (BACKDROP *)JallocAlloc(sizeof(BACKDROP),YES,"BDROP");

	bDrop->draw		= 1;
	bDrop->scaleX	= 1024;
	bDrop->scaleY	= 1024;
	bDrop->r		= 127;
	bDrop->g		= 127;
	bDrop->b		= 127;
	bDrop->a		= 255;

	bDrop->xPos		= 0;
	bDrop->yPos		= 0;

	FindTexture(&bDrop->texture,UpdateCRC(txtrName),YES, txtrName);

	if(bDrop)
		dprintf"done !\n"));
	else
		dprintf"error !\n"));

	return bDrop;
}


/*	--------------------------------------------------------------------------------
	Function		: FreeBackdrop
	Purpose			: frees specified backdrop
	Parameters		: BACKDROP *
	Returns			: void
	Info			:
*/
void FreeBackdrop(BACKDROP *bDrop)
{
	if(bDrop)
		JallocFree((UBYTE**)&bDrop);
}
