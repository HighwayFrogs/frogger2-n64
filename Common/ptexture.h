/*
	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: ptexture.h
	Purpose		: Procedurally generated textures
	Programmer	: Jim Hubbard
	Date		: 2/12/99 10:30

----------------------------------------------------------------------------------------------- */
#ifndef __PTEXTURE_H
#define __PTEXTURE_H


typedef struct TAGPROCTEXTURE
{
	struct TAGPROCTEXTURE *next;

	unsigned long timer;
	unsigned char *buf1, *buf2, active;
	short *palette;

	TEXTURE *tex;

	void  (*Update) ();

} PROCTEXTURE;


extern PROCTEXTURE *steamTex;

extern short lightX, lightY;


extern PROCTEXTURE *prcTexList;
extern POLYGON *rpList;

extern void CreateAndAddProceduralTexture( TEXTURE *tex, char *name );
extern POLYGON *CreateAndAddRandomPoly( TEXTURE *tex, VECTOR *pos, VECTOR *normal, float w, float h );
extern void ProcessProcTextures( );
extern void FreeProcTextures( );
extern void FreeRandomPolyList( );

extern void ProcessPTFire( PROCTEXTURE *pt );
extern void ProcessPTForcefield( PROCTEXTURE *pt );
extern void ProcessPTWaterRipples( PROCTEXTURE *pt );
extern void ProcessPTWaterDrops( PROCTEXTURE *pt );
extern void ProcessPTWaterBubbler( PROCTEXTURE *pt );
extern void ProcessPTWaterTrail( PROCTEXTURE *pt );
extern void ProcessPTWaterWaves( PROCTEXTURE *pt );
extern void ProcessPTSteam( PROCTEXTURE *pt );
extern void ProcessPTCandle( PROCTEXTURE *pt );

extern void ProcessPTFrogger( PROCTEXTURE *pt );

extern void ProcessPTWaterRipplesBM( PROCTEXTURE *pt );

#endif