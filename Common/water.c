/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: water.c
	Programmer	: Andy Eder
	Date		: 20/10/99 16:47:47

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI_2

#include <ultra64.h>
#include "incs.h"

#ifdef N64_VERSION

ACTOR2 *watActor = NULL;

short numN64WaterObjects = 0;
short currN64WaterObject = 0;
SHORT2DVECTOR *wTC[MAX_N64_WATEROBJECTS];

short numN64ModgyTexObjects = 0;
short currN64ModgyTexObject = 0;
SHORT2DVECTOR *mTC[MAX_N64_MODGYTEXOBJECTS];


/*	--------------------------------------------------------------------------------
	Function		: RunWaterDemo
	Purpose			: runs the water demo (development)
	Parameters		: 
	Returns			: void
	Info			: 
*/
void RunWaterDemo()
{
	static u16 button,lastbutton;
	static s16 stickX,stickY;
	int j;
	
	if(frameCount == 1)
	{
		float x,y,z;
		int i;
		TEXTOVERLAY *txtTmp;
		
		FreeMenuItems();
		LoadTextureBank(SYSTEM_TEX_BANK);
		LoadObjectBank(INGAMEGENERIC_OBJ_BANK);

		CreateAndAddTextOverlay(30,20,"water stuff",NO,255,smallFont,0,0);
		txtTmp = CreateAndAddTextOverlay(32,22,"water stuff",NO,255,smallFont,0,0);
		txtTmp->r = 0;	txtTmp->g = 0;	txtTmp->b = 0;

		// add the water actor
		watActor = CreateAndAddActor("wat_fall.obe",0,0,100,0,0,0);
		watActor->flags = ACTOR_WATER;

		watActor->actor->qRot.x = -0.5F;
	}

	// read controller
	button = controllerdata[ActiveController].button;
	stickX = controllerdata[ActiveController].stick_x;
	stickY = controllerdata[ActiveController].stick_y;

	if((button & CONT_UP) && !(lastbutton & CONT_UP))
	{
	}

	if((button & CONT_DOWN) && !(lastbutton & CONT_DOWN))
	{
	}

	if((button & CONT_LEFT) && !(lastbutton & CONT_LEFT))
	{
	}

	if((button & CONT_RIGHT) && !(lastbutton & CONT_RIGHT))
	{
	}

	if((button & CONT_A) && !(lastbutton & CONT_A))
	{
	}

	if((button & CONT_B) && !(lastbutton & CONT_B))
	{
	}

	if((button & CONT_L) && !(lastbutton & CONT_L))
	{
	}

	if((button & CONT_R) && !(lastbutton & CONT_R))
	{
	}

	if((button & CONT_G) && !(lastbutton & CONT_G))
	{
		FreeAllLists();
		developmentMode = 0;
		gameState.mode = MENU_MODE;
		gameState.menuMode = TITLE_MODE;

		frameCount = 0;
		lastbutton = 0;
		return;
	}

	lastbutton = button;
}

//***** FOR WATER OBJECTS ************************************************************************

/*	--------------------------------------------------------------------------------
	Function		: UpdateWaterN64
	Purpose			: updates the specified water actor / object - new method
	Parameters		: ACTOR *
	Returns			: void
	Info			: 
*/
float modi1 = 0.065;
float modi2 = 1.29;
float modi3 = 2.5;
float modi4 = 96.0;

int modc1 = 32;
int modc2 = 96;
int modc3 = 16;

int slideSpeed = 0;

