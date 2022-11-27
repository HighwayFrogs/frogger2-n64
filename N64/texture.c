/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: texture.c
	Programmer	: Andy Eder
	Date		: 19/04/99 12:39:48

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI_2

#include <ultra64.h>

#include "incs.h"


TEXTURE_BANK	textureBanks[MAX_TEXTURE_BANKS];		//final texture bank is for font
TEXTURE *tempTexture;

unsigned long numTextureBanks = 0;


/*	--------------------------------------------------------------------------------
	Function		: FreeAllTextureBanks
	Purpose			: frees up ALL texture banks
	Parameters		: 
	Returns			: void
	Info			: NOTE: leaves system texture bank alone !
*/
void FreeAllTextureBanks()
{
	unsigned long i;

	for(i=0; i<MAX_TEXTURE_BANKS; i++)
	{
		if(textureBanks[i].freePtr)
		{
			dprintf"Freeing TBank %d...",i));
			JallocFree((UBYTE**)&textureBanks[i].freePtr);
			dprintf"done !\n",i));
		}
	}
	numTextureBanks = 0;
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void InitTextureBanks()
{
	int x;

	for(x = 0; x < MAX_TEXTURE_BANKS; x++)
	{
		textureBanks[x].freePtr = NULL;
		textureBanks[x].data = NULL;
		textureBanks[x].numTextures = 0;
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: LoadTexureBank
	Purpose 	: dma's a texture bank from rom into ram.
	Parameters 	: char *texturebank
	Returns 	: none
	Info 		:
*/
void LoadTextureBank(int num)
{
	char	*textureBank;
	u32		bankRomStart, bankRomEnd, bankSize;
	short	x = 0, y = 0;
	TEXTURE *tex;
	char	*temp;

	switch(num)
	{
		case SYSTEM_TEX_BANK:
			bankRomStart	= (u32)&_texBank1SegmentRomStart;
			bankRomEnd		= (u32)&_texBank1SegmentRomEnd;
			dprintf"SYSTEM TEXTURE BANK - "));
			break;
		case TITLESGENERIC_TEX_BANK:
			bankRomStart	= (u32)&_texBank2SegmentRomStart;
			bankRomEnd		= (u32)&_texBank2SegmentRomEnd;
			dprintf"GENERIC TITLES TEXTURE BANK - "));
			break;
		case INGAMEGENERIC_TEX_BANK:
			bankRomStart	= (u32)&_texBank3SegmentRomStart;
			bankRomEnd		= (u32)&_texBank3SegmentRomEnd;
			dprintf"GENERIC IN-GAME TEXTURE BANK - "));
			break;
		
		// WORLD BANKS ---------------------------------------------------------------------------

		case FRONTEND_TEX_BANK:
			bankRomStart	= (u32)&_texBankFrontendSegmentRomStart;
			bankRomEnd		= (u32)&_texBankFrontendSegmentRomEnd;
			dprintf"FRONTEND TEXTURE BANK - "));
			break;

		case GARDEN_TEX_BANK:
			bankRomStart	= (u32)&_texBankGardenSegmentRomStart;
			bankRomEnd		= (u32)&_texBankGardenSegmentRomEnd;
			dprintf"GARDEN TEXTURE BANK - "));
			break;

		case ANCIENT_TEX_BANK:
			bankRomStart	= (u32)&_texBankAncientSegmentRomStart;
			bankRomEnd		= (u32)&_texBankAncientSegmentRomEnd;
			dprintf"ANCIENTS TEXTURE BANK - "));
			break;
		
		case SPACE_TEX_BANK:
			bankRomStart	= (u32)&_texBankSpaceSegmentRomStart;
			bankRomEnd		= (u32)&_texBankSpaceSegmentRomEnd;
			dprintf"SPACE TEXTURE BANK - "));
			break;
		
		case CITY_TEX_BANK:
			bankRomStart	= (u32)&_texBankCitySegmentRomStart;
			bankRomEnd		= (u32)&_texBankCitySegmentRomEnd;
			dprintf"CITY TEXTURE BANK - "));
			break;
		
		case SUBTERRANEAN_TEX_BANK:
			bankRomStart	= (u32)&_texBankSubSegmentRomStart;
			bankRomEnd		= (u32)&_texBankSubSegmentRomEnd;
			dprintf"SUBTERRANEAN TEXTURE BANK - "));
			break;
		
		case LABORATORY_TEX_BANK:
			bankRomStart	= (u32)&_texBankLabSegmentRomStart;
			bankRomEnd		= (u32)&_texBankLabSegmentRomEnd;
			dprintf"LABORATORY TEXTURE BANK - "));
			break;
		
		case TOYSHOP_TEX_BANK:
			bankRomStart	= (u32)&_texBankToySegmentRomStart;
			bankRomEnd		= (u32)&_texBankToySegmentRomEnd;
			dprintf"TOYSHOP TEXTURE BANK - "));
			break;
		
		case HALLOWEEN_TEX_BANK:
			bankRomStart	= (u32)&_texBankHallowSegmentRomStart;
			bankRomEnd		= (u32)&_texBankHallowSegmentRomEnd;
			dprintf"HALLOWEEN TEXTURE BANK - "));
			break;
		
		case SUPERRETRO_TEX_BANK:
			bankRomStart	= (u32)&_texBankSRetroSegmentRomStart;
			bankRomEnd		= (u32)&_texBankSRetroSegmentRomEnd;
			dprintf"SUPERRETRO TEXTURE BANK - "));
			break;

		default:
			dprintf"Invalid texture bank!\n"));
			for(;;);
			break;
	}
	bankSize = bankRomEnd - bankRomStart;

	if(num != SYSTEM_TEX_BANK)
		textureBank = (char *)JallocAlloc(DMAGetSize(bankRomStart,bankRomEnd),YES,"TXTRBANK");
	else
	{
		smallFont->data = (char *)JallocAlloc(DMAGetSize(bankRomStart,bankRomEnd),YES,"SYSTXTR");
		oldeFont->data = &smallFont->data[0];
		textureBank = &smallFont->data[0];
	}

	DMAMemory(textureBank, bankRomStart, bankRomEnd);
	if(textureBank)
		dprintf"LOADED OK (size: %d)\n",bankSize));
	else
	{
		dprintf"ERROR : UNABLE TO LOAD TEXTURE BANK\n"));
		return;
	}

	if(num != SYSTEM_TEX_BANK)
	{
		//store a pointer to the texture bank so it may be free'd at a later date	
		while(textureBanks[numTextureBanks].freePtr != 0);
		if(numTextureBanks >= MAX_TEXTURE_BANKS)
		{
			dprintf"ERROR:too many texture banks downloaded\n"));
			return;
		}
	}

	textureBanks[numTextureBanks].freePtr = textureBank;
	textureBanks[numTextureBanks].numTextures = *(int *)textureBank;
	textureBank += 4;
	textureBanks[numTextureBanks].data = textureBank;

	temp = textureBanks[numTextureBanks].data;
	for(y = 0; y < textureBanks[numTextureBanks].numTextures; y++)
	{
		tex = (TEXTURE *)temp;
		tex->data = (char *)((int)tex + sizeof(TEXTURE) );
		if(tex->palette)
			tex->palette = (short *)((int)tex->palette + (int)tex);
		temp += tex->size;

		// check if this is a procedural texture...

	}

	numTextureBanks++;
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void FindTexture(TEXTURE **texPtr, int texID, BOOL report)
{
	int x, y;
	TEXTURE *tex;
	char *temp;
			
	for(x = MAX_TEXTURE_BANKS - 1; x >= 0; x--)
//	for(x = 0; x < MAX_TEXTURE_BANKS; x++)
	{
		if(textureBanks[x].freePtr)
		{
			temp = textureBanks[x].data;
			for(y = 0; y < textureBanks[x].numTextures; y++)
			{
				tex = (TEXTURE *)temp;
				if(tex->texID == texID)
				{
					(*texPtr) = tex;
					
					return;
				}
				temp += tex->size;
			}
		}
	}
	*texPtr = NULL;
	if(report)
	{
		dprintf"Whoops, texture '%X' not found!\n",texID));
	}

