/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: Block.c
	Programmer	: Andrew Eder
	Date		: 11/6/98

----------------------------------------------------------------------------------------------- */

//#define ROM_BUILD

#define F3DEX_GBI_2

#include <ultra64.h>
#include <assert.h>
#include "incs.h"

#define RDP_OUTPUT_SIZE		4096

unsigned long actFrameCount = 0,lastActFrameCount = 0,currentFrameTime = 0;
char codeRunning = 0;

Mtx Identity;

ACTOR2 *clouds;
TEXTURE *cloudtex	= NULL;

// Symbol genererated by "makerom" to indicate the end of the code segment in virtual (and physical) memory
extern char _codeSegmentEnd[];

u64	bootStack[SMALLSTACKSIZE/sizeof(u64)];

char musicPriority = 140;

static OSThread	idleThread;								// Lowest priority idle thread - must have idle thread !
static u64 idleThreadStack[SMALLSTACKSIZE/sizeof(u64)];	// Stack used by lowest priority idle thread

OSThread mainThread;									// The main thread
u64 mainThreadStack[STACKSIZE/sizeof(u64)];				// Stack used by the main thread

OSThread drawGraphicsThread;							// The graphics drawing thread
u64 drawGraphicsThreadStack[STACKSIZE/sizeof(u64)];		// Stack used by graphics drawing thread

OSThread ControllerThread;
u64      ControllerThreadStack[SMALLSTACKSIZE/sizeof(u64)];

#define NUM_PI_MSGS		12

static OSMesg		PiMessages[NUM_PI_MSGS];			// PI Messages 
static OSMesgQueue	PiMessageQ;							// PI message queue

// Message queues
OSMesgQueue		dmaMessageQ, rspMessageQ, rdpMessageQ, retraceMessageQ;
OSMesg			dmaMessageBuf, rspMessageBuf, rdpMessageBuf, retraceMessageBuf;
OSMesg			dummyMessage;
OSIoMesg		dmaIOMessageBuf;
OSMesg			dummyMesg;

char			outputMessageBuffer[256];

char			ShadingMode         =  GOURAUD;
int             UseGlobalTransforms =  0;
int             ChangeVideoModes    =  0;
char			numtasks			=  1;

int             ScreenWidth         =  SCREEN_WD;
int             ScreenWidthDelta    =  0;
int				UseUCode            =  3;
int				OutLen              =  RDP_OUTPUT_BUF_LEN;

char			transparentSurf		= 0;
char			xluSurf				= 0;
char			aiSurf				= 0;

short			farPlaneDist		= 2000;
short			nearPlaneDist		= 6;

int				rdp_output_size		= RDP_OUTPUT_BUF_LEN;
u64				rdp_output_len;
u8				rdpSendBuffer[2000];

char			mirrorTextures		= FALSE;
char			viewTextures		= 1;

// ------------------------------------------------

char			displayLogos		= 1;
char			displayOverlays		= TRUE;
char			onlyDrawBackdrops	= FALSE;
char			oldFogMode			= 0;

char			codeDrawingRequest	= TRUE;
char			gfxIsDrawing		= FALSE;

char			idlePriority		= 1;
char			codePriority		= 10;
char			graphicsPriority	= 20;
char			controllerPriority	= 115;

int				gfxTasks			= 0;

OSMesgQueue		*sched_gfxMQ;

NNSched			sched;
OSMesgQueue		gfx_msgQ;
OSMesg			gfx_msgbuf[8];
OSMesgQueue		main_msgQ;
OSMesg			main_msgbuf[8];
NNScClient		gfx_client;
NNScClient		main_client;
u64				gfxYieldBuf[OS_YIELD_DATA_SIZE/sizeof(u64)];
GFXMsg			gfx_msg;

// Task descriptor
NNScTask		tlist[2][MAXTASKS];
NNScTask		*tlistPointers[2][MAXTASKS];
NNScTask		**tlistp;

int				actualGFXListLength[MAXTASKS];
Gfx				*glistp;

Dynamic			*dynamicp;							// Ptr to the dynamic data
Dynamic			dynamic[2];							// Dynamic data

char			draw_buffer			= 0;
void			*cfb_ptrs[2];

char			currentTask			= 1;
unsigned int	TicksPerFrame;
float			TicksPerSec;

int dontClearScreen = 0;

short dontDoAnims = 0;

// ------------------------------------------------

u64				dram_stack[SP_DRAM_STACK_SIZE64+2];	// For RSP tasks - used for matrix stack
    

//----- [ FORWARD DECLARATIONS ] -----//

static void idle(void *arg);
static void main_(void *arg);


/*	--------------------------------------------------------------------------------
	Function		: debugPrintf(int num)
	Purpose			: prints debug information to the file server output window
	Parameters		:
	Returns			: none
	Info			:
*/
void debugPrintf(int num)
{
    #ifndef ROM_BUILD
	PCwrite(-1, outputMessageBuffer, num);
    #endif
}

/*	--------------------------------------------------------------------------------
	Function		: Crash(char *mess)
	Purpose			: prints a message in file server output window if system crash
	Parameters		:
	Returns			: none
	Info			:
*/
void Crash(char *mess)
{
//    asm("break 0x407");
//      NEW_TEXT(160, 120, 255, 255, 255, 255, 1, 1, 0, mess, YES);
        dprintf"\n\nCRASHED %s!!!!!!!!!!!!!!!\n",mess));
//      while(1);
}

/*	--------------------------------------------------------------------------------
	Function		: boot()
	Purpose			: boots and creates the idle thread
	Parameters		: none
	Returns			: none
	Info			: creates and starts the idle system thread
*/
void boot()
{
	int i;

    // NOTE: Cannot call osSyncPrintf() until an idle thread.is set-up
    
    osInitialize();
	
#ifndef ROM_BUILD
	init_debug();
	PCinit();
    asm("break 0x407");
	dprintf"\n\n** Frogger2 Started **\n"));
#endif

    // Create and start the idle thread
	osCreateThread(&idleThread,1,idle,(void *)0,idleThreadStack+SMALLSTACKSIZE/sizeof(u64),idlePriority);
    osStartThread(&idleThread);

	// ...never reach here...
}

/*	--------------------------------------------------------------------------------
	Function		: idle(void *arg)
	Purpose			: init. video and start PI manager for cartridge access
	Parameters		:
	Returns			: none
	Info			: creates and starts the main thread
*/
static void idle(void *arg)
{
	// Start PI Mgr for access to cartridge
	osCreatePiManager((OSPri)OS_PRIORITY_PIMGR,&PiMessageQ,PiMessages,NUM_PI_MSGS);
  
	// Create and start main thread
	osCreateThread(&mainThread,3,main_,arg,mainThreadStack+STACKSIZE/sizeof(u64),codePriority);
	osStartThread(&mainThread);
  
	// Become the idle thread
	osSetThreadPri(0,0);
  
	for (;;);
}

/*	--------------------------------------------------------------------------------
	Function		: CreateMessageQueues()
	Purpose			: creates the message queues
	Parameters		: none
	Returns			: none
	Info			:
*/
void CreateMessageQueues()
{
	osCreateMesgQueue(&dmaMessageQ,&dmaMessageBuf,1);			// Create dma message queue

	osCreateMesgQueue(&rspMessageQ,&rspMessageBuf,1);			// Create rsp message queue
//	osSetEventMesg(OS_EVENT_SP,&rspMessageQ,dummyMessage);

	osCreateMesgQueue(&rdpMessageQ,&rdpMessageBuf,1);			// Create rdp message queue
//	osSetEventMesg(OS_EVENT_DP,&rdpMessageQ,dummyMessage);

	osCreateMesgQueue(&retraceMessageQ,&retraceMessageBuf,1);	// Create VI retrace message queue
	//	osViSetEvent(&retraceMessageQ,dummyMessage,1);  
}

