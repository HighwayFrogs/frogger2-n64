/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: types2dgfx.h
	Programmer	: Andy Eder
	Date		: 20/04/99 09:13:49

----------------------------------------------------------------------------------------------- */

#ifndef TYPES2DGFX_H_INCLUDED
#define TYPES2DGFX_H_INCLUDED


//----- [ DEFINES ] ----------------------------------------------------------------------------//

#define MAX_SPR_FRAMES	32


//----- [ DATA STRUCTURES ] --------------------------------------------------------------------//

typedef struct TAGTEXTOVERLAY
{
	struct		TAGTEXTOVERLAY *next,*prev;

	char		*text;
	char		flags;
	int			ID;
	float		waveAmplitude;
	float		waveStart;
	float		xPos,yPos;
	float		xPosTo, yPosTo;
	short		wtotal;
	float		scale;
	short		centred;
	short		kill;
	short		life;

	float		speed;

	UBYTE		r,g,b,a,oa;

	UBYTE		draw;
	FONT		*font;

} TEXTOVERLAY;


typedef struct TAGSPRITEOVERLAY
{
	struct		TAGSPRITEOVERLAY *next,*prev;

	float		xPos, yPos;
	float		xPosTo, yPosTo;
	short		width;
	short		height;

	short		flags;
	float		speed;
	
	float		r,g,b,a;
	UBYTE		draw;
	long		num;
	short		numFrames;
	short		currFrame;
	float		animTime;
	float		animSpeed;
	TEXTURE		*frames[MAX_SPR_FRAMES];

} SPRITEOVERLAY;


typedef struct TAGSPRITE3D
{
	struct		TAGSPRITE3D *next;

	TEXTURE		*texture;
	float		xPos;
	float		yPos;
	short		zPos;

	float		scaleX;
	float		scaleY;

	UBYTE		r,g,b,a;
	UBYTE		draw;

	Sprite		spr;
	Bitmap		*bmp;

} SPRITE3D;



#endif
