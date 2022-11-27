/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: actor.c
	Programmer	: Matthew Cloy
	Date		: 11/11/98


----------------------------------------------------------------------------------------------- */


#define F3DEX_GBI_2

#include <ultra64.h>

#ifndef PC_VERSION
#include <PR/ramrom.h>					// Needed for argument passing into the app
#include <PR/gbi.h>
#include <PR/os.h>
#endif

#include <assert.h>

#include "incs.h"

#ifdef PC_VERSION
#include "mdxDDraw.h"
#include "mdxD3D.h"
#include "mavis.h"
#endif

#define MAX_UNIQUE_ACTORS	500
void XformActor(ACTOR *ptr);

unsigned long ACTOR_DRAWDISTANCEINNER = 300000;
unsigned long ACTOR_DRAWDISTANCEOUTER = 600000;

float bFOV = 450.0;
float texSlideSpeed = 40;
extern float fStart,fEnd, FOV;
extern long noClipping;

#define WATER_XLU 70
long waterObject = 0;
long modgyObject = 0;
long sludgeObject = 0;
long leafObject = 0;
int objectMatrix = 0;
long showActorNames = 0;
ACTOR2 *actList = NULL;				// entire actor list
ACTOR2 *backGnd = NULL;
ACTOR2 *globalLevelActor = NULL;	// ptr to actor representing level

//used to keep a count of how many of each enemy are present at the same time
//char uniqueEnemyCount[20];

int uniqueActorCRC[MAX_UNIQUE_ACTORS];
int numUniqueActors = 0;
extern ACTOR2 *hat[MAX_FROGS];
float ACTOR_DRAWFADERANGE;

/* --------------------------------------------------------------------------------	
	Programmer	: Matthew Cloy
	Function    : 
	Purpose		:
	Parameters	: 
	Returns		: 
*/

void SlideObjectTextures(OBJECT *obj)
{
	int i;
	
	// For all the faces.....
	for (i=0; i<obj->mesh->numFaces; i++)
	{
		// Do the sliding.
		obj->mesh->faceTC[(i*3)].v[Y] -= (gameSpeed * texSlideSpeed);		
		obj->mesh->faceTC[(i*3)+1].v[Y] -= (gameSpeed * texSlideSpeed);		
		obj->mesh->faceTC[(i*3)+2].v[Y] -= (gameSpeed * texSlideSpeed);		

		// Deal with the case when they might wrap.
		if ((obj->mesh->faceTC[(i*3)].v[Y] < 4096) || (obj->mesh->faceTC[(i*3)+1].v[Y]<4096) || (obj->mesh->faceTC[(i*3)+2].v[Y]<4096))
		{
			obj->mesh->faceTC[(i*3)].v[Y] += 8192;		
			obj->mesh->faceTC[(i*3)+1].v[Y] += 8192;
			obj->mesh->faceTC[(i*3)+2].v[Y] += 8192;		
		}
	}
}

/*	--------------------------------------------------------------------------------
	Function		: 
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/

extern ACTOR2 *currentDrawActor2;

void XformActorList()
{
	ACTOR2 *cur;
	
	cur = actList;
	while(cur)
	{
		currentDrawActor2 = cur;
		if (gameState.mode == INGAME_MODE)
		{
			// calculate the distance between the camera and this actor
			cur->distanceFromFrog = DistanceBetweenPointsSquared(&cur->actor->pos,&frog[0]->actor->pos);

			// transform actor
			if(cur->distanceFromFrog < ACTOR_DRAWDISTANCEOUTER || cur->flags & ACTOR_DRAW_ALWAYS)
				XformActor(cur->actor);
		}
		else
		{
			// transform actor
			XformActor(cur->actor);
		}

		cur = cur->next;
	}
}

/*	--------------------------------------------------------------------------------
	Function		: DrawActorList
	Purpose			: draws the actors in the actor list....doh !!
	Parameters		: 
	Returns			: void
	Info			: 
*/

void DrawBackground(void)
{
	float oFs = fStart, oFe = fEnd;
	
	waterObject = modgyObject = 0;
	fStart = 7000.0;	fEnd = 7001.0;
	backGnd->actor->visible = 1;
	
	noClipping = 1;

	SetVector (&(backGnd->actor->pos),&(currCamSource));
	currentDrawActor2 = backGnd;
	XformActor(backGnd->actor);
	DrawActor(backGnd->actor);

	noClipping = 0;

	fStart = oFs;	fEnd = oFe;

	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ZWRITEENABLE,FALSE);
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ZENABLE,FALSE);
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_TEXTUREADDRESS, D3DTADDRESS_CLAMP);	// clamp textures
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_TEXTUREMAG,D3DFILTER_NEAREST);//D3DFILTER_LINEAR);

	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ALPHABLENDENABLE,TRUE);
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ALPHATESTENABLE,TRUE);
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ALPHAREF,0);
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ALPHAFUNC,D3DCMP_NOTEQUAL);
	
	DrawBatchedPolys();
	
	// Draw the second mavis frame set, Transparent objects (non water objects)
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ALPHATESTENABLE,FALSE);
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ZWRITEENABLE,TRUE);
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ZENABLE,TRUE);
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_TEXTUREADDRESS, D3DTADDRESS_WRAP);	// wrap textures
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_TEXTUREMAG,D3DFILTER_NEAREST);
	
	BlankFrame(_);
}

