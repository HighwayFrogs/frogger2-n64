/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.
	
	  
	File		: cam.c
	Programmer	: Andrew Eder
	Date		: 11/10/98
	
----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI_2

#include <ultra64.h>

#include "incs.h"

VECTOR	cameraUpVect = { 0,1,0 };
VECTOR	camVect;

VECTOR	currCamSource[4] = {{ 0,0,-100 },{ 0,0,-100 },{ 0,0,-100 },{ 0,0,-100 }};
VECTOR	currCamTarget[4] = {{ 0,0,0 },{ 0,0,0 },{ 0,0,0 },{ 0,0,0 }};

VECTOR	actualCamSource[2];
VECTOR	actualCamTarget[2];

VECTOR currCamDist = {0,0,10};

float	xFOVNew				= 320.0F / 240.0F;
float	yFOVNew				= 45.0F;
float	xFOV				= 320.0F / 240.0F;
float	yFOV				= 45.0F;
float	precScaleFactor		= 1.0F;

VECTOR camSource[4]			= {{ 0,569,-200 },{ 0,569,-200 },{ 0,569,-200 },{ 0,569,-200 }};
VECTOR camTarget[4];
float camLookOfs			= 50;
float camLookOfsNew			= 50;

VECTOR camDist				= { 0,680,192 };
float camSpeed				= 15; //30; //9;
float camSpeed2				= 15; //25; //9;
float camSpeed3				= 15; //30; //8;
float camSpeed4				= 15; //25; //8;
float fovSpd				= 2;
float transCamSpeedMult		= 1.0F;

int	camFacing				= 0;
int nextCamFacing			= 0;
float scaleV				= 1.1F;

char controlCamera			= 0;
char fixedDir = 0;
char fixedPos = 0;
char firstPerson = 0;
char fixedUp = 0;
TRANSCAMERA *transCameraList = NULL;

extern long idleCamera;
VECTOR idleCamDist	= { 0,100,102 };

float sideSwaySpeed = 0.005,sideSwayAmt=50;

float FindMaxInterFrogDistance( );


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
void CheckForDynamicCameraChange(GAMETILE *tile)
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
				SetVector(&camSource[0], &cur->camOffset);
				fixedDir = 1;
				fixedPos = 1;
				fixedUp = 0;
				break;

			case FIXED_SOURCE:
				SetVector(&camSource[0], &cur->camOffset);
				fixedDir = 0;
				fixedPos = 1;
				fixedUp = 0;
			break;

			case LOOK_AT_TILE:
				SetVector(&camSource[0], &cur->camOffset);
				SetVector(&camTarget[0], &tile->centre );
				fixedDir = 1;
				fixedPos = 1;
				fixedUp = 0;
				break;

			case LOOK_IN_DIR:
				SetVector(&camSource[0], &cur->camOffset );
				
				SetVector(&camTarget[0],&cur->camLookAt);
				ScaleVector(&camTarget[0],200);
				AddToVector(&camTarget[0], &camSource[0]);
				
				fixedDir = 1;
				fixedPos = 1;
				fixedUp = 1;
				break;
			}

			if (cur->dirCamMustFace)
				camFacing = cur->dirCamMustFace - 1;

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

	fixedDir = 0;
	fixedPos = 0;
	firstPerson = 0;
}


/* --------------------------------------------------------------------------------
	Programmer	: Matthew Cloy
	Function	: CameraLookAtFrog

	Purpose		:
	Parameters	: (void)
	Returns		: void 
*/
void CameraLookAtFrog(void)
{
	if(frog[0] && !fixedDir && !controlCamera)
	{
		// Average frog position	
		float afx,afy,afz,sc;
		int i,l;
		afx = afy = afz = 0;
		l = 0;
		for (i=0; i<NUM_FROGS; i++)
		{
			if (frog[i]->action.healthPoints > 0)
			{
				afx += frog[i]->actor->pos.v[0];
				afy += frog[i]->actor->pos.v[1];
				afz += frog[i]->actor->pos.v[2];
				l++;
			}

			// Zoom in/out to keep multiplayer frogs in view
			sc = FindMaxInterFrogDistance( );
			if( sc != -1 ) scaleV = (sc*0.00115) + 0.6;
		}
		
		if (l)
		{
			afx/=l;
			afy/=l;
			afz/=l;
		}

		if (fixedPos)
		{
			camTarget[0].v[0] = afx + currTile[0]->normal.v[0];	
			camTarget[0].v[1] = afy + currTile[0]->normal.v[1];	
			camTarget[0].v[2] = afz + currTile[0]->normal.v[2];
		}
		else
		{
			if (!idleCamera)
			{
				camTarget[0].v[0] = afx+currTile[0]->dirVector[frogFacing[0]].v[0]*camLookOfs + currTile[0]->normal.v[0];
				camTarget[0].v[1] = afy+currTile[0]->dirVector[frogFacing[0]].v[1]*camLookOfs + currTile[0]->normal.v[1];
				camTarget[0].v[2] = afz+currTile[0]->dirVector[frogFacing[0]].v[2]*camLookOfs + currTile[0]->normal.v[2];
			}
			else
			{
				camTarget[0].v[0] = afx+currTile[0]->normal.v[0];	
				camTarget[0].v[1] = afy+currTile[0]->normal.v[1];	
				camTarget[0].v[2] = afz+currTile[0]->normal.v[2];
			}
		}
		
	}
	
}


