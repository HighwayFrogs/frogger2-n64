/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: Subcube.h
	Programmer	: Andy Eder
	Date		: 15/04/99 10:40:33

----------------------------------------------------------------------------------------------- */

#ifndef SUBCUBE_H_INCLUDED
#define SUBCUBE_H_INCLUDED


extern short transform;

extern float rotmat[4][4], scalemat[4][4], transmat[4][4];

enum
{
	AA_MODE_NONE,
	AA_MODE_REDUCED,
	AA_MODE_FULL,
};

enum
{
	TEXTURE_MODE_NONE,
	TEXTURE_MODE_NORMAL,
	TEXTURE_MODE_MIP_MAP,
	TEXTURE_MODE_BLEND,
	TEXTURE_MODE_COLOUR_BLEND,
	TEXTURE_MODE_COLOUR_BLEND_AFTERLIGHT,
};

typedef struct
{
	UBYTE	transparentSurf;
	UBYTE	aiSurf;
	UBYTE	xluSurf;
	UBYTE	useAAMode;
	UBYTE	useZMode;
	UBYTE	useTextureMode;
	UBYTE	mirrorTextures;
	UBYTE	phongMode;
	UBYTE	faceColour;
	UBYTE	objectColour;
	UBYTE	actorColour;
	UBYTE	pixelOut;
	UBYTE	phongAllowed;
}RENDER_MODE;


extern RENDER_MODE renderMode;
extern char lookatNum;
extern char hiliteNum;
extern float hedSpeed;

extern ACTOR2 *hat[MAX_FROGS];

void InitActor(ACTOR *tempActor, char *name, float x, float y, float z, int initFlags);
void SubActor(ACTOR2 *actor);
void InitActorStructures(ACTOR *tempActor, int initFlags);
void LoadTexture(TEXTURE *texture);

void WriteObjectDisplayListGouraud(OBJECT *obj);
void WriteObjectDisplayListFlat(OBJECT *obj);
void TransformAndDrawObject(OBJECT *obj, float time, short animStart, short animEnd);
void DrawActor(ACTOR *actor);

void DeformTextureCoords(OBJECT *obj);

void TransformObject ( OBJECT *obj, float time );
void FindToFromQKeys ( QKEYFRAME *keys,short *from,short *to,float *interp,float time,int numKeys );
void FindToFromVKeys ( VKEYFRAME *keys,short *from,short *to,float *interp,float time,int numKeys );
void DrawObject		( OBJECT *obj, Gfx *drawList, int skinned );
void SetupRenderModeForObject ( OBJECT *obj );

void TransformSkinnedObject(OBJECT *obj, float time);

void XfmPoint(VECTOR *vTemp2,VECTOR *in);

void ConvertQuatShortToQuat(QUATERNIONSHORT *quatShort,QUATERNION *quat);
void FindToFromKeysSHORT(KEYFRAMESHORT *keys,short *from,short *to,float *interp,float time,int numKeys);


extern TEXTURE	*tempTexture;
extern char		*tempObjectPtr, *tempObjectPtr2;
extern VECTOR	*actorScale;
extern VECTOR	parentScale;
extern Vtx		*vtxPtr;
extern Vtx	objectsVtx[2][2000];
extern short	xluFact;
extern char		changeRenderMode;

#endif