/*	--------------------------------------------------------------------------------
	Function		: DrawActorList
	Purpose			: draws the actors in the actor list....doh !!
	Parameters		: 
	Returns			: void
	Info			: 
*/

void DrawAttachedObjects(void)
{
	if (hat[0])
	{
		currentDrawActor2 = hat[0];

		XformActor(hat[0]->actor);
		DrawActor(hat[0]->actor);	
	}
}

/*	--------------------------------------------------------------------------------
	Function		: RenderObjects
	Purpose			: 
	Parameters		: 
	Returns			: void
	Info			: 
*/
extern unsigned long numHaloPoints;
void StoreHaloPoints(void);
void CheckHaloPoints(void);
void DrawHalos(void);

void RenderObjects(void)
{
	// Draw the first frame-set, opaque objects.
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_TEXTUREMAG,D3DFILTER_NEAREST);//D3DFILTER_LINEAR);
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_CULLMODE,D3DCULL_CW);
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ZFUNC,D3DCMP_LESS);
	
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ALPHABLENDENABLE,TRUE);

	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ALPHATESTENABLE,TRUE);
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ALPHAREF,0);
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ALPHAFUNC,D3DCMP_NOTEQUAL);
	
	DrawBatchedPolys();
	BlankFrame(_);
	
	// Draw the second mavis frame set, Transparent objects (non water objects)
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ALPHATESTENABLE,FALSE);
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ZWRITEENABLE,FALSE);
	
	SwapFrame(1);
	DrawBatchedPolys();
	BlankFrame(_);

	// Draw Additive frameset (num 3)
	SwapFrame(3);
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_SRCBLEND,D3DBLEND_SRCALPHA);
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_DESTBLEND,D3DBLEND_ONE);


	DrawBatchedPolys();
	BlankFrame(_);

	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_SRCBLEND,D3DBLEND_SRCALPHA);
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_DESTBLEND,D3DBLEND_ONE);
	
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_SRCBLEND,D3DBLEND_SRCALPHA);
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_DESTBLEND,D3DBLEND_INVSRCALPHA);
	// Draw the third mavis frame set, Transparent objects (non water objects)
	SwapFrame(2);
	DrawBatchedPolys();
	BlankFrame(_);	
	
	//SwapFrame(4);
	//DrawBatchedPolys();
	//BlankFrame(_);
	//pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_TEXTUREMAG,D3DFILTER_NEAREST);
	
	// We could take this out, I doubt it really matters which frame we use (Unless I put in varying poly counts, might be worth it.... Dunno)
	// SwapFrame(0);
	
	//EndDrawHardware();
	StoreHaloPoints();
	//BeginDrawHardware();
	
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ZWRITEENABLE,TRUE);
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ZENABLE,TRUE);

	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_SRCBLEND,D3DBLEND_ZERO);
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_DESTBLEND,D3DBLEND_ONE);

	SwapFrame(4);
	DrawBatchedPolys();
	BlankFrame(_);	

	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_SRCBLEND,D3DBLEND_SRCALPHA);
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_DESTBLEND,D3DBLEND_INVSRCALPHA);

	//EndDrawHardware();
	
	
	//BeginDrawHardware();
		
}

/*	--------------------------------------------------------------------------------
	Function		: DrawObjects
	Purpose			: 
	Parameters		: 
	Returns			: void
	Info			: 
*/

float slideSpeeds[4] = {0,16,32,64};

void DrawObjects(void)
{
	ACTOR2 *cur;
	unsigned long slideVal;
	
	// Init for Stuff.
	SwapFrame(0);

	// The loop that fills out mavis frame info (The complex bit)
	for (cur = actList; cur; cur = cur->next)
		if ((cur->actor->objectController) && (cur->draw))
		{
			// Slide Texture Coordinates if appropriate
			slideVal = ((cur->flags>>5) & 3);
			if (slideVal)
			{
				texSlideSpeed = slideSpeeds[slideVal];

				SlideObjectTextures(cur->actor->objectController->object);
			}
			
			// Do we modge?
			modgyObject = (cur->flags & ACTOR_MODGETEX);
			 
			// Main Draw bit, only draw if we are within range.
			if(cur->distanceFromFrog < ACTOR_DRAWDISTANCEOUTER || cur->flags & ACTOR_DRAW_ALWAYS)
			{
				if (cur->flags & ACTOR_WATER)
				{
					// Draw a waterbased object, this does the setting, and setting back of states since it is called far less frequently
					waterObject = 1;
						
					if (cur->flags & ACTOR_SLUDGE)
						sludgeObject = 1;
					else
						sludgeObject = 0;

					if (cur->flags & ACTOR_LEAVES)
						leafObject = 1;
					else
						leafObject = 0;

					SwapFrame(2);

					DrawActor(cur->actor);

					SwapFrame(0);
					waterObject = 0;
				}
				else
				{
					// Put translucent clipping back in when objects are ordered right
//					if( cur->distanceFromFrog > ACTOR_DRAWDISTANCEINNER )
//						cur->actor->xluOverride = 100*(1.0-(sqrtf(((float)(cur->distanceFromFrog - ACTOR_DRAWDISTANCEINNER)))/ACTOR_DRAWFADERANGE));

					if ((cur->actor->objectController->object->flags & OBJECT_FLAGS_XLU) || (cur->actor->flags & OBJECT_FLAGS_XLU))
					{
						// Draw transparent objects.
						if (cur->flags & ACTOR_ADDITIVE)
						{
							// Additive-Xlu
							SwapFrame(3);
							DrawActor(cur->actor);
						}
						else
						{			
							// Semi-Xlu
							SwapFrame(1);
							DrawActor(cur->actor);
						}
						SwapFrame(0);
					}
					else
					{
						// Opaque objects are a piece of piss.
						DrawActor(cur->actor);
					}
				}
			}
		}
	
	// Draw the hats (Would be much better if we had an "attached" object field in the OBJECT structure, 
	DrawAttachedObjects();
}

