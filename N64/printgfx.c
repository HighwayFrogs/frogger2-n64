/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: printgfx.c
	Programmer	: Andy Eder
	Date		: 19/04/99 11:12:48

----------------------------------------------------------------------------------------------- */


#define F3DEX_GBI_2

#include <ultra64.h>
#include "incs.h"

//----- [ GLOBALS ] ----------------------------------------------------------------------------//

// STATIC VERTICES FOR VARIOUS SPECIAL EFFECTS
Vtx shadowVtx[4] =
{
	{ 1,0,1,0,		0,0,63,63,63,255 },
	{ 1,0,-1,0,		0,1024,63,63,63,255 },
	{ -1,0,-1,0,	1024,1024,63,63,63,255 },
	{ -1,0,1,0,		1024,0,63,63,63,255 }
};

Vtx rippleVtx[4] =
{
	{ 1,0,1,0,		0,0,255,255,255,255 },
	{ 1,0,-1,0,		0,1024,255,255,255,255 },
	{ -1,0,-1,0,	1024,1024,255,255,255,255 },
	{ -1,0,1,0,		1024,0,255,255,255,255 }
};

Vtx ringVtx[4] =
{
	{ 1,0,1,0,		0,0,255,255,255,255 },
	{ 1,0,-1,0,		0,1024,255,255,255,255 },
	{ -1,0,-1,0,	1024,1024,255,255,255,255 },
	{ -1,0,1,0,		1024,0,255,255,255,255 }
};


static Bitmap template_bm[NUM_template_BMS];
static Gfx template_dl[NUM_DL(NUM_template_BMS)];

Sprite template_sprite =
{
	0,0,						// xy position
	0,0,						// sprite size in xy (in texels)
	1.0,1.0,					// sprite scale in xy
	0,0,						// explosion spacing
	SP_TRANSPARENT,				// sprite attributes
	0x00,						// sprite depth in z
	255,255,255,255,			// sprite RGBA colour
	0,1,NULL,					// CLUT : start index, length, address
	0,1,						// sprite bitmap index: start index, step increment
	NUM_template_BMS,			// number of bitmaps
	NUM_DL(NUM_template_BMS),	// number of allocated dl locations
	128,208,					// sprite bm height: used height, physical height
	G_IM_FMT_RGBA,				// sprite image format
	G_IM_SIZ_16b,				// sprite bitmap texel size
	template_bm,				// ptr to bitmaps
	template_dl,				// dl memory
	NULL,						// HACK : dynamic_dl ptr
};



//----- [ RENDER STATES ] ----------------------------------------------------------------------//

Gfx rdpInitForBackdrops_dl[] = 
{
	gsDPSetCycleType(G_CYC_1CYCLE),    
	gsDPPipelineMode(G_PM_NPRIMITIVE), 
	gsDPSetTextureLOD(G_TL_TILE),
	gsDPSetTextureLUT(G_TT_RGBA16),
	gsDPSetTextureDetail(G_TD_CLAMP),
	gsDPSetTexturePersp(G_TP_NONE),
#ifdef NOFILTER
	gsDPSetTextureFilter(G_TF_POINT),
#else
	gsDPSetTextureFilter(G_TF_BILERP),
#endif
	gsDPSetTextureConvert(G_TC_FILT),
	gsDPSetCombineMode(G_CC_MODULATEPRIMRGBA, G_CC_MODULATEPRIMRGBA),
	gsDPSetCombineKey(G_CK_NONE),
	gsDPSetAlphaCompare(G_AC_NONE),
	gsDPSetRenderMode(G_RM_AA_XLU_SURF, G_RM_AA_XLU_SURF2),
	gsDPSetColorDither(G_CD_BAYER),
	gsDPPipeSync(),
	gsSPEndDisplayList(),
};

Gfx rspInitForBackdrops_dl[] = 
{
	gsSPClearGeometryMode(G_SHADE | G_SHADING_SMOOTH | G_CULL_BOTH |
			G_FOG | G_LIGHTING | G_TEXTURE_GEN |
			G_TEXTURE_GEN_LINEAR | G_LOD ),
	gsSPTexture(0, 0, 0, 0, G_OFF),
	gsSPSetGeometryMode(G_SHADE | G_SHADING_SMOOTH),
	gsSPEndDisplayList(),  
};

Gfx rdpInitForOverlays_dl[] = 
{
	gsDPSetTextureFilter(G_TF_BILERP),
	gsDPSetTexturePersp(G_TP_NONE),
	gsDPSetTextureLUT(G_TT_RGBA16),
//	gsDPSetRenderMode(G_RM_CLD_SURF,G_RM_CLD_SURF2),
	gsDPSetRenderMode(G_RM_XLU_SURF,G_RM_XLU_SURF2),
	gsDPSetCombineMode(G_CC_MODULATEPRIMRGBA,G_CC_MODULATEPRIMRGBA),
	gsSPEndDisplayList(),
};

Gfx rspInitForOverlays_dl[] = 
{
	gsSPClearGeometryMode(G_CULL_BOTH | G_LIGHTING),
    gsSPSetGeometryMode(G_SHADE | G_SHADING_SMOOTH),	
	gsSPTexture(0xffff,0xffff,0,G_TX_RENDERTILE,G_ON),
	gsSPEndDisplayList(),
};


Gfx rspInitForSprites_dl[] = 
{
//	gsSPClearGeometryMode(G_SHADE | G_SHADING_SMOOTH | G_CULL_BOTH |
//			G_FOG | G_LIGHTING | G_TEXTURE_GEN |
//			G_TEXTURE_GEN_LINEAR | G_LOD ),
//	gsSPTexture(0, 0, 0, 0, G_OFF),
//	gsSPSetGeometryMode(G_SHADE | G_SHADING_SMOOTH),
	gsSPEndDisplayList(),
};

Gfx rdpInitForSprites_dl[] =
{
	gsDPPipeSync(),
	gsDPSetCycleType(G_CYC_1CYCLE),
	gsDPPipelineMode(G_PM_NPRIMITIVE),
	gsDPSetTextureLOD(G_TL_TILE),
	gsDPSetTextureDetail(G_TD_CLAMP),
	gsDPSetTexturePersp(G_TP_NONE),
	gsDPSetTextureFilter(G_TF_BILERP),
	gsDPSetTextureConvert(G_TC_FILT),
	gsDPSetCombineKey(G_CK_NONE),
	gsDPSetAlphaCompare(G_AC_NONE),
	gsDPSetColorDither(G_CD_DISABLE),
	gsDPSetDepthSource(G_ZS_PRIM),
	gsSPEndDisplayList(),
};


