/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.
	
	  
	File		: cam.c
	Programmer	: Andrew Eder
	Date		: 11/10/98
	
----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI_2

#include <ultra64.h>

#include "incs.h"


VECTOR cameraUpVect		= { 0,1,0 };
VECTOR camVect;

VECTOR currCamSource	= { 0,0,-100 };
VECTOR currCamTarget	= { 0,0,0 };
VECTOR currCamDist		= { 0,0,10 };
VECTOR camSource		= { 0,569,-200 };
VECTOR camTarget;

/*
VECTOR	actualCamSource[2];
VECTOR	actualCamTarget[2];
*/

// in floaty camera mode, the offset of the camera from the player
VECTOR currCamOffset, camOffset = {0,100,100};		

float	xFOVNew				= 320.0F / 240.0F;
float	yFOVNew				= 45.0F;
float	xFOV				= 320.0F / 240.0F;
float	yFOV				= 45.0F;
float	precScaleFactor		= 1.0F;

float camLookOfs			= 50;
float camLookOfsNew			= 50;

VECTOR camDist				= { 0,680,192 };
float camSpeed				= 15; 
float camSpeed2				= 15; 
float camSpeed3				= 20;	// source & target interp.
float camSpeed4				= 15;
float fovSpd				= 2;
float transCamSpeedMult		= 1.0F;

int	camFacing[4]			= {0,0,0,0};
short startCamFacing = 0, startFrogFacing = 0;
float scaleV				= 1.1F;

char controlCamera			= 0;
char fixedDir = 0;
char fixedPos = 0;
char firstPerson = 0;
char fixedUp = 0;
TRANSCAMERA *transCameraList = NULL;

extern long idleCamera;
VECTOR idleCamDist	= { 0,100,102 };

float sideSwaySpeed = 0.005,sideSwayAmt=50,swayModifier = 1.0f;

float camSideOfs = 0;
GAMETILE *lastTile[4] = {0,0,0,0};
long prevCamFacing[4] = {0,0,0,0};

CAM_BOX_LIST cameraBoxes;
CAM_BOX *currCamBox;
float cam_edge_spacing = 100;

float cam_shakiness = 0.0f;
float cam_shake_falloff = 0.1f;

char playerFocus = 0;

// functions

float FindMaxInterFrogDistance( );
void CameraSetSource(void);
int CameraBoundPosition(VECTOR *v, CAM_BOX *box, float edge);
void CalcSPCamera( VECTOR *target );
void SlurpCamPosition( );

/*	--------------------------------------------------------------------------------
	Function		: CameraBoundPosition
	Purpose			: limits a vector to a camera space, with edge padding
	Parameters		: VECTOR *, CAM_BOX *, float
	Returns			: 

int CameraBoundPosition(VECTOR *v, CAM_BOX *box, float edge)
{
	int p;
	float dist, foo;
	CAM_PLANE *plane;
	VECTOR a, new_v;
	int count;

	if (!box)
		return 0;

	count = 0;
	plane = box->planes;
	SetVector(&new_v, v);
	foo = edge;

	for (p = box->numPlanes; p; p--, plane++)
	{
		if (!plane->status) continue;

		dist = DotProduct(&new_v, &plane->normal) - plane->k;

		if (dist < edge)
		{
			if (dist < foo)
				foo = dist;

			dist -= edge;

			SetVector(&a, &plane->normal);
			ScaleVector(&a, -dist);
			AddToVector(&new_v, &a);
			
			count++;
		}
	}

	SetVector(v, &new_v);

	return count;
}
*/

int GetCamLimitVector(VECTOR *out, VECTOR *v, CAM_BOX *box, float edge)
{
	int p;
	float dist, foo;
	CAM_PLANE *plane;
	VECTOR a, res;
	int count;

	if (!box)
		return 0;

	count = 0;
	plane = box->planes;
	ZeroVector(&res);
	foo = edge;

	for (p = box->numPlanes; p; p--, plane++)
	{
		if (!plane->status) continue;

		dist = DotProduct(v, &plane->normal) - plane->k;

		if (dist < edge)
		{
			if (dist < foo)
				foo = dist;

			dist -= edge;

			SetVector(&a, &plane->normal);
			ScaleVector(&a, -dist);
			AddToVector(&res, &a);
			
			count++;
		}
	}

	SetVector(out, &res);

	return count;
}