/*	--------------------------------------------------------------------------------
	Function		: CreateTaskStructure
	Purpose			: creates graphics task and selects the ucode
	Parameters		: int,int
	Returns			: none
	Info			:
*/
void CreateTaskStructure(int n,int ucode)
{
	// Build graphics task
	tlistp[n]->list.t.ucode_boot		= (u64 *)rspbootTextStart;
	tlistp[n]->list.t.ucode_boot_size	= (u32)rspbootTextEnd - (u32)rspbootTextStart;

	tlistp[n]->list.t.output_buff_size	= (u64 *)&rdp_output[RDP_OUTPUT_SIZE];
	tlistp[n]->list.t.flags				= 0;

	switch(ucode)
	{
		case UCODE_POLY:
			// use the gspF3DEX2 ucode
			tlistp[n]->list.t.ucode			= (u64 *)gspF3DEX2_fifoTextStart;
			tlistp[n]->list.t.ucode_data	= (u64 *)gspF3DEX2_fifoDataStart;
			break;

		case UCODE_LINE:
			// use the gspL3DEX2 ucode
			tlistp[n]->list.t.ucode			= (u64 *)gspL3DEX2_fifoTextStart;
			tlistp[n]->list.t.ucode_data	= (u64 *)gspL3DEX2_fifoDataStart;
			break;

		case UCODE_SPRITE:
			// use the gspS2DEX2 ucode
			tlistp[n]->list.t.ucode			= (u64 *)gspS2DEX2_fifoTextStart;
			tlistp[n]->list.t.ucode_data	= (u64 *)gspS2DEX2_fifoDataStart;
			break;		  
	}

	// Initial display list
	tlistp[n+1]->list.t.data_ptr		= (u64 *)glistp;
}


// Create light structure with ambient colour amd 1 diffuse light source
Lights1 diffuseL1 = gdSPDefLights1(0x10,0x10,0x10,			// Ambient light colour
									0xff,0xff,0xff,			// Diffuse light colour
									0,0,127					// Light direction components TOWARDS light source
									);

/*	--------------------------------------------------------------------------------
	Function		: SetupViewing()
	Purpose			: sets up viewing
	Parameters		: none
	Returns			: none
	Info			:
*/
void SetupViewing()
{
	u16 perspNorm;

	guPerspective(&dynamicp->projection[0],&perspNorm,yFOV,xFOV,nearPlaneDist,farPlaneDist,precScaleFactor);

//	if(ShadingMode == LIGHTING)
/*
	{
		guLookAtHilite(&(dynamicp->viewing[0]),&(dynamicp->lookat[0]),&(dynamicp->hilite[0]),
			actualCamSource[draw_buffer].v[X],actualCamSource[draw_buffer].v[Y],actualCamSource[draw_buffer].v[Z],
			actualCamTarget[draw_buffer].v[X],actualCamTarget[draw_buffer].v[Y],actualCamTarget[draw_buffer].v[Z],
			camVect.v[X],camVect.v[Y],camVect.v[Z],
			0.0, 0.0, 127.0, 
			0.0, 0.0, 127.0,
			32,32);					
	}
*/
//	else

	{
		guLookAt(&dynamicp->viewing[0], 
			actualCamSource[draw_buffer].v[X],actualCamSource[draw_buffer].v[Y],actualCamSource[draw_buffer].v[Z],
			actualCamTarget[draw_buffer].v[X],actualCamTarget[draw_buffer].v[Y],actualCamTarget[draw_buffer].v[Z],
			camVect.v[X],camVect.v[Y],camVect.v[Z]);
    }

	gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(&(dynamicp->projection[0])),
		G_MTX_PROJECTION|G_MTX_LOAD|G_MTX_NOPUSH);

	gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(&(dynamicp->viewing[0])),
		G_MTX_PROJECTION|G_MTX_MUL|G_MTX_NOPUSH);  

	gSPPerspNormalize(glistp++,perspNorm);  
	gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(&Identity),
		G_MTX_MODELVIEW|G_MTX_LOAD|G_MTX_NOPUSH);
}

/*	--------------------------------------------------------------------------------
	Function		: SetRenderMode()
	Purpose			: sets the render mode (?)
	Parameters		: none
	Returns			: none
	Info			:
*/

//#define G_CC_MYNEWMODE a,b,c,d,A,B,C,D
#define G_CC_MYNEWMODE1 TEXEL0,0,SHADE,0,	0,0,SHADE,0