/*	--------------------------------------------------------------------------------
	Function		: DrawActorList
	Purpose			: 
	Parameters		: 
	Returns			: void
	Info			: 
*/

void DrawActorList(void)
{
	// Kill any left over frame information
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ALPHABLENDENABLE,FALSE);
	BlankFrame(_);
	
	// Draw our background object, if applicable
	if (backGnd)
		DrawBackground();

	// Calculate our actor fade range!
	ACTOR_DRAWFADERANGE = sqrtf((float)ACTOR_DRAWDISTANCEOUTER - (float)ACTOR_DRAWDISTANCEINNER);	

	// Draws all the objects (Have added to mavis, support for multiple frames allowing all the objects to be drawn in one go.)
	DrawObjects();

	// Renders the Mavis Set
	RenderObjects();
}








































































/*	--------------------------------------------------------------------------------
	Function		: DrawActorList
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/

void DrawActorListOLD()
{
	/****************************************************************************************/
	// IMPORTANT NOTE FROM SHARKY...
	// I HAVE USED #ifdef's TO SPLIT UP THE DRAWING OF ACTORS BASED ON THE HARDWARE BEING
	//	USED...I.E. PC OR N64...
	//	THIS IS BECAUSE IT'S BEING A PAIN IN THE ARSE ON THE N64 AND I'M GONNA KICK IT'S
	//	COCK OFF AND FIX IT AS SOON AS I CAN.
	/****************************************************************************************/
	
	ACTOR2 *cur;
//	float ACTOR_DRAWFADERANGE = sqrtf(ACTOR_DRAWDISTANCEOUTER) - sqrtf(ACTOR_DRAWDISTANCEINNER);

	/****************************************************************************************/
	/* PC SPECIFIC VERSION																	*/
	/****************************************************************************************/

#ifdef PC_VERSION
		
/*
	if (!cur->draw || cur->flags & ACTOR_WATER || !cur->actor->objectController)
			continue;
	
		if ((cur->flags & ACTOR_SLIDYTEX))
		{
			if ((cur->flags & ACTOR_SLIDYTEX2))
				slideSpeed = 64;
			else
				slideSpeed = 16;
		}
		else
			if ((cur->flags & ACTOR_SLIDYTEX2))
				slideSpeed = 32;
		*/
	
/*		if( (cur->flags & ACTOR_DRAW_CULLED) && (cur->distanceFromFrog > ACTOR_DRAWDISTANCEINNER) && !(cur->flags & ACTOR_DRAW_ALWAYS))
		{
			if( cur->distanceFromFrog < ACTOR_DRAWDISTANCEOUTER )
			{
				float fadeValue =  1.0-(sqrtf(((float)(cur->distanceFromFrog - ACTOR_DRAWDISTANCEINNER)))/ACTOR_DRAWFADERANGE);

				cur->actor->objectController->object->flags |= OBJECT_FLAGS_XLU;
				cur->actor->xluOverride = fadeValue*100;				
			}
			else
			{
				cur->actor->objectController->object->flags &= ~OBJECT_FLAGS_XLU;
			}
		}
		else
		{
			if( cur->flags & ACTOR_DRAW_CULLED )
			{
				cur->actor->xluOverride = 100;
				cur->actor->objectController->object->flags &= ~OBJECT_FLAGS_XLU;
			}

			if( cur->draw && !(cur->actor->objectController->object->flags & OBJECT_FLAGS_XLU) )
			{
				DrawActor(cur->actor);
			}
		}
*/	
	/* Water objects */
	#ifndef RELEASE_BUILD
	if (showActorNames)
		dprintf"-------------------------------------------------------------\n"));
	#endif

	waterObject = 1;
	for (cur = actList; cur; cur = cur->next)
	{
		#ifndef RELEASE_BUILD
		if (showActorNames)
		{
			if (cur->actor->objectController)
				dprintf"--- %s ---\n",cur->actor->objectController->object->name));
			else
				dprintf"*** nocontroll ***\n",cur->actor->objectController->object->name));
		}
		#endif

		if(cur->flags & ACTOR_WATER && cur->actor->objectController)
			DrawActor(cur->actor);
	}
	
	#ifndef RELEASE_BUILD
	if (showActorNames)
	{
		dprintf"-------------------------------------------------------------\n"));
		showActorNames = 0;
	}
	#endif

	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ALPHABLENDENABLE,TRUE);
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ZWRITEENABLE,FALSE);
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_TEXTUREMAG,D3DFILTER_NEAREST);

	DrawBatchedPolys();
	BlankFrame(x);

	/* Transparent objects..? */
	
	waterObject = 0;
	for(cur = actList; cur; cur = cur->next)
	{
		if( !cur->draw || (cur->flags & (ACTOR_WATER | ACTOR_DRAW_CULLED)) || cur->distanceFromFrog > ACTOR_DRAWDISTANCEOUTER)
			continue;

		if( !cur->actor->objectController || !cur->actor->objectController->object->flags & OBJECT_FLAGS_XLU )
			continue;

		modgyObject = (cur->flags & ACTOR_MODGETEX);

		DrawActor(cur->actor);
		
		if (cur->flags & ACTOR_ADDITIVE)
		{
			pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_SRCBLEND,D3DBLEND_SRCALPHA);
			pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_DESTBLEND,D3DBLEND_ONE);