// display list for intensity alpha and the croak - ANDYE
// assumes 32x32 texture will be loaded (8bpp grayscale) into TMEM
Gfx croakRing_dl[] =
{
	gsDPPipeSync(),
	gsSPClearGeometryMode(G_CULL_BOTH | G_FOG),
	gsSPSetGeometryMode(G_ZBUFFER | G_SHADE | G_SHADING_SMOOTH),
	gsSPSetGeometryMode(G_TEXTURE_GEN | G_TEXTURE_GEN_LINEAR),

	gsDPSetCombineMode(G_CC_MODULATERGBA,G_CC_MODULATERGBA),
//	gsDPSetRenderMode(G_RM_AA_ZB_XLU_SURF,G_RM_AA_ZB_XLU_SURF2),
	gsDPSetRenderMode(G_RM_ZB_XLU_SURF,G_RM_ZB_XLU_SURF2),
	gsDPSetCycleType(G_CYC_1CYCLE),
	gsSPTexture(0xffff,0xffff,0,G_TX_RENDERTILE,G_ON),
    gsDPPipelineMode(G_PM_NPRIMITIVE),
	gsDPSetColorDither(G_CD_DISABLE),
	gsDPSetTexturePersp(G_TP_PERSP),
	gsDPSetTextureLOD(G_TL_TILE),
	gsDPSetTextureFilter(G_TF_BILERP),
	gsDPSetTextureConvert(G_TC_FILT),
	gsDPSetTextureLUT(G_TT_NONE),
	gsSPEndDisplayList()
};

#define G_CC_MOTIONBLUR TEXEL0,ENVIRONMENT,PRIMITIVE,SHADE,  TEXEL0,0,SHADE,0

// For full screen blur fx and stuff
Gfx polyNoZ_dl[] =
{
	gsDPPipeSync(),
	gsSPClearGeometryMode( G_ZBUFFER | G_CULL_BOTH | G_FOG ),
	gsSPSetGeometryMode( G_SHADE | G_SHADING_SMOOTH ),
	
	gsDPSetCombineMode(G_CC_MOTIONBLUR,G_CC_MOTIONBLUR),

	gsDPSetRenderMode(G_RM_XLU_SURF,G_RM_XLU_SURF2),
	gsDPSetCycleType(G_CYC_1CYCLE),

	gsSPTexture(0xffff,0xffff,0,G_TX_RENDERTILE,G_ON),

    gsDPPipelineMode(G_PM_NPRIMITIVE),
	gsDPSetColorDither(G_CD_DISABLE),
	gsDPSetTexturePersp(G_TP_PERSP),
	gsDPSetTextureLOD(G_TL_TILE),
	gsDPSetTextureFilter(G_TF_POINT),
	gsDPSetTextureConvert(G_TC_FILT),
	gsDPSetTextureLUT(G_TT_NONE),
	gsSPEndDisplayList()
};

Gfx setrend_light_fog2[] =
{
	gsDPPipeSync(),
	gsDPSetCycleType(G_CYC_2CYCLE),
	gsDPPipeSync(),
	gsSPEndDisplayList()
};


Vtx *vPtr = NULL;

float rotMtx[4][4],scaleMtx[4][4],transMtx[4][4],tempMtx[4][4];



/*	--------------------------------------------------------------------------------
	Function		: 
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			:
*/
void PrintBackdrops()
{
	BACKDROP *backdrop;

	if(backdropList.numEntries == 0)
		return;

	gDPPipeSync(glistp++);
	gSPDisplayList(glistp++, rdpInitForBackdrops_dl);
	gSPObjRenderMode(glistp ++, G_OBJRM_XLU);
	for(backdrop = backdropList.head.next;backdrop != &backdropList.head;backdrop = backdrop->next)
	{
		if((backdrop->draw) && (backdrop->texture))
		{
			if(backdrop->flags & BACKDROP_FILTER)
			{
				gDPSetTextureFilter(glistp++,G_TF_BILERP);
			}
			else
			{
				gDPSetTextureFilter(glistp++,G_TF_POINT);
			}

			if((backdrop->a == 255) && (backdrop->texture->format != G_IM_FMT_IA))
			{
				if(backdrop->flags & BACKDROP_FILTER)
				{
					gSPObjRenderMode(glistp ++, G_OBJRM_BILERP);
				}
				else
				{
					gSPObjRenderMode(glistp ++, 0);
				}
				gDPSetRenderMode(glistp++, G_RM_SPRITE, G_RM_SPRITE2);
			}
			else
			{
				if(backdrop->flags & BACKDROP_FILTER)
				{
					gSPObjRenderMode(glistp ++, G_OBJRM_XLU | G_OBJRM_BILERP);
				}
				else
				{
					gSPObjRenderMode(glistp ++, G_OBJRM_XLU);
				}
				gDPSetRenderMode(glistp++, G_RM_XLU_SPRITE, G_RM_XLU_SPRITE2);
			}

			switch(backdrop->texture->pixSize)
			{
				case G_IM_SIZ_4b:
					gDPSetTextureLUT(glistp++,G_TT_RGBA16);
					gDPLoadTLUT_pal16(glistp++,0,backdrop->texture->palette);
					break;
				case G_IM_SIZ_8b:
					gDPSetTextureLUT(glistp++,G_TT_RGBA16);
					gDPLoadTLUT_pal256(glistp++,backdrop->texture->palette);
					break;
				case G_IM_SIZ_16b:
					gDPSetTextureLUT(glistp++,G_TT_NONE);
					break;
			}
			gDPSetPrimColor(glistp++,0,0,backdrop->r,backdrop->g,backdrop->b,backdrop->a);
			gSPBgRect1Cyc(glistp++,&backdrop->background);
		}
	}
	gDPPipeSync(glistp++);
}


