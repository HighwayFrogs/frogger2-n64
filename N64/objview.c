/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: objview.c
	Programmer	: Andrew Eder
	Date		: 11/10/98

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI_2

#include <ultra64.h>

#include "incs.h"


unsigned long objIDs[128];
ACTOR2 *viewActor = NULL;

OBJECT_VIEWER objectViewer;
					 
char objName[32];
char message[10];

static TEXTOVERLAY *lev[MAX_LEVELS];


/*	--------------------------------------------------------------------------------
	Function		: ObjViewInit()
	Purpose			: initialises object viewer
	Parameters		: 
	Returns			: void
	Info			: 
*/
void ObjViewInit()
{
	// set object viewer to object bank selection mode
	objectViewer.mode		= SELECTOBJECTBANK_MODE;
	objectViewer.viewMode	= OBJVIEW_GEOM;
	
	// set default initial values for object position
	objectViewer.objPos.v[0] = 0;
	objectViewer.objPos.v[1] = 0;
	objectViewer.objPos.v[2] = 500;

	// set initial rotation variables
	objectViewer.rotateAboutX = 0;
	objectViewer.rotateAboutY = 0;
	objectViewer.rotateAboutZ = 0;

	objectViewer.currObjNum		= 0;
	objectViewer.numObjects		= 0;

	objectViewer.currObj		= NULL;
}

/*	--------------------------------------------------------------------------------
	Function		: RunObjectViewer()
	Purpose			: runs the object viewer
	Parameters		: 
	Returns			: none
	Info			:
*/
void RunObjectViewer()
{
	switch(objectViewer.mode)
	{
		case SELECTOBJECTBANK_MODE:
			SelectObjectBank();
			break;

		case VIEWOBJECTBANK_MODE:
			ViewObjectBank();
			break;
	}
}

/*	--------------------------------------------------------------------------------
	Function		: SelectObjectBank
	Purpose			: object bank selection screen
	Parameters		: 
	Returns			: void
	Info			: 
*/
void SelectObjectBank()
{
	static unsigned long currentWorldSelection = 0;
	static unsigned long currentLevelSelection = 0;
	static u16 button,lastbutton;
	static TEXTOVERLAY *misc,*tOverObjBank = NULL;
	int i = 0,j = 0;
	
	if(frameCount == 1)
	{	
		char msgTmp[4];

		FreeMenuItems();
		LoadTextureBank(SYSTEM_TEX_BANK);
		LoadTextureBank(INGAMEGENERIC_TEX_BANK);

		misc = CreateAndAddTextOverlay(30,24,"OBJECT VIEWER",NO,255,smallFont,0,0);
		misc = CreateAndAddTextOverlay(32,26,"OBJECT VIEWER",NO,255,smallFont,0,0);
		misc->r = 255;	misc->g = 255;	misc->b = 255;

		CreateAndAddTextOverlay(30,55,"up and down selects world",NO,255,oldeFont,0,0);
		CreateAndAddTextOverlay(30,65,"left and right selects level",NO,255,oldeFont,0,0);

		// top pane
		CreateAndAddSpriteOverlay(25,20,"tippane.bmp",270,25,191,0);
		
		for(i=0; i<MAX_LEVELS; i++)
		{
			lev[i] = CreateAndAddTextOverlay(30 + (i * 24),130,"1",YES,91,smallFont,0,0);
//			DisableTextOverlay(lev[i]);
		}

		tOverObjBank = CreateAndAddTextOverlay(70,100,"************************",YES,255,smallFont,0,0);

		objectViewer.currWorldID = 0;
		sprintf(tOverObjBank->text,"%s (%d/%d)",worldVisualData[objectViewer.currWorldID].description,objectViewer.currWorldID+1,MAX_WORLDS);
		
		ResetParameters();

		objectViewer.numObjects = 0;
		objectViewer.currObjNum = 0;

		objectViewer.currWorldID = 0;
		objectViewer.currLevelID = 0;
	}

	button = controllerdata[ActiveController].button;

	// select previous world
	if((button & CONT_UP) && !(lastbutton & CONT_UP))
    {
		if(currentWorldSelection > 0)
			currentWorldSelection--;
	}
	// select next world
	else if((button & CONT_DOWN) && !(lastbutton & CONT_DOWN))
    {
		if(currentWorldSelection < (MAX_WORLDS - 1))
			currentWorldSelection++;
	}
	// select previous level
	else if((button & CONT_LEFT) && !(lastbutton & CONT_LEFT))
    {
		if(currentLevelSelection > 0)
			currentLevelSelection--;
	}
	// select next level
	else if((button & CONT_RIGHT) && !(lastbutton & CONT_RIGHT))
    {
		if(currentLevelSelection < (worldVisualData[objectViewer.currWorldID].numLevels - 1))
			currentLevelSelection++;
	}

	objectViewer.currWorldID = currentWorldSelection;
	objectViewer.currLevelID = currentLevelSelection;

	sprintf(tOverObjBank->text,"%s (%d/%d)",worldVisualData[objectViewer.currWorldID].description,objectViewer.currWorldID+1,MAX_WORLDS);
	
	if(frameCount > 15)
	{
		if((button & CONT_B) && !(lastbutton & CONT_B))
		{
			FreeAllLists();
			objectViewer.currObjNum = 0;
			objectViewer.numObjects = 0;
			objectViewer.mode		= SELECTOBJECTBANK_MODE;
			viewActor = NULL;

			gameState.mode			= MENU_MODE;
			gameState.menuMode		= DEVELOPMENT_MODE;
			developmentMode			= -1;
			frameCount				= 0;
			lastbutton				= 0;
			return;
		}

		if(((button & CONT_START) && !(lastbutton & CONT_START)) ||
		   ((button & CONT_A) && !(lastbutton & CONT_A)))
		{
			StopDrawing("wldbanks");

			// load relevant object bank and texture bank...
			FreeAllLists();

			objectViewer.mode		= VIEWOBJECTBANK_MODE;
			objectViewer.viewMode	= OBJVIEW_GEOM;

			frameCount = 0;
			lastbutton = 0;

			StartDrawing("wldbanks");

			return;
		}			
	}

	lastbutton = button;
}