void SetRenderMode()
{
	changeRenderMode = 0;
    gDPPipeSync(glistp++);

	gSPSetGeometryMode(glistp++,
						G_ZBUFFER |
						G_SHADE |
						G_SHADING_SMOOTH);

	if(fog.mode == 1)
	{
		gSPSetGeometryMode(glistp++, G_FOG);

	    gDPSetCycleType(glistp++, G_CYC_2CYCLE);

		if(renderMode.xluSurf)
		{
			if(renderMode.faceColour)
			{
				gDPSetEnvColor(glistp++,255,255,255,xluFact);
				gDPSetCombineMode(glistp++,G_CC_MODULATEPRIM_ENVALPHA, G_CC_MODULATEPRIM_ENVALPHA);
			}
			else if(renderMode.transparentSurf)
			{
				gDPSetCombineMode(glistp++,G_CC_MODULATERGB_MODULATEPRIMA, G_CC_PASS2);
			}
			else
			{
				gDPSetCombineMode(glistp++, G_CC_MODULATERGBPRIMA, G_CC_PASS2);
			}
			gDPSetPrimColor(glistp++,0,0,255,255,255,xluFact);

			if(renderMode.pixelOut)
			{
				gSPSetGeometryMode(glistp++, G_ZBUFFER);
				gDPSetPrimColor(glistp++,0,0,255,255,255,renderMode.pixelOut);
				gDPSetRenderMode(glistp++, 
					(renderMode.useTextureMode != TEXTURE_MODE_NORMAL) ? G_RM_PASS : G_RM_FOG_SHADE_A, G_RM_ZB_CLD_SURF2);      
			}
			else if((renderMode.useAAMode == AA_MODE_FULL) && (renderMode.useZMode))
			{
				gSPSetGeometryMode(glistp++, G_ZBUFFER);
				gDPSetRenderMode(glistp++,
					(renderMode.useTextureMode != TEXTURE_MODE_NORMAL) ? G_RM_PASS : G_RM_FOG_SHADE_A, G_RM_AA_ZB_XLU_SURF2);      
			}
			else if((renderMode.useAAMode == AA_MODE_NONE) && (renderMode.useZMode))
			{
				gSPSetGeometryMode(glistp++, G_ZBUFFER);
				gDPSetRenderMode(glistp++, 
					(renderMode.useTextureMode != TEXTURE_MODE_NORMAL) ? G_RM_PASS : G_RM_FOG_SHADE_A, G_RM_ZB_XLU_SURF2);      
			}
			else if((renderMode.useAAMode == AA_MODE_FULL) && (!renderMode.useZMode))
			{
				gSPClearGeometryMode(glistp++, G_ZBUFFER);
				gDPSetRenderMode(glistp++, 
					(renderMode.useTextureMode != TEXTURE_MODE_NORMAL) ? G_RM_PASS : G_RM_FOG_SHADE_A, G_RM_AA_XLU_SURF2);      
			}
			else if((renderMode.useAAMode == AA_MODE_REDUCED) && (renderMode.useZMode))
			{
				gSPSetGeometryMode(glistp++, G_ZBUFFER);
				gDPSetRenderMode(glistp++, 
					(renderMode.useTextureMode != TEXTURE_MODE_NORMAL) ? G_RM_PASS : G_RM_FOG_SHADE_A, G_RM_RA_ZB_OPA_SURF2);      
			}
			else if((renderMode.useAAMode == AA_MODE_REDUCED) && (!renderMode.useZMode))
			{
				gSPClearGeometryMode(glistp++, G_ZBUFFER);
				gDPSetRenderMode(glistp++, 
					(renderMode.useTextureMode != TEXTURE_MODE_NORMAL) ? G_RM_PASS : G_RM_FOG_SHADE_A, G_RM_RA_OPA_SURF2);      
			}
			else
			{
				gSPClearGeometryMode(glistp++, G_ZBUFFER);
				gDPSetRenderMode(glistp++, 
					(renderMode.useTextureMode != TEXTURE_MODE_NORMAL) ? G_RM_PASS : G_RM_FOG_SHADE_A, G_RM_XLU_SURF2);      
			}  
		}
		else	//fog
		{
			if(renderMode.transparentSurf)
			{
				gDPSetCombineMode(glistp++, G_CC_MODULATERGB_MODULATEPRIMA, G_CC_PASS2);

				if(renderMode.pixelOut)
				{
					gSPSetGeometryMode(glistp++, G_ZBUFFER);
					gDPSetPrimColor(glistp++,0,0,255,255,255,renderMode.pixelOut);
					gDPSetRenderMode(glistp++, 
						(renderMode.useTextureMode != TEXTURE_MODE_NORMAL) ? G_RM_PASS : G_RM_FOG_SHADE_A, G_RM_AA_ZB_TEX_EDGE2);
				}
				else if((renderMode.useAAMode == AA_MODE_FULL) && (renderMode.useZMode))
				{
					gSPSetGeometryMode(glistp++, G_ZBUFFER);
					gDPSetRenderMode(glistp++, 
						(renderMode.useTextureMode != TEXTURE_MODE_NORMAL) ? G_RM_PASS : G_RM_FOG_SHADE_A, G_RM_AA_ZB_TEX_EDGE2);      
				}
				else if((renderMode.useAAMode == AA_MODE_NONE) && (renderMode.useZMode))
				{
					gSPSetGeometryMode(glistp++, G_ZBUFFER);
					gDPSetRenderMode(glistp++, 
						(renderMode.useTextureMode != TEXTURE_MODE_NORMAL) ? G_RM_PASS : G_RM_FOG_SHADE_A , G_RM_AA_ZB_TEX_EDGE2);      
				}
				else if((renderMode.useAAMode == AA_MODE_FULL) && (!renderMode.useZMode))
				{
					gSPClearGeometryMode(glistp++, G_ZBUFFER);
					gDPSetRenderMode(glistp++, 
						(renderMode.useTextureMode != TEXTURE_MODE_NORMAL) ? G_RM_PASS : G_RM_FOG_SHADE_A, G_RM_AA_TEX_EDGE2);      
				}
				else if((renderMode.useAAMode == AA_MODE_REDUCED) && (renderMode.useZMode))
				{
					gSPSetGeometryMode(glistp++, G_ZBUFFER);
					gDPSetRenderMode(glistp++, 
						(renderMode.useTextureMode != TEXTURE_MODE_NORMAL) ? G_RM_PASS : G_RM_FOG_SHADE_A, G_RM_AA_ZB_TEX_EDGE2);      
				}
				else if((renderMode.useAAMode == AA_MODE_REDUCED) && (!renderMode.useZMode))
				{
					gSPClearGeometryMode(glistp++, G_ZBUFFER);
					gDPSetRenderMode(glistp++, 
						(renderMode.useTextureMode != TEXTURE_MODE_NORMAL) ? G_RM_PASS : G_RM_FOG_SHADE_A, G_RM_AA_TEX_EDGE2);      
				}
				else
				{
					gSPClearGeometryMode(glistp++, G_ZBUFFER);
					gDPSetRenderMode(glistp++, 
						(renderMode.useTextureMode != TEXTURE_MODE_NORMAL) ? G_RM_PASS : G_RM_FOG_SHADE_A, G_RM_TEX_EDGE2);      
				}  
			}
			else
			{
				if(renderMode.faceColour)
				{
					gDPSetCombineMode(glistp++,G_CC_MODULATEPRIM, G_CC_MODULATEPRIM);
				}
				else
				{
					gDPSetCombineMode(glistp++, G_CC_MODULATERGBPRIMA, G_CC_PASS2);
				}

				if(renderMode.pixelOut)
				{
					gSPSetGeometryMode(glistp++, G_ZBUFFER);
					gDPSetPrimColor(glistp++,0,0,255,255,255,renderMode.pixelOut);
					gDPSetRenderMode(glistp++, 
						(renderMode.useTextureMode != TEXTURE_MODE_NORMAL) ? G_RM_PASS : G_RM_AA_ZB_PCL_SURF, G_RM_AA_ZB_PCL_SURF2);      
				}
				else if((renderMode.useAAMode == AA_MODE_FULL) && (renderMode.useZMode))
				{
					gSPSetGeometryMode(glistp++, G_ZBUFFER);
					gDPSetRenderMode(glistp++, 
						(renderMode.useTextureMode != TEXTURE_MODE_NORMAL) ? G_RM_PASS : G_RM_FOG_SHADE_A, G_RM_AA_ZB_OPA_SURF2);      
				}
				else if((renderMode.useAAMode == AA_MODE_NONE) && (renderMode.useZMode))
				{
					gSPSetGeometryMode(glistp++, G_ZBUFFER);
					gDPSetRenderMode(glistp++, 
						(renderMode.useTextureMode != TEXTURE_MODE_NORMAL) ? G_RM_PASS : G_RM_FOG_SHADE_A , G_RM_ZB_OPA_SURF2);      
				}
				else if((renderMode.useAAMode == AA_MODE_FULL) && (!renderMode.useZMode))
				{
					gSPClearGeometryMode(glistp++, G_ZBUFFER);
					gDPSetRenderMode(glistp++, 
						(renderMode.useTextureMode != TEXTURE_MODE_NORMAL) ? G_RM_PASS : G_RM_FOG_SHADE_A, G_RM_AA_OPA_SURF2);      
				}
				else if((renderMode.useAAMode == AA_MODE_REDUCED) && (renderMode.useZMode))
				{
					gSPSetGeometryMode(glistp++, G_ZBUFFER);
					gDPSetRenderMode(glistp++, 
						(renderMode.useTextureMode != TEXTURE_MODE_NORMAL) ? G_RM_PASS : G_RM_FOG_SHADE_A, G_RM_RA_ZB_OPA_SURF2);      
				}
				else if((renderMode.useAAMode == AA_MODE_REDUCED) && (!renderMode.useZMode))
				{
					gSPClearGeometryMode(glistp++, G_ZBUFFER);
					gDPSetRenderMode(glistp++, 
						(renderMode.useTextureMode != TEXTURE_MODE_NORMAL) ? G_RM_PASS : G_RM_FOG_SHADE_A, G_RM_RA_OPA_SURF2);      
				}
				else
				{
					gSPClearGeometryMode(glistp++, G_ZBUFFER);
					gDPSetRenderMode(glistp++, 
						(renderMode.useTextureMode != TEXTURE_MODE_NORMAL) ? G_RM_PASS : G_RM_FOG_SHADE_A, G_RM_OPA_SURF2);      
				}  

			}
		}
	}
	else
	{
		if(renderMode.faceColour)
		{
			gDPSetCombineMode(glistp++,G_CC_MODULATEPRIM, G_CC_MODULATEPRIM);
		}
		else if(renderMode.transparentSurf) //nofog+transparent
		{
			gDPSetCombineMode(glistp++,G_CC_MODULATERGB_MODULATEPRIMA,G_CC_MODULATERGB_MODULATEPRIMA);
		}
		else
		{
//			gDPSetCombineMode(glistp++, G_CC_MODULATERGBPRIMA, G_CC_MODULATERGBPRIMA);
			gDPSetCombineMode(glistp++, G_CC_MODULATERGBA, G_CC_MODULATERGBA);
		}
//		gDPSetPrimColor(glistp++,0,0,255,255,255,xluFact);

		//nofog & xlu
		if(renderMode.xluSurf)
		{
			if(!renderMode.faceColour)
			{
				gDPSetCombineMode(glistp++,G_CC_MODULATERGBA,G_CC_MODULATERGBA);
//				gDPSetCombineMode(glistp++,G_CC_MODULATERGB_MODULATEPRIMA,G_CC_MODULATERGB_MODULATEPRIMA);
			}
			else
			{
				gDPSetEnvColor(glistp++,255,255,255,xluFact);
				gDPSetCombineMode(glistp++,G_CC_MODULATEPRIM_ENVALPHA, G_CC_MODULATEPRIM_ENVALPHA);
			}
			if(renderMode.pixelOut)
			{
				gSPSetGeometryMode(glistp++, G_ZBUFFER);
				gDPSetPrimColor(glistp++,0,0,255,255,255,renderMode.pixelOut);
				gDPSetRenderMode(glistp++, 
					(renderMode.useTextureMode != TEXTURE_MODE_NORMAL) ? G_RM_PASS : G_RM_ZB_CLD_SURF, G_RM_ZB_CLD_SURF2);      
			}
			else if((renderMode.useAAMode == AA_MODE_FULL) && (renderMode.useZMode))
			{
				gSPSetGeometryMode(glistp++, G_ZBUFFER);
				gDPSetRenderMode(glistp++,
					(renderMode.useTextureMode != TEXTURE_MODE_NORMAL) ? G_RM_PASS : G_RM_AA_ZB_XLU_SURF, G_RM_AA_ZB_XLU_SURF2);      
			}
			else if((renderMode.useAAMode == AA_MODE_NONE) && (renderMode.useZMode))
			{
				gSPSetGeometryMode(glistp++, G_ZBUFFER);
				gDPSetRenderMode(glistp++, 
					(renderMode.useTextureMode != TEXTURE_MODE_NORMAL) ? G_RM_PASS : G_RM_ZB_XLU_SURF , G_RM_ZB_XLU_SURF2);      
			}
			else if((renderMode.useAAMode == AA_MODE_FULL) && (!renderMode.useZMode))
			{
				gSPClearGeometryMode(glistp++, G_ZBUFFER);
				gDPSetRenderMode(glistp++, 
					(renderMode.useTextureMode != TEXTURE_MODE_NORMAL) ? G_RM_PASS : G_RM_AA_XLU_SURF, G_RM_AA_XLU_SURF2);      
			}
			else if((renderMode.useAAMode == AA_MODE_REDUCED) && (renderMode.useZMode))
			{
				gSPSetGeometryMode(glistp++, G_ZBUFFER);
				gDPSetRenderMode(glistp++, 
					(renderMode.useTextureMode != TEXTURE_MODE_NORMAL) ? G_RM_PASS : G_RM_RA_ZB_OPA_SURF, G_RM_RA_ZB_OPA_SURF2);      
			}
			else if((renderMode.useAAMode == AA_MODE_REDUCED) && (!renderMode.useZMode))
			{
				gSPClearGeometryMode(glistp++, G_ZBUFFER);
				gDPSetRenderMode(glistp++, 
					(renderMode.useTextureMode != TEXTURE_MODE_NORMAL) ? G_RM_PASS : G_RM_RA_OPA_SURF, G_RM_RA_OPA_SURF2);      
			}
			else
			{
				gSPClearGeometryMode(glistp++, G_ZBUFFER);
				gDPSetRenderMode(glistp++, 
					(renderMode.useTextureMode != TEXTURE_MODE_NORMAL) ? G_RM_PASS : G_RM_XLU_SURF, G_RM_XLU_SURF2);      
			}  
		}
		else
		{
			if(renderMode.transparentSurf) //nofog+transparent
			{
				if(renderMode.pixelOut)
				{
					gSPSetGeometryMode(glistp++, G_ZBUFFER);
					gDPSetPrimColor(glistp++,0,0,255,255,255,renderMode.pixelOut);
					gDPSetRenderMode(glistp++, 
						(renderMode.useTextureMode != TEXTURE_MODE_NORMAL) ? G_RM_PASS : G_RM_AA_ZB_TEX_EDGE, G_RM_AA_ZB_TEX_EDGE2);
				}
				else if((renderMode.useAAMode == AA_MODE_FULL) && (renderMode.useZMode))
				{
					gSPSetGeometryMode(glistp++, G_ZBUFFER);
					gDPSetRenderMode(glistp++, 
						(renderMode.useTextureMode != TEXTURE_MODE_NORMAL) ? G_RM_PASS : G_RM_AA_ZB_TEX_EDGE, G_RM_AA_ZB_TEX_EDGE2);      
				}
				else if((renderMode.useAAMode == AA_MODE_NONE) && (renderMode.useZMode))
				{
					gSPSetGeometryMode(glistp++, G_ZBUFFER);
					gDPSetRenderMode(glistp++, 
						(renderMode.useTextureMode != TEXTURE_MODE_NORMAL) ? G_RM_PASS : G_RM_AA_ZB_TEX_EDGE , G_RM_AA_ZB_TEX_EDGE2);      
				}
				else if((renderMode.useAAMode == AA_MODE_FULL) && (!renderMode.useZMode))
				{
					gSPClearGeometryMode(glistp++, G_ZBUFFER);
					gDPSetRenderMode(glistp++, 
						(renderMode.useTextureMode != TEXTURE_MODE_NORMAL) ? G_RM_PASS : G_RM_AA_TEX_EDGE, G_RM_AA_TEX_EDGE2);      
				}
				else if((renderMode.useAAMode == AA_MODE_REDUCED) && (renderMode.useZMode))
				{
					gSPSetGeometryMode(glistp++, G_ZBUFFER);
					gDPSetRenderMode(glistp++, 
						(renderMode.useTextureMode != TEXTURE_MODE_NORMAL) ? G_RM_PASS : G_RM_AA_ZB_TEX_EDGE, G_RM_AA_ZB_TEX_EDGE2);      
				}
				else if((renderMode.useAAMode == AA_MODE_REDUCED) && (!renderMode.useZMode))
				{
					gSPClearGeometryMode(glistp++, G_ZBUFFER);
					gDPSetRenderMode(glistp++, 
						(renderMode.useTextureMode != TEXTURE_MODE_NORMAL) ? G_RM_PASS : G_RM_AA_TEX_EDGE, G_RM_AA_TEX_EDGE2);      
				}
				else
				{
					gSPClearGeometryMode(glistp++, G_ZBUFFER);
					gDPSetRenderMode(glistp++, 
						(renderMode.useTextureMode != TEXTURE_MODE_NORMAL) ? G_RM_PASS : G_RM_TEX_EDGE, G_RM_TEX_EDGE2);      
				}  
			}
			else	//nofog + solid
			{
				if(renderMode.pixelOut)
				{
					gSPSetGeometryMode(glistp++, G_ZBUFFER);
					gDPSetPrimColor(glistp++,0,0,255,255,255,renderMode.pixelOut);
					gDPSetRenderMode(glistp++, 
						(renderMode.useTextureMode != TEXTURE_MODE_NORMAL) ? G_RM_PASS : G_RM_AA_ZB_PCL_SURF, G_RM_AA_ZB_PCL_SURF2);      
				}
				else if((renderMode.useAAMode == AA_MODE_FULL) && (renderMode.useZMode))
				{
					gSPSetGeometryMode(glistp++, G_ZBUFFER);
					gDPSetRenderMode(glistp++, 
						(renderMode.useTextureMode != TEXTURE_MODE_NORMAL) ? G_RM_PASS : G_RM_AA_ZB_OPA_SURF, G_RM_AA_ZB_OPA_SURF2);      
				}
				else if((renderMode.useAAMode == AA_MODE_NONE) && (renderMode.useZMode))
				{
					gSPSetGeometryMode(glistp++, G_ZBUFFER);
					gDPSetRenderMode(glistp++, 
						(renderMode.useTextureMode != TEXTURE_MODE_NORMAL) ? G_RM_PASS : G_RM_ZB_OPA_SURF , G_RM_ZB_OPA_SURF2);      
				}
				else if((renderMode.useAAMode == AA_MODE_FULL) && (!renderMode.useZMode))
				{
					gSPClearGeometryMode(glistp++, G_ZBUFFER);
					gDPSetRenderMode(glistp++, 
						(renderMode.useTextureMode != TEXTURE_MODE_NORMAL) ? G_RM_PASS : G_RM_AA_OPA_SURF, G_RM_AA_OPA_SURF2);      
				}
				else if((renderMode.useAAMode == AA_MODE_REDUCED) && (renderMode.useZMode))
				{
					gSPSetGeometryMode(glistp++, G_ZBUFFER);
					gDPSetRenderMode(glistp++, 
						(renderMode.useTextureMode != TEXTURE_MODE_NORMAL) ? G_RM_PASS : G_RM_RA_ZB_OPA_SURF, G_RM_RA_ZB_OPA_SURF2);      
				}
				else if((renderMode.useAAMode == AA_MODE_REDUCED) && (!renderMode.useZMode))
				{
					gSPClearGeometryMode(glistp++, G_ZBUFFER);
					gDPSetRenderMode(glistp++, 
						(renderMode.useTextureMode != TEXTURE_MODE_NORMAL) ? G_RM_PASS : G_RM_RA_OPA_SURF, G_RM_RA_OPA_SURF2);      
				}
				else
				{
					gSPClearGeometryMode(glistp++, G_ZBUFFER);
					gDPSetRenderMode(glistp++, 
						(renderMode.useTextureMode != TEXTURE_MODE_NORMAL) ? G_RM_PASS : G_RM_OPA_SURF, G_RM_OPA_SURF2);      
				}  
			}
		}
	}
}


