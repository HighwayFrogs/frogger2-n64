/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: backdrops.h
	Programmer	: Andy Eder
	Date		: 19/04/99 09:43:32

----------------------------------------------------------------------------------------------- */

#ifndef BACKDROPS_H_INCLUDED
#define BACKDROPS_H_INCLUDED

#include "types2dgfx.h"

//----- [ GLOBALS ] ----------------------------------------------------------------------------//

extern BACKDROP		*myBackdrop;


//----- [ FUNCTION PROTOTYPES ] ----------------------------------------------------------------//

extern BACKDROP *CreateAndInitBackdrop(char *txtrName);
extern void FreeBackdrop(BACKDROP *bDrop);


#endif
