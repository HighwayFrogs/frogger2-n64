/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.
	
----------------------------------------------------------------------------------------------- */

#ifndef NNSCHED_H_INCLUDED
#define NNSCHED_H_INCLUDED

#include <ultra64.h>

#define NN_SC_STACKSIZE    0x2000  
#define NN_SC_SWAPBUFFER   0x0040  

#define NN_SC_RETRACE_MSG  1  
#define NN_SC_DONE_MSG     2  
#define NN_SC_PRE_NMI_MSG  3  
#define NN_SC_GTASKEND_MSG 4  
#define NN_SC_MAX_MESGS    8  

#define NN_SC_GFX_RETRACE_MSG  9  
#define NN_SC_PAL_RETRACE_MSG  10
#define NN_SC_CODE_RETRACE_MSG  11

#define NN_SC_PRI          120  //120
#define NN_SC_AUDIO_PRI    110  //110
#define NN_SC_GRAPHICS_PRI 100  //100

#define VIDEO_MSG       666  
#define RSP_DONE_MSG    667  
#define RDP_DONE_MSG    668  
#define PRE_NMI_MSG     669  

#define NN_SC_PERF_NUM    4
#define NN_SC_GTASK_NUM   8  
#define NN_SC_AUTASK_NUM   4 


/*===========================================================================*/
typedef short NNScMsg;  

typedef struct SCClient_s {  
    struct SCClient_s   *next;  
    OSMesgQueue         *msgQ;  
} NNScClient;

typedef struct SCTask_s {  
    struct SCTask_s     *next;  
    u32                 state;
    u32			flags;
    void		*framebuffer;  
    OSTask              list;
    OSMesgQueue         *msgQ;
    OSMesg              msg;
} NNScTask;

typedef struct {  

  NNScMsg   retraceMsg;
  NNScMsg   prenmiMsg;

  OSMesgQueue audioRequestMQ;
  OSMesg      audioRequestBuf[NN_SC_MAX_MESGS];
  OSMesgQueue graphicsRequestMQ;
  OSMesg      graphicsRequestBuf[NN_SC_MAX_MESGS];

  OSMesgQueue retraceMQ;
  OSMesg      retraceMsgBuf[NN_SC_MAX_MESGS];
  OSMesgQueue rspMQ;
  OSMesg      rspMsgBuf[NN_SC_MAX_MESGS];
  OSMesgQueue rdpMQ;
  OSMesg      rdpMsgBuf[NN_SC_MAX_MESGS];

  OSMesgQueue waitMQ;
  OSMesg      waitMsgBuf[NN_SC_MAX_MESGS];

  OSThread    schedulerThread;	
  OSThread    audioThread;		
  OSThread    graphicsThread;	

  NNScClient  *clientList;

  NNScTask    *curGraphicsTask;
  NNScTask    *curAudioTask;
  NNScTask    *graphicsTaskSuspended;
  

  u32         firstTime;

} NNSched;


typedef struct {

  u32 gtask_cnt;                      
  u32 autask_cnt;                     
  u64 retrace_time;                   
  u64 gtask_stime[NN_SC_GTASK_NUM];   
  u64 rdp_etime[NN_SC_GTASK_NUM];     
  u64 rsp_etime[NN_SC_GTASK_NUM];     
  u64 autask_stime[NN_SC_AUTASK_NUM]; 
  u64 autask_etime[NN_SC_AUTASK_NUM]; 
  u32 endflag;
} NNScPerf;

  

#ifdef NN_SC_PERF
extern NNScPerf* nnsc_perf_ptr;

#endif /* NN_SC_PERF */

extern float accelerator;
extern float gameSpeed;
extern float GAME_SPEED2;
extern float framesPerSec;
extern void            nnScCreateScheduler(NNSched *sc, u8 videoMode, u8 
numFields);
extern void            nnScAddClient(NNSched *sc, NNScClient *, OSMesgQueue *mq);
extern void            nnScRemoveClient(NNSched *sc, NNScClient 
*client);
extern void            nnScEventHandler(NNSched *sc);
extern void            nnScEventBroadcast(NNSched *sc, NNScMsg *msg);
extern void            nnScExecuteAudio(NNSched *sc);
extern void            nnScExecuteGraphics(NNSched *sc);
extern void            nnScWaitTaskReady(NNSched *sc, NNScTask *task);
extern OSMesgQueue *   nnScGetGfxMQ(NNSched *sc);
extern OSMesgQueue *   nnScGetAudioMQ(NNSched *sc);
extern char	desiredFrameRate;
extern char	newDesiredFrameRate;
void SetRetraceSpeed(NNSched *sc,char number);
void TimerProc(void *arg);


#endif