long screen_clipScale = 2;

/*	--------------------------------------------------------------------------------
	Function		: SetScissor
	Purpose			: 
	Parameters		: none
	Returns			: none
	Info			: 
*/
void SetScissor()
{
	gDPPipeSync(glistp++);

	switch (playMode)
	{
		case NORMAL_PMODE:
			dynamicp->vp[screenNum].vp.vscale[0] = SCREEN_WD*2;
			dynamicp->vp[screenNum].vp.vscale[1] = SCREEN_HT*2;
			dynamicp->vp[screenNum].vp.vscale[2] = G_MAXZ/2;
			dynamicp->vp[screenNum].vp.vscale[3] = 0;
			dynamicp->vp[screenNum].vp.vtrans[0] = SCREEN_WD*screen_clipScale;
			dynamicp->vp[screenNum].vp.vtrans[1] = SCREEN_HT*screen_clipScale;
			dynamicp->vp[screenNum].vp.vtrans[2] = G_MAXZ/2;
			dynamicp->vp[screenNum].vp.vtrans[3] = 0;
		break;
		case TWO_PMODE:
			dynamicp->vp[screenNum].vp.vscale[0] = SCREEN_WD*2;
			dynamicp->vp[screenNum].vp.vscale[1] = SCREEN_HT;
			dynamicp->vp[screenNum].vp.vscale[2] = G_MAXZ/2;
			dynamicp->vp[screenNum].vp.vscale[3] = 0;
			switch (screenNum)
			{
				case 0:
					dynamicp->vp[screenNum].vp.vtrans[0] = SCREEN_WD*2;
					dynamicp->vp[screenNum].vp.vtrans[1] = SCREEN_HT;
					dynamicp->vp[screenNum].vp.vtrans[2] = G_MAXZ/2;
					dynamicp->vp[screenNum].vp.vtrans[3] = 0;
					break;
				default:
					dynamicp->vp[screenNum].vp.vtrans[0] = SCREEN_WD*2;
					dynamicp->vp[screenNum].vp.vtrans[1] = SCREEN_HT*3;
					dynamicp->vp[screenNum].vp.vtrans[2] = G_MAXZ/2;
					dynamicp->vp[screenNum].vp.vtrans[3] = 0;
					break;
			}
		break;
		case FOUR_PMODE:
			dynamicp->vp[screenNum].vp.vscale[0] = SCREEN_WD-100;
			dynamicp->vp[screenNum].vp.vscale[1] = SCREEN_HT;
			dynamicp->vp[screenNum].vp.vscale[2] = G_MAXZ/2;
			dynamicp->vp[screenNum].vp.vscale[3] = 0;
			switch (screenNum)
			{
				case 0:
					dynamicp->vp[screenNum].vp.vtrans[0] = SCREEN_WD;
					dynamicp->vp[screenNum].vp.vtrans[1] = SCREEN_HT;
					dynamicp->vp[screenNum].vp.vtrans[2] = G_MAXZ/2;
					dynamicp->vp[screenNum].vp.vtrans[3] = 0;
					break;
				case 1:
					dynamicp->vp[screenNum].vp.vtrans[0] = SCREEN_WD*3;
					dynamicp->vp[screenNum].vp.vtrans[1] = SCREEN_HT;
					dynamicp->vp[screenNum].vp.vtrans[2] = G_MAXZ/2;
					dynamicp->vp[screenNum].vp.vtrans[3] = 0;
					break;
				case 2:
					dynamicp->vp[screenNum].vp.vtrans[0] = SCREEN_WD;
					dynamicp->vp[screenNum].vp.vtrans[1] = SCREEN_HT*3;
					dynamicp->vp[screenNum].vp.vtrans[2] = G_MAXZ/2;
					dynamicp->vp[screenNum].vp.vtrans[3] = 0;
					break;
				default:
					dynamicp->vp[screenNum].vp.vtrans[0] = SCREEN_WD*3;
					dynamicp->vp[screenNum].vp.vtrans[1] = SCREEN_HT*3;
					dynamicp->vp[screenNum].vp.vtrans[2] = G_MAXZ/2;
					dynamicp->vp[screenNum].vp.vtrans[3] = 0;
					break;
			}
		break;
	}
	
	switch (playMode)
	{
		case NORMAL_PMODE:
			gDPSetScissor(glistp++, G_SC_NON_INTERLACE, 0, 0, SCREEN_WD, SCREEN_HT);
			break;
		case TWO_PMODE:
			switch (screenNum)
			{
				case 0:
					gDPSetScissor(glistp++, G_SC_NON_INTERLACE, 50, 0, SCREEN_WD-50, (SCREEN_HT/2)-2);
					break;
				default:
					gDPSetScissor(glistp++, G_SC_NON_INTERLACE, 50, (SCREEN_HT/2)+2, SCREEN_WD-50, SCREEN_HT);
					break;
			}
			break;
		case FOUR_PMODE:
			switch (screenNum)
			{
				case 0:
					gDPSetScissor(glistp++, G_SC_NON_INTERLACE, 0, 0, SCREEN_WD/2, SCREEN_HT/2);
					break;
				case 1:
					gDPSetScissor(glistp++, G_SC_NON_INTERLACE, SCREEN_WD/2, 0, SCREEN_WD, SCREEN_HT/2);
					break;
				case 2:
					gDPSetScissor(glistp++, G_SC_NON_INTERLACE, 0, SCREEN_HT/2, SCREEN_WD/2, SCREEN_HT);
					break;
				default:
					gDPSetScissor(glistp++, G_SC_NON_INTERLACE,  SCREEN_WD/2, SCREEN_HT/2, SCREEN_WD, SCREEN_HT);
					break;
			}
		break;
	}
	gDPPipeSync(glistp++);
}