//				pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_DESTBLEND,D3DBLEND_INVSRCALPHA);
			pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_TEXTUREMAG,D3DFILTER_NEAREST);//D3DFILTER_LINEAR);
		}

		DrawBatchedPolys();
		BlankFrame(x);

		if (cur->flags & ACTOR_ADDITIVE)
		{
			pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_TEXTUREMAG,D3DFILTER_NEAREST);
			pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_SRCBLEND,D3DBLEND_SRCALPHA);
			pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_DESTBLEND,D3DBLEND_INVSRCALPHA);
		}
	}

/*	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ALPHABLENDENABLE,TRUE);
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ZWRITEENABLE,FALSE);
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_TEXTUREMAG,D3DFILTER_LINEAR);

	DrawBatchedPolys();
	BlankFrame();

	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_TEXTUREMAG,D3DFILTER_NEAREST);
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ZWRITEENABLE,TRUE);
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ALPHABLENDENABLE,FALSE);
*/	

	
#endif



	/****************************************************************************************/
	/* N64 SPECIFIC VERSION																	*/
	/****************************************************************************************/
	
#ifdef N64_VERSION
	
	vtxPtr = &(objectsVtx[draw_buffer][0]);

	// fogging ?
	if(fog.mode)
	{
	   gDPSetFogColor(glistp++,fog.r,fog.g,fog.b,255);
	   gSPFogPosition(glistp++,fog.min,fog.max);
	}

	// draw non-xlu objects and calculate xlu values for actor fading
	cur = actList;
	waterObject = 0;
	currN64ModgyTexObject = numN64ModgyTexObjects - 1;	// number of N64 modgy tex objects to draw
	while(cur)
	{
		if((cur->flags & ACTOR_DRAW_NEVER) || (cur->flags & ACTOR_WATER))
		{
			// do not display this object yet, if at all
			cur = cur->next;
			continue;
		}

		if((!cur->actor->objectController) ||
			(cur->actor->objectController->object->flags & OBJECT_FLAGS_XLU) ||
			(cur->flags & ACTOR_DRAW_LAST))
		{
			// don't draw object yet
			cur = cur->next;
			continue;
		}

		if(cur->flags & ACTOR_DRAW_ALWAYS)
		{
			// always draw this actor without "xlu-culling"
			if(cur->flags & ACTOR_MODGETEX)
			{
				UpdateModgyTexN64(cur);
				DrawActor(cur->actor);
				currN64ModgyTexObject--;
			}
			else
				DrawActor(cur->actor);
		}
		else
		{
			// check if distance from frog is outside inner draw distance radius
			if(cur->distanceFromFrog < ACTOR_DRAWDISTANCEINNER)
			{
				// draw actor
				DrawActor(cur->actor);
			}
		}

		cur = cur->next;
	}

	// go ahead and draw the xlu water objects
	waterObject = 1;
	cur = actList;
	currN64WaterObject = numN64WaterObjects - 1;	// number of N64 water objects to draw
	while(cur)
	{
		// only draw xlu water objects
		if(cur->flags & ACTOR_WATER)
		{
			UpdateWaterN64(cur);
			DrawActor(cur->actor);
			currN64WaterObject--;					// decrement number of water objects to draw
		}
		
		cur = cur->next;
	}

	// now draw the xlu objects that aren't water objects
	waterObject = 0;
	cur = actList;
	while(cur)
	{
		// don't draw the water objects
		if(!(cur->actor->objectController) || (cur->flags & ACTOR_WATER))
		{
			cur = cur->next;
			continue;
		}

		if(cur->actor->objectController->object->flags & OBJECT_FLAGS_XLU)
		{
			if(cur->distanceFromFrog < ACTOR_DRAWDISTANCEOUTER)
			{
				DrawActor(cur->actor);
			}
		}
		
		cur = cur->next;
	}

