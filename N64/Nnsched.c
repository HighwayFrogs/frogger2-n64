/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: Nnsched.c
	Programmer	: 
	Date		: 29/04/99 11:58:05

----------------------------------------------------------------------------------------------- */


#include <ultra64.h>

#include "incs.h"


extern int gfxTasks;

u64 nnScStack[NN_SC_STACKSIZE/8];
u64 nnScAudioStack[NN_SC_STACKSIZE/8];
u64 nnScGraphicsStack[NN_SC_STACKSIZE/8];

OSThread TimerThread;
u64      TimerThreadStack[SMALLSTACKSIZE/sizeof(u64)];

OSViFieldRegs originalRegs;
char		originalVideoMode;

char preNmiCount = 0;

#ifdef NN_SC_PERF
static u32       nnsc_perf_index = 0;
static u32       nnsc_perf_flag = 0;
static NNScPerf  nnsc_perf[NN_SC_PERF_NUM];
static NNScPerf* nnsc_perf_inptr;     
NNScPerf* nnsc_perf_ptr;              
#endif 

float framesPerSec = 2;
float gameSpeed = 1;
float GAME_SPEED2 = 1;
float accelerator = 1.0;



void SetRetraceSpeed(NNSched *sc,char number)
{
	osViSetEvent(&sc->retraceMQ, (OSMesg)VIDEO_MSG, number);    
}

/************************************************************
  nnScCreateScheduler()
************************************************************/
void nnScCreateScheduler(NNSched *sc, u8 videoMode, u8 numFields)
{
	OSViFieldRegs regs;
	short vstart = 60,vend = 600;
	float yscale = 0.88;

  sc->curGraphicsTask = 0;
  sc->curAudioTask    = 0;
  sc->graphicsTaskSuspended = 0;
  sc->clientList      = 0;
  sc->firstTime       = 1; 
  sc->retraceMsg      = NN_SC_RETRACE_MSG;
  sc->prenmiMsg       = NN_SC_PRE_NMI_MSG;

  osCreateMesgQueue(&sc->retraceMQ, sc->retraceMsgBuf, NN_SC_MAX_MESGS);
  osCreateMesgQueue(&sc->rspMQ, sc->rspMsgBuf, NN_SC_MAX_MESGS);
  osCreateMesgQueue(&sc->rdpMQ, sc->rdpMsgBuf, NN_SC_MAX_MESGS);
  osCreateMesgQueue(&sc->graphicsRequestMQ, sc->graphicsRequestBuf, 
NN_SC_MAX_MESGS);
  osCreateMesgQueue(&sc->audioRequestMQ, sc->audioRequestBuf, 
NN_SC_MAX_MESGS);
  osCreateMesgQueue(&sc->waitMQ, sc->waitMsgBuf, NN_SC_MAX_MESGS);

  osCreateViManager(OS_PRIORITY_VIMGR);    
  osViSetMode(&osViModeTable[videoMode]);
  osViBlack(TRUE);

	if(osTvType == 0)
	{
		memcpy((char *)&regs,(char *)&osViModeTable[videoMode].fldRegs[0],sizeof(OSViFieldRegs));
		memcpy((char *)&originalRegs,(char *)&osViModeTable[videoMode].fldRegs[0],sizeof(OSViFieldRegs));

		regs.vStart = (vstart<<16) + (vend);
		memcpy((char *)&osViModeTable[videoMode].fldRegs[0],(char *)&regs,sizeof(OSViFieldRegs));
		osViSetYScale(yscale);
		originalVideoMode = videoMode;
//		osViModeTable[videoMode].fldRegs[0].vStart
	}

  osViSetEvent(&sc->retraceMQ, (OSMesg)VIDEO_MSG, numFields);    
  osSetEventMesg(OS_EVENT_SP, &sc->rspMQ, (OSMesg)RSP_DONE_MSG);
  osSetEventMesg(OS_EVENT_DP, &sc->rdpMQ, (OSMesg)RDP_DONE_MSG);
  osSetEventMesg(OS_EVENT_PRENMI, &sc->retraceMQ, (OSMesg)PRE_NMI_MSG);   


  osCreateThread(&sc->schedulerThread, 19, (void(*)(void*))nnScEventHandler,
		 (void *)sc, nnScStack+NN_SC_STACKSIZE/sizeof(u64),
		 NN_SC_PRI);
  osStartThread(&sc->schedulerThread);

/*
	osCreateThread(&TimerThread, 5, TimerProc, &sched,
		 TimerThreadStack+SMALLSTACKSIZE/sizeof(u64), timerPriority);
  	osStartThread(&TimerThread);
*/

  osCreateThread(&sc->audioThread, 18, (void(*)(void *))nnScExecuteAudio,
		 (void *)sc, nnScAudioStack+NN_SC_STACKSIZE/sizeof(u64),
		 NN_SC_AUDIO_PRI);
  osStartThread(&sc->audioThread);

  osCreateThread(&sc->graphicsThread, 17,(void(*)(void*))nnScExecuteGraphics,
		 (void *)sc, nnScGraphicsStack+NN_SC_STACKSIZE/sizeof(u64),
		 NN_SC_GRAPHICS_PRI);
  osStartThread(&sc->graphicsThread);

}