/*	--------------------------------------------------------------------------------
	Function		: ViewObject
	Purpose			: views objects in the specified 'world' object bank
	Parameters		: unsigned long
	Returns			: 
	Info			: 
*/
void ViewObjectBank()
{
	static TEXTOVERLAY *viewTxt = NULL;
	static TEXTOVERLAY *msgTxt = NULL;
	ACTOR2 *temp;
	float rotMtxX[4][4],rotMtxY[4][4],rotMtxZ[4][4],resultMtx[4][4];
	float tempMtx[4][4];
	int i,j;

	if(frameCount == 1)
	{
		OBJECT_DESCRIPTOR *objDesc = NULL;
		OBJECT_CONTROLLER *objCont = NULL;

		LoadTextureBank(SYSTEM_TEX_BANK);
		LoadTextureBank(INGAMEGENERIC_TEX_BANK);
		LoadVisualBanksForWorld(objectViewer.currWorldID,objectViewer.currLevelID);

		// ...and get the object ID's from the bank
		objectViewer.numObjects = 0;
		for(j=0; j<MAX_OBJECT_BANKS; j++)
		{
			if(objectBanks[j].freePtr)
			{
				objectViewer.numObjects += objectBanks[j].numObjects;
				objDesc = objectBanks[j].objList;
					
				for(i=0; i<objectBanks[j].numObjects; i++)
				{
					objCont = objDesc[i].objCont;
					dprintf"Object ID %d : %x : %s\n",objectViewer.currObjNum,objDesc[i].objectID,objCont->object->name));
					
					if(gstrcmp(objCont->object->name,"nothing") == 0)
						continue;

					objIDs[objectViewer.currObjNum] = objDesc[i].objectID;

					viewActor = (ACTOR2 *)JallocAlloc(sizeof(ACTOR2),YES,"A2");
					viewActor->actor = (ACTOR *)JallocAlloc(sizeof(ACTOR),YES,"A");

					FindObject(&viewActor->actor->objectController,objIDs[objectViewer.currObjNum],"",YES);
					viewActor->flags = ACTOR_DRAW_NEVER;
					if(!viewActor->actor->objectController)
						continue;

					sprintf(viewActor->actor->objectController->object->name,objCont->object->name);
					InitActorStructures(viewActor->actor,INIT_ANIMATION);
					AnimateActor(viewActor->actor,0,YES,NO,0.5F,0,0);

					MakeUniqueActor(viewActor->actor,0);

					SetVector(&viewActor->actor->pos,&objectViewer.objPos);
					viewActor->actor->scale.v[X] = viewActor->actor->scale.v[Y] = viewActor->actor->scale.v[Z] = 1;

					ZeroQuaternion(&viewActor->actor->qRot);

					viewActor->next = actList;
					actList			= viewActor;
					objectViewer.currObjNum++;
				}
			}
		}

		// set to the first object
		objectViewer.currObj		= actList;
		objectViewer.currObjNum		= 0;
		objectViewer.currObj->flags = ACTOR_DRAW_ALWAYS;
	}

	if(!viewTxt)
		viewTxt	= CreateAndAddTextOverlay(30,25,NULL,NO,255,smallFont,0,0);
	if(!msgTxt)
		msgTxt	= CreateAndAddTextOverlay(30,205,NULL,NO,255,smallFont,0,0);

	sprintf(objName,"%s (%d/%d)",objectViewer.currObj->actor->objectController->object->name,objectViewer.currObjNum+1,objectViewer.numObjects);
	viewTxt->text = objName;

	objectMatrix = 0;

	// Build translation and rotation matrices
	guRotateF(rotMtxZ,objectViewer.rotateAboutZ,0,0,1);
	guRotateF(rotMtxY,objectViewer.rotateAboutY,0,1,0);
	guRotateF(rotMtxX,objectViewer.rotateAboutX,1,0,0);
	guMtxCatF(rotMtxY,rotMtxX,tempMtx);
	guMtxCatF(rotMtxZ,tempMtx,resultMtx);
	MatrixToQuaternion((MATRIX *)resultMtx,&objectViewer.currObj->actor->qRot);

	switch(objectViewer.viewMode)
	{
		case OBJVIEW_GEOM:
			sprintf(message,"geometry");
			msgTxt->text = message;

			if(frameCount > 10)
				ObjViewReadControllerGeom();
			break;

		case OBJVIEW_ANIM:
			sprintf(message,"animation");
			msgTxt->text = message;

			if(frameCount > 10)
				ObjViewReadControllerAnim();
			break;
	}
}