/*	--------------------------------------------------------------------------------
	Function		: IsPointInCameraBox
	Purpose			: checks if a 3D point is within a camera space
	Parameters		: 
	Returns			: 
*/

int IsPointInCameraBox(VECTOR v, CAM_BOX *box)
{
	int p;
	CAM_PLANE *plane = box->planes;

	for (p = box->numPlanes; p; p--, plane++)
	{
		if ((DotProduct(&v, &plane->normal) - plane->k) < 0)
			return 0;
	}
	return 1;
}

/*	--------------------------------------------------------------------------------
	Function		: CheckCameraBoxes
	Purpose			: checks the current camera box, handles transitions between boxes
	Parameters		: 
	Returns			: 
*/

void CheckCameraBoxes(void)
{
	if (!currCamBox || !IsPointInCameraBox(camTarget, currCamBox))
	{
		CAM_BOX *box = cameraBoxes.boxes;
		int b, c;

		if (currCamBox)
			dprintf"Frogger leaves cam box (%08x)\n", box));

		currCamBox = NULL;
	
		for (b = cameraBoxes.numBoxes, c = 0; b; b--, c++, box++)
		{
			if (IsPointInCameraBox(camTarget, box))
			{
				currCamBox = box;
				dprintf"Frogger enters cam box #%d (%08x)\n", c, box));
				break;
			}
		}
	}
}



/*	--------------------------------------------------------------------------------
	Function		: CreateAndAddTransCamera
	Purpose			: adds a dynamic camera angle change for a specified game tile
	Parameters		: GAMETILE *,unsigned long,float,float,float
	Returns			: TRANSCAMERA *
	Info			: 
*/
TRANSCAMERA *CreateAndAddTransCamera(GAMETILE *tile,unsigned long dirCamMustFace, VECTOR *v, unsigned long flags)
{
	TRANSCAMERA *newItem = (TRANSCAMERA *)JallocAlloc(sizeof(TRANSCAMERA),YES,"TRNSCAM");

	newItem->tile			= tile;
	newItem->dirCamMustFace	= dirCamMustFace;
	SetVector(&newItem->camOffset, v);
	newItem->flags			= flags;
	newItem->FOV			= 45.0;
	newItem->speed			= 1.0;

	newItem->next			= transCameraList;
	transCameraList			= newItem;

	return newItem;
}


/*	--------------------------------------------------------------------------------
	Function		: CheckForDynamicCameraChange
	Purpose			: checks current tile to see if a camera change is needed
	Parameters		: GAMETILE *
	Returns			: void
	Info			: 
*/
void CheckForDynamicCameraChange(GAMETILE *tile, int pl)
{
	char specialCaseTile = 0;
	TRANSCAMERA *cur;
	static VECTOR stdVector = { 0,480,192 };
	
	cur = transCameraList;
	while(cur)
	{
		if(cur->tile == tile)
		{
			switch (cur->flags)
			{
			case LOOK_AT_FROG:
				SetVector(&camDist,&cur->camOffset);
				specialCaseTile = 1;
				fixedDir = 0;
				fixedPos = 0;
				fixedUp	 = 0;
				break;

			case LOOK_AT_FROG_NOTILT:
				SetVector(&camDist,&cur->camOffset);
				specialCaseTile = 1;
				fixedDir = 0;
				fixedPos = 0;
				fixedUp	 = 1;
				break;

			case STATIC_CAMERA:
				SetVector(&camSource, &cur->camOffset);
				fixedDir = 1;
				fixedPos = 1;
				fixedUp = 0;
				break;

			case FIXED_SOURCE:
				SetVector(&camSource, &cur->camOffset);
				fixedDir = 0;
				fixedPos = 1;
				fixedUp = 0;
			break;

			case LOOK_AT_TILE:
				SetVector(&camSource, &cur->camOffset);
				SetVector(&camTarget, &tile->centre );
				fixedDir = 1;
				fixedPos = 1;
				fixedUp = 0;
				break;

			case LOOK_IN_DIR:
				SetVector(&camSource, &cur->camOffset );
				
				SetVector(&camTarget,&cur->camLookAt);
				ScaleVector(&camTarget,200);
				AddToVector(&camTarget, &camSource);
				
				fixedDir = 1;
				fixedPos = 1;
				fixedUp = 1;
				break;
			}

			SubVector(&currCamOffset, &currCamSource, &currCamTarget);

			if (cur->dirCamMustFace)
				camFacing[pl] = cur->dirCamMustFace - 1;

			if( cur->speed )
				transCamSpeedMult = cur->speed;
			else
				transCamSpeedMult = 1.0F;

			yFOVNew = cur->FOV;

			break;
		}
		cur = cur->next;
	}

//	if(!specialCaseTile)
//		SetVector(&camDist,&stdVector);
}