/************************************************************
  nnScGetAudioMQ()
************************************************************/
OSMesgQueue *nnScGetAudioMQ(NNSched *sc)
{
  return( &sc->audioRequestMQ );
}

/************************************************************
  nnScGetGfxMQ()
************************************************************/
OSMesgQueue *nnScGetGfxMQ(NNSched *sc)
{
  return( &sc->graphicsRequestMQ );
}

/************************************************************
  nnScEventHandler()
************************************************************/
char	desiredFrameRate = 2;
char	newDesiredFrameRate = 2;
NNScMsg   gfxRetraceMsg = NN_SC_GFX_RETRACE_MSG;
NNScMsg   codeRetraceMsg = NN_SC_CODE_RETRACE_MSG;

void nnScEventHandler(NNSched *sc)
{
  OSMesg msg = (OSMesg)0;
	static short gcount = 0;
	static short acount = 0;
	static short fcount = 0;

  while(1) 
  {
    osRecvMesg(&sc->retraceMQ, &msg, OS_MESG_BLOCK);


//	desiredFrameRate = 3;
    switch ( (int)msg ) 
	{
		case VIDEO_MSG:
			nnScEventBroadcast( sc, &sc->retraceMsg );
			
			gcount++;
			acount++;
			fcount++;
			actFrameCount++;
			if((gcount >= desiredFrameRate) && (gfxTasks == 0) && (codeRunning == 0))
			{
				desiredFrameRate = newDesiredFrameRate;
				gcount = 0;
				nnScEventBroadcast( sc, &gfxRetraceMsg);
				framesPerSec = fcount;
				fcount = 0;
			}
			else
			{
//				if((gfxTasks) && (gcount >= desiredFrameRate))
				{
					gcount = desiredFrameRate - 1;
				}
			}

/*
			if(acount >= 3)
			{
				acount = 0;
				nnScEventBroadcast( sc, &sc->retraceMsg );
			}
*/
			if(preNmiCount)
			{
				preNmiCount--;
				if(preNmiCount < 8)
				{
//					fadeOverlay.xluOverride=255;
//					gameInfo.fadeVal = 300;
				}
				if(preNmiCount == 0)
				{
				    osStopThread(&mainThread);
				    osStopThread(&drawGraphicsThread);
				    osStopThread(&ControllerThread);
				    osStopThread(&sched.audioThread);
					
					if(osTvType == 0)
					{
						memcpy((char *)&osViModeTable[originalVideoMode].fldRegs[0],(char *)&originalRegs,sizeof(OSViFieldRegs));
						osViSetYScale(1);
					}
										
				}
			}


	      break;
    case PRE_NMI_MSG:	
      nnScEventBroadcast( sc, &sc->prenmiMsg );
//		if(osTvType == 0)
		{
#ifdef AUDIO
			MusHandleStop(audioCtrl.musicHandle[0], 20);
			MusHandleStop(audioCtrl.musicHandle[1], 20);
#endif
			preNmiCount = 20;
		}
//		FadeScreenOut();
      break;
    }
  }
}