/*	--------------------------------------------------------------------------------
	Function		: InitRDP()
	Purpose			: inits. the RDP state for rendering
	Parameters		: none
	Returns			: none
	Info			:
*/
void InitRDP()
{
	gDPSetScissor(glistp++, G_SC_NON_INTERLACE, 0, 0, SCREEN_WD, SCREEN_HT);
	gDPPipelineMode(glistp++, G_PM_NPRIMITIVE);

	gDPSetTextureLOD(glistp++, G_TL_TILE);
	gDPSetTextureLUT(glistp++, G_TT_NONE);
	gDPSetTextureDetail(glistp++, G_TD_CLAMP);
	gDPSetTexturePersp(glistp++, G_TP_PERSP);
#ifdef NOFILTER
	gDPSetTextureFilter(glistp++, G_TF_POINT);
#else
	gDPSetTextureFilter(glistp++, G_TF_BILERP);
#endif
	gDPSetTextureConvert(glistp++, G_TC_FILT);
	gDPSetCombineKey(glistp++, G_CK_NONE);
    gDPSetAlphaCompare(glistp++, G_AC_NONE);
	gDPSetBlendColor(glistp++, 255, 255, 255, 0);
	gDPSetColorDither(glistp++, G_CD_DISABLE);
	gDPSetMaskImage(glistp++, zbuffer);
	gDPSetPrimColor(glistp++, 0, 0, 0, 0, 0, 255);
	gDPSetDepthSource(glistp++,G_ZS_PIXEL);

	gDPPipeSync(glistp++);
}

/*	--------------------------------------------------------------------------------
	Function		: InitRSP()
	Purpose			: inits. the RSP state for rendering
	Parameters		: none
	Returns			: none
	Info			:
*/
void InitRSP()
{
	gSPViewport(glistp++,&dynamicp->vp[screenNum]);

    gSPClearGeometryMode(glistp++,G_SHADE | G_SHADING_SMOOTH | G_CULL_BOTH | G_FOG |
									G_LIGHTING | G_TEXTURE_GEN |
									G_TEXTURE_GEN_LINEAR | G_LOD | G_ZBUFFER);

	gSPClipRatio(glistp++,FRUSTRATIO_4);

	gSPSetGeometryMode(glistp++,G_ZBUFFER | G_CULL_BACK);

	if(ShadingMode == LIGHTING)	gSPSetGeometryMode(glistp++,G_SHADE | G_LIGHTING);
	if(ShadingMode == GOURAUD)	gSPSetGeometryMode(glistp++,G_SHADE | G_SHADING_SMOOTH);
}

/*	--------------------------------------------------------------------------------
	Function		: ClearZBuffer()
	Purpose			: clears the z-buffer...
	Parameters		: none
	Returns			: none
	Info			:
*/
void ClearZBuffer()
{
	gDPSetColorImage(glistp++,G_IM_FMT_RGBA,G_IM_SIZ_16b,SCREEN_WD,OS_K0_TO_PHYSICAL(zbuffer));
	gDPPipeSync(glistp++);
	gDPSetCycleType(glistp++,G_CYC_FILL);
	gDPSetFillColor(glistp++,GPACK_ZDZ(G_MAXFBZ,0) << 16 | GPACK_ZDZ(G_MAXFBZ,0));
	gDPFillRectangle(glistp++,0,0,SCREEN_WD-1,SCREEN_HT-1);
	gDPPipeSync(glistp++);
}