/* --------------------------------------------------------------------------------
	Programmer	: Matthew Cloy
	Function	: SlurpCamPosition

	Purpose		:
	Parameters	: (void)
	Returns		: void 
*/
extern long initialCamera;

void SlurpCamPosition(long cam)
{
	float s1,s2,s3,s4;
	unsigned long afc = actFrameCount;

	if (idleCamera)
	{
		s1 = camSpeed * transCamSpeedMult * 6;
		s2 = camSpeed2 * transCamSpeedMult * 6;
		s3 = camSpeed3 * transCamSpeedMult * 6;
		s4 = camSpeed4 * transCamSpeedMult * 6;
	}
	else
	{
		s1 = camSpeed * transCamSpeedMult;
		s2 = camSpeed2 * transCamSpeedMult;
		s3 = camSpeed3 * transCamSpeedMult;
		s4 = camSpeed4 * transCamSpeedMult;
	}

	if( !lastActFrameCount )
		lastActFrameCount = afc - 1;

	while( lastActFrameCount < afc )
	{
		if (!initialCamera)
		{
			currCamSource[cam].v[0] -= (currCamSource[cam].v[0] - camSource[cam].v[0])/s1;
			currCamSource[cam].v[1] -= (currCamSource[cam].v[1] - camSource[cam].v[1])/s1;
			currCamSource[cam].v[2] -= (currCamSource[cam].v[2] - camSource[cam].v[2])/s1;

			currCamTarget[cam].v[0] -= (currCamTarget[cam].v[0] - camTarget[cam].v[0])/s3;
			currCamTarget[cam].v[1] -= (currCamTarget[cam].v[1] - camTarget[cam].v[1])/s3;
			currCamTarget[cam].v[2] -= (currCamTarget[cam].v[2] - camTarget[cam].v[2])/s3;

			if (idleCamera)
			{
				currCamDist.v[0] -= (currCamDist.v[0] - idleCamDist.v[0])/s4;
				currCamDist.v[1] -= (currCamDist.v[1] - idleCamDist.v[1])/s4;
				currCamDist.v[2] -= (currCamDist.v[2] - idleCamDist.v[2])/s4;
			}
			else
			{
				currCamDist.v[0] -= (currCamDist.v[0] - camDist.v[0]*scaleV)/s4;
				currCamDist.v[1] -= (currCamDist.v[1] - camDist.v[1]*scaleV)/s4;
				currCamDist.v[2] -= (currCamDist.v[2] - camDist.v[2]*scaleV)/s4;
			}
		}
		else
		{
			currCamSource[cam].v[0] = camSource[cam].v[0];
			currCamSource[cam].v[1] = camSource[cam].v[1];
			currCamSource[cam].v[2] = camSource[cam].v[2];

			currCamTarget[cam].v[0] = camTarget[cam].v[0];
			currCamTarget[cam].v[1] = camTarget[cam].v[1];
			currCamTarget[cam].v[2] = camTarget[cam].v[2];
		
			currCamDist.v[0] = camDist.v[0]*scaleV;
			currCamDist.v[1] = camDist.v[1]*scaleV;
			currCamDist.v[2] = camDist.v[2]*scaleV;
	
			initialCamera = 0;
		}
		
		if ( gameState.mode != CAMEO_MODE )
		{
			VECTOR t = { 0,0,0 };
			int i;
		
			for (i=0; i<NUM_FROGS; i++)
			{
				if (frog[i]->action.healthPoints > 0)
				{
					t.v[0]+=currTile[i]->normal.v[0];
					t.v[1]+=currTile[i]->normal.v[1];
					t.v[2]+=currTile[i]->normal.v[2];
				}
			}

			MakeUnit (&t);
			
			if (!fixedUp)
			{
				camVect.v[0] -= (camVect.v[0] - t.v[0])/s2;
				camVect.v[1] -= (camVect.v[1] - t.v[1])/s2;
				camVect.v[2] -= (camVect.v[2] - t.v[2])/s2;
			}
			else
			{
				camVect.v[0] = 0;
				camVect.v[1] = 1;
				camVect.v[2] = 0;
			}
			
			
		}

//		xFOV		-= (xFOV - xFOVNew) / (s1*fovSpd);
//		yFOV		-= (yFOV - yFOVNew) / (s1*fovSpd);
		camLookOfs	-= (camLookOfs - camLookOfsNew) / s1;

		lastActFrameCount++;
	}
}