/************************************************************
  nnScAddClient()
************************************************************/
void nnScAddClient(NNSched *sc, NNScClient *c, OSMesgQueue *msgQ)
{
  OSIntMask mask;

  mask = osSetIntMask(OS_IM_NONE);
  c->msgQ = msgQ;
  c->next = sc->clientList;
  sc->clientList = c;
  osSetIntMask(mask);
}

/************************************************************
  nnScRemoveClient()
************************************************************/
void nnScRemoveClient(NNSched *sc, NNScClient *c)
{
  NNScClient *client = sc->clientList; 
  NNScClient *prev   = 0;
  OSIntMask  mask;

  mask = osSetIntMask(OS_IM_NONE);
    
  while (client != 0) {
    if (client == c) {
      if(prev)
	prev->next = c->next;
      else
	sc->clientList = c->next;
      break;
    }
    prev   = client;
    client = client->next;
  }
  osSetIntMask(mask);
}

/************************************************************
 nnScEventBroadcast()
************************************************************/
void nnScEventBroadcast(NNSched *sc, NNScMsg *msg)
{
  NNScClient *client;

  for (client = sc->clientList; client != 0; client = client->next) 
  {
    osSendMesg(client->msgQ, (OSMesg *)msg, OS_MESG_NOBLOCK);
  }
}

/************************************************************
  nnScExecuteAudio()
************************************************************/
void nnScExecuteAudio(NNSched *sc)
{
  OSMesg msg = (OSMesg)0;
  NNScTask *task = (NNScTask *)0;
  NNScTask *gfxTask = (NNScTask *)0;
  u32 yieldFlag = 0;

#ifdef NN_SC_PERF
  u32 task_cnt;

#endif /* NN_SC_PERF */

  while(1) 
  {

    osRecvMesg(&sc->audioRequestMQ, (OSMesg *)&task, OS_MESG_BLOCK);


    osWritebackDCacheAll();


    yieldFlag = 0;
    gfxTask = sc->curGraphicsTask;
    if( gfxTask ) 
	{

      osSpTaskYield();		
      osRecvMesg(&sc->rspMQ, &msg, OS_MESG_BLOCK);

      if (osSpTaskYielded(&gfxTask->list))
	  {
		yieldFlag =1;
      }
	  else 
	  {
		yieldFlag =2;
      }
    }

#ifdef NN_SC_PERF
    if(nnsc_perf_inptr->autask_cnt < NN_SC_AUTASK_NUM)
	{
      task_cnt = nnsc_perf_inptr->autask_cnt;
      nnsc_perf_inptr->autask_stime[task_cnt] =
	OS_CYCLES_TO_USEC(osGetTime()) - nnsc_perf_inptr->retrace_time;
    }
#endif /* NN_SC_PERF */


//    TIMER_StartTimer(7,"MUSIC");
    sc->curAudioTask = task;
    osSpTaskStart(&task->list);

    osRecvMesg(&sc->rspMQ, &msg, OS_MESG_BLOCK);
    sc->curAudioTask = (NNScTask *)0;
//	TIMER_EndTimer(7);

#ifdef NN_SC_PERF
    if(nnsc_perf_inptr->autask_cnt < NN_SC_AUTASK_NUM){
      nnsc_perf_inptr->autask_etime[task_cnt] =
	OS_CYCLES_TO_USEC(osGetTime()) - nnsc_perf_inptr->retrace_time;
      nnsc_perf_inptr->autask_cnt++;
    }
#endif /* NN_SC_PERF */

    if( sc->graphicsTaskSuspended )
      osSendMesg( &sc->waitMQ, &msg, OS_MESG_BLOCK );

    if( yieldFlag == 1 ) {
      osSpTaskStart(&gfxTask->list);    
    } else if ( yieldFlag == 2 ) {
      osSendMesg(&sc->rspMQ, &msg, OS_MESG_BLOCK);
    }

    osSendMesg(task->msgQ, task->msg, OS_MESG_BLOCK);
  }

}