/*	--------------------------------------------------------------------------------
	Function		: ClearFrameBuffer()
	Purpose			: clears the colour frame buffer....
	Parameters		: none
	Returns			: none
	Info			:
*/
void ClearFrameBuffer()
{
	// Clear colour framebuffer
	gDPSetCycleType(glistp++,G_CYC_FILL);

	gDPSetColorImage(glistp++,G_IM_FMT_RGBA,G_IM_SIZ_16b,SCREEN_WD,OS_K0_TO_PHYSICAL(cfb_ptrs[draw_buffer]));
  
	// Clear screen (framebuffer) to colour
	if(gfxIsDrawing)
	{
		gDPSetFillColor(glistp++,GPACK_RGBA5551(fog.r,fog.g,fog.b,1) << 16 | GPACK_RGBA5551(fog.r,fog.g,fog.b,1));
	}
	else
	{
		gDPSetFillColor(glistp++, GPACK_RGBA5551(0,0,0,1) << 16 | GPACK_RGBA5551(0,0,0,1));
	}

	gDPFillRectangle(glistp++,0,0,SCREEN_WD-1,SCREEN_HT-1);
	gDPPipeSync(glistp++);
	
	if(renderMode.useTextureMode == 2)
		gDPSetCycleType(glistp++, G_CYC_2CYCLE)
	else
		gDPSetCycleType(glistp++, G_CYC_1CYCLE);
}

/*	--------------------------------------------------------------------------------
	Function		: InitDisplayLists()
	Purpose			: initialises the display lists
	Parameters		: none
	Returns			: none
	Info			:
*/
void InitDisplayLists(void)
{
	// Initialize RDP / RSP state
	InitRDP();
	InitRSP();

	if(renderMode.useTextureMode)
	{
#ifdef NOFILTER
		gDPSetTextureFilter(glistp++, G_TF_POINT);
#else
		gDPSetTextureFilter(glistp++, G_TF_BILERP);
#endif

		if (renderMode.useTextureMode == 2)
		{
			gSPTexture(glistp++, 0xffff, 0xffff, 5, G_TX_RENDERTILE, G_ON);
			gDPSetTextureDetail(glistp++, G_TD_CLAMP);      
			gDPSetTextureLOD(glistp++, G_TL_LOD);
			gDPPipelineMode(glistp++, G_PM_1PRIMITIVE);
	  
			if (ShadingMode == PRIMCOLOR)
				gDPSetCombineMode(glistp++, G_CC_TRILERP, G_CC_DECALRGB2)
			else
				gDPSetCombineMode(glistp++, G_CC_TRILERP, G_CC_MODULATERGB2); 
		}
		else
		{
			gSPTexture(glistp++, 0xffff, 0xffff, 0, G_TX_RENDERTILE, G_ON);
	  
			if (ShadingMode == PRIMCOLOR)
				gDPSetCombineMode(glistp++, G_CC_DECALRGB, G_CC_DECALRGB)
			else
				gDPSetCombineMode(glistp++, G_CC_MODULATERGBA, G_CC_MODULATERGBA);
		}
	}
	else
    {
		gSPTexture(glistp++, 0, 0, 0, 0, G_OFF);

		if (ShadingMode == PRIMCOLOR)
			gDPSetCombineMode(glistp++, G_CC_PRIMITIVE, G_CC_PRIMITIVE)
		else 
			gDPSetCombineMode(glistp++, G_CC_SHADE, G_CC_SHADE);          
    }
}

/*	--------------------------------------------------------------------------------
	Function		: CleanupAndSendDisplayList
	Purpose			: cleans up and sends display list for processing
	Parameters		: int,int
	Returns			: none
	Info			:
*/
void CleanupAndSendDisplayList(int ucode,int flag)
{
	int listSize;

	gDPFullSync(glistp++);
	gSPEndDisplayList(glistp++);

	tlistp[currentTask]->list.t.data_size = (u32)((glistp - (Gfx *)tlistp[currentTask]->list.t.data_ptr) * sizeof(Gfx));
	/* Write back dirty cache lines that need to be read by the RCP */
//	osWritebackDCache(dynamicp, sizeof(dynamic[0]));

	actualGFXListLength[currentTask] = tlistp[currentTask]->list.t.data_size;

	if(tlistp[currentTask]->list.t.data_size >= GLIST_LEN * sizeof(Gfx))	
	{
		dprintf"ERROR: Graphics list overflow !!!\n"));
		return;
   		Crash("GFX LIST");
	}

	
	// Start up the RSP task
	CreateTaskStructure(currentTask, ucode);

	tlistp[currentTask]->flags = flag;
	tlistp[currentTask]->framebuffer = cfb_ptrs[draw_buffer];

	if(flag == NN_SC_SWAPBUFFER)
	{
		listSize = (u32)(glistp - (Gfx *)dynamicp->glist) * sizeof(Gfx);
		draw_buffer ^= 1;
	}

	//osSendMesg(sched_gfxMQ, (OSMesg *) tlistp[currentTask], OS_MESG_BLOCK);
	//osSendMesg(nextFrame_msgQ, (OSMesg *) tlistp[currentTask], OS_MESG_BLOCK);
	//gfxTasks++;
	currentTask++;	
}

/*	--------------------------------------------------------------------------------
	Function		: SendLastFrame()
	Purpose			:
	Parameters		:
	Returns			:
	Info			:
*/
void SendLastFrame()
{
	int i;

	if(currentTask == 1) 
		return;

	osWritebackDCache(dynamicp,sizeof(dynamic[0]));

	for(i=1; i<currentTask; i++)
	{
		osSendMesg(sched_gfxMQ,(OSMesg *)tlistp[i],OS_MESG_BLOCK);
		gfxTasks++;
	}
}

/*	--------------------------------------------------------------------------------
	Function		: DrawGraphics(void *arg)
	Purpose			: handles the threaded graphics drawing
	Parameters		: void *arg
	Returns			: void
	Info			:
*/
void XformActorList();
int myAA = 0;

void DrawGraphics(void *arg)
{
	SPRITE *sprite = NULL;
	int displayListLength = 0;
	short *msg_type = NULL;
	int ucodeType = 0;
	int i;

	while(1)
	{
		osRecvMesg(&gfx_msgQ,(OSMesg *)&msg_type,OS_MESG_BLOCK);

		switch(*msg_type)
		{
			case NN_SC_GFX_RETRACE_MSG:
				
				if(gfxTasks == 0)
				{
					if(codeDrawingRequest == TRUE)
						gfxIsDrawing = TRUE;

					if(codeDrawingRequest == FALSE)
						gfxIsDrawing = FALSE;
				}

				dispFrameCount++;

				SendLastFrame();

				dynamicp = &dynamic[draw_buffer];
				tlistp = &tlistPointers[draw_buffer][0];

				glistp = dynamicp->glist;
				currentTask = 1;
				tlistp[currentTask]->list.t.data_ptr = (u64 *)glistp;

				InitDisplayLists();

				ClearZBuffer();		
				ClearFrameBuffer();				
				
				SetScissor();
				objectMatrix = 0;

				if(gfxIsDrawing == FALSE)
					goto cleanup;

				SetVector(&actualCamSource[draw_buffer],&currCamSource[0]);
				SetVector(&actualCamTarget[draw_buffer],&currCamTarget[0]);

				SetupViewing();
//				PrintBackdrops();
//				CleanupAndSendDisplayList(UCODE_POLY,0);

//				if(onlyDrawBackdrops)
//					goto cleanup;

				InitDisplayLists();

				//***********************************
	
				// turn off AA for now on N64....AndyE
				renderMode.useAAMode = myAA;

				SetRenderMode();

				//***********************************

				AnimateSprites();

				TIMER_StartTimer(3,"XFORMACTLIST");
				XformActorList();
				TIMER_EndTimer(3);

				if(spriteList.numEntries)
				{
					sprite = PrintSpritesOpaque();
					gDPPipeSync(glistp++);
					InitDisplayLists();
					SetRenderMode();
					SetupViewing();
				}

				DrawActorList();

				if(spriteList.numEntries)
					PrintSpritesTranslucent(sprite);

				DrawSpecialFX();

				ClearViewing();

				// draw non-transformed objects / actors here (perhaps)....

				oldFogMode = fog.mode;
				fog.mode = 0;
				SetRenderMode();

				if( gameState.mode == INGAME_MODE && text3DList.numEntries )
					Print3DText( );					

				if(displayOverlays)
				{
					PrintSpriteOverlays();
					PrintTextOverlays();
				}

cleanup:
				TIMER_PrintTimers();

			    gDPPipeSync(glistp++);
				gDPSetCycleType(glistp++, G_CYC_1CYCLE);
				fog.mode = oldFogMode;

				CleanupAndSendDisplayList(UCODE_POLY,NN_SC_SWAPBUFFER);
				displayListLength = glistp - dynamicp->glist;
				break;

			case NN_SC_DONE_MSG:
				gfxTasks--;

				if( !gfxTasks )
				{
					if( gameState.mode && text3DList.numEntries )
						Calculate3DText( );					
				}
				break;
		}
	}
}

/* --------------------------------------------------------------------------------
	Programmer	: Matthew Cloy
	Function	: doPoly

	Purpose		:
	Parameters	: (void *arg)
	Returns		: void 
*/