//	(*texPtr) = &fadeTexture;
	return;
}


/*	--------------------------------------------------------------------------------
	Function 	: AnimateTexture
	Purpose 	: animates a texture for a drawlisted object
	Parameters 	: TEXTURE_ANIMATION *,Gfx *
	Returns 	: void
	Info 		:
*/
void AnimateTexture(TEXTURE_ANIMATION *anim,Gfx *dl)
{
	int lastFrame;
	float speed;

//	if(gamePaused)
//		return;

	lastFrame = anim->currentFrame;
	speed = 32 * gameSpeed;
	anim->counter -= speed;
	while(anim->counter <= 0)
	{
		if(anim->flags & TEX_ANIM_FLAGS_REVERSE)
		{
			anim->currentFrame--;
			if(anim->currentFrame >= anim->frameList->numFrames)
			{
				if(anim->flags & TEX_ANIM_FLAGS_FLIPFLOP)
				{
					anim->currentFrame = 1;
					anim->flags &= -1 - TEX_ANIM_FLAGS_REVERSE;
				}
				else
					anim->currentFrame = anim->frameList->numFrames-1;
			}
		}
		else
		{
			anim->currentFrame++;
			if(anim->currentFrame >= anim->frameList->numFrames)
			{
				if(anim->flags & TEX_ANIM_FLAGS_FLIPFLOP)
				{
					anim->currentFrame = anim->frameList->numFrames - 2;
					anim->flags |= TEX_ANIM_FLAGS_REVERSE;
				}
				else
		 			anim->currentFrame = 0;
			}
		}

		anim->counter += anim->delay;
	}
/*
	if(lastFrame != anim->currentFrame)
	{
		if(anim->frameList == &spriteFrameList[WATER_ANIM])
			ReplaceTextureInDrawList(dl,NULL,(u32)(*(anim->frameList->texture + anim->currentFrame))->data, YES);
		else
			ReplaceTextureInDrawList(dl,(u32)(*(anim->frameList->texture + lastFrame))->data,(u32)(*(anim->frameList->texture + anim->currentFrame))->data, NO);

	}
*/
}