#endif
}













/* --------------------------------------------------------------------------------
	Programmer	: Matthew Cloy
	Function	: FreeActorList

	Purpose		:
	Parameters	: (void)
	Returns		: void 
*/

void FreeActor(ACTOR *c)
{
	if((c->objectController) && (c->objectController->object))
	{
	 	FreeObjectSprites(c->objectController->object);

		// NEW
		if(c->objectController->drawList)
		{
			JallocFree((UBYTE **)&c->objectController->vtx[0]);
			JallocFree((UBYTE **)&c->objectController->drawList);
		}

		// NEW
		RemoveUniqueObject(c->objectController->object);
		JallocFree((UBYTE **)&c->objectController);
	}

	/*
	if(c->LODObjectController)
		JallocFree((UBYTE **)&c->LODObjectController);
*/
	if(c->matrix)
		JallocFree((UBYTE **)&c->matrix);

	if(c->animation)
		JallocFree((UBYTE **)&c->animation);

	if(c->shadow)
		JallocFree((UBYTE **)&c->shadow);

	JallocFree((UBYTE**)&c);
}


void FreeActor2(ACTOR2 *c)
{
	if (!c) return;
	
	FreeActor(c->actor);
	JallocFree((UBYTE**)&c);
}

/* --------------------------------------------------------------------------------
	Programmer	: Matthew Cloy
	Function	: FreeActorList

	Purpose		:
	Parameters	: (void)
	Returns		: void 
*/
void FreeActorList()
{
	ACTOR2 *cur,*next;
	
	dprintf"Freeing linked list : ACTOR2\n"));
	
	cur = actList;
	while (cur)
	{
		next = cur->next;
		FreeActor2(cur);		
		cur = next;
	}
	actList = NULL;

	FreeActor2(backGnd);		
	backGnd = NULL;

	FreeActor2(hat[0]);		
	hat[0] = NULL;
}

/* --------------------------------------------------------------------------------
	Programmer	: Matthew Cloy
	Function	: CreateAndAddActor 
	Purpose		:
	Parameters	: 
	Returns		: ACTOR2 *
*/
ACTOR2 *CreateAndAddActor(char *name,float cx,float cy,float cz,int initFlags)
{
	ACTOR2 *newItem;
	currentDrawActor2 = NULL;

	newItem			= (ACTOR2 *)JallocAlloc(sizeof(ACTOR2),YES,"ACTOR2");
	newItem->actor	= (ACTOR *)JallocAlloc(sizeof(ACTOR),YES,"ACTOR");

	InitActor(newItem->actor,name,cx,cy,cz,initFlags);
	MakeUniqueActor(newItem->actor,0);

	newItem->actor->oldpos.v[X]	= cx;
	newItem->actor->oldpos.v[Y]	= cy;
	newItem->actor->oldpos.v[Z]	= cz;

	newItem->flags = 0;
	newItem->draw	= 1;
	newItem->radius	= 0.0F;
	newItem->animSpeed = 1.0F;
	newItem->value1 = 0.0F;
	newItem->effects = 0;

	// add actor object sprites to sprite list
	if((newItem->actor->objectController) && (newItem->actor->objectController->object))
		AddObjectsSpritesToSpriteList(newItem->actor->objectController->object,0);
	
	if (strncmp("ghostie",name,6)==0)
	{
		newItem->flags = ACTOR_WATER;
	}
	else
	if(name[0] != 'x' && name[1] != 'x')
		newItem->flags = ACTOR_DRAW_CULLED;
	else
		newItem->flags = ACTOR_DRAW_ALWAYS;

	if(name[0] != 'x' && name[1] != 'x')
		newItem->flags = ACTOR_DRAW_CULLED;
	else
	{
		if (name[3]=='g')
		{
			newItem->flags = ACTOR_DRAW_ALWAYS | ACTOR_MODGETEX;
			if (name[4]=='_')
				newItem->flags |= ACTOR_SLIDYTEX;

		
#ifdef N64_VERSION
			// add support for modgy objects
			AddN64ModgyTexObjectResource(newItem->actor);
#endif
		}
		else
		{
			if (name[2]=='a')
			{
				newItem->flags = ACTOR_DRAW_ALWAYS | ACTOR_ADDITIVE;

#ifdef N64_VERSION
				// add support for modgy objects
				AddN64ModgyTexObjectResource(newItem->actor);
#endif
			}
			else
			{
				newItem->flags = ACTOR_DRAW_ALWAYS;
			}
		}
	}

	if (name[0] == 's')
		if (name[1] == 'p')
		{
			switch (name[2])
			{
				case 'l':
					newItem->flags |= ACTOR_SLIDYTEX;
					break;
				case 'm':
					newItem->flags |= ACTOR_SLIDYTEX2;
					break;
				case 'f':
					newItem->flags |= (ACTOR_SLIDYTEX | ACTOR_SLIDYTEX2);
					break;
			}
		}


	newItem->speed				= 18.0;
	newItem->offset				= 0.0;
	newItem->distanceFromFrog	= 0.0F;

	newItem->next = actList;
	newItem->prev = NULL;
	
	if (actList) actList->prev = newItem;
	actList = newItem;
	
	return newItem;
}