/************************************************************
 * nnScExecuteGraphics()
************************************************************/
void nnScExecuteGraphics(NNSched *sc)
{
  OSMesg msg = (OSMesg)0;
  NNScTask *task;

  while(1) 
  {

    osRecvMesg(&sc->graphicsRequestMQ, (OSMesg *)&task, OS_MESG_BLOCK);

	if(task->framebuffer)
		nnScWaitTaskReady(sc, task);

    if( sc->curAudioTask ) 
	{
      sc->graphicsTaskSuspended = task;
      osRecvMesg( &sc->waitMQ, &msg, OS_MESG_BLOCK );
      sc->graphicsTaskSuspended = (NNScTask *)0;
    }

    sc->curGraphicsTask = task;
//	osInvalDCache(&rdp_output_len, 8);
//	dprintf"%x\n", rdp_output_len));
//	osInvalDCache(&rdp_output_len, sizeof(rdp_output_len));

	osWritebackDCacheAll();

    TIMER_StartTimer(8,"RSP");
    TIMER_StartTimer(9,"RDP");
    osSpTaskStart(&task->list);

    osRecvMesg(&sc->rspMQ, &msg, OS_MESG_BLOCK);
    sc->curGraphicsTask = (NNScTask *)0;
	TIMER_EndTimer(8);

	osRecvMesg(&sc->rdpMQ, &msg, OS_MESG_BLOCK);
	TIMER_EndTimer(9);

//	dprintf"%x\n", rdp_output_len));

    if (sc->firstTime) 
	{
      osViBlack(FALSE);
      sc->firstTime = 0;
    }

    if ( task->flags & NN_SC_SWAPBUFFER )
	{
		if(task->framebuffer)
			osViSwapBuffer(task->framebuffer);
    }
    osSendMesg(task->msgQ, task->msg, OS_MESG_BLOCK);
  }
}

/************************************************************
  nnScWaitTaskReady()
************************************************************/
void nnScWaitTaskReady(NNSched *sc, NNScTask *task)
{
  OSMesg msg = (OSMesg)0;
  NNScClient client;
  void *fb = task->framebuffer;

  while(osViGetCurrentFramebuffer() == fb || osViGetNextFramebuffer() == fb ) 
  {
    nnScAddClient( sc, &client, &sc->waitMQ );  
    osRecvMesg( &sc->waitMQ, &msg, OS_MESG_BLOCK );
    nnScRemoveClient( sc, &client );
  }
}





//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void TimerProc(void *arg)
//void TimerProc(NNSched *sc)
{
	OSMesgQueue lmsgQ;
	OSMesg    lmsgbuf[8];
	NNScClient client;
	OSTimer	timer;
	short *msg_type = NULL;
	static short gcount = 0;
	static short acount = 0;
	NNSched *sc = (NNSched *)arg;
 
 
 
 	osCreateMesgQueue(&lmsgQ, lmsgbuf, 8);

  	nnScAddClient(&sched, &client, &lmsgQ);
	
	osSetTimer(&timer, 0, OS_USEC_TO_CYCLES(19667), &lmsgQ, (OSMesg)NN_SC_PAL_RETRACE_MSG);
//	osSetTimer(&timer, 0, OS_USEC_TO_CYCLES(16667), &lmsgQ, (OSMesg)NN_SC_PAL_RETRACE_MSG);
	
	while(1)
	{
	
		(void)osRecvMesg(&lmsgQ,(OSMesg *)&msg_type, OS_MESG_BLOCK);

		if((int)msg_type == NN_SC_PAL_RETRACE_MSG)
		{
			gcount++;
			acount++;
			actFrameCount++;
			if((gcount >= desiredFrameRate) && (gfxTasks == 0))
			{

				desiredFrameRate = newDesiredFrameRate;
				gcount = 0;
//				nnScEventBroadcast( sc, &codeRetraceMsg);
				nnScEventBroadcast( sc, &gfxRetraceMsg);
	//				nnScEventBroadcast( sc, &sc->retraceMsg );
			}
			else
			{
				if((gfxTasks) && (gcount >= desiredFrameRate))
				{
					gcount = desiredFrameRate - 1;
				}
			}

 
			if(acount >= 3)
			{
				acount = 0;
				nnScEventBroadcast( sc, &sc->retraceMsg );
			}
 		}
	}
}