/*	--------------------------------------------------------------------------------
	Function		: ObjViewResetObjectView()
	Purpose			: resets default values for object viewer and centres the object
	Parameters		: none
	Returns			: none
	Info			:
*/
void ObjViewResetObjectView()
{
}

/*	--------------------------------------------------------------------------------
	Function		: ObjViewChangeViewMode()
	Purpose			: change viewing mode for object viewing
	Parameters		: none
	Returns			: none
	Info			: geometry only / anims + geometry, etc.
*/
void ObjViewChangeViewMode()
{
	objectViewer.viewMode++;
	objectViewer.viewMode %= OBJVIEW_MAXMODES;
}

/*	--------------------------------------------------------------------------------
	Function		: ObjViewGotoNextObject()
	Purpose			: moves to next object in the current object bank
	Parameters		: none
	Returns			: none
	Info			:
*/
void ObjViewGotoNextObject()
{
	int i;
	ACTOR2 *cur;

	objectViewer.currObj->flags = ACTOR_DRAW_NEVER;
	if(++objectViewer.currObjNum >= objectViewer.numObjects)
		objectViewer.currObjNum = 0;

	cur = actList;
	for(i=0; i<objectViewer.currObjNum; i++)
		cur = cur->next;

	objectViewer.currObj = cur;
	objectViewer.currObj->flags = ACTOR_DRAW_ALWAYS;
}

/*	--------------------------------------------------------------------------------
	Function		: ObjViewGotoPreviousObject()
	Purpose			: moves to previous object in the current object bank
	Parameters		: none
	Returns			: none
	Info			:
*/
void ObjViewGotoPreviousObject()
{
	int i;
	ACTOR2 *cur;

	objectViewer.currObj->flags = ACTOR_DRAW_NEVER;
	if(--objectViewer.currObjNum < 0)
		objectViewer.currObjNum = objectViewer.numObjects - 1;

	cur = actList;
	for(i=0; i<objectViewer.currObjNum; i++)
		cur = cur->next;

	objectViewer.currObj = cur;
	objectViewer.currObj->flags = ACTOR_DRAW_ALWAYS;
}

/*	--------------------------------------------------------------------------------
	Function		: ObjViewGotoFirstObject()
	Purpose			: moves to first object in the current object bank
	Parameters		: none
	Returns			: none
	Info			:
*/
void ObjViewGotoFirstObject()
{
	objectViewer.currObjNum = 0;
	objectViewer.currObj = actList;
}

/*	--------------------------------------------------------------------------------
	Function		: ObjViewGotoLastObject()
	Purpose			: moves to last object in the current object bank
	Parameters		: none
	Returns			: none
	Info			:
*/
void ObjViewGotoLastObject()
{
	objectViewer.currObjNum = objectViewer.numObjects - 1;
	objectViewer.currObj = &actList[objectViewer.currObjNum];
}