/*	--------------------------------------------------------------------------------
	Function		: AddObjectsSpritesToSpriteList
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/

void AddObjectsSpritesToSpriteList(OBJECT *obj,short flags)
{
	SPRITE *sprite;
	int i;

	if(obj->numSprites > 0)
	{
		for(i=0; i<obj->numSprites; i++)
		{
			sprite = AllocateSprites( 1 );//(SPRITE *)JallocAlloc(sizeof(SPRITE),YES,"ObjSprite");

			sprite->texture = obj->sprites[i].textureID;
			SetVector(&sprite->pos,&zero);

			if((obj->sprites[i].flags & SPRITE_DONE) == 0)
			{
				obj->sprites[i].flags |= SPRITE_DONE;

#ifdef N64_VERSION
				switch(sprite->texture->sx)
				{
					case 8:
//						obj->sprites[i].sx /= 2;
						break;
					case 16:
						obj->sprites[i].sx *= 2;
						break;
					case 32:
						obj->sprites[i].sx *= 3;
						break;
				}
				switch(sprite->texture->sy)
				{
					case 8:
//						obj->sprites[i].sy /= 2;
						break;
					case 16:
						obj->sprites[i].sy *= 2;
						break;
					case 32:
						obj->sprites[i].sy *= 3;
						break;
				}
#endif
			}

			sprite->r = sprite->g = sprite->b = 255;
			sprite->a = 128;
			if(obj->xlu < 255)
				sprite->flags = SPRITE_TRANSLUCENT;
			else
				sprite->flags = 0;

			sprite->flags |= flags;

			sprite->offsetX = -32 / 2;
			sprite->offsetY = -32 / 2;

			//AddSprite(sprite,NULL);
			obj->sprites[i].sprite = sprite;
		}
	}

	if(obj->children)
		AddObjectsSpritesToSpriteList(obj->children,flags);

	if(obj->next)
		AddObjectsSpritesToSpriteList(obj->next,flags);
}

/*	--------------------------------------------------------------------------------
	Function		: RemoveObjectSprites
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/
/*
void RemoveObjectSprites(OBJECT *obj,BOOL f)
{
	int i;

	if(obj->sprites)
	{
		for(i=0; i<obj->numSprites; i++)
		{
			if(obj->sprites[i].sprite)
				DeallocateSprites(obj->sprites[i].sprite, 1);
//			if(f)
//				JallocFree((UBYTE**)&obj->sprites[i].sprite);
		}
	}

	if(obj->children)
		RemoveObjectSprites(obj->children, f);
	
	if(obj->next)
		RemoveObjectSprites(obj->next, f);
}
*/
/*	--------------------------------------------------------------------------------
	Function		: FreeObjectSprites
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/

void FreeObjectSprites(OBJECT *obj)
{
	int i;

	if(obj->sprites)
		for(i=0; i<obj->numSprites; i++)
			if(obj->sprites[i].sprite)
				DeallocateSprites(obj->sprites[i].sprite,1);

	if(obj->children)
		FreeObjectSprites(obj->children);
	
	if(obj->next)
		FreeObjectSprites(obj->next);
}


/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void MakeUniqueVtx(OBJECT_CONTROLLER *objC)
{
	Vtx *vtxa;
	Vtx *vtxb;
	Vtx *oldVtxa, *oldVtxb;

	oldVtxa = objC->vtx[0];
	oldVtxb = objC->vtx[1];

	vtxa = (Vtx *)JallocAlloc(sizeof(Vtx) * objC->numVtx * 2, NO, "unqVtx");
	vtxb = vtxa + objC->numVtx;

	memcpy(vtxa, oldVtxa, sizeof(Vtx) * objC->numVtx);
	memcpy(vtxb, oldVtxb, sizeof(Vtx) * objC->numVtx);

	//controller now references new vtx's, must also make sure that drawlist is updated
	objC->vtx[0] = vtxa;
	objC->vtx[1] = vtxb;
}

/*	--------------------------------------------------------------------------------
	Function		: MakeUniqueObject
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/

OBJECT *MakeUniqueObject(OBJECT *object)
{
	OBJECT	*obj;	
	OBJECTSPRITE **spr, *tempSpr;
//	int		i;
		
	obj = object;
	object = (OBJECT *)JallocAlloc(sizeof(OBJECT), YES, "UniqObj");
	memcpy(object, obj, sizeof(OBJECT));

	if(obj->numSprites)
	{
		spr = &object->sprites;
		tempSpr = (OBJECTSPRITE *)JallocAlloc(sizeof(OBJECTSPRITE) * obj->numSprites, YES, "UniqSpr");
		memcpy(tempSpr, *spr, sizeof(OBJECTSPRITE) * obj->numSprites);
		*spr = tempSpr;
/*		for(i = 0; i < obj->numSprites; i++)
		{
			spr = &object->sprites;
			spr += i;
			tempSpr = (OBJECTSPRITE *)JallocAlloc(sizeof(OBJECTSPRITE), YES, "UniqSpr");
			memcpy(tempSpr, *spr, sizeof(OBJECTSPRITE));
			*spr = tempSpr;
		}
 */
	}

	if(object->children)
		object->children = MakeUniqueObject(object->children);

	if(object->next)
		object->next = MakeUniqueObject(object->next);
		
	return object;
}