/*	--------------------------------------------------------------------------------
	Function		: FreeTransCameraList
	Purpose			: frees the camera list
	Parameters		: 
	Returns			: void
	Info			: 
*/
void FreeTransCameraList()
{
	TRANSCAMERA *cur,*next;

	cur = transCameraList;
	while(cur)
	{
		next = cur->next;
		JallocFree((UBYTE**)&cur);
		cur = next;
	}

	transCameraList = NULL;
	lastTile[0] = NULL;
	lastTile[1] = NULL;
	lastTile[2] = NULL;
	lastTile[3] = NULL;

	fixedDir = 0;
	fixedPos = 0;
	firstPerson = 0;

	startCamFacing = startFrogFacing = 0;
}


/* --------------------------------------------------------------------------------
	Function	: CameraLookAtFrog
	Purpose		: Sets camTarget to the frog, or an average position
	Parameters	: (void)
	Returns		: void 
*/
void CameraLookAtFrog(void)
{
	if(frog[0] && !fixedDir && !controlCamera)
	{
		// Average frog position	
		VECTOR target;

		if( gameState.multi == SINGLEPLAYER )
			CalcSPCamera( &target );
		else
			CalcMPCamera( &target );

/*		if (!idleCamera)
		{
			VECTOR v;
			SetVector(&v, currTile[0]->dirVector[frogFacing[0]]);
			ScaleVector(&v, camLookOfs);
			AddToVector(&target, v);
		}
*/
		SetVector(&camTarget, &target);
	}
	
}

void CalcSPCamera( VECTOR *target )
{
	VECTOR v;
	float t = player[0].jumpTime;
	
	if (t > 0 && t < 1)	// jumping; calculate linear position
	{
		SetVector(target, &player[0].jumpOrigin);

		// horizontal
		SetVector(&v, &player[0].jumpFwdVector);
		ScaleVector(&v, t);
		AddToVector(target, &v);

		// vertical
		//SetVector(&v, &player[i].jumpUpVector);
		//ScaleVector(&v, player[i].heightJumped);
		//AddToVector(&target, &v);
	}										
	else
		SetVector(target, &frog[0]->actor->pos);
}


void CameraSetOffset(void)
{
	long nC;
	int i, l=0;

	if( gameState.multi == SINGLEPLAYER )
	{
		VECTOR v;
		nC = (camFacing[0]+1)&3;

		SetVector(&v, &camVect);
		ScaleVector(&v, currCamDist.v[1]);
		SetVector(&camOffset, &v);

		SetVector(&v, &currTile[0]->dirVector[camFacing[0]]);
		ScaleVector(&v, -currCamDist.v[2]);
		AddToVector(&camOffset, &v);

		SetVector(&v, &currTile[0]->dirVector[nC]);
		ScaleVector(&v, camSideOfs);
		AddToVector(&camOffset, &v);
	}
	else
	{
		float afx=0,afy=0,afz=0;
	
		for( i=0; i<NUM_FROGS; i++ )
		{
			nC = (camFacing[i]+1)&3;
			if( player[i].healthPoints && !(player[i].frogState & FROGSTATUS_ISDEAD) )
			{
				afx += currTile[i]->normal.v[0]*currCamDist.v[1] - currTile[i]->dirVector[camFacing[i]].v[0]*currCamDist.v[2]; 
				afy += currTile[i]->normal.v[1]*currCamDist.v[1] - currTile[i]->dirVector[camFacing[i]].v[1]*currCamDist.v[2];
				afz += currTile[i]->normal.v[2]*currCamDist.v[1] - currTile[i]->dirVector[camFacing[i]].v[2]*currCamDist.v[2];

				afx += currTile[i]->dirVector[nC].v[0]*camSideOfs;
				afy += currTile[i]->dirVector[nC].v[1]*camSideOfs;
				afz += currTile[i]->dirVector[nC].v[2]*camSideOfs;

				l++;
			}
		}

		camOffset.v[0] = afx;
		camOffset.v[1] = afy;
		camOffset.v[2] = afz;

		if (l > 1)
		{
			float scale = 1.0f/l;
			ScaleVector( &camOffset, scale );
		}
	}
}


