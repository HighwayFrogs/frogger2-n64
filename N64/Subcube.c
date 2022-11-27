/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.
	
	  
	File		: Subcube.c
	Programmer	: Andrew Eder
	Date		: 11/24/98
	
----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI_2

#include <ultra64.h>

#include "incs.h"


#define LODDist (700 * 700)

short transform = 1;

VECTOR pointVec = {0,0,1};
float hedSpeed = 0.2;

Vtx	objectsVtx[2][2000];
Vtx *vtxPtr;

char changeRenderMode	= 1;

float unitMatrix[4][4] = {0, 0, 0, 0,
				  0, 0, 0, 0,
				  0, 0, 0, 0,
				  0, 0, 0, 0};

char	*tempObjectPtr, *tempObjectPtr2;

VECTOR	*actorScale;
VECTOR	parentScaleStack[10];
int		parentScaleStackLevel = 0;

int		matrixStackCount = 0;
short	xluFact = 0x8f;
short	xluOverride = 0;

ACTOR	*currentDrawActor;
Mtx		*staticObjectMtx;
char	calcMtx = FALSE;

char	staticObj;

float rotmat[4][4], scalemat[4][4], transmat[4][4];
OBJECT_CONTROLLER *currentObjectController;

char lookatNum = 0;
char hiliteNum = 0;

void SetRendermodeForEnviroment(void);

ACTOR2 *hat[MAX_FROGS];


RENDER_MODE renderMode = 
{
	0,	//	UBYTE	transparentSurf;
	0,	//	UBYTE	aiSurf;         
	0,	//	UBYTE	xluSurf;        
	2,	//	UBYTE	useAAMode;      
	1,	//	UBYTE	useZMode;       
	1,	//	UBYTE	useTextureMode; 
	0,	//	UBYTE	mirrorTextures; 
	0,	//	UBYTE	phongMode;      
	0,	//	UBYTE	faceColour;     
	0,	//	UBYTE	objectColour;   
	0,	//	UBYTE	actorColour;    
	0,	//	UBYTE	pixelOut;       
	0,	//	UBYTE	phongAllowed;   
};


/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void InitActorStructures(ACTOR *tempActor, int initFlags)
{
	int j;
	char temp[24];

	tempActor->scale.v[X] = tempActor->scale.v[Y] = tempActor->scale.v[Z] = 1;

	if(initFlags & INIT_ANIMATION)
	{
		if((tempActor->objectController) && (tempActor->objectController->animation))
		{
			tempActor->animation = (ACTOR_ANIMATION *)JallocAlloc(sizeof(ACTOR_ANIMATION), YES, "Act_Anim");
			InitActorAnim(tempActor);
		}
	}

	if(initFlags & INIT_SHADOW)
	{
		tempActor->shadow = (ACTOR_SHADOW *)JallocAlloc(sizeof(ACTOR_SHADOW),YES,"ACTSHAD");
		memcpy(tempActor->shadow->vert,shadowVtx,sizeof(Vtx) * 4);
		tempActor->shadow->alpha = tempActor->shadow->alphaAim = 255;
		tempActor->shadow->alphaAimSpeed = 10;
	}

	tempActor->status = ACTIVE;
	tempActor->xluOverride = 100;
}

/*	--------------------------------------------------------------------------------
	Function 	: InitActor()
	Purpose 	: inits structures etc for an actor with NO COLLISION DATA
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void InitActor(ACTOR *tempActor, char *name, float x, float y, float z, int initFlags)
{
	FindObject(&tempActor->objectController,UpdateCRC(name),name,YES);
	if(!tempActor->objectController)
		return;

	InitActorStructures(tempActor,initFlags);

	tempActor->pos.v[X] = x;
	tempActor->pos.v[Y] = y;
	tempActor->pos.v[Z] = z;
	tempActor->scale.v[X] = tempActor->scale.v[Y] = tempActor->scale.v[Z] = 1;

//init actors rotation
	ZeroQuaternion(&tempActor->qRot);
}


/*	--------------------------------------------------------------------------------
	Function		: SubActor
	Purpose			: removes actor from list
	Parameters		: ACTOR2 *
	Returns			: void
	Info			: 
*/
void SubActor(ACTOR2 *actor)
{
	if (!actor) return;

	if (actor->next)
		actor->next->prev = actor->prev;

	if (actor->prev)
		actor->prev->next = actor->next;
	else
		actList = actor->next;

	if(actor->actor)
	{
		if(actor->actor->animation)
		{
			// free any animation associated with ACTOR type
			JallocFree((UBYTE**)&actor->actor->animation);
		}

		if(actor->actor->shadow)
		{
			// free any shadow associated with ACTOR type
			JallocFree((UBYTE**)&actor->actor->shadow);
		}

		if((actor->actor->objectController) && (actor->actor->objectController->object))
		{
			// free any object sprites for this actor
			FreeObjectSprites(actor->actor->objectController->object);
		}

		// free associated ACTOR type
		JallocFree((UBYTE**)&actor->actor);
	}

	JallocFree((UBYTE**)&actor);
}


long TestDistanceFromFrog = 0;

extern float meMod;
float adjF = 0.3;
/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void XformActor(ACTOR *actor)
{
	OBJECT_CONTROLLER *objectC = actor->objectController;
	float animTime;

	if((actor->xluOverride == 0) || (objectC->object->xlu == 0))
	{
		actor->visible = 0;
		return;
	}

	if((actor->matrix) || (actor->objectController->objectSize == 1))
	{
		actor->visible = TRUE;
		return;
	}
	
	UpdateAnimations(actor);

	actor->visible = TRUE;

	xluOverride = actor->xluOverride;

//	numActorsXformed++;
	staticObjectMtx = actor->matrix;
	currentDrawActor = actor;
	currentObjectController = objectC;
	actorScale = &actor->scale;

	if((actor->animation) && (actor->animation->anims))
		animTime = actor->animation->animTime;
	else
		animTime = 0;

	parentScaleStackLevel = 0;
	parentScaleStack[parentScaleStackLevel].v[X] = parentScaleStack[parentScaleStackLevel].v[Y] = parentScaleStack[parentScaleStackLevel].v[Z] = 1;

	if(objectC->drawList)	//if object is skinned, transform mesh before aplying object TM
	{
		if(objectC->object)
		{
			if((actor->LODObjectController) && (actor->distanceFromCamera - actor->maxRadius > (LODDist)))
				TransformSkinnedObject(actor->LODObjectController->object, animTime);
			else
				TransformSkinnedObject(objectC->object, animTime);
		}
	}

	guTranslateF(transmat, actor->pos.v[X], actor->pos.v[Y], actor->pos.v[Z]);
	PushMatrix(transmat);

	QuaternionToMatrix(&actor->qRot,(MATRIX *)rotmat);
	PushMatrix(rotmat);

	if(objectC->drawList == NULL)
	{
		if(objectC->object)
		{
			if((actor->LODObjectController) && (actor->distanceFromCamera - actor->maxRadius > (LODDist)))
				TransformObject(actor->LODObjectController->object, animTime);
			else
				TransformObject(objectC->object, animTime);
		}
	}
	else
	{
		//skinned objects need to store the TM, including the scale which can't be done at vertex xform stage due to interer inaccuracies
		guScaleF(scalemat,actor->scale.v[X],actor->scale.v[Y],actor->scale.v[Z]);
		PushMatrix(scalemat);
		guMtxF2L(matrixStack.stack[matrixStack.stackPosition], &objectC->object->objMatrix);
		PopMatrix();
	}

  	parentScaleStackLevel = 0;
	parentScaleStack[parentScaleStackLevel].v[X] = parentScaleStack[parentScaleStackLevel].v[Y] = parentScaleStack[parentScaleStackLevel].v[Z] = 1;
	
	matrixStack.stackPosition-=2;
//	PopMatrix();
//	PopMatrix();
}