/* --------------------------------------------------------------------------------
	Programmer	: Matthew Cloy
	Function	: UpdateCameraPosition

	Purpose		:
	Parameters	: (void)
	Returns		: void 
*/
float camSideOfs = 0;
GAMETILE *lastTile = NULL;
long prevCamFacing = 0;

void UpdateCameraPosition(long cam)
{
	VECTOR result;

	if(!frog[0] || !currTile[0] || controlCamera)
		return;
	
	if ( gameState.mode != CAMEO_MODE && !fixedPos )
	{
		float afx,afy,afz;
		float afx2,afy2,afz2;
		float afx3,afy3,afz3;
		long nC;
		int i,l;
		afx = afy = afz = 0;
		afx2 = afy2 = afz2 = 0;
		l=0;
		for (i=0; i<NUM_FROGS; i++)
		{
			if (frog[i]->action.healthPoints > 0)
			{
				afx += frog[i]->actor->pos.v[0];
				afy += frog[i]->actor->pos.v[1];
				afz += frog[i]->actor->pos.v[2];

		//		afx += frog[i]->actor->pos.v[0];
		//		afy += frog[i]->actor->pos.v[1];
		//		afz += frog[i]->actor->pos.v[2];

				if (fixedUp)
				{
					afx2 += 0;
					afy2 += 1*currCamDist.v[1];
					afz2 += 0;
				}
				else
				{
						afx2 += currTile[i]->normal.v[0]*currCamDist.v[1];
						afy2 += currTile[i]->normal.v[1]*currCamDist.v[1];
						afz2 += currTile[i]->normal.v[2]*currCamDist.v[1];
				}

				
				if (currTile[0] != lastTile)
				{
					if ((currTile[0]!=NULL) && (lastTile!=NULL))
					{
						if (camFacing == prevCamFacing)
							camFacing = GetTilesMatchingDirection(lastTile,camFacing,currTile[0]);

						prevCamFacing = camFacing;
					}

					lastTile = currTile[0];
				}

				afx2 -= currTile[0]->dirVector[camFacing].v[0]*currCamDist.v[2];
				afy2 -= currTile[0]->dirVector[camFacing].v[1]*currCamDist.v[2];
				afz2 -= currTile[0]->dirVector[camFacing].v[2]*currCamDist.v[2];

				l++;
			}
		}

		nC = (camFacing+1)&3;

		afx3 = currTile[0]->dirVector[nC].v[0]*camSideOfs;
		afy3 = currTile[0]->dirVector[nC].v[1]*camSideOfs;
		afz3 = currTile[0]->dirVector[nC].v[2]*camSideOfs;
	
		if (l)
		{
		
			afx/=l;
			afy/=l;
			afz/=l;
		
			afx2/=l;
			afy2/=l;
			afz2/=l;
		}

		if (idleCamera)
		{
			camSource[0].v[0] = afx+afx2+afx3;
			camSource[0].v[1] = afy+afy2+afy3;
			camSource[0].v[2] = afz+afz2+afz3;
		}
		else
		{
			camSource[0].v[0] = afx+afx2+afx3+currTile[0]->dirVector[frogFacing[0]].v[0]*camLookOfs;
			camSource[0].v[1] = afy+afy2+afy3+currTile[0]->dirVector[frogFacing[0]].v[1]*camLookOfs;
			camSource[0].v[2] = afz+afz2+afz3+currTile[0]->dirVector[frogFacing[0]].v[2]*camLookOfs;
		}
	}

	if (fixedUp)
	{
		camVect.v[0] = 0;
		camVect.v[1] = 1;
		camVect.v[2] = 0;
	}

	SlurpCamPosition(0);

	// Had to take this out because it was driving everyone nuts
	if( swingCam )
		camSideOfs = ((sinf(actFrameCount*sideSwaySpeed)*sideSwayAmt) * camDist.v[2]) / 350.0;
}


float FindMaxInterFrogDistance( )
{
	int i, max=0;
	float dist, best=-100000;
	VECTOR sep;

	for( i=0; i<NUM_FROGS; i++ )
	{
		SubVector( &sep, &frog[i]->actor->pos, &frog[0]->actor->pos );
		dist = Magnitude( &sep );

		if( dist > best )
		{
			max = i;
			best = dist;
		}
	}

	if( max ) return best;
	
	return -1;
}