void doPoly(void *arg)
{	
	short tmp;
	short *msg_type = NULL;
	unsigned long newCount;
	
	// init music system
	InitMusicDriver();

	// initialise font system and other gfx related stuff
	InitFont();
	fog.r = fog.g = fog.b = 0;

	// initialise controllers
	ActiveController = initControllers();

	// set up for eeprom handling
	InitEepromMessageQueue();
	osCreateThread(&ControllerThread, 6, ControllerProc, arg,
		 ControllerThreadStack+SMALLSTACKSIZE/sizeof(u64), controllerPriority);
  	osStartThread(&ControllerThread); 

	frameCount = 1;					   

	//InitEeprom();

    osCreateMesgQueue(&main_msgQ,main_msgbuf,8);
	nnScAddClient(&sched,&main_client,&main_msgQ);

	// Handle graphics drawing
	osCreateThread(&drawGraphicsThread,4,DrawGraphics,arg,drawGraphicsThreadStack+STACKSIZE/sizeof(u64),graphicsPriority);
	osStartThread(&drawGraphicsThread);
/*
	InitEeprom();

	if ( validEeprom )
	{
		dprintf"Valid Eeprom"));
		LoadLevelScores();
		LoadGame();
	}
	else*/
	{
		dprintf"Not A Valid Eeprom"));
	}

	// start the game from the start options level
	FreeAllLists();

#ifndef USE_MENUS
	gameState.mode = INGAME_MODE;
	InitLevel(WORLDID_FRONTEND,LEVELID_FRONTEND1);
#else
	gameState.mode		= MENU_MODE;
	gameState.menuMode	= TITLE_MODE;
#endif

	while(1) 
	{
		osRecvMesg(&main_msgQ,(OSMesg *)&msg_type,OS_MESG_BLOCK);

		if(*msg_type == NN_SC_GFX_RETRACE_MSG)
		{
			gameSpeed = framesPerSec;

			codeRunning = 1;

			// Actually handle the game loop
			TIMER_StartTimer(0,"GAMELOOP");
			GameLoop();
			TIMER_EndTimer(0);

			DoubleBufferSkinVtx();
		}

		codeRunning = 0;
	}
}

/*	--------------------------------------------------------------------------------
	Function		: main_(void *arg)
	Purpose			: main function to init. program
	Parameters		:
	Returns			: none
	Info			:
*/

static void main_(void *arg)
{
	int i = 0;

	// set scheduler type (NTSC / PAL)
	switch(osTvType)
	{
		case 0:
			while(1);
			break;
		case 1:
			nnScCreateScheduler(&sched,OS_VI_NTSC_LAN1,1);
			break;
		case 2:
			nnScCreateScheduler(&sched,OS_VI_MPAL_LAN1,1);
			break;
	}

	// create gfx message queue
    osCreateMesgQueue(&gfx_msgQ,gfx_msgbuf,8);
	nnScAddClient(&sched,&gfx_client,&gfx_msgQ);
	sched_gfxMQ = nnScGetGfxMQ(&sched);
	gfx_msg.gen.type = NN_SC_DONE_MSG;

	// set relevant display features
    osViSetSpecialFeatures(OS_VI_DITHER_FILTER_ON + OS_VI_DIVOT_ON + OS_VI_GAMMA_OFF + OS_VI_GAMMA_DITHER_ON);
		
	
	dprintf"\n"));
	dprintf"\nINITIALISING MEMORY - Getting max. memory : %lu bytes\n",0x80400000 - (u32)_staticSegmentStart));
	// get maximum memory
	JallocInit((ULONG)_staticSegmentStart,0x80400000 - (u32)_staticSegmentStart);
	ShowMemorySituation(0);

	// set the task lists
	tlistp = &tlistPointers[0][0];
	for(i = 0;i < MAXTASKS;i++) 
	{
		tlistp[i] = &tlist[0][i];
		tlistp[i]->list.t.type = M_GFXTASK;
		tlistp[i]->list.t.flags = 0;
		tlistp[i]->list.t.ucode_size = SP_UCODE_SIZE;
		tlistp[i]->list.t.ucode_data_size = SP_UCODE_DATA_SIZE;
		tlistp[i]->list.t.dram_stack	= (u64 *) (((int) &(dram_stack[0]) +0xf) & 0xfffffff0);
		tlistp[i]->list.t.dram_stack_size = SP_DRAM_STACK_SIZE8;
		tlistp[i]->list.t.output_buff = (u64 *) &(rdp_output[0]);
		tlistp[i]->list.t.output_buff_size = (u64 *)&rdp_output[RDP_BUFFER_LEN];
		tlistp[i]->list.t.yield_data_ptr = (u64 *) gfxYieldBuf;
		tlistp[i]->list.t.yield_data_size = OS_YIELD_DATA_SIZE;

		tlistp[i]->next  = 0;
		tlistp[i]->flags = 0;
		tlistp[i]->msgQ  = &gfx_msgQ;
		tlistp[i]->msg = (OSMesg)&gfx_msg;
	}
	tlistp = &tlistPointers[1][0];
	for(i = 0;i < MAXTASKS;i++) 
	{
		tlistp[i] = &tlist[1][i];
		tlistp[i]->list.t.type = M_GFXTASK;
		tlistp[i]->list.t.flags = 0;
		tlistp[i]->list.t.ucode_size = SP_UCODE_SIZE;
		tlistp[i]->list.t.ucode_data_size = SP_UCODE_DATA_SIZE;
		tlistp[i]->list.t.dram_stack	= (u64 *) (((int) &(dram_stack[0]) +0xf) & 0xfffffff0);
		tlistp[i]->list.t.dram_stack_size = SP_DRAM_STACK_SIZE8;
		tlistp[i]->list.t.output_buff = (u64 *) &(rdp_output[0]);
		tlistp[i]->list.t.output_buff_size = (u64 *)&rdp_output[RDP_BUFFER_LEN];
		tlistp[i]->list.t.yield_data_ptr = (u64 *) gfxYieldBuf;
		tlistp[i]->list.t.yield_data_size = OS_YIELD_DATA_SIZE;

		tlistp[i]->next  = 0;
		tlistp[i]->flags = 0;
		tlistp[i]->msgQ  = &gfx_msgQ;
		tlistp[i]->msg = (OSMesg)&gfx_msg;
	}
	
	dynamicp = &dynamic[draw_buffer];
	tlistp = &tlistPointers[draw_buffer][0];
	cfb_ptrs[0] = cfb_16_a; 
	cfb_ptrs[1] = cfb_16_b; 
	
	// Create the various message queues for dma,RSP,RDP, etc....
	CreateMessageQueues();

	// Initialise core program elements
	ComputeClockSpeed();
	InitCRCTable();

	// initialise the matrix stacks and other math related stuff
	InitMatrixStack();
	InitRMatrixStack();
	InitAtan();
	guMtxIdent(&Identity);

	// set initial camera up vector
	SetVector(&camVect,&cameraUpVect);

	// initialise timer system
	TIMER_InitTimers();

	StopDrawing("main");

	doPoly(arg);
}

/*	--------------------------------------------------------------------------------
	Function		: ClearViewing()
	Purpose			: clears viewing to allow rendering of non-transforming objects
	Parameters		: none
	Returns			: none
	Info			: used to draw 3D fonts, etc....
*/

void ClearViewing()
{
	u16 perspNorm;

    gDPPipeSync(glistp++);
	guPerspective(&dynamicp->noProjection,&perspNorm,yFOV,xFOV,nearPlaneDist,farPlaneDist,precScaleFactor);

	guLookAt(&dynamicp->noViewing,
		0,0,0,
		0,0,1,
		0,1,0);
	
	gSPMatrix(glistp++,OS_K0_TO_PHYSICAL(&(dynamicp->noProjection)),G_MTX_PROJECTION|G_MTX_LOAD|G_MTX_NOPUSH);
	gSPMatrix(glistp++,OS_K0_TO_PHYSICAL(&(dynamicp->noViewing)),G_MTX_PROJECTION|G_MTX_MUL|G_MTX_NOPUSH);  
	gSPPerspNormalize(glistp++,perspNorm);
}