/* --------------------------------------------------------------------------------
	Function	: SlurpCamPosition
	Purpose		:
	Parameters	: (void)
	Returns		: void 
*/
void SlurpCamPosition( )
{
	float s1,s2,s3,s4;
	VECTOR v, w;

	if (idleCamera)
	{
		s1 = gameSpeed / (camSpeed * transCamSpeedMult * 6);
		s2 = gameSpeed / (camSpeed2 * transCamSpeedMult * 6);
		s3 = gameSpeed / (camSpeed3 * transCamSpeedMult * 6);
		s4 = gameSpeed / (camSpeed4 * transCamSpeedMult * 6);
	}
	else
	{
		s1 = gameSpeed / (camSpeed * transCamSpeedMult);
		s2 = gameSpeed / (camSpeed2 * transCamSpeedMult);
		s3 = gameSpeed / (camSpeed3 * transCamSpeedMult);
		s4 = gameSpeed / (camSpeed4 * transCamSpeedMult);
	}

	SubVector(&v, &camTarget, &currCamTarget);
	ScaleVector(&v, s3);
	AddToVector(&currCamTarget, &v);

	GetCamLimitVector(&v, &currCamTarget, currCamBox, cam_edge_spacing);
	ScaleVector(&v, s3);
	AddToVector(&currCamTarget, &v);

	if (!fixedPos)
	{
		// use offset from target..
		CameraSetOffset();

		SubVector(&v, &camOffset, &currCamOffset);
		ScaleVector(&v, s3);
		AddToVector(&currCamOffset, &v);

		AddVector(&currCamSource, &currCamTarget, &currCamOffset);
	}
	else
	{
		// .. otherwise interpolate source independantly
		SubVector(&v, &camSource, &currCamSource);
		ScaleVector(&v, s3);
		AddToVector(&currCamSource, &v);

		// do we actually WANT to limit the camera source?
		// duuhh...
		GetCamLimitVector(&v, &currCamSource, currCamBox, cam_edge_spacing);
		ScaleVector(&v, s3);
		AddToVector(&currCamSource, &v);
	}

	// camVect (up vector)

	if (!fixedUp)
	{
		SubVector(&v, &currTile[0]->normal, &camVect);
		ScaleVector(&v, s2);
		AddToVector(&camVect, &v);
	}

	// camDist

	currCamDist.v[0] = camDist.v[0]*scaleV;
	currCamDist.v[1] = camDist.v[1]*scaleV;
	currCamDist.v[2] = camDist.v[2]*scaleV;
	
	if ( gameState.mode != CAMEO_MODE )
	{
		VECTOR t = { 0,0,0 };
		int i;
	
		for (i=0; i<NUM_FROGS; i++)
		{
			if( player[i].healthPoints )
			{
				t.v[0]+=currTile[i]->normal.v[0];
				t.v[1]+=currTile[i]->normal.v[1];
				t.v[2]+=currTile[i]->normal.v[2];
			}
		}

		MakeUnit (&t);
		
		if (!fixedUp)
		{
			camVect.v[0] -= s2 * (camVect.v[0] - t.v[0]);
			camVect.v[1] -= s2 * (camVect.v[1] - t.v[1]);
			camVect.v[2] -= s2 * (camVect.v[2] - t.v[2]);
		}
		/*else
		{
			camVect.v[0] = 0;
			camVect.v[1] = 1;
			camVect.v[2] = 0;
		}*/
	}

	camLookOfs	-= s1 * (camLookOfs - camLookOfsNew);
}