/*	--------------------------------------------------------------------------------
	Function		: ObjViewGotoNextAnimation()
	Purpose			: moves to next animation for the current bject
	Parameters		: none
	Returns			: none
	Info			: 
*/
void ObjViewGotoNextAnimation()
{
	if(objectViewer.currObj->actor->animation)
	{
		if(++objectViewer.currObj->actor->animation->currentAnimation >= objectViewer.currObj->actor->animation->numAnimations)
			objectViewer.currObj->actor->animation->currentAnimation = 0;
	}
}

/*	--------------------------------------------------------------------------------
	Function		: ObjViewGotoPreviousAnimation()
	Purpose			: moves to previous animation for the current object
	Parameters		: none
	Returns			: none
	Info			:
*/
void ObjViewGotoPreviousAnimation()
{
	if(objectViewer.currObj->actor->animation)
	{
		if(--objectViewer.currObj->actor->animation->currentAnimation < 0)
			objectViewer.currObj->actor->animation->currentAnimation = objectViewer.currObj->actor->animation->numAnimations - 1;
	}
}

/*	--------------------------------------------------------------------------------
	Function		: ObjViewGotoFirstAnimation()
	Purpose			: moves to first animation for the current object
	Parameters		: none
	Returns			: none
	Info			:
*/
void ObjViewGotoFirstAnimation()
{
	if(objectViewer.currObj->actor->animation)
		objectViewer.currObj->actor->animation->currentAnimation = 0;
}

/*	--------------------------------------------------------------------------------
	Function		: ObjViewGotoLastAnimation()
	Purpose			: moves to last animation for the current object
	Parameters		: none
	Returns			: none
	Info			:
*/
void ObjViewGotoLastAnimation()
{
	if(objectViewer.currObj->actor->animation)
		objectViewer.currObj->actor->animation->currentAnimation = objectViewer.currObj->actor->animation->numAnimations - 1;
}

/*	--------------------------------------------------------------------------------
	Function		: ObjViewChangeAnimSpeed(float speedChange)
	Purpose			: changes the animation playback speed
	Parameters		: speedChange either increases or decreases the current playback speed
	Returns			: none
	Info			: #define SPEED_UP and SPEED_DOWN can be found in the header file
*/
void ObjViewChangeAnimSpeed(float speedChange)
{
	// Alter current animation playback speed
	if(objectViewer.currObj->actor->animation)
		objectViewer.currObj->actor->animation->animationSpeed += speedChange;
}

/*	--------------------------------------------------------------------------------
	Function		: ObjViewReadControllerGeom()
	Purpose			: reads controller for viewing in geometry only mode
	Parameters		: none
	Returns			: none
	Info			:
*/
void ObjViewReadControllerGeom()
{
	static u16 button,lastbutton;
	static s16 stickX,stickY;

	button = controllerdata[ActiveController].button;
	stickX = controllerdata[ActiveController].stick_x;
	stickY = controllerdata[ActiveController].stick_y;

	if((button & CONT_START) && !(lastbutton & CONT_START))
	{
		// exit from object viewing
		FreeAllLists();
		
		frameCount = 0;
		lastbutton = 0;

		objectViewer.mode = SELECTOBJECTBANK_MODE;
		objectViewer.numObjects = 0;
		objectViewer.currObj = 0;
		return;
	}

	// ----- [ SIMPLE OBJECT MOVEMENT ] ---------- //
	
	if(stickY > 35 && !(button & CONT_G))
	{
		// Move object up
		objectViewer.currObj->actor->pos.v[1] += 4;
	}

	if(stickY < -35 && !(button & CONT_G))
	{
		// Move object down
		objectViewer.currObj->actor->pos.v[1] -= 4;
	}

	if(stickX < -35 && !(button & CONT_G))
	{
		// Move object left
		objectViewer.currObj->actor->pos.v[0] += 4;
	}

	if(stickX > 35 && !(button & CONT_G))
	{
		// Move object right
		objectViewer.currObj->actor->pos.v[0] -= 4;
	}

	if(stickY > 35 && (button & CONT_G))
	{
		// Move object along z axis
		objectViewer.currObj->actor->pos.v[2] += 8;
	}

	if(stickY < -35 && (button & CONT_G))
	{
		// Move object along z axis
		objectViewer.currObj->actor->pos.v[2] -= 8;
	}

	// ----- [ OTHER CONTROLS ] ---------- //
	
	if((button & CONT_L) && !(lastbutton & CONT_L))
	{
		// Select previous object
		ObjViewGotoPreviousObject();
	}

	if((button & CONT_R) && !(lastbutton & CONT_R))
	{
		// Select next object
		ObjViewGotoNextObject();
	}

	if((button & CONT_E) && !(button & CONT_G))
	{
		// Rotate about the x-axis
		objectViewer.rotateAboutX -= 4;
	}

	if((button & CONT_D) && !(button & CONT_G))
	{
		// Rotate about the x-axis
		objectViewer.rotateAboutX += 4;
	}

	if((button & CONT_F) && !(button & CONT_G))
	{
		// Rotate about the y-axis
		objectViewer.rotateAboutY += 4;
	}

	if((button & CONT_C) && !(button & CONT_G))
	{
		// Rotate about the y-axis
		objectViewer.rotateAboutY -= 4;
	}

	if((button & CONT_C) && (button & CONT_G))
	{
		// Rotate about the z-axis
		objectViewer.rotateAboutZ -= 4;
	}

	if((button & CONT_F) && (button & CONT_G))
	{
		// Rotate about the z-axis
		objectViewer.rotateAboutZ += 4;
	}

	if((button & CONT_B) && !(lastbutton & CONT_B))
	{
		// Toggle on/off antialiasing
	}

	if((button & CONT_A) && !(lastbutton & CONT_A))
	{
		// Change viewing mode
		ObjViewChangeViewMode();
	}

	lastbutton = button;
}

