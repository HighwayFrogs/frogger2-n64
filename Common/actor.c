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

#define MAX_UNIQUE_ACTORS	50


unsigned long ACTOR_DRAWDISTANCEINNER = 250000;
unsigned long ACTOR_DRAWDISTANCEOUTER = 500000;


#define WATER_XLU 70
long waterObject = 0;
long modgyObject = 0;
int objectMatrix = 0;

ACTOR2 *actList = NULL;				// entire actor list
ACTOR2 *backGnd = NULL;
ACTOR2 *globalLevelActor = NULL;	// ptr to actor representing level

//used to keep a count of how many of each enemy are present at the same time
//char uniqueEnemyCount[20];

int uniqueActorCRC[MAX_UNIQUE_ACTORS];
char numUniqueActors = 0;
extern ACTOR2 *hat[MAX_FROGS];

/* --------------------------------------------------------------------------------	
	Programmer	: Matthew Cloy
	Function    : DrawActorList

	Purpose		:
	Parameters	: (void)
	Returns		: void 
*/
void XformActor(ACTOR *ptr);
float texSlideSpeed = 40;
void SlideObjectTextures(OBJECT *obj)
{
	int i;
	for (i=0; i<obj->mesh->numFaces*3; i++)
	{
		obj->mesh->faceTC[i].v[Y] -= (gameSpeed * texSlideSpeed);
	}
}

