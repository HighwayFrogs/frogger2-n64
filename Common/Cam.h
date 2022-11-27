/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: Cam.h
	Programmer	: Andy Eder
	Date		: 15/04/99 15:01:33

----------------------------------------------------------------------------------------------- */

#ifndef CAM_H_INCLUDED
#define CAM_H_INCLUDED


#include "define.h"

enum
{
	STATIC_CAMERA,
	FIXED_SOURCE,
	LOOK_AT_FROG,
	LOOK_AT_FROG_NOTILT,
	LOOK_AT_TILE,
	LOOK_IN_DIR,
};


//----- [ FUNCTION PROTOTYPES ] ----------------------------------------------------------------//

typedef struct TAGTRANSCAMERA
{
	GAMETILE		*tile;
	unsigned long		dirCamMustFace;
	unsigned long		flags;
	VECTOR			camOffset, camLookAt;
	float FOV;
	float speed;

	struct TAGTRANSCAMERA	*next;

} TRANSCAMERA;

// ------------- Camera boxes ----------------

typedef struct
{
	VECTOR	normal;
	float	k;
	unsigned long status;
} CAM_PLANE;

typedef struct
{
	int numPlanes;
	CAM_PLANE *planes;
} CAM_BOX;

typedef struct
{
	int numBoxes;
	CAM_BOX *boxes;
} CAM_BOX_LIST;

//----- [ GLOBALS ] ----------------------------------------------------------------------------//

extern VECTOR	cameraUpVect;
extern VECTOR	camVect;
extern VECTOR	camDist;

extern float	camSpeed;
extern float	camLookOfs;
extern int		camFacing[];
extern long		prevCamFacing[];
extern short	startCamFacing, startFrogFacing;

extern VECTOR	camSource;
extern VECTOR	camTarget;
extern VECTOR	currCamSource;
extern VECTOR	currCamTarget;

extern VECTOR	currCamDist;
/*
extern VECTOR	actualCamSource[2];
extern VECTOR	actualCamTarget[2];
*/
extern float	xFOV;				// actually the SCREEN RATIO, i.e. 4/3
extern float	yFOV;				// THIS is the field of view
extern float	precScaleFactor;

extern float	scaleV;

extern char		controlCamera;
extern char		fixedDir;
extern char		fixedPos;
extern char		fixedUp;
extern char		firstPerson;

extern float cam_shakiness;
extern float cam_shake_falloff;

extern char playerFocus;
extern VECTOR currCamOffset, camOffset;
extern GAMETILE *lastTile[];

extern CAM_BOX_LIST cameraBoxes;

//----- [ FUNCTION PROTOTYPES ] ----------------------------------------------------------------//

TRANSCAMERA *CreateAndAddTransCamera(GAMETILE *tile,unsigned long dirCamMustFace, VECTOR *v, unsigned long flags);
void CheckForDynamicCameraChange(GAMETILE *tile, int pl);
void FreeTransCameraList();

void CameraLookAtFrog();
void UpdateCameraPosition( );

void InitCamera(void);


#endif