/*	--------------------------------------------------------------------------------
	Function 	: ComputeClockSpeed()
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void ComputeClockSpeed()
{
	u32 count0,count1;

	osRecvMesg(&gfx_msgQ, 0, OS_MESG_BLOCK);
	osRecvMesg(&gfx_msgQ, 0, OS_MESG_BLOCK);
	osRecvMesg(&gfx_msgQ, 0, OS_MESG_BLOCK);
	osRecvMesg(&gfx_msgQ, 0, OS_MESG_BLOCK);
	osRecvMesg(&gfx_msgQ, 0, OS_MESG_BLOCK);
	osRecvMesg(&gfx_msgQ, 0, OS_MESG_BLOCK);
	osRecvMesg(&gfx_msgQ, 0, OS_MESG_BLOCK);
	osRecvMesg(&gfx_msgQ, 0, OS_MESG_BLOCK);
	osRecvMesg(&gfx_msgQ, 0, OS_MESG_BLOCK);
	osRecvMesg(&gfx_msgQ, 0, OS_MESG_BLOCK);
	osRecvMesg(&gfx_msgQ, 0, OS_MESG_BLOCK);
	osRecvMesg(&gfx_msgQ, 0, OS_MESG_BLOCK);
	count0 = osGetCount();
	osRecvMesg(&gfx_msgQ, 0, OS_MESG_BLOCK);
	count1 = osGetCount();

	TicksPerFrame = count1 - count0;
	//  dprintf"%u\n",TicksPerFrame));
	TicksPerSec = ((osTvType == 0) ? 50.0 : 60.0) * (count1 - count0);
}


/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void ControllerProc(void *arg)
{
	OSMesgQueue lmsgQ;
	OSMesg    lmsgbuf[8];
	NNScClient client;
	static short	disableController = FALSE;
	short *msg_type = NULL;
	int j;

	osCreateMesgQueue(&lmsgQ, lmsgbuf, 8);

  	nnScAddClient(&sched, &client, &lmsgQ);

	while(1)
	{
		if(disableController == FALSE)
		{
			(void)osRecvMesg(&lmsgQ,(OSMesg *)&msg_type, OS_MESG_BLOCK);

			if(*msg_type == NN_SC_RETRACE_MSG)
			{
				if(anyRumblePresent)
					UpdateRumble();
				if(eepromMessageQueue[0] != EEPROM_IDLE)
				{
					BOOL oldDC=disableController;

					disableController = TRUE;
						
					switch(eepromMessageQueue[0])
					{
/*						case EEPROM_SAVESLOT:
							EepromSaveSlots();
							break;
						case EEPROM_LOADSLOT:
							EepromLoadSlots();
							break;
						case EEPROM_SAVESCORES:
							EepromSaveHiScores();
							break;
						case EEPROM_LOADSCORES:
							EepromLoadHiScores();
							break;
						case EEPROM_VALID:
							EepromValid();
							break;
						case EEPROM_SAVEID:
							EepromSaveID();
							break;
						case EEPROM_SAVECRC:
							EepromSaveCRC();
							break;
						case EEPROM_LOADCRC:
							EepromLoadCRC();
							break;
*/					}

					GetEepromMessage();

					disableController = oldDC;
				}
			}
			else if(*msg_type == NN_SC_GFX_RETRACE_MSG)
			{			
				frameStart = osGetCount();
				TIMER_ClearTimers();
				TIMER_StartTimer(2,"CONTROL");
				osContStartReadData(&controllerMsgQ);
				(void)osRecvMesg(&controllerMsgQ, NULL, OS_MESG_BLOCK);
//				if((msg_type) && (*msg_type == NN_SC_GFX_RETRACE_MSG))
				{
					osContGetReadData(controllerdata);

//					if(autoPlayback == PLAY)
//					{
//						ReadControllerFromData(ActiveController);
//					}
//					else
					{
						for(j=0; j<NUM_FROGS; j++)
						{
							ActiveController = j;
							ReadController(j);
						}
					}

					if(NUM_FROGS == 1)
						ReadController(1);
					if(NUM_FROGS < 4)
						ReadController(3);

					TIMER_EndTimer(2);
				}
			}
		}
	}
}



// -----------------------------------------------------------------------------------------------
// MEMORY DEBUGGING / CHECKING ROUTINES - ANDYE - CURRENTLY SPECIFIC TO N64
// -----------------------------------------------------------------------------------------------

float ConvertKb(unsigned long op1)
{
	return ((float)op1 / 1024.0F);
}

int GetNumStaticBlocksUsed()
{
	int i,numBlocks = 0;

	if(jallocControl.lastUsedStatic == -1)
		return numBlocks;

	for(i=0; i<jallocControl.lastUsedStatic; i++)
		if(jallocControl.blocks[i].inuse)
			numBlocks++;

	return numBlocks;
}

int GetNumDynamicBlocksUsed()
{
	int i,numBlocks = 0;

	for(i=MAXJALLOCS-1; i>=jallocControl.lastUsedDynamic; i--)
		if(jallocControl.blocks[i].inuse)
			numBlocks++;

	return numBlocks;
}

unsigned long GetBaseAddrMemoryArea()
{
	return jallocControl.base;
}

long GetSizeOfMemoryArea()
{
	return jallocControl.size;
}

unsigned long GetBytesUsedForStatic()
{
	return jallocControl.staticUsed;
}

unsigned long GetBytesUsedForDynamic()
{
	return jallocControl.dynamicUsed;
}

unsigned long GetBytesUsedForUniques()
{
	int i;
	unsigned long bytes = 0;

	for(i=0; i<MAXJALLOCS; i++)
	{
		if(jallocControl.blocks[i].inuse)
		{
			if(	gstrcmp("uniqDL",jallocControl.blocks[i].name) == 0 ||
				gstrcmp("unqVtx",jallocControl.blocks[i].name) == 0 ||
				gstrcmp("UniqObj",jallocControl.blocks[i].name) == 0 ||
				gstrcmp("UniqObjC",jallocControl.blocks[i].name) == 0 ||
				gstrcmp("UniqSpr",jallocControl.blocks[i].name) == 0)
			{
				bytes += jallocControl.blocks[i].size;
			}
		}
	}

	return bytes;
}

unsigned long GetBytesUsed(char *search)
{
	int i;
	unsigned long bytes = 0;

	for(i=0; i<MAXJALLOCS; i++)
	{
		if(jallocControl.blocks[i].inuse)
		{
			if(gstrcmp(search,jallocControl.blocks[i].name) == 0)
				bytes += jallocControl.blocks[i].size;
		}
	}

	return bytes;
}

void ShowMemorySituation(char info)
{
	unsigned long totalBytes = 0;
	unsigned long totalBytesFree = 0;
	unsigned long totalBytesUsed = 0;
	unsigned long bytes = 0;

	totalBytes		= GetSizeOfMemoryArea();
	totalBytesFree	= totalBytes - GetBytesUsedForStatic() - GetBytesUsedForDynamic();
	totalBytesUsed	= GetBytesUsedForStatic() + GetBytesUsedForDynamic();

	dprintf"\n"));
	dprintf"\nCURRENT MEMORY SITUATION"));
	dprintf"\n   Size of memory area : %lu bytes (free: %lu)",totalBytes,totalBytesFree));
	dprintf"\n   Blocks used : %lu / %lu",GetNumStaticBlocksUsed() + GetNumDynamicBlocksUsed(),MAXJALLOCS));
	dprintf"\n      Static : %lu bytes",GetBytesUsedForStatic()));
	dprintf"\n      Dynamic : %lu bytes",GetBytesUsedForDynamic()));
	dprintf"\n      Total : %lu bytes",totalBytesUsed));
	dprintf"\n"));

	if(info == MEM_SHOW_INGAMEINFO)
	{
		bytes = GetBytesUsed("TXTRBANK");
		bytes += GetBytesUsed("SYSTXTR");
		dprintf"\n   Texture banks : %lu / %lu bytes  (%.0f kb)",bytes,totalBytesUsed,ConvertKb(bytes)));
		bytes = GetBytesUsed("OBJBANK");
		dprintf"\n   Object banks : %lu / %lu bytes  (%.0f kb)",bytes,totalBytesUsed,ConvertKb(bytes)));
		bytes = GetBytesUsed("COLLBANK");
		dprintf"\n   Collision bank: %lu / %lu bytes  (%.0f kb)",bytes,totalBytesUsed,ConvertKb(bytes)));
		bytes = GetBytesUsed("SCENBANK");
		dprintf"\n   Scenics bank: %lu / %lu bytes  (%.0f kb)",bytes,totalBytesUsed,ConvertKb(bytes)));
		bytes = GetBytesUsed("ENTBANK");
		dprintf"\n   Entity bank: %lu / %lu bytes  (%.0f kb)",bytes,totalBytesUsed,ConvertKb(bytes)));
		bytes = GetBytesUsed("SCRBANK");
		dprintf"\n   Script bank: %lu / %lu bytes  (%.0f kb)",bytes,totalBytesUsed,ConvertKb(bytes)));
	}

	dprintf"\n\n"));
}