/*	--------------------------------------------------------------------------------
	Function 	: MakeUniqueActor
	Purpose 	: Makes an actor unique by giving it its own object controller, and calls
        		  make unique object, to sort out the object (including children)
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void MakeUniqueActor(ACTOR *actor,int type)
{
	OBJECT_CONTROLLER *objCont;
	short	unique = TRUE;
	short	i;
	int		CRC;
	
	if(!actor->objectController)
		return;
		
	CRC = UpdateCRC(actor->objectController->object->name);

	//check all crc's to see if actor is among them
	for(i = 0; i < numUniqueActors; i++)
	{
		if(uniqueActorCRC[i] == CRC)
		{
			//if it is, actor is not unique and must have seperate stuff
			//dprintf"found duplicate actor %s\n", actor->objectController->object->name));
			unique = FALSE;
			break;
		}
	}
	//if actor is not in list
	if(unique == TRUE)
	{
		uniqueActorCRC[numUniqueActors++] = CRC;
	}	

	objCont = actor->objectController;
	actor->objectController = (OBJECT_CONTROLLER *)JallocAlloc(sizeof(OBJECT_CONTROLLER), YES, "UniqObjC");
	memcpy(actor->objectController, objCont, sizeof(OBJECT_CONTROLLER));
	actor->objectController->object = MakeUniqueObject(actor->objectController->object);

	if(unique == FALSE)
	{
		//if actor is skinned, duplicate Vtx's
		if(actor->objectController->drawList)
		{
			MakeUniqueVtx(actor->objectController);
			
			XformActor(actor);
		}

		// if actor is a water object, duplicate Vtx's
		if(type == ACTOR_WATER)
		{
			dprintf"Adding water object...\n"));
			MakeUniqueVtx(actor->objectController);
			XformActor(actor);
		}
	}
/*
	objCont = actor->LODObjectController;
	if(objCont)
	{
		actor->LODObjectController = (OBJECT_CONTROLLER *)JallocAlloc(sizeof(OBJECT_CONTROLLER), YES, "UniqObjC");
		memcpy(actor->LODObjectController, objCont, sizeof(OBJECT_CONTROLLER));
		actor->LODObjectController->object = MakeUniqueObject(actor->LODObjectController->object);
	}
	*/
}


/*	--------------------------------------------------------------------------------
	Function 	: ResetUniqueActorList
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void ResetUniqueActorList()
{
	int i;

	for(i=0; i<MAX_UNIQUE_ACTORS; i++)
		uniqueActorCRC[i] = 0;
}



/*	--------------------------------------------------------------------------------
	Function 	: RemoveUniqueObject
	Purpose 	: Frees all the unique stuff from an object
	Parameters 	: OBJECT *object
	Returns 	: OBJECT *
	Info 		:
*/
void RemoveUniqueObject(OBJECT *object)
{
	OBJECT	*obj;	
	OBJECTSPRITE **spr;
		
	obj = object;

	if(object->numSprites)
	{
		spr = &object->sprites;
		JallocFree((UBYTE**)spr);
	}

	if(object->children)
		RemoveUniqueObject(object->children);

	if(object->next)
		RemoveUniqueObject(object->next);
		
	JallocFree((UBYTE**)&obj);
}

/*	-------------------------------------------------------------
	Function	: SwapActorObject
	Purpose		: Swaps the object associated with la la la blah
	Returns		:
*/
void SwapActorObject(ACTOR2 *act, const char* name)
{
	VECTOR pos = act->actor->pos;

	FreeActor(act->actor);

	act->actor	= (ACTOR *)JallocAlloc(sizeof(ACTOR),YES,"ACTOR");
	InitActor(act->actor, (char*)name, pos.v[0], pos.v[1], pos.v[2], INIT_ANIMATION);
	MakeUniqueActor(act->actor,0);
}

