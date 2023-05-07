/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: Controll.c
	Programmer	: Andrew Eder
	Date		: 11/9/98

----------------------------------------------------------------------------------------------- */


#include <ultra64.h>

#include "incs.h"



OSMesgQueue	controllerMsgQ;
OSMesg		controllerMsgBuf[8];
OSMesgQueue     serialMsgQ;
OSMesg          serialMsg;

int initControllers(void);

OSPfs			pfsRumble[MAXCONTROLLERS];

OSContStatus    statusdata[MAXCONTROLLERS];
OSContPad       controllerdata[MAXCONTROLLERS];

static u16		lastbutton;

int				ControllerMode = 0;
char			debugCtrlMode[20];

char			controllerPresent = FALSE;
char			rumblePresent[4];

//extern char UseAAMode;
extern char useFilt;


/*	--------------------------------------------------------------------------------
	Function		: initControllers()
	Purpose			: init. controllers...
	Parameters		: none
	Returns			: lowest number controller connected to system
	Info			:
*/

int initControllers()
{
    int i,lowestPad = -1;
    u8 pattern;
	short res;

    osCreateMesgQueue(&serialMsgQ, &serialMsg, 1);
    osSetEventMesg(OS_EVENT_SI, &serialMsgQ, (OSMesg)1);

    osContInit(&serialMsgQ, &pattern, &statusdata[0]);

    osCreateMesgQueue(&controllerMsgQ, controllerMsgBuf, 8);
    osSetEventMesg(OS_EVENT_SI, &controllerMsgQ, (OSMesg)0);
	Wait(10000);

	i = 4;
	while(i--)
		player[i].inputPause = INPUT_POLLPAUSE;

	i = MAXCONTROLLERS - 1;
	while(i--)
	{
		if((pattern & (1<<i)) && !(statusdata[i].errno & CONT_NO_RESPONSE_ERROR))
		{
			lowestPad = i;
			res = osMotorInit(&controllerMsgQ,&pfsRumble[i],i);
			if(res == PFS_ERR_DEVICE)
				dprintf"Controller %d : RumblePak - PFS_ERR_DEVICE\n",i));
			if(res == PFS_ERR_NOPACK)
				dprintf"Controller %d : RumblePak - PFS_ERR_NOPACK\n",i));

			if(res == 0)
				rumblePresent[i] = TRUE;
		}
    }

	if(lowestPad != 0)
	{
		controllerPresent = FALSE;
		lowestPad = 0;
	}
	else
		controllerPresent = TRUE;

	if(controllerPresent)
		InitRumble();

    return lowestPad;
}

/*	--------------------------------------------------------------------------------
	Function		: ResetParameters()
	Purpose			: reset program settings to defaults
	Parameters		: none
	Returns			: none
	Info			:
*/

void ResetParameters()
{
	int i;

	ScreenWidthDelta    = 0;

	UseUCode            = 4;
	OutLen              = RDP_OUTPUT_BUF_LEN;
  
	for (i=0; i<4; i++)
	{
		currCamSource[i].v[0] = 0;	currCamSource[i].v[1] = 0;	currCamSource[i].v[2] = -100;
		currCamTarget[i].v[0] = 0;	currCamTarget[i].v[1] = 0;	currCamTarget[i].v[2] = 0;
	}

	SetVector(&camVect,&cameraUpVect);
//	camera.rot.v[0]		= 0;	
//	camera.rot.v[1]		= 0;	
//	camera.rot.v[2]		= 0;

	ShadingMode         = GOURAUD;
	UseGlobalTransforms = 0;
}

/*	--------------------------------------------------------------------------------
	Function		: ReadController0()
	Purpose			: reads controller 0....
	Parameters		: none
	Returns			: none
	Info			:
*/

void ReadControllerData()
{
/*	static u16 button;
	static s16 stickX, stickY;      
    
	(void)osRecvMesg(&controllerMsgQ, NULL, OS_MESG_BLOCK);
	osContGetReadData(controllerdata);*/
}

/*	--------------------------------------------------------------------------------
	Function		: ReadController1()
	Purpose			: reads controller 1....
	Parameters		: none
	Returns			: none
	Info			: this controller mode used for viewing animations during object viewing
*/

float freeMove = 2;