/*	--------------------------------------------------------------------------------
	Function		: PrintTextAsOverlay
	Purpose			: prints text as an overlay
	Parameters		: none
	Returns			: void
	Info			:
*/
void PrintTextAsOverlay(TEXTOVERLAY *tOver)
{
	unsigned int pos = 0,length;
	unsigned int x,y,width;
	unsigned char letter,letterCount;
	static float wSeed = 0.0F;

	gSPDisplayList(glistp++,rdpInitForOverlays_dl);
	gSPDisplayList(glistp++,rspInitForOverlays_dl);
			
	x = tOver->xPos;
	y = tOver->yPos;

	if(tOver->centred)
	{
		// Centre text along screen x-axis
		length	= strlen(tOver->text);
		width	= length * (tOver->font->xSpacing[0]);
		x		= 160 - (width >> 1);

		if((length & 1) != 0)
			x -= 5;
	}

	gDPSetPrimColor(glistp++,0,0,tOver->r,tOver->g,tOver->b,tOver->a);

	letterCount = 0;
	while(tOver->text[pos] != '\0')
	{
		letter = tOver->text[pos];

		if(tOver->flags & TEXTOVERLAY_WAVECHARS)
		{
			letterCount++;
			y = tOver->yPos + sinf(wSeed + tOver->waveStart + (letterCount * (PI_OVER_4))) * tOver->waveAmplitude;
		}

		if(letter == 32)
		{
			x += tOver->font->xSpacing[0];
		}
		else
		{
			gDPLoadTLUT_pal16(glistp++,0,&tOver->font->data[tOver->font->offset[characterMap[letter]] + tOver->font->palOffset]);

			gDPLoadTextureBlock_4b(glistp++,&tOver->font->data[tOver->font->offset[characterMap[letter]]],G_IM_FMT_CI,
									tOver->font->width,tOver->font->height,
									0,G_TX_CLAMP|G_TX_NOMIRROR,G_TX_CLAMP|G_TX_NOMIRROR,0,0,G_TX_NOLOD,G_TX_NOLOD);

			gSPScisTextureRectangle(glistp++,x << 2,y << 2,(x + tOver->font->width) << 2,(y + tOver->font->height) << 2,
									G_TX_RENDERTILE,0,0,1<<10,1<<10);

			x += tOver->font->xSpacing[characterMap[letter]];
		}

		pos++;
	}

	wSeed += (0.05F * desiredFrameRate);
}