/*	--------------------------------------------------------------------------------
	Function		: 
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/
void XformActorList()
{
	ACTOR2 *cur;
	
	cur = actList;
	while(cur)
	{
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
float bFOV = 450.0;
extern float fStart,fEnd, FOV;
extern long noClipping;
void DrawActorList()
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
	float ACTOR_DRAWFADERANGE = sqrtf((float)ACTOR_DRAWDISTANCEOUTER - (float)ACTOR_DRAWDISTANCEINNER);
	
	BlankFrame(_);

	if (backGnd)
	{
		float oFs = fStart, oFe = fEnd;
		
		waterObject = 0;
		modgyObject = 0;

		fStart = 7000.0;
		fEnd = 7001.0;

		backGnd->actor->visible = 1;
		
		noClipping = 1;

		SetVector (&(backGnd->actor->pos),&(currCamSource[0]));
		XformActor(backGnd->actor);
		DrawActor(backGnd->actor);

		noClipping = 0;

		fStart = oFs;
		fEnd = oFe;

		pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ZWRITEENABLE,FALSE);
		pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ZENABLE,FALSE);
		pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_TEXTUREADDRESS, D3DTADDRESS_CLAMP);	// clamp textures
//		pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_WRAPV, 0);	// wrap textures
		pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_TEXTUREMAG,D3DFILTER_LINEAR);
		
		DrawBatchedPolys();
		BlankFrame(_);

		pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_TEXTUREADDRESS, D3DTADDRESS_WRAP);	// wrap textures
	}

	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ZWRITEENABLE,TRUE);
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ZENABLE,TRUE);
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_TEXTUREMAG,D3DFILTER_NEAREST);
		
	cur = actList;
	waterObject = 0;

	BlankFrame(_);

	while(cur)
	{
		float slideSpeed = 0;

		waterObject = 0;
		modgyObject = 0;

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

	
		if (slideSpeed>1)
		{
			texSlideSpeed = slideSpeed;
			if (cur->actor->objectController)
				SlideObjectTextures(cur->actor->objectController->object);
		}

		if(((cur->flags & ACTOR_WATER)) || (!cur->actor->objectController))
		{
			cur = cur->next;
			continue;
		}
		
		if (cur->flags & ACTOR_MODGETEX)
			modgyObject = 1;
		else
			modgyObject = 0;

		if( (cur->flags & ACTOR_DRAW_CULLED) &&
			(cur->distanceFromFrog > ACTOR_DRAWDISTANCEINNER) &&
			!(cur->flags & ACTOR_DRAW_ALWAYS) )
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

			if(gameState.mode == INGAME_MODE || gameState.mode == OBJVIEW_MODE || 
			   gameState.mode == RECORDKEY_MODE || gameState.mode == LEVELPLAYING_MODE ||
			   gameState.mode == FRONTEND_MODE  || gameState.mode == CAMEO_MODE || gameState.mode == PAUSE_MODE )
			{
				if( cur->draw )
				if( !(cur->actor->objectController->object->flags & OBJECT_FLAGS_XLU) )
				{
					DrawActor(cur->actor);
				}
			}
		}
	
		cur = cur->next;
	}
	
//	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ALPHABLENDENABLE,FALSE);
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_CULLMODE,D3DCULL_CW);
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ZENABLE,TRUE);
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ZFUNC,D3DCMP_LESS);
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ZWRITEENABLE,TRUE);

	if (hat[0])
	{
		XformActor(hat[0]->actor);
		DrawActor(hat[0]->actor);	
	}

	DrawBatchedPolys();
	BlankFrame();
	
	waterObject = 1;
	cur = actList;
	while(cur)
	{
		if((!(cur->flags & ACTOR_WATER)) || (!cur->actor->objectController))
		{
			cur = cur->next;
			continue;
		}

		if( gameState.mode == INGAME_MODE || gameState.mode == OBJVIEW_MODE || 
			gameState.mode == RECORDKEY_MODE || gameState.mode == LEVELPLAYING_MODE ||
			gameState.mode == FRONTEND_MODE  || gameState.mode == CAMEO_MODE || gameState.mode == PAUSE_MODE )
		{
			DrawActor(cur->actor);
		}
		
		cur = cur->next;
	}

	waterObject = 0;
	cur = actList;
	while(cur)
	{
		if( ((cur->flags & ACTOR_WATER)) || (!cur->actor->objectController))
		{
			cur = cur->next;
			continue;
		}

		if( cur->actor->objectController->object->flags & OBJECT_FLAGS_XLU )
		if( !((cur->flags & ACTOR_DRAW_CULLED) && (cur->distanceFromFrog > ACTOR_DRAWDISTANCEOUTER)) )
		if( gameState.mode == INGAME_MODE || gameState.mode == OBJVIEW_MODE || 
			gameState.mode == RECORDKEY_MODE || gameState.mode == LEVELPLAYING_MODE ||
			gameState.mode == FRONTEND_MODE  || gameState.mode == CAMEO_MODE || gameState.mode == PAUSE_MODE )
		{
				
		if (cur->flags & ACTOR_MODGETEX)
			modgyObject = 1;
		else
			modgyObject = 0;

			DrawActor(cur->actor);
		}
		
		cur = cur->next;
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

void FreeActor(ACTOR2 *c)
{
	ACTOR2 *cur = c;

	if (!cur)
		return;

	if((cur->actor->objectController) && (cur->actor->objectController->object))
	{
	 	FreeObjectSprites(cur->actor->objectController->object);

		// NEW
		if(cur->actor->objectController->drawList)
		{
			JallocFree((UBYTE **)&cur->actor->objectController->vtx[0]);
			JallocFree((UBYTE **)&cur->actor->objectController->drawList);
		}

		// NEW
		RemoveUniqueObject(cur->actor->objectController->object);
		JallocFree((UBYTE **)&cur->actor->objectController);
	}

	if(cur->actor->LODObjectController)
		JallocFree((UBYTE **)&cur->actor->LODObjectController);

	if(cur->actor->matrix)
		JallocFree((UBYTE **)&cur->actor->matrix);

	if(cur->actor->animation)
		JallocFree((UBYTE **)&cur->actor->animation);

	if(cur->actor->shadow)
		JallocFree((UBYTE **)&cur->actor->shadow);

	JallocFree((UBYTE**)&cur->actor);
	JallocFree((UBYTE**)&cur);
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
		FreeActor(cur);		
		cur = next;
	}
	actList = NULL;

	FreeActor(backGnd);		
	backGnd = NULL;

	FreeActor(hat[0]);		
	hat[0] = NULL;
}

/* --------------------------------------------------------------------------------
	Programmer	: Matthew Cloy
	Function	: CreateAndAddActor 
	Purpose		:
	Parameters	: 
	Returns		: ACTOR2 *
*/
ACTOR2 *CreateAndAddActor(char *name,float cx,float cy,float cz,int initFlags,float offset,int startNode)
{
	ACTOR2 *newItem;
	
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

#ifdef N64_VERSION
			// add support for modgy objects
			AddN64ModgyTexObjectResource(newItem->actor);
#endif
		}
		else
		{
			if (name[2]=='a')
			{
				newItem->flags = ACTOR_DRAW_ALWAYS | ACTOR_MODGETEX | ACTOR_SLIDYTEX;

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
			sprite = (SPRITE *)JallocAlloc(sizeof(SPRITE),YES,"ObjSprite");

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

			sprite->offsetX = -sprite->texture->sx / 2;
			sprite->offsetY = -sprite->texture->sy / 2;

			AddSprite(sprite,NULL);
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
void RemoveObjectSprites(OBJECT *obj,BOOL f)
{
	int i;

	if(obj->sprites)
	{
		for(i=0; i<obj->numSprites; i++)
		{
			if(obj->sprites[i].sprite)
				SubSprite(obj->sprites[i].sprite);
			if(f)
				JallocFree((UBYTE**)&obj->sprites[i].sprite);
		}
	}

	if(obj->children)
		RemoveObjectSprites(obj->children, f);
	
	if(obj->next)
		RemoveObjectSprites(obj->next, f);
}

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
	{
		for(i=0; i<obj->numSprites; i++)
		{
			if(obj->sprites[i].sprite)
			{
				SubSprite(obj->sprites[i].sprite);
				JallocFree((UBYTE**)&obj->sprites[i].sprite);
			}
		}
	}

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
	short i;
	Vtx *vtxa;
	Vtx *vtxb;
	Vtx *oldVtxa, *oldVtxb;
	int offset;

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
	int		i;
		
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

	objCont = actor->LODObjectController;
	if(objCont)
	{
		actor->LODObjectController = (OBJECT_CONTROLLER *)JallocAlloc(sizeof(OBJECT_CONTROLLER), YES, "UniqObjC");
		memcpy(actor->LODObjectController, objCont, sizeof(OBJECT_CONTROLLER));
		actor->LODObjectController->object = MakeUniqueObject(actor->LODObjectController->object);
	}
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
	int		i;
		
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


void Orientate(QUATERNION *me, VECTOR *fd, VECTOR *mfd, VECTOR *up)
{
	VECTOR dirn;
	QUATERNION rotn,q;
	float dp,m;
	
	CalculateQuatForPlane2( 0, me, up);
	RotateVectorByQuaternion( &dirn, mfd, me);
	dp = DotProduct( fd, &dirn );
	CrossProduct( (VECTOR *)&rotn, &dirn, fd );
	if(dp > -0.99)
	{
		m = Magnitude( (VECTOR *)&rotn );
		if(m > 0.0001)
		{
			ScaleVector( (VECTOR *)&rotn, 1/m );

			if (dp<0.99)
				rotn.w = acos(dp);
			else
				rotn.w = 0;
			
			GetQuaternionFromRotation( &q, &rotn );
			QuaternionMultiply( me, &q, me );
		}
	}
	else
	{
		vertQ.w = PI;
		GetQuaternionFromRotation(&q,&vertQ);
		QuaternionMultiply(me,me,&q);
	}
}


void SitAndFace(ACTOR2 *me, GAMETILE *tile, long fFacing)
{
	VECTOR fwdVec = { 0,0,1 };
	VECTOR dirn2;
	QUATERNION rotn,q;
	float frogMatrix[4][4];
	float frogMatrix2[4][4];
	float dp,m;
	
	CalculateQuatForPlane2(0,&me->actor->qRot,&tile->normal);
	RotateVectorByQuaternion(&dirn2,&fwdVec,&me->actor->qRot);
	dp = DotProduct(&tile->dirVector[fFacing],&dirn2);
	CrossProduct((VECTOR *)&rotn,&dirn2,&tile->dirVector[fFacing]);
	if(dp > -0.99)
	{
		m = Magnitude((VECTOR *)&rotn);
		if(m > 0.0001)
		{
			ScaleVector((VECTOR *)&rotn,1/m);
			rotn.w = acos(dp);
			GetQuaternionFromRotation(&q,&rotn);
			QuaternionMultiply(&me->actor->qRot,&q,&me->actor->qRot);
		}
	}
	else
	{
		vertQ.w = PI;
		GetQuaternionFromRotation(&q,&vertQ);
		QuaternionMultiply(&me->actor->qRot,&me->actor->qRot,&q);
	}
}






/*

void RemoveUniqueActor(ACTOR *actor,int type)
{
	if((actor->objectController) && (actor->objectController->object))
	{
		if((type < 0) || ((type >= 0) && (type < CAMEO_ACTOR)))
		{
			SubActor(actor);
			FreeObjectSprites(actor->objectController->object);

			if(actor->objectController->drawList)
			{
				JallocFree((UBYTE **)&actor->objectController->Vtx[0]);
				JallocFree((UBYTE **)&actor->objectController->drawList);
			}


			RemoveUniqueObject(actor->objectController->object);
			JallocFree((UBYTE **)&actor->objectController);
		
			if((actor->LODObjectController) && (actor->LODObjectController->object))
			{
				RemoveUniqueObject(actor->LODObjectController->object);
				JallocFree((UBYTE **)&actor->LODObjectController);
			}
		}
	}
}
*/