void ReadDebugPad()
{
	static u16 button;
	static s16 stickX,stickY;
    
	static u16 button2,lastbutton2;
	static s16 stickX2,stickY2;

	//(void)osRecvMesg(&controllerMsgQ, NULL, OS_MESG_BLOCK);
	//osContGetReadData(controllerdata);

	button	= controllerdata[1].button;
	stickX	= controllerdata[1].stick_x;
	stickY	= controllerdata[1].stick_y;

	// FOR TESTING - VIDEO FOOTAGE CREATION - GENERAL FRIGS AND FUDGES ---------------------------

	button2 = controllerdata[2].button;
	stickX2	= controllerdata[2].stick_x;
	stickY2	= controllerdata[2].stick_y;


	if(stickX2 > 15)
		frog[0]->actor->pos.v[X] += freeMove;
	else if(stickX2 < -15)
		frog[0]->actor->pos.v[X] -= freeMove;

	if(stickY2 > 15)
		frog[0]->actor->pos.v[Z] -= freeMove;
	else if(stickY2 < -15)
		frog[0]->actor->pos.v[Z] += freeMove;

	if(button2 & CONT_D)
		frog[0]->actor->pos.v[Y] += freeMove;
	if(button2 & CONT_E)
		frog[0]->actor->pos.v[Y] -= freeMove;


	if((button2 & CONT_UP) && !(lastbutton2 & CONT_UP))
    {
		AnimateActor(frog[0]->actor,FROG_ANIM_SCRATCHHEAD,NO,NO,0.4F,0,0);
		AnimateActor(frog[0]->actor,FROG_ANIM_BREATHE,YES,YES,0.4F,0,0);
	}
	if((button2 & CONT_DOWN) && !(lastbutton2 & CONT_DOWN))
    {
		AnimateActor(frog[0]->actor,FROG_ANIM_LOOKDOWN,NO,NO,0.4F,0,0);
		AnimateActor(frog[0]->actor,FROG_ANIM_BREATHE,YES,YES,0.4F,0,0);
	}
	if((button2 & CONT_LEFT) && !(lastbutton2 & CONT_LEFT))
    {
		AnimateActor(frog[0]->actor,FROG_ANIM_LOOKLEFTANDRIGHT,NO,NO,0.4F,0,0);
		AnimateActor(frog[0]->actor,FROG_ANIM_BREATHE,YES,YES,0.4F,0,0);
	}
	if((button2 & CONT_RIGHT) && !(lastbutton2 & CONT_RIGHT))
    {
		AnimateActor(frog[0]->actor,FROG_ANIM_ASSONFIRE,NO,NO,0.4F,0,0);
		AnimateActor(frog[0]->actor,FROG_ANIM_BREATHE,YES,YES,0.4F,0,0);
	}

	if((button2 & CONT_A) && !(lastbutton2 & CONT_A))
    {
		AnimateActor(frog[0]->actor,42,NO,NO,0.4F,0,0);
		AnimateActor(frog[0]->actor,FROG_ANIM_BREATHE,YES,YES,0.4F,0,0);
	}
	if((button2 & CONT_B) && !(lastbutton2 & CONT_B))
    {
		AnimateActor(frog[0]->actor,FROG_ANIM_DANCE2,NO,NO,0.4F,0,0);
		AnimateActor(frog[0]->actor,FROG_ANIM_BREATHE,YES,YES,0.4F,0,0);
	}
	if((button2 & CONT_G) && !(lastbutton2 & CONT_G))
    {
		AnimateActor(frog[0]->actor,FROG_ANIM_DANCE3,NO,NO,0.4F,0,0);
		AnimateActor(frog[0]->actor,FROG_ANIM_BREATHE,YES,YES,0.4F,0,0);
	}

	if((button2 & CONT_C) && !(lastbutton2 & CONT_C))
    {
		AnimateActor(frog[0]->actor,FROG_ANIM_HOPLEFT,NO,NO,0.4F,0,0);
		AnimateActor(frog[0]->actor,FROG_ANIM_BREATHE,YES,YES,0.4F,0,0);
	}
	if((button2 & CONT_F) && !(lastbutton2 & CONT_F))
    {
		AnimateActor(frog[0]->actor,FROG_ANIM_HOPRIGHT,NO,NO,0.4F,0,0);
		AnimateActor(frog[0]->actor,FROG_ANIM_BREATHE,YES,YES,0.4F,0,0);
	}

	lastbutton2 = button2;

	//--------------------------------------------------------------------------------------------

	if(stickX > 15)
    {
		camDist.v[1]+=5;
	}	
	else if(stickX < -15)
    {	
		camDist.v[1]-=5;
    }

	if(stickY > 15)
    {
		camDist.v[2]+=5;
    }
	else if(stickY < -15)
    {
		camDist.v[2]-=5;
	}

	if(button & CONT_UP)
    {
		camLookOfs+=5;
	}
	    
	if(button & CONT_DOWN)
    {
		camLookOfs-=5;
	}

	if((button & CONT_LEFT) && !(lastbutton & CONT_LEFT))
    {
		camDist.v[0]-=5;
	}

	if((button & CONT_RIGHT) && !(lastbutton & CONT_RIGHT))
    {
		camDist.v[0]+=5;
	}

	if((button & CONT_E) && !(lastbutton & CONT_E))
    {
		ShowMemorySituation(MEM_SHOW_INGAMEINFO);
    }

	if((button & CONT_D) && !(lastbutton & CONT_D))
    {
		ShowJalloc();
		CheckJalloc();
	}

	if((button & CONT_F) && !(lastbutton & CONT_F))
    {
		ShowMemorySituation(0);
	}

	if(button & CONT_C)
    {
	}

	if((button & CONT_G) && !(lastbutton & CONT_G))
    {
	}

	if((button & CONT_A) && !(lastbutton & CONT_A))
    {
	}

	if((button & CONT_B) && !(lastbutton & CONT_B))
    {
		if(autoHop)
			autoHop = 0;
		else
			autoHop = 10000;
	}

	if((button & CONT_L) && !(lastbutton & CONT_L))
    {
	}

	if((button & CONT_R) && !(lastbutton & CONT_R))
    {
	}

	if((button & CONT_START) && !(lastbutton & CONT_START))
	{
		timerMode++;
		timerMode %= 3;
	}
  
	lastbutton = button;
}

/*	--------------------------------------------------------------------------------
	Function		: ReadController()
	Purpose			: select which controller is currently being used
	Parameters		: none
	Returns			: none
	Info			:
*/

void ReadController()
{
	int i = 4;

	ReadControllerData();
	ReadDebugPad();
}