/*---------------------------------------------------------------------------------------------
	Function	: DrawObject
	Parameters	: (OBJECT *obj)
	Returns		: void 
*/

void DrawObject(OBJECT *obj, Gfx *drawList, int skinned)
{
//	if(obj->textureAnim)
//		AnimateTexture(obj->textureAnim,obj->drawList);

	if(staticObjectMtx == NULL)
	{
		SetMatrix((float *)&dynamicp->modeling4[objectMatrix], (float *)&obj->objMatrix);

	    gSPMatrix(glistp++, 
		OS_K0_TO_PHYSICAL(&(dynamicp->modeling4[objectMatrix++])),
		G_MTX_MODELVIEW|G_MTX_MUL|G_MTX_PUSH);
	}
	else
	{
		SetMatrix((float *)&dynamicp->modeling4[objectMatrix], (float *)staticObjectMtx);

	    gSPMatrix(glistp++, 
		OS_K0_TO_PHYSICAL(&(dynamicp->modeling4[objectMatrix++])),
		G_MTX_MODELVIEW|G_MTX_MUL|G_MTX_PUSH);
	}

	SetupRenderModeForObject(obj);

	if(drawList)
	{
		gSPDisplayList(glistp++,drawList);
	}
	else if(obj->mesh->numFaces)
	{
		if(obj->flags & OBJECT_FLAGS_GOURAUD_SHADED)
		{
			if(!(obj->flags & OBJECT_FLAGS_PRELIT))
			{
				gSPSetGeometryMode(glistp++, G_LIGHTING);
			}
			else
			{
				gSPClearGeometryMode(glistp++, G_LIGHTING);
			}
			if(obj->flags & OBJECT_FLAGS_PHONG)
				WriteObjectDisplayListGouraudPhong(obj);
			else
				WriteObjectDisplayListGouraud(obj);
		}
		else
		{
			if(!(obj->flags & OBJECT_FLAGS_PRELIT))
			{
				gSPSetGeometryMode(glistp++, G_LIGHTING);
			}
			else
			{
				gSPClearGeometryMode(glistp++, G_LIGHTING);
			}
			WriteObjectDisplayListFlat(obj);
		}
	}

	gSPPopMatrix(glistp++, G_MTX_MODELVIEW);

	if(skinned)
		return;

	if(obj->children)
		DrawObject(obj->children, obj->children->drawList, skinned);

	if(obj->next)
		DrawObject(obj->next, obj->next->drawList, skinned);
}


