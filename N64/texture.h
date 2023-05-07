/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: texture.h
	Programmer	: Andy Eder
	Date		: 19/04/99 12:22:29

----------------------------------------------------------------------------------------------- */

#ifndef TEXTURE_H_INCLUDED
#define TEXTURE_H_INCLUDED


#define MAX_TEXTURE_BANKS		5

#define TEX_ANIM_FLAGS_REVERSE	(1<<0)
#define TEX_ANIM_FLAGS_FLIPFLOP (1<<1)


typedef struct
{
	char	*data;
	char	*freePtr;
	int		numTextures;

}TEXTURE_BANK;


extern TEXTURE_BANK	textureBanks[MAX_TEXTURE_BANKS];
extern unsigned long numTextureBanks;


void LoadTextureBank(int num);
void InitTextureBanks();
void FindTexture(TEXTURE **texPtr, int texID, BOOL report);
void AnimateTexture(TEXTURE_ANIMATION *anim,Gfx *dl);


#endif