void UpdateWaterN64(ACTOR2 *wAct)
{
	// update the water - assumes drawlisted and skinned object....
	if(wAct)
	{
		float t,t2,xval,zval,mV;
		Vtx *in = NULL;
		int i,colMod;
		OBJECT_CONTROLLER *wObjCont = wAct->actor->objectController;

		in = wObjCont->vtx[wAct->actor->objectController->vtxBuf];
		i = wObjCont->numVtx;

		t = actFrameCount * modi1;

		while(i--)
		{
			if(wAct->flags & ACTOR_SLIDYTEX)
			{
				in->v.tc[1] -= (gameSpeed * slideSpeed * 1024);
				in->v.cn[3] = modc2;
			}
			else
			{
				// modge vertices
				xval = in->v.ob[X] * modi2;
				zval = in->v.ob[Z] * modi2;

				t2 = sinf(t + xval * zval * 0.5) - cosf(t + xval * 0.3 * zval);

				mV = t2 * modi4;

				in->v.ob[Y] += t2 * modi3;

				colMod = modc2 + (t2 * modi3 * modc3);

				if(colMod > 255)
					colMod = 255;
				else if(colMod < modc1)
					colMod = modc1;

				in->v.cn[3] = colMod;

				// modge texture co-ordinates
				in->v.tc[0] = wTC[currN64WaterObject][i].v[0] + mV;
				in->v.tc[1] = wTC[currN64WaterObject][i].v[1] + mV;
			}

			in++;
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function		: SetWaterModifiersN64
	Purpose			: sets specific water modifier values per world / level
	Parameters		: 
	Returns			: void
	Info			: 
*/
void SetWaterModifiersN64(short worldID,short levelID)
{
	// currently based on world - but can go to level based
	switch(worldID)
	{
		case WORLDID_FRONTEND:
			modc1 = 32;		modc2 = 96;		modc3 = 16;
			break;

		case WORLDID_SPACE:
			modc1 = 64;		modc2 = 128;	modc3 = 8;
			break;
	}
}


/*	--------------------------------------------------------------------------------
	Function		: AddN64WaterObjectResource
	Purpose			: adds N64 specific water related stuff
	Parameters		: 
	Returns			: void
	Info			: 
*/
void AddN64WaterObjectResource(ACTOR *wAct)
{
	int i;
	Vtx *in;

	if(wAct->objectController && (numN64WaterObjects < MAX_N64_WATEROBJECTS))
	{
		// set water modifiers for current world and level
		SetWaterModifiersN64(player[0].worldNum,player[0].levelNum);

		// copy texture co-ords to an array
		in = wAct->objectController->vtx[wAct->objectController->vtxBuf];
		i = wAct->objectController->numVtx;

		dprintf"Allocating %d vertices for water object %d\n",i,numN64WaterObjects));
		wTC[numN64WaterObjects] = (SHORT2DVECTOR *)JallocAlloc(sizeof(SHORT2DVECTOR) * i,YES,"s2dv");

		while(i--)
		{
			wTC[numN64WaterObjects][i].v[0] = in->v.tc[0];
			wTC[numN64WaterObjects][i].v[1] = in->v.tc[1];
			in++;
		}

		numN64WaterObjects++;
	}
}


/*	--------------------------------------------------------------------------------
	Function		: FreeN64WaterResources
	Purpose			: frees N64 specific water related stuff
	Parameters		: 
	Returns			: void
	Info			: 
*/
void FreeN64WaterResources()
{
	int i;

	for(i=0; i<numN64WaterObjects; i++)
		JallocFree((UBYTE **)&wTC[i]);

	numN64WaterObjects = 0;
	currN64WaterObject = 0;
}


//***** FOR MODGY (NON-WATER) OBJECTS ************************************************************


/*	--------------------------------------------------------------------------------
	Function		: UpdateModgyTexN64
	Purpose			: updates the specified modgy actor / object - new method
	Parameters		: ACTOR *
	Returns			: void
	Info			: 
*/
void UpdateModgyTexN64(ACTOR2 *mAct)
{
	// update the water - assumes drawlisted and skinned object....
	if(mAct)
	{
		float t,t2,xval,zval,mV;
		Vtx *in = NULL;
		int i;

		in = mAct->actor->objectController->vtx[mAct->actor->objectController->vtxBuf];
		i = mAct->actor->objectController->numVtx;

		t = actFrameCount * 0.02;
		while(i--)
		{
			// modge vertices
			xval = in->v.ob[X] * 0.05;
			zval = in->v.ob[Z] * 0.05;

			t2 = sinf(t + xval * zval * 0.5) - cosf(t + xval * 0.3 * zval);

			mV = t2 * 96;

			// modge texture co-ordinates
			in->v.tc[0] = mTC[currN64ModgyTexObject][i].v[0] + mV;
			in->v.tc[1] = mTC[currN64ModgyTexObject][i].v[1] + mV;

			in++;
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function		: AddN64ModgyTexObjectResource
	Purpose			: adds N64 specific modgy tex related stuff
	Parameters		: 
	Returns			: void
	Info			: 
*/
void AddN64ModgyTexObjectResource(ACTOR *mAct)
{
	int i;
	Vtx *in;

	if(mAct->objectController && (numN64ModgyTexObjects < MAX_N64_MODGYTEXOBJECTS))
	{
		// copy texture co-ords to an array
		in = mAct->objectController->vtx[mAct->objectController->vtxBuf];
		i = mAct->objectController->numVtx;

		dprintf"Allocating %d vertices for modgy tex object %d\n",i,numN64ModgyTexObjects));
		mTC[numN64ModgyTexObjects] = (SHORT2DVECTOR *)JallocAlloc(sizeof(SHORT2DVECTOR) * i,YES,"s2dv");

		while(i--)
		{
			mTC[numN64ModgyTexObjects][i].v[0] = in->v.tc[0];
			mTC[numN64ModgyTexObjects][i].v[1] = in->v.tc[1];
			in++;
		}

		numN64ModgyTexObjects++;
	}
}


/*	--------------------------------------------------------------------------------
	Function		: FreeN64ModgyTexResources
	Purpose			: frees N64 specific modgy tex related stuff
	Parameters		: 
	Returns			: void
	Info			: 
*/
void FreeN64ModgyTexResources()
{
	int i;

	for(i=0; i<numN64ModgyTexObjects; i++)
		JallocFree((UBYTE **)&mTC[i]);

	numN64ModgyTexObjects = 0;
	currN64ModgyTexObject = 0;
}



#endif