void DrawActor(ACTOR *actor)
{
	OBJECT_CONTROLLER *objectC = actor->objectController;
	int col;
	float animTime;

	if(actor->visible == FALSE)
		return;

	staticObjectMtx = actor->matrix;

	xluOverride = actor->xluOverride;
/*
	if((actor->stats) && (actor->stats->inShadow))
	{
		renderMode.actorColour = 1;
		col = 255-actor->stats->inShadow;
		ProcessBackgroundLightsWithCols(1,col,col,col);
	}
	else if(renderMode.actorColour)
	{
		renderMode.actorColour = 0;
		ProcessBackgroundLightsWithCols(0,0,0,0);
	}
*/
	currentDrawActor = actor;
	actorScale = &actor->scale;

	if((actor->animation) && (actor->animation->anims))
		animTime = actor->animation->animTime;
	else
		animTime = 0;

	//quick draw
	if(objectC->objectSize == 1)
	{
		SetupRenderModeForObject(objectC->object);
		gSPDisplayList(glistp++, objectC->object->drawList);
	}
	else
	{
		if(objectC->drawList)	//if object is skinned, draw mesh now
 		{
			gSPSegment(glistp++, 1, objectC->vtx[objectC->vtxBuf]);
			DrawObject(objectC->object, objectC->drawList, TRUE);
		}
		else
		{
			DrawObject(objectC->object, objectC->object->drawList, FALSE);
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: DeformTextureCoords
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void DeformTextureCoords(OBJECT *obj)
{
/*	int i,j,plus;
	USHORT2DVECTOR *texCoord, *oTexCoord;
	VECTOR	*vectP = obj->mesh->vertices;
	short	voffset = 0;
		
	texCoord = obj->mesh->faceTC;
	oTexCoord = obj->mesh->originalFaceTC;

	for(i = 0; i < obj->mesh->numFaces; i++)
	{
		for(j = 0;j < 3;j++)
		{
			voffset = obj->mesh->faceIndex[i].v[j];
			vectP = obj->mesh->vertices + voffset;
			plus = SineWave(10, (short)(vectP->v[X]+vectP->v[Y]+vectP->v[Z]),frameCount)*16;
			texCoord->v[X] += plus; 
			plus = SineWave(10, (short)(vectP->v[X]+vectP->v[Y]+vectP->v[Z])/2,frameCount)*16;
			texCoord->v[Y] += plus;
			texCoord++;
			oTexCoord++;
		}
	}*/
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
QUATERNION	tempQuat,morphFromQuat,morphToQuat,morphResultQuat;
void TransformObject(OBJECT *obj, float time)
{
	QUATERNION quat1,quat2;
	VECTOR translation,scale;
	VECTOR	morphFrom,morphTo;
	SPRITE	*sprite;
	float	tempFloat,interp;
	short i, j;
	short	fromKey, toKey;
	short	xluVal;

//handle position keyframes
	if((obj->numMoveKeys > 1) && (currentDrawActor->animation))
	{
		if(currentDrawActor->animation->numMorphFrames)
		{
			// Find from vector
			FindToFromVKeys(obj->moveKeys,&fromKey,&toKey,&interp,currentDrawActor->animation->morphFrom,obj->numMoveKeys);
			LinearInterp(&morphFrom,&obj->moveKeys[fromKey].vect,&obj->moveKeys[toKey].vect,interp);

			// Find to vector
			FindToFromVKeys(obj->moveKeys,&fromKey,&toKey,&interp,currentDrawActor->animation->morphTo,obj->numMoveKeys);
			LinearInterp(&morphTo,&obj->moveKeys[fromKey].vect,&obj->moveKeys[toKey].vect,interp);

			// Interp between from and to vectors
			tempFloat = (float)currentDrawActor->animation->currentMorphFrame/(float)(Fabs(currentDrawActor->animation->numMorphFrames));
			LinearInterp(&translation,&morphFrom,&morphTo,tempFloat);
		}
		else
		{
			FindToFromVKeys(obj->moveKeys,&fromKey,&toKey,&interp,time,obj->numMoveKeys);
			LinearInterp(&translation,&obj->moveKeys[fromKey].vect,&obj->moveKeys[toKey].vect,interp);
		}
	}
	else
	{
		SetVector(&translation,&obj->moveKeys[0].vect);
	}
	if((obj->numScaleKeys > 1) && (currentDrawActor->animation))
	{
		if(currentDrawActor->animation->numMorphFrames)
		{
			// Find from vector
			FindToFromVKeys(obj->scaleKeys,&fromKey,&toKey,&interp,currentDrawActor->animation->morphFrom,obj->numScaleKeys);
			LinearInterp(&morphFrom,&obj->scaleKeys[fromKey].vect,&obj->scaleKeys[toKey].vect,interp);

			// Find to vector
			FindToFromVKeys(obj->scaleKeys,&fromKey,&toKey,&interp,currentDrawActor->animation->morphTo,obj->numScaleKeys);
			LinearInterp(&morphTo,&obj->scaleKeys[fromKey].vect,&obj->scaleKeys[toKey].vect,interp);

			// Slerp between from and to vectors
			tempFloat = (float)currentDrawActor->animation->currentMorphFrame/(float)(Fabs(currentDrawActor->animation->numMorphFrames));
			LinearInterp(&scale,&morphFrom,&morphTo,tempFloat);
		}
		else
		{
			FindToFromVKeys(obj->scaleKeys,&fromKey,&toKey,&interp,time,obj->numScaleKeys);
			LinearInterp(&scale,&obj->scaleKeys[fromKey].vect,&obj->scaleKeys[toKey].vect,interp);
		}
	}
	else
	{
		SetVector(&scale,&obj->scaleKeys[0].vect);
	}
	// handle rotation keyframes
	if((obj->numRotateKeys > 1) && (currentDrawActor->animation))
	{
		if(currentDrawActor->animation->numMorphFrames)
		{
/*
			// Find from vector
			FindToFromQKeys(obj->rotateKeys,&fromKey,&toKey,&interp,currentDrawActor->animation->morphFrom,obj->numRotateKeys);
			QuatSlerp(&obj->rotateKeys[fromKey].quat, &obj->rotateKeys[toKey].quat, interp, &morphFromQuat);

			// Find to vector
			FindToFromQKeys(obj->rotateKeys,&fromKey,&toKey,&interp,currentDrawActor->animation->morphTo,obj->numRotateKeys);
			QuatSlerp(&obj->rotateKeys[fromKey].quat, &obj->rotateKeys[toKey].quat, interp, &morphToQuat);

			// Slerp between from and to vectors
			tempFloat = (float)currentDrawActor->animation->currentMorphFrame/(float)(Fabs(currentDrawActor->animation->numMorphFrames));
			QuatSlerp(&morphFromQuat, &morphToQuat, tempFloat, &morphResultQuat);

			QuaternionToMatrix(&morphResultQuat, (MATRIX *)rotmat);
*/

			// USE NEW QUATERNIONSHORT DATA TYPE

			// Find from vector
			FindToFromKeysSHORT(obj->rotateKeys,&fromKey,&toKey,&interp,currentDrawActor->animation->morphFrom,obj->numRotateKeys);
			ConvertQuatShortToQuat(&obj->rotateKeys[fromKey].quat,&quat1);
			ConvertQuatShortToQuat(&obj->rotateKeys[toKey].quat,&quat2);
			QuatSlerp(&quat1,&quat2,interp,&morphFromQuat);

			// Find to vector
			FindToFromKeysSHORT(obj->rotateKeys,&fromKey,&toKey,&interp,currentDrawActor->animation->morphTo,obj->numRotateKeys);
			ConvertQuatShortToQuat(&obj->rotateKeys[fromKey].quat,&quat1);
			ConvertQuatShortToQuat(&obj->rotateKeys[toKey].quat,&quat2);
			QuatSlerp(&quat1,&quat2,interp,&morphToQuat);

			// Slerp between from and to vectors
			tempFloat = (float)currentDrawActor->animation->currentMorphFrame/(float)(Fabs(currentDrawActor->animation->numMorphFrames));
			QuatSlerp(&morphFromQuat,&morphToQuat,tempFloat,&morphResultQuat);

			// convert back to rotation matrix
			QuaternionToMatrix(&morphResultQuat,(MATRIX *)rotmat);
		}
		else
		{
/*
			FindToFromQKeys(obj->rotateKeys,&fromKey,&toKey,&interp,time,obj->numRotateKeys);
			QuatSlerp(&obj->rotateKeys[fromKey].quat, &obj->rotateKeys[toKey].quat, interp, &tempQuat);

			// convert back to rotation matrix
			QuaternionToMatrix(&tempQuat, (MATRIX *)rotmat);
*/

			// USE NEW QUATERNIONSHORT DATA TYPE

			FindToFromKeysSHORT(obj->rotateKeys,&fromKey,&toKey,&interp,time,obj->numRotateKeys);
			ConvertQuatShortToQuat(&obj->rotateKeys[fromKey].quat,&quat1);
			ConvertQuatShortToQuat(&obj->rotateKeys[toKey].quat,&quat2);
			QuatSlerp(&quat1,&quat2,interp,&tempQuat);

			// convert back to rotation matrix
			QuaternionToMatrix(&tempQuat,(MATRIX *)rotmat);
		}
	}
	else
	{
//		QuaternionToMatrix(&obj->rotateKeys[0].quat, (MATRIX *)rotmat);

		// USE NEW QUATERNIONSHORT DATA TYPE
		ConvertQuatShortToQuat(&obj->rotateKeys[0].quat,&quat1);
		QuaternionToMatrix(&quat1,(MATRIX *)rotmat);
	}

	rotmat[3][0] = translation.v[X] * actorScale->v[X] * parentScaleStack[parentScaleStackLevel].v[X];
	rotmat[3][1] = translation.v[Y] * actorScale->v[Y] * parentScaleStack[parentScaleStackLevel].v[Y];
	rotmat[3][2] = translation.v[Z] * actorScale->v[Z] * parentScaleStack[parentScaleStackLevel].v[Z];

	PushMatrix(rotmat);

	guScaleF(scalemat, scale.v[X] * actorScale->v[X], scale.v[Y] * actorScale->v[Y], scale.v[Z] * actorScale->v[Z]);

	PushMatrix(scalemat);


	//if the object has a collision sphere
	if(obj->collSphere)
	{
		SetVector(&obj->collSphere->oldOffset, &obj->collSphere->offset);
		guMtxXFMF(matrixStack.stack[matrixStack.stackPosition], 0, 0, 0,&obj->collSphere->offset.v[X], &obj->collSphere->offset.v[Y], &obj->collSphere->offset.v[Z]);
	}

	//maintain posision of all objects sprites        
//	if(obj->numSprites > 0)
	{
		for(i = 0; i < obj->numSprites; i++)
		{
			sprite = obj->sprites[i].sprite;
			if(sprite)
			{
				obj->sprites[i].sprite->scaleX = (float)obj->sprites[i].sx * actorScale->v[X] * scale.v[X];
				obj->sprites[i].sprite->scaleY = (float)obj->sprites[i].sy * actorScale->v[Y] * scale.v[Y];
				xluVal = ((int)(obj->xlu * xluOverride))/100;
				if(xluOverride <= 10)
					xluVal = 0;

				obj->sprites[i].sprite->a = Min(255,xluVal);
				
				if(obj->sprites[i].sprite->a >= 255)
					obj->sprites[i].sprite->flags &= -1 - SPRITE_TRANSLUCENT;
				else
					obj->sprites[i].sprite->flags |= SPRITE_TRANSLUCENT;
				guMtxXFMF(matrixStack.stack[matrixStack.stackPosition], obj->sprites[i].x, obj->sprites[i].y, -obj->sprites[i].z,
												&sprite->pos.v[X], &sprite->pos.v[Y], &sprite->pos.v[Z]);
				if(renderMode.pixelOut)
				{
					sprite->flags |= SPRITE_FLAGS_PIXEL_OUT;
					sprite->a = (sprite->a * obj->pixelOutAlpha)/255;
				}
				else
					sprite->flags &= -1 - SPRITE_FLAGS_PIXEL_OUT;
			}
		}
	}
		
  	guMtxF2L(matrixStack.stack[matrixStack.stackPosition], &obj->objMatrix);

	PopMatrix();


//transform children
	if(obj->children)
	{
		parentScaleStackLevel++;
		SetVector(&parentScaleStack[parentScaleStackLevel],&scale);
		TransformObject(obj->children, time);
		parentScaleStackLevel--;
	}

	PopMatrix();

	if(obj->next)
	{
		TransformObject(obj->next, time);
	}
}



/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void TransformSkinnedObject(OBJECT *obj, float time)
{
	QUATERNION quat1,quat2;
	VECTOR translation,scale, rotation;
	VECTOR	morphFrom,morphTo;
	SPRITE	*sprite;
	float	tempFloat,interp;
	short i, j;
	short	fromKey, toKey;
	short	xluVal;
	char tmp[6],wasHed;
	Vtx		*vtx = currentObjectController->vtx[currentObjectController->vtxBuf];

//handle position keyframes
	if((obj->numMoveKeys > 1) && (currentDrawActor->animation))
	{
		if(currentDrawActor->animation->numMorphFrames)
		{
			// Find from vector
			FindToFromVKeys(obj->moveKeys,&fromKey,&toKey,&interp,currentDrawActor->animation->morphFrom,obj->numMoveKeys);
			LinearInterp(&morphFrom,&obj->moveKeys[fromKey].vect,&obj->moveKeys[toKey].vect,interp);

			// Find to vector
			FindToFromVKeys(obj->moveKeys,&fromKey,&toKey,&interp,currentDrawActor->animation->morphTo,obj->numMoveKeys);
			LinearInterp(&morphTo,&obj->moveKeys[fromKey].vect,&obj->moveKeys[toKey].vect,interp);

			// Interp between from and to vectors
			tempFloat = (float)currentDrawActor->animation->currentMorphFrame/(float)(Fabs(currentDrawActor->animation->numMorphFrames));
			LinearInterp(&translation,&morphFrom,&morphTo,tempFloat);
		}
		else
		{
			FindToFromVKeys(obj->moveKeys,&fromKey,&toKey,&interp,time,obj->numMoveKeys);
			LinearInterp(&translation,&obj->moveKeys[fromKey].vect,&obj->moveKeys[toKey].vect,interp);
		}
	}
	else
	{
		SetVector(&translation,&obj->moveKeys[0].vect);
	}
	if((obj->numScaleKeys > 1) && (currentDrawActor->animation))
	{
		if(currentDrawActor->animation->numMorphFrames)
		{
			// Find from vector
			FindToFromVKeys(obj->scaleKeys,&fromKey,&toKey,&interp,currentDrawActor->animation->morphFrom,obj->numScaleKeys);
			LinearInterp(&morphFrom,&obj->scaleKeys[fromKey].vect,&obj->scaleKeys[toKey].vect,interp);

			// Find to vector
			FindToFromVKeys(obj->scaleKeys,&fromKey,&toKey,&interp,currentDrawActor->animation->morphTo,obj->numScaleKeys);
			LinearInterp(&morphTo,&obj->scaleKeys[fromKey].vect,&obj->scaleKeys[toKey].vect,interp);

			// Slerp between from and to vectors
			tempFloat = (float)currentDrawActor->animation->currentMorphFrame/(float)(Fabs(currentDrawActor->animation->numMorphFrames));
			LinearInterp(&scale,&morphFrom,&morphTo,tempFloat);
		}
		else
		{
			FindToFromVKeys(obj->scaleKeys,&fromKey,&toKey,&interp,time,obj->numScaleKeys);
			LinearInterp(&scale,&obj->scaleKeys[fromKey].vect,&obj->scaleKeys[toKey].vect,interp);
		}
	}
	else
	{
		SetVector(&scale,&obj->scaleKeys[0].vect);
	}

	// handle rotation keyframes
	if((obj->numRotateKeys > 1) && (currentDrawActor->animation))
	{
		if(currentDrawActor->animation->numMorphFrames)
		{
/*
			// Find from vector
			FindToFromQKeys(obj->rotateKeys,&fromKey,&toKey,&interp,currentDrawActor->animation->morphFrom,obj->numRotateKeys);
			QuatSlerp(&obj->rotateKeys[fromKey].quat, &obj->rotateKeys[toKey].quat, interp, &morphFromQuat);

			// Find to vector
			FindToFromQKeys(obj->rotateKeys,&fromKey,&toKey,&interp,currentDrawActor->animation->morphTo,obj->numRotateKeys);
			QuatSlerp(&obj->rotateKeys[fromKey].quat, &obj->rotateKeys[toKey].quat, interp, &morphToQuat);

			// Slerp between from and to vectors
			tempFloat = (float)currentDrawActor->animation->currentMorphFrame/(float)(Fabs(currentDrawActor->animation->numMorphFrames));
			QuatSlerp(&morphFromQuat, &morphToQuat, tempFloat, &morphResultQuat);

			QuaternionToMatrix(&morphResultQuat, (MATRIX *)rotmat);
*/

			// USE NEW QUATERNIONSHORT DATA TYPE

			// Find from vector
			FindToFromKeysSHORT(obj->rotateKeys,&fromKey,&toKey,&interp,currentDrawActor->animation->morphFrom,obj->numRotateKeys);
			ConvertQuatShortToQuat(&obj->rotateKeys[fromKey].quat,&quat1);
			ConvertQuatShortToQuat(&obj->rotateKeys[toKey].quat,&quat2);
			QuatSlerp(&quat1,&quat2,interp,&morphFromQuat);

			// Find to vector
			FindToFromKeysSHORT(obj->rotateKeys,&fromKey,&toKey,&interp,currentDrawActor->animation->morphTo,obj->numRotateKeys);
			ConvertQuatShortToQuat(&obj->rotateKeys[fromKey].quat,&quat1);
			ConvertQuatShortToQuat(&obj->rotateKeys[toKey].quat,&quat2);
			QuatSlerp(&quat1,&quat2,interp,&morphToQuat);

			// Slerp between from and to vectors
			tempFloat = (float)currentDrawActor->animation->currentMorphFrame/(float)(Fabs(currentDrawActor->animation->numMorphFrames));
			QuatSlerp(&morphFromQuat,&morphToQuat,tempFloat,&morphResultQuat);

			// convert back to rotation matrix
			QuaternionToMatrix(&morphResultQuat,(MATRIX *)rotmat);
		}
		else
		{
/*
			FindToFromQKeys(obj->rotateKeys,&fromKey,&toKey,&interp,time,obj->numRotateKeys);
			QuatSlerp(&obj->rotateKeys[fromKey].quat, &obj->rotateKeys[toKey].quat, interp, &tempQuat);

			// convert back to rotation matrix
			QuaternionToMatrix(&tempQuat, (MATRIX *)rotmat);
*/

			// USE NEW QUATERNIONSHORT DATA TYPE

			FindToFromKeysSHORT(obj->rotateKeys,&fromKey,&toKey,&interp,time,obj->numRotateKeys);
			ConvertQuatShortToQuat(&obj->rotateKeys[fromKey].quat,&quat1);
			ConvertQuatShortToQuat(&obj->rotateKeys[toKey].quat,&quat2);
			QuatSlerp(&quat1,&quat2,interp,&tempQuat);

			// convert back to rotation matrix
			QuaternionToMatrix(&tempQuat,(MATRIX *)rotmat);
		}
	}
	else
	{
//		QuaternionToMatrix(&obj->rotateKeys[0].quat, (MATRIX *)rotmat);

		// USE NEW QUATERNIONSHORT DATA TYPE
		ConvertQuatShortToQuat(&obj->rotateKeys[0].quat,&quat1);
		QuaternionToMatrix(&quat1,(MATRIX *)rotmat);
	}

	//transform the objects vertex normals ONLY if the object is lit.
	if((obj->flags & OBJECT_FLAGS_PRELIT) == 0)
	{
		PushRMatrix(rotmat);	//uses special matrix stack which only records the rotation
		
		for(i = 0; i < obj->numVerts; i++)
		{
			guMtxXFMF(rMatrixStack.stack[rMatrixStack.stackPosition], obj->originalVerts[i].n[X], obj->originalVerts[i].n[Y], obj->originalVerts[i].n[Z],
											&rotation.v[X], &rotation.v[Y], &rotation.v[Z]);

			for(j = 0; j < obj->effectedVerts[i].numVerts; j++)
			{
				vtx[obj->effectedVerts[i].verts[j]].n.n[X] = (s8)rotation.v[X];
				vtx[obj->effectedVerts[i].verts[j]].n.n[Y] = (s8)rotation.v[Y];
				vtx[obj->effectedVerts[i].verts[j]].n.n[Z] = (s8)rotation.v[Z];
			}
		}
	}

	// check for frog head....
	wasHed = 0;
	for(i=0; i<5; i++)
		tmp[i] = obj->name[i];
	tmp[5] = '\0';

	// ...and move it if necessary
	if((gameState.mode == INGAME_MODE || runningIntro) && (!gstrcmp(tmp,"fghed\0")))
	{
		float rotmat2[4][4];
		QUATERNION quat, rot = {0,1,0,0};
		VECTOR actVec;

		wasHed = 1;
		if(player[0].canJump)
		{
			if (pointOfInterest)
			{
				SubVector (&actVec,pointOfInterest,&(frog[0]->actor->pos));
			}
			else
			{
				SetVector (&actVec,&(currTile[0]->dirVector[frogFacing[0]]));
			}

			MakeUnit (&actVec);

			pointVec.v[X] += (actVec.v[X] - pointVec.v[X]) * hedSpeed;
			pointVec.v[Y] += (actVec.v[Y] - pointVec.v[Y]) * hedSpeed;
			pointVec.v[Z] += (actVec.v[Z] - pointVec.v[Z]) * hedSpeed;
		
			MakeUnit (&pointVec);

			// could cause problems if point pass through frog... should never happen
	
			CrossProduct( (VECTOR *)&rot, &(currTile[0]->dirVector[frogFacing[0]]),&pointVec);
			rot.w = DotProduct(&pointVec,&(currTile[0]->dirVector[frogFacing[0]]));
			if (rot.w>1)
				rot.w = 0;
			else
				rot.w = acos(rot.w);

			GetQuaternionFromRotation (&quat,&rot);
			QuaternionToMatrix(&quat, (MATRIX*)rotmat2);
			guMtxCatF(rotmat,rotmat2,rotmat);
		}
		else
		{
			SetVector(&pointVec,&(currTile[0]->dirVector[frogFacing[0]]));
		}
	}

	rotmat[3][0] = translation.v[X] * parentScaleStack[parentScaleStackLevel].v[X];
	rotmat[3][1] = translation.v[Y] * parentScaleStack[parentScaleStackLevel].v[Y];
	rotmat[3][2] = translation.v[Z] * parentScaleStack[parentScaleStackLevel].v[Z];

	PushMatrix(rotmat);

	guScaleF(scalemat, scale.v[X], scale.v[Y], scale.v[Z]);

	PushMatrix(scalemat);

	//if the object has a collision sphere
	if(obj->collSphere)
	{
		SetVector(&obj->collSphere->oldOffset, &obj->collSphere->offset);
		guMtxXFMF(matrixStack.stack[matrixStack.stackPosition], 0, 0, 0,&obj->collSphere->offset.v[X], &obj->collSphere->offset.v[Y], &obj->collSphere->offset.v[Z]);
	}

	//maintain posision of all objects sprites        
//	if(obj->numSprites > 0)
	{
		for(i = 0; i < obj->numSprites; i++)
		{
			sprite = obj->sprites[i].sprite;
			if(sprite)
			{
				obj->sprites[i].sprite->scaleX = (float)obj->sprites[i].sx * actorScale->v[X] * scale.v[X];
				obj->sprites[i].sprite->scaleY = (float)obj->sprites[i].sy * actorScale->v[Y] * scale.v[Y];
				xluVal = ((int)(obj->xlu * xluOverride))/100;

				obj->sprites[i].sprite->a = Min(255,xluVal);
				if(obj->sprites[i].sprite->a >= 255)
					obj->sprites[i].sprite->flags &= -1 - SPRITE_TRANSLUCENT;
				else
					obj->sprites[i].sprite->flags |= SPRITE_TRANSLUCENT;

				guMtxXFMF(matrixStack.stack[matrixStack.stackPosition], obj->sprites[i].x, obj->sprites[i].y, -obj->sprites[i].z,
												&sprite->pos.v[X], &sprite->pos.v[Y], &sprite->pos.v[Z]);
				if(renderMode.pixelOut)
				{
					sprite->flags |= SPRITE_FLAGS_PIXEL_OUT;
					sprite->a = (sprite->a * obj->pixelOutAlpha)/255;
				}
				else
					sprite->flags &= -1 - SPRITE_FLAGS_PIXEL_OUT;
			}
		}
	}

	for(i = 0; i < obj->numVerts; i++)
	{
		//xform the vertex positions
		guMtxXFMF(matrixStack.stack[matrixStack.stackPosition], obj->originalVerts[i].v[X], obj->originalVerts[i].v[Y], obj->originalVerts[i].v[Z],
										&translation.v[X], &translation.v[Y], &translation.v[Z]);

		for(j = 0; j < obj->effectedVerts[i].numVerts; j++)
		{
			vtx[obj->effectedVerts[i].verts[j]].n.ob[X] = translation.v[X];
			vtx[obj->effectedVerts[i].verts[j]].n.ob[Y] = translation.v[Y];
			vtx[obj->effectedVerts[i].verts[j]].n.ob[Z] = translation.v[Z];
		}

	}

	PopMatrix();

//transform children
	if(obj->children)
	{
		parentScaleStackLevel++;
		SetVector(&parentScaleStack[parentScaleStackLevel],&scale);
		TransformSkinnedObject(obj->children, time);
		parentScaleStackLevel--;
	}

	PopMatrix();

	if((obj->flags & OBJECT_FLAGS_PRELIT) == 0)
	{
		PopRMatrix();
	}

	if(obj->next)
	{
		TransformSkinnedObject(obj->next, time);
	}
}





/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void FindToFromVKeys(VKEYFRAME *keys,short *from,short *to,float *interp,float time,int numKeys)
{
	int			i;
	int			low,high;
	short		fromKey = -1, toKey;
	float		tempFloat;

	low = 0;
	high = numKeys - 1;

	while(low <= high)
	{
		i = (low + high)>>1;
		if(keys[i].time == time)
		{
			if(i == numKeys - 1)
			{
				fromKey = -2;
			}
			else
			{
				fromKey = i;
				toKey = i + 1;
			}
			break;
		}
		else
		{
			if(keys[i].time > time)
			{
				high = i - 1;
			}
			else
			{
				low = i + 1;
			}
		}
	}

	if((fromKey == -1) && (high+1 < numKeys) && (keys[high + 1].time > time))
	{
		fromKey = high;
		toKey = high + 1;
	}

	if(fromKey >= 0)		//time is within valid keyframes
	{
		tempFloat = abs(keys[toKey].time - keys[fromKey].time);
		*interp = (time - (float)keys[fromKey].time) / tempFloat;
	}
	else					// must be between last and first keyframe
	{
		fromKey = numKeys-1;
		toKey = fromKey;
		*interp = 1;
	}
	*from = fromKey;
	*to = toKey;
}


/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void FindToFromQKeys(QKEYFRAME *keys,short *from,short *to,float *interp,float time,int numKeys)
{
	int			i;
	int			low,high;
	short		fromKey = -1, toKey;
	float		tempFloat;

	low = 0;
	high = numKeys - 1;

	while(low <= high)
	{
		i = (low + high)>>1;
		if(keys[i].time == time)
		{
			if(i == numKeys - 1)
			{
				fromKey = -2;
			}
			else
			{
				fromKey = i;
				toKey = i + 1;
			}
			break;
		}
		else
		{
			if(keys[i].time > time)
			{
				high = i - 1;
			}
			else
			{
				low = i + 1;
			}
		}
	}

	if((fromKey == -1) && (high+1 < numKeys) && (keys[high + 1].time > time))
	{
		fromKey = high;
		toKey = high + 1;
	}

	if(fromKey >= 0)		//time is within valid keyframes
	{
		tempFloat = abs(keys[toKey].time - keys[fromKey].time);
		*interp = (time - (float)keys[fromKey].time) / tempFloat;
	}
	else					// must be between last and first keyframe
	{
		fromKey = numKeys-1;
		toKey = fromKey;
		*interp = 1;
	}
	*from = fromKey;
	*to = toKey;
}




/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void SetupRenderModeForObject ( OBJECT *obj )
{
	VECTOR	tempVect;
	float	tempFloat;
	Mtx		nomatrix;
	int		r,g,b;
	int		phongX,phongY;

//////////////////////////////////////////////////
//check to see if render mode needs to be changed
	changeRenderMode = 0;


	if(!renderMode.aiSurf)
	{
		/*if(obj->flags & OBJECT_FLAGS_OBJECT_COLOUR)
		{
			renderMode.objectColour = 1;
			if((currentDrawActor->stats) && (currentDrawActor->stats->inShadow > 0))
			{
				r = (float)(obj->colour.r*(255-currentDrawActor->stats->inShadow))/255.0;
				g = (float)(obj->colour.g*(255-currentDrawActor->stats->inShadow))/255.0;
				b = (float)(obj->colour.b*(255-currentDrawActor->stats->inShadow))/255.0;
				ProcessBackgroundLightsWithCols(1,r,g,b);
			}
			else
				ProcessBackgroundLightsWithCols(1,obj->colour.r,obj->colour.g,obj->colour.b);
		}
		else if(renderMode.objectColour)
		{
			renderMode.objectColour = 0;
			if(renderMode.actorColour)
			{
				r = 255-currentDrawActor->stats->inShadow;
				ProcessBackgroundLightsWithCols(1,r,r,r);
			}
			else
				ProcessBackgroundLightsWithCols(0,0,0,0);
		}*/

		if(obj->flags & OBJECT_FLAGS_TRANSPARENT)
		{
			if(renderMode.transparentSurf == FALSE)
			{
				renderMode.transparentSurf = TRUE;
				changeRenderMode = TRUE;
			}
		}
		else
		{
			if(renderMode.transparentSurf == TRUE)
			{
				renderMode.transparentSurf = FALSE;
				changeRenderMode = TRUE;
			}
		}

		if(obj->flags & OBJECT_FLAGS_XLU)
		{
			if(renderMode.xluSurf == FALSE)
			{
				renderMode.xluSurf = TRUE;
				changeRenderMode = 1;
			}
			if(obj->drawList == 0)
				renderMode.xluSurf = TRUE;

			tempFloat = (float)xluOverride / 100;
			tempFloat *= (float)obj->xlu;
			if(tempFloat > 255)
				tempFloat = 255;
			if((int)tempFloat != xluFact)
				changeRenderMode = 1;
			xluFact = tempFloat;
		}
		else
		{
			if(renderMode.xluSurf == TRUE)
			{
				renderMode.xluSurf = FALSE;
				changeRenderMode = 1;
			}
		}

		if(obj->flags & OBJECT_FLAGS_FACE_COLOUR)
		{
			if((renderMode.faceColour == NO) && ((!(obj->flags & OBJECT_FLAGS_PHONG)) || (renderMode.phongAllowed == 0)))
			{
				renderMode.faceColour = YES;
				changeRenderMode = YES;
			}
		}
		else if(renderMode.faceColour)
		{
			renderMode.faceColour = NO;
			changeRenderMode = YES;
		}

		if((obj->pixelOutAlpha) && (obj->pixelOutAlpha < 250))
		{
	   		changeRenderMode = YES;
			renderMode.pixelOut = obj->pixelOutAlpha;
		    gDPSetAlphaCompare(glistp++, G_AC_DITHER);
			xluFact = obj->pixelOutAlpha;
		}
		else if(renderMode.pixelOut)
		{
			renderMode.pixelOut = 0;
			changeRenderMode = YES;
		    gDPSetAlphaCompare(glistp++, G_AC_NONE);
			if(!renderMode.xluSurf)
				xluFact = 255;
		}
		/*if((obj->phongTex) && (renderMode.phongAllowed))
		{
			if((lookatNum < MAX_LOOKATS) && (hiliteNum < MAX_HILITES))
			{
				phongX = obj->phongTex->sx;
				phongY = obj->phongTex->sy;

				if(changeRenderMode)
					SetRenderMode();
				changeRenderMode = 0;
				renderMode.useTextureMode = 1;//TEXTURE_MODE_NORMAL;
				renderMode.phongMode = ON;
				if(obj->flags & OBJECT_FLAGS_FACE_COLOUR)
				{
					/*if((currentDrawActor->objectType == BALL_TYPE_OBJECT_CRYSTAL_BALL) && (crystalBall))
					{
						SubVector(&tempVect,&crystalBall->pos,&camera->pos);
						MakeUnit(&tempVect);
						ScaleVector(&tempVect,-127);
						guLookAtHilite(&nomatrix, 
							&dynamicp->lookat[lookatNum], &dynamicp->hilite[hiliteNum],
							actualCamPos[draw_buffer].v[X],actualCamPos[draw_buffer].v[Y],actualCamPos[draw_buffer].v[Z],
							currentDrawActor->pos.v[X], currentDrawActor->pos.v[Y],currentDrawActor->pos.v[Z],
							camVect.v[X],camVect.v[Y],camVect.v[Z],
							0,1,0, 	// ignored 
							tempVect.v[X],tempVect.v[Y],tempVect.v[Z],
							phongX, phongY);	// texture width 
					}
					else
					{	*/
		/*				guLookAtHilite(&nomatrix, 
							&dynamicp->lookat[lookatNum], &dynamicp->hilite[hiliteNum],
							actualCamPos[draw_buffer].v[X],actualCamPos[draw_buffer].v[Y],actualCamPos[draw_buffer].v[Z],
							currentDrawActor->pos.v[X], currentDrawActor->pos.v[Y],currentDrawActor->pos.v[Z],
							camVect.v[X],camVect.v[Y],camVect.v[Z],
							0,1,0, 	// ignored 
							backgroundLightList.head.next->direction.v[X],backgroundLightList.head.next->direction.v[Y],backgroundLightList.head.next->direction.v[Z], 	// light 1 direction 
							phongX, phongY);	// texture width 
					//}
				}
				else
				{
					guLookAtHilite(&nomatrix, 
						&dynamicp->lookat[lookatNum], &dynamicp->hilite[hiliteNum],
						actualCamPos[draw_buffer].v[X],actualCamPos[draw_buffer].v[Y],actualCamPos[draw_buffer].v[Z],
						currentDrawActor->pos.v[X], currentDrawActor->pos.v[Y],currentDrawActor->pos.v[Z],
						camVect.v[X],camVect.v[Y],camVect.v[Z],
						backgroundLightList.head.next->direction.v[X],backgroundLightList.head.next->direction.v[Y],backgroundLightList.head.next->direction.v[Z], 	// light 1 direction 
						0,1,0, 	// light 2 direction 
						phongX, phongY);	// texture width 
				}

				gSPLookAt(glistp++, &dynamicp->lookat[lookatNum]);

				gDPPipeSync(glistp++);
				gSPSetGeometryMode(glistp++,G_ZBUFFER | G_SHADE | G_SHADING_SMOOTH | 
					G_CULL_BACK | G_LIGHTING | G_TEXTURE_GEN);

				gDPSetCycleType(glistp++,G_CYC_2CYCLE);

				gSPTexture(glistp++,(phongX<<6),(phongY<<6), 0, G_TX_RENDERTILE+1, G_ON);

				gDPSetTexturePersp(glistp++,G_TP_PERSP);
				gDPSetTextureFilter(glistp++,G_TF_BILERP);

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!Light 1 colour
				gDPSetPrimColor(glistp++,0, 0, 255, 255, 255, xluFact);


				if(obj->flags & OBJECT_FLAGS_FACE_COLOUR)
				{
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!Light 1 colour

					if(renderMode.actorColour)
					{
						gDPSetEnvColor(glistp++,envCol.r*(255-currentDrawActor->stats->inShadow)/255, envCol.g*(255-currentDrawActor->stats->inShadow)/255, envCol.b*(255-currentDrawActor->stats->inShadow)/255, xluFact);
					}
					else
					{
						gDPSetEnvColor(glistp++,envCol.r, envCol.g, envCol.b, xluFact);
					}

					gDPSetCombineMode(glistp++,G_CC_HILITEFACECOLOUR1, G_CC_HILITEFACECOLOUR2);
					if(obj->phongTex->pixSize == G_IM_SIZ_4b)
					{
						gDPSetTile(glistp++,obj->phongTex->format, obj->phongTex->pixSize, (((obj->phongTex->sx)+7)>>4),
							0, G_TX_RENDERTILE+2, 0, G_TX_WRAP | G_TX_NOMIRROR,
							obj->phongTex->TCScaleX, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, obj->phongTex->TCScaleY, G_TX_NOLOD);
					}
					else if(obj->phongTex->pixSize == G_IM_SIZ_16b)
					{
						gDPSetTile(glistp++,obj->phongTex->format, obj->phongTex->pixSize, (((obj->phongTex->sx * obj->phongTex->pixSize)+7)>>3),
							256, G_TX_RENDERTILE+2, 0, G_TX_WRAP | G_TX_NOMIRROR,
							obj->phongTex->TCScaleX, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, obj->phongTex->TCScaleY, G_TX_NOLOD);
					}
					else
					{
						gDPSetTile(glistp++,obj->phongTex->format, obj->phongTex->pixSize, (((obj->phongTex->sx * obj->phongTex->pixSize)+7)>>3),
							0, G_TX_RENDERTILE+2, 0, G_TX_WRAP | G_TX_NOMIRROR,
							obj->phongTex->TCScaleX, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, obj->phongTex->TCScaleY, G_TX_NOLOD);
					}
					gDPSetHilite2Tile(glistp++,G_TX_RENDERTILE+2, &dynamicp->hilite[hiliteNum], obj->phongTex->sx,obj->phongTex->sy);

				}
				else
				{
					gDPSetCombineMode(glistp++,G_CC_HILITERGB_PRIMA, G_CC_PASS2);
				}
				if(renderMode.xluSurf)
				{
					if(renderMode.pixelOut)
					{
						if(renderMode.useZMode)
						{
							gDPSetRenderMode(glistp++, G_RM_PASS,G_RM_ZB_CLD_SURF2);      
						}
						else
						{
							gDPSetRenderMode(glistp++, G_RM_PASS,G_RM_CLD_SURF2);      
						}
					}
					else
					{
						if(renderMode.useZMode)
						{
							gDPSetRenderMode(glistp++,G_RM_PASS,G_RM_AA_ZB_XLU_SURF2);
						}
						else
						{
							gDPSetRenderMode(glistp++,G_RM_PASS,G_RM_AA_XLU_SURF2);
						}
					}
				}
				else
				{
					if(renderMode.transparentSurf)
					{
						if(renderMode.useZMode)
						{
							gDPSetRenderMode(glistp++,G_RM_PASS,G_RM_AA_ZB_TEX_EDGE2);
						}
						else
						{
							gDPSetRenderMode(glistp++,G_RM_PASS,G_RM_AA_TEX_EDGE2);
						}
					}
					else
					{
						if(renderMode.pixelOut)
						{
							if(renderMode.useZMode)
							{
								gDPSetRenderMode(glistp++,G_RM_PASS,G_RM_AA_ZB_PCL_SURF2);
							}
							else
							{
								gDPSetRenderMode(glistp++,G_RM_PASS,G_RM_AA_PCL_SURF2);
							}
						}
						else
						{
							if(renderMode.useZMode)
							{
								gDPSetRenderMode(glistp++,G_RM_PASS,G_RM_AA_ZB_OPA_SURF2);
							}
							else
							{
								gDPSetRenderMode(glistp++,G_RM_PASS,G_RM_AA_OPA_SURF2);
							}
						}
					}
				}
				gDPSetHilite1Tile(glistp++,G_TX_RENDERTILE+1, &dynamicp->hilite[hiliteNum], phongX,phongY);
				lookatNum++;
				hiliteNum++;
			}
			else
				dprintf"RUN OUT OF HILITES!!!!!!!!!!!!!!\n"));
		}*/
		/*else*/ if(renderMode.phongMode)
		{
			renderMode.phongMode = 0;
			changeRenderMode = YES;
			gDPPipeSync(glistp++);
		    gDPSetCycleType(glistp++, G_CYC_1CYCLE);
			gSPClearGeometryMode(glistp++,G_TEXTURE_GEN);
			gSPTexture(glistp++,0xffff, 0xffff, 0, G_TX_RENDERTILE, G_ON);
		}
		/*if(obj->flags & OBJECT_FLAGS_MIP_MAP)
		{
		    gDPPipeSync(glistp++);
			renderMode.useTextureMode = 2;//TEXTURE_MODE_MIP_MAP;
			SetRenderMode();
			gSPTexture(glistp++, 0xffff, 0xffff, MIP_MAP_NUMTILES, MIP_MAP_STARTTILE, G_ON);
			gDPSetTextureLOD(glistp++, G_TL_LOD);
//			gDPPipelineMode(glistp++, G_PM_1PRIMITIVE);
			gDPSetCombineMode(glistp++, G_CC_MYTRILERP, G_CC_MODULATERGB_MODULATEPRIMA2); 
			gDPSetCycleType(glistp++, G_CYC_2CYCLE);
		}*/
		/*else*/ if(obj->flags & OBJECT_FLAGS_TEXTURE_BLEND)
		{
		    gDPPipeSync(glistp++);
			renderMode.useTextureMode = 3;//TEXTURE_MODE_BLEND;
			SetRenderMode();
			gSPTexture(glistp++, 0xffff, 0xffff, 0, G_TX_RENDERTILE, G_ON);
			gDPSetTextureLOD(glistp++, G_TL_TILE);
			/*if(obj->flags & OBJECT_FLAGS_TEXTURE_BLEND_ENV)
			{
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//				gDPSetEnvColor(glistp++,0,0,0,obj->textureBlendAlpha);
				gDPSetEnvColor(glistp++,0,0,0,SineWave(80,frameCount)*127 + 127);
				gDPSetCombineMode(glistp++, G_CC_TEXTUREBLENDENV, G_CC_TEXTUREBLEND2); 
			}
			else
			{  */
				gDPSetCombineMode(glistp++, G_CC_TEXTUREBLEND, G_CC_TEXTUREBLEND2); 
			/*}*/
			gDPSetCycleType(glistp++, G_CYC_2CYCLE);
		}
		else if(obj->flags & OBJECT_FLAGS_COLOUR_BLEND)
		{
			gDPPipeSync(glistp++);
			renderMode.useTextureMode = 4;//TEXTURE_MODE_COLOUR_BLEND;
			SetRenderMode();
			gSPTexture(glistp++, 0xffff, 0xffff, 0, G_TX_RENDERTILE, G_ON);
			gDPSetTextureLOD(glistp++, G_TL_TILE);
			gDPSetEnvColor(glistp++,obj->colour.r,obj->colour.g,obj->colour.b,obj->colour.a);
			if(obj->flags & OBJECT_FLAGS_FACE_COLOUR)
			{
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//do only when xluFact for object changes to save time - need an oldxlu type thang
				if(obj->flags & OBJECT_FLAGS_XLU)
					SetXluInDrawList(obj->drawList,xluFact);
				gDPSetCombineMode(glistp++,G_CC_FACECOLOURBLEND,G_CC_FACECOLOURBLEND2);
			}
			else
			{
				gDPSetCombineMode(glistp++,G_CC_COLOURBLEND,G_CC_COLOURBLEND2);
			}
			gDPSetCycleType(glistp++, G_CYC_2CYCLE);
		}
		else if(obj->flags & OBJECT_FLAGS_COLOUR_BLEND_AFTERLIGHT)
		{
			gDPPipeSync(glistp++);
			renderMode.useTextureMode = 5;//TEXTURE_MODE_COLOUR_BLEND_AFTERLIGHT;
			SetRenderMode();
			gSPTexture(glistp++, 0xffff, 0xffff, 0, G_TX_RENDERTILE, G_ON);
			gDPSetTextureLOD(glistp++, G_TL_TILE);
			gDPSetEnvColor(glistp++,obj->colour.r,obj->colour.g,obj->colour.b,obj->colour.a);
			if(obj->flags & OBJECT_FLAGS_FACE_COLOUR)
			{
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//do only when xluFact for object changes to save time - need an oldxlu type thang
				if(obj->flags & OBJECT_FLAGS_XLU)
					SetXluInDrawList(obj->drawList,xluFact);
				gDPSetCombineMode(glistp++,G_CC_FACECOLOURBLEND_AFTERLIGHT,G_CC_FACECOLOURBLEND_AFTERLIGHT2);
			}
			else
			{
				gDPSetCombineMode(glistp++,G_CC_COLOURBLEND_AFTERLIGHT,G_CC_COLOURBLEND_AFTERLIGHT2);
			}
			gDPSetCycleType(glistp++, G_CYC_2CYCLE);
		}
		else if(renderMode.useTextureMode != 1/*TEXTURE_MODE_NORMAL*/)
		{
		    gDPPipeSync(glistp++);
			renderMode.useTextureMode = 1;//TEXTURE_MODE_NORMAL;
			gDPSetTextureLOD(glistp++, G_TL_TILE);
			gSPTexture(glistp++, 0xffff, 0xffff, 0, G_TX_RENDERTILE, G_ON);
			gDPSetCycleType(glistp++, G_CYC_1CYCLE);
			changeRenderMode = TRUE;
		}
		if(changeRenderMode)
			SetRenderMode();
	}
	else
	{

		if(obj->flags & OBJECT_FLAGS_XLU)
		{
			tempFloat = (float)xluOverride / 100;
			tempFloat *= (float)obj->xlu;
			if(tempFloat > 255)
				tempFloat = 255;
			xluFact = tempFloat;
		}
		gDPSetPrimColor(glistp++,0,0,/*r*/255,/*g*/255,/*b*/255,xluFact);
//		gDPSetPrimColor(glistp++,0,0,/*r*/255,/*g*/255,/*b*/255,obj->xlu);
	}

	SetRendermodeForEnviroment();
}


/*	--------------------------------------------------------------------------------
	Function		: XfmPoint
	Purpose			: transforms a point
	Parameters		: VECTOR *,VECTOR *
	Returns			: 
	Info			: 
*/

long DIST		= 0;
long FOV		= 512;
float horizClip	= 300;
float vertClip	= 225;

void XfmPoint(VECTOR *vTemp2,VECTOR *in)
{
	float c[4][4];
	guLookAtF(c,
			currCamTarget[screenNum].v[X],currCamTarget[screenNum].v[Y],currCamTarget[screenNum].v[Z],
			currCamSource[screenNum].v[X],currCamSource[screenNum].v[Y],currCamSource[screenNum].v[Z],
			//stx,sty,stz,
			//ctx,cty,ctz,
			//0,1,0);
			camVect.v[X],camVect.v[Y],camVect.v[Z]);

	guMtxXFMF(c,in->v[X],in->v[Y],in->v[Z],
		&(vTemp2->v[X]),&(vTemp2->v[Y]),&(vTemp2->v[Z]));
	
		if  (((vTemp2->v[Z]+DIST)>nearPlaneDist) &&
			((vTemp2->v[Z]+DIST)<farPlaneDist) &&
			((vTemp2->v[X])>-horizClip) &&
			((vTemp2->v[X])<horizClip) &&
			((vTemp2->v[Y])>-vertClip) &&
			((vTemp2->v[Y])<vertClip))
		{
			float oozd = -1/(vTemp2->v[Z]+DIST);
			vTemp2->v[X] = 320+((vTemp2->v[X] * FOV) * oozd);
			vTemp2->v[Y] = 220+((vTemp2->v[Y] * FOV) * oozd);
		}
		else
			vTemp2->v[Z] = 0;
}


/*	--------------------------------------------------------------------------------
	Function		: ConvertQuatShortToQuat
	Purpose			: converts a short based quaternion to normal float based quaternion
	Parameters		: QUATERNIONSHORT *,QUATERNION *
	Returns			: void
	Info			: 
*/
void ConvertQuatShortToQuat(QUATERNIONSHORT *quatShort,QUATERNION *quat)
{
	quat->x = quatShort->x;
	quat->x /= 32768;
	quat->y = quatShort->y;
	quat->y /= 32768;
	quat->z = quatShort->z;
	quat->z /= 32768;
	quat->w = quatShort->w;
	quat->w /= 32768;
}


/*	--------------------------------------------------------------------------------
	Function		: FindToFromKeysSHORT
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/
void FindToFromKeysSHORT(KEYFRAMESHORT *keys,short *from,short *to,float *slerp,float time,int numKeys)
{
	int			i;
	int			low,high;
	short		fromKey,toKey;
	float		tempFloat;

	low=0;
	high=numKeys-1;

	if (time < keys[low].time)
	{
		*from = 0;
		*to = 1;
		*slerp = 0;
		return;
	}
	if (time > keys[high].time)
	{
		*from = high-1;
		*to = high-1;
		*slerp = 1;
		return;
	}

	for (;;)
	{
		i= (low+high) >> 1;
		if (keys[i].time==time)
		{
			low = i;
			high = i+1;
			break;
		}
		else
		{
			if (time<keys[i].time)
			{
				high = i;
				if ((high-low)<2) break;
			}
			else
			{
				low = i;
				if ((high-low)<2) break;
			}
		}
	}

	fromKey = low;
	toKey = low+1;

	if(fromKey != -1)		//time is within valid keyframes
	{
		tempFloat = abs(keys[toKey].time - keys[fromKey].time);
		tempFloat = (time - (float)keys[fromKey].time) / tempFloat;
	}
	else					// must be between last and first keyframe
	{
		fromKey = numKeys-1;
		toKey = fromKey;
		tempFloat = 1;
	}
	*from = fromKey;
	*to = toKey;
	*slerp = tempFloat;
}