/* --------------------------------------------------------------------------------
	Function	: UpdateCameraPosition
	Purpose		: updates the position of the camera, and, er, does some other stuff
	Parameters	: (void)
	Returns		: void 
*/
void UpdateCameraPosition( )
{
	int i;

	if(!frog[playerFocus] || !currTile[playerFocus] || controlCamera)
		return;

	for( i=0; i<NUM_FROGS; i++ )
	{
		if (currTile[i] != lastTile[i])	//This causes lots of problems with camera transitions..
		{
			if (currTile[i] && lastTile[i])
			{
				if (camFacing[i] == prevCamFacing[i])
					camFacing[i] = GetTilesMatchingDirection(lastTile[i],camFacing[i],currTile[i]);
			}
			prevCamFacing[i] = camFacing[i];
			lastTile[i] = currTile[i];
			CheckForDynamicCameraChange(currTile[i], i);
		}
	}

	CameraLookAtFrog();

	CheckCameraBoxes();

	//if ( gameState.mode != CAMEO_MODE && !fixedPos )
	//	CameraSetSource();

	/*if (fixedUp)
	{
		camVect.v[0] = 0;
		camVect.v[1] = 1;
		camVect.v[2] = 0;
	}*/

	SlurpCamPosition();

	// Handle shaky camera nonsense
	
	if (cam_shakiness > 0.0)
	{
		int i;
		for (i = 0; i < 3; i++)
		{
			currCamTarget.v[i] += (Random(1000)*0.001f - 0.5f)*cam_shakiness;
			currCamSource.v[i] += (Random(1000)*0.001f - 0.5f)*cam_shakiness;
		}
		cam_shakiness -= cam_shake_falloff * gameSpeed;
	}

	// edge spacing should probably related to tan(FOV/2)
	cam_edge_spacing = DistanceBetweenPoints(&currCamSource, &currCamTarget) * 0.75f;

	if( swingCam )
	{
		// if the player is on their last life - give the swaying camera more 'urgency' - subtle
		// bunch of arse, more like
		if(player[0].lives < 3)
		{
			swayModifier	= 3.0f;
			sideSwayAmt		= 25.0f;
		}
		else
		{
			swayModifier	= 1.0f;
			sideSwayAmt		= 50.0f;
		}

		camSideOfs = ((sinf(actFrameCount*sideSwaySpeed*swayModifier)*sideSwayAmt) * camDist.v[2]) / 350.0;
	}
}


void CameraShake(float amount)
{
	cam_shakiness = amount;
}


void ResetCamera( )
{
	VECTOR initialCamOffset = { 0,100,100 };
	VECTOR initialCamDist = { 0,680,192 };
	VECTOR initialCamSource	= { 0,569,-200 };
	VECTOR initialCurrCamSource = { 0,100,100 };

	SetVector( &camSource, &initialCamSource );
	SetVector( &currCamSource, &initialCurrCamSource );
	SetVector( &camOffset, &initialCamOffset );
	SetVector( &currCamOffset, &initialCamOffset );
	SetVector( &camDist, &initialCamDist );
	ZeroVector( &currCamTarget );
	SetVector( &cameraUpVect, &upVec );
}

/*	--------------------------------------------------------------------------------
	Function		: InitCamera
	Purpose			: 
	Parameters		: 
	Returns			: 
*/

void InitCamera(void)
{
	int i;
	currCamBox = NULL;
	CheckCameraBoxes();

	if( gameState.multi == SINGLEPLAYER )
	{
		lastTile[0] = NULL;
		CheckForDynamicCameraChange(currTile[0],0);
	}
	else
	{
		for( i=0; i<NUM_FROGS; i++ )
		{
			camFacing[i] = 0;
			lastTile[i] = NULL;
			CheckForDynamicCameraChange(currTile[i],i);
		}
	}

	cam_shakiness = 0;

	SubVector(&currCamOffset, &camTarget, &camSource);
	UpdateCameraPosition();

	SetVector(&currCamSource, &camSource);
	SetVector(&currCamTarget, &camTarget);
	SetVector(&currCamOffset, &camOffset);
	SetVector(&camVect, &currTile[0]->normal);
}




float FindMaxInterFrogDistance( )
{
	int i, j, max=0;
	float dist, best=-100000;
	VECTOR sep;

	for( i=0; i<NUM_FROGS; i++ )
	{
		for( j=0; j<NUM_FROGS; j++ )
		{
			SubVector( &sep, &frog[i]->actor->pos, &frog[j]->actor->pos );
			dist = Magnitude( &sep );

			if( dist > best )
			{
				max = i;
				best = dist;
			}
		}
	}

	if( max ) return best;
	
	return -1;
}