/*	--------------------------------------------------------------------------------
	Function 	: RemoveUniqueActor
	Purpose 	: removes unique actors
	Parameters 	: ACTOR *,int
	Returns 	: void
	Info 		:
*/
void ActorLookAt( ACTOR *act, VECTOR *at, long flags )
{
	VECTOR forward, dir;
	QUATERNION q,q2,q3;
	float a,b;
	
	SubVector( &dir, &act->pos, at );

	SetVector( &forward, &dir );
	forward.v[Y] = 0;
	MakeUnit(&forward);
	a = -DotProduct(&forward,&inVec);
	q.x = 0;
	q.y = 1;
	q.z = 0;
	if (forward.v[X]<0)
		q.w = acos(a);
	else
		q.w = -acos(a);

	if( flags == LOOKAT_ANYWHERE )
	{
		GetQuaternionFromRotation(&q3,&q);

		SetVector( &forward, &dir );
		b = sqrtf(forward.v[X]*forward.v[X]+forward.v[Z]*forward.v[Z]);
		
		forward.v[X] = 0;
		forward.v[Z] = b;
		MakeUnit(&forward);

		a = DotProduct(&forward,&upVec);
		q.x = 1;
		q.y = 0;
		q.z = 0;
		if (forward.v[Y]<0)
			q.w = (acos(a)-PI_OVER_2);
		else
			q.w = (PI_OVER_2-acos(a));
		
		GetQuaternionFromRotation(&q2,&q);

		QuaternionMultiply (&act->qRot,&q3,&q2);
	}
	else
	{
		GetQuaternionFromRotation(&act->qRot,&q);
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: Orientate
	Purpose 	: Creates an orientation quaternion from a given UNIT forward and UNIT up vector
	Parameters 	: ->quat, forward', forward, up'
	Returns 	: void
	Info 		:
*/
#define OEPSILON ((float)0.01)

void Orientate(QUATERNION *me, VECTOR *fd, VECTOR *up)
{
	QUATERNION r1,q1,q2;
	VECTOR v1,v2,v3,v4;
	float dp,dp2,scl;
	float thetaOver2;
	float sinThetaOver2;
	long q1Unimportant = 0,q1Flip = 0;
	
	dp = up->v[Y];
	
	if (dp<1-OEPSILON)
	{
		if (dp>OEPSILON-1)
		{
			r1.x = -up->v[Z];
			r1.y = 0;
			r1.z = up->v[X];
			r1.w = acos(dp);

			// Normalise it (only need to do 2 coords!)
			scl = 1.0/sqrtf((r1.x*r1.x)+(r1.z*r1.z));
			r1.x *= scl;
			r1.z *= scl;

			RotateVectorByXZRotation(&v1,fd,&r1);
	
			r1.w = -r1.w;
			v1.v[Y] = 0;

			GetQuaternionFromXZRotation(&q1,&r1);
		}
		else
		{
			// We're just fliping the X&Y coords!
			v1.v[X] = fd->v[X];
			v1.v[Y] = 0;
			v1.v[Z] = -fd->v[Z];
			q1.w = q1.y = q1.z = 0;
			q1.x = 1;			

			q1Flip = 1;			// We don't need the qMult later on!
		}
	}
	else
	{
		v1.v[X] = fd->v[X];
		v1.v[Y] = 0;
		v1.v[Z] = fd->v[Z];
		
		q1.x = q1.y = q1.z = 0;
		q1.w = 1;			

		q1Unimportant = 1;	
	}

	// Again, makeunit from the two used values
	scl = 1.0/sqrtf((v1.v[X]*v1.v[X])+(v1.v[Z]*v1.v[Z]));	
	dp2 = v1.v[Z] * scl;

	if (dp2<1-OEPSILON)
	{
		if (dp2>OEPSILON-1)
		{
			
			if (q1Unimportant)
			{

				if (v1.v[X]>0)
					thetaOver2 = acos(dp2)*0.5;
				else
					thetaOver2 = acos(dp2)*-0.5;

				sinThetaOver2 = sinf(thetaOver2);

				me->x = me->z = 0;
				me->w = cosf(thetaOver2);
				me->y = sinThetaOver2;
			}
			else
			{
				SetVector((VECTOR *)(&r1),up);
				
				if (v1.v[X]>0)
					r1.w = acos(dp2);
				else
					r1.w = -acos(dp2);

				GetQuaternionFromRotation(&q2,&r1);			

				if (q1Flip)
				{
					me->w = -q2.x;
					me->x = q2.w;
					me->y = q2.z;
					me->z = -q2.y;
				}
				else
					QuaternionMultiply(me,&q2,&q1);
			}
		}
		else
		{	
			// This could be more optimal!
			q2.w = 0;
			q2.x = -up->v[X];
			q2.y = -up->v[Y];
			q2.z = -up->v[Z];
			
			if (q1Unimportant)
				SetQuaternion(me,&q2);
			else
			{
				if (q1Flip)
				{
					me->w = -q2.x;
					me->x = q2.w;
					me->y = q2.z;
					me->z = -q2.y;
				}
				else
				{
					// QuatMult of a w = 0 quaternion.
					me->w = -q2.x*q1.x - q2.y*q1.y - q2.z*q1.z;
					me->x = q2.x*q1.w + q2.y*q1.z - q2.z*q1.y;
					me->y = q2.y*q1.w + q2.z*q1.x - q2.x*q1.z;
					me->z = q2.z*q1.w + q2.x*q1.y - q2.y*q1.x;
				}
			}
		}
	}
	else // No change, hence no need for qmult.		
		SetQuaternion(me,&q1);			
}