/*	--------------------------------------------------------------------------------
	Function		: PrintSpriteOverlays
	Purpose		.	: draws overlays - icons, etc....
	Parameters		: none
	Returns			: void
	Info			:
*/
void PrintSpriteOverlays()
{
	float tScaleX,tScaleY;
	SPRITEOVERLAY *cur,*next;
	TEXTURE *texture;

	if(spriteOverlayList.numEntries == 0)
		return;

	gSPDisplayList(glistp++,rdpInitForOverlays_dl);
	gSPDisplayList(glistp++,rspInitForOverlays_dl);

	for(cur = spriteOverlayList.head.next; cur != &spriteOverlayList.head; cur = next)
	{
		next = cur->next;

		// update the sprite animation if an animated sprite overlay
		if(cur->draw)
		{
			// check animation playback type - cycle,ping-pong, etc.
			if(cur->flags & ANIMATION_CYCLE)
			{
				// sprite is cycling through frames - check if cycling forwards or backwards
				if(cur->flags & ANIMATION_FORWARDS)
				{
					// cycling forwards
					cur->currFrame = (short)cur->animTime;
					cur->animTime += cur->animSpeed;
					if(cur->animTime > cur->numFrames)
					{
						cur->animTime = 0;
						cur->currFrame = 0;
					}
				}
				else if(cur->flags & ANIMATION_BACKWARDS)
				{
					// cycling backwards
					cur->currFrame = ROUND2SHORT(cur->animTime);
					cur->animTime -= cur->animSpeed;
					if(cur->animTime < 0)
					{
						cur->animTime = cur->numFrames-1;
						cur->currFrame = cur->numFrames-1;
					}
				}
			}
			else if(cur->flags & ANIMATION_PINGPONG)
			{
				// sprite ping-ponging through frames - check if currently forwards or backwards
				if(cur->flags & ANIMATION_FORWARDS)
				{
					// cycling forwards
					cur->currFrame = (short)cur->animTime;
					cur->animTime += cur->animSpeed;
					if(cur->animTime >= cur->numFrames)
					{
						cur->animTime = cur->numFrames-1;
						cur->currFrame = cur->numFrames-1;
						cur->flags &= ~ANIMATION_FORWARDS;
						cur->flags |= ANIMATION_BACKWARDS;
					}
				}
				else if(cur->flags & ANIMATION_BACKWARDS)
				{
					// cycling backwards
					cur->currFrame = ROUND2SHORT(cur->animTime);
					cur->animTime -= cur->animSpeed;
					if(cur->animTime <= 0)
					{
						cur->animTime = 0;
						cur->currFrame = 0;
						cur->flags &= ~ANIMATION_BACKWARDS;
						cur->flags |= ANIMATION_FORWARDS;
					}
				}
			}
			else if(cur->flags & ANIMATION_RANDOMFRAMES)
			{
				// choose a random frame and display it
				cur->currFrame = Random(cur->numFrames);
			}

			texture = cur->frames[cur->currFrame];

			// perform scaling
			tScaleX = ((float)cur->width  / (float)texture->sx) * 1024.0;
			tScaleY = ((float)cur->height / (float)texture->sy) * 1024.0;

			tScaleX = (1024.0 * 1024.0) / tScaleX;
			tScaleY = (1024.0 * 1024.0) / tScaleY;

			gDPSetPrimColor(glistp++,0,0,cur->r,cur->g,cur->b,cur->a);
/*
			gDPLoadTLUT_pal256(glistp++,texture->palette);
			gDPLoadTextureBlock(glistp++,texture->data,G_IM_FMT_RGBA,G_IM_SIZ_8b,
								  texture->sx,texture->sy,0,
								  G_TX_CLAMP|G_TX_NOMIRROR,
								  G_TX_CLAMP|G_TX_NOMIRROR,
								  0,0,
								  G_TX_NOLOD,G_TX_NOLOD);
*/
			LoadTexture(texture);

			gSPScisTextureRectangle(glistp++,
								(unsigned long)cur->xPos << 2,
								(unsigned long)cur->yPos << 2,
								(unsigned long)(cur->xPos + cur->width) << 2,
								(unsigned long)(cur->yPos + cur->height) << 2,
								G_TX_RENDERTILE,
								0,0,
								(short)tScaleX,
								(short)tScaleY);
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function		: DrawSpecialFX
	Purpose			: updates and draws the various special FX
	Parameters		: 
	Returns			: void
	Info			: 
*/
void DrawSpecialFX()
{
	if(	(gameState.mode == INGAME_MODE) || (gameState.mode == PAUSE_MODE) ||
		(gameState.mode == CAMEO_MODE))
	{
		ProcessShadows();
		
		if(specFXList.numEntries)
		{
			SPECFX *fx;
			for(fx=specFXList.head.next; fx!=&specFXList.head; fx=fx->next)
			{
				if(fx->Draw)
				{
					// perform RSP / RDP processing here to minimise texture loading, etc.

					// set primitive colour
					gDPSetPrimColor(glistp++,0,0,fx->r,fx->g,fx->b,fx->a);

					// load texture - perhaps move this elsewhere to minimise texture loads ??? ANDYE
					gDPSetTextureLUT(glistp++,G_TT_NONE);
					gDPLoadTextureBlock(glistp++,fx->tex->data,G_IM_FMT_IA,G_IM_SIZ_16b,fx->tex->sx,fx->tex->sy,0,
										G_TX_CLAMP,G_TX_CLAMP,fx->tex->TCScaleX,fx->tex->TCScaleY,G_TX_NOLOD,G_TX_NOLOD);
					fx->Draw(fx);
				}
			}
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function		: DrawFXRipple
	Purpose			: draws the ripple based FX
	Parameters		: SPECFX *
	Returns			: void
	Info			: 
*/
void DrawFXRipple(SPECFX *ripple)
{
	QUATERNION q3,q2,q1,cross;
	float t;

	if(ripple->deadCount)
		return;

	// create the translation matrix
	guTranslateF(transMtx,ripple->origin.v[X],ripple->origin.v[Y],ripple->origin.v[Z]);

	// create the scaling matrix
	guScaleF(scaleMtx,ripple->scale.v[X],1,ripple->scale.v[Z]);

	// create the rotation matrix
	CrossProduct((VECTOR *)&q1,&ripple->normal,&upVec);
	MakeUnit((VECTOR *)&q1);
	t = DotProduct(&ripple->normal,&upVec);
	q1.w = -acos(t);
	GetQuaternionFromRotation(&q2,&q1);

	// determine ripple type
	if(ripple->type == FXTYPE_GARIBCOLLECT)
	{
		// rotate around axis
		SetVector((VECTOR *)&q1,&ripple->normal);
		q1.w = ripple->angle;
		GetQuaternionFromRotation(&q3,&q1);
		QuaternionMultiply(&q1,&q2,&q3);
	}
	else
	{
		SetQuaternion(&q1,&q2);
	}

	QuaternionToMatrix(&q1,(MATRIX *)rotMtx);

	// combine matrices into a single transformation matrix
	guMtxCatF(rotMtx,transMtx,tempMtx);
	guMtxCatF(scaleMtx,tempMtx,tempMtx);
	guMtxF2L(tempMtx,&dynamicp->modeling4[objectMatrix]);

	// push onto matrix stack
	gSPMatrix(glistp++,OS_K0_TO_PHYSICAL(&(dynamicp->modeling4[objectMatrix++])),
				G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);

	// load vertices into vertex cache
	gSPVertex(glistp++,rippleVtx,4,0);
	gSP2Triangles(glistp++,0,1,2,0,0,2,3,0);

	// pop modelview matrix stack
	gSPPopMatrix(glistp++,G_MTX_MODELVIEW);
}


/*	--------------------------------------------------------------------------------
	Function		: DrawFXRing
	Purpose			: draws the ring based FX
	Parameters		: SPECFX *
	Returns			: void
	Info			: 
*/
void DrawFXRing(SPECFX *ring)
{
	QUATERNION q3,q2,q1,cross;
	float t;
	TEXTURE *tx1 = NULL;

	if(ring->deadCount)
		return;

	// create the translation matrix
	guTranslateF(transMtx,ring->origin.v[X],ring->origin.v[Y],ring->origin.v[Z]);

	// create the scaling matrix
	guScaleF(scaleMtx,ring->scale.v[X],1,ring->scale.v[Z]);

	// create the rotation matrix
	SetVector((VECTOR *)&q1,&ring->normal);
	q1.w = ring->angle;
	GetQuaternionFromRotation(&q2,&q1);

	CrossProduct((VECTOR *)&cross,(VECTOR *)&q1,&upVec);
	MakeUnit((VECTOR *)&cross);
	t = DotProduct( (VECTOR *)&q1,&upVec);
	cross.w = -acos(t);
	GetQuaternionFromRotation(&q3,&cross);

	QuaternionMultiply(&q1,&q2,&q3);
	QuaternionToMatrix(&q1,(MATRIX *)rotMtx);

	// combine matrices into a single transformation matrix
	guMtxCatF(rotMtx,transMtx,tempMtx);
	guMtxCatF(scaleMtx,tempMtx,tempMtx);
	guMtxF2L(tempMtx,&dynamicp->modeling4[objectMatrix]);

	// push onto matrix stack
	gSPMatrix(glistp++,OS_K0_TO_PHYSICAL(&(dynamicp->modeling4[objectMatrix++])),
				G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);

	// load vertices into vertex cache
	gSPVertex(glistp++,ringVtx,4,0);
	gSP2Triangles(glistp++,0,1,2,0,0,2,3,0);

	// pop modelview matrix stack
	gSPPopMatrix(glistp++,G_MTX_MODELVIEW);
}


/*	--------------------------------------------------------------------------------
	Function		: DrawFXTrail
	Purpose			: draws the trail based FX
	Parameters		: SPECFX *
	Returns			: void
	Info			: 
*/
void DrawFXTrail(SPECFX *trail)
{
}


/*	--------------------------------------------------------------------------------
	Function		: DrawFXLightning
	Purpose			: draws the lightning based FX
	Parameters		: SPECFX *
	Returns			: void
	Info			: 
*/
void DrawFXLightning(SPECFX *fx)
{
}


/*	--------------------------------------------------------------------------------
	Function		: ProcessShadows
	Purpose			: processes the shadows
	Parameters		: 
	Returns			: void
	Info			: 
*/
void ProcessShadows()
{
	TEXTURE *theTexture = NULL;
	VECTOR vec;
	ENEMY *nme;
	PLATFORM *plat;
//	GARIB *garib;
	int i;
	float height;

	gDPSetRenderMode(glistp++,G_RM_ZB_CLD_SURF,G_RM_ZB_CLD_SURF2);
	gDPSetCombineMode(glistp++,G_CC_MODULATEPRIMRGBA,G_CC_MODULATEPRIMRGBA);

	FindTexture(&theTexture,UpdateCRC("ai_circle.bmp"),YES);
	gDPSetTextureLUT(glistp++,G_TT_NONE);
	gDPLoadTextureBlock(glistp++,theTexture->data,G_IM_FMT_IA,G_IM_SIZ_16b,theTexture->sx,theTexture->sy,0,
						G_TX_CLAMP,G_TX_CLAMP,theTexture->TCScaleX,theTexture->TCScaleY,G_TX_NOLOD,G_TX_NOLOD);	

	// process frog shadows
	for(i=0; i<NUM_FROGS; i++ )
	{
		if(frog[i]->actor->shadow)
		{
			SubVector(&vec,&frog[i]->actor->pos,&currTile[i]->centre);
			height = DotProduct(&vec,&currTile[i]->normal);
			DrawShadow(&frog[i]->actor->pos,&currTile[i]->normal,frog[i]->actor->shadow->radius,-height + 1,frog[i]->actor->shadow->alpha,frog[i]->actor->shadow->vert);
		}
	}

	//------------------------------------------------------------------------------------------------

	// process enemy shadows
	for(nme = enemyList.head.next; nme != &enemyList.head; nme = nme->next)
	{
		if(!nme->active || !nme->nmeActor)
			continue;

		if(nme->nmeActor->actor->shadow && nme->inTile && nme->nmeActor->distanceFromFrog < ACTOR_DRAWDISTANCEINNER)
			DrawShadow(&nme->nmeActor->actor->pos,&nme->inTile->normal,nme->nmeActor->actor->shadow->radius, -nme->path->nodes[nme->path->fromNode].offset+1,nme->nmeActor->actor->shadow->alpha,nme->nmeActor->actor->shadow->vert);
	}

	// process platform shadows
	for(plat = platformList.head.next; plat != &platformList.head; plat = plat->next)
	{
		if(!plat->active || !plat->pltActor)
			continue;

		if(plat->pltActor->actor->shadow && plat->inTile && plat->pltActor->distanceFromFrog < ACTOR_DRAWDISTANCEINNER)
			DrawShadow(&plat->pltActor->actor->pos,&plat->inTile[0]->normal,plat->pltActor->actor->shadow->radius, -plat->path->nodes[plat->path->fromNode].offset+1,plat->pltActor->actor->shadow->alpha,plat->pltActor->actor->shadow->vert);
	}

	// process garib shadows
/*	for(garib = garibCollectableList.head.next; garib != &garibCollectableList.head; garib = garib->next)
	{
		if(garib->distanceFromFrog < SPRITE_DRAWDISTANCE)
		{
			vec.v[X] = garib->sprite.pos.v[X];
			vec.v[Y] = garib->sprite.pos.v[Y] + garib->sprite.offsetY;
			vec.v[Z] = garib->sprite.pos.v[Z];
			DrawShadow( &vec, &garib->gameTile->normal, garib->shadow.radius, 0, garib->shadow.alpha, tex );
		}
	}*/
}


/*	--------------------------------------------------------------------------------
	Function		: DrawShadow
	Purpose			: draws a shadow
	Parameters		: 
	Returns			: void
	Info			: 
*/
void DrawShadow(VECTOR *pos,VECTOR *normal,float size,float offset,short alpha,Vtx *sVtx)
{
	QUATERNION q,cross;
	float t;
	
	// set y-offset for vertices, and the alpha
	sVtx[0].v.ob[Y] = offset;	sVtx[0].v.cn[A] = alpha;
	sVtx[1].v.ob[Y] = offset;	sVtx[1].v.cn[A] = alpha;
	sVtx[2].v.ob[Y] = offset;	sVtx[2].v.cn[A] = alpha;
	sVtx[3].v.ob[Y] = offset;	sVtx[3].v.cn[A] = alpha;

	// create the translation matrix
	guTranslateF(transMtx,pos->v[X],pos->v[Y],pos->v[Z]);

	// create the scaling matrix
	guScaleF(scaleMtx,size,1,size);

	// create the rotation matrix
	CrossProduct((VECTOR *)&cross,normal,&upVec);
	MakeUnit((VECTOR *)&cross);
	t = DotProduct(normal,&upVec);
	
	if(cross.x >= 0)
		cross.w = acos(t);
	else
		cross.w = -acos(t);

	GetQuaternionFromRotation(&q,&cross);
	QuaternionToMatrix(&q,(MATRIX *)rotMtx);

	// combine matrices into a single transformation matrix
	guMtxCatF(rotMtx,transMtx,tempMtx);
	guMtxCatF(scaleMtx,tempMtx,tempMtx);
	guMtxF2L(tempMtx,&dynamicp->modeling4[objectMatrix]);

	// push onto matrix stack
	gSPMatrix(glistp++,OS_K0_TO_PHYSICAL(&(dynamicp->modeling4[objectMatrix++])),
				G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);

	// set primitive colour
	gDPSetPrimColor(glistp++,0,0,0,0,0,alpha);

	// load vertices into vertex cache
	gSPVertex(glistp++,sVtx,4,0);
	gSP2Triangles(glistp++,0,1,2,0,0,2,3,0);

	// pop modelview matrix stack
	gSPPopMatrix(glistp++,G_MTX_MODELVIEW);
}


/*	--------------------------------------------------------------------------------
	Function		: PrintSpritesOpaque
	Purpose			: prints opaque sprites
	Parameters		: 
	Returns			: SPRITE *
	Info			: 
*/
float printSpritesProj[4][4][4];
float fm;
SPRITE *PrintSpritesOpaque()
{
	SPRITE *cur,*next;
	Mtx temp;

	spriteList.lastTexture = NULL;

	gSPDisplayList(glistp++,rspInitForSprites_dl);
	gSPDisplayList(glistp++,rdpInitForSprites_dl);
	gDPSetScissor(glistp++,G_SC_NON_INTERLACE,0,0,SCREEN_WD,SCREEN_HT);

	if(fog.mode)
	{
		fm = 128000/(fog.max - fog.min);	
		gSPDisplayList(glistp++,setrend_light_fog2); 
	}

	guMtxCatL(&(dynamicp->viewing[0]),&(dynamicp->projection[0]),&temp);
	guMtxL2F(printSpritesProj[0],&temp);

	spriteList.xluMode = NO;

	if(fog.mode)
	{
		gDPSetRenderMode(glistp++,G_RM_FOG_PRIM_A,G_RM_AA_ZB_TEX_EDGE2);
		gDPSetCombineMode(glistp++,G_CC_MODULATEPRIMRGBA,G_CC_PASS2);
	}
	else
	{
		gDPSetRenderMode(glistp++,G_RM_AA_ZB_TEX_EDGE,G_RM_AA_ZB_TEX_EDGE2);
		gDPSetCombineMode(glistp++,G_CC_MODULATEPRIMRGBA,G_CC_MODULATEPRIMRGBA);
	}

	if(!pauseMode)
	{
		for(cur = spriteList.head.next; (cur != &spriteList.head) && ((cur->flags & SPRITE_TRANSLUCENT) == 0); cur = next)
		{
			next = cur->next;

			PrintSprite(cur);
		}
	}

	return cur;
}

/*	--------------------------------------------------------------------------------
	Function		: PrintSpritesTranslucent
	Purpose			: prints translucent sprites
	Parameters		: SPRITE *
	Returns			: void
	Info			: 
*/
void PrintSpritesTranslucent(SPRITE *sprite)
{
	Mtx temp;

	spriteList.lastTexture = NULL;

	gSPDisplayList(glistp++,rspInitForSprites_dl);
	gSPDisplayList(glistp++,rdpInitForSprites_dl);
	gDPSetScissor(glistp++,G_SC_NON_INTERLACE,0,0,SCREEN_WD,SCREEN_HT);

	if(fog.mode)
	{
		fm = 128000/(fog.max - fog.min);	
		gSPDisplayList(glistp++,setrend_light_fog2); 
	}

	guMtxCatL(&(dynamicp->viewing[0]),&(dynamicp->projection[0]),&temp);
	guMtxL2F(printSpritesProj[0],&temp);

	if(fog.mode)
	{
		gDPSetRenderMode(glistp++,G_RM_FOG_PRIM_A,G_RM_AA_ZB_TEX_EDGE2);
		gDPSetCombineMode(glistp++,G_CC_MODULATEPRIMRGBA,G_CC_PASS2);
	}
	else
	{
		gDPSetRenderMode(glistp++,G_RM_AA_ZB_TEX_EDGE,G_RM_AA_ZB_TEX_EDGE2);
		gDPSetCombineMode(glistp++,G_CC_MODULATEPRIMRGBA,G_CC_MODULATEPRIMRGBA);
	}

	if(!pauseMode)
	{
		// print sprite listed sprites
		for(; sprite != &spriteList.head; sprite = sprite->next)
		{
			PrintSprite(sprite);
		}
	}

	// IMPORTANT - MUST SET THESE STATES
	gDPSetDepthSource(glistp++,G_ZS_PIXEL);
	gDPSetTexturePersp(glistp++,G_TP_PERSP);
}

/*	--------------------------------------------------------------------------------
	Function		: TileRectangle
	Purpose			: displays a tile rectangle
	Parameters		: Gfx**,SPRITE *,f32,f32,int,int,int
	Returns			: void
	Info			: 
*/
void TileRectangle(Gfx **glistp,SPRITE *sprite,f32 x0,f32 y0,int z,int scaleX,int scaleY)
{
	Gfx *gfx = *glistp;
	int incx,incy;
	float tempx,tempy;
	int s,t;
	int sval,tval;

	gDPSetPrimDepth(gfx++,z,0);

	if(sprite->flags & SPRITE_FLIPX)
		incx = -scaleX;
	else
		incx = scaleX;

	if(sprite->flags & SPRITE_FLIPY)
		incy = -scaleY;
	else
		incy = scaleY;

	t = (scaleX < 512 ? -16 : -16 - (((int)y0) & 3) * 8);
	s = -16;
	
	if(sprite->texture != spriteList.lastTexture)
	{
		switch(sprite->texture->pixSize)
		{
			case G_IM_SIZ_4b:

				gDPSetTextureLUT(gfx++,G_TT_RGBA16);
				gDPLoadTLUT_pal16(gfx++,0,sprite->texture->palette);
				gDPLoadTextureTile_4b(gfx++,(void *)sprite->texture->data,sprite->texture->format,
					sprite->texture->sx,sprite->texture->sy,
					0,0,sprite->texture->sx - 1,sprite->texture->sy - 1,0,
					G_TX_NOMIRROR | G_TX_CLAMP,G_TX_NOMIRROR | G_TX_CLAMP,
					0,0,G_TX_NOLOD,G_TX_NOLOD);
				break;

			case G_IM_SIZ_8b:
				gDPSetTextureLUT(gfx++,G_TT_RGBA16);
				gDPLoadTLUT_pal256(gfx++,sprite->texture->palette);
				gDPLoadTextureTile(gfx++,(void *)sprite->texture->data,sprite->texture->format,
					G_IM_SIZ_8b,sprite->texture->sx,sprite->texture->sy,
					0,0,sprite->texture->sx - 1,sprite->texture->sy - 1,0,
					G_TX_NOMIRROR | G_TX_CLAMP,G_TX_NOMIRROR | G_TX_CLAMP,
					0,0,G_TX_NOLOD,G_TX_NOLOD);
				break;

			case G_IM_SIZ_16b:
				gDPSetTextureLUT(gfx++,G_TT_NONE);
				gDPLoadTextureTile(gfx++,(void *)sprite->texture->data,sprite->texture->format,
					G_IM_SIZ_16b,sprite->texture->sx,sprite->texture->sy,
					0,0,sprite->texture->sx - 1,sprite->texture->sy - 1,0,
					G_TX_NOMIRROR | G_TX_CLAMP,G_TX_NOMIRROR | G_TX_CLAMP,
					0,0,G_TX_NOLOD,G_TX_NOLOD);
				break;

			case G_IM_SIZ_32b:
				gDPLoadTextureTile(gfx++,(void *)sprite->texture->data,sprite->texture->format,
					G_IM_SIZ_32b,sprite->texture->sx,sprite->texture->sy,
					0,0,sprite->texture->sx - 1,sprite->texture->sy - 1,0,
					G_TX_NOMIRROR | G_TX_CLAMP,G_TX_NOMIRROR | G_TX_CLAMP,
					0,0,G_TX_NOLOD,G_TX_NOLOD);
				break;
		}
	}

	tempx = sprite->texture->sx;
	tempx *= (1 << 12);
	tempx /= (float)scaleX;
	tempx = x0 + tempx;

	tempy = sprite->texture->sy;
	tempy *= (1 << 12);
	tempy /= (float)scaleY;
	tempy = y0 + tempy;

	if(sprite->flags & SPRITE_FLIPX)
		sval = ((sprite->texture->sx) << 5) - s - 32;
	else
		sval = s;

	if(sprite->flags & SPRITE_FLIPY)
		tval = ((sprite->texture->sy) << 5) - t - 32;
	else
		tval = t;

	gSPScisTextureRectangle(gfx++,x0,y0,(int)tempx,(int)tempy,G_TX_RENDERTILE,sval,tval,incx,incy);
	*glistp = gfx;
}

/*	--------------------------------------------------------------------------------
	Function		: PrintSprite
	Purpose			: prints a sprite
	Parameters		: SPRITE *
	Returns			: void
	Info			: 
*/
void PrintSprite(SPRITE *sprite)
{
	float x,y,z,w,fogVal;
	float dist;
	int scaleX,scaleY;
	int sprScaleX,sprScaleY;

	if((!sprite->texture) || (sprite->scaleX == 0) || (sprite->scaleY == 0))
		return;

	dist = DistanceBetweenPointsSquared(&actualCamSource[draw_buffer],&sprite->pos);
	if((dist > farPlaneDist * farPlaneDist) || (dist < nearPlaneDist * nearPlaneDist))
		return;

	w = printSpritesProj[screenNum][0][3] * sprite->pos.v[X] + 
		printSpritesProj[screenNum][1][3] * sprite->pos.v[Y] +
		printSpritesProj[screenNum][2][3] * sprite->pos.v[Z] +
		printSpritesProj[screenNum][3][3];

	if(w <= 0)
		return;

	z = printSpritesProj[screenNum][0][2] * sprite->pos.v[X] + 
		printSpritesProj[screenNum][1][2] * sprite->pos.v[Y] +
		printSpritesProj[screenNum][2][2] * sprite->pos.v[Z] +
		printSpritesProj[screenNum][3][2];

	if(z < 0)
		return;

	x = printSpritesProj[screenNum][0][0] * sprite->pos.v[X] +
		printSpritesProj[screenNum][1][0] * sprite->pos.v[Y] +
		printSpritesProj[screenNum][2][0] * sprite->pos.v[Z] +
		printSpritesProj[screenNum][3][0];

	y = printSpritesProj[screenNum][0][1] * sprite->pos.v[X] +
		printSpritesProj[screenNum][1][1] * sprite->pos.v[Y] +
		printSpritesProj[screenNum][2][1] * sprite->pos.v[Z] +
		printSpritesProj[screenNum][3][1];

	dist = sqrtf(dist);

	x = ((x * dynamicp->vp[screenNum].vp.vscale[0] / w) + dynamicp->vp[screenNum].vp.vtrans[0]);
	y = ((-y * dynamicp->vp[screenNum].vp.vscale[1] / w) + dynamicp->vp[screenNum].vp.vtrans[1]);
	z = 32 * ((z * dynamicp->vp[screenNum].vp.vscale[2] / w) + dynamicp->vp[screenNum].vp.vtrans[2]);

	sprScaleX = 33 * sprite->scaleX * ((float)dynamicp->vp[screenNum].vp.vscale[0] / (float)(SCREEN_WD * 2));
	x = (f32)x + (f32)(sprite->offsetX * sprScaleX << 4) / (dist * yFOV);
	if((x > SCREEN_WD * 4) || (x + (sprite->texture->sx * sprScaleX << 4) / (dist * yFOV) <= 0))
		return;

	sprScaleY = 33 * sprite->scaleY * ((float)dynamicp->vp[screenNum].vp.vscale[1] / (float)(SCREEN_HT * 2));
	y = (f32)y + (f32)(sprite->offsetY * sprScaleY << 4) / (dist * yFOV);
	if((y > SCREEN_HT * 4) || (y + (sprite->texture->sy * sprScaleY << 4) / (dist * yFOV) <= 0))
		return;

	gDPSetPrimColor(glistp++,0,0,sprite->r,sprite->g,sprite->b,sprite->a);

	if(sprite->flags & SPRITE_FLAGS_PIXEL_OUT)
	{
		gDPSetAlphaCompare(glistp++,G_AC_DITHER);
	}
	else
	{
		gDPSetAlphaCompare(glistp++,G_AC_NONE);
	}
		
	if(fog.mode)
	{
		fogVal = MAX(0,((z-16360)/16360)*fm + 255 - fm);
		if(fogVal >= 255)
			return;
		gDPSetFogColor(glistp++,fog.r,fog.g,fog.b,(unsigned int)fogVal);
		if(sprite->flags & SPRITE_TRANSLUCENT)
		{
			gDPSetRenderMode(glistp++,G_RM_FOG_PRIM_A,G_RM_ZB_CLD_SURF2);
		}
		else
		{
			gDPSetRenderMode(glistp++,G_RM_FOG_PRIM_A,G_RM_AA_ZB_TEX_EDGE2);
		}
		gDPSetCombineMode(glistp++,G_CC_MODULATEPRIMRGBA, G_CC_PASS2);
		gDPSetCycleType(glistp++,G_CYC_2CYCLE);
	}
	else
	{
		if(sprite->flags & SPRITE_FLAGS_COLOUR_BLEND)
		{
			if(sprite->flags & SPRITE_TRANSLUCENT)
			{
				gDPSetRenderMode(glistp++,G_RM_PASS,G_RM_ZB_CLD_SURF2);
			}
			else
			{
				gDPSetRenderMode(glistp++,G_RM_PASS,G_RM_AA_ZB_TEX_EDGE2);
			}
			gDPSetCombineMode(glistp++,G_CC_COLOURBLEND_SPRITE,G_CC_COLOURBLEND_SPRITE2);
			gDPSetEnvColor(glistp++,sprite->red2,sprite->green2,sprite->blue2,sprite->alpha2);
			gDPSetCycleType(glistp++,G_CYC_2CYCLE);
		}
		else if(sprite->flags & SPRITE_FLAGS_COLOUR_BLEND_AFTERLIGHT)
		{
			if(sprite->flags & SPRITE_TRANSLUCENT)
			{
				gDPSetRenderMode(glistp++,G_RM_PASS,G_RM_ZB_CLD_SURF2);
			}
			else
			{
				gDPSetRenderMode(glistp++,G_RM_PASS,G_RM_AA_ZB_TEX_EDGE2);
			}
			gDPSetCombineMode(glistp++,G_CC_COLOURBLEND_SPRITE_AFTERLIGHT,G_CC_COLOURBLEND_SPRITE_AFTERLIGHT2);
			gDPSetEnvColor(glistp++,sprite->red2,sprite->green2,sprite->blue2,sprite->alpha2);
			gDPSetCycleType(glistp++,G_CYC_2CYCLE);
		}
		else
		{
			if(sprite->flags & SPRITE_TRANSLUCENT)
			{
				if(sprite->flags & SPRITE_FLAGS_NOZB)
				{
					gDPSetRenderMode(glistp++,G_RM_CLD_SURF,G_RM_CLD_SURF2);
				}
				else
				{
					gDPSetRenderMode(glistp++,G_RM_ZB_CLD_SURF,G_RM_ZB_CLD_SURF2);
				}
			}
			else
			{
				if(sprite->flags & SPRITE_FLAGS_NOZB)
				{
					gDPSetRenderMode(glistp++,G_RM_AA_TEX_EDGE,G_RM_AA_TEX_EDGE2);
				}
				else
				{
					gDPSetRenderMode(glistp++,G_RM_AA_ZB_TEX_EDGE,G_RM_AA_ZB_TEX_EDGE2);
				}
			}
			gDPSetCombineMode(glistp++,G_CC_MODULATEPRIMRGBA,G_CC_MODULATEPRIMRGBA);
			gDPSetCycleType(glistp++,G_CYC_1CYCLE);
		}
	}

/* NOT IN 
	gDPSetAlphaCompare(glistp++,G_AC_NONE);

	if(sprite->flags & SPRITE_TRANSLUCENT)
	{
		gDPSetRenderMode(glistp++,G_RM_ZB_CLD_SURF,G_RM_ZB_CLD_SURF2);
	}
	else
	{
		gDPSetRenderMode(glistp++,G_RM_AA_ZB_TEX_EDGE,G_RM_AA_ZB_TEX_EDGE2);
	}

	gDPSetCombineMode(glistp++,G_CC_MODULATEPRIMRGBA,G_CC_MODULATEPRIMRGBA);
	gDPSetCycleType(glistp++,G_CYC_1CYCLE);
*/

	scaleY = (dist * yFOV);
	scaleX = (scaleY << 8) / sprScaleX;
	scaleY = (scaleY << 8) / sprScaleY;

	TileRectangle(&glistp,sprite,x,y,z,scaleX,scaleY);
	gDPPipeSync(glistp++);

	spriteList.lastTexture = sprite->texture;
}


/*	--------------------------------------------------------------------------------
	Function		: 
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/
void SetRendermodeForEnviroment(void)
{
	gSPClearGeometryMode(glistp++,G_TEXTURE_GEN);
}


/*	--------------------------------------------------------------------------------
	Function		: IsPointVisible
	Purpose			: determine if a point in 3D space is visible in 2D screen space
	Parameters		: VECTOR *
	Returns			: char
	Info			: 
*/
char IsPointVisible(VECTOR *p)
{
	VECTOR m;
	XfmPoint (&m,p);
	if (m.v[3]==0)
		return 0;
	if (m.v[0]<0) 
		return 0;
	if (m.v[0]>640) 
		return 0;
	if (m.v[1]<0) 
		return 0;
	if (m.v[1]>480)
		return 0;
	return 1;
}


/*	--------------------------------------------------------------------------------
	Function		: ScreenShot
	Purpose			: performs a screen grab
	Parameters		: 
	Returns			: void
	Info			: 
*/
void ScreenShot()
{
	static int picnum = 0;
	char	filename[16];
	int	file;
	int x, y;
	u16 *screen = cfb_16_a;
	u16 pixel;
	u8	col;
	u8	line[SCREEN_WD * 4];
	int	linePos;
	char header[] =	   {0x42,0x4D,0x36,0x84,0x03,0x00,0x00,0x00,0x00,0x00,0x36,0x00,0x00,0x00,0x28,0x00,
						0x00,0x00,0x40,0x01,0x00,0x00,0xF0,0x00,0x00,0x00,0x01,0x00,0x18,0x00,0x00,0x00,
						0x00,0x00,0x00,0x84,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
						0x00,0x00,0x00,0x00,0x00,0x00};


	StopDrawing("scchot");
	dontClearScreen = TRUE;
//help	disableGraphics = TRUE;

	dprintf"==================\n"));
	dprintf"Taking screen shot\n"));


	sprintf(filename, "c:\\n64%04d.bmp", picnum++);
	file = PCcreat(filename, 0);
	if(file == -1)
	{
		StartDrawing("scsho");
//help		disableGraphics = FALSE;
		dprintf"FILEERROR:could not open file:\n"));
		return;
	}

	PCwrite(file, header, sizeof(header));	

	y = SCREEN_HT-1;
	while (y >= 0)
	{
		linePos = 0;
		for(x = 0; x < SCREEN_WD; x++)
		{
			pixel = screen[x + SCREEN_WD * y];

			col = ((pixel>>1)<<3)&0xFF;
			line[linePos++] = col;
			col = ((pixel>>6)<<3)&0xFF;
			line[linePos++] = col;
			col = ((pixel>>11)<<3)&0xFF;
			line[linePos++] = col;


		}
		y--;

			PCwrite(file, line, linePos);	
		
	}


//	PCwrite(file, cfb_ptrs[1 - draw_buffer], SCREEN_WD*SCREEN_HT * 2);	
	PCclose(file);
	dontClearScreen = FALSE;
	StartDrawing("bum");
//help	disableGraphics = FALSE;

}



/*	--------------------------------------------------------------------------------
	Function		: N64SurfaceBlit
	Purpose			: performs N64 surface blit for procedural texturing
	Parameters		: 
	Returns			: 
	Info			: 
*/
void N64SurfaceBlit(unsigned char *to,unsigned char *buf,unsigned short *pal)
{
	long i = 928;

	while(i--)
	{
		((unsigned short *)to)[i] = (unsigned short)pal[(unsigned char)buf[i]];
	}
}