/*	--------------------------------------------------------------------------------
	Function		: ObjViewReadControllerAnim()
	Purpose			: reads controller for viewing with animations
	Parameters		: none
	Returns			: none
	Info			:
*/
void ObjViewReadControllerAnim()
{
	static u16 button,lastbutton = -1;
	static s16 stickX,stickY;

	button = controllerdata[ActiveController].button;
	stickX = controllerdata[ActiveController].stick_x;
	stickY = controllerdata[ActiveController].stick_y;

	if((button & CONT_START) && !(lastbutton & CONT_START))
	{
		// exit from object viewing
		FreeAllLists();
		
		frameCount = 0;
		lastbutton = 0;

		objectViewer.mode = SELECTOBJECTBANK_MODE;
		objectViewer.numObjects = 0;
		objectViewer.currObj = 0;
		return;
	}

	// ----- [ SIMPLE OBJECT MOVEMENT ] ---------- //

	if(stickY > 35 && !(button & CONT_G))
	{
		// Move object up
		objectViewer.currObj->actor->pos.v[1] += 4;
	}

	if(stickY < -35 && !(button & CONT_G))
	{
		// Move object down
		objectViewer.currObj->actor->pos.v[1] -= 4;
	}

	if(stickX < -35 && !(button & CONT_G))
	{
		// Move object left
		objectViewer.currObj->actor->pos.v[0] += 4;
	}

	if(stickX > 35 && !(button & CONT_G))
	{
		// Move object right
		objectViewer.currObj->actor->pos.v[0] -= 4;
	}

	if(stickY > 35 && (button & CONT_G))
	{
		// Move object along z axis
		objectViewer.currObj->actor->pos.v[2] += 16;
	}

	if(stickY < -35 && (button & CONT_G))
	{
		// Move object along z axis
		objectViewer.currObj->actor->pos.v[2] -= 16;
	}

	// ----- [ OTHER CONTROLS ] ---------- //

	if((button & CONT_L) && !(lastbutton & CONT_L))
	{
		// Select previous object
		ObjViewGotoPreviousObject();
	}

	if((button & CONT_R) && !(lastbutton & CONT_R))
	{
		// Select next object
		ObjViewGotoNextObject();
	}

	if(button & CONT_E)
	{
		// Increase animation speed
		ObjViewChangeAnimSpeed(SPEED_UP);
	}

	if(button & CONT_D)
	{
		// Decrease animation speed
		ObjViewChangeAnimSpeed(SPEED_DOWN);
	}

	if((button & CONT_F) && !(lastbutton & CONT_F))
	{
		// Select next animation
		ObjViewGotoNextAnimation();
	}

	if((button & CONT_C) && !(lastbutton & CONT_C))
	{
		// Select previous animation
		ObjViewGotoPreviousAnimation();
	}

	if((button & CONT_B) && !(lastbutton & CONT_B))
	{
		// Jump to first animation for the current object
		ObjViewGotoFirstAnimation();
	}

	if((button & CONT_A) && !(lastbutton & CONT_A))
	{
		// Change viewing mode
		ObjViewChangeViewMode();
	}

	lastbutton = button